#ifndef VRWINDOW_H
#define VRWINDOW_H

#include <QWidget>
#include <QMouseEvent>
#include <QResizeEvent>
#include "Array.hpp"
#ifdef HAVE_VOLPACK
#include "volpack.h"

class VRWidget : public QWidget
{
  Q_OBJECT
public:
  VRWidget(QWidget *parent, const Array &dp, 
	   const Array& scalar_ramp, 
	   const Array &color_ramp,
	   int material_count);
  ~VRWidget();
  QSize minimumSizeHint() const;
  QSize sizeHint() const;
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void resizeEvent(QResizeEvent *event);
  void setVolume(Array dp);
  void setColorTransferFunction(Array dp);
  void setOpacityRamp(Array dp);
  void rerender();
protected:
  void paintEvent(QPaintEvent *event);
private:
  QPoint lastPos;
  float curquat[4];
  float lastquat[4];
  int W, H;
  QImage backImg;
  Array data;
  Array color_transfer_ramp;
  Array opacity_ramp;
  Array ones;
  vpContext *contxt;
  float *shade_table;
  float *weight_table;
};

#endif

#endif
