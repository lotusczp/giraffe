#ifndef BUILTINFUNCS_H
#define BUILTINFUNCS_H

#include <QHash>

#include "ASTNode.h"

class BuiltInFuncs
{
public:
    BuiltInFuncs();
    ~BuiltInFuncs();

    const QHash<QString, FuncDefNode*>& getBuiltInFuncs() const;

private:

    typedef QHash<QString, FuncDefNode*> FunctionTable;

    FunctionTable m_functions;
};

#endif // BUILTINFUNCS_H
