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
#ifndef __Scanner_hpp__
#define __Scanner_hpp__

#include "Token.hpp"
#include "FMLib.hpp"

namespace FM
{

  class Scanner {
  protected:
    FMString m_filename;
    FMString m_text;
    int m_ptr;
    int m_strlen;
    int m_linenumber;
    FMStack<bool> m_ignorews;
    Token m_tok;
    bool m_tokValid;
    bool m_debugFlag;
    bool m_inContinuationState;
    int  m_bracketDepth;
    bool m_blobFlag;
    bool m_rolFlag;
    TokenValueType current();
    TokenValueType previous();
    TokenValueType ahead(int n);
    void fetchComment();
    void fetchRestOfLine();
    void fetchContinuation();
    void fetchNumber();
    void fetchString();
    void fetchBlob();
    bool tryFetchBinary(const char* op, TokenValueType tok);
    bool tryFetchTrinary(const char* op, TokenValueType tok);
    void setToken(TokenValueType tok, FMString text = FMString());
    bool isBreakpointLine(int num);
    void deleteBreakpoint(int num);
    virtual void fetchWhitespace();
    virtual void fetchOther();
    virtual void fetch();
    virtual void fetchIdentifier();
  public:
    Scanner(FMString buf, FMString fname);
    virtual ~Scanner() {}
    // Methods accessed by the parser
    const Token& next();
    void consume();
    bool match(TokenValueType tok);
    void setDebug(bool debugFlag) {m_debugFlag = debugFlag;}
    // Warning: Ugly Hack.  When in Special Call mode, the
    // rules for what constitutes a string change completely.
    void setBlobMode(bool blobFlag) {m_blobFlag = blobFlag;}
    void setRolMode(bool rolFlag) {m_rolFlag = rolFlag;}
    void pushWSFlag(bool ignoreWS);
    void popWSFlag();
    bool done();
    bool peek(int chars, TokenValueType tok);
    unsigned position() {return m_ptr;}
    unsigned contextNum();
    FMString context(unsigned pos);
    FMString context();
    FMString snippet(unsigned pos1, unsigned pos2);
    bool inContinuationState();
    bool inBracket();
  };
}

#endif
