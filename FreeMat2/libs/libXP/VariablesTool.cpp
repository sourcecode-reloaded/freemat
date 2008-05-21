/*
 * Copyright (c) 2002-2006 Samit Basu
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
#include "VariablesTool.hpp"
#include "Scope.hpp"
#include <QtGui>
#include "Array.hpp"
#include "Print.hpp"

VariablesTool::VariablesTool(QWidget *parent) : QWidget(parent) {
  QVBoxLayout *layout = new QVBoxLayout;
  m_flist = new QTableWidget;
  layout->addWidget(m_flist);
  setLayout(layout);
  setObjectName("variables");
  context = NULL;
  setMinimumSize(50,50);
}

void VariablesTool::refresh() {
  //   qDebug() << "**********Variables refresh called...";
  if (!context) return;
  //   qDebug() << "**********Variables refresh legit...";
  //   if (!context->getMutex()->tryLock()) return;
  //   qDebug() << "**********Variables refresh...";
  //   QMutexLocker lock(context->getMutex());
  //   context->getMutex()->unlock();
  m_flist->clear();
  StringVector varnames(context->listAllVariables());
  std::sort(varnames.begin(),varnames.end());
  m_flist->setRowCount(varnames.size());
  m_flist->setColumnCount(5);
  m_flist->setHorizontalHeaderLabels(QStringList() << "Name" << "Type" << "Flags" << "Size" << "Value");
  for (int i=0;i<varnames.size();i++) {
    QString varname(varnames[i]);
    QString type;
    QString flags;
    QString size;
    QString value;
    Array lookup;
    ArrayReference ptr;
    ptr = context->lookupVariable(varnames[i]);
    if (!ptr.valid()) {
      type = "undefined";
    } else {
      lookup = *ptr;
      type = lookup.className();
      if (lookup.isSparse())
	flags = "Sparse ";
      if (context->isVariableGlobal(varnames[i])) {
	flags += "Global ";
      } else if (context->isVariablePersistent(varnames[i])) {
	flags += "Persistent ";
      }
      size = lookup.dimensions().toString();
      try {
	value = ArrayToPrintableString(lookup);
      } catch (Exception& e) {
      }
    }
    m_flist->setItem(i,0,new QTableWidgetItem(varname));
    m_flist->setItem(i,1,new QTableWidgetItem(type));
    m_flist->setItem(i,2,new QTableWidgetItem(flags));
    m_flist->setItem(i,3,new QTableWidgetItem(size));
    m_flist->setItem(i,4,new QTableWidgetItem(value));
  }
  m_flist->resizeColumnsToContents();
  m_flist->verticalHeader()->hide();
}

void VariablesTool::setContext(Context *watch) {
  context = watch;
  refresh();
}
