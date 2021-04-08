#include "login.h"
#include "ui_login.h"

Login::Login(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    this->setWindowTitle("Вход");
    ui->textPassword->setEchoMode(QLineEdit::Password);
    this->setFixedSize(777,501);

    registration = new Registration();
    connect(registration, &Registration::back, this, &Login::show);
}

Login::~Login()
{
    delete ui;

}

void Login::on_signUp_clicked()
{
    registration->show();
    this->close();
}

void Login::on_signIn_clicked()
{
    database = new Database();
    messages = new Messages();
    QString login = ui->textLogin->text();
    QString password = ui->textPassword->text();

    if (login == "admin" && password == "admin")
    {
        admin = new Admin();
        this->close();
        admin->show();
        return;
    }

    database->query.exec("SELECT * FROM auth WHERE login = '" + login + "' AND password = '" +  password + "'");
    database->query.first();
    database->queryRecord = database->query.record();

    if (login.isEmpty() || password.isEmpty()){
        QMessageBox::critical(this, "Ошибка входа", "Пожалуйста, введите логин и пароль.", QMessageBox::Ok);
        return;
    }

    else if  (!database->query.size()){
        QMessageBox::critical(this, "Ошибка входа", "Неправильный логин или пароль.", QMessageBox::Ok);
        return;
    }

    else if (database->query.value( database->queryRecord.indexOf("isBlocked")).toString() == "1"){
        QMessageBox::critical(this, "Ошибка входа", "Ваш профиль был заблокирован, обратитесь в службу поддержки.", QMessageBox::Ok);
        return;
    }

    else if (database->query.value( database->queryRecord.indexOf("isVerified")).toString() == "0"){
        QMessageBox::critical(this, "Ошибка входа", "Ваш профиль еще не активирован, подождите, пока администратор подтвердит регистрацию.", QMessageBox::Ok);
        return;
    }

    database->queryRecord.clear();
    database->query.clear();

    database->query.exec("SELECT acc_ID FROM auth WHERE login = '"+ login + "' AND password = '" + password +"'");
    database->query.first();
    database->queryRecord = database->query.record();
    messages->acc_id = database->query.value( database->queryRecord.indexOf("acc_ID")).toInt();
    messages->shortDisplay();
    messages->show();
    this->close();
}

void Login::on_clear_clicked()
{
    ui->textLogin->clear();
    ui->textPassword->clear();
}

void Login::on_showPassword_clicked()
{
    ui->textPassword->echoMode() == QLineEdit::Normal ? ui->textPassword->setEchoMode(QLineEdit::Password)
                                                      : ui->textPassword->setEchoMode(QLineEdit::Normal);
}


