#include <QtGui>
#include <QtOpenGL>

#include <math.h>

#include "GLWindow.hpp"
#include "Array.hpp"
#include "HandleList.hpp"

class GLMaterial {
public:
  float ambient[4];
  float diffuse[4];
  float specular[4];
  float shininess;
};

QMap<QString,GLMaterial> material_dictionary;
  

GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent) {
  object = 0;
  xRot = 0;
  yRot = 0;
  zRot = 0;
  
  trolltechGreen = QColor::fromCmykF(0.40, 0.0, 1.0, 0.0);
  trolltechPurple = QColor::fromCmykF(0.39, 0.39, 0.0, 0.0);
}

GLWidget::~GLWidget()  {
  makeCurrent();
  glDeleteLists(object, 1);
}

QSize GLWidget::minimumSizeHint() const {
  return QSize(50, 50);
}

QSize GLWidget::sizeHint() const {
  return QSize(400, 400);
}

void GLWidget::setXRotation(int angle) {
  normalizeAngle(&angle);
  if (angle != xRot) {
    xRot = angle;
    emit xRotationChanged(angle);
    updateGL();
  }
}

void GLWidget::setYRotation(int angle) {
  normalizeAngle(&angle);
  if (angle != yRot) {
    yRot = angle;
    emit yRotationChanged(angle);
    updateGL();
  }
}

void GLWidget::setZRotation(int angle) {
  normalizeAngle(&angle);
  if (angle != zRot) {
    zRot = angle;
    emit zRotationChanged(angle);
    updateGL();
  }
}

GLfloat LightAmbient[] = {0,0,0,1};
GLfloat LightDiffuse[] = {1,1,1,1};
GLfloat LightSpecular[] = {1,1,1,1};
GLfloat LightPosition[] = {2,2,0,1};

void GLWidget::initializeGL() {
  //  qglClearColor(trolltechPurple.dark());
  qglClearColor(Qt::black);
  object = makeObject();
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  //  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpecular);
  glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);
  glEnable(GL_LIGHT1);
}



void getNormal(double p1[3], double p2[3], double p3[3], double pn[3]) {
  double a[3];
  double b[3];
  for (int i=0;i<3;i++) {
    a[i] = p1[i] - p2[i];
    b[i] = p3[i] - p2[i];
  }
  pn[0] = a[1] * b[2] - a[2] * b[1];
  pn[1] = a[2] * b[0] - a[0] * b[2];
  pn[2] = a[0] * b[1] - a[1] * b[0];
  double len = sqrt(pn[0]*pn[0] + pn[1]*pn[1] + pn[2]*pn[2]);
  if (len) {
    pn[0] /= len;
    pn[1] /= len;
    pn[2] /= len;
  }
}

void GLWidget::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  glTranslated(0.0, 0.0, -10.0);
  glRotated(xRot / 16.0, 1.0, 0.0, 0.0);
  glRotated(yRot / 16.0, 0.0, 1.0, 0.0);
  glRotated(zRot / 16.0, 0.0, 0.0, 1.0);
  glBegin(GL_TRIANGLES);
  double p1[3];
  double p2[3];
  double p3[3];
  double pn[3];
  QMapIterator<QString,GLObject> i(map);
  while (i.hasNext()) {
    i.next();
    GLObject obj(i.value());
    GLMaterial mat = material_dictionary.value(obj.material);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat.ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat.diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat.specular);
    glMaterialf(GL_FRONT, GL_SHININESS, mat.shininess*128.0);
    for (int i=0;i<obj.points.size();i+=9) {
      p1[0] = obj.points[i]; p1[1] = obj.points[i+1]; p1[2] = obj.points[i+2];
      p2[0] = obj.points[i+3]; p2[1] = obj.points[i+4]; p2[2] = obj.points[i+5];
      p3[0] = obj.points[i+6]; p3[1] = obj.points[i+7]; p3[2] = obj.points[i+8];
      getNormal(p1,p2,p3,pn);
      glNormal3d(pn[0],pn[1],pn[2]);
      glVertex3d(p1[0],p1[1],p1[2]);
      glVertex3d(p2[0],p2[1],p2[2]);
      glVertex3d(p3[0],p3[1],p3[2]);
    }
  }
  glEnd();

}

 void GLWidget::resizeGL(int width, int height) {
   int side = qMin(width, height);
   glViewport((width - side) / 2, (height - side) / 2, side, side);
   
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-0.5, +0.5, +0.5, -0.5, 4.0, 15.0);
   glMatrixMode(GL_MODELVIEW);
 }

 void GLWidget::mousePressEvent(QMouseEvent *event) {
   lastPos = event->pos();
 }

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
  int dx = event->x() - lastPos.x();
  int dy = event->y() - lastPos.y();
  
  if (event->buttons() & Qt::LeftButton) {
    setXRotation(xRot + 8 * dy);
    setYRotation(yRot + 8 * dx);
  } else if (event->buttons() & Qt::RightButton) {
    setXRotation(xRot + 8 * dy);
    setZRotation(zRot + 8 * dx);
  }
  lastPos = event->pos();
}

GLuint GLWidget::makeObject() {
  GLuint list = glGenLists(1);
  glNewList(list, GL_COMPILE);
  
  glBegin(GL_QUADS);
  
  GLdouble x1 = +0.06;
  GLdouble y1 = -0.14;
  GLdouble x2 = +0.14;
  GLdouble y2 = -0.06;
  GLdouble x3 = +0.08;
  GLdouble y3 = +0.00;
  GLdouble x4 = +0.30;
  GLdouble y4 = +0.22;
  
  quad(x1, y1, x2, y2, y2, x2, y1, x1);
  quad(x3, y3, x4, y4, y4, x4, y3, x3);
  
  extrude(x1, y1, x2, y2);
  extrude(x2, y2, y2, x2);
  extrude(y2, x2, y1, x1);
  extrude(y1, x1, x1, y1);
  extrude(x3, y3, x4, y4);
  extrude(x4, y4, y4, x4);
  extrude(y4, x4, y3, x3);
  
  const double Pi = 3.14159265358979323846;
  const int NumSectors = 200;
  
  for (int i = 0; i < NumSectors; ++i) {
    double angle1 = (i * 2 * Pi) / NumSectors;
    GLdouble x5 = 0.30 * sin(angle1);
    GLdouble y5 = 0.30 * cos(angle1);
    GLdouble x6 = 0.20 * sin(angle1);
    GLdouble y6 = 0.20 * cos(angle1);
    
    double angle2 = ((i + 1) * 2 * Pi) / NumSectors;
    GLdouble x7 = 0.20 * sin(angle2);
    GLdouble y7 = 0.20 * cos(angle2);
    GLdouble x8 = 0.30 * sin(angle2);
    GLdouble y8 = 0.30 * cos(angle2);
    
    quad(x5, y5, x6, y6, x7, y7, x8, y8);
    
    extrude(x6, y6, x7, y7);
    extrude(x8, y8, x5, y5);
  }
  
  glEnd();
  
  glEndList();
  return list;
}

void GLWidget::quad(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2,
		    GLdouble x3, GLdouble y3, GLdouble x4, GLdouble y4) {
  qglColor(trolltechGreen);
  
  glVertex3d(x1, y1, -0.05);
  glVertex3d(x2, y2, -0.05);
  glVertex3d(x3, y3, -0.05);
  glVertex3d(x4, y4, -0.05);
  
  glVertex3d(x4, y4, +0.05);
  glVertex3d(x3, y3, +0.05);
  glVertex3d(x2, y2, +0.05);
  glVertex3d(x1, y1, +0.05);
}

 void GLWidget::extrude(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2) {
   qglColor(trolltechGreen.dark(250 + int(100 * x1)));
   
   glVertex3d(x1, y1, +0.05);
   glVertex3d(x2, y2, +0.05);
   glVertex3d(x2, y2, -0.05);
   glVertex3d(x1, y1, -0.05);
 }

void GLWidget::normalizeAngle(int *angle) {
  while (*angle < 0)
    *angle += 360 * 16;
  while (*angle > 360 * 16)
    *angle -= 360 * 16;
}

HandleList<GLWidget*> glHandles;

//!
//@Module GLNEWWIN Open a New GL Window
//@@Section HANDLE
//@@Usage
//Create a new GL window.  The syntax for its use is
//@[
//  handle = glnewwin
//@]
//@@Signature
//gfunction glnewwin GLNewWinFunction
//input none
//output handle
//!
ArrayVector GLNewWinFunction(int nargout, const ArrayVector& arg) {
  GLWidget *t  = new GLWidget;
  t->setWindowIcon(QPixmap(":/images/freemat_small_mod_64.png"));
  unsigned int rethan = glHandles.assignHandle(t);
  t->setWindowTitle(QString("GL Figure %1").arg(rethan));
  t->show();
  return ArrayVector(Array(double(rethan)));
}

//!
//@Module GLDEFMATERIAL Defines a GL Material
//@@Section HANDLE
//@@Usage
//Define a material.  The syntax for its use is
//@[
//  gldefmaterial(name,ambient,diffuse,specular,shininess)
//@]
//where @|name| is the name of the material, and @|ambient|
//is a @|4 x 1| vector containing the ambient component of 
//the material property, and @|diffuse| is a @|4 x 1| vector
//and @|specular| is a @|4 x 1| vector containing the specular
//component of the material properties and @|shininess| is
//the exponent that governs the shinines of the material.
//@@Signature
//gfunction gldefmaterial GLDefMaterialFunction
//input name ambient diffuse specular shininess
//output none
//!
ArrayVector GLDefMaterialFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 5) 
    throw Exception("gldefmaterial requires 5 parameters");
  QString name = arg[0].asString();
  Array ambient = arg[1].toClass(Double);
  if (arg[1].length() != 4) 
    throw Exception("ambient parameter must be length 4");
  Array diffuse = arg[2].toClass(Double);
  if (arg[2].length() != 4)
    throw Exception("diffuse parameter must be length 4");
  Array specular = arg[3].toClass(Double);
  if (arg[3].length() != 4)
    throw Exception("specular parameter must be length 4");
  double shininess = arg[4].asDouble();
  GLMaterial mat;
  mat.shininess = shininess;
  for (int i=0;i<4;i++) {
    mat.ambient[i] = ambient.get((index_t)(i+1)).asDouble();
    mat.diffuse[i] = diffuse.get((index_t)(i+1)).asDouble();
    mat.specular[i] = specular.get((index_t)(i+1)).asDouble();
  }
  material_dictionary[name] = mat;
  return ArrayVector();
}


//!
//@Module GLADDTRIMESH Add a Triangle Mesh to a GL Window
//@@Section HANDLE
//@@Usage
//Add a triangle mesh to a GL Window.  The syntax for is
//use is
//@[
//  gladdtrimesh(winhandle,name,material,pointset)
//@]
//@@Signature
//gfunction gladdtrimesh GLAddTriMeshFunction
//input winhandle name material pointset
//output none
//!
ArrayVector GLAddTriMeshFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 4) throw Exception("gladdtrimesh requires four arguments");
  int winhandle = arg[0].asInteger();
  GLWidget *t = glHandles.lookupHandle(winhandle);
  QString name = arg[1].asString();
  QString material = arg[2].asString();
  Array pointset = arg[3].toClass(Double);
  const BasicArray<double> &points_rp(pointset.constReal<double>());
  if (pointset.rows() != 3)
    throw Exception("pointset argument must be a 3 x N matrix");
  GLObject p;
  if (!material_dictionary.contains(material))
    throw Exception(QString("material '") + material + QString("' is not defined"));
  p.material = material;
  for (index_t i=1;i<=pointset.cols();i++) {
    p.points.push_back(points_rp[NTuple(1,i)]);
    p.points.push_back(points_rp[NTuple(2,i)]);
    p.points.push_back(points_rp[NTuple(3,i)]);
  }
  t->map[name] = p;
  return ArrayVector();
}
