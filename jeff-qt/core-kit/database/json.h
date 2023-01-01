#ifndef JSON_H
#define JSON_H

#include "core-kit/database/sqlite.h"
#include "core-kit/model/keystore.h"
#include "core-kit/model/message.h"
#include "core-kit/model/nlp/cache.h"
#include "core-kit/model/python/script.h"
#include "core-kit/model/source.h"
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QList>

/*!
 * @class Json
 * @brief Provides methods for reading and writing data in/from json files.
 * @sa SQLite
 */
class Json : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(Json)
public:
  // Functions described in `json.cpp`:
  Json(QString settingsPath, QObject *parent = nullptr);
  Sources  read_source_list     (SQLite *sql) ;
  Messages read_message_history (QFile *file) ;
  Messages read_message_history ()            ;
  Cache    read_NLP_cache       ()            ;
  Scripts  read_scripts         ()            ;
  KeyStore read_memory          ()            ;
  void write_source_list     (SQLite *sql, Sources sources)          ;
  void write_message_history (Messages message_history, QFile *file) ;
  void write_message_history (Messages message_history)              ;
  void write_NLP_cache       (Cache cache)                           ;
  void write_scripts         (Scripts scripts)                       ;
  void write_memory          (KeyStore memory)                       ;

private:
  // Constants:
  const char *sources_store_filename = "jeff_sources.json"   ;
  const char *aiml_store_filename    = "jeff_aiml_files.json";
  const char *cache_store_filename   = "jeff_nlp_cache.json" ;
  const char *history_store_filename = "jeff_history.json"   ;
  const char *scripts_store_filename = "jeff_scripts.json"   ;
  const char *memory_store_filename  = "jeff_memory.json"    ;
  const char *subdir_name            = "jeff-qt"             ;

  // Objects:
  QString _settings_path;

  // Functions described in `json.cpp`:
  void check_or_create_subdir();
  QJsonArray read_json(QFile *file);
  void write_json(QFile *savefile, const QJsonArray &json_array);
};

#endif