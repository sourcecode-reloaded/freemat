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
#include "DebugWin.hpp"

class DebugBuf : public std::stringbuf
{
private:
    QTextCursor* dbwin; 
public:
    DebugBuf() : dbwin(0) {};
    void setOutWindow( QTextCursor* _dbwin ) { dbwin = _dbwin; };
    
 //   virtual std::streamsize xsputn(const char_type *_Ptr, std::streamsize _Count ){
	//std::streamsize sz = std::stringbuf::xsputn( _Ptr, _Count );
	//if( dbwin ){
	//    std::string s( _Ptr, _Count );
	//    dbwin->insertText( s.c_str() );
	//}
	//return sz;
 //   }
    int pubsync( void ){
	if( dbwin ){
	    dbwin->insertText( str().c_str() );
	}
	return sync();
    };
};

class DebugStream : public std::ostream
{
private:
    QTextCursor* dbwin;    
    DebugBuf* dbuf;
public:
    DebugStream() : std::ostream( dbuf = new DebugBuf() ) {};
    ~DebugStream() { delete rdbuf(); };
    void setOutWindow( QTextCursor* _dbwin ) { dbwin = _dbwin; dbuf->setOutWindow( dbwin ); };
};

extern DebugStream dbout;
#endif /* __DEBUGSTREAM_HPP__ */