#include "VRWindow.hpp"
#include "trackball.h"
#include <QtGui>

typedef unsigned char Scalar;
typedef unsigned short Normal;
typedef unsigned char Gradient;
typedef struct {
  Normal normal;
  Scalar scalar;
  Gradient gradient;
} Voxel;

const int NUM_FIELDS = 3;
const int NUM_SHADE_FIELDS = 2;
const int NUM_CLASSIFY_FIELDS = 1;
const int NORM_FIELD = 0;
const int NORM_MAX = VP_NORM_MAX;
const int SCALAR_FIELD = 1;
const int SCALAR_MAX = 255;
const int GRAD_FIELD = 2;
const int GRAD_MAX = VP_GRAD_MAX;
const int COLOR_CHANNELS = 3;

VRWidget::VRWidget(QWidget *parent, const Array &dp,
		   const Array &scalar_ramp, 
		   const Array &color_ramp,
		   int material_count) 
  : QWidget(parent) {
  Voxel *dummy_voxel;
  contxt = vpCreateContext();
  int rows = dp.rows();
  int cols = dp.cols();
  int slices = dp.dimensions()[2];
  int vcount = rows*cols*slices;
  data = dp.asDenseArray().toClass(UInt8);
  color_transfer_ramp = color_ramp.asDenseArray().toClass(Float);
  opacity_ramp = scalar_ramp.asDenseArray().toClass(Float);
  if (vpSetVolumeSize(contxt,rows,cols,slices) != VP_OK) 
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  if (vpSetVoxelSize(contxt,sizeof(Voxel),NUM_FIELDS,NUM_SHADE_FIELDS,
		     NUM_CLASSIFY_FIELDS) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));    
  if (vpSetVoxelField(contxt,NORM_FIELD,sizeof(dummy_voxel->normal),
		      vpFieldOffset(dummy_voxel, normal), NORM_MAX) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));    
  if (vpSetVoxelField(contxt,SCALAR_FIELD,sizeof(dummy_voxel->scalar),
		      vpFieldOffset(dummy_voxel, scalar), SCALAR_MAX) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));    
  if (vpSetVoxelField(contxt,GRAD_FIELD,sizeof(dummy_voxel->gradient),
		      vpFieldOffset(dummy_voxel, gradient), GRAD_MAX) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  BasicArray<uint8> &cp(data.real<uint8>());
  int size = vcount * sizeof(Voxel);
  Voxel *vxl = new Voxel[vcount];
  for (int i=0;i<vcount;i++)
    vxl[i].scalar = cp[index_t(i+1)];
  if (vpSetRawVoxels(contxt,(char*) vxl,size,sizeof(Voxel),
		     rows*sizeof(Voxel), rows*cols*sizeof(Voxel)) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  if (vpVolumeNormals(contxt,cp.data(),rows*cols*slices,
		      SCALAR_FIELD,GRAD_FIELD,NORM_FIELD) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  if (vpSetClassifierTable(contxt,0,SCALAR_FIELD,
			   (float*)(opacity_ramp.getVoidPointer()),
			   opacity_ramp.length()*sizeof(float)) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  if (vpSetd(contxt, VP_MIN_VOXEL_OPACITY, 0.05) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  if (vpClassifyVolume(contxt) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  BasicArray<float> ones_a(NTuple(SCALAR_MAX,1));
  ones_a.fill(1);
  ones = Array(ones_a);
  shade_table = new float[COLOR_CHANNELS*(NORM_MAX+1)*material_count];
  if (vpSetLookupShader(contxt,COLOR_CHANNELS,material_count,NORM_FIELD,
			shade_table,
			COLOR_CHANNELS*material_count*(NORM_MAX+1)*sizeof(float),
			0,0,0) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  
  if (vpSetMaterial(contxt, VP_MATERIAL0, VP_AMBIENT, VP_BOTH_SIDES,
		    0.85, 0.0, 0.0) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));    

  if (vpSetMaterial(contxt, VP_MATERIAL0, VP_DIFFUSE, VP_BOTH_SIDES,
		    0.40, 0.0, 0.0) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));

  if (vpSetMaterial(contxt, VP_MATERIAL0, VP_SPECULAR, VP_BOTH_SIDES,
		    0.90, 0.0, 0.0) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));

  if (vpSetMaterial(contxt, VP_MATERIAL0, VP_SHINYNESS, VP_BOTH_SIDES,30.0,0.0,0.0) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));

  if (vpSetLight(contxt, VP_LIGHT0, VP_DIRECTION, 0.3, 0.3, 1.0) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  if (vpSetLight(contxt, VP_LIGHT0, VP_COLOR, 1.0, 1.0, 1.0) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  if (vpEnable(contxt, VP_LIGHT0, 1) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  trackball(curquat, 0, 0, 0, 0);
  W = width(); H = height();
}

VRWidget::~VRWidget() {
  vpDestroyContext(contxt);
}

QSize VRWidget::minimumSizeHint() const {
  return QSize(50, 50);
}

QSize VRWidget::sizeHint() const {
  return QSize(400, 400);
}

void VRWidget::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  backImg = QImage(qMax(8,width()),qMax(8,height()),QImage::Format_ARGB32);
  if (vpSetImage(contxt,backImg.bits(),backImg.width(),backImg.height(),
		 backImg.bytesPerLine(),VP_ABGR) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  W = width(); H = height();
  rerender();
}

void VRWidget::mousePressEvent(QMouseEvent *event) {
  lastPos = event->pos();
}

void VRWidget::paintEvent(QPaintEvent *event) {
  qDebug() << "Paint";
  QPainter painter(this);
  painter.drawImage(rect(),backImg);
}

void VRWidget::mouseMoveEvent(QMouseEvent *event) {
  trackball(lastquat,
	    (2.0*lastPos.x() - W)/W,
	    (2.0*lastPos.y() - H)/H,
	    (2.0*event->x() - W)/W,
	    (2.0*event->y() - H)/H);
  
  add_quats(lastquat, curquat, curquat);
  rerender();
  update();
  lastPos = event->pos();
}

void VRWidget::rerender() {
  qDebug() << "render" << curquat[0] << " " << curquat[1] << " " << curquat[2] << " " << curquat[3] << "\n";;
  if (vpCurrentMatrix(contxt, VP_MODEL) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  vpMatrix4 m;
  build_rotmatrix(m,curquat);
  vpMatrix4 mtrans;
  for (int i=0;i<4;i++)
    for (int k=0;k<4;k++)
      mtrans[i][k] = m[k][i];
  vpSetMatrix(contxt, mtrans);
  
  //    if (vpTranslate(contxt,0,0,-10) != VP_OK)
  //      throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  //   if (vpMultMatrix(contxt,m) != VP_OK)
  //     throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  //    if (vpScale(contxt,1/500,1/500,1/500) != VP_OK)
  //      throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  
  //    if (vpCurrentMatrix(contxt, VP_VIEW) != VP_OK)
  //      throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  //    if (vpIdentityMatrix(contxt) != VP_OK)
  //      throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  //    if (vpTranslate(contxt, 0.1, 0.0, 0.0) != VP_OK)
  //      throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  
  //    if (vpCurrentMatrix(contxt, VP_PROJECT) != VP_OK)
  //      throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  //    if (vpWindow(contxt, VP_PARALLEL, -0.5, 0.5, -0.5, 0.5, -0.5, 0.5) != VP_OK)
  //      throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  
  //    /* set the initial viewing parameters */
  //   if (vpSeti(contxt, VP_CONCAT_MODE, VP_CONCAT_LEFT) != VP_OK)
  //     throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  //   if (vpRotate(contxt, VP_Y_AXIS, 130.0) != VP_OK)
  //     throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  //   if (vpRotate(contxt, VP_X_AXIS, -15.0) != VP_OK)
  //     throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  
  if (vpCurrentMatrix(contxt, VP_PROJECT) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  if (vpIdentityMatrix(contxt) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  if (vpWindow(contxt, VP_PARALLEL, -0.5, 0.5, -0.5, 0.5, -0.5, 0.5) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
//   if (vpCurrentMatrix(contxt, VP_MODEL) != VP_OK)
//     throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  
  if (vpShadeTable(contxt) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
  if (vpRenderClassifiedVolume(contxt) != VP_OK)
    throw Exception(QString("VolPack error: ") + vpGetErrorString(vpGetError(contxt)));
}

//!
//@Module VOLVIEW Volume View Function
//!
ArrayVector VolViewFunction(int nargout, const ArrayVector& arg) {
  VRWidget *wid = new VRWidget(NULL,arg[0],arg[1],arg[2],1);
  wid->setWindowTitle("Volume Viewer");
  wid->show();
  return ArrayVector();
}
