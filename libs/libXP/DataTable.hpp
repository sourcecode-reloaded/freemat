#ifndef __DataTable_hpp__
#define __DataTable_hpp__

#include <QAbstractTableModel>
#include <QTreeView>
#include <QMenu>
#include <QAction>
#include <QHeaderView>
#include <QSettings>
#include <QMap>
#include <QIcon>

class DataTable : public QAbstractTableModel {
  Q_OBJECT
public:
  DataTable(const QStringList &column_names);
  virtual ~DataTable() {}
  void loadData(const QList<QVariant> &);
  int rowCount(const QModelIndex&) const;
  int columnCount(const QModelIndex&) const;
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  void sort(int column, Qt::SortOrder order);
private:
  int m_columns;
  QList<QVariant> m_data;
  QStringList m_colnames;
  QMap<QString,QIcon> m_icons;
};

class DataHeaderView;

class DataView : public QTreeView {
  Q_OBJECT
public:
  DataView(QWidget* parent) : QTreeView(parent) {}
  virtual ~DataView() {}
  void setModel(QAbstractItemModel *m);
  void loadSettings(QSettings *set, QString tagname, QVector<int> default_cols);
  void saveSettings(QSettings *set, QString tagname);
private:
  QMenu *m_popup;
  QAction **m_fields;
  DataHeaderView *m_header;
}; 

class DataHeaderView : public QHeaderView {
  Q_OBJECT
public:
  DataHeaderView(Qt::Orientation o, DataView *parent) : QHeaderView(o,parent), m_dview(parent) {}
  virtual ~DataHeaderView() {}
  void initializeFieldChooser();
  void contextMenuEvent(QContextMenuEvent *e);
  void setColumnHidden(int col, bool stat);
private:
  QMenu *m_popup;
  QAction **m_fields;
  DataView *m_dview;
private slots:
  void updateChecked(bool);
};

#endif
