#ifndef __TermIF_hpp__
#define __TermIF_hpp__

#include "FMLib.hpp"
#include <iostream>
#include <stdarg.h>

namespace FM
{

  typedef enum {
    format_short,
    format_long,
    format_short_e,
    format_long_e,
  } FMFormatMode;
  
  
  class TermIF
  {
  public:
    virtual void output(const FMString &msg) = 0;
    virtual void output(const char *format,...) = 0;
    virtual void error(const FMString &msg) = 0;
    virtual void warning(const FMString &msg) = 0;
    virtual int getTerminalWidth() = 0;
    virtual FMFormatMode getFormatMode() = 0;
  };
  
  class StdIOTermIF : public TermIF
  {
    FMFormatMode _mode;
  public:
    StdIOTermIF() : _mode(format_short) {}
    FMFormatMode getFormatMode() 
    {
      return _mode;
    }
    void output(const FMString &msg)
    {
      std::cout << msg;
    }
    void output(const char *format,...)
    {
      va_list ap;
      va_start(ap,format);
      vprintf(format,ap);
      va_end(ap);
    }
    void error(const FMString &msg)
    {
      std::cout << msg;
    }
    void warning(const FMString &msg)
    {
      std::cout << msg;
    }
    int getTerminalWidth()
    {
      return 80;
    }
  };
};

#endif
