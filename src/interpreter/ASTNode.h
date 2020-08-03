#ifndef ASTNODE_H
#define ASTNODE_H


#include <QString>

#include "Symbol.h"

#include <QDebug>

class ASTNode
{
public:
    ASTNode() {}
    virtual ~ASTNode() {}
    virtual Symbol eval() = 0;
};


class SymbolNode : public ASTNode
{
public:
    SymbolNode(double v) : value(v) {}
    SymbolNode(QString s) : value(s) {}
    virtual Symbol eval() {return value;}

private:
    SymbolNode() {}

    Symbol value;
};

class VariableNode : public ASTNode
{
public:
    VariableNode(QString a_name) : name(a_name) {}
    virtual Symbol eval();
    void setValue(Symbol value);

private:
    VariableNode() {}

    QString name;
};

class IdleNode : public ASTNode
{
public:
    IdleNode() {}
    virtual Symbol eval() {return Symbol(0);}
};

class SequenceNode : public ASTNode
{
public:
    SequenceNode(ASTNode *l, ASTNode *r) : left(l), right(r) {}
    ~SequenceNode() {if(left){delete left, left=NULL;} if(right) {delete right, right=NULL;} }
    virtual Symbol eval()
    {
        left->eval();
        Symbol retval = right->eval();
        return retval;
    }

private:
    SequenceNode() {}
    ASTNode* left;
    ASTNode* right;
};

class BinaryMathOpNode : public ASTNode
{
public:
    BinaryMathOpNode(QString o, ASTNode *l, ASTNode *r) : op(o), left(l), right(r) {}
    ~BinaryMathOpNode() {if(left){delete left, left=NULL;} if(right) {delete right, right=NULL;} }
    virtual Symbol eval()
    {
        if (op=="+") {
            return left->eval() + right->eval();
        }
        else if (op=="-") {
            return left->eval() - right->eval();
        }
        else if (op=="*") {
            return left->eval() * right->eval();
        }
        else if (op=="/") {
            return left->eval() / right->eval();
        }
        else {
            return Symbol(0);
        }
    }

private:
    BinaryMathOpNode() {}

    QString op;
    ASTNode* left;
    ASTNode* right;
};

class LogicalNode : public ASTNode
{
public:
    LogicalNode(QString o, ASTNode *l, ASTNode *r) : op(o), left(l), right(r) {}
    ~LogicalNode() {if(left){delete left, left=NULL;} if(right) {delete right, right=NULL;} }
    virtual Symbol eval()
    {
        if (op=="and") {
            if ((left->eval()!=Symbol(0)) && (right->eval()!=Symbol(0)))
                return Symbol(1);
            else
                return Symbol(0);
        }
        else if (op=="or") {
            if ((left->eval()!=Symbol(0)) || (right->eval()!=Symbol(0)))
                return Symbol(1);
            else
                return Symbol(0);
        }
        else {
            return Symbol(0);
        }
    }

private:
    LogicalNode() {}

    QString op;
    ASTNode* left;
    ASTNode* right;
};

class IfNode : public ASTNode
{
public:
    IfNode(ASTNode *c, ASTNode* s, ASTNode *u) : condition(c), satisfied(s), unsatisfied(u) {}
    ~IfNode() {if(condition){delete condition, condition=NULL;} if(satisfied) {delete satisfied, satisfied=NULL;} if(unsatisfied) {delete unsatisfied, unsatisfied=NULL;} }
    virtual Symbol eval()
    {
        if (condition->eval() == Symbol(1)) {
            satisfied->eval();
        }
        else {
            unsatisfied->eval();
        }

        return Symbol(1);
    }

private:
    IfNode() {}

    ASTNode* condition;
    ASTNode* satisfied;
    ASTNode* unsatisfied;
};

class WhileNode : public ASTNode
{
public:
    WhileNode(ASTNode *l, ASTNode *r) : condition(l), satisfied(r) {}
    ~WhileNode() {if(condition){delete condition, condition=NULL;} if(satisfied) {delete satisfied, satisfied=NULL;} }
    virtual Symbol eval()
    {
        while (condition->eval() == Symbol(1)) {
            satisfied->eval();
        }

        return Symbol(1);
    }

private:
    WhileNode() {}

    ASTNode* condition;
    ASTNode* satisfied;
};

class RelationalNode : public ASTNode
{
public:
    RelationalNode(QString o, ASTNode *l, ASTNode *r) : op(o), left(l), right(r) {}
    ~RelationalNode() {if(left){delete left, left=NULL;} if(right) {delete right, right=NULL;} }
    virtual Symbol eval()
    {
        if (op==">") {
            return (left->eval() > right->eval()) ? Symbol(1) : Symbol(0);
        }
        else if (op=="<") {
            return (left->eval() < right->eval()) ? Symbol(1) : Symbol(0);
        }
        else if (op==">=") {
            return (left->eval() >= right->eval()) ? Symbol(1) : Symbol(0);
        }
        else if (op=="<=") {
            return (left->eval() <= right->eval()) ? Symbol(1) : Symbol(0);
        }
        else if (op=="==") {
            return (left->eval() == right->eval()) ? Symbol(1) : Symbol(0);
        }
        else if (op=="!=") {
            return (left->eval() != right->eval()) ? Symbol(1) : Symbol(0);
        }
        else {
            return Symbol(0);
        }
    }

private:
    RelationalNode() {}

    QString op;
    ASTNode* left;
    ASTNode* right;
};

class AssignNode : public ASTNode
{
public:
    AssignNode(VariableNode* l, ASTNode *r) : left(l), right(r) {}
    ~AssignNode() {if(left){delete left, left=NULL;} if(right) {delete right, right=NULL;} }
    virtual Symbol eval()
    {
        left->setValue(right->eval());
        return Symbol(1);
    }

private:
    AssignNode() {}

    VariableNode* left;
    ASTNode* right;
};

class AssertNode : public ASTNode
{
public:
    AssertNode(ASTNode *c) : condition(c) {}
    ~AssertNode() { if(condition){delete condition, condition=NULL;} }
    virtual Symbol eval()
    {
        if (condition->eval() == Symbol(0)) {
            QString strMsg = "Assert in line ...";
            throw (strMsg.toStdString());
        }
        return Symbol(1);
    }

private:
    AssertNode() {}

    ASTNode* condition;
};

class PrintNode : public ASTNode
{
public:
    PrintNode(ASTNode *e) : exp(e) {}
    ~PrintNode() { if(exp){delete exp, exp=NULL;} }
    virtual Symbol eval();

private:
    PrintNode() {}

    ASTNode* exp;
};


/* --------------------------------------------------------------------- */
/* -------------------------- function related ------------------------- */
/* --------------------------------------------------------------------- */
class InstanceCallFuncNode;
class FuncCallNode : public ASTNode
{
public:
    friend class InstanceCallFuncNode;
    FuncCallNode(QString str, QList<ASTNode*> paras) : name(str) {arguments=paras;}
    ~FuncCallNode();
    virtual Symbol eval();
    Symbol eval(QList<Symbol> values);

private:
    FuncCallNode() {}
    QString name;
    QList<ASTNode*> arguments;
};

class FuncDefNode : public ASTNode
{
public:
    friend class FuncCallNode;
    FuncDefNode(QString str, ASTNode* b, QList<VariableNode*> paras) : name(str) {body=b; parameters=paras;}
    ~FuncDefNode();
    virtual Symbol eval();

private:
    FuncDefNode() {}
    QString name;
    ASTNode* body;
    QList<VariableNode*> parameters;
};


/* ----------------------------------------------------------------- */
/* -------------------------- class related------------------------- */
/* ----------------------------------------------------------------- */
class NewInstanceNode : public ASTNode
{
public:
    NewInstanceNode(QString instance, QString strClassName) {instancename = instance; classname = strClassName;}
    ~NewInstanceNode() {}
    virtual Symbol eval();
private:
    NewInstanceNode() {}
    QString instancename;
    QString classname;
};

class InstanceCallFuncNode : public ASTNode
{
public:
    InstanceCallFuncNode(QString instance, FuncCallNode* f) {instancename = instance; funcCall = f;}
    ~InstanceCallFuncNode();
    virtual Symbol eval();
private:
    InstanceCallFuncNode() {}
    QString instancename;
    FuncCallNode* funcCall;
};

class ClassDefNode : public ASTNode
{
public:
    ClassDefNode(QString name, QString parent, QList<ASTNode*> m, QList<FuncDefNode*> f) {classname = name; parentname = parent; members = m; funcs = f;}
    ~ClassDefNode();
    virtual Symbol eval();
    void declareAllMembers()
    {
        int iMemberNum = members.size();
        for (int i=0;i<iMemberNum;i++) {
            if (members[i]) members[i]->eval();
        }
    }
    void declareAllFuncs()
    {
        int iFuncNum = funcs.size();
        for (int i=0;i<iFuncNum;i++) {
            if (funcs[i]) funcs[i]->eval();
        }
    }
    bool hasParent() {return parentname!="";}
    QString getParentName() const {return parentname;}

private:
    ClassDefNode() {}
    QString classname;
    QString parentname;
    QList<ASTNode*> members;
    QList<FuncDefNode*> funcs;
};

void enterScope();

void deleteScope();

void returnPreScope();

#endif // ASTNODE_H
