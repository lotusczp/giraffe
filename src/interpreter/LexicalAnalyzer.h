#ifndef LEXICALANALYZER_H
#define LEXICALANALYZER_H

/* ---------------------------------------------------------------------
 *
 * now the analyzer can support :
 *
 * operators:       "+", "-", "*", "/"
 *
 * number:          int, fraction
 *
 * comments:        all strings begin with # and end with \n
 *
 * rel operators:   ">", "<", "=", ">=", "<=", "==", "!="
 *
 * logic operators:
 *
 * parentheses:     "(", ")", "{", "}"
 *
 * other op:    ",", "\"", "&"
 *
 * reserved words:  "while", "if", "elif", "else", "assert", "print", "def", "and", "or", "class", "extends", "new"
 *
 * ---------------------------------------------------------------------*/

#include <QList>
#include <QString>

#include "Token.h"



class LexicalAnalyzer
{
public:
    LexicalAnalyzer(const QString& a_rstrInput);
    ~LexicalAnalyzer();

    void scan();

    bool isComplete() const;

    const QList<Token*>& getTokenList() const;

private:
    LexicalAnalyzer();

    char getChar();

    void ungetChar();

    void setComplete();

private:
    enum State {
        State_Error = -1,
        State_Start,
        State_Int,
        State_Double,
        State_Op,
        State_Name,
        State_String,
        State_Comments
    };

    QList<Token*> m_listToken;

    QString m_strInput;

    int m_iCurrentIndex;
};

#endif // LEXICALANALYZER_H
