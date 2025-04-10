#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "HttpManager.h"


RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::RegisterDialog)
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
    // 连接注册信号和处理的槽函数
    connect(HttpManager::GetInstance().get(), &HttpManager::sig_reg_mod_finish, this, &RegisterDialog::slot_reg_mod_finish);
    // 注册http处理回调函数
    initHttpHandles();

    // 检查填入数据合规性
    CheckAllEdit();


    // 注册成功提示界面
    _countdown_timer = new QTimer(this);
    connect(_countdown_timer, &QTimer::timeout, this, [this](){
        _countdown--;
        if(_countdown == 0){
            _countdown_timer->stop();
            emit sigSwitchLogin();
            return;
        }
        QString str = QString("注册成功，将在 %1s 后返回登录界面").arg(_countdown);
        ui->success_label->setText(str);
    });
}

RegisterDialog::~RegisterDialog()
{
    qDebug("RegisterDialog Destructor");
    delete ui;
}

void RegisterDialog::on_get_verifycode_btn_clicked()
{  // 发送验证码按钮
    // 检测email是否合规
    if(CheckEmail() == true){
        // 发送验证码
        QJsonObject json;
        json["email"] = ui->email_edit->text();
        HttpManager::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/get_verify_code"),
                                                json,
                                                RequireId::ID_REGISTER_GET_VERIFY_CODE,
                                                Modules::MOD_REGISTER);
        showTip("正在发送验证码...", false);
    }
}

void RegisterDialog::slot_reg_mod_finish(RequireId req_id, QString res, ErrorCodes err)
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

void RegisterDialog::showTip(const char* msg, bool is_error)
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

void RegisterDialog::showTip(const QString &msg, bool is_error)
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
    _handlers.insert(RequireId::ID_REGISTER_GET_VERIFY_CODE, [this](const QJsonObject& json){
        int status = json["status"].toInt();
        QString msg = json["msg"].toString();
        if(status != ErrorCodes::SUCCESS){
            showTip(msg, true);
            return;
        }
        showTip("验证码已经发送到邮箱，请注意查收", false);
    });

    // 用户注册的回调函数
    _handlers.insert(RequireId::ID_USER_REGESTER, [this](const QJsonObject& json){
        int status = json["status"].toInt();
        QString msg = json["msg"].toString();
        if(status != ErrorCodes::SUCCESS){
            showTip(msg, true);
            return;
        }
        showTip("用户注册成功", false);
        ChangeSuccessPage();
    });
}


void RegisterDialog::ChangeSuccessPage()
{
    ui->stackedWidget->setCurrentWidget(ui->page_2);
    _countdown = 5;
    ui->success_label->setText("注册成功，将在 5s 后返回登录界面");
    _countdown_timer->start(1000);
}


bool RegisterDialog::CheckUserName()
{
    if(ui->user_edit->text() == ""){
        showTip("用户名不能为空", true);
        return false;
    }
    return true;
}


bool RegisterDialog::CheckEmail()
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

bool RegisterDialog::CheckPassword()
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


bool RegisterDialog::CheckPassword2()
{
    if(ui->password2_edit->text() != ui->password_edit->text()){
        showTip("确认密码不一致", true);
        return false;
    }
    return true;
}


bool RegisterDialog::CheckVerifyCode()
{
    if(ui->verifycode_edit->text() == ""){
        showTip("验证码不能为空", true);
        return false;
    }
    return true;
}


bool RegisterDialog::CheckAllEdit()
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

void RegisterDialog::on_confirm_btn_clicked()
{  // 注册按钮
    if(CheckAllEdit() == false) return;
    QJsonObject json;
    json["user"] = ui->user_edit->text();
    json["email"] = ui->email_edit->text();
    json["passwd"] = md5Encrypt(ui->password_edit->text());
    json["passwd2"] = md5Encrypt(ui->password2_edit->text());
    json["verifycode"] = ui->verifycode_edit->text();
    HttpManager::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/user_register"),
                                            json,
                                            RequireId::ID_USER_REGESTER,
                                            Modules::MOD_REGISTER);
}

void RegisterDialog::on_user_edit_editingFinished()
{
    CheckAllEdit();
}


void RegisterDialog::on_email_edit_editingFinished()
{
    CheckAllEdit();
}


void RegisterDialog::on_password_edit_editingFinished()
{
    CheckAllEdit();
}


void RegisterDialog::on_password2_edit_editingFinished()
{
    CheckAllEdit();
}


void RegisterDialog::on_verifycode_edit_editingFinished()
{
    CheckAllEdit();
}


void RegisterDialog::on_password_visible_clicked()
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


void RegisterDialog::on_password2_visible_clicked()
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

// 切换回登录界面
void RegisterDialog::on_cancel_btn_clicked()
{
    emit sigSwitchLogin();
}


void RegisterDialog::on_success_btn_clicked()
{
    _countdown_timer->stop();
    emit sigSwitchLogin();
}

