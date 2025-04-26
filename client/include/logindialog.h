#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "global.h"
#include "HttpManager.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private:
    void showHint(const char* msg ,bool is_error = true, int time = 3);
    void showHint(QString msg ,bool is_error = true, int time = 3);
    bool CheckEmail();
    bool CheckPassword();
    void LockOperation();
    void UnlockOperation();

    Ui::LoginDialog *ui;
    QTimer *_countdown_timer;
    int _countdown;
    int _uid;
    QString _token;

signals:
    void sigSwitchRegister();
    void sigSwitchReset();
    void sig_connect_tcp(ServerInfo si);

private slots:
    void slot_login_mod_finish(RequireId req_id, QString res, ErrorCodes err);
    void on_register_btn_clicked();
    void on_forget_btn_clicked();
    void on_login_btn_clicked();
    void on_password_visible_clicked();
    void slot_tcp_connect_finish(bool success);
    void slot_login_failed(int err);
};

#endif // LOGINDIALOG_H
