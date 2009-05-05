#include "HandleWidget.hpp"
#include <QtGui>

HandleWidget::HandleWidget(QWidget *parent, HandleFigure *fig) : QWidget(parent) {
  m_fig = fig;
}

HandleWidget::~HandleWidget() {
  delete m_fig;
}

QSize HandleWidget::minimumSizeHint() const {
  return QSize(50,50);
}

QSize HandleWidget::sizeHint() const {
  return QSize(640,480);
}

void HandleWidget::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  m_fig->render(&painter);
}
