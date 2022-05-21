#ifndef NOTIFY_CLIENT
#define NOTIFY_CLIENT

#include "core/model/message.h"
#include "core/model/python/script.h"
#include <QHostAddress>
#include <QJsonObject>
#include <QList>
#include <QTcpSocket>

/*! @class NotifyClient
 *  @brief TBD  */
class NotifyClient : public QObject {
  Q_OBJECT
public:
  // Functions described in `notify-client.cpp`:
  NotifyClient(QObject *parent = nullptr);
  void subscribe(ServerScript *script);
  void unsubscribe(ServerScript *script);
  void unsubscribe_all();
  void notify(MessageData mdata);
  
private:
  // Objects:
  QList<ServerScript *> scripts;
  
  // Functions described in `notify-client.cpp`:
  void send_event(MessageData mdata, QHostAddress addr, quint16 port);
};

#endif