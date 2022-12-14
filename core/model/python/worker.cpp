#include "worker.h"

PythonWorker::PythonWorker(Basis *_basis, HProcessor *_hp, QObject *parent)
  : QObject(parent), basis(_basis), hp(_hp)
{
  Py_InitializeEx(1);
  // Adds current path to sys.path for importing scripts from this directory.
  _current_path = QDir::toNativeSeparators(QDir::currentPath());
  QString command = "import sys; sys.path.append('" + _current_path + "')";
  PyRun_SimpleString(command.toStdString().c_str());
}

PythonWorker::~PythonWorker() {
  Py_Finalize();
}

QJsonObject PythonWorker::run(QString path, QString def_name, QJsonObject transport) {
  QFileInfo module_info(path);
  QString dir_path = QDir::toNativeSeparators(module_info.canonicalPath());
  if (dir_path != _current_path) {
    QString command = "import sys; sys.path.append('" + dir_path + "')";
    PyRun_SimpleString(command.toStdString().c_str());
  }
  QString module_nick = module_info.fileName().split('.')[0];
  Object module_name = PyUnicode_FromString(module_nick.toStdString().c_str());
  Object mod = PyImport_Import(module_name);
  if (not mod) {
    emit script_exception(tr("Failed to connect the module."));
    return {{basis->errorTypeWk, 1}};
  }
  Object answer_func = PyObject_GetAttrString(mod, def_name.toStdString().c_str());
  if (not answer_func) {
    emit script_exception(tr("Could not find \"answer\" attribute in module."));
    return {{basis->errorTypeWk, 2}};
  }
  if (not (answer_func and PyCallable_Check(answer_func))) {
    emit script_exception(tr("Cannot call \"answer\": this is not a function!"));
    return {{basis->errorTypeWk, 3}};
  }
  QJsonDocument send_doc(transport);
  Object arg = PyUnicode_FromString(send_doc.toJson(QJsonDocument::Compact).constData());
  if (not arg) {
    emit script_exception(tr("Failed to construct argument from string."));
    return {{basis->errorTypeWk, 4}};
  }
  /*! If we pack in tuple our beautiful Object instead of PyObject, a segmentation fault will  */
  Object args = PyTuple_Pack(1, arg.get());      /*!< come out. Instead we send pure PyObject. */
  if (not args) {
    emit script_exception(tr("Failed to construct a tuple from a string."));
    return {{basis->errorTypeWk, 5}};
  }
  Object answer_result = PyObject_CallObject(answer_func, args);
  if (not answer_result) {
    emit script_exception(tr(
      "The function could not execute correctly because it did not return a result."
    ));
    return {{basis->errorTypeWk, 6}};
  }
  Object objects_representation = PyObject_Repr(answer_result);
  auto result = QString::fromUtf8(PyUnicode_AsUTF8(objects_representation)).chopped(1).remove(0, 1);
  auto recv_doc = QJsonDocument::fromJson(result.toUtf8());
  if (recv_doc.isNull()) {
    emit script_exception(tr("No valid JSON received."));
    return {{basis->errorTypeWk, 7}};
  }
  if (not recv_doc.isObject()) {
    emit script_exception(tr("No JSON object received."));
    return {{basis->errorTypeWk, 8}};
  }
  return recv_doc.object();
}

/*!
 * @brief Processes the script configuration and gets the necessary information from it.
 * @details Handles next things:
 *   1. `number_of_hist_messages` prop
 *   2. `memory_cells` prop
 *   3. `properties` from @a expression
 *   4. `path` and `fn_name` props
 *   5. `user_expression` prop
 *   6. <!-- script runs -->
 *   7. `store_in_memory` prop from script
 */
QJsonObject PythonWorker::request_answer(
  ReactScript *script, const Expression &expression, const QString &user_expression
) {
  QJsonObject transport;
  if (not script->memory_cells.isEmpty()) {
    QJsonObject memory_cells;
    for (auto key : script->memory_cells) memory_cells[key] = basis->memory(key);
    transport[basis->memoryValuesWk] = memory_cells;
  }
  if (script->hist_parts) {
    QJsonArray history_array;
    auto history = hp->recent(script->hist_parts);
    for (auto msg : history) 
      history_array.append(
        QString("%1: %2").arg(msg.author == Author::User ? "User" : "Jeff").arg(msg.content)
      );
    transport[basis->recentMessagesWk] = history_array;
  }
  if (script->needs_user_input) transport["user_expression"] = user_expression;
  if (not expression.properties.isEmpty()) {
    transport[basis->exprPropsWk] = Phrase::pack_props(expression.properties);
  }
  if (not script->path.length()) {
    emit script_exception(tr("The path to the module is empty."));
    return {{basis->errorTypeWk, 10}};
  }
  if (not script->fn_name.length()) {
    emit script_exception(tr("The function name is empty."));
    return {{basis->errorTypeWk, 12}};
  }
  QJsonObject result = run(script->path, script->fn_name, transport);
  basis->handle_from_script(result, true);
  return result;
}

/*! @brief Prepares data for requesting next step in scenery execution. */
QJsonObject PythonWorker::request_next_step(SceneryScript *script, const QString &user_expression) {
  QJsonObject transport;
  if (not script->memory_cells.isEmpty()) {
    QJsonObject memory_cells;
    for (auto key : script->memory_cells) memory_cells[key] = basis->memory(key);
    transport[basis->memoryValuesWk] = memory_cells;
  }
  transport["user_expression"] = user_expression;
  if (not script->path.length()) {
    emit script_exception(tr("The path to the module is empty."));
    return {{basis->errorTypeWk, 10}};
  }
  if (not script->fn_name.length()) {
    emit script_exception(tr("The function name is empty."));
    return {{basis->errorTypeWk, 12}};
  }
  QJsonObject result = run(script->path, script->fn_name, transport);
  basis->handle_from_script(result, true);
  return result;
}

/*! @brief Prepares data for custom scanning. */
QJsonObject PythonWorker::request_scan(CustomScanScript *script, const QString &user_expression) {
  QJsonObject transport;
  transport["user_expression"] = user_expression;
  return run(script->path, script->fn_name, transport);
}

/*! @brief Prepares data for custom composing. */
QJsonObject PythonWorker::request_compose(
  CustomComposeScript *script, const QString &user_expression, CacheWithIndices sorted
) {
  QJsonObject transport;
  transport["user_expression"] = user_expression;
  QJsonArray candidates;
  for (auto key : sorted.keys()) {
    QJsonObject candidate;
    QJsonObject indices;
    for (auto first_indice : sorted[key].first.keys()) {
      indices[QString::number(first_indice)] = sorted[key].first[first_indice];
    }
    candidate["indices"] = indices;
    candidate["reagent_expression"] = sorted[key].second.to_json();
    candidates.append(candidate);
  }
  transport["candidates"] = candidates;
  return run(script->path, script->fn_name, transport);
}