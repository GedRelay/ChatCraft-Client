#ifndef HTTPMANAGER_H
#define HTTPMANAGER_H
#include "global.h"
#include "Singleton.h"
#include <QString>
#include <QUrl>
#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>

class HttpManager :
                    public QObject,
                    public Singleton<HttpManager>,
                    public std::enable_shared_from_this<HttpManager>{
    friend class Singleton<HttpManager>;
    Q_OBJECT
public:
    ~HttpManager();
    void PostHttpReq(QUrl url, QJsonObject json, RequireId req_id, Modules mod);

private:
    HttpManager();
    QNetworkAccessManager _manager;


private slots:
    void slot_http_finish(RequireId req_id, QString res, ErrorCodes err, Modules mod);

signals:
    void sig_http_finish(RequireId req_id, QString res, ErrorCodes err, Modules mod);
    void sig_reg_mod_finish(RequireId req_id, QString res, ErrorCodes err);
    void sig_reset_mod_finish(RequireId req_id, QString res, ErrorCodes err);
};


#endif // HTTPMANAGER_H
