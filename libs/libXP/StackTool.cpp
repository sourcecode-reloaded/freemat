#include "StackTool.hpp"
#include <QtGui>
#include "DebugStream.hpp"

StackTool::StackTool(QWidget *parent) :
  QWidget(parent) {
  QHBoxLayout *layout = new QHBoxLayout;
  cb = new QComboBox;
  cb->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  layout->addWidget(new QLabel("Stack:"));
  layout->addWidget(cb);
  setLayout(layout);
  connect(cb,SIGNAL(activated(int)),this,SLOT(cbActivated(int)));
  m_activeDepth = 0;
}

void StackTool::cbActivated(int x) {
  emit updateStackDepth(x-m_activeDepth);
  m_activeDepth = x;  
}

void StackTool::updateStackView(QStringList p) {
  if (m_text == p)  return;
  cb->clear();
  m_activeDepth = 0;
  for (int i=0;i<p.size();i++) {
    QString entry = p[i];
    if (entry.startsWith(QChar('*'))) {
      cb->addItem(entry.remove(0,1));
      m_activeDepth = i;
    } else
      cb->addItem(entry);
  }
  cb->setCurrentIndex(m_activeDepth);
  m_text = p;
}
