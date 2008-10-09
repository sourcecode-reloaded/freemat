#include <QImage>
#include <QImageWriter>
#include "Array.hpp"
#include "Algorithms.hpp"
#include <QtGui>
#include "PathSearch.hpp"
#include "Interpreter.hpp"
#include "Math.hpp"

//!
//@Module IMREAD Read Image File To Matrix
//@@Section IO
//@@Usage
//Reads the image data from the given file into a matrix.  Note that
//FreeMat's support for @|imread| is not complete.  Only some of the
//formats specified in the MATLAB API are implemented.  The syntax
//for its use is
//@[
//  [A,map,alpha] = imread(filename)
//@]
//where @|filename| is the name of the file to read from.  The returned
//arrays @|A| contain the image data, @|map| contains the colormap information
//(for indexed images), and @|alpha| contains the alphamap (transparency).
//The returned values will depend on the type of the original image.  Generally
//you can read images in the @|jpg,png,xpm,ppm| and some other formats.
//@@Signature
//sgfunction imread ImReadFunction
//inputs filename
//outputs A map alpha
//!
static ArrayVector imreadHelperIndexed(QImage img) {
  QVector<QRgb> colorTable(img.colorTable());
  int numcol = colorTable.size();
  BasicArray<double> ctable_dp(NTuple(numcol,3));
  for (int i=0;i<numcol;i++) {
    QColor c(colorTable[i]);
    ctable_dp[i+1] = (double) c.redF();
    ctable_dp[i+numcol+1] = (double) c.greenF();
    ctable_dp[i+2*numcol+1] = (double) c.blueF();
  }
  Array ctable(ctable_dp);
  BasicArray<uint8> img_data_dp(NTuple(img.height(),img.width()));
  for (int row=0;row<img.height();row++) {
    uchar *p = img.scanLine(row);
    for (int col=0;col<img.width();col++) 
      img_data_dp[row+col*img.height()+1] = p[col];
  }
  Array A(img_data_dp);
  QImage alpha(img.alphaChannel());
  BasicArray<uint8> img_alpha_dp(NTuple(img.height(),img.width()));
  for (int row=0;row<alpha.height();row++) {
    uchar *p = alpha.scanLine(row);
    for (int col=0;col<alpha.width();col++)
      img_alpha_dp[row+col*img.height()+1] = p[col];
  }
  Array trans(img_alpha_dp);
  return ArrayVector() << A << ctable << trans;
}

static ArrayVector imreadHelperRGB32(QImage img) {
  if (img.allGray()) {
    BasicArray<uint8> img_data_dp(NTuple(img.height(),img.width()));
    for (int row=0;row<img.height();row++) {
      QRgb *p = (QRgb*) img.scanLine(row);
      for (int col=0;col<img.width();col++) {
	int ndx = row+col*img.height();
	img_data_dp[ndx+1] = qGray(p[col]);
      }
    }
    return ArrayVector() << Array(img_data_dp)
			 << EmptyConstructor() 
			 << EmptyConstructor();
  }
  else {
    BasicArray<uint8> img_data_dp(NTuple(img.height(),img.width(),3));
    int imgcnt = img.height()*img.width();
    for (int row=0;row<img.height();row++) {
      QRgb *p = (QRgb*) img.scanLine(row);
      for (int col=0;col<img.width();col++) {
	int ndx = row+col*img.height();
	img_data_dp[ndx+1] = qRed(p[col]);
	img_data_dp[ndx+1*imgcnt+1] = qGreen(p[col]);
	img_data_dp[ndx+2*imgcnt+1] = qBlue(p[col]);
      }
    }
    return ArrayVector() << Array(img_data_dp)
			 << EmptyConstructor() 
			 << EmptyConstructor();
  }
}

static ArrayVector imreadHelperARGB32(QImage img) {
  BasicArray<uint8> img_alpha_dp(NTuple(img.height(),img.width()));
  if (img.allGray()) {
    BasicArray<uint8> img_data_dp(NTuple(img.height(),img.width()));
    for (int row=0;row<img.height();row++) {
      QRgb *p = (QRgb*) img.scanLine(row);
      for (int col=0;col<img.width();col++) {
	int ndx = row+col*img.height();
	img_data_dp[ndx+1] = qGray(p[col]);
	img_alpha_dp[ndx+1] = qAlpha(p[col]);
      }
    }
    return ArrayVector() << Array(img_data_dp)
			 << EmptyConstructor() 
			 << Array(img_alpha_dp);
  } else {
    BasicArray<uint8> img_data_dp(NTuple(img.height(),img.width(),3));
    int imgcnt = img.height()*img.width();
    for (int row=0;row<img.height();row++) {
      QRgb *p = (QRgb*) img.scanLine(row);
      for (int col=0;col<img.width();col++) {
	int ndx = row+col*img.height();
	img_data_dp[ndx+1] = qRed(p[col]);
	img_data_dp[ndx+1*imgcnt+1] = qGreen(p[col]);
	img_data_dp[ndx+2*imgcnt+1] = qBlue(p[col]);
	img_alpha_dp[ndx+1] = qAlpha(p[col]);
      }
    }
    return ArrayVector() << Array(img_data_dp)
			 << EmptyConstructor() 
			 << Array(img_alpha_dp);
  }
}

ArrayVector ImReadFunction(int nargout, const ArrayVector& arg, 
			   Interpreter* eval) {
  PathSearcher psearch(eval->getTotalPath());
  if (arg.size() == 0)
    throw Exception("imread requires a filename to read.");
  QString filename(arg[0].asString());
  QString completename;

  completename = psearch.ResolvePath(filename);
  if( completename.isNull() )
	throw Exception("unable to find file " + completename);

  // Construct the QImage object
  QImage img(completename);
  if (img.isNull())
    throw Exception("unable to read file " + completename);
  if (img.format() == QImage::Format_Invalid)
    throw Exception("file " + completename + " is invalid");
  if (img.format() == QImage::Format_Indexed8) return imreadHelperIndexed(img);
  if (img.format() == QImage::Format_RGB32) return imreadHelperRGB32(img);
  if (img.format() == QImage::Format_ARGB32) return imreadHelperARGB32(img);
  throw Exception("unsupported image format - only 8 bit indexed and 24 bit RGB and 32 bit ARGB images are supported");
  return ArrayVector();
}

//!
//@Module IMWRITE Write Matrix to Image File
//@@Section IO
//@@Usage
//Write the image data from the matrix into a given file.  Note that
//FreeMat's support for @|imwrite| is not complete.
//You can write images in the @|jpg,png,xpm,ppm| and some other formats.
//The syntax for its use is
//@[
//  imwrite(filename, A)
//  imwrite(filename, A, map)
//  imwrite(filename, A, map, alpha)
//@]
//where @|filename| is the name of the file to write to.  The input array 
//@|A| contains the image data (2D for gray or indexed, and 3D for color).  
//If @|A| is an integer array (int8, uint8, int16, uint16, int32, uint32), 
//the values of its elements should be within 0-255.  If @|A| is a 
//floating-point array (float or double), the value of its elements should be 
//within 0-1.  @|map| contains the colormap information (for indexed images),
//and @|alpha| the alphamap (transparency).
//@@Signature
//sgfunction imwrite ImWriteFunction
//inputs filename A map alpha
//outputs none
//!
static QImage imwriteHelperIndexed(Array A, Array ctable, Array trans) {
  QImage img(int(A.columns()), int(A.rows()), QImage::Format_Indexed8);
  uint8 *img_data_dp = A.real<uint8>().data();
  uint8 *ctable_dp = ctable.real<uint8>().data();
  uint8 *img_alpha_dp = trans.real<uint8>().data();

  for (int row=0;row<img.height();row++) {
    uchar *p = img.scanLine(row);
    for (int col=0;col<img.width();col++) 
      p[col] = img_data_dp[row+col*img.height()];
  }
  
  if (ctable_dp) {
    QVector<QRgb> colorTable(int(ctable.length()/3));
    int numcol = colorTable.size();
    for (int i=0;i<numcol;i++)
      colorTable[i] = qRgb(int(ctable_dp[i]),
			   int(ctable_dp[i+numcol]),
			   int(ctable_dp[i+2*numcol]));
    img.setColorTable(colorTable);
  }
  else {
    int numrow = 256;
    QVector<QRgb> colorTable(numrow);
    for (int i=0;i<numrow;i++)
      colorTable[i] = qRgb(i, i, i);
    img.setColorTable(colorTable);
  }
  
  if (img_alpha_dp) {
    QImage alpha(int(A.columns()), int(A.rows()), QImage::Format_Indexed8);
    for (int row=0;row<alpha.height();row++) {
      uchar *p = alpha.scanLine(row);
      for (int col=0;col<alpha.width();col++)
	p[col] = img_alpha_dp[row+col*img.height()];
    }
    img.setAlphaChannel(alpha);
  }
  return img;
}

static QImage imwriteHelperRGB32(Array A) {
  uint8 *img_data_dp = A.real<uint8>().data();
  QImage img(int(A.columns()), int(A.rows()), QImage::Format_RGB32);
  int imgcnt = img.height()*img.width();
  for (int row=0;row<img.height();row++) {
    QRgb *p = (QRgb*) img.scanLine(row);
    for (int col=0;col<img.width();col++) {
      int ndx = row+col*img.height();
      p[col] = qRgb(img_data_dp[ndx], 
		    img_data_dp[ndx+1*imgcnt], 
		    img_data_dp[ndx+2*imgcnt]);
    }
  }
  return img;
}

static QImage imwriteHelperARGB32(Array A, Array trans) {
  QImage img(int(A.columns()), int(A.rows()), QImage::Format_ARGB32);
  uint8 *img_data_dp = A.real<uint8>().data();
  uint8 *img_alpha_dp = trans.real<uint8>().data();
  int imgcnt = img.height()*img.width();
  for (int row=0;row<img.height();row++) {
    QRgb *p = (QRgb*) img.scanLine(row);
    for (int col=0;col<img.width();col++) {
      int ndx = row+col*img.height();
      p[col] = qRgba(img_data_dp[ndx], 
		     img_data_dp[ndx+1*imgcnt], 
		     img_data_dp[ndx+2*imgcnt], 
		     img_alpha_dp[ndx]);
    }
  }
  return img;
}

static Array convert2uint8(Array A) {
  if (A.dataClass() ==  UInt8) return A;
  if (A.dataClass() == Bool) A = A.toClass(Double);
  if ((A.dataClass() == Float) || (A.dataClass() == Double))
    return DotMultiply(A,Array(double(255.0))).toClass(UInt8);
  return A.toClass(UInt8);
}  

ArrayVector ImWriteFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  PathSearcher psearch(eval->getTotalPath());
  if (arg.size() < 2)
    throw Exception("imwrite requires at least a filename and a matrix");
  QString FileName = arg[0].asString();
  QByteArray ImageFormat;
  ImageFormat.append(QFileInfo(FileName).suffix());
  // Construct the QImageWriter object
  QImageWriter imgWriter(FileName,ImageFormat);
  if (!imgWriter.canWrite()) {
    throw Exception("unable to write image file " + FileName);
  }

  Array A(arg[1]);
  if (A.dimensions().lastNotOne() == 2) { // choose QImage::Format_Indexed8
    if (arg.size() == 2) { // 8-bit grayscale image
      Array ctable(UInt8,NTuple(255,1));
      Array trans(UInt8, A.dimensions());
      QImage img = imwriteHelperIndexed(convert2uint8(A), ctable, trans);
      if (!imgWriter.write(QImage(img)))
	throw Exception("cannot create image file" + FileName);
    }
    else if (arg.size() == 3) { // 8-bit indexed color image
      Array ctable(arg[2]);
      if (ctable.length() != 0 && ctable.cols() != 3)
	throw Exception("color map should be a 3 columns matrix");
      Array trans(UInt8,A.dimensions());
      QImage img = imwriteHelperIndexed(convert2uint8(A), convert2uint8(ctable), trans);
      if (!imgWriter.write(img))
	throw Exception("cannot create image file" + FileName);
    }
    else if (arg.size() == 4) { // 8-bit indexed color image with alpha channel
      Array ctable(arg[2]);
      if (ctable.length() != 0 && ctable.cols() != 3)
	throw Exception("color map should be a 3 columns matrix");
      Array trans(arg[3]);
      eval->warningMessage("saving alpha/transparent channel will increase file size");
      QImage img = imwriteHelperIndexed(convert2uint8(A), convert2uint8(ctable), convert2uint8(trans));
      if (!imgWriter.write(img))
	throw Exception("cannot create image file" + FileName);
    }
    else
      throw Exception("invalide input number of arguments");
  }
  else if (A.dimensions().lastNotOne() == 3) { // choose QImage::Format_RGB32 or Format_ARGB32
    if (arg.size() == 2) {
      QImage img = imwriteHelperRGB32(convert2uint8(A));
      if (!imgWriter.write(QImage(img)))
	throw Exception("cannot create image file" + FileName);
    }
    else if (arg.size() == 3) {
      Array trans(arg[2]);
      if (A.rows() == trans.rows() && A.columns() == trans.columns() ) {
	// the third argument is alpha channel
	QImage img = imwriteHelperARGB32(convert2uint8(A), convert2uint8(trans));
	if (!imgWriter.write(QImage(img)))
	  throw Exception("cannot create image file" + FileName);
      }
      else {
	if (trans.length() != 0)
	  eval->warningMessage("ignore colormap argument");
	QImage img = imwriteHelperRGB32(convert2uint8(A));
	if (!imgWriter.write(QImage(img)))
	  throw Exception("cannot create image file" + FileName);
      }
    }
    else if (arg.size() == 4) {
      Array ctable(arg[2]);
      if (ctable.length() != 0)
	eval->warningMessage("ignore colormap argument");
      Array trans(arg[3]);
      if (A.rows() == trans.rows() && A.columns() == trans.columns() ) {
	// the third argument is alpha/transparent channel
	QImage img = imwriteHelperARGB32(convert2uint8(A), convert2uint8(trans));
	if (!imgWriter.write(QImage(img)))
	  throw Exception("cannot create image file" + FileName);
      }
      else {
	if (trans.length() != 0)
	  eval->warningMessage("ignore invalid transparent argument");
	QImage img = imwriteHelperRGB32(convert2uint8(A));
	if (!imgWriter.write(QImage(img)))
	  throw Exception("cannot create image file" + FileName);
      }
    }
    else
      throw Exception("invalide input number of arguments");
  }
  else
    throw Exception("invalid matrix dimension");

  return ArrayVector();
}
