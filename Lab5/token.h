
#ifndef TOKEN_H
#define TOKEN_H

typedef enum {
    IF,
    THEN,
    ELSE,
    END,
    REPEAT,
    UNTIL,
    READ,
    WRITE,
    PLUS,
    MINUS,
    TIMES,
    DIVISION,
    NUMBER,
    ID,
    ENTER,
    UNKNOWN,
    ENDOF,
	IGNORE
} TokenType;


#endif // TOKEN_H
