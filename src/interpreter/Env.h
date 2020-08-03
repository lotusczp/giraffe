#ifndef ENV_H
#define ENV_H

#include <QHash>
#include <QString>
#include <QObject>

#include "Symbol.h"

#include "ASTNode.h"


class Env : public QObject
{
    Q_OBJECT
public:
    friend void enterScope();
    friend void deleteScope();
    friend void returnPreScope();

    Env(Env* a_pPrevious);
    ~Env();


    void deleteAllPrevious();

    bool isTopEnv() const;


    bool isVariableExist(const QString& a_strVariableName) const;

    Symbol getValue(const QString& a_strVariableName) const;

    void setValue(const QString& a_strVariableName, Symbol a_dValue);

    void removeVariable(const QString& a_strVariableName);

    void clearVariableTable();


    bool isFuncExist(const QString& a_strVariableName) const;

    FuncDefNode* getFuncDefNode(const QString& a_strVariableName);

    void setFuncDefNode(const QString& a_strVariableName, FuncDefNode* a_pNode);

    void addFuncDef(const QHash<QString, FuncDefNode*>& a_rFuncTable);

    void removeFuncDefNode(const QString& a_strVariableName);


    void addClassDefNode(const QString& a_rstrInstanceName, ClassDefNode* a_pNode);

    bool isClassExist(const QString& a_rstrClassName);

    ClassDefNode* getClassDefNode(const QString& a_rstrClassName);


    Env* getEnvByName(const QString& a_rstrInstanceName);

    void setEnvByName(const QString& a_rstrInstanceName, Env* a_pEnv);

    bool isInstanceEnv() const;

private:
    Env();
    void clearInstanceEnv();

private:
    Env* m_pPrevious;

    //! normal variables
    typedef QHash<QString, Symbol> SymbolTable;
    SymbolTable m_variables;

    //! functions
    typedef QHash<QString, FuncDefNode*> FunctionTable;
    typedef QHashIterator<QString, FuncDefNode*> FunctionTableIter;
    FunctionTable m_functions;


    //! user defined classes and instances stuff
    typedef QHash<QString, ClassDefNode*> ClassDefTable;
    ClassDefTable m_classDef;

    typedef QHash<QString, Env*> InstanceEnvTable;
    typedef QHashIterator<QString, Env*> InstanceEnvTableIter;
    InstanceEnvTable m_instanceEnvs;

    bool m_bIsInstanceEnv;
};

extern Env* g_pCurrentEnv;

#endif // ENV_H
