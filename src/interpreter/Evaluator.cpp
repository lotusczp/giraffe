#include "Evaluator.h"

#include "Env.h"

#include <QDebug>

Env* g_pCurrentEnv = NULL;

Evaluator::Evaluator(ASTNode *a_nodeRoot)
{
    m_pRootNode = a_nodeRoot;

    g_pCurrentEnv = new Env(NULL); // top enviromwnt

    // add built in functions
    g_pCurrentEnv->addFuncDef(m_builtInFuncs.getBuiltInFuncs());
}

Evaluator::~Evaluator()
{
}

void Evaluator::evaluate()
{
    if (m_pRootNode) {
        m_pRootNode->eval();
    }
}

void Evaluator::clearEnv()
{
    // delete all enviroment
    g_pCurrentEnv->deleteAllPrevious();
    delete g_pCurrentEnv, g_pCurrentEnv = NULL;
}

void Evaluator::clearBuiltInFuncs()
{
    // delete all built in functions
    if(g_pCurrentEnv) {
        QListIterator<QString> i(m_builtInFuncs.getBuiltInFuncs().keys());
        while(i.hasNext()) {
            QString strFuncName = i.next();
            g_pCurrentEnv->removeFuncDefNode(strFuncName);
        }
    }
}

Evaluator::Evaluator()
{
}
