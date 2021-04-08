#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include <QWidget>
#include <QBoxLayout>
#include <QLabel>
#include <QValidator>
#include <QMessageBox>
#include "database.h"
#include "registration.h"
#include "admin.h"
#include "messages.h"

class Registration;
class Admin;
class Messages;

namespace Ui { class Login; }

class Login : public QMainWindow
{
    Q_OBJECT
public:
    Login(QWidget *parent = nullptr);
    ~Login();

private:
    Ui::Login *ui;
    Database *database;
    Registration *registration;
    Admin *admin;
    Messages *messages;


private slots:
    void on_signIn_clicked();
    void on_signUp_clicked();
    void on_clear_clicked();
    void on_showPassword_clicked();

signals:
    void next();
};
#endif // LOGIN_H
