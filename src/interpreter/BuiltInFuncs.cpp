#include "BuiltInFuncs.h"

#include <QDateTime>
#include <QTimer>
#include <QEventLoop>

class SinBody : public ASTNode
{
public:
    friend class BuiltInFuncs;

    SinBody() {}
    ~SinBody()
    {
        /// \note do not delete para, it will be deleted in the destruction of FuncDefNode
    }
    virtual Symbol eval()
    {
        Symbol value = para->eval();
        if (value.type() == Symbol::NumberType) {
            return Symbol(sin(para->eval().getNumber()));
        }
        else {
            QString strErrorMsg = "Type Erroe: function \"sin\" must take a number as argument.";
            throw (strErrorMsg.toStdString());
        }
    }

private:
    VariableNode* para;
};

class CosBody : public ASTNode
{
public:
    friend class BuiltInFuncs;

    CosBody() {}
    ~CosBody()
    {
        /// \note do not delete para, it will be deleted in the destruction of FuncDefNode
    }
    virtual Symbol eval()
    {
        Symbol value = para->eval();
        if (value.type() == Symbol::NumberType) {
            return Symbol(cos(para->eval().getNumber()));
        }
        else {
            QString strErrorMsg = "Type Erroe: function \"cos\" must take a number as argument.";
            throw (strErrorMsg.toStdString());
        }
    }

private:
    VariableNode* para;
};

class currentTimeBody : public ASTNode
{
public:
    currentTimeBody() {}
    ~currentTimeBody()
    {
        /// \note do not delete parameters, it will be deleted in the destruction of FuncDefNode
    }
    virtual Symbol eval()
    {
        return Symbol(QDateTime::currentMSecsSinceEpoch());
    }
};

class SqrtBody : public ASTNode
{
public:
    friend class BuiltInFuncs;

    SqrtBody() {}
    ~SqrtBody()
    {
        /// \note do not delete parameters, it will be deleted in the destruction of FuncDefNode
    }
    virtual Symbol eval()
    {
        Symbol value = para->eval();
        if (value.type() == Symbol::NumberType) {
            return Symbol(sqrt(para->eval().getNumber()));
        }
        else {
            QString strErrorMsg = "Type Erroe: function \"sqrt\" must take a number as argument.";
            throw (strErrorMsg.toStdString());
        }
    }
private:
    VariableNode* para;
};

class WaitBody : public ASTNode
{
public:
    friend class BuiltInFuncs;

    WaitBody() {}
    ~WaitBody()
    {
        /// \note do not delete parameters, it will be deleted in the destruction of FuncDefNode
    }
    virtual Symbol eval()
    {
        Symbol value = para->eval();
        if (value.type() == Symbol::NumberType) {
            int ms = (int)value.getNumber();
#if 0
            QElapsedTimer t;
            t.start();
            while(t.elapsed() < ms) {
                QCoreApplication::processEvents();
            }
#endif
            QEventLoop eventloop;
            QTimer::singleShot(ms, &eventloop, SLOT(quit()));
            eventloop.exec();
        }
        else {
            QString strErrorMsg = "Type Erroe: function \"wait\" must take a number as argument.";
            throw (strErrorMsg.toStdString());
        }
        return Symbol(1);
    }
private:
    VariableNode* para;
};

BuiltInFuncs::BuiltInFuncs()
{
    // create sin(long double) function
    QString strFunc_sin = "sin";
    SinBody* body_sin = new SinBody();
    body_sin->para = new VariableNode("para1");
    QList<VariableNode*> parameters_sin;
    parameters_sin.append(body_sin->para);
    FuncDefNode* pFunc_sin = new FuncDefNode(strFunc_sin, body_sin, parameters_sin);
    m_functions[strFunc_sin] = pFunc_sin;

    // create currentTime() function
    QString strFunc_currentTime = "currentTime";
    currentTimeBody* body_currentTime = new currentTimeBody();
    QList<VariableNode*> parameters_currentTime;
    FuncDefNode* pFunc_currentTime = new FuncDefNode(strFunc_currentTime, body_currentTime, parameters_currentTime);
    m_functions[strFunc_currentTime] = pFunc_currentTime;

    // create sqrt(long double) function
    QString strFunc_sqrt = "sqrt";
    SqrtBody* body_sqrt = new SqrtBody();
    body_sqrt->para = new VariableNode("para1");
    QList<VariableNode*> parameters_sqrt;
    parameters_sqrt.append(body_sqrt->para);
    FuncDefNode* pFunc_sqrt = new FuncDefNode(strFunc_sqrt, body_sqrt, parameters_sqrt);
    m_functions[strFunc_sqrt] = pFunc_sqrt;

    // create sqrt(long double) function
    QString strFunc_wait = "wait";
    WaitBody* body_wait = new WaitBody();
    body_wait->para = new VariableNode("para1");
    QList<VariableNode*> parameters_wait;
    parameters_wait.append(body_wait->para);
    FuncDefNode* pFunc_wait = new FuncDefNode(strFunc_wait, body_wait, parameters_wait);
    m_functions[strFunc_wait] = pFunc_wait;
}

BuiltInFuncs::~BuiltInFuncs()
{
}

const QHash<QString, FuncDefNode *> &BuiltInFuncs::getBuiltInFuncs() const
{
    return m_functions;
}
