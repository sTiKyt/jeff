#ifndef JCK_H
#define JCK_H

#include "core-kit/basis.h"
#include "core-kit/database/sqlite.h"
#include "core-kit/model/expression.h"
#include "core-kit/model/nlp/cache.h"
#include "core-kit/model/nlp/cacher.h"
#include "core-kit/model/nlp/stringssearch.h"
#include "core-kit/model/python/worker.h"
#include "core-kit/standard-templates.h"
#include <QList>
#include <QMap>
#include <QObject>
#include <QPair>
#include <QRandomGenerator>
#include <QStringList>
#include <QSqlDatabase>
#include <QThread>
#include <QTime>

/*!
 * @class JeffCoreKit @aka JCK
 * @brief Looks for regular expressions in user input and displays answers to them.
 * @sa Basis, JCKController
 */
class JCK : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(JCK)
public:
  // Functions described in `jeff-core-kit.cpp`:
  JCK(Basis *_basis, HProcessor *_hp, QObject *parent = nullptr);
  ~JCK();
  void thread_setup();
  void load_cache();
  void save_cache();
  void search_for_suggests(const QString &input);
  void set_default_scanner();
  void set_custom_scanner(CustomScanScript *custom_scanner);
  void set_default_composer();
  void set_custom_composer(CustomComposeScript *custom_composer);

signals:
  /*! @brief Sends a response expression to @a Core. */
  QString response(QString response);
  /*! @brief Notifies of a script error. */
  QString script_exception(QString error);
  /*! @brief Informs that JCK needs to wait for the scenario to run and sends it the last message. */
  ScenarioScript *setup_scenario(ScenarioScript *script);

private:
  // Objects:
  Basis *basis = nullptr;
  PythonWorker *pw = nullptr;
  HProcessor *hp = nullptr;
  QRandomGenerator *gen = nullptr;
  CustomScanScript *scanner = nullptr;
  CustomComposeScript *composer = nullptr;

  // Constants:
  const char *cache_path = "";
  const char *thread_conn_wk = "thread_conn";

  // Functions described in `jeff-core-kit.cpp`:
  CacheWithIndices select_from_cache(const QString &input);
  CacheWithIndices select_from_db(const QString &input);
  CacheWithIndices select_candidates(CacheWithIndices selection, QString input);
  QPair<QString, QString> compose_answer(QString input, CacheWithIndices candidates);
  void reset_cache_use_cases(CacheWithIndices &selection);
};

/*! @class JCKController
 *  @brief TBD  */
class JCKController : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(JCKController)
public:
  // Functions:
  /*! @brief The constructor. */
  JCKController(Basis *_basis, HProcessor *_hp, QObject *parent = nullptr) : QObject(parent) {
    auto *jck = new JCK(_basis, _hp);
    jck->moveToThread(&jck_thread);
    connect(&jck_thread, &QThread::started, jck, &JCK::thread_setup);
    connect(&jck_thread, &QThread::finished, jck, &QObject::deleteLater);
    connect(this, &JCKController::sfs, jck, &JCK::search_for_suggests);
    connect(this, &JCKController::scs, jck, &JCK::set_custom_scanner);
    connect(this, &JCKController::scc, jck, &JCK::set_custom_composer);
    connect(jck, &JCK::script_exception, this, [this](QString e) {
      emit script_exception(e);
    });
    connect(jck, &JCK::response, this, [this](QString r) { emit response(r); });
    connect(jck, &JCK::setup_scenario, this, [this](ScenarioScript *s) {
      emit setup_scenario(s);
    });
    jck_thread.start();
  }
  /*! @brief The destructor. */
  ~JCKController() {
    jck_thread.quit();
    jck_thread.wait();
  }
  void search_for_suggests(const QString &input) { emit sfs(input); }
  void set_custom_scanner(CustomScanScript *custom_scanner) { emit scs(custom_scanner); }
  void set_custom_composer(CustomComposeScript *custom_composer) { emit scc(custom_composer); }

signals:
  /*! @brief Functions to thread. */
  QString sfs(QString input);
  CustomScanScript *scs(CustomScanScript *script);
  CustomComposeScript *scc(CustomComposeScript *script);
  /*! @brief Real JCK signals. */
  QString response(QString response);
  QString script_exception(QString error);
  ScenarioScript *setup_scenario(ScenarioScript *script);

private:
  // Objects:
  QThread jck_thread;
};

#endif