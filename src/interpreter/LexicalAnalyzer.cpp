#include "LexicalAnalyzer.h"

#include <QDebug>

#include "ErrorHandler.h"

#define NUM_RESERVED_WORDS 12

const QString c_reservedWords[NUM_RESERVED_WORDS] = {"while", "if", "elif", "else", "assert", "print", "def", "and", "or", "class", "extends", "new"};

static bool isReservedWords(const QString& a_rstrWord)
{
    bool retval = false;
    for(int i=0; i<NUM_RESERVED_WORDS; i++) {
        if (c_reservedWords[i] == a_rstrWord) {
            retval = true;
        }
    }
    return retval;
}

static bool isDigit(char ch)
{
    if ( (ch >= '0') && (ch <= '9') )
        return true;
    else
        return false;
}

static bool isOp(char ch)
{
    return (ch=='+')
            || (ch=='-')
            || (ch=='*')
            || (ch=='/')
            || (ch=='>')
            || (ch=='<')
            || (ch=='=')
            || (ch=='(')
            || (ch==')')
            || (ch=='{')
            || (ch=='}')
            || (ch=='|')
            || (ch=='&')
            || (ch==',')
            || (ch=='!');
}

static bool isLatinChar(char ch)
{
    if ( ((ch>='a')&&(ch<='z')) || ((ch>='A')&&(ch<='Z')) )
         return true;
    else
         return false;
}


LexicalAnalyzer::LexicalAnalyzer(const QString &a_rstrInput)
{
    m_iCurrentIndex = -1;
    m_strInput = a_rstrInput;
    m_strInput.append('\n');
}

LexicalAnalyzer::~LexicalAnalyzer()
{
    if(!m_listToken.isEmpty()) {
        QListIterator<Token*> i(m_listToken);
        while(i.hasNext()) {
            Token* pToken = i.next();
            if (pToken)
                delete pToken;
        }
        m_listToken.clear();
    }
}

void LexicalAnalyzer::scan()
{
    State state = State_Start;
    QString strTmp;
    while (!isComplete()) {
        char ch = getChar();
        switch (state)
        {
        case State_Start:
            if (isDigit(ch)) {
                state = State_Int;
                ungetChar();
            }
            else if (isOp(ch)) {
                state = State_Op;
                ungetChar();
            }
            else if (ch == '.') {
                state = State_Op;
                ungetChar();
            }
            else if (ch == '#') {
                state = State_Comments;
            }
            else if ( (isLatinChar(ch)) || (ch=='_') || (ch=='@') ) {
                state = State_Name;
                ungetChar();
            }
            else if(ch == '\"') {
                state = State_String;
            }
            break;
        case State_Int:
            do {
                strTmp.append(ch);
                ch = getChar();
            } while (isDigit(ch));
            if (ch == '.') {
                state = State_Double;
                ungetChar();
            }
            else {
                state = State_Start;
                ungetChar();
                m_listToken.append(new Token(strTmp, Token::Number));
                strTmp.clear();
            }
            break;
        case State_Double:
            do {
                strTmp.append(ch);
                ch = getChar();
            } while (isDigit(ch));
            if (ch == '.') {
                state = State_Error;
                ErrorHandler::instance()->setError("Token Error: unexpected \"" + strTmp + "." + "\"");
                ungetChar();
            }
            else {
                state = State_Start;
                ungetChar();
                m_listToken.append(new Token(strTmp, Token::Number));
                strTmp.clear();
            }
            break;
        case State_Op:
            if ( (ch=='+') || (ch=='-') || (ch=='*') || (ch=='/') ) {
                m_listToken.append(new Token(QString(QChar(ch)), Token::Operators));
            }
            else if ((ch=='>') || (ch=='<') || (ch=='=')) {
                char chTmp = getChar();
                if (chTmp == '=') {
                    QString str;
                    str.append(ch);
                    str.append("=");
                    m_listToken.append(new Token(str, Token::Operators));
                }
                else {
                    ungetChar();
                    m_listToken.append(new Token(QString(QChar(ch)), Token::Operators));
                }
            }
            else if (ch=='!') {
                char chTmp = getChar();
                if (chTmp == '=') {
                    m_listToken.append(new Token("!=", Token::Operators));
                }
                else {
                    ungetChar();
                    ErrorHandler::instance()->setError("Token Error: missing \"=\" after \"!\"");
                }
            }
            /*
            else if((ch=='|') || (ch=='&')) {
                char chTmp = getChar();
                if (chTmp == ch) {
                    QString str;
                    str.append(ch);
                    str.append(ch);
                    m_listToken.append(new Token(str, Token::Operators));
                }
                else {
                    ungetChar();
                    ErrorHandler::instance()->setError("Token Error: unexpected \"" + QString(QChar(ch)) + "\", only support || or &&");
                }
            }*/
            else {
                m_listToken.append(new Token(QString(QChar(ch)), Token::Operators));
            }
            state = State_Start;
            break;
        case State_Name:
            do {
                strTmp.append(ch);
                ch = getChar();
            } while ( (isLatinChar(ch)) || (ch=='_') || (isDigit(ch)) || (ch=='@'));
            ungetChar();
            if (isReservedWords(strTmp)) {
                m_listToken.append(new Token(strTmp, Token::ReservedWords));
            }
            else {
                m_listToken.append(new Token(strTmp, Token::Name));
            }
            strTmp.clear();

            state = State_Start;
            break;
        case State_String:
            while (1) {
                if (ch == '\\') {
                    strTmp.append(ch);
                    ch = getChar();
                }
                else if(ch == '\"') {
                    break;
                }
                if (isComplete()) {
                    ErrorHandler::instance()->setError("Token Error: missing \" ");
                    break;
                }
                strTmp.append(ch);
                ch = getChar();
            }
            m_listToken.append(new Token(strTmp, Token::StringType));
            strTmp.clear();
            state = State_Start;
            break;
        case State_Comments:
            while (ch !='\n') {
                ch = getChar();
            }
            state = State_Start;
            break;
        case State_Error:
            setComplete();
            break;
        default:
            break;
        }
    }


    // debug
#if 1
    QListIterator<Token*> i(m_listToken);
    while(i.hasNext()) {
        Token* pToken = i.next();
        if (pToken) {
            qDebug() << pToken->content;
        }
    }
#endif

#if 0
    QListIterator<Token*> i(m_listToken);
    while(i.hasNext()) {
        Token* pToken = i.next();
        if (pToken) {
            qDebug() << pToken->content;


            if (pToken->getTokenType() == Token::NumberType) {
                NumberToken* pNumberToken =dynamic_cast<NumberToken*>(pToken);
                if (pNumberToken) {
                    qDebug() << QString::number(pNumberToken->value, 'g', 16);
                }
            }
            else if (pToken->getTokenType() == Token::OperatorType) {
                OperatorToken* pOperatorToken = dynamic_cast<OperatorToken*>(pToken);
                if (pOperatorToken)
                    qDebug() << pOperatorToken->op;
            }
        }
    }
#endif
}

bool LexicalAnalyzer::isComplete() const
{
    return (m_iCurrentIndex == (m_strInput.length() - 1));
}

const QList<Token *> &LexicalAnalyzer::getTokenList() const
{
    return m_listToken;
}

LexicalAnalyzer::LexicalAnalyzer()
{
    m_iCurrentIndex = -1;
}

char LexicalAnalyzer::getChar()
{
    if (m_iCurrentIndex < m_strInput.length() - 1 )
        m_iCurrentIndex++;
    return m_strInput[m_iCurrentIndex].toLatin1();
}

void LexicalAnalyzer::ungetChar()
{
    if (m_iCurrentIndex > -1)
        m_iCurrentIndex--;
}

void LexicalAnalyzer::setComplete()
{
    m_iCurrentIndex = (m_strInput.length() - 1);
}
