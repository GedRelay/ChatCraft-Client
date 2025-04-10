#include "HttpManager.h"


HttpManager::HttpManager()
{
    connect(this, &HttpManager::sig_http_finish, this, &HttpManager::slot_http_finish);
}

HttpManager::~HttpManager()
{

}

void HttpManager::PostHttpReq(QUrl url, QJsonObject json, RequireId req_id, Modules mod)
{
    // 创建一个HTTP POST请求，并设置请求头和请求体
    QByteArray data = QJsonDocument(json).toJson();
    // 通过构造http请求
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));
    // 发送请求
    QNetworkReply * reply = _manager.post(request, data);
    // 设置信号和槽等待发送完成
    QObject::connect(reply, &QNetworkReply::finished, [reply, req_id, mod, self = shared_from_this()](){
        // 处理错误的情况
        if(reply->error() != QNetworkReply::NoError){
            qDebug() << reply->errorString();
            // 发送信号通知完成
            emit self->sig_http_finish(req_id, "", ErrorCodes::ERR_NETWORK, mod);
            reply->deleteLater();
            return;
        }

        // 无错误则读回请求
        QString res = reply->readAll();

        // 发送信号通知完成
        emit self->sig_http_finish(req_id, res, ErrorCodes::SUCCESS, mod);
        reply->deleteLater();
        return;
    });
}

void HttpManager::slot_http_finish(RequireId req_id, QString res, ErrorCodes err, Modules mod)
{
    // 注册模块
    if(mod == Modules::MOD_REGISTER){
        // 发送信号通知指定http
        emit sig_reg_mod_finish(req_id, res, err);
    }

    // 重置密码模块
    else if(mod == Modules::MOD_RESET){
        emit sig_reset_mod_finish(req_id, res, err);
    }
}




