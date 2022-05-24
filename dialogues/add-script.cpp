#include "add-script.h"

/*! @brief The constructor. */
AddScriptDialog::AddScriptDialog(
  QWidget *parent = nullptr,
  Basis *_basis = nullptr,
  PythonModule *_pm = nullptr, 
  ModalHandler *m_handler = nullptr
) : ScrollFreezerWidget(parent), basis(_basis), pm(_pm), _m_handler(m_handler) {
  if (_m_handler) {
    _m_handler->setPrisoner(this);
    setObjectName("add_script_dialog");
  }
  auto *path_info = new QLabel(tr("Specify script path:"), this);
  stype_info = new QLabel(tr("Specify script type:"), this);
  path_input = new Button(tr("Select a file..."), this);
  connect(path_input, &Button::clicked, this, [this]() {
    auto _path = 
      QFileDialog::getOpenFileName(nullptr, tr("Select file..."), "", tr("Python script") + "(*.py)");
    if (not _path.isEmpty()) path_input->setText(_path); 
    else path_input->setText(tr("Select a file..."));
  });
  stype_input = new ComboBox(this);
  stype_input->addItems({
    tr("Startup script (suitable for startup prompts)"),
    tr("Daemon (works in background with Jeff's server)"),
    tr("Server (works in background and receives all messages)"),
    tr("Custom scanner (another answering system)"),
    tr("Custom composer (receives chosed variants and answers on them in another manner)")
  });
  connect(
    stype_input, QOverload<int>::of(&QComboBox::currentIndexChanged),
    this, [this](int _stype) { change_stype(_stype); }
  );
  auto *cancel_btn = new Button(tr("Cancel"), this);
  cancel_btn->setIcon(QIcon::fromTheme("window-close", QIcon(":/arts/icons/16/window-close.svg")));
  save_btn = new Button(tr("Add script"), this);
  save_btn->setIcon(QIcon::fromTheme("dialog-ok-apply", QIcon(":/arts/icons/16/dialog-ok-apply.svg")));
  if (not _m_handler) save_btn->setText(tr("Save script"));
  auto *buttons_layout = new QGridLayout();
  buttons_layout->setSpacing(0);
  buttons_layout->setMargin(0);
  buttons_layout->addWidget(cancel_btn, 0, 0);
  buttons_layout->addWidget(save_btn, 0, 1);
  auto *buttons_widget = new QWidget(this);
  buttons_widget->setContentsMargins(0, 0, 0, 0);
  buttons_widget->setLayout(buttons_layout);
  dynamic_properties_layout = new QGridLayout();
  dynamic_properties_layout->setMargin(0);
  auto *dynamic_properties_widget = new QWidget(this);
  dynamic_properties_widget->setContentsMargins(0, 0, 0, 0);
  dynamic_properties_widget->setLayout(dynamic_properties_layout);
  auto *main_layout = new QGridLayout();
  main_layout->addWidget(path_info, 0, 0);
  main_layout->addWidget(path_input, 0, 1);
  main_layout->addWidget(stype_info, 1, 0);
  main_layout->addWidget(stype_input, 1, 1);
  main_layout->addWidget(dynamic_properties_widget, 2, 0, 1, 2);
  main_layout->addWidget(buttons_widget, 3, 0, 1, 2);
  setLayout(main_layout);
  if (_m_handler) connect(cancel_btn, &Button::clicked, this, [this] { _m_handler->closePrisoner(); });
  change_stype();
}

/*! @brief The destructor. */
AddScriptDialog::~AddScriptDialog() {
  QLayoutItem *child = nullptr;
  if (dynamic_properties_layout->count()) 
    while ((child = dynamic_properties_layout->takeAt(0)) != nullptr) {
      delete child->widget();
      delete child;
    }
}

/*! @brief Changes the layout depending on the selected script type. */
void AddScriptDialog::change_stype() {
  disconnect(save_btn, &Button::clicked, nullptr, nullptr);
  stype_input->setEnabled(false);
  QLayoutItem *child = nullptr;
  if (dynamic_properties_layout->count()) 
    while ((child = dynamic_properties_layout->takeAt(0)) != nullptr) {
      delete child->widget();
      delete child;
    }
  if (stype == 0) {
    // Startup script (suitable for startup prompts)
    auto *fn_name_info = new QLabel(tr("Specify function name:"));
    auto *fn_name_input = new LineEdit();
    fn_name_input->setPlaceholderText(tr("Function name..."));
    auto *memory_cells_info = new QLabel(tr("Add the memory cells to be passed to the script:"));
    auto *memory_cells_list = new EditList();
    memory_cells_list->set_add_btn_text(tr("Add memory cell"));
    memory_cells_list->set_rem_btn_text(tr("Remove selected cell"));
    memory_cells_list->set_lineedit_placeholder_text(tr("Memory cell name..."));
    memory_cells_list->set_list_headers({tr("Cells list")});
    dynamic_properties_layout->addWidget(fn_name_info, 0, 0);
    dynamic_properties_layout->addWidget(fn_name_input, 1, 0);
    dynamic_properties_layout->addWidget(memory_cells_info, 2, 0);
    dynamic_properties_layout->addWidget(memory_cells_list, 3, 0);
    connect(save_btn, &Button::clicked, this, [this, fn_name_input, memory_cells_list]() {
      if (
        path_input->text() == tr("Select a file...") or
        fn_name_input->text().isEmpty() or
        memory_cells_list->is_empty()
      ) {
        basis->warn_about(tr("Please complete all fields before saving."));
        return;
      }
      auto *startup_script = new StartupScript();
      startup_script->path = path_input->text();
      startup_script->memory_cells = memory_cells_list->get_list();
      startup_script->fn_name = fn_name_input->text();
      if (_m_handler) {
        pm->add_script(startup_script);
        _m_handler->closePrisoner();
      } else {
        emit saved(startup_script->toJson());
        delete startup_script;
      }
    });
  } else if (stype == 1) {
    // Daemon (works in background with Jeff's server)
  } else if (stype == 2) {
    // Server (works in background and receives all messages)
  } else if (stype == 3) {
    // Custom scanner (another answering system)
  } else if (stype == 4) {
    // Custom composer (receives chosed variants and answers on them in another manner)
  } else if (stype == 5) {
    /*! This option is not listed and is responsible for editing react scripts saved as 
     *  phrase text in the database. */
    auto *specify_history_amount = 
      new QLabel(tr("Specify amount of message history to be sent:"), this);
    specify_history_amount->setWordWrap(true);
    auto *history_amount = new QSpinBox(this);
  }
  stype_input->setEnabled(true);
}

/*! @brief Changes the script type and then calls the layout change function. */
void AddScriptDialog::change_stype(int _stype) { stype = _stype; change_stype(); }

/*! @brief Fixes the script type, making it immutable. */
void AddScriptDialog::set_stype(int _stype) {
  disconnect(stype_input, QOverload<int>::of(&QComboBox::currentIndexChanged), nullptr, nullptr);
  stype_input->setEnabled(false);
  stype_input->hide();
  stype_info->hide();
}

/*! @brief TBD */
bool AddScriptDialog::load_from_text(QString json_text) {
  if (json_text.isEmpty()) return false;
  auto *script = ScriptsCast::to_script(json_text);
  if (not script) return false;
  return load_from_script(script);
}

/*! @brief TBD */
bool AddScriptDialog::load_from_script(ScriptMetadata *script) {
  if (script->stype == ScriptType::Startup) {
    auto *s = dynamic_cast<StartupScript *>(script);
    
    return true;
  } else if (script->stype == ScriptType::Daemon) {
    auto *s = dynamic_cast<DaemonScript *>(script);
    
    return true;
  } else if (script->stype == ScriptType::Server) {
    auto *s = dynamic_cast<ServerScript *>(script);
    
    return true;
  } else if (script->stype == ScriptType::CustomScan) {
    auto *s = dynamic_cast<CustomScanScript *>(script);
    
    return true;
  } else if (script->stype == ScriptType::CustomCompose) {
    auto *s = dynamic_cast<CustomComposeScript *>(script);
    
    return true;
  } else if (script->stype == ScriptType::React) {
    auto *s = dynamic_cast<ReactScript *>(script);
    set_stype(5);
    return true;
  } else return false;
}
