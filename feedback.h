#ifndef FEEDBACK_H
#define FEEDBACK_H

void error(char* message);
void error_token(Token* token, char* message);
void error_line(int line, char* message);

#endif