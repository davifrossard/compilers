
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
    ASSIGN,
    EQ,
    GT,
    LT,
    GE,
    LE,
    NUMBER,
    ID,
    ENTER,
    UNKNOWN,
    ENDFILE,
	LPAREN,
	RPAREN,
	NEWLINE
} TokenType;


#endif // TOKEN_H
