#ifndef PARSER_H
#define PARSER_H

#include <fstream>
#include <stdio.h>
#include <string>
#include "database.h"


using namespace std;
class Parser {

public:
    Parser(){};
    ~Parser(){};
    void sendInfo(int);

private:

    string to;
    string from;
    string subject;
    string text;
    string date;
    Database db;





};
#endif // PARSER_H
