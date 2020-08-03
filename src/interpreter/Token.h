#ifndef TOKEN_H
#define TOKEN_H

#include <QString>

class Token
{
public:

    enum TokenType {
        End = -1,
        Number,
        Operators,
        Name,
        ReservedWords,
        StringType
    };

    QString     content;
    TokenType   type;
    int         line;
    int         column;

    Token(QString a_strContent, TokenType a_type) {content = a_strContent;type = a_type;line=0;column=0;}
    virtual ~Token() {}

private:
    Token() {}
};


#endif // TOKEN_H
