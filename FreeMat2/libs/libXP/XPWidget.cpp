#include "XPWidget.hpp"
#include <qpainter.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qprinter.h>
#include "QTGC.hpp"
#include <iostream>

XPWidget::XPWidget(XPWidget *parent, Point2D size) 
  : QWidget(parent), m_size(size) {
  
}

void XPWidget::paintEvent(QPaintEvent* e) {
  QWidget::paintEvent(e);

  QPainter paint(this);
  QTGC gc(paint,width(),height());
  OnDraw(gc);
}

void XPWidget::mousePressEvent(QMouseEvent* e) {
  m_mousepressed = true;
  OnMouseDown(Point2D(e->x(),e->y()));
}

void XPWidget::mouseReleaseEvent(QMouseEvent* e) {
  m_mousepressed = false;
  OnMouseUp(Point2D(e->x(),e->y()));
}

void XPWidget::mouseMoveEvent(QMouseEvent* e) {
  if (m_mousepressed)
    OnMouseDrag(Point2D(e->x(),e->y()));
}

int XPWidget::GetWidth() {
  return width();
}

int XPWidget::GetHeight() {
  return height();
}

void XPWidget::Hide() {
  hide();
}

void XPWidget::Show() {
  show();
}

void XPWidget::Resize(Point2D pt) {
  resize(pt.x,pt.y);
}

void XPWidget::Print(std::string filename) {
  QPixmap pxmap(width(),height());
  QPainter paint(&pxmap);
  QTGC gc(paint,width(),height());
  OnDraw(gc);
  QImage img(pxmap.convertToImage());
  img.save(filename,"PNG");
  QStringList list = img.outputFormatList();
  QStringList::Iterator it = list.begin();
  while( it != list.end() ) {
    std::cout << "image format " << *it << "\n";
    ++it;
  }
#if 0
  QPrinter prnt;
  prnt.setOutputToFile(TRUE);
  prnt.setOutputFileName("test.ps");
  QPainter paint(&prnt);
  QTGC gc(paint,width(),height());
  OnDraw(gc);
#endif
}

void XPWidget::GetClick(int &x, int &y) {
  //todo
}

XPWidget* XPWidget::GetParent() {
  return m_parent;
}

void XPWidget::Redraw() {
  repaint();
}

void XPWidget::resizeEvent(QResizeEvent* e) {
  m_size = Point2D(width(),height());
  OnResize();
}


QSize XPWidget::sizeHint() {
  return QSize(m_size.x,m_size.y);
}
