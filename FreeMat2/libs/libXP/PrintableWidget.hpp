#ifndef __PrintableWidget_hpp__
#define __PrintableWidget_hpp__

#include "GraphicsContext.hpp"

class PrintableWidget {
public:
  PrintableWidget(int x, int y, int w, int h);
  virtual void OnDraw(GraphicsContext &gc) = 0;
  virtual int GetHeight();
  virtual int GetWidth();
  void Redraw();
  void GetClick(int &x, int &y);
  void Resize(int x, int y, int w, int h);
};

#endif
