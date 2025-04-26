#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _login_dlg = new LoginDialog();
    setCentralWidget(_login_dlg);
    connect(_login_dlg, &LoginDialog::sigSwitchRegister, this, &MainWindow::SlotSwitchRegister);
    connect(_login_dlg, &LoginDialog::sigSwitchReset, this, &MainWindow::SlotSwitchReset);
    connect(TCPManager::GetInstance().get(), &TCPManager::sig_switch_chatdlg, this, &MainWindow::SlotSwitchChat);
    _login_dlg->show();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete _login_dlg;
}

void MainWindow::SlotSwitchRegister()
{
    _register_dlg = new RegisterDialog();
    takeCentralWidget();
    setCentralWidget(_register_dlg);
    // 连接注册界面切换登录界面信号
    connect(_register_dlg, &RegisterDialog::sigSwitchLogin, this, &MainWindow::SlotSwitchLogin);
    _login_dlg->hide();
    _register_dlg->show();
}

void MainWindow::SlotSwitchReset()
{
    _reset_dlg = new ResetDialog();
    takeCentralWidget();
    setCentralWidget(_reset_dlg);
    // 连接注册界面切换登录界面信号
    connect(_reset_dlg, &ResetDialog::sigSwitchLogin, this, &MainWindow::SlotSwitchLogin);
    _login_dlg->hide();
    _reset_dlg->show();
}

void MainWindow::SlotSwitchLogin()
{
    if(_register_dlg){
        delete _register_dlg;
        _register_dlg = nullptr;
    }
    if(_reset_dlg){
        delete _reset_dlg;
        _reset_dlg = nullptr;
    }
    takeCentralWidget();
    setCentralWidget(_login_dlg);
    _login_dlg->show();
}


void MainWindow::SlotSwitchChat()
{
    // _login_dlg->hide();
}
