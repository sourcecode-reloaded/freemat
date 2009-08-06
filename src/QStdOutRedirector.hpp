//
// http://myweb.tiscali.co.uk/dolbey/QtOpenCascade/doc/html/qstdoutredirector_8h-source.html, license claim is GPL 2.0
// Appears to be from 
// http://www.qtcentre.org/forum/f-qt-programming-2/t-redirect-printf-to-qtextedit-3901.html
//
#ifndef QSTDOUTREDIRECTOR_H
#define QSTDOUTREDIRECTOR_H
#include <QString>
#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include <QTextEdit> 
#include <QTimer>
#include "DebugWin.hpp"

#ifdef WIN32
#define PIPE _pipe
#define FILENO _fileno
#define DUP _dup
#define DUP2 dup2
#define OBIN _O_BINARY
#define READ _read
#else
#define PIPE pipe
#define FILENO fileno
#define DUP dup
#define DUP2 dup2
#define OBIN O_BINARY
#define READ read
#endif

class QStdOutRedirector : public QObject
{    
  Q_OBJECT
  
  public:
  
  QStdOutRedirector() : online( false ), running( false )
  {        
    // Redirect 
#ifdef WIN32       
    if( PIPE( fdguistd, 4096, OBIN ) == -1 )
      {
	printf( "failed!" );
      }
#else
    if (PIPE( fdguistd ) == -1)
      {
	printf("failed!");
      }
#endif
    
    //int tr = fcntl(fdguistd, O_NONBLOCK);        
    
    // Duplicate stdout file descriptor (next line will close original)
    
    fStdOut  = FILENO( stdout );
    fdStdOut = DUP( fStdOut );        
    
    // Duplicate write end of pipe to stdout file descriptor        
    if( DUP2( fdguistd[1], fStdOut ) != 0 )
      {
	printf( "failed!" );
      }
    
    // Close original         
    close( fStdOut );        
    
    // Duplicate write end of original        
    DUP2( fdguistd[1], fStdOut );     
    online = true;
  }    
  
  void start( int millisecs = 100 )    
  {
    if (online)
      {
	redirTimer = new QTimer;
	connect(redirTimer, SIGNAL(timeout()), this, SLOT(readOutsToTF()));
	redirTimer->start(millisecs);    
      }
  }
  
public slots:
  
  void readOutsToTF()    
  {        
    if ( !running )
      {
	running = true;
	int n_out;
	char buffer[4096];
	QString str;
        
	//char buffer[512];        
	printf("\n");        
	fflush( stdout );         
        
	n_out = READ(fdguistd[0], buffer, 4096);                
	
	if( n_out > 1 ) 
	  {            
	    str.append( QString( buffer ) );            
	    int con = str.lastIndexOf( '\n' );            
	    int remv = str.at( con - 1 ) == '\n' ? 1 : 0;            
            
	    if( con ) 
	      {
		str = str.remove( con - remv, str.length() );
		emit sendString(str);
	      }        
	  }     
	running = false;
      }
  }
  
signals:
  void sendString(QString);

private:    
  QTimer          *redirTimer;
  bool            online;
  int             fStdOut; 
  int             fdStdOut;    
  int             fdguistd[2];
  bool            running;                        
};

#endif 
/*
  
  StdOutRedirector *redir = new StdOutRedirector;     
  //printf("This should go to fdguistd\n");      
  QApplication app(argc, argv);    
  TestWidget tw;    
  tw.show();    
  redir->setOutputTF(tw.qte);    
  printf("Testing redirection\n");    
  redir->readOutsToTF();     
  // Make a QTimer that update the read every 40 ms    
  QTimer *redirTimer = new QTimer;    
  
  //QObject::connect(redirTimer, SIGNAL(timeout()), redir, SLOT(readOutsToTF()));    
  //redirTimer->start(40);        
  //Q_DebugStream cout(std::cout, tw.qte);    
  //Q_DebugStream cerr(std::cerr, tw.qte);     
  std::cout << "Printing something else" << std::endl;    
  redir->readOutsToTF();
   
*/
