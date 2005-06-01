// Demos a base window class for GTK.  The base window class must be able to:
// Draw itself using double buffering to the screen
// Receive mouse commands
// Support hiding, showing, focus, select, etc.

#include <gtk/gtk.h>
#include <string>
#include <iostream>


class XPWindow {
public:
  GtkWidget *window;
  GdkPixmap *pixmap;
  GtkWidget *drawing_area;
public:
  XPWindow(int width, int height, std::string title);
  virtual ~XPWindow();
  virtual void OnDraw() {}
  virtual void OnResize() {}
  void Show();
  void Hide();
  void Raise();
  void SetTitle(std::string title);
  virtual void OnMouseDown(int x, int y) {}
  virtual void OnMouseDrag(int x, int y);
  virtual void OnMouseUp(int x, int y) {}
  virtual void OnKeyPress(int key) {}
  int GetHeight();
  int GetWidth();
};

void XPWindow::OnMouseDrag(int x, int y) {
  gdk_draw_rectangle(pixmap, window->style->black_gc,
		     TRUE, x-2,y-2,4,4);
  gtk_widget_queue_draw_area(window, x-2, y-2, 4, 4);
}

void XPWindow::Raise() {
  // Don't know this one!
}

void XPWindow::Show() {
  gtk_widget_show(drawing_area);
  gtk_widget_show(window);
}

void XPWindow::Hide() {
  gtk_widget_hide(drawing_area);
  gtk_widget_hide(window);
}

int XPWindow::GetHeight() {
  return window->allocation.height;
}

int XPWindow::GetWidth() {
  return window->allocation.width;
}

static gint expose_event(GtkWidget *widget,
			 GdkEventExpose *event) {
  XPWindow *winptr;
  winptr = (XPWindow*) g_object_get_data(G_OBJECT(widget),"this");
  gdk_draw_drawable (widget->window,
		     widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
		     winptr->pixmap,
		     event->area.x, event->area.y,
		     event->area.x, event->area.y,
		     event->area.width, event->area.height);
  return FALSE;
}


static gint button_press_event(GtkWidget *widget,
			       GdkEventButton *event) {
  XPWindow *winptr;
  winptr = (XPWindow*) g_object_get_data(G_OBJECT(widget),"this");
  if (event->button == 1)
    winptr->OnMouseDown(event->x, event->y);
  return TRUE;
}

static gint button_release_event(GtkWidget *widget,
				 GdkEventButton *event) {
  XPWindow *winptr;
  winptr = (XPWindow*) g_object_get_data(G_OBJECT(widget),"this");
  if (event->button == 1)
    winptr->OnMouseUp(event->x, event->y);
  return TRUE;
}

static gint motion_notify_event(GtkWidget *widget,
				GdkEventMotion *event) {
  if (!(event->state & GDK_BUTTON1_MASK)) return FALSE;
  XPWindow *winptr;
  winptr = (XPWindow*) g_object_get_data(G_OBJECT(widget),"this");
  winptr->OnMouseDrag(event->x, event->y);
  return TRUE;
}

static gint keypress_event(GtkWidget *widget,
			   GdkEventKey *event) {
  XPWindow *winptr;
  winptr = (XPWindow*) g_object_get_data(G_OBJECT(widget),"this");
  winptr->OnKeyPress(event->keyval);
  return TRUE;
}

/* Create a new backing pixmap of the appropriate size */
static gint configure_event( GtkWidget         *widget,
                             GdkEventConfigure *event )
{
  XPWindow *winptr;
  winptr = (XPWindow*) g_object_get_data(G_OBJECT(widget),"this");
  if (winptr->pixmap)
    g_object_unref (winptr->pixmap);
  
  winptr->pixmap = gdk_pixmap_new (widget->window,
				   widget->allocation.width,
				   widget->allocation.height,
				   -1);
  gdk_draw_rectangle (winptr->pixmap,
		      widget->style->white_gc,
		      TRUE,
		      0, 0,
		      widget->allocation.width,
		      widget->allocation.height);
  return TRUE;
}


XPWindow::XPWindow(int width, int height, std::string title) {
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_usize(window, width, height);
  drawing_area = gtk_drawing_area_new();
  gtk_widget_set_size_request(drawing_area,width,height);
  gtk_container_add(GTK_CONTAINER(window),drawing_area);
  g_object_set_data(G_OBJECT(drawing_area),"this",this);
  g_signal_connect (G_OBJECT (drawing_area), "expose_event",
		    G_CALLBACK (expose_event), NULL);
  g_signal_connect (G_OBJECT (drawing_area),"configure_event",
		    G_CALLBACK (configure_event), NULL);
  g_signal_connect (G_OBJECT (drawing_area), "motion_notify_event",
		    G_CALLBACK (motion_notify_event), NULL);
  g_signal_connect (G_OBJECT (drawing_area), "button_press_event",
		    G_CALLBACK (button_press_event), NULL);
  gtk_widget_set_events (drawing_area, GDK_EXPOSURE_MASK
			 | GDK_LEAVE_NOTIFY_MASK
			 | GDK_BUTTON_PRESS_MASK
			 | GDK_POINTER_MOTION_MASK);
  pixmap = NULL;
}

XPWindow::~XPWindow() {
}

int main(int argc, char *argv[]) {
  gtk_init(&argc, &argv);
  XPWindow *win = new XPWindow(400,400,"Hello");
  win->Show();
  gtk_main();
  return 0;
}
