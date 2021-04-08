#include "parser.h"
#include <vector>
#include <fstream>
#include <vector>

using namespace std;


void Parser::sendInfo(int id){


    ifstream file("/private/var/mail/pavelprodanov");

    string fromFile;
    char c;

    while (!file.eof()){
        file.get(c);
        fromFile.push_back(c);

    }

    file.close();
    system("> /private/var/mail/pavelprodanov");

    long int subjectTmp = fromFile.find("Subject: ");
    long int fromTmp = fromFile.find("From: ");
    long int toTmp = fromFile.find("To: ");
    long int boundaryTmp = fromFile.find("boundary=");

    if (subjectTmp==-1) return;
    boundaryTmp+=10;

    string boundaryValue;

    for (unsigned long int i = 0; i<=fromFile.length(); i++) {

        for (long int i=fromFile.find("\n",subjectTmp); subjectTmp<=i; subjectTmp++){

            this->subject.push_back(fromFile.at(subjectTmp));

        }
        for (long int i=fromFile.find("\n",fromTmp); fromTmp<=i; fromTmp++){

            this->from.push_back(fromFile.at(fromTmp));

        }

        for (long int i=fromFile.find("\n",toTmp); toTmp<=i; toTmp++){

            this->to.push_back(fromFile.at(toTmp));

        }

        for (long int i=fromFile.find("\n",boundaryTmp); boundaryTmp<i-1; boundaryTmp++){

            boundaryValue.push_back(fromFile.at(boundaryTmp));

        }

        long int lowerBound = fromFile.find(boundaryValue,boundaryTmp);
        lowerBound += boundaryValue.length();

        for (long int i=fromFile.find(boundaryValue,lowerBound); lowerBound<i-2; lowerBound++){

            this->text.push_back(fromFile.at(lowerBound));

        }

        this->text.erase(0,text.find("\n",1));

        QString acc_id;
        acc_id.setNum(id);
        QString result = "INSERT INTO data(acc_id,themeMsg, fromWho, textMsg) VALUES (" + acc_id   + ", '" +  QString::fromStdString(subject) + "', '" +
                QString::fromStdString(from) + "', '" +
                QString::fromStdString(text) + "')";

        db.query.exec(result);
        subject.clear();
        from.clear();
        to.clear();
        boundaryValue.clear();
        text.clear();
        db.query.clear();

        subjectTmp = fromFile.find("Subject: ",boundaryTmp) ;
        fromTmp = fromFile.find("From: ",boundaryTmp) ;
        toTmp = fromFile.find("To: ",boundaryTmp) ;
        boundaryTmp = fromFile.find("boundary=",toTmp) + 10;
        if (subjectTmp==-1) break;

    }

}
