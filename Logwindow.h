#ifndef _LOGWINDOW_H_
#define _LOGWINDOW_H_

typedef struct tagLogWindow
{
   HWND hWnd, hParentWnd;
   HFONT font;
}LogWindow;

void CreateLogWindow(LogWindow *wnd, int x, int y, int width, int height, char *Font);
void ResizeLogWindow(LogWindow *wnd, int x, int y, int width, int height);
void ClearLogWindow(LogWindow *wnd);
void AppendLogWindow(LogWindow *wnd, char *Text);
void RemoveLogWindow(LogWindow *wnd);

#endif /* _LOGWINDOW_H_ */
