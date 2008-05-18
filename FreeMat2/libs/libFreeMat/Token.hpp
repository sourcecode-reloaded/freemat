#ifndef __Token_hpp__
#define __Token_hpp__

#include "Array.hpp"
#include <QTextStream>
#include <QChar>

typedef uint16 TokenType;

const TokenType TOK_INVALID = 0;
const TokenType TOK_IDENT = 130;
const TokenType TOK_SPACE = 132;
const TokenType TOK_STRING = 133;
const TokenType TOK_KEYWORD = 134;
const TokenType TOK_BREAK = 135;
const TokenType TOK_CASE = 136;
const TokenType TOK_CATCH = 137;
const TokenType TOK_CONTINUE = 138;
const TokenType TOK_DBSTEP = 139;
const TokenType TOK_DBTRACE = 140;
const TokenType TOK_ELSE = 141;
const TokenType TOK_ELSEIF = 142;
const TokenType TOK_END = 143;
const TokenType TOK_FOR = 144;
const TokenType TOK_FUNCTION = 145;
const TokenType TOK_GLOBAL = 146;
const TokenType TOK_IF = 147;
const TokenType TOK_KEYBOARD = 148;
const TokenType TOK_OTHERWISE = 149;
const TokenType TOK_PERSISTENT = 150;
const TokenType TOK_QUIT = 151;
const TokenType TOK_RETALL = 152;
const TokenType TOK_RETURN = 153;
const TokenType TOK_SWITCH = 154;
const TokenType TOK_TRY = 155;
const TokenType TOK_WHILE = 156;
// Generated (synthetic) token;
const TokenType TOK_MULTI = 157;
const TokenType TOK_SPECIAL = 158;
const TokenType TOK_VARIABLE = 159;
const TokenType TOK_DYN = 160;
const TokenType TOK_BLOCK = 161;
const TokenType TOK_EOF = 162;
const TokenType TOK_MATDEF = 163;
const TokenType TOK_CELLDEF = 164;
const TokenType TOK_PARENS = 165;
const TokenType TOK_BRACES = 166;
const TokenType TOK_BRACKETS = 167;
const TokenType TOK_ROWDEF = 168;
const TokenType TOK_UNARY_MINUS = 169;
const TokenType TOK_UNARY_PLUS = 170;
const TokenType TOK_EXPR = 171;
const TokenType TOK_DOTTIMES = 172;
const TokenType TOK_DOTRDIV = 173;
const TokenType TOK_DOTLDIV = 174;
const TokenType TOK_DOTPOWER = 175;
const TokenType TOK_DOTTRANSPOSE = 176;
const TokenType TOK_LE = 177;
const TokenType TOK_GE = 178;
const TokenType TOK_EQ = 179;
const TokenType TOK_NE = 180;
const TokenType TOK_SOR = 181;
const TokenType TOK_SAND = 182;
const TokenType TOK_QSTATEMENT = 183;
const TokenType TOK_STATEMENT = 184;
const TokenType TOK_REAL = 187;
const TokenType TOK_IMAG = 188;
const TokenType TOK_FUNCTION_DEFS = 190;
const TokenType TOK_SCRIPT = 191;
const TokenType TOK_ANONYMOUS_FUNC = 192;
const TokenType TOK_NEST_FUNC = 193;
const TokenType TOK_TYPE_DECL = 194;

class Serialize;

class ParseException {
  unsigned m_pos;
  QString m_text;
public:
  ParseException(unsigned pos = 0, QString text = QString()) : m_pos(pos), m_text(text) {}
  unsigned Position() {return m_pos;}
  QString Text() {return m_text;}
};

class Token {
  TokenType m_tok;
  unsigned m_pos;
  QString m_text;
  Array m_array;
public:
  Token();
  Token(TokenType tok, unsigned pos = 0, QString text = QString());
  Token(Serialize *s);
  void freeze(Serialize *s) const;
  bool is(TokenType tok) const {return m_tok == tok;}
  bool isBinaryOperator() const;
  bool isUnaryOperator() const;
  unsigned precedence() const;
  bool isRightAssociative() const;
  TokenType value() const {return m_tok;}
  void setValue(TokenType a) {m_tok = a;}
  unsigned position()  const {return m_pos;}
  QString text()  const {return m_text;}
  void setText(QString txt) {m_text = txt;}
  Array array() const {return m_array;}
  void fillArray();
  void print(QTextStream& o) const;
};

QString TokenToString(const Token& b);

QTextStream& operator<<(QTextStream& o, const Token& b);

void FreezeToken(const Token& a, Serialize *s);

Token ThawToken(Serialize *s);

#endif
