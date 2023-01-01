#ifndef CORE_H
#define CORE_H

#include "core-kit/basis.h"
#include "core-kit/history-processor.h"
#include "core-kit/jeff-core-kit.h"
#include "core-kit/local-server.h"
#include "core-kit/notify-client.h"
#include "core-kit/script-engine-module.h"
#include "core-kit/standard-templates.h"
#include "core-kit/database/sqlite.h"
#include "core-kit/model/python/script.h"
#include "dialogues/modal-handler.h"
#include <QObject>
#include <QPair>
#include <QTimer>

/*!
 * @class Core
 * @brief Controls I/O.
 * @details Manages objects associated with receiving messages from different sources.
 * @sa Basis, HProcessor, NLPmodule, StdTemplates, MessageData
 */
class Core : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(Core)
public:
  // Objects:
  Basis *basis = new Basis();
  HProcessor *hp = new HProcessor(basis);
  class Server *server = new class Server(basis);
  NotifyClient *notifier = new NotifyClient();
  SEModule *sem = new SEModule(hp, basis, notifier);

  // Functions described in `core.cpp`:
  Core(QObject *parent = nullptr);
  ~Core();
  void got_message_from_user(const QString &user_expression);
  void got_message_from_jck(const QString &result_expression);
  void got_scenario_start(ScenarioScript *scenario);
  void got_scenario_shutting();
  void notify_scenario_first_time();
  void got_message_from_script(const QString &outter_message);
  void got_message_to_search_again(const QString &rephrased_message);
  void got_message_from_script_as_user(const QString &message);
  void got_status_from_script(QPair<QString, QString> id_and_message);
  void got_warning(const QString &warning_text);
  void got_error(const QString &error_text);
#ifdef JEFF_WITH_QT_WIDGETS
  void got_modal(ModalHandler *m_handler);
#endif
  void set_monologue_enabled(const bool enabled);
  MessageData get_message(const QString &content, Author author, 
                          ContentType content_type, Theme theme);
  void start();

signals:
#ifdef JEFF_WITH_QT_WIDGETS
  /*! @brief Sends a message to Display. */
  ModalHandler *show_modal(MessageData message_data, ModalHandler *handler);
  /*! @brief Sets the monologue mode in MenuBar. */
  bool changeMenuBarMonologueCheckbox(bool enabled);
#endif
  /*! @brief Sends a message. */
  MessageData show(MessageData message_data);
  /*! @brief Sends an updateable message. */
  QPair<QString, MessageData> show_status(QPair<QString, MessageData> id_and_message_data);

private:
  // Objects:
  bool monologue_enabled = false;
  JCKController *jck = new JCKController(basis, hp);
  StandardTemplates *std_templates = new StandardTemplates(basis, hp, sem);
  CustomScanScript *custom_scanner = nullptr;
  CustomComposeScript *custom_composer = nullptr;
  ScenarioScript *current_scenario = nullptr;
};

#endif