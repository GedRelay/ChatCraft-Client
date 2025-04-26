#include "logindialog.h"
#include "ui_logindialog.h"
#include "TCPManager.h"
#include <QTimer>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    // 密码可见性
    ui->password_edit->setEchoMode(QLineEdit::Password);
    ui->password_visible->setProperty("state", "unvisible");

    // 信息提示label
    QSizePolicy sp = ui->hint_label->sizePolicy();
    sp.setRetainSizeWhenHidden(true);
    ui->hint_label->setSizePolicy(sp);
    ui->hint_label->hide();
    _countdown_timer = new QTimer(this);
    connect(_countdown_timer, &QTimer::timeout, this, [this](){
        _countdown--;
        if(_countdown == 0){
            _countdown_timer->stop();
            ui->hint_label->hide();
            return;
        }
    });

    // 连接登录事件函数和槽函数
    connect(HttpManager::GetInstance().get(), &HttpManager::sig_login_mod_finish, this, &LoginDialog::slot_login_mod_finish);

    // 连接tcp请求的信号和槽函数
    connect(this, &LoginDialog::sig_connect_tcp, TCPManager::GetInstance().get(), &TCPManager::slot_connect_tcp);

    // 连接tcp连接成功的信号和槽函数
    connect(TCPManager::GetInstance().get(), &TCPManager::sig_connect_success, this, &LoginDialog::slot_tcp_connect_finish);

    // 连接登录失败信号和槽函数
    connect(TCPManager::GetInstance().get(), &TCPManager::sig_login_failed, this, &LoginDialog::slot_login_failed);
}

LoginDialog::~LoginDialog()
{
    qDebug("LoginDialog Destructor");
    delete ui;
}


void LoginDialog::slot_login_mod_finish(RequireId req_id, QString res, ErrorCodes err)
{
    if(err != ErrorCodes::SUCCESS){
        showHint("网络请求错误,请稍后再试", true);
        UnlockOperation();
        return;
    }

    // 解析json
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    // 解析错误
    if(jsonDoc.isNull() || !jsonDoc.isObject()){
        showHint("Json解析失败", true);
        UnlockOperation();
        return;
    }

    if(req_id == RequireId::ID_USER_LOGIN){
        QJsonObject json = jsonDoc.object();
        int status = json["status"].toInt();
        QString msg = json["msg"].toString();
        if(status != ErrorCodes::SUCCESS){
            showHint(msg, true, 0);
            UnlockOperation();
            return;
        }
        qDebug() << "uid: " << json["uid"].toString();
        qDebug() << "name: " << json["name"].toString();
        qDebug() << "host: " << json["host"].toString();
        qDebug() << "port: " << json["port"].toString();
        qDebug() << "token: " << json["token"].toString();

        ServerInfo si;
        si.uid = json["uid"].toString().toInt();
        si.host = json["host"].toString();
        si.port = json["port"].toString();
        si.token = json["token"].toString();

        _uid = si.uid;
        _token = si.token;

        emit sig_connect_tcp(si);
    }
}


void LoginDialog::slot_tcp_connect_finish(bool success)
{
    if(success == false){
        showHint("网络异常", true, 5);
        UnlockOperation();
    }
    else{
        showHint("服务器连接成功，正在登录...", false, 0);
        QJsonObject json;
        json["uid"] = _uid;
        json["token"] = _token;
        qDebug() << "发送登录信息：" << json["uid"] << " " << json["token"];
        QJsonDocument doc(json);
        QString message = doc.toJson(QJsonDocument::Indented);
        emit TCPManager::GetInstance()->send_msg(RequireId::ID_CHAT_LOGIN, message);
    }
}

void LoginDialog::slot_login_failed(int err)
{
    if(err == ErrorCodes::ERR_JSON){
        showHint("登录失败: json解析失败", true, 5);
    }
    else{
        showHint("登录失败: 未知错误", true, 5);
    }
    UnlockOperation();
}


void LoginDialog::LockOperation()
{
    ui->login_btn->setEnabled(false);
    ui->register_btn->setEnabled(false);
    ui->forget_btn->setEnabled(false);
    ui->email_edit->setEnabled(false);
    ui->password_edit->setEnabled(false);
}


void LoginDialog::UnlockOperation()
{
    ui->login_btn->setEnabled(true);
    ui->register_btn->setEnabled(true);
    ui->forget_btn->setEnabled(true);
    ui->email_edit->setEnabled(true);
    ui->password_edit->setEnabled(true);
}


void LoginDialog::on_register_btn_clicked()
{// 注册
    emit sigSwitchRegister();
}


void LoginDialog::on_forget_btn_clicked()
{// 忘记密码
    emit sigSwitchReset();
}


void LoginDialog::on_login_btn_clicked()
{// 登录
    if(CheckEmail() == false) return;
    if(CheckPassword() == false) return;

    LockOperation();
    showHint("登录中...", false, 0);
    QJsonObject json;
    json["email"] = ui->email_edit->text();
    json["password"] = md5Encrypt(ui->password_edit->text());
    HttpManager::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/user_login"),
                                            json,
                                            RequireId::ID_USER_LOGIN,
                                            Modules::MOD_LOGIN);
}


void LoginDialog::showHint(const char* msg ,bool is_error, int time)
{
    showHint(tr(msg), is_error, time);
}

void LoginDialog::showHint(QString msg, bool is_error, int time)
{
    ui->hint_label->setText(msg);
    if(is_error){
        ui->hint_label->setStyleSheet("color:red");
    }
    else{
        ui->hint_label->setStyleSheet("color:green");
    }
    if(ui->hint_label->isHidden()){
        ui->hint_label->show();
    }
    if(time > 0){
        _countdown = time;
        _countdown_timer->start(1000);
    }
    repolish(ui->hint_label);
}


bool LoginDialog::CheckEmail()
{
    QString email = ui->email_edit->text();
    if(email == ""){
        showHint("请输入邮箱", true);
        return false;
    }
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch();
    if(match == false){
        showHint("邮箱地址不正确", true);
        return false;
    }
    return true;
}

bool LoginDialog::CheckPassword()
{
    QString password = ui->password_edit->text();
    if(password == ""){
        showHint("请输入密码", true);
        return false;
    }
    QRegularExpression regex(R"(^(?=.*[0-9])(?=.*[a-zA-Z])[A-Za-z0-9@#$%^&\-+=()]{8,20}$)");
    bool match = regex.match(password).hasMatch();
    if(match == false){
        showHint("密码不符合规则", true);
        return false;
    }
    return true;
}

void LoginDialog::on_password_visible_clicked()
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

