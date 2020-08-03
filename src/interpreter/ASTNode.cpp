#include "ASTNode.h"
#include "Pipeline.h"
#include "Env.h"

Symbol VariableNode::eval()
{
    if(g_pCurrentEnv) {
        if(g_pCurrentEnv->isVariableExist(name)) {
            return g_pCurrentEnv->getValue(name);
        }
        else {
            QString strErrorMsg = "Name Error: the name \"" + name +"\" is not defined";
            throw (strErrorMsg.toStdString());
        }
    }
    else {
        QString strErrorMsg = "Bad Exception: enviroment not exist";
        throw (strErrorMsg.toStdString());
    }
}

void VariableNode::setValue(Symbol value)
{
    if(g_pCurrentEnv) {
        if (name[0] == QChar('@')) {
            // this is a class member
            Env* pSavedEnv = g_pCurrentEnv;
            while( (!g_pCurrentEnv->isTopEnv()) && (!g_pCurrentEnv->isInstanceEnv())) {
                returnPreScope();
            }
            g_pCurrentEnv->setValue(name, value);
            g_pCurrentEnv = pSavedEnv;
        }
        else {
            g_pCurrentEnv->setValue(name, value);
        }
    }
    else {
        QString strErrorMsg = "Bad Exception: enviroment not exist";
        throw (strErrorMsg.toStdString());
    }
}


Symbol PrintNode::eval()
{
    Symbol val(exp->eval());
    if (val.type() == Symbol::NumberType) {
        Pipeline::instance()->console_out_num(val.getNumber());
    }
    else if (val.type() == Symbol::StringType) {
        Pipeline::instance()->console_out_string(val.getString());
    }
    else if (val.type() == Symbol::invalid) {
        Pipeline::instance()->console_out_string("invalid");
    }
    return Symbol(1);
}


FuncCallNode::~FuncCallNode()
{
    QListIterator<ASTNode*> i(arguments);
    while (i.hasNext()) {
        ASTNode* paraNode = i.next();
        if(paraNode) delete paraNode;
    }
    arguments.clear();
}

Symbol FuncCallNode::eval()
{
    if(g_pCurrentEnv) {
        if(g_pCurrentEnv->isFuncExist(name)) {
            int iParaNum = g_pCurrentEnv->getFuncDefNode(name)->parameters.size();
            if (iParaNum == arguments.size()) {
                enterScope();
                if (iParaNum != 0) {
                    for (int i=0; i<iParaNum; i++) {
                        // assign the value to parameter
                        g_pCurrentEnv->getFuncDefNode(name)->parameters[i]->setValue(arguments[i]->eval());
                    }
                }
                Symbol retval = g_pCurrentEnv->getFuncDefNode(name)->body->eval();
                deleteScope();
                return retval;
            }
            else {
                QString strErrorMsg = "Function Error: the function \"" + name +"\" does not take " + QString::number(arguments.size()) + " arguments.";
                throw (strErrorMsg.toStdString());
            }
        }
        else {
            QString strErrorMsg = "Name Error: the function name \"" + name +"\" is not defined";
            throw (strErrorMsg.toStdString());
        }
    }
    else {
        QString strErrorMsg = "Bad Exception: enviroment not exist";
        throw (strErrorMsg.toStdString());
    }
}

Symbol FuncCallNode::eval(QList<Symbol> values)
{
    if(g_pCurrentEnv) {
        if(g_pCurrentEnv->isFuncExist(name)) {
            int iParaNum = g_pCurrentEnv->getFuncDefNode(name)->parameters.size();
            if (iParaNum == values.size()) {
                enterScope();
                if (iParaNum != 0) {
                    for (int i=0; i<iParaNum; i++) {
                        // assign the value to parameter
                        g_pCurrentEnv->getFuncDefNode(name)->parameters[i]->setValue(values[i]);
                    }
                }
                Symbol retval = g_pCurrentEnv->getFuncDefNode(name)->body->eval();
                deleteScope();
                return retval;
            }
            else {
                QString strErrorMsg = "Function Error: the function \"" + name +"\" does not take " + QString::number(values.size()) + " arguments.";
                throw (strErrorMsg.toStdString());
            }
        }
        else {
            QString strErrorMsg = "Name Error: the function name \"" + name +"\" is not defined";
            throw (strErrorMsg.toStdString());
        }
    }
    else {
        QString strErrorMsg = "Bad Exception: enviroment not exist";
        throw (strErrorMsg.toStdString());
    }
}


FuncDefNode::~FuncDefNode()
{
    if(body) {delete body, body=NULL;}

    QListIterator<VariableNode*> i(parameters);
    while (i.hasNext()) {
        VariableNode* paraNode = i.next();
        if(paraNode) delete paraNode;
    }
    parameters.clear();
}

Symbol FuncDefNode::eval()
{
    if(g_pCurrentEnv) {
        /// \note the newly defined function will rewrite the function table if this function name already exist.
        g_pCurrentEnv->setFuncDefNode(name, this);

    }
    else {
        QString strErrorMsg = "Bad Exception: enviroment not exist";
        throw (strErrorMsg.toStdString());
    }
    return Symbol(1);
}


Symbol NewInstanceNode::eval()
{
    if(g_pCurrentEnv) {
        // step 1: create a new instance namespace
        Env* pEnv = new Env(g_pCurrentEnv);
        g_pCurrentEnv->setEnvByName(instancename, pEnv);
        g_pCurrentEnv = pEnv;

        // step 2: declare members and functions
        QList<ClassDefNode*> inheritChain;
        QString strClassName = classname;
        while (g_pCurrentEnv->isClassExist(strClassName)) {
            inheritChain.append(g_pCurrentEnv->getClassDefNode(strClassName));
            if (g_pCurrentEnv->getClassDefNode(strClassName)->hasParent()) {
                strClassName = g_pCurrentEnv->getClassDefNode(strClassName)->getParentName();
            }
            else {
                break;
            }
        }
        for (int i=inheritChain.size()-1; i>=0;i--) {
            if (inheritChain[i]) {
                inheritChain[i]->declareAllMembers();
                inheritChain[i]->declareAllFuncs();
            }
        }

        // step 3: exit this namespace but don't delete
        returnPreScope();
    }
    else {
        QString strErrorMsg = "Bad Exception: enviroment not exist";
        throw (strErrorMsg.toStdString());
    }
    return Symbol(1);
}


InstanceCallFuncNode::~InstanceCallFuncNode()
{
    if (funcCall) {
        delete funcCall, funcCall = NULL;
    }
}

Symbol InstanceCallFuncNode::eval()
{
    if(g_pCurrentEnv) {
        if (instancename[0] != QChar('@')) {
            // step 1: move to the corresponding namespace
            Env* pEnv = g_pCurrentEnv->getEnvByName(instancename);
            if (pEnv) {
                g_pCurrentEnv = pEnv;

                // step 2: call function
                Symbol retval = funcCall->eval();

                // step 3: exit this namespace but don't delete
                returnPreScope();

                return retval;
            }
            else {
                QString strErrorMsg = "Name Error: the instance \"" + instancename +"\" has not been created, you may \"new\" one first.";
                throw (strErrorMsg.toStdString());
            }
        }
        else {
            // might be an instance member
            Env* pSavedEnv = g_pCurrentEnv;

            // step 1: calculate the arguments first
            QList<Symbol> savedArgus;
            for (int i=0; i<funcCall->arguments.size(); i++) {
                savedArgus.append(funcCall->arguments[i]->eval());
            }

            while( (!g_pCurrentEnv->isTopEnv()) && (!g_pCurrentEnv->isInstanceEnv())) {
                returnPreScope();
            }
            Env* pEnv = g_pCurrentEnv->getEnvByName(instancename);
            if (pEnv) {
                g_pCurrentEnv = pEnv;
                Symbol retval = funcCall->eval(savedArgus);
                g_pCurrentEnv = pSavedEnv;
                return retval;
            }
            else {
                g_pCurrentEnv = pSavedEnv;
                QString strErrorMsg = "Name Error: the instance \"" + instancename +"\" has not been created, you may \"new\" one first.";
                throw (strErrorMsg.toStdString());
            }
        }
    }
    else {
        QString strErrorMsg = "Bad Exception: enviroment not exist";
        throw (strErrorMsg.toStdString());
    }
}


ClassDefNode::~ClassDefNode()
{
    QListIterator<ASTNode*> i(members);
    while(i.hasNext()) {
        ASTNode* pNode = i.next();
        if (pNode) delete pNode;
    }
    members.clear();

    QListIterator<FuncDefNode*> j(funcs);
    while(j.hasNext()) {
        FuncDefNode* pFunc = j.next();
        if (pFunc) delete pFunc;
    }
    funcs.clear();
}

Symbol ClassDefNode::eval()
{
    if(g_pCurrentEnv) {
        /// \note the newly defined class will rewrite the class table if this class name already exist.
        g_pCurrentEnv->addClassDefNode(classname, this);
    }
    else {
        QString strErrorMsg = "Bad Exception: enviroment not exist";
        throw (strErrorMsg.toStdString());
    }
    return Symbol(1);
}


void enterScope()
{
    g_pCurrentEnv = new Env(g_pCurrentEnv);
}


void deleteScope()
{
    if(g_pCurrentEnv) {
        Env* pSavedEnv = g_pCurrentEnv;
        g_pCurrentEnv = g_pCurrentEnv->m_pPrevious;
        delete pSavedEnv, pSavedEnv = NULL;
    }
}


void returnPreScope()
{
    if( (g_pCurrentEnv) && (g_pCurrentEnv->m_pPrevious) ) {
        g_pCurrentEnv = g_pCurrentEnv->m_pPrevious;
    }
}
