/*
 * Copyright (c) 2008 Eugene Ingerman
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

#ifndef __DEBUGSTREAM_HPP__
#define __DEBUGSTREAM_HPP__

#include <ostream>
#include <iostream>
#include <sstream>
#include <string>
#include <QTextCursor>
#include <QString>
#include <QTextStream>
#include "DebugWin.hpp"

#ifdef _DEBUG
class DebugStream 
{
private:
    QTextCursor* dbwin;   
	QString stream;
	QTextStream ts;

public:
	DebugStream(){ ts.setString( &stream, QIODevice::Unbuffered ); };
    ~DebugStream() { };
    void setOutWindow( QTextCursor* _dbwin ) { dbwin = _dbwin; };

	inline DebugStream &sync(void) { 
		if( dbwin ){
			ts.flush(); 
			dbwin->insertText( stream ); 
		}
		stream.clear(); 
		return *this; 
	}

	inline DebugStream &operator<<(QChar t) { ts << "\'" << t << "\'"; return sync(); }
    inline DebugStream &operator<<(bool t) { ts << (t ? "true" : "false"); return sync(); }
    inline DebugStream &operator<<(char t) { ts << t; return sync(); }
    inline DebugStream &operator<<(signed short t) { ts << t; return sync(); }
    inline DebugStream &operator<<(unsigned short t) { ts << t; return sync(); }
    inline DebugStream &operator<<(signed int t) { ts << t; return sync(); }
    inline DebugStream &operator<<(unsigned int t) { ts << t; return sync(); }
    inline DebugStream &operator<<(signed long t) { ts << t; return sync(); }
    inline DebugStream &operator<<(unsigned long t) { ts << t; return sync(); }
    inline DebugStream &operator<<(qint64 t)
        { ts << QString::number(t); return sync(); }
    inline DebugStream &operator<<(quint64 t)
        { ts << QString::number(t); return sync(); }
    inline DebugStream &operator<<(float t) { ts << t; return sync(); }
    inline DebugStream &operator<<(double t) { ts << t; return sync(); }
    inline DebugStream &operator<<(const char* t) { ts << QString::fromAscii(t); return sync(); }
    inline DebugStream &operator<<(const QString & t) { ts << "\"" << t  << "\""; return sync(); }
	inline DebugStream &operator<<(const std::string & t) { ts << "\"" << t.c_str()  << "\""; return sync(); }
    inline DebugStream &operator<<(const QLatin1String &t) { ts << "\""  << t.latin1() << "\""; return sync(); }
    inline DebugStream &operator<<(const QByteArray & t) { ts  << "\"" << t << "\""; return sync(); }
    inline DebugStream &operator<<(const void * t) { ts << t; return sync(); }
    inline DebugStream &operator<<(QTextStreamFunction f) {
        ts << f;
        return *this;
    }
    inline DebugStream &operator<<(QTextStreamManipulator m)
    { ts << m; return *this; }

};
#else
//in release version we disable debug output.
class DebugStream 
{

public:
	DebugStream(){ };
    ~DebugStream() { };
    void setOutWindow( QTextCursor* _dbwin ) {  };

	template<typename T> DebugStream &operator<<(T t) { return *this; }

};
#endif

extern DebugStream dbout;
#endif /* __DEBUGSTREAM_HPP__ */
