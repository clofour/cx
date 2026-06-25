#include "keywords.h"

Keyword keywords[] = {
    { "and", TOKEN_AND },
    { "class", TOKEN_CLASS },
    { "else", TOKEN_ELSE },
    { "false", TOKEN_FALSE },
    { "for", TOKEN_FOR },
    { "function", TOKEN_FUNCTION },
    { "if", TOKEN_IF },
    { "null", TOKEN_NULL },
    { "or", TOKEN_OR },
    { "print", TOKEN_PRINT },
    { "return", TOKEN_RETURN },
    { "true", TOKEN_TRUE },
    { "var", TOKEN_VAR },
    { "while", TOKEN_WHILE },
};
int keyword_count = sizeof(keywords) / sizeof(Keyword);
