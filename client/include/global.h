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
};

enum Modules{
    MOD_REGISTER = 0,
    MOD_RESET = 1,
};

enum ErrorCodes{
    SUCCESS = 0,
    ERR_JSON = 1,  // json解析失败
    ERR_NETWORK = 2,  // 网络错误
};

extern QString gate_url_prefix;


#endif // GLOBAL_H
