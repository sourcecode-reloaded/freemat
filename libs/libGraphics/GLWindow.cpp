#include <QtGui>
#include <QtOpenGL>

#include <math.h>

#include "GLWindow.hpp"
#include "Array.hpp"
#include "HandleList.hpp"

/*
 * (c) Copyright 1993, 1994, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED
 * Permission to use, copy, modify, and distribute this software for
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that
 * the name of Silicon Graphics, Inc. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * US Government Users Restricted Rights
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software
 * clause at DFARS 252.227-7013 and/or in similar or successor
 * clauses in the FAR or the DOD or NASA FAR Supplement.
 * Unpublished-- rights reserved under the copyright laws of the
 * United States.  Contractor/manufacturer is Silicon Graphics,
 * Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
 *
 * OpenGL(TM) is a trademark of Silicon Graphics, Inc.
 */
/*
 * Trackball code:
 *
 * Implementation of a virtual trackball.
 * Implemented by Gavin Bell, lots of ideas from Thant Tessman and
 *   the August '88 issue of Siggraph's "Computer Graphics," pp. 121-129.
 *
 * Vector manip code:
 *
 * Original code from:
 * David M. Ciemiewicz, Mark Grossman, Henry Moreton, and Paul Haeberli
 *
 * Much mucking with by:
 * Gavin Bell
 */
#include <math.h>
//#include "trackball.h"

/*
 * This size should really be based on the distance from the center of
 * rotation to the point on the object underneath the mouse.  That
 * point would then track the mouse as closely as possible.  This is a
 * simple example, though, so that is left as an Exercise for the
 * Programmer.
 */
#define TRACKBALLSIZE  (0.8f)

/*
 * Local function prototypes (not defined in trackball.h)
 */
static float tb_project_to_sphere(float, float, float);
static void normalize_quat(float [4]);

void
vzero(float *v)
{
    v[0] = 0.0;
    v[1] = 0.0;
    v[2] = 0.0;
}

void
vset(float *v, float x, float y, float z)
{
    v[0] = x;
    v[1] = y;
    v[2] = z;
}

void
vsub(const float *src1, const float *src2, float *dst)
{
    dst[0] = src1[0] - src2[0];
    dst[1] = src1[1] - src2[1];
    dst[2] = src1[2] - src2[2];
}

void
vcopy(const float *v1, float *v2)
{
    register int i;
    for (i = 0 ; i < 3 ; i++)
        v2[i] = v1[i];
}

void
vcross(const float *v1, const float *v2, float *cross)
{
    float temp[3];

    temp[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
    temp[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
    temp[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
    vcopy(temp, cross);
}

float
vlength(const float *v)
{
    return (float) sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

void
vscale(float *v, float div)
{
    v[0] *= div;
    v[1] *= div;
    v[2] *= div;
}

void
vnormal(float *v)
{
    vscale(v, 1.0f/vlength(v));
}

float
vdot(const float *v1, const float *v2)
{
    return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

void
vadd(const float *src1, const float *src2, float *dst)
{
    dst[0] = src1[0] + src2[0];
    dst[1] = src1[1] + src2[1];
    dst[2] = src1[2] + src2[2];
}

/*
 *  Given an axis and angle, compute quaternion.
 */
void
axis_to_quat(float a[3], float phi, float q[4])
{
    vnormal(a);
    vcopy(a, q);
    vscale(q, (float) sin(phi/2.0));
    q[3] = (float) cos(phi/2.0);
}

/*
 * Ok, simulate a track-ball.  Project the points onto the virtual
 * trackball, then figure out the axis of rotation, which is the cross
 * product of P1 P2 and O P1 (O is the center of the ball, 0,0,0)
 * Note:  This is a deformed trackball-- is a trackball in the center,
 * but is deformed into a hyperbolic sheet of rotation away from the
 * center.  This particular function was chosen after trying out
 * several variations.
 *
 * It is assumed that the arguments to this routine are in the range
 * (-1.0 ... 1.0)
 */
void
trackball(float q[4], float p1x, float p1y, float p2x, float p2y)
{
    float a[3]; /* Axis of rotation */
    float phi;  /* how much to rotate about axis */
    float p1[3], p2[3], d[3];
    float t;

    if (p1x == p2x && p1y == p2y) {
        /* Zero rotation */
        vzero(q);
        q[3] = 1.0;
        return;
    }

    /*
     * First, figure out z-coordinates for projection of P1 and P2 to
     * deformed sphere
     */
    vset(p1, p1x, p1y, tb_project_to_sphere(TRACKBALLSIZE, p1x, p1y));
    vset(p2, p2x, p2y, tb_project_to_sphere(TRACKBALLSIZE, p2x, p2y));

    /*
     *  Now, we want the cross product of P1 and P2
     */
    vcross(p2,p1,a);

    /*
     *  Figure out how much to rotate around that axis.
     */
    vsub(p1, p2, d);
    t = vlength(d) / (2.0f*TRACKBALLSIZE);

    /*
     * Avoid problems with out-of-control values...
     */
    if (t > 1.0) t = 1.0;
    if (t < -1.0) t = -1.0;
    phi = 2.0f * (float) asin(t);

    axis_to_quat(a,phi,q);
}


/*
 * Project an x,y pair onto a sphere of radius r OR a hyperbolic sheet
 * if we are away from the center of the sphere.
 */
static float
tb_project_to_sphere(float r, float x, float y)
{
    float d, t, z;

    d = (float) sqrt(x*x + y*y);
    if (d < r * 0.70710678118654752440) {    /* Inside sphere */
        z = (float) sqrt(r*r - d*d);
    } else {           /* On hyperbola */
        t = r / 1.41421356237309504880f;
        z = t*t / d;
    }
    return z;
}

/*
 * Given two rotations, e1 and e2, expressed as quaternion rotations,
 * figure out the equivalent single rotation and stuff it into dest.
 *
 * This routine also normalizes the result every RENORMCOUNT times it is
 * called, to keep error from creeping in.
 *
 * NOTE: This routine is written so that q1 or q2 may be the same
 * as dest (or each other).
 */

#define RENORMCOUNT 97

void
add_quats(float q1[4], float q2[4], float dest[4])
{
    static int count=0;
    float t1[4], t2[4], t3[4];
    float tf[4];

    vcopy(q1,t1);
    vscale(t1,q2[3]);

    vcopy(q2,t2);
    vscale(t2,q1[3]);

    vcross(q2,q1,t3);
    vadd(t1,t2,tf);
    vadd(t3,tf,tf);
    tf[3] = q1[3] * q2[3] - vdot(q1,q2);

    dest[0] = tf[0];
    dest[1] = tf[1];
    dest[2] = tf[2];
    dest[3] = tf[3];

    if (++count > RENORMCOUNT) {
        count = 0;
        normalize_quat(dest);
    }
}

/*
 * Quaternions always obey:  a^2 + b^2 + c^2 + d^2 = 1.0
 * If they don't add up to 1.0, dividing by their magnitued will
 * renormalize them.
 *
 * Note: See the following for more information on quaternions:
 *
 * - Shoemake, K., Animating rotation with quaternion curves, Computer
 *   Graphics 19, No 3 (Proc. SIGGRAPH'85), 245-254, 1985.
 * - Pletinckx, D., Quaternion calculus as a basic tool in computer
 *   graphics, The Visual Computer 5, 2-13, 1989.
 */
static void
normalize_quat(float q[4])
{
    int i;
    float mag;

    mag = (q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
    for (i = 0; i < 4; i++) q[i] /= mag;
}

/*
 * Build a rotation matrix, given a quaternion rotation.
 *
 */
void
build_rotmatrix(float m[4][4], float q[4])
{
    m[0][0] = 1.0f - 2.0f * (q[1] * q[1] + q[2] * q[2]);
    m[0][1] = 2.0f * (q[0] * q[1] - q[2] * q[3]);
    m[0][2] = 2.0f * (q[2] * q[0] + q[1] * q[3]);
    m[0][3] = 0.0f;

    m[1][0] = 2.0f * (q[0] * q[1] + q[2] * q[3]);
    m[1][1]= 1.0f - 2.0f * (q[2] * q[2] + q[0] * q[0]);
    m[1][2] = 2.0f * (q[1] * q[2] - q[0] * q[3]);
    m[1][3] = 0.0f;

    m[2][0] = 2.0f * (q[2] * q[0] - q[1] * q[3]);
    m[2][1] = 2.0f * (q[1] * q[2] + q[0] * q[3]);
    m[2][2] = 1.0f - 2.0f * (q[1] * q[1] + q[0] * q[0]);
    m[2][3] = 0.0f;

    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;
}


class GLMaterial {
public:
  float ambient[4];
  float diffuse[4];
  float specular[4];
  float shininess;
};

QMap<QString,GLMaterial> material_dictionary;
  

GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent) {
}

GLWidget::~GLWidget()  {
}

QSize GLWidget::minimumSizeHint() const {
  return QSize(50, 50);
}

QSize GLWidget::sizeHint() const {
  return QSize(400, 400);
}

GLfloat LightAmbient[] = {0,0,0,1};
GLfloat LightDiffuse[] = {1,1,1,1};
GLfloat LightSpecular[] = {1,1,1,1};
GLfloat LightPosition[] = {2,2,0,1};

void GLWidget::initializeGL() {
  qglClearColor(Qt::black);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  //  glEnable(GL_CULL_FACE);
  trackball(curquat, 0, 0, 0, 0);
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
  GLfloat m[4][4];
  build_rotmatrix(m,curquat);
  glTranslated(0.0, 0.0, -10.0);
  glMultMatrixf(&m[0][0]);
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
  W = width;
  H = height;
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
  trackball(lastquat,
	    (2.0*lastPos.x() - W)/W,
	    (2.0*lastPos.y() - H)/H,
	    (2.0*event->x() - W)/W,
	    (2.0*event->y() - H)/H);
  
  add_quats(lastquat, curquat, curquat);
  updateGL();
  
  lastPos = event->pos();
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
