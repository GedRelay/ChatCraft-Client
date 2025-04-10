#ifndef RESETDIALOG_H
#define RESETDIALOG_H

#include <QDialog>
#include "global.h"

namespace Ui {
class ResetDialog;
}

class ResetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResetDialog(QWidget *parent = nullptr);
    ~ResetDialog();

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

    Ui::ResetDialog *ui;
    QMap<RequireId, std::function<void(const QJsonObject&)>> _handlers;

signals:
    void sigSwitchLogin();

private slots:
    void on_cancel_btn_clicked();
    void on_user_edit_editingFinished();
    void on_email_edit_editingFinished();
    void on_password_edit_editingFinished();
    void on_password2_edit_editingFinished();
    void on_verifycode_edit_editingFinished();
    void on_password_visible_clicked();
    void on_password2_visible_clicked();
    void on_get_verifycode_btn_clicked();

    void slot_reset_mod_finish(RequireId req_id, QString res, ErrorCodes err);
    void on_confirm_btn_clicked();
};

#endif // RESETDIALOG_H
