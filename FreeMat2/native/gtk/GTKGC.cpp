#include "GTKGC.hpp"


Point2D GraphicsContext::GetCanvasSize() {
  return Point2D(m_width,m_height);
}

void GraphicsContext::DrawLine(Point2D pos1, Point2D pos2) {
  gdk_draw_line(pixmap,gc,pos1.x,pos1.y,pos2.x,pos2.y);
}

void GraphicsContext::DrawPoint(Point2D pos) {
  gdk_draw_point(pixmap,gc,pos1.x,pos1.y);
}

void GraphicsContext::DrawCircle(Point2D pos, int radius) {
  gdk_draw_arc(pixmap,gc,FALSE,pos.x-radius,pos.y-radius,radius*2,radius*2,0,360*64);
}

void GraphicsContext::DrawRectangle(Rect2D rect) {
  gdk_draw_rectangle(pixmap,gc,FALSE,rect.x1,rect.y1,rect.width,rect.height);
}

void GraphicsContext::FillRectangle(Rect2D rect) {
  gdk_draw_rectangle(pixmap,gc,TRUE,rect.x1,rect.y1,rect.width,rect.height);  
}

void GraphicsContext::FillQuad(Point2D p1, Point2D p2, Point2D p3, Point2D p4) {
  GdkPoint pt[4];
  pt[0].x = p1.x; pt[0].y = p1.y;
  pt[1].x = p2.x; pt[1].y = p2.y;
  pt[2].x = p3.x; pt[2].y = p3.y;
  pt[3].x = p4.x; pt[3].y = p4.y;
  gdk_draw_polygon(pixmap,gc,TRUE,pt,4);
}

void GraphicsContext::DrawQuad(Point2D p1, Point2D p2, Point2D p3, Point2D p4) {
  GdkPoint pt[4];
  pt[0].x = p1.x; pt[0].y = p1.y;
  pt[1].x = p2.x; pt[1].y = p2.y;
  pt[2].x = p3.x; pt[2].y = p3.y;
  pt[3].x = p4.x; pt[3].y = p4.y;
  gdk_draw_polygon(pixmap,gc,FALSE,pt,4);
}

void GraphicsContext::DrawLines(std::vector<Point2D> pts) {
  GdkPoint *gpts = new GdkPoint[pts.size()];
  for (int i=0;i<pts.size();i++) {
    gpts[i].x = pts[i].x;
    gpts[i].y = pts[i].y;
  }
  gdk_draw_lines(pixmap,gc,gpts,pts.size());
}

void GraphicsContext::PushClippingRegion(Rect2D rect) {
  clips.push_back(rect);
  GdkRectangle grect;
  grect.x = rect.x1;
  grect.y = rect.y1;
  grect.width = rect.width;
  grect.height = rect.height;
  gdk_gc_set_clip_mask(gc,&grect);
}

Rect2D GraphicsContext::PopClippingRegion() {
  Rect2D ret(clips.back());
  clips.pop_back();
  if (!clips.empty()) {
    GdkRectangle grect;
    grect.x = rect.x1;
    grect.y = rect.y1;
    grect.width = rect.width;
    grect.height = rect.height;
    gdk_gc_set_clip_mask(gc,&grect);
  }
  return ret; 
}

Color GTKGC::SetBackgroundColor(Color col) {
  Color ret(m_bg);
  m_bg = col;
  GdkColor gcol;
  gcol.red = col.red << 8;
  gcol.green = col.green << 8;
  gcol.blue = col.blue << 8;
  gdk_gc_set_rgb_bg_color(gc,&gcol);
  return ret;
}

Color GTKGC::SetForegroundColor(Color col) {
  Color ret(m_fg);
  m_fg = col;
  GdkColor gcol;
  gcol.red = col.red << 8;
  gcol.green = col.green << 8;
  gcol.blue = col.blue << 8;
  gdk_gc_set_rgb_fg_color(gc,&gcol);
  return ret;
}

void GTKGC::SetFont(int fontsize) {
  if (myFont)
    pango_font_description_free(myFont);
  myFont = pango_font_description_new();
  pango_font_description_set_family( myFont, "sans" );
  pango_font_description_set_size( myFont, size*PANGO_SCALE);
  pango_layout_set_font_description( pango, myFont);
}

Point2D GTKGC::GetTextExtent(std::string label) {
  pango_layout_set_text(pango, label.c_str(), label.size());
  int w, h;
  pango_layout_get_pixel_size(pango, &w, &h);
  return Point2D(w,h);
}

void GTKGC::DrawTextString(std::string label, Point2D pos, OrientationType orient) {
  
}

LineStyleType GTKGC::SetLineStyle(LineStyleType style) {
  LineStyleType ret(m_lst);
  m_lst = style;
  static unsigned char line_dashed_list[2] = {4, 4};
  static unsigned char line_dotted_list[2] = {3, 1};
  static unsigned char line_dash_dot_list[4] = {3, 4, 3, 1};
  switch (ls) {
  case LINE_SOLID:
    gdk_gc_set_line_attributes(gc, 1, GDK_LINE_SOLID, GDK_CAP_BUTT, GDK_JOIN_MITER);
    break;
  case LINE_DASHED:
    gdk_gc_set_line_attributes(gc, 1, GDK_LINE_ON_OFF_DASH, GDK_CAP_BUTT, GDK_JOIN_MITER);
    gdk_gc_set_dashes(gc, 0, (const char*) line_dashed_list, 2);
    break;
  case LINE_DOTTED:
    gdk_gc_set_line_attributes(gc, 1, GDK_LINE_ON_OFF_DASH, GDK_CAP_BUTT, GDK_JOIN_MITER);
    gdk_gc_set_dashes(gc, 0, (const char*) line_dotted_list, 2);
    break;
  case LINE_DASH_DOT:
    gdk_gc_set_line_attributes(gc, 1, GDK_LINE_ON_OFF_DASH, GDK_CAP_BUTT, GDK_JOIN_MITER);
    gdk_gc_set_dashes(gc, 0, (const char*) line_dash_dot_list, 4);
    break;
  }
  return ret;
}
