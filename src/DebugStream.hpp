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

template <class CharT, class TraitsT = std::char_traits<CharT> >
class DebugBuf : public std::basic_stringbuf<CharT, TraitsT>
{
private:
    QTextCursor* dbwin; 
public:
    DebugBuf() : dbwin(0) {};
    ~DebugBuf() { sync(); }
    void setOutWindow( QTextCursor* _dbwin ) { dbwin = _dbwin; };

protected:
    virtual int sync(  ){
	if( dbwin ){
	    dbwin->insertText( std::basic_stringbuf<CharT, TraitsT>::str().c_str() );
	}
	str(std::basic_string<CharT>());
	return 0;
    };
};

template<class CharT, class TraitsT = std::char_traits<CharT> >
class basic_DebugStream : public std::basic_ostream<CharT, TraitsT>
{
private:
    QTextCursor* dbwin;    
    DebugBuf<CharT, TraitsT>* dbuf;
public:
    basic_DebugStream() : std::basic_ostream<CharT, TraitsT>( dbuf = new DebugBuf<CharT, TraitsT>() ) { std::basic_ostream<CharT, TraitsT>::setf(std::ios::unitbuf); };
    ~basic_DebugStream() { dbuf->pubsync(); delete std::basic_ostream<CharT, TraitsT>::rdbuf(); };
    void setOutWindow( QTextCursor* _dbwin ) { dbwin = _dbwin; dbuf->setOutWindow( dbwin ); };
};

typedef basic_DebugStream<char>    DebugStream;
extern DebugStream dbout;
#endif /* __DEBUGSTREAM_HPP__ */