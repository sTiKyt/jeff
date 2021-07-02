#ifndef SOURCE_H
#define SOURCE_H

#include <QJsonObject>
#include <QString>

/*!
 * @class Source
 * @brief Contains data about the source of regular expressions for the
 * NLPmodule.
 */
class Source {
public:
  /*! Constructors. */
  Source() {}
  Source(const QJsonObject &json_object) {
    path = json_object["path"].toString();
    table_name = json_object["container"].toString();
    table_title = json_object["title"].toString();
    is_disabled = json_object["disabled"].toBool();
  }
  /*! Path to database. */
  QString path = "";
  /*! Table name [UUID, name of database table]. */
  QString table_name = "";
  /*! Table title [username]. */
  QString table_title = "";
  /*! Means that does not participate in the selection of reagents. */
  bool is_disabled = false;
  /*! Means that not available for adding expressions from ASW. */
  bool is_read_only = false;
  /*! Means that unavailable until purposeful entry. */
  bool is_private = false;
  /*! Means that the search for reagents occurs exclusively in it. */
  bool is_catching = false;
  /*! Means that the input is first validated using this source. */
  bool is_prioritised = false;
  /*! Compares two sources. They are identical if they have the same table name
   * and are in the same database. */
  friend bool operator==(Source s1, Source s2) {
    return s1.table_name == s2.table_name and s1.path == s2.path;
  }
  /*!
   * @fn to_json
   * @brief Turns @a source into a JSON object.
   * @param[in] source source parameters
   * @returns converted properties of @a source
   */
  QJsonObject to_json() {
    return {{"container", table_name},
            {"disabled", is_disabled},
            {"path", path},
            {"title", table_title}};
  }
};

#endif // SOURCE_H
