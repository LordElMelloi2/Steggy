#ifndef stegy_token_h
#define stegy_token_h

typedef enum {
    TOKEN_FLAG,
    TOKEN_STRING,
} TokenType;

typedef struct {
    char* value;
    TokenType type;
} Token;

#endif // stegy_token_h
