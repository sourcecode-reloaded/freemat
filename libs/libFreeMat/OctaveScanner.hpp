#ifndef __OctaveScanner_hpp__
#define __OctaveScanner_hpp__

#include "Scanner.hpp"
#include "Token.hpp"
#include "FMLib.hpp"

class OctaveScanner : public Scanner {
protected:
  virtual void fetch();
  virtual void fetchOther();
  virtual void fetchIdentifier();
  virtual void fetchWhitespace();
  virtual void fetchOctString();
public:
  OctaveScanner(FMString buf, FMString fname);
  virtual ~OctaveScanner() {}
};

#endif
