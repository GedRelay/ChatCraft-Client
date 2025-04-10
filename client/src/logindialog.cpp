#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    ui->forget_btn->setCursor(Qt::PointingHandCursor);
}

LoginDialog::~LoginDialog()
{
    qDebug("LoginDialog Destructor");
    delete ui;
}

void LoginDialog::on_register_btn_clicked()
{
    emit sigSwitchRegister();
}


void LoginDialog::on_forget_btn_clicked()
{
    emit sigSwitchReset();
}

