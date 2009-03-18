#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>

class GLWidget : public QGLWidget
{
  Q_OBJECT

public:
  QString name;

  GLWidget(QWidget *parent = 0);
  ~GLWidget();
  
  QSize minimumSizeHint() const;
  QSize sizeHint() const;
			
protected:
  void initializeGL();
  void paintGL();
  void paintAssembly(QString aname);
  void paintNode(QString aname);
  void paintClump(QString aname);
  void resizeGL(int width, int height);
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  
private:
  QPoint lastPos;
  float curquat[4];
  float lastquat[4];
  int W, H;
};

 #endif
