#include "global.h"

QString gate_url_prefix = "";

std::function<void(QWidget*)> repolish = [](QWidget* w){
    w->style()->unpolish(w);
    w->style()->polish(w);
};


// 字符串md5加密
QString md5Encrypt(const QString &input_str)
{
    QByteArray byte_array = input_str.toUtf8();
    QByteArray hash = QCryptographicHash::hash(byte_array, QCryptographicHash::Md5);
    return QString(hash.toHex());
}
