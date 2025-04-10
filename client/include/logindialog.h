#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

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
    Ui::LoginDialog *ui;

signals:
    void sigSwitchRegister();
    void sigSwitchReset();
private slots:
    void on_register_btn_clicked();
    void on_forget_btn_clicked();
};

#endif // LOGINDIALOG_H
