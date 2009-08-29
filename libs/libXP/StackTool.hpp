#ifndef __StackTool_hpp__
#define __StackTool_hpp__

#include <QComboBox>

class StackTool : public QWidget {
  Q_OBJECT

  QComboBox *cb;
  int m_activeDepth;
  QStringList m_text;
public:
  StackTool(QWidget *parent = 0);
signals:
  void updateStackDepth(int);
protected slots:
  void updateStackView(QStringList);
  void cbActivated(int);
};

#endif
