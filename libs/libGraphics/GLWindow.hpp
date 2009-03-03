#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>

class GLObject {
public:
  QString name;
  //  QString type;
  QString material;
  //  float transform[4][4];
  QVector<double> points;
  //  QVector<GLObject> parts;
};

class GLWidget : public QGLWidget
{
  Q_OBJECT

public:
  QMap<QString,GLObject> map;

  GLWidget(QWidget *parent = 0);
  ~GLWidget();
  
  QSize minimumSizeHint() const;
  QSize sizeHint() const;
			
protected:
  void initializeGL();
  void paintGL();
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
