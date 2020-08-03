#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "ASTNode.h"
#include "BuiltInFuncs.h"

class Evaluator
{
public:
    Evaluator(ASTNode* a_nodeRoot);
    ~Evaluator();

    void evaluate();

    void clearEnv();

    void clearBuiltInFuncs();

private:
    Evaluator();

    ASTNode* m_pRootNode;

    BuiltInFuncs m_builtInFuncs;
};

#endif // EVALUATOR_H
