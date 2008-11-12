#include "FMFontDialog.hpp"
#include <QtGui>

FMFontDialog::FMFontDialog(const QFont &ref, QWidget *parent) : QDialog(parent) {
  ui.setupUi(this);
  connect(ui.namelist,SIGNAL(currentTextChanged(const QString&)),this,SLOT(namePicked(const QString&)));
  connect(ui.namelist,SIGNAL(currentTextChanged(const QString&)),this,SLOT(updateSample()));
  connect(ui.sizelist,SIGNAL(currentTextChanged(const QString&)),this,SLOT(sizePicked(const QString&)));
  connect(ui.sizelist,SIGNAL(currentTextChanged(const QString&)),this,SLOT(updateSample()));
  // Populate the font list with font names
  QStringList families(fdb.families());
  for (int i=0;i<families.size();i++) {
    if (fdb.isFixedPitch(families[i]))
      new QListWidgetItem(families[i],ui.namelist);
  }
  name = ref.family();
  size = ref.pointSize();
  QList<QListWidgetItem*> findset = ui.namelist->findItems(name,Qt::MatchContains);
  if (findset.size() > 0)
    ui.namelist->setCurrentItem(findset[0]);
};

QFont FMFontDialog::font() {
  return QFont(name,size);
}

void FMFontDialog::namePicked(const QString &itemtext) {
  ui.sizelist->clear();
  QList<int> pointSizes(fdb.pointSizes(itemtext));
  for (int i=0;i<pointSizes.size();i++) {
    new QListWidgetItem(QString::number(pointSizes[i]),ui.sizelist);
  }
  name = itemtext;
  QList<QListWidgetItem*> findset = ui.sizelist->findItems(QString::number(size),Qt::MatchFixedString);
  if (findset.size() > 0)
    ui.sizelist->setCurrentItem(findset[0]);
  else
    ui.sizelist->setCurrentRow(0);
}

void FMFontDialog::sizePicked(const QString &itemtext) {
  if (itemtext.toInt() > 0)
    size = itemtext.toInt();
}

void FMFontDialog::updateSample() {
  ui.sample->setFont(QFont(name,size));
}
