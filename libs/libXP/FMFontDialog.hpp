#ifndef __FMFontDialog_hpp__
#define __FMFontDialog_hpp__

#include "fontform.ui.h"
#include <QFontDatabase>

class FMFontDialog : public QDialog {
  Q_OBJECT
public:
  FMFontDialog(const QFont &ref, QWidget *parent = 0);
  QFont font();
private slots:
  void namePicked(const QString &name);
  void sizePicked(const QString &name);
  void updateSample();
private:
  QFontDatabase fdb;
  Ui::FixedFontDialog ui;
  QString name;
  int size;
};

#endif
