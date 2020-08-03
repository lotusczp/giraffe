#include "ASTParser.h"

#include <QDebug>

#include "ErrorHandler.h"


ASTParser::ASTParser()
{
    m_iCurrentIndex = -1;
    m_pCurrentToken = NULL;
    m_pRootNode = NULL;
}

ASTParser::~ASTParser()
{
    // delete abstract syntax tree
    deleteAST();
}

void ASTParser::setTokenList(const QList<Token *> &a_rTokenList)
{
    m_listToken = a_rTokenList;
    m_listToken.append(new Token("", Token::End));
}

void ASTParser::parse()
{
    m_pCurrentToken = next();
    m_pRootNode = program();
}

ASTNode *ASTParser::getAST()
{
    return m_pRootNode;
}

void ASTParser::deleteAST()
{
    if (m_pRootNode) {
        delete m_pRootNode, m_pRootNode = NULL;
    }
}

Token *ASTParser::next()
{
    m_iCurrentIndex++;
    if ( (m_iCurrentIndex >= m_listToken.size()) || (m_iCurrentIndex < 0) )
    {
        return NULL;
    }
    else {
        return m_listToken[m_iCurrentIndex];
    }
}

Token *ASTParser::peekNext()
{
    if ( (m_iCurrentIndex >= m_listToken.size() - 1) || (m_iCurrentIndex + 1 < 0) )
    {
        return NULL;
    }
    else {
        return m_listToken[m_iCurrentIndex+1];
    }
}

Token *ASTParser::previous()
{
    m_iCurrentIndex--;
    if ( (m_iCurrentIndex >= m_listToken.size()) || (m_iCurrentIndex < 0) )
    {
        return NULL;
    }
    else {
        return m_listToken[m_iCurrentIndex];
    }
}

Token *ASTParser::peekSecondNext()
{
    if ( (m_iCurrentIndex >= m_listToken.size() - 2) || (m_iCurrentIndex + 2 < 0) )
    {
        return NULL;
    }
    else {
        return m_listToken[m_iCurrentIndex+2];
    }
}

ASTNode *ASTParser::program()
{
    /* program ::= stat_list */
    return statList();
}

ASTNode *ASTParser::classDefStat()
{
    /* classdef_stat ::= "class" NAME ["extends" NAME]  class_body */
    if ( (m_pCurrentToken->type == Token::ReservedWords) && (m_pCurrentToken->content == "class") ) {
        m_pCurrentToken = next(); // eat "class"
        if (m_pCurrentToken->type == Token::Name) {
            QString strClassName = m_pCurrentToken->content;
            QString strParentName = "";
            if ( (peekNext()->type == Token::ReservedWords) && (peekNext()->content == "extends") ) {
                // has a parent class
                next();// eat "extends"
                m_pCurrentToken = next();
                if (m_pCurrentToken->type == Token::Name) {
                    strParentName = m_pCurrentToken->content;
                }
                else {
                    ErrorHandler::instance()->setError("Syntax Error: expect a class name");
                    return NULL;
                }
            }
            m_pCurrentToken = next();
            QList<ASTNode*> members;
            QList<FuncDefNode*> functions;
            if (classBody(members, functions)) {
                return new ClassDefNode(strClassName, strParentName, members, functions);
            }
            else {
                return NULL;
            }
        }
        else {
            ErrorHandler::instance()->setError("Syntax Error: expect a class name");
            return NULL;
        }
    }
    else {
        ErrorHandler::instance()->setError("Syntax Error: expect \"class\"");
        return NULL;
    }
}

bool ASTParser::classBody(QList<ASTNode*>& a_rMembers, QList<FuncDefNode*>& a_rFunctions)
{
    /* class_body ::= "{" {assignment_stat} {funcdef_stat} "}" */
    if (m_pCurrentToken->content == "{")  {
        QList<ASTNode*> members;
        QList<FuncDefNode*> functions;
        while (peekNext()->type == Token::Name) {
            m_pCurrentToken = next();
            ASTNode* pNode = assignmentStat();
            if (pNode) {
                members.append(pNode);
            }
            else {
                return false;
            }
        }
        while ( (peekNext()->type == Token::ReservedWords) && (peekNext()->content == "def") ) {
            m_pCurrentToken =  next();
            ASTNode* pNode = funcDefStat();
            FuncDefNode* pFunc = dynamic_cast<FuncDefNode*>(pNode);
            if (pFunc) {
                functions.append(pFunc);
            }
            else {
                return false;
            }
        }
        m_pCurrentToken =  next();
        if (m_pCurrentToken->content == "}") {
            // success
            a_rMembers = members;
            a_rFunctions = functions;
            return true;
        }
        else {
            ErrorHandler::instance()->setError("Syntax Error: expect \"}\"");
            return false;
        }
    }
    else {
        ErrorHandler::instance()->setError("Syntax Error: expect \"{\"");
        return false;
    }
}

ASTNode *ASTParser::postfixExp()
{
    /* postfix_exp ::= NAME "." NAME argu_list */
    if ( (m_pCurrentToken->type == Token::Name) && (peekNext()->content == ".") ) {
        QString strInstance = m_pCurrentToken->content;
        next(); // eat "."
        m_pCurrentToken = next();
        if (m_pCurrentToken->type == Token::Name) {
            QString strFunction = m_pCurrentToken->content;
            m_pCurrentToken = next();
            QList<ASTNode *> arguments;
            if (arguList(arguments)) {
                return new InstanceCallFuncNode(strInstance, new FuncCallNode(strFunction, arguments));
            }
            else {
                return NULL;
            }
        }
        else {
            ErrorHandler::instance()->setError("Syntax Error: expect a function name");
            return NULL;
        }
    }
    else {
        return NULL;
    }
}

ASTNode *ASTParser::funcDefStat()
{
    /* funcdef_stat ::= "def" NAME para_list block_stat */
    if ( (m_pCurrentToken->type == Token::ReservedWords) && (m_pCurrentToken->content == "def") ) {
        m_pCurrentToken = next(); // eat "def"
        if (m_pCurrentToken->type == Token::Name) {
            QString strFuncName = m_pCurrentToken->content;
            m_pCurrentToken = next();
            QList<VariableNode *> paraments;
            if (paraList(paraments)) {
                m_pCurrentToken = next();
                ASTNode* body = blockStat();
                if (body) {
                    return new FuncDefNode(strFuncName, body, paraments);
                }
                else {
                    return NULL;
                }
            }
            else {
                return NULL;
            }
        }
        else if (m_pCurrentToken->type == Token::ReservedWords) {
            ErrorHandler::instance()->setError("Syntax Error: can not use " + m_pCurrentToken->content + " as function name. It's a reserved word.");
            return NULL;
        }
        else {
            ErrorHandler::instance()->setError("Syntax Error: expect a function name");
            return NULL;
        }
    }
    else {
        ErrorHandler::instance()->setError("Syntax Error: expect \"def\"");
        return NULL;
    }
}

bool ASTParser::arguList(QList<ASTNode *> &a_rArguments)
{
    /* argu_list ::= "(" additive_exp {"," additive_exp} ")"
     *             | "(" ")"
     */
    QList<ASTNode *> arguments;
    bool retval = false;
    if (m_pCurrentToken->content == "(") {
        if (peekNext()->content == ")"){
            // no arguments
            retval = true;
            a_rArguments = arguments;
            m_pCurrentToken = next();
        }
        else {
            // at least one parameter
            m_pCurrentToken = next();
            ASTNode *e = additiveExp();
            if(e) {
                arguments.append(e);
                while (1) {
                    if (peekNext()->content == ")") {
                        m_pCurrentToken = next();
                        a_rArguments = arguments;
                        retval = true;
                        break;
                    }
                    if (peekNext()->content == ",") {
                        next(); // eat ","
                        m_pCurrentToken = next();
                        ASTNode *f = additiveExp();
                        if (f) {
                            arguments.append(f);
                        }
                        else {
                            // at least one argument is illegal, delete the ones already created.
                            QListIterator<ASTNode *> iter(arguments);
                            while(iter.hasNext()) {
                                ASTNode *pNode = iter.next();
                                if (pNode) delete pNode;
                            }
                            arguments.clear();
                            a_rArguments = arguments;
                            retval = false;
                            break;
                        }
                    }
                    else {
                        ErrorHandler::instance()->setError("Syntax Error: expect \",\"");
                        retval = false;
                    }
                }
            }
            else {
                arguments.clear();
                a_rArguments = arguments;
                retval = false;
            }
        }
    }
    else {
        ErrorHandler::instance()->setError("Syntax Error: expect \"(\"");
        retval = false;
    }
    return retval;
}

bool ASTParser::paraList(QList<VariableNode *> &a_rParameters)
{
    /* para_list ::= "(" NAME {"," NAME} ")"
     *             | "(" ")"
     */

    QList<VariableNode *> parameters;
    bool retval = false;
    if (m_pCurrentToken->content == "(") {
        if (peekNext()->content == ")"){
            // no arguments
            retval = true;
            a_rParameters = parameters;
            m_pCurrentToken = next();
        }
        else {
            // at least one parameter
            m_pCurrentToken = next();
            ASTNode *pAtom = atom();
            VariableNode* pVarNode = dynamic_cast<VariableNode*>(pAtom);
            if(pVarNode) {
                parameters.append(pVarNode);
                while (1) {
                    if (peekNext()->content == ")") {
                        m_pCurrentToken = next();
                        a_rParameters = parameters;
                        retval = true;
                        break;
                    }
                    if (peekNext()->content == ",") {
                        next(); // eat ","
                        m_pCurrentToken = next();
                        ASTNode *pAtom2 = atom();
                        VariableNode* pVarNode2 = dynamic_cast<VariableNode*>(pAtom2);
                        if (pVarNode2) {
                            parameters.append(pVarNode2);
                        }
                        else {
                            // at least one parameter is illegal, delete the ones already created.
                            QListIterator<VariableNode *> iter(parameters);
                            while(iter.hasNext()) {
                                ASTNode *pNode = iter.next();
                                if (pNode) delete pNode;
                            }
                            parameters.clear();
                            a_rParameters = parameters;
                            retval = false;
                            break;
                        }
                    }
                    else {
                        ErrorHandler::instance()->setError("Syntax Error: expect \",\"");
                        retval = false;
                    }
                }
            }
            else {
                parameters.clear();
                a_rParameters = parameters;
                retval = false;
            }
        }
    }
    else {
        ErrorHandler::instance()->setError("Syntax Error: expect \"(\"");
        retval = false;
    }
    return retval;
}

ASTNode *ASTParser::stat()
{
    /* stat ::= assignment_stat
     *        | block_stat
     *        | selection_stat
     *        | iteration_stat
     *        | exp_stat
     *        | funcdef_stat
     *        | assert_stat
     *        | print_stat
     *        | classdef_stat
     */

    if ( (m_pCurrentToken->type == Token::Operators) && (m_pCurrentToken->content == "{") ) {
        return blockStat();
    }
    else if( (m_pCurrentToken->type == Token::ReservedWords) && (m_pCurrentToken->content == "if") ){
        return selectionStat();
    }
    else if( (m_pCurrentToken->type == Token::ReservedWords) && (m_pCurrentToken->content == "while") ) {
        return iterationStat();
    }
    else if( (m_pCurrentToken->type == Token::Name) && (peekNext()->content == "=") ) {
        return assignmentStat();
    }
    else if( (m_pCurrentToken->type == Token::ReservedWords) && (m_pCurrentToken->content == "assert") ) {
        return assertStat();
    }
    else if( (m_pCurrentToken->type == Token::ReservedWords) && (m_pCurrentToken->content == "print") ) {
        return printStat();
    }
    else if( (m_pCurrentToken->type == Token::ReservedWords) && (m_pCurrentToken->content == "def") ) {
        return funcDefStat();
    }
    else if( (m_pCurrentToken->type == Token::ReservedWords) && (m_pCurrentToken->content == "class") ) {
        return classDefStat();
    }
    else {
        return expStat();
    }
}

ASTNode *ASTParser::assertStat()
{
    /* assert_stat ::= "assert" "(" logical_exp ")" */
    if ( (m_pCurrentToken->type == Token::ReservedWords) && (m_pCurrentToken->content == "assert") ) {
        m_pCurrentToken = next(); // eat "assert"
        if (m_pCurrentToken->content == "(") {
            m_pCurrentToken = next(); // eat "("
            ASTNode *pLogicalExp = logicalExp();
            if(pLogicalExp) {
                if(peekNext()->content == ")") {
                    m_pCurrentToken = next();
                    return new AssertNode(pLogicalExp);
                }
                else {
                    ErrorHandler::instance()->setError("Syntax Error: expect \")\"");
                    return NULL;
                }
            }
            else {
                return NULL;
            }
        }
        else {
            ErrorHandler::instance()->setError("Syntax Error: expect \"(\"");
            return NULL;
        }
    }
    else {
        ErrorHandler::instance()->setError("Syntax Error: expect \"assert\"");
        return NULL;
    }
}

ASTNode *ASTParser::printStat()
{
    /* print_stat ::= "print" exp_stat */
    if ( (m_pCurrentToken->type == Token::ReservedWords) && (m_pCurrentToken->content == "print") ) {
        m_pCurrentToken = next(); // eat "print"

        ASTNode *pExpStat = expStat();
        if(pExpStat) {
            return new PrintNode(pExpStat);
        }
        else {
            return NULL;
        }

    }
    else {
        ErrorHandler::instance()->setError("Syntax Error: expect \"print\"");
        return NULL;
    }
}

ASTNode *ASTParser::blockStat()
{
    /* block_stat ::= "{" stat_list "}"
     *              | "{" "}" */
    if ( (m_pCurrentToken->type == Token::Operators) && (m_pCurrentToken->content == "{") ) {
        m_pCurrentToken = next(); // eat "{"
        if (m_pCurrentToken->content == "}") {
            return new IdleNode;
        }
        else {
            ASTNode *e = statList();
            if (e) {
                if (peekNext()->content == "}") {
                    next();
#if 0
                    ASTNode *pEnterScope = new EnterScopeNode;
                    ASTNode *pExitScope = new ExitScopeNode;
                    e = new SequenceNode(pEnterScope, e);
                    e = new SequenceNode(e, pExitScope);
#endif
                    return e;
                }
                else {
                    ErrorHandler::instance()->setError("Syntax Error: expect \"}\"");
                    return NULL;
                }
            }
            else {
                return NULL;
            }
        }
    }
    else {
        ErrorHandler::instance()->setError("Syntax Error: expect \"{\"");
        return NULL;
    }
}

ASTNode *ASTParser::statList()
{
    /* stat_list ::= stat {stat} */
    ASTNode *e = stat();
    if(e) {
        while (1) {
            if (peekNext()->content == "}") {
                break;
            }
            else if( peekNext()->type == Token::End) {
                break;
            }

            m_pCurrentToken = next();
            ASTNode *f = stat();
            if (f) {
                e = new SequenceNode(e, f);
            }
            else {
                delete e, e = NULL;
                break;
            }
        }
        return e;
    }
    else {
        return NULL;
    }
}

ASTNode *ASTParser::iterationStat()
{
    /* iteration_stat ::= "while" "(" logical_exp ")" block_stat
    */

    if( (m_pCurrentToken->type == Token::ReservedWords) && (m_pCurrentToken->content == "while") ) {
        m_pCurrentToken = next(); // eat "while"
        if (m_pCurrentToken->content == "(") {
            m_pCurrentToken = next(); //eat "("
            ASTNode *e = logicalExp();
            if (e) {
                if (peekNext()->content == ")") {
                    next(); //eat ")"
                    m_pCurrentToken = next();
                    ASTNode *f = blockStat();
                    if(f) {
                        return new WhileNode(e, f);
                    }
                    else {
                        return NULL;
                    }
                }
                else {
                    ErrorHandler::instance()->setError("Syntax Error: expect \")\"");
                    return NULL;
                }
            }
            else {
                return NULL;
            }
        }
        else {
            ErrorHandler::instance()->setError("Syntax Error: expect \"(\" ");
            return NULL;
        }
    }
    else {
        ErrorHandler::instance()->setError("Syntax Error: expect an \"while\" stat");
        return NULL;
    }
}

ASTNode *ASTParser::selectionStat()
{
    /* selection_stat ::= "if" "(" logical_exp ")" block_stat {"elif" "(" logical_exp ")" block_stat} ["else" block_stat] */

    if( (m_pCurrentToken->type == Token::ReservedWords) && (m_pCurrentToken->content == "if") ){
        m_pCurrentToken = next(); // eat "if"
        if (m_pCurrentToken->content == "(") {
            m_pCurrentToken = next(); //eat "("
            ASTNode *pCondition = logicalExp();
            if (pCondition) {
                if (peekNext()->content == ")") {
                    next(); //eat ")"
                    m_pCurrentToken = next();
                    ASTNode *pSatisfied = blockStat();
                    if(pSatisfied) {
                        if( (peekNext()->type == Token::ReservedWords) && (peekNext()->content == "elif") ){
                            m_pCurrentToken = next();
                            ASTNode *pUnsatisfied = elifStat();
                            if (pUnsatisfied) {
                                return new IfNode(pCondition, pSatisfied, pUnsatisfied);
                            }
                            else {
                                return NULL;
                            }
                        }
                        else if( (peekNext()->type == Token::ReservedWords) && (peekNext()->content == "else") ){
                            m_pCurrentToken = next();
                            ASTNode *pUnsatisfied = elseStat();
                            if (pUnsatisfied) {
                                return new IfNode(pCondition, pSatisfied, pUnsatisfied);
                            }
                            else {
                                return NULL;
                            }
                        }
                        else {
                            return new IfNode(pCondition, pSatisfied, new  IdleNode);
                        }
                    }
                    else {
                        return NULL;
                    }
                }
                else {
                    ErrorHandler::instance()->setError("Syntax Error: expect \")\"");
                    return NULL;
                }
            }
            else {
                return NULL;
            }
        }
        else {
            ErrorHandler::instance()->setError("Syntax Error: expect \"(\" ");
            return NULL;
        }
    }
    else {
        ErrorHandler::instance()->setError("Syntax Error: expect an \"if\" stat");
        return NULL;
    }
}

ASTNode *ASTParser::elifStat()
{
    if (peekNext()->content == "(") {
        next(); // eat "("
        m_pCurrentToken = next();
        ASTNode *pCondition = logicalExp();
        if (pCondition) {
            if (peekNext()->content == ")") {
                next(); //eat ")"
                m_pCurrentToken = next();
                ASTNode *pSatisfied = blockStat();
                if (pSatisfied) {
                    if( (peekNext()->type == Token::ReservedWords) && (peekNext()->content == "elif") ){
                        m_pCurrentToken = next();
                        ASTNode *pUnsatisfied = elifStat();
                        if (pUnsatisfied) {
                            return new IfNode(pCondition, pSatisfied, pUnsatisfied);
                        }
                        else {
                            return NULL;
                        }
                    }
                    else if( (peekNext()->type == Token::ReservedWords) && (peekNext()->content == "else") ){
                        m_pCurrentToken = next();
                        ASTNode *pUnsatisfied = elseStat();
                        if (pUnsatisfied) {
                            return new IfNode(pCondition, pSatisfied, pUnsatisfied);
                        }
                        else {
                            return NULL;
                        }
                    }
                    else {
                        return new IfNode(pCondition, pSatisfied, new IdleNode);
                    }
                }
                else {
                    return NULL;
                }
            }
            else {
                ErrorHandler::instance()->setError("Syntax Error: expect \")\" ");
                return NULL;
            }
        }
        else {
            return NULL;
        }
    }
    else  {
        ErrorHandler::instance()->setError("Syntax Error: expect \"(\" ");
        return NULL;
    }
}

ASTNode *ASTParser::elseStat()
{
    m_pCurrentToken = next();
    ASTNode *pUnsatisfied = blockStat();
    if(pUnsatisfied) {
        return pUnsatisfied;
    }
    else {
        return NULL;
    }
}

ASTNode *ASTParser::assignmentStat()
{
    /* assignment_stat ::= NAME "=" exp_stat
     *                   | NAME "=" "new" NAME
     */
    if( (m_pCurrentToken->type == Token::Name) && (peekNext()->content == "=") && (peekSecondNext()->content != "new") ){
        ASTNode *pAtomNode = atom();
        VariableNode* pVarNode = dynamic_cast<VariableNode*>(pAtomNode);
        if(pVarNode) {
            next(); // eat "="
            m_pCurrentToken = next();
            ASTNode *pValueNode = expStat();
            if (pValueNode) {
                return new AssignNode(pVarNode, pValueNode);
            }
            else {
                return NULL;
            }
        }
        else {
            ErrorHandler::instance()->setError("Syntax Error: expect a variable");
            return NULL;
        }
    }
    else if( (m_pCurrentToken->type == Token::Name) && (peekNext()->content == "=") && (peekSecondNext()->content == "new") ){
        QString strInstance = m_pCurrentToken->content;
        next(); // eat "="
        next(); // eat "new"
        m_pCurrentToken = next();
        if (m_pCurrentToken->type == Token::Name) {
            QString strClass = m_pCurrentToken->content;
            return new NewInstanceNode(strInstance, strClass);
        }
        else {
            ErrorHandler::instance()->setError("Syntax Error: expect an class name");
            return NULL;
        }
    }
    else {
        ErrorHandler::instance()->setError("Syntax Error: expect an assignment statement");
        return NULL;
    }
}

ASTNode *ASTParser::expStat()
{
    return additiveExp();
}

ASTNode *ASTParser::logicalExp()
{
    /* logical_exp ::= logical_or_exp */
    return logicalOrExp();
}

ASTNode *ASTParser::logicalOrExp()
{
    /* logical_or_exp ::= logical_and_exp {"or" logical_and_exp} */
    ASTNode *e = logicalAndExp();
    if(e) {
        while ( peekNext()->content == "or" ) {
            next(); // eat "or"
            m_pCurrentToken = next();
            ASTNode *f = logicalAndExp();
            if(f)
                e = new LogicalNode("or", e, f);
            else {
                delete e, e = NULL; // delete the sub tree already built
                break;
            }
        }
        return e;
    }
    else {
        return NULL;
    }
}

ASTNode *ASTParser::logicalAndExp()
{
    /* logical_and_exp ::= relational_exp {"and" relational_exp} */
    ASTNode *e = relationalExp();
    if(e) {
        while ( peekNext()->content == "and" ) {
            next(); // eat "and"
            m_pCurrentToken = next();
            ASTNode *f = relationalExp();
            if(f)
                e = new LogicalNode("and", e, f);
            else {
                delete e, e = NULL; // delete the sub tree already built
                break;
            }
        }
        return e;
    }
    else {
        return NULL;
    }
}

ASTNode *ASTParser::relationalExp()
{
    /* relational_exp ::= additive_exp (">"|"<"|">="|"<="|"=="|"!=") additive_exp
    */
    ASTNode *e = additiveExp();
    if(e) {
        if ( (peekNext()->content == ">")
                || (peekNext()->content == "<")
                || (peekNext()->content == ">=")
                || (peekNext()->content == "<=")
                || (peekNext()->content == "==")
                || (peekNext()->content == "!=") ) {
            QString strOp = next()->content;
            m_pCurrentToken = next();
            ASTNode *f = additiveExp();
            if (f) {
                e = new RelationalNode(strOp, e, f);
            }
            else {
                delete e, e = NULL; // delete the sub tree already built
            }
            return e;
        }
        else {
            ErrorHandler::instance()->setError("Syntax Error: expect a relational expression");
            return NULL;
        }
    }
    else {
        return NULL;
    }
    /// \todo relational expression can not support () for now
#if 0
    else {
        if ( (m_pCurrentToken->type == Token::Operators) && (m_pCurrentToken->content == "(") ) {
            m_pCurrentToken = next();
            ASTNode *e = logicalOrExp();
            if (e) {
                if (peekNext()->content == ")") {
                    next(); //eat ")"
                    return e;
                }
                else {
                    ErrorHandler::instance()->setError("Syntax Error: expect \")\" ");
                    return NULL;
                }
            }
            else {
                return NULL;
            }
        }
        else {
            ErrorHandler::instance()->setError("Syntax Error: expect a relational expression");
            return NULL;
        }
    }
#endif
}

ASTNode *ASTParser::additiveExp()
{
    ASTNode *e = multExp();
    if(e) {
        while ( (peekNext()->content == "+") || (peekNext()->content == "-") ) {
            QString strOp = next()->content;
            m_pCurrentToken = next();
            ASTNode *f = multExp();
            if (f) {
                e = new BinaryMathOpNode(strOp, e, f);
            }
            else {
                delete e, e = NULL; // delete the sub tree already built
                break;
            }
        }
        return e;
    }
    else {
        return NULL;
    }
}

ASTNode *ASTParser::multExp()
{
    ASTNode *e = atom();
    if (e) {
        while ( (peekNext()->content == "*") || (peekNext()->content == "/") ) {
            QString strOp = next()->content;
            m_pCurrentToken = next();
            ASTNode *f = atom();
            if (f) {
                e = new BinaryMathOpNode(strOp, e, f);
            }
            else {
                delete e, e = NULL; // delete the sub tree already built
                break;
            }
        }
        return e;
    }
    else {
        return NULL;
    }
}

ASTNode *ASTParser::atom()
{
    /* atom ::= NAME
     *        | NUMBER
     *        | STRING
     *        | "(" additive_exp ")"
     *        | NAME argu_list
     *        | postfix_exp
     */

    if ( (m_pCurrentToken->type == Token::Operators) && (m_pCurrentToken->content == "(") ) {
        m_pCurrentToken = next(); // eat "("
        ASTNode *e = additiveExp();
        if (e) {
            if (peekNext()->content == ")") {
                next(); //eat ")"
                return e;
            }
            else {
                ErrorHandler::instance()->setError("Syntax Error: expect \")\" ");
                return NULL;
            }
        }
        else {
            return NULL;
        }
    }
    else if (m_pCurrentToken->type == Token::Number) {
        return new SymbolNode(m_pCurrentToken->content.toDouble());
    }
    else if (m_pCurrentToken->type == Token::StringType) {
        return new SymbolNode(m_pCurrentToken->content);
    }
    else if((m_pCurrentToken->type == Token::Name) && (peekNext()->content == ".") ) {
        return postfixExp();
    }
    else if((m_pCurrentToken->type == Token::Name) && (peekNext()->content != "(") ) {
        return new VariableNode(m_pCurrentToken->content);
    }
    else if((m_pCurrentToken->type == Token::Name) && (peekNext()->content == "(") ) {
        // function call
        QString strFuncName = m_pCurrentToken->content;
        QList<ASTNode *> arguments;
        m_pCurrentToken = next();
        if (arguList(arguments)) {
            return new FuncCallNode(strFuncName, arguments);
        }
        else {
            return NULL;
        }
    }
    else if (m_pCurrentToken->type == Token::End) {
        return NULL;
    }
    else {
        ErrorHandler::instance()->setError("Syntax Error: expect a number , a string, a variable name or an expression");
        return NULL;
    }
}
