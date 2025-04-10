#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "logindialog.h"
#include "registerdialog.h"
#include "resetdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void SlotSwitchRegister();
    void SlotSwitchReset();
    void SlotSwitchLogin();

private:
    Ui::MainWindow *ui;
    LoginDialog *_login_dlg = nullptr;
    RegisterDialog *_register_dlg = nullptr;
    ResetDialog *_reset_dlg = nullptr;
};
#endif // MAINWINDOW_H
