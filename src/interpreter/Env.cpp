#include "Env.h"

#include "ErrorHandler.h"

Env::Env(Env *a_pPrevious)
{
    m_pPrevious = a_pPrevious;

    m_bIsInstanceEnv = false;
}

Env::~Env()
{
}

void Env::deleteAllPrevious()
{
    if (m_pPrevious) {
        m_pPrevious->deleteAllPrevious();
        delete m_pPrevious, m_pPrevious = NULL;
    }
}

bool Env::isTopEnv() const
{
    return m_pPrevious == NULL;
}

bool Env::isVariableExist(const QString &a_strVariableName) const
{
    bool retval = false;
    if (m_variables.contains(a_strVariableName)) {
        retval = true;
    }
    else {
        if (m_pPrevious) {
            retval = m_pPrevious->isVariableExist(a_strVariableName);
        }
        else {
            retval = false;
        }
    }

    return retval;
}

Symbol Env::getValue(const QString &a_strVariableName) const
{
    if (m_variables.contains(a_strVariableName)) {
        return m_variables[a_strVariableName];
    }
    else {
        if (m_pPrevious) {
            return m_pPrevious->getValue(a_strVariableName);
        }
        else {
            ErrorHandler::instance()->setError("Undefined variable: " + a_strVariableName);
            Symbol invalid;
            return invalid;
        }
    }
}

void Env::setValue(const QString &a_strVariableName, Symbol a_dValue)
{
    m_variables.insert(a_strVariableName, a_dValue);
}

void Env::removeVariable(const QString &a_strVariableName)
{
    m_variables.remove(a_strVariableName);
}

void Env::clearVariableTable()
{
    m_variables.clear();
}

bool Env::isFuncExist(const QString &a_strVariableName) const
{
    bool retval = false;
    if (m_functions.contains(a_strVariableName)) {
        retval = true;
    }
    else {
        if (m_pPrevious) {
            retval = m_pPrevious->isFuncExist(a_strVariableName);
        }
        else {
            retval = false;
        }
    }
    return retval;
}

FuncDefNode *Env::getFuncDefNode(const QString &a_strVariableName)
{
    FuncDefNode *pRet = NULL;
    if (m_functions.contains(a_strVariableName)) {
        pRet = m_functions[a_strVariableName];
    }
    else {
        if (m_pPrevious) {
            pRet = m_pPrevious->getFuncDefNode(a_strVariableName);
        }
        else {
            ErrorHandler::instance()->setError("Undefined function: " + a_strVariableName);
        }
    }
    return pRet;
}

void Env::setFuncDefNode(const QString &a_strVariableName, FuncDefNode *a_pNode)
{
    m_functions.insert(a_strVariableName, a_pNode);
}

void Env::addFuncDef(const QHash<QString, FuncDefNode *> &a_rFuncTable)
{
    m_functions = m_functions.unite(a_rFuncTable);

}

void Env::removeFuncDefNode(const QString &a_strVariableName)
{
    if (m_functions.contains(a_strVariableName)) {
        if (m_functions.value(a_strVariableName) != NULL) {
            delete m_functions.value(a_strVariableName);
        }
        m_functions.remove(a_strVariableName);
    }
}

void Env::addClassDefNode(const QString &a_rstrInstanceName, ClassDefNode *a_pNode)
{
    m_classDef.insert(a_rstrInstanceName, a_pNode);
}

bool Env::isClassExist(const QString &a_rstrClassName)
{
    bool retval = false;
    if (m_classDef.contains(a_rstrClassName)) {
        retval = true;
    }
    else {
        if (m_pPrevious) {
            retval = m_pPrevious->isClassExist(a_rstrClassName);
        }
        else {
            retval = false;
        }
    }
    return retval;
}

ClassDefNode *Env::getClassDefNode(const QString &a_rstrClassName)
{
    ClassDefNode *pRet = NULL;
    if (m_classDef.contains(a_rstrClassName)) {
        pRet = m_classDef.value(a_rstrClassName);
    }
    else {
        if (m_pPrevious) {
            pRet = m_pPrevious->getClassDefNode(a_rstrClassName);
        }
        else {
            ErrorHandler::instance()->setError("Undefined class: " + a_rstrClassName);
        }
    }
    return pRet;
}

Env *Env::getEnvByName(const QString &a_rstrInstanceName)
{
    if (m_instanceEnvs.contains(a_rstrInstanceName))
        return m_instanceEnvs.value(a_rstrInstanceName);
    else
        return NULL;
}

void Env::setEnvByName(const QString &a_rstrInstanceName, Env *a_pEnv)
{
    a_pEnv->m_bIsInstanceEnv = true;
    m_instanceEnvs.insert(a_rstrInstanceName, a_pEnv);
}

bool Env::isInstanceEnv() const
{
    return m_bIsInstanceEnv;
}

Env::Env()
{
}

void Env::clearInstanceEnv()
{
    InstanceEnvTableIter i(m_instanceEnvs);
    while (i.hasNext()) {
        i.next();
        if (i.value()) delete i.value();
    }
    m_instanceEnvs.clear();
}
