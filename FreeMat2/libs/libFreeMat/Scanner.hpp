#ifndef __Scanner_hpp__
#define __Scanner_hpp__

#include "Token.hpp"
#include <QString>
#include <QStack>

class Scanner {
  QString m_filename;
  QString m_text;
  int m_ptr;
  int m_strlen;
  int m_linenumber;
  QStack<bool> m_ignorews;
  Token m_tok;
  bool m_tokValid;
  bool m_debugFlag;
  bool m_inContinuationState;
  int  m_bracketDepth;
  bool m_blobFlag;
  byte current();
  byte previous();
  byte ahead(int n);
  void fetch();
  void fetchWhitespace();
  void fetchIdentifier();
  void fetchComment();
  void fetchContinuation();
  void fetchNumber();
  void fetchString();
  void fetchBlob();
  void fetchOther();
  bool tryFetchBinary(const char* op, byte tok);
  void setToken(byte tok, QString text = QString());
  bool isBreakpointLine(int num);
  void deleteBreakpoint(int num);
public:
  Scanner(QString buf, QString fname);
  // Methods accessed by the parser
  const Token& next();
  void consume();
  bool match(byte tok);
  void setDebug(bool debugFlag) {m_debugFlag = debugFlag;}
  // Warning: Ugly Hack.  When in Special Call mode, the
  // rules for what constitutes a string change completely.
  void setBlobMode(bool blobFlag) {m_blobFlag = blobFlag;}
  void pushWSFlag(bool ignoreWS);
  void popWSFlag();
  bool done();
  bool peek(int chars, byte tok);
  unsigned position() {return m_ptr;}
  unsigned contextNum();
  QString context(unsigned pos);
  QString context();
  QString snippet(unsigned pos1, unsigned pos2);
  bool inContinuationState();
  bool inBracket();
};

#endif
