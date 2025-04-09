#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include "global.h"
#include <QTimer>

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

private slots:
    void on_get_code_btn_clicked();
    void slot_reg_mod_finish(RequireId req_id, QString res, ErrorCodes err);
    void on_confirm_btn_clicked();
    void on_user_edit_editingFinished();
    void on_email_edit_editingFinished();
    void on_password_edit_editingFinished();
    void on_password2_edit_editingFinished();
    void on_verify_edit_editingFinished();

    void on_password_visible_clicked();

    void on_password2_visible_clicked();

    void on_cancel_btn_clicked();

    void on_success_btn_clicked();

private:
    void showTip(const char* msg ,bool is_error = true);
    void showTip(const QString &msg, bool is_error = true);
    void initHttpHandles();
    bool CheckUserName();
    bool CheckEmail();
    bool CheckPassword();
    bool CheckPassword2();
    bool CheckVerifyCode();
    bool CheckAllEdit();
    void ChangeSuccessPage();
    QString md5Encrypt(const QString &input_str);

    QTimer *_countdown_timer;
    int _countdown;

    Ui::RegisterDialog *ui;
    QMap<RequireId, std::function<void(const QJsonObject&)>> _handlers;

signals:
    void sigSwitchLogin();
};

#endif // REGISTERDIALOG_H
