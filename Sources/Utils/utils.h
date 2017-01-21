/*
 * utils.h
 *
 *  Created on: Dec 26, 2015
 *      Author: Karl
 */

#define EOS   '\0'
#define EOIL   -1
#define MAXFIND 5    // 10
#define MAXLENFIND 20 // 28
#define EOL   '\n'

int find(char *text, char *word, int *rtas);
unsigned int strLen(char *value);
int strsub(char *text, int init, int end, char *rta);
void setLCD(char *data);






