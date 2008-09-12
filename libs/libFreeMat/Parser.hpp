#ifndef __Parser_hpp__
#define __Parser_hpp__

#include <QString>
#include "Scanner.hpp"
#include "Tree.hpp"

class Parser {
  Scanner &m_lex;
  unsigned lastpos;
  QString lasterr;
  void serror(QString);
  bool match(TokenValueType t);
  void consume();
  const Token& expect(TokenValueType t);
  const Token& next();
  bool matchNumber();
  void flushSeperators();
private:
  Tree* transposeFixup(Tree* base);
  Tree* forStatement();
  Tree* functionDefinition();
  Tree* forIndexExpression();
  Tree* singletonStatement();
  Tree* dBStepOrTraceStatement();
  Tree* whileStatement();
  Tree* ifStatement();
  Tree* switchStatement();
  Tree* tryStatement();
  Tree* expression();
  Tree* statement();
  Tree* declarationStatement();
  Tree* statementSeperator();
  Tree* identifier();
  Tree* assignmentStatement();
  Tree* variableDereference(bool blankRefOK = true);
  Tree* multiFunctionCall();
  Tree* matDef(TokenValueType basetok, TokenValueType closebracket);
  Tree* matrixDefinition();
  Tree* rowVectorDefinition();
  Tree* specialFunctionCall();
  Tree* keyword();
  Tree* exp(unsigned p);
  Tree* primaryExpression();
  Tree* anonymousFunction();
  Tree* statementList();
public:
  Parser(Scanner& lex);
  CodeBlock process();
  CodeBlock processStatementList();
  QString lastErr() {return lasterr;}
  unsigned lastPos() {return lastpos;}
  void dump(); 

  friend CodeBlock ParseExpressionString(QString);
};

CodeBlock ParseString(QString arg);
CodeBlock ParseExpressionString(QString arg);

#endif
