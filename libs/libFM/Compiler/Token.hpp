/*
 * Copyright (c) 2009 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef __Token_hpp__
#define __Token_hpp__

#include <stdint.h>
#include "FMLib.hpp"

namespace FM 
{
  typedef uint16_t TokenValueType;

  const TokenValueType TOK_INVALID = 0;
  const TokenValueType TOK_IDENT = 130;
  const TokenValueType TOK_SPACE = 132;
  const TokenValueType TOK_STRING = 133;
  const TokenValueType TOK_KEYWORD = 134;
  const TokenValueType TOK_BREAK = 135;
  const TokenValueType TOK_CASE = 136;
  const TokenValueType TOK_CATCH = 137;
  const TokenValueType TOK_CONTINUE = 138;
  const TokenValueType TOK_ELSE = 141;
  const TokenValueType TOK_ELSEIF = 142;
  const TokenValueType TOK_END = 143;
  const TokenValueType TOK_FOR = 144;
  const TokenValueType TOK_FUNCTION = 145;
  const TokenValueType TOK_GLOBAL = 146;
  const TokenValueType TOK_IF = 147;
  const TokenValueType TOK_KEYBOARD = 148;
  const TokenValueType TOK_OTHERWISE = 149;
  const TokenValueType TOK_PERSISTENT = 150;
  const TokenValueType TOK_QUIT = 151;
  const TokenValueType TOK_RETALL = 152;
  const TokenValueType TOK_RETURN = 153;
  const TokenValueType TOK_SWITCH = 154;
  const TokenValueType TOK_TRY = 155;
  const TokenValueType TOK_WHILE = 156;
  // Generated (synthetic) token;
  const TokenValueType TOK_MULTI = 157;
  const TokenValueType TOK_SPECIAL = 158;
  const TokenValueType TOK_VARIABLE = 159;
  const TokenValueType TOK_DYN = 160;
  const TokenValueType TOK_BLOCK = 161;
  const TokenValueType TOK_EOF = 162;
  const TokenValueType TOK_MATDEF = 163;
  const TokenValueType TOK_CELLDEF = 164;
  const TokenValueType TOK_PARENS = 165;
  const TokenValueType TOK_BRACES = 166;
  const TokenValueType TOK_BRACKETS = 167;
  const TokenValueType TOK_ROWDEF = 168;
  const TokenValueType TOK_UNARY_MINUS = 169;
  const TokenValueType TOK_UNARY_PLUS = 170;
  const TokenValueType TOK_EXPR = 171;
  const TokenValueType TOK_DOTTIMES = 172;
  const TokenValueType TOK_DOTRDIV = 173;
  const TokenValueType TOK_DOTLDIV = 174;
  const TokenValueType TOK_DOTPOWER = 175;
  const TokenValueType TOK_DOTTRANSPOSE = 176;
  const TokenValueType TOK_LE = 177;
  const TokenValueType TOK_GE = 178;
  const TokenValueType TOK_EQ = 179;
  const TokenValueType TOK_NE = 180;
  const TokenValueType TOK_SOR = 181;
  const TokenValueType TOK_SAND = 182;
  const TokenValueType TOK_QSTATEMENT = 183;
  const TokenValueType TOK_STATEMENT = 184;
  const TokenValueType TOK_REALF = 185;
  const TokenValueType TOK_IMAGF = 186;
  const TokenValueType TOK_REAL = 187;
  const TokenValueType TOK_IMAG = 188;
  const TokenValueType TOK_FUNCTION_DEFS = 190;
  const TokenValueType TOK_SCRIPT = 191;
  const TokenValueType TOK_ANONYMOUS_FUNC = 192;
  const TokenValueType TOK_NEST_FUNC = 193;
  const TokenValueType TOK_TYPE_DECL = 194;
  const TokenValueType TOK_REINDEX = 197;
  const TokenValueType TOK_INCR = 198;
  const TokenValueType TOK_DECR = 199;
  const TokenValueType TOK_INCR_PREFIX = 200;
  const TokenValueType TOK_DECR_PREFIX = 201;
  const TokenValueType TOK_INCR_POSTFIX = 202;
  const TokenValueType TOK_DECR_POSTFIX = 203;
  const TokenValueType TOK_PLUS_EQ = 204;
  const TokenValueType TOK_MINUS_EQ = 205;
  const TokenValueType TOK_AND_EQ = 206;
  const TokenValueType TOK_OR_EQ = 207;
  const TokenValueType TOK_TIMES_EQ = 208;
  const TokenValueType TOK_DOTTIMES_EQ = 209;
  const TokenValueType TOK_THROW = 210;
  const TokenValueType TOK_CLASSDEF = 211;
  const TokenValueType TOK_PROPERTIES = 212;
  const TokenValueType TOK_METHODS = 213;
  const TokenValueType TOK_ATTRIBUTES = 214;
  const TokenValueType TOK_SCOPED_IDENT = 215;
  const TokenValueType TOK_GET_METHOD = 216;
  const TokenValueType TOK_EVENTS = 217;

  class ParseException {
    unsigned m_pos;
    FMString m_text;
  public:
    ParseException(unsigned pos = 0, FMString text = FMString()) : m_pos(pos), m_text(text) {}
    unsigned Position() {return m_pos;}
    FMString Text() {return m_text;}
  };

  class Token {
    TokenValueType m_tok;
    unsigned m_pos;
    FMString m_text;
    int m_UID;
  public:
    Token();
    Token(TokenValueType tok, unsigned pos = 0, FMString text = FMString());
    bool is(TokenValueType tok) const {return m_tok == tok;}
    bool isBinaryOperator() const;
    bool isUnaryOperator() const;
    unsigned precedence() const;
    bool isRightAssociative() const;
    void setUID(int uid) {m_UID = uid;}
    int UID() const {return m_UID;}
    TokenValueType value() const {return m_tok;}
    void setValue(TokenValueType a) {m_tok = a;}
    unsigned position()  const {return m_pos;}
    FMString text()  const {return m_text;}
    void setText(FMString txt) {m_text = txt;}
    void print(FMTextStream& o) const;
  };
  FMString TokenToString(const Token& b);
  inline int LineNumber(int x) {return (x & 0xffff);}

}

#endif
