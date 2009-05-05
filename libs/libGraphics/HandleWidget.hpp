#ifndef __HandleWidget_hpp__
#define __HandleWidget_hpp__

#include <QWidget>

#include <QMouseEvent>
#include <QResizeEvent>

class HandleFigure;

class HandleWidget : public QWidget
{
  Q_OBJECT
  HandleFigure *m_fig;
public:
  HandleWidget(QWidget *parent, HandleFigure *fig);
  ~HandleWidget();
  QSize minimumSizeHint() const;
  QSize sizeHint() const;
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void resizeEvent(QResizeEvent *event);
protected:
  void paintEvent(QPaintEvent *event);
};

#endif
