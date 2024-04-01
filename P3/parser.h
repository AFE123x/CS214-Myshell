#ifndef PARSER_H
#define PARSER_H
#include<stdlib.h>
struct data* capygetline(int fd);
struct data{
    char** myarray;
    int size;
};
#endif
