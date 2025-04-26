#include "TCPManager.h"


TCPManager::TCPManager():
    _head_aready(false),
    _message_id(0),
    _message_len(0)
{
    // TCP连接成功时的回调函数
    QObject::connect(&_socket, &QTcpSocket::connected, this, [&](){
        emit sig_connect_success(true);
    });

    // 消息可读取时的回调函数
    QObject::connect(&_socket, &QTcpSocket::readyRead, this, [&](){
        _buffer.append(_socket.readAll());
        QDataStream stream(&_buffer, QIODevice::ReadOnly);
        while(true){
            if(_head_aready == false){
                if(_buffer.size() < 4) return;
                // 读取出消息id和长度
                stream >> _message_id >> _message_len;
                _buffer = _buffer.mid(4);  // 将buffer中前4个字节去除
                _head_aready = true;
            }
            else{  // _head_aready == true
                if(_buffer.size() < _message_len) return;
                // 读取消息体
                QByteArray message_body = _buffer.mid(0, _message_len);
                qDebug() << "message id:" << _message_id;
                qDebug() << "message len:" << _message_len;
                qDebug() << "message:" << message_body;
                _buffer = _buffer.mid(_message_len);
                _head_aready = false;
                handle_msg(static_cast<RequireId>(_message_id), _message_len, message_body);
            }
        }
    });

    // 错误处理
    QObject::connect(&_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred), [&](QAbstractSocket::SocketError socketError) {
        Q_UNUSED(socketError)
        qDebug() << "Error:" << _socket.errorString();
        emit sig_connect_success(false);
    });

    // 连接断开回调函数
    QObject::connect(&_socket, &QTcpSocket::disconnected, this, [&](){
        qDebug() << "Disconnected from server.";
    });

    // 注册消息处理函数
    init_handlers();
}


void TCPManager::init_handlers()
{
    // 登录请求回包处理
    _handlers.insert(ID_CHAT_LOGIN_RSP, [this](int id, int len, QByteArray data){
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if(jsonDoc.isNull()){
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject json = jsonDoc.object();

        if(!json.contains("error")){
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Login Failed, err is Json Parse Err" << err ;
            emit sig_login_failed(err);
            return;
        }

        int err = json["error"].toInt();
        if(err != ErrorCodes::SUCCESS){
            qDebug() << "Login Failed, err is " << err ;
            emit sig_login_failed(err);
            return;
        }

        UserManager::GetInstance()->set_uid(json["uid"].toInt());
        UserManager::GetInstance()->set_name(json["name"].toString());
        UserManager::GetInstance()->set_token(json["token"].toString());
        emit sig_switch_chatdlg();  // 切换至聊天窗口
    });
}



void TCPManager::slot_connect_tcp(ServerInfo si)
{
    qDebug() << si.host << ":" << si.port.toUInt();
    _socket.connectToHost(si.host, si.port.toUInt());
}

// 发送数据
void TCPManager::send_msg(RequireId id, QString data)
{
    quint16 msg_len = static_cast<quint16>(data.size());
    QByteArray package;
    QDataStream out(&package, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);  // 设置网络字节序
    out << static_cast<quint16>(id) << msg_len;
    package.append(data);
    // 发送数据
    _socket.write(package);
}


// 处理回包
void TCPManager::handle_msg(RequireId id, int len, QByteArray data)
{
    auto it = _handlers.find(id);
    if(it == _handlers.end()){
        qDebug() << "unknow require id to handle: " << id;
        return;
    }
    it.value()(id, len, data);
}



