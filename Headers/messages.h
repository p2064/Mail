#ifndef MESSAGES_H
#define MESSAGES_H

#include <QListWidget>
#include <QFont>
#include <QFileDialog>
#include <QAction>
#include <QMenu>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include "database.h"
#include "login.h"
#include "parser.h"

namespace Ui { class Messages; }

class Messages : public QWidget
{
    Q_OBJECT
public:
    explicit Messages(QWidget *parent = nullptr);
    int acc_id;
    void shortDisplay();
    ~Messages();

private:
    Ui::Messages *ui;
    Database *database;
    QListWidget *listWidget;
    QString queryString;
    Parser *parser;
    void refresh();
    void updateInfo();



private slots:
    void display (QListWidgetItem *);
    void on_refreshButton_clicked();
    void on_hideButton_clicked();
    void on_choiceButton_clicked();
    void on_changePersInfo_clicked();
    void on_applyChanges_clicked();
    void on_toolButton_clicked();
    void slotBlock();
    void slotDelete();
    void changePassword();
};

#endif // MESSAGES_H
