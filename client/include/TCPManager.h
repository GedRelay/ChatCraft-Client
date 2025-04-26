#ifndef TCPMANAGER_H
#define TCPMANAGER_H

#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include "global.h"
#include "Singleton.h"
#include <QMap>
#include "UserManager.h"

class TCPManager: public QObject,
                   public Singleton<TCPManager>,
                   public std::enable_shared_from_this<TCPManager>
{
    friend class Singleton<TCPManager>;
    Q_OBJECT;
public:
    ~TCPManager() = default;

private:
    TCPManager();
    void init_handlers();
    void handle_msg(RequireId id, int len, QByteArray dat);
    QMap<RequireId, std::function<void(RequireId id, int len, QByteArray data)>> _handlers;

    QTcpSocket _socket;
    QByteArray _buffer;
    bool _head_aready;
    quint16 _message_id;
    quint16 _message_len;


signals:
    void sig_connect_success(bool success);
    void sig_login_failed(int err);
    void sig_switch_chatdlg();
    void send_msg(RequireId id, QString data);

public slots:
    void slot_connect_tcp(ServerInfo);
};

#endif // TCPMANAGER_H
