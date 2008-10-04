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
#include <QVBoxLayout>
#include <QTextDocument>
#include <QTextCursor>
#include "DebugWin.hpp"
#include "DebugStream.hpp"

DebugStream dbout;

DebugWin::DebugWin()
{
  //QDialog* dbg_dialog = new QDialog( m_win );
  QVBoxLayout* layout = new QVBoxLayout( this );
  dbwin = new QTextEdit(this);
  dbwin->setReadOnly(true);
  layout->addWidget( dbwin );

  //textdoc = new QTextDocument( dbwin );
  //textcursor = new QTextCursor( textdoc );
  //dbwin->setDocument( textdoc );
  dbout.setWin( dbwin );

  this->show();
//  dbwin->show();
}

