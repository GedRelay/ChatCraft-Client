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
    _login_dlg->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SlotSwitchRegister()
{
    if(_login_dlg){
        delete _login_dlg;
        _login_dlg = nullptr;
    }
    _register_dlg = new RegisterDialog();
    takeCentralWidget();
    setCentralWidget(_register_dlg);
    // 连接注册界面切换登录界面信号
    connect(_register_dlg, &RegisterDialog::sigSwitchLogin, this, &MainWindow::SlotSwitchLogin);
    _register_dlg->show();
}

void MainWindow::SlotSwitchLogin()
{
    if(_register_dlg){
        delete _register_dlg;
        _register_dlg = nullptr;
    }
    _login_dlg = new LoginDialog();
    takeCentralWidget();
    setCentralWidget(_login_dlg);
    // 连接登录界面切换注册界面信号
    connect(_login_dlg, &LoginDialog::sigSwitchRegister, this, &MainWindow::SlotSwitchRegister);
    _login_dlg->show();
}
