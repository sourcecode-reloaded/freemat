#include "Interpreter.hpp"
#include "Array.hpp"
#include "helpwidget.hpp"
#include <QtCore>
#include <QApplication>
#include "PathSearch.hpp"
#include "MainApp.hpp"

extern MainApp *m_app;

//!
//@Module HELP Help
//@@Section FREEMAT
//@@Usage
//Displays help on a function available in FreeMat.  The help function
//takes one argument:
//@[
//  help topic
//@]
//where @|topic| is the topic to look for help on.  For scripts, the 
//result of running @|help| is the contents of the comments at the top
//of the file.  If FreeMat finds no comments, then it simply displays
//the function declaration.
//@@Signature
//sfunction help HelpFunction
//inputs function
//outputs none
//!
ArrayVector HelpFunction(int nargout, const ArrayVector& arg, Interpreter* eval)
{
  PathSearcher psearch(eval->getTotalPath());

  if (arg.size() != 1)
    throw Exception("help function requires a single argument (the function or script name)");
  QString fname = arg[0].asString();
  bool isFun;
  FuncPtr val;
  isFun = eval->getContext()->lookupFunction(fname,val);
  if (isFun && (val->type() == FM_M_FUNCTION)) {
    MFunctionDef *mptr;
    mptr = (MFunctionDef *) val;
    mptr->updateCode(eval);
    for (int i=0;i<(int)mptr->helpText.size();i++)
      eval->outputMessage(mptr->helpText[i]);
    return ArrayVector();
  } else {
    // Check for a mdc file with the given name
    QString mdcname = fname + ".mdc";
    mdcname = psearch.ResolvePath(mdcname);
	if( mdcname.isNull() )
		throw Exception("no help available on " + fname);

	QFile fp(mdcname);
    if (!fp.open(QIODevice::ReadOnly))
      throw Exception(QString("No help available on ") + fname);
    QTextStream io(&fp);
    int outputWidth = eval->getTerminalWidth() - 20;
    while (!io.atEnd()) {
      QString cp = io.readLine();
      eval->outputMessage("\n          ");
      int charsRemaining = outputWidth - 10;
      QStringList words = cp.split(" ");
      for (int i=0;i<words.size();i++) {
	QString word = words[i] + " ";
	if ((word.size() >= outputWidth) || (word.size() < charsRemaining)) {
	  eval->outputMessage(word);
	  charsRemaining -= word.size();
	} else {
	  eval->outputMessage("\n          ");
	  charsRemaining = outputWidth-word.size();
	  eval->outputMessage(word);
	}
      }
    }
    return ArrayVector();
  }
  throw Exception("no help for that topic");
}

static HelpWindow *m_helpwin=0;

bool inBundleMode() {
  QDir dir(QApplication::applicationDirPath());
  dir.cdUp();
  return (dir.dirName() == "Contents");
}

//!
//@Module HELPWIN Online Help Window
//@@Section FREEMAT
//@@Usage
//Brings up the online help window with the FreeMat manual.  The
//@|helpwin| function takes no arguments:
//@[
//  helpwin
//  helpwin FunctionName
//@]
//@@Signature
//sgfunction helpwin HelpWinFunction
//inputs functionname
//outputs none
//!
ArrayVector HelpWinFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {  
  QDir dir;
  if (inBundleMode()) {
    dir = QDir(QString(qApp->applicationDirPath() + "/../Resources/help/html"));
  } else {
    QSettings settings("FreeMat", Interpreter::getVersionString());
#ifdef Q_WS_WIN
    QString dirpath = QString(settings.value("root", "").toString());
    
    if( dirpath.isEmpty() )
	dirpath = QCoreApplication::applicationDirPath()+QString("/../");

    dir = QDir(dirpath+"/help/html");
#else
    dir = QDir(QString(settings.value("root", RESOURCEDIR).toString())+"/help/html");
#endif
  }
  if (!m_helpwin){
    m_helpwin = new HelpWindow(dir.canonicalPath());
    QObject::connect(m_helpwin,SIGNAL(EvaluateText(QString)),m_app->GetKeyManager(),SLOT(QueueMultiString(QString)));
  }
  if (arg.size() == 0) {
    m_helpwin->show();
    m_helpwin->raise();
  }
  else if (arg.size() == 1) {
    QString fulltext = arg[0].asString();
    m_helpwin->helpText(fulltext);
    m_helpwin->show();
    m_helpwin->raise();
  }
  else
    throw Exception("helpwin function accepts at most 1 argument.");
  return ArrayVector();
}
