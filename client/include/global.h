#ifndef GLOBAL_H
#define GLOBAL_H
#include <QWidget>
#include <functional>
#include "QStyle"
#include <QRegularExpression>
#include <QByteArray>
#include <QNetworkReply>
#include <QJsonObject>
#include <QDir>
#include <QSettings>

// 用来刷新qss
extern std::function<void(QWidget*)> repolish;
extern QString md5Encrypt(const QString &input_str);

enum RequireId{
    ID_REGISTER_GET_VERIFY_CODE = 1001,  // 注册获取验证码
    ID_USER_REGESTER = 1002,  // 注册用户
    ID_RESET_GET_VERIFY_CODE = 1003,  // 重置时获取验证码
    ID_RESET = 1004,  // 重置密码
    ID_USER_LOGIN = 1005,  // 登录
    ID_CHAT_LOGIN = 1006,  // 登录聊天服务器
    ID_CHAT_LOGIN_RSP = 1007,  // 登录回复
};

enum Modules{
    MOD_REGISTER = 0,  // 注册模块
    MOD_RESET = 1,  // 重置模块
    MOD_LOGIN = 2,  // 登录模块
};

enum ErrorCodes{
    SUCCESS = 0,
    ERR_JSON = 1,  // json解析失败
    ERR_NETWORK = 2,  // 网络错误
};

struct ServerInfo{
    QString host;
    QString port;
    QString token;
    int uid;
};

extern QString gate_url_prefix;


#endif // GLOBAL_H
