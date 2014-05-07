#ifndef __TermIF_hpp__
#define __TermIF_hpp__

#include "FMLib.hpp"
#include <iostream>
#include <stdarg.h>

class TermIF
{
public:
  virtual void outputMessage(const FMString &msg) = 0;
  virtual void outputMessage(const char *format,...) = 0;
  virtual void errorMessage(const FMString &msg) = 0;
  virtual void warningMessage(const FMString &msg) = 0;
  virtual int getTerminalWidth() = 0;
};

class StdIOTermIF : public TermIF
{
public:
  void outputMessage(const FMString &msg)
  {
    std::cout << msg;
  }
  void outputMessage(const char *format,...)
  {
    va_list ap;
    va_start(ap,format);
    vprintf(format,ap);
    va_end(ap);
  }
  void errorMessage(const FMString &msg)
  {
    std::cout << msg;
  }
  void warningMessage(const FMString &msg)
  {
    std::cout << msg;
  }
  int getTerminalWidth()
  {
    return 80;
  }
};

#endif
