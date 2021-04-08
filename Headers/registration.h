#ifndef REGISTRATION_H
#define REGISTRATION_H

#include "database.h"
#include "login.h"

namespace Ui { class Registration; }

class Registration : public QWidget
{
    Q_OBJECT
public:
    explicit Registration(QWidget *parent = nullptr);
    ~Registration();

private:
    Ui::Registration *ui;
    Database *database;

private slots:
    void on_back_clicked();
    void on_signUp_clicked();
    void on_clear_clicked();
    void on_showPassword_clicked();

signals:
    void back();
};

#endif // REGISTRATION_H
