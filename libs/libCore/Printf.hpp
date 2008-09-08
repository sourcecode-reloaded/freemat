#ifndef __Printf_hpp__
#define __Printf_hpp__

#include "Array.hpp"
#include "Interpreter.hpp"
#include <QString>
#include <QFile>

QString ConvertEscapeSequences(const QString &src);
QString XprintfFunction(int nargout, const ArrayVector& arg);
ArrayVector PrintfFunction(int nargout, const ArrayVector& arg, Interpreter* eval);
ArrayVector ScanfFunction(QFile *fp, QString format);
#endif
