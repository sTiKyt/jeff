#ifndef BASIS_H
#define BASIS_H

#include "core/database/json.h"
#include "core/database/sqlite.h"
#include "core/model/keystore.h"
#include "core/model/message.h"
#include "core/model/nlp/cacher.h"
#include "core/model/nlp/options.h"
#include "core/model/source.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QList>
#include <QMutex>
#include <QPair>
#include <QSettings>
#include <QString>
#include <QTextStream>
#include <QVariant>

/*!
 * @class Basis
 * @brief Provides methods for intra-component work.
 * @details Basis does a lot of important things:
 *   - provides identifiers for saving settings
 *   - reads and writes a list of sources, settings and message history
 *   - stores a list of sources and cached expressions
 *   - checks the settings file for errors
 *   - stores context and memory map.
 * @sa SQLite, Json, PythonModule, NLPmodule, QSettings
 */
class Basis : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(Basis)
public:
  // Objects:
  SQLite *sql = new SQLite(this);    /*!< SQLite handler. */
  Cacher *cacher = new Cacher(this); /*!< Cache handler.  */
  Json *json = nullptr;              /*!< Json handler.   */

  // Constants:
  const char *companyName     = "cclc";
  const char *applicationName = "jeff";

  const char *isMenuBarHiddenSt = "jeff-qt/menubarishidden";
  const char *sizeSt            = "jeff-qt/size";
  const char *isFullScreenSt    = "jeff-qt/isfullscreen";
  const char *isNotFirstStartSt = "jeff-qt/isnotfirststart";

  const char *isMonologueEnabledSt = "core/ismonologuemodeenabled";
  const char *isDelayEnabledSt     = "core/isdelayenabled";
  const char *minDelaySt           = "core/mindelay";
  const char *maxDelaySt           = "core/maxdelay";
  const char *isKeepingEnabledSt   = "core/iskeepingenabled";
  const char *isHintsEnabledSt     = "core/ishintsenabled";
  
  const char *defaultSourcePath      = "sources/defaultsourcepath";
  const char *defaultSourceContainer = "sources/defaultsourcecontainer";
  
  const char *serverAuthorizationOnSt = "server/authorize";
  const char *serverAuthKeySt         = "server/auth_key";
  
  const char *scriptPathWk   = "script_path";
  const char *funcNameWk     = "func_name";
  const char *getHistWk      = "get_hist";
  const char *errorTypeWk    = "error_type";
  const char *recentWk       = "recent";
  const char *readMemoryWk   = "need_values";
  const char *readContextWk  = "need_context";
  const char *valuesWk       = "values";
  const char *contextWk      = "context";
  const char *exprPropsWk    = "expression_properties";
  const char *writeMemoryWk  = "store_values";
  const char *writeContextWk = "store_context";
  const char *sendWk         = "send";
  const char *searchAgainWk  = "search_again";
  const char *sendAsUserWk   = "send_as_user";
  const char *sendStatusWk   = "send_status";
  const char *authKeyWk      = "auth_key";

  // Functions:
  /*! @brief The constructor. */
  Basis(QObject *parent = nullptr) : QObject(parent) {
    json = new Json(get_settings_path(), this);
    load_sources();
    load_memory();
  }
  
  /*! @brief The destructor. */
  ~Basis() { save_memory(); }

  /*! @brief Determines if a settings file exists. */
  inline bool exists() { return QFile::exists(_settings.fileName()); }
  /*! @brief Determines if a database specified by @a filename exists. */
  inline bool db_exists(const QString &filename) { return QFile::exists(filename); }
  /*! @brief Determines whether the settings file is readable or writable. */
  inline bool accessible() { return _settings.status() != QSettings::AccessError; }
  /*! @brief Determines the correctness of the settings file format. */
  inline bool correct() { return _settings.status() != QSettings::FormatError; }
  
  /*! @brief Reads the setting. */
  inline QVariant read(const QString &key)       { return _settings.value(key); }
  inline QVariant operator[](const char *key)    { return _settings.value(key); }
  inline QVariant operator[](const QString &key) { return read(key); }
  
  /*! @brief Says if there is a parameter with this key in the settings. */
  inline bool contains(const char *key) { return _settings.contains(key); }
  /*! @brief Determines where application settings are stored. */
  inline QString get_settings_path() { return QFileInfo(_settings.fileName()).absolutePath(); }

  // Functions described in `basis.cpp`:
  void write(const QString &key, const QVariant &data);
  void check_settings_file();
  void check_default_source();
  void load_sources();
  void load_memory();
  void save_memory();
  Sources sources();
  void sources(Sources s);
  void context(const QString &key, const QString &value);
  void memory(const QString &key, QJsonValue data);
  QString context(const QString &key);
  QJsonValue memory(const QString &key);
  QJsonObject handle_to_script(const QJsonObject &object);
  void handle_from_script(const QJsonObject &object, bool except_send = false);

signals:
  /*! @brief Reports a result of checking the settings file, if it is incorrect. */
  QString settings_warning(QString warning_text);
  QString send(QString outter_message);
  QString search_again(QString rephrased_message);
  QString send_as_user(QString outter_message);
  QPair<QString, QString> send_status(QPair<QString, QString> id_and_message);

private:
  // Objects:
  QMutex sources_mutex;
  QMutex context_mutex;
  QMutex memory_mutex;
  Sources _sources; /*!< List of sources for @a NLPmodule. */
  Options _context; /*!< Context values (@sa AIML). */
  KeyStore _memory; /*!< Long-life memory. */
  
  /*! Qt settings object. */
  QSettings _settings = QSettings(
    QSettings::IniFormat, QSettings::UserScope, companyName, applicationName
  );
  
  // Functions described in `basis.cpp`:
  void set_first_source_as_default();
};

#endif
