#ifndef ASTPARSER_H
#define ASTPARSER_H

/* ----------------------------------------------------------------------------------------------------------------------------
 * current grammar in BNF format:
 *
 * program ::= stat_list
 *
 *
 * classdef_stat ::= "class" NAME ["extends" NAME]  class_body
 *
 * class_body ::= "{" {assignment_stat} {funcdef_stat} "}"
 *
 * postfix_exp ::= NAME "." NAME argu_list
 *
 *
 *
 * funcdef_stat ::= "def" NAME para_list block_stat
 *
 * argu_list ::= "(" additive_exp {"," additive_exp} ")"
 *             | "(" ")"
 *
 * para_list ::= "(" NAME {"," NAME} ")"
 *             | "(" ")"
 *
 *
 * stat ::= assignment_stat
 *        | block_stat
 *        | selection_stat
 *        | iteration_stat
 *        | exp_stat
 *        | funcdef_stat
 *        | assert_stat
 *        | print_stat
 *        | classdef_stat
 *
 * assert_stat ::= "assert" "(" logical_exp ")"
 *
 * print_stat ::= "print" exp_stat
 *
 * block_stat ::= "{" stat_list "}"
 *              | "{" "}"
 *
 * stat_list ::= stat {stat}
 *
 * iteration_stat ::= "while" "(" logical_exp ")" block_stat
 *
 * selection_stat ::= "if" "(" logical_exp ")" block_stat {"elif" "(" logical_exp ")" block_stat} ["else" block_stat]
 *
 * assignment_stat ::= NAME "=" exp_stat
 *                   | NAME "=" "new" NAME
 *
 * exp_stat ::= additive_exp
 *
 *
 *
 * logical_exp ::= logical_or_exp
 *
 * logical_or_exp ::= logical_and_exp {"or" logical_and_exp}
 *
 * logical_and_exp ::= relational_exp {"and" relational_exp}
 *
 * relational_exp ::= additive_exp (">"|"<"|">="|"<="|"=="|"!=") additive_exp
 *
 *
 *
 * additive_exp ::= mult_exp {("+"|"-") mult_exp}
 *
 * mult_exp ::= atom {("*"|"/") atom}
 *
 * atom ::= NAME
 *        | NUMBER
 *        | STRING
 *        | "(" additive_exp ")"
 *        | NAME argu_list
 *        | postfix_exp
 *
 *
 *----------------------------------------------------------------------------------------------------------------------------- */

#include <QList>

#include "Token.h"
#include "ASTNode.h"

class ASTParser
{
public:
    ASTParser();
    ~ASTParser();

    void setTokenList(const QList<Token*>& a_rTokenList);

    void parse();

    ASTNode* getAST();

    void deleteAST();

private:
    Token* next();
    Token* peekNext();
    Token* previous();
    Token* peekSecondNext();


    ASTNode* program();

    ASTNode* classDefStat();
    bool classBody(QList<ASTNode *> &a_rMembers, QList<FuncDefNode *> &a_rFunctions);
    ASTNode* postfixExp();

    ASTNode* funcDefStat();
    bool arguList(QList<ASTNode*>& a_rArguments);
    bool paraList(QList<VariableNode*>& a_rParameters);

    ASTNode* stat();
    ASTNode* assertStat();
    ASTNode* printStat();
    ASTNode* blockStat();
    ASTNode* statList();
    ASTNode* iterationStat();
    ASTNode* selectionStat();
    ASTNode* elifStat();
    ASTNode* elseStat();
    ASTNode* assignmentStat();
    ASTNode* expStat();

    ASTNode* logicalExp();
    ASTNode* logicalOrExp();
    ASTNode* logicalAndExp();
    ASTNode* relationalExp();

    ASTNode* additiveExp();
    ASTNode* multExp();
    ASTNode* atom();

private:
    QList<Token*> m_listToken;

    int m_iCurrentIndex;

    Token* m_pCurrentToken;

    ASTNode* m_pRootNode;
};

#endif // ASTPARSER_H
