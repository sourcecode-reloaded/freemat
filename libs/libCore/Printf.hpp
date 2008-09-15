#ifndef __Printf_hpp__
#define __Printf_hpp__

#include "Array.hpp"
#include "Interpreter.hpp"
#include <QString>
#include <QFile>

QString ConvertEscapeSequences(const QString &src);
ArrayVector PrintfFunction(int nargout, const ArrayVector& arg, Interpreter* eval);
#endif
