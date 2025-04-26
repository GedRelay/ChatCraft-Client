#ifndef USERMANAGER_H
#define USERMANAGER_H

#include "global.h"
#include "Singleton.h"

class UserManager: public QObject,
                    public Singleton<UserManager>,
                    public std::enable_shared_from_this<UserManager>
{
    friend class Singleton<UserManager>;
    Q_OBJECT;
public:
    ~UserManager() = default;
    void set_name(QString name);
    void set_uid(int uid);
    void set_token(QString token);

private:
    UserManager();
    QString _name;
    QString _token;
    int _uid;
};

#endif // USERMANAGER_H
