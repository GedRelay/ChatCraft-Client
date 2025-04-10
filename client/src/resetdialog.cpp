#include "resetdialog.h"
#include "ui_resetdialog.h"
#include "HttpManager.h"

ResetDialog::ResetDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ResetDialog)
{
    ui->setupUi(this);
    // 设置错误提示label的样式
    ui->error_label->setProperty("state","normal");
    repolish(ui->error_label);
    // 设置密码可见性的样式
    ui->password_edit->setEchoMode(QLineEdit::Password);
    ui->password2_edit->setEchoMode(QLineEdit::Password);
    ui->password_visible->setProperty("state", "unvisible");
    repolish(ui->password_visible);
    ui->password2_visible->setProperty("state", "unvisible");
    repolish(ui->password2_visible);

    // 检查填入数据合规性
    CheckAllEdit();

    // 连接注册信号和处理的槽函数
    connect(HttpManager::GetInstance().get(), &HttpManager::sig_reset_mod_finish, this, &ResetDialog::slot_reset_mod_finish);

    // 注册http处理回调函数
    initHttpHandles();
}

ResetDialog::~ResetDialog()
{
    qDebug("ResetDialog Destructor");
    delete ui;
}


void ResetDialog::slot_reset_mod_finish(RequireId req_id, QString res, ErrorCodes err)
{
    if(err != ErrorCodes::SUCCESS){
        showTip("网络请求错误", true);
        return;
    }

    // 解析json字符串
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    if(jsonDoc.isNull()){
        showTip("json解析失败", true);
        return;
    }
    if(!jsonDoc.isObject()){
        showTip("json解析失败", true);
        return;
    }

    _handlers[req_id](jsonDoc.object());
}


void ResetDialog::initHttpHandles()
{
    // 获取验证码的回调函数
    _handlers.insert(RequireId::ID_RESET_GET_VERIFY_CODE, [this](const QJsonObject& json){
        int status = json["status"].toInt();
        QString msg = json["msg"].toString();
        if(status != ErrorCodes::SUCCESS){
            showTip(msg, true);
            return;
        }
        showTip("验证码已经发送到邮箱，请注意查收", false);
    });

    // 重置密码的回调函数
    _handlers.insert(RequireId::ID_RESET, [this](const QJsonObject& json){
        int status = json["status"].toInt();
        QString msg = json["msg"].toString();
        if(status != ErrorCodes::SUCCESS){
            showTip(msg, true);
            return;
        }
        showTip("密码修改成功!", false);
    });
}


void ResetDialog::on_cancel_btn_clicked()
{
    emit sigSwitchLogin();
}

void ResetDialog::showTip(const char* msg, bool is_error)
{
    ui->error_label->setText(tr(msg));
    if(is_error){
        ui->error_label->setProperty("state", "error");
    }
    else{
        ui->error_label->setProperty("state", "normal");
    }
    repolish(ui->error_label);
}

void ResetDialog::showTip(const QString &msg, bool is_error)
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


bool ResetDialog::CheckUserName()
{
    if(ui->user_edit->text() == ""){
        showTip("用户名不能为空", true);
        return false;
    }
    return true;
}


bool ResetDialog::CheckEmail()
{
    QString email = ui->email_edit->text();
    if(email == ""){
        showTip("邮箱不能为空", true);
        return false;
    }
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch();
    if(match == false){
        showTip("邮箱地址不正确", true);
        return false;
    }
    return true;
}

bool ResetDialog::CheckPassword()
{
    QString password = ui->password_edit->text();
    if(password == ""){
        showTip("密码不能为空", true);
        return false;
    }
    QRegularExpression regex(R"(^(?=.*[0-9])(?=.*[a-zA-Z])[A-Za-z0-9@#$%^&\-+=()]{8,20}$)");
    bool match = regex.match(password).hasMatch();
    if(match == false){
        showTip("密码不符合规则", true);
        return false;
    }
    return true;
}


bool ResetDialog::CheckPassword2()
{
    if(ui->password2_edit->text() != ui->password_edit->text()){
        showTip("确认密码不一致", true);
        return false;
    }
    return true;
}


bool ResetDialog::CheckVerifyCode()
{
    if(ui->verifycode_edit->text() == ""){
        showTip("验证码不能为空", true);
        return false;
    }
    return true;
}


bool ResetDialog::CheckAllEdit()
{
    this->ui->get_verifycode_btn->setEnabled(false);
    if(CheckUserName() == false) return false;
    if(CheckEmail() == false) return false;
    if(CheckPassword() == false) return false;
    if(CheckPassword2() == false) return false;
    this->ui->get_verifycode_btn->setEnabled(true);
    if(CheckVerifyCode() == false) return false;
    return true;
}

void ResetDialog::on_user_edit_editingFinished()
{
    CheckAllEdit();
}


void ResetDialog::on_email_edit_editingFinished()
{
    CheckAllEdit();
}


void ResetDialog::on_password_edit_editingFinished()
{
    CheckAllEdit();
}


void ResetDialog::on_password2_edit_editingFinished()
{
    CheckAllEdit();
}


void ResetDialog::on_verifycode_edit_editingFinished()
{
    CheckAllEdit();
}

void ResetDialog::on_password_visible_clicked()
{
    QString state = ui->password_visible->property("state").toString();
    if(state == "visible"){
        ui->password_visible->setProperty("state", "unvisible");
        repolish(ui->password_visible);
        ui->password_edit->setEchoMode(QLineEdit::Password);
    }
    else {
        ui->password_visible->setProperty("state", "visible");
        repolish(ui->password_visible);
        ui->password_edit->setEchoMode(QLineEdit::Normal);
    }
}


void ResetDialog::on_password2_visible_clicked()
{
    QString state = ui->password2_visible->property("state").toString();
    if(state == "visible"){
        ui->password2_visible->setProperty("state", "unvisible");
        repolish(ui->password2_visible);
        ui->password2_edit->setEchoMode(QLineEdit::Password);
    }
    else {
        ui->password2_visible->setProperty("state", "visible");
        repolish(ui->password2_visible);
        ui->password2_edit->setEchoMode(QLineEdit::Normal);
    }
}


void ResetDialog::on_get_verifycode_btn_clicked()
{
    if(CheckEmail() == true){
        // 发送验证码
        QJsonObject json;
        json["user"] = ui->user_edit->text();
        json["email"] = ui->email_edit->text();
        HttpManager::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/reset_get_verify_code"),
                                                json,
                                                RequireId::ID_RESET_GET_VERIFY_CODE,
                                                Modules::MOD_RESET);
        showTip("正在发送验证码...", false);
    }
}


void ResetDialog::on_confirm_btn_clicked()
{  // 修改密码
    if(CheckAllEdit() == false) return;
    QJsonObject json;
    json["user"] = ui->user_edit->text();
    json["email"] = ui->email_edit->text();
    json["passwd"] = md5Encrypt(ui->password_edit->text());
    json["passwd2"] = md5Encrypt(ui->password2_edit->text());
    json["verifycode"] = ui->verifycode_edit->text();
    HttpManager::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/reset_password"),
                                            json,
                                            RequireId::ID_RESET,
                                            Modules::MOD_RESET);
}

