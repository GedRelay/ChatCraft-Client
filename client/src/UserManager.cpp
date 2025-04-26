#include "../include/UserManager.h"

UserManager::UserManager()
{

}


void UserManager::set_name(QString name)
{
    _name = name;
}


void UserManager::set_uid(int uid)
{
    _uid = uid;
}


void UserManager::set_token(QString token)
{
    _token = token;
}
