#include "Parser.hpp"
#include "Exception.hpp"
#include "Tree.hpp"

// This one is still a question:
//    [1 3' + 5]
// Need: 
//       Error reporting
//
// Done:
//       Reference counting for the tree?
//       strings (embedded quotes)
//       Comments
//       ...
//       Number recognition
//       Function handles
//       Keywords
//       Scripts
//
// [1 -2] --> [1 -2] 
// [1 - 2] --> [-1]
// [1 - - 2] --> [3]
// [1 - 2 - 3 -4] --> [-4 -4]
// [1 - 2 - 3 - 4] --> -8
// [1 -2 - 3 -4] --> [1 -5 -4]
// [- 3] --> [-3]
// [2 -(3)] --> [2 -3]
// [(2 -(3))] --> [-1]
// [1 --2] --> [1 2]
//
// An additional set of cases to consider:
// [A (3)]  [A {4}] 
// both of which are incorrectly parsed as
// [A(3)] and [A{4}] 
//
// Question:
//
//   if a +b print
//
// Suggestion.. if have unary operator followed by a nonwhitespace
// tag it as a possible unary operator.  
//
// Conclusion - if we have tight binding between the unary operators 
// 
// Conclusion - do not allow spaces after unary operators (unless you have to)
// [a' 1 b'] --> [a',1,b']
// [a ' 1 b'] --> [a,' 1 b']
// [a ' 1 b] --> error
// [a .' 1 b] --> error
// Conclusion - do not allow spaces before transpose operators
// [1 -3, 4] --> [1,-3,4]
// Conclusion - spaces and commas are equivalent!
// [a (3)] --> [a,3], not [a(3)]
// [(a (3))] --> [a(3)]
// Outside
// fprintf('%d\n',a (3)) --> works as a(3)
// 
// Special calls are causing more trouble...
//
// Consider:
//  foo /bar
// Is this treated as an expression? or as a special function
// call?
// Also
//  foo bar.dat
// causes trouble.
// Now in general, if we have an identifier (outside a bracket) followed
// by a character, it must be a special call.  That takes care of the
// above syntax.
//
// The only one missing case is the one described above.  
//
static bool HasNestedFunctions(Tree *root) {
  if (root->is(TOK_NEST_FUNC)) return true;
  for (int i=0;i<root->numChildren();i++)
    if (HasNestedFunctions(root->child(i))) return true;
  return false;
}

static unsigned AdjustContextOne(unsigned m) {
  return (((m & 0xffff) - 1) | (m & 0xffff0000));
}

Tree* Parser::statementSeperator() {
  Tree* root = new Tree;
  if (match(';')) {
    delete root;
    root = new Tree(TOK_QSTATEMENT,AdjustContextOne(m_lex.contextNum()));
    consume();
    if (match('\n')) 
      consume();
  } else if (match('\n')) {
    delete root;
    root = new Tree(TOK_STATEMENT,AdjustContextOne(m_lex.contextNum()));
    consume();
  } else if (match(',')) {
    delete root;
    root = new Tree(TOK_STATEMENT,AdjustContextOne(m_lex.contextNum()));
    consume();
  }
  return root;
}

Tree* Parser::singletonStatement() {
  Tree* root = new Tree(next());
  consume();
  return root;
}

Tree* Parser::dBStepOrTraceStatement() {
  Tree* root = new Tree(next());
  consume();
  if (match(',') || match(';') || match('\n'))
    return root;
  root->addChild(expression());
  return root;
}


Tree* Parser::multiFunctionCall() {
  Tree* root(new Tree(expect('[')));
  root->rename(TOK_MULTI);
  Tree* lhs = new Tree(TOK_BRACKETS,m_lex.contextNum());
  while (!match(']')) {
    lhs->addChild(variableDereference());
    if (match(',')) consume();
  }
  expect(']');
  root->addChild(lhs);
  expect('=');
  root->addChild(expression());
  return root;
}

Tree* Parser::functionDefinition() {
  Tree* root(new Tree(expect(TOK_FUNCTION)));
  if (match('[')) {
    consume();
    Tree* lhs = new Tree(TOK_BRACKETS,m_lex.contextNum());
    while (!match(']')) {
      lhs->addChild(identifier());
      if (match(',')) consume();
    }
    expect(']');
    root->addChild(lhs);
    expect('=');
    root->addChild(identifier());
  } else {
    // Two possible parses here
    Tree* save = identifier();
    if (match('=')) {
      Tree* lhs = new Tree(TOK_BRACKETS,m_lex.contextNum());
      lhs->addChild(save);
      root->addChild(lhs);
      expect('=');
      root->addChild(identifier());
    } else {
      root->addChild(new Tree(TOK_BRACKETS,m_lex.contextNum()));
      root->addChild(save);
    }
  }
  // Process (optional) args
  if (match('(')) {
    consume();
    Tree* args = new Tree(TOK_PARENS,m_lex.contextNum());
    while (!match(')')) {
      Tree* ident;
      if (match('&')) {
	ident = new Tree(expect('&'));
	ident->addChild(identifier());
      } else 
	ident = identifier();
      args->addChild(ident);
      if (match(',')) consume();
    }
    expect(')');
    root->addChild(args);
  } else {
    root->addChild(new Tree(TOK_PARENS,m_lex.contextNum()));
  }
  delete statementSeperator();
  root->addChild(statementList());
  //  if (HasNestedFunctions(root))
  //    expect(TOK_END);
  return root;
}

bool Parser::matchNumber() {
  return (match(TOK_INTEGER) || match(TOK_FLOAT) ||
	  match(TOK_DOUBLE) || match(TOK_COMPLEX) || 
	  match(TOK_DCOMPLEX));
}

Tree* Parser::specialFunctionCall() {
  m_lex.pushWSFlag(false);
  Tree* root = new Tree(TOK_SPECIAL,m_lex.contextNum());
  root->addChild(identifier());
  // Next must be a whitespace
  if (!match(TOK_SPACE)) serror("Not special call");
  consume();
  {
    Scanner t_lex(m_lex);
    if (t_lex.next().is(';') ||
	t_lex.next().is('\n') ||
	t_lex.next().is('(') ||
	t_lex.next().is(','))
      serror("Not special call");
    if (t_lex.next().isBinaryOperator() || 
	t_lex.next().isUnaryOperator()) {
      t_lex.consume();
      if (t_lex.next().is(TOK_SPACE)) serror("Not special call");
    }
  }
  // If the next thing is a character or a number, we grab "blobs"
  m_lex.setBlobMode(true);
  while (!match(';') && !match('\n') && !(match(','))) {
    root->addChild(new Tree(next()));
    consume();
    if (match(TOK_SPACE)) consume();
  }
  m_lex.setBlobMode(false);
  m_lex.popWSFlag();
  return root;
}

Tree* Parser::forIndexExpression() {
  if (match('(')) {
    consume();
    Tree* ret = forIndexExpression();
    expect(')');
    return ret;
  }
  Tree* ident = identifier();
  if (match('=')) {
    Tree* root(new Tree(next()));
    consume();
    Tree* expr = expression();
    root->addChildren(ident,expr);
    return root;
  } else
    return ident;
}

Tree* Parser::forStatement() {
  Tree* root(new Tree(expect(TOK_FOR)));
  Tree* index = forIndexExpression();
  delete statementSeperator();
  Tree* block = statementList();
  expect(TOK_END);
  root->addChildren(index,block);
  return root;
}

Tree* Parser::whileStatement() {
  Tree* root(new Tree(expect(TOK_WHILE)));
  Tree* warg = expression();
  delete statementSeperator();
  Tree* block = statementList();
  expect(TOK_END);
  root->addChildren(warg,block);
  return root;
}

Tree* Parser::ifStatement() {
  Tree* root(new Tree(expect(TOK_IF)));
  Tree* test = expression();
  delete statementSeperator();
  Tree* trueblock = statementList();
  root->addChildren(test,trueblock);
  while (match(TOK_ELSEIF)) {
    Tree* elseif(new Tree(next()));
    consume();
    Tree* test = expression();
    Tree* block = statementList();
    elseif->addChildren(test,block);
    root->addChild(elseif);
  }
  if (match(TOK_ELSE)) {
    Tree* elseblk(new Tree(next()));
    consume();
    Tree* block = statementList();
    elseblk->addChild(block);
    root->addChild(elseblk);
  }
  expect(TOK_END);
  return root;
}

Tree* Parser::identifier() {
  if (!match(TOK_IDENT))
    serror("expecting identifier");
  Tree* ret = new Tree(next());
  consume();
  return ret;
}

Tree* Parser::declarationStatement() {
  Tree* root(new Tree(next()));
  consume();
  while (match(TOK_IDENT))
    root->addChild(identifier());
  return root;
}

Tree* Parser::tryStatement() {
  Tree* root(new Tree(expect(TOK_TRY)));
  delete statementSeperator();
  Tree* block = statementList();
  root->addChild(block);
  if (match(TOK_CATCH)) {
    Tree* catchblock(new Tree(next()));
    consume();
    delete statementSeperator();
    Tree* block = statementList();
    catchblock->addChild(block);
    root->addChild(catchblock);
  }
  expect(TOK_END);
  return root;
}

Tree* Parser::keyword() {
  Tree* root(new Tree(expect('/')));
  root->rename(TOK_KEYWORD);
  root->addChild(identifier());
  if (match('=')) {
    consume();
    root->addChild(expression());
  }
  return root;
}

// Parse A(foo).goo{1:3}... etc
Tree* Parser::variableDereference(bool blankRefOK) {
  Tree* ident = identifier();
  Tree* root = new Tree(TOK_VARIABLE,m_lex.contextNum());
  root->addChild(ident);
  bool deref = true;
  while (deref) {
    if (match('(')) {
      consume();
      Tree* sub = new Tree(TOK_PARENS,m_lex.contextNum());
      while (!match(')')) {
	if (match(':'))
	  sub->addChild(new Tree(expect(':')));
	else if (match('/'))
	  sub->addChild(keyword());
	else
	  sub->addChild(expression());
	if (match(',')) consume();
      }
      if ((sub->numChildren() == 0) && (!blankRefOK))
	serror("The expression A() is not allowed.");
      expect(')');
      root->addChild(sub);
    } else if (match('{')) {
      consume();
      Tree* sub = new Tree(TOK_BRACES,m_lex.contextNum());
      while (!match('}')) {
	if (match(':'))
	  sub->addChild(new Tree(expect(':')));
	else
	  sub->addChild(expression());
	if (match(',')) consume();
      }
      if (sub->numChildren() == 0)
	serror("The expression A{} is not allowed.");
      expect('}');
      root->addChild(sub);
    } else if (match('.')) {
      Tree* dynroot(new Tree(next()));
      consume();
      if (match('(')) {
	consume();
	dynroot->rename(TOK_DYN);
	dynroot->addChild(expression());
	root->addChild(dynroot);
	expect(')');
      } else {
	dynroot->addChild(identifier());
	root->addChild(dynroot);
      }
    } else
      deref = false;
  }
  return root;
}

Tree* Parser::assignmentStatement() {
  Tree* ident = variableDereference(false);
  Tree* root(new Tree(expect('=')));
  Tree* expr = expression();
  root->addChildren(ident,expr);
  return root;
}

void Parser::flushSeperators() {
  while (1) {
    Tree* term = statementSeperator();
    if (!term) return;
    if (!term->valid()) {
      delete term;
      return;
    }
    delete term;
  }
}

Tree* Parser::switchStatement() {
  Tree* root(new Tree(expect(TOK_SWITCH)));
  Tree* swexpr = expression();
  root->addChild(swexpr);
  flushSeperators();
  while (match(TOK_CASE)) {
    Tree* caseblock(new Tree(next()));
    consume();
    Tree* csexpr = expression();
    delete statementSeperator();
    Tree* block = statementList();
    caseblock->addChildren(csexpr,block);
    root->addChild(caseblock);
  }
  if (match(TOK_OTHERWISE)) {
    Tree* otherwise(new Tree(next()));
    consume();
    delete statementSeperator();
    Tree* block = statementList();
    otherwise->addChild(block);
    root->addChild(otherwise);
  }
  expect(TOK_END);
  return root;
}

Tree* Parser::statement() {
  if (match(TOK_EOF))
    return new Tree;
  if (match(TOK_END))
    return new Tree;
  if (match(TOK_FOR))
    return forStatement();
  if (match(TOK_BREAK))
    return singletonStatement();
  if (match(TOK_CONTINUE))
    return singletonStatement();
  if (match(TOK_WHILE))
    return whileStatement();
  if (match(TOK_DBSTEP) || match(TOK_DBTRACE))
    return dBStepOrTraceStatement();
  if (match(TOK_IF))
    return ifStatement();
  if (match(TOK_SWITCH))
    return switchStatement();
  if (match(TOK_TRY))
    return tryStatement();
  if (match(TOK_KEYBOARD) || match(TOK_RETURN) || 
      match(TOK_RETALL) || match(TOK_QUIT))
    return singletonStatement();
  if (match(TOK_GLOBAL) || match(TOK_PERSISTENT))
    return declarationStatement();
  // Now come the tentative parses
  Scanner save(m_lex);
  if (match(TOK_IDENT)) {
    try {
      Tree* retval = assignmentStatement();
      return retval;
    } catch (ParseException &e) {
      m_lex = save;
    } 
  }
  if (match('[')) {
    try {
      Tree* retval = multiFunctionCall();
      return retval;
    } catch (ParseException &e) {
      m_lex = save;
    }
  }
  if (match(TOK_IDENT)) {
    try {
      Tree* retval = specialFunctionCall();
      return retval;
    } catch (ParseException &e) {
      m_lex = save;
    } 
  }
  if (match(TOK_FUNCTION)) {
    try {
      Tree* retval = functionDefinition();
      retval->rename(TOK_NEST_FUNC);
      expect(TOK_END);
      return retval;
    } catch (ParseException &e) {
      m_lex = save;
    }
  }
  try {
    Tree* retval(new Tree(TOK_EXPR,m_lex.contextNum()));
    retval->addChild(expression());
    return retval;
  } catch (ParseException &e) {
    m_lex = save;
  }
  return new Tree;
}

Tree* Parser::statementList() {
  Tree* stlist = new Tree(TOK_BLOCK,m_lex.contextNum());
  flushSeperators();
  Tree* s = statement();
  while (s->valid()) {
    Tree* sep = statementSeperator();
    if (!sep->valid()) return stlist;
    sep->addChild(s);
    stlist->addChild(sep);
    flushSeperators();
    s = statement();
  }
  return stlist;
}

Tree* Parser::expression() {
  if (match(TOK_SPACE)) consume();
  return exp(0);
}

Parser::Parser(Scanner& lex) : m_lex(lex), lastpos(0) {
}

const Token& Parser::next() {
  return m_lex.next();
}

void Parser::serror(string errmsg) {
  if (m_lex.contextNum() > lastpos) {
    lasterr = errmsg;
    lastpos = m_lex.contextNum();
  }
  throw ParseException(m_lex.contextNum(),errmsg);
}

const Token & Parser::expect(byte a) {
  const Token & ret(next());
  if (!m_lex.next().is(a)) {
    if (a != TOK_EOF)
      serror(string("Expecting ") + TokenToString(Token(a,0)));
    else
      serror(string("Unexpected input"));
  }  else {
    consume();
  }
  return ret;
}

static unsigned precedence(const Token& t) {
  switch(t.value()) {
  case TOK_SOR: return 1;
  case TOK_SAND: return 2;
  case '|': return 3;
  case '&': return 4;
  case '<': return 5;
  case '>': return 5;
  case TOK_LE : return 5;
  case TOK_GE: return 5;
  case TOK_EQ: return 5;
  case TOK_NE: return 5;
  case ':': return 6;
  case '+': return 7;
  case '-': return 7;
  case '*': return 8;
  case '/': return 8;
  case '\\': return 8;
  case TOK_DOTTIMES: return 8;
  case TOK_DOTRDIV: return 8;
  case TOK_DOTLDIV: return 8;
  case TOK_UNARY_PLUS: return 9;
  case TOK_UNARY_MINUS: return 9;
  case '~': return 9;
  case '^': return 10;
  case TOK_DOTPOWER: return 10;
  }
  return 1;
}

Tree* Parser::matDef(byte basetok, byte closebracket) {
  m_lex.pushWSFlag(false);
  Tree* matdef(new Tree(basetok));
  if (match(TOK_SPACE)) consume();
  while (!match(closebracket)) {
    Tree* rowdef(new Tree(TOK_ROWDEF,m_lex.contextNum()));
    while (!match(';') && !match('\n') && !match(closebracket)) {
      rowdef->addChild(expression());
      if (match(',')) {
	consume();
	while (match(TOK_SPACE)) consume();
      } else if (match(TOK_SPACE))
	consume();
    }
    if (match(';') || match('\n'))
      consume();
    if (match(TOK_SPACE)) consume();
    matdef->addChild(rowdef);
  }
  m_lex.popWSFlag();
  return matdef;
}

Tree* Parser::transposeFixup(Tree* base) {
  while ((next().value() == '\'') || (next().value() == TOK_DOTTRANSPOSE)) {
    base = new Tree(next(),base);
    consume();
  }
  if (match(TOK_SPACE))
    if (!((m_lex.peek(0,'-') || m_lex.peek(0,'+')) && !m_lex.peek(1,' ')))
      consume();
  return base;
}

Tree* Parser::anonymousFunction() {
  unsigned pos1, pos2;
  pos1 = m_lex.contextNum();
  Tree* root(new Tree(TOK_ANONYMOUS_FUNC,m_lex.contextNum()));
  expect('(');
  Tree* args = new Tree(TOK_PARENS,m_lex.contextNum());
  while (!match(')')) {
    args->addChild(identifier());
    if (!match(')')) expect(',');
  }
  expect(')');
  root->addChild(args);
  root->addChild(expression());
  pos2 = m_lex.contextNum();
  root->setText("(" + m_lex.snippet(pos1,pos2));
  return root;
}

Tree* Parser::primaryExpression() {
  if (next().isUnaryOperator()) {
    Token opr(next());
    consume();
    if (match(TOK_SPACE)) consume();
    if (opr.is('+')) opr.setValue(TOK_UNARY_PLUS);
    if (opr.is('-')) opr.setValue(TOK_UNARY_MINUS);
    unsigned q = precedence(opr);
    Tree* child = exp(q);
    Tree* root(new Tree(opr,child));
    return root;
  } else if (match('(')) {
    consume();
    m_lex.pushWSFlag(true);
    Tree* t = exp(0);
    m_lex.popWSFlag();
    expect(')');
    return transposeFixup(t);
  } else if (match('@')) {
    Tree* root(new Tree(next()));
    consume();
    if (match('('))
      root->addChild(anonymousFunction());
    else
      root->addChild(identifier());
    return transposeFixup(root);
  } else if (matchNumber() || match(TOK_STRING)) {
    Tree* t = new Tree(next());
    consume();
    return transposeFixup(t);
  } else if (match(TOK_END)) {
    return transposeFixup(new Tree(expect(TOK_END)));
  } else if (match(TOK_IDENT)) {
    Tree* t = variableDereference();
    return transposeFixup(t);
  } else if (match('[')) {
    consume();
    Tree* t = matDef(TOK_MATDEF,']');
    expect(']');
    return transposeFixup(t);
  } else if (match('{')) {
    consume();
    Tree* t = matDef(TOK_CELLDEF,'}');
    expect('}');
    return transposeFixup(t);
  } else {
    if (match(')') || match(']') || match('}'))
      serror("mismatched parenthesis");
    else
      serror("unrecognized token");
  }
  return new Tree;
}

Tree* Parser::exp(unsigned p) {
  Tree* t = primaryExpression();
  while (next().isBinaryOperator() && (precedence(next()) >= p)) {
    Token opr_save(next());
    consume();
    if (match(TOK_SPACE)) consume();
    unsigned q;
    if (opr_save.isRightAssociative())
      q = precedence(opr_save);
    else
      q = 1+precedence(opr_save);
    Tree* t1 = exp(q);
    t = new Tree(opr_save,t,t1);
  }
  return t;
}

bool Parser::match(byte a) {
  return m_lex.next().is(a);
}

void Parser::consume() {
  m_lex.consume();
}

// NOTES - 
//   There are still some issues here...  
//    We need to introduce another tentative parse for functions
//    Consider the case:
//     function foo
//       statements
//       function hoo
//           function sub
//           end
//       end
//     end
//  The current code will parse foo into a function,
//   

CodeBlock Parser::process() {
  lastpos = 0;
  Tree* root;
  while (match('\n'))
    consume();
  try {
    if (match(TOK_FUNCTION)) {
      root = new Tree(TOK_FUNCTION_DEFS,m_lex.contextNum());
      while (match(TOK_FUNCTION)) {
	Tree* child(functionDefinition());
	root->addChild(child);
	while (match('\n')) consume();
      }
      if (HasNestedFunctions(root) || match(TOK_END))
	expect(TOK_END);
      while (match('\n')) consume();
      while (match(TOK_FUNCTION)) {
	root->addChild(functionDefinition());
	if (HasNestedFunctions(root) || match(TOK_END))
	  expect(TOK_END);
	while (match('\n')) consume();
      }
    } else {
      root = new Tree(TOK_SCRIPT,m_lex.contextNum());
      root->addChild(statementList());
    }
  } catch(ParseException &e) {
    throw Exception(lastErr() + m_lex.context(lastPos()));
  }
  try {
    expect(TOK_EOF);
  } catch (ParseException &e) {
    throw Exception("Unexpected input" + m_lex.context());
  }
  return CodeBlock(root);
}

CodeBlock Parser::processStatementList() {
  return CodeBlock(statementList());
}

CodeBlock ParseString(string arg) {
  Scanner S(arg,"");
  Parser P(S);
  return P.processStatementList();
}

CodeBlock ParseExpressionString(string arg) {
  Scanner S(arg,"");
  Parser P(S);
  try {
    return CodeBlock(P.expression());
  } catch(ParseException &e) {
    return CodeBlock(new Tree);
  }
}