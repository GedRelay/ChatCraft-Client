#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "HttpManager.h"


RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
    ui->password_edit->setEchoMode(QLineEdit::Password);
    ui->password2_edit->setEchoMode(QLineEdit::Password);
    // 设置错误提示label的样式
    ui->error_label->setProperty("state","normal");
    repolish(ui->error_label);
    // 连接注册信号和处理的槽函数
    connect(HttpManager::GetInstance().get(), &HttpManager::sig_reg_mod_finish, this, &RegisterDialog::slot_reg_mod_finish);
    // 注册http处理回调函数
    initHttpHandles();

}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::on_get_code_btn_clicked()
{
    // 检测email是否合规
    QString email = ui->email_edit->text();
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch();
    if(match){
        // 发送验证码
        qDebug("发送验证码");
    }
    else{
        showTip(tr("邮箱地址不正确"), true);
    }
}

void RegisterDialog::slot_reg_mod_finish(RequireId req_id, QString res, ErrorCodes err)
{
    if(err != ErrorCodes::SUCCESS){
        showTip(tr("网络请求错误"), true);
        return;
    }

    // 解析json字符串
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    if(jsonDoc.isNull()){
        showTip(tr("json解析失败"), true);
        return;
    }
    if(!jsonDoc.isObject()){
        showTip(tr("json解析失败"), true);
        return;
    }

    _handlers[req_id](jsonDoc.object());
}

void RegisterDialog::showTip(QString msg, bool is_error)
{
    ui->error_label->setText(msg);
    if(is_error){
        ui->error_label->setProperty("state", "error");
    }
    else{
        ui->error_label->setProperty("state", "normal");
    }
    repolish(ui->error_label);
}

void RegisterDialog::initHttpHandles()
{
    // 获取验证码的回调函数
    _handlers.insert(RequireId::ID_GET_VARIFY_CODE, [this](const QJsonObject& json){
        int error = json["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"), true);
            return;
        }
        QString email = json["email"].toString();
        showTip(tr("验证码已经发送到邮箱，请注意查收"), false);
        qDebug() << "email is " << email;
    });
}

