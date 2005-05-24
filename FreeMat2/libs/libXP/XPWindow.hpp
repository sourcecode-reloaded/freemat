#ifndef __XPWindow_hpp__
#define __XPWindow_hpp__

#include <string>
#include "PrintableWidget.hpp"

namespace FreeMat {
  class XPWindow {
  public:
    XPWindow();
    ~XPWindow();
    XPWindow(int width, int height, std::string title);
    int GetWidth();
    int GetHeight();
    void Redraw();
    void Hide();
    void Show();
    void Title(std::string title);
    void Resize(int width, int height);
    void AddWidget(PrintableWidget *widget);
    void Print(std::string filename);
    void OnMouseDown(int x, int y);
    void OnDrag(int x, int y);
  };

  void SaveFocus();
  void RestoreFocus();
}

#endif
