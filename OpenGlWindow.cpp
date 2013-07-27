//////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2007-2013 zSpace, Inc.  All Rights Reserved.
//
//  File:       OpenGlWindow.h
//  Content:    The OpenGlWindow class for the zSpace OpenGl stereo samples.
//
//////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "OpenGlWindow.h"

#include <gl/GL.h>
#include <sstream>

int OpenGlWindow::m_id = 0;
std::map<HWND, OpenGlWindow*> OpenGlWindow::m_windowMap;

OpenGlWindow::OpenGlWindow(const char* name, int x, int y, int width, int height)
: m_windowName(name)
, m_windowClassName("OpenGlStereoWindowClass")
, m_x(x)
, m_y(y)
, m_width(width)
, m_height(height)
, m_hRC(NULL)
, m_hDC(NULL)
, m_hWnd(NULL)
, m_hInstance(NULL)
{
  // Append a unique id to the window class name.
  std::stringstream stringStream;
  stringStream << m_id++;
  m_windowClassName += stringStream.str();

  initialize();
}


OpenGlWindow::~OpenGlWindow()
{
  shutdown();
}


int OpenGlWindow::getX() const
{
  return m_x;
}


int OpenGlWindow::getY() const
{
  return m_y;
}


int OpenGlWindow::getWidth() const
{
  return m_width;
}


int OpenGlWindow::getHeight() const
{
  return m_height;
}


HGLRC OpenGlWindow::getRenderingContext() const
{
  return m_hRC;
}


HDC OpenGlWindow::getDeviceContext() const
{
  return m_hDC;
}


HWND OpenGlWindow::getWindowHandle() const
{
  return m_hWnd;
}


HINSTANCE OpenGlWindow::getApplicationInstance() const
{
  return m_hInstance;
}


LRESULT CALLBACK OpenGlWindow::windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  OpenGlWindow* openGlWindow = m_windowMap[hWnd];

  // Call the window proc for the OpenGlWindow if valid.
  if (openGlWindow) 
    return openGlWindow->windowProcInternal(hWnd, message, wParam, lParam); 

  // If the OpenGlWindow is not valid, call the default window proc.
  return DefWindowProc(hWnd, message, wParam, lParam);
}


void OpenGlWindow::initialize()
{
  GLuint		pixelFormat;                              // Holds The Results After Searching For A Match
  WNDCLASS	windowClass;                              // Windows Class Structure

  m_hInstance	= GetModuleHandle(NULL);                // Grab An Instance For Our Window
  
  ZeroMemory(&windowClass, sizeof(WNDCLASS));
  windowClass.style			    = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	  // Redraw On Move, And Own DC For Window
  windowClass.lpfnWndProc		= (WNDPROC)&OpenGlWindow::windowProc;   // WindowProc Handles Messages
  windowClass.cbClsExtra		= 0;									                  // No Extra Window Data
  windowClass.cbWndExtra		= 0;                                    // No Extra Window Data
  windowClass.hInstance		  = m_hInstance;                          // Set The Instance
  windowClass.hCursor			  = LoadCursor(NULL, IDC_ARROW);          // Load The Arrow Pointer
  windowClass.hbrBackground	= NULL;                                 // No Background Required For GL
  windowClass.lpszMenuName	= NULL;                                 // We Don't Want A Menu
  windowClass.lpszClassName	= m_windowClassName.c_str();            // Set The Class Name
  windowClass.hIcon         = (HICON)LoadImage(NULL, "Infinite_Z.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE);

  // Attempt To Register The Window Class
  if (!RegisterClass(&windowClass))
  {
    MessageBox(NULL, "Failed To Register The Window Class.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
    return;
  }

  m_hWnd = CreateWindowEx(NULL,                               // Extended Style For The Window
                          m_windowClassName.c_str(),          // Class Name
                          m_windowName.c_str(),               // Window Title
                          WS_OVERLAPPEDWINDOW |
                          WS_CLIPCHILDREN     |
                          WS_CLIPSIBLINGS,
                          m_x, m_y,                           // Window Position
                          m_width,                            // Window Width
                          m_height,                           // Window Height
                          NULL,                               // No Parent Window
                          NULL,                               // No Menu
                          m_hInstance,                        // Instance
                          NULL);

  if (!m_hWnd)								        
  {
    shutdown();											      
    MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
    return;
  }

  // <SampleCode PixelFormatDescriptor Setup>
  // Set the appropriate pixel format.
  PIXELFORMATDESCRIPTOR pfd =					
  {
    sizeof(PIXELFORMATDESCRIPTOR),            // Size Of This Pixel Format Descriptor
    1,                                        // Version Number
    PFD_DRAW_TO_WINDOW |                      // Format Must Support Window
    PFD_SUPPORT_OPENGL |                      // Format Must Support OpenGL
    PFD_STEREO         |                      // Format Must Support Quad-buffer Stereo
    PFD_DOUBLEBUFFER,                         // Must Support Double Buffering
    PFD_TYPE_RGBA,                            // Request An RGBA Format
    24,                                       // 24-bit color depth
    0, 0, 0, 0, 0, 0,                         // Color Bits Ignored
    0,                                        // No Alpha Buffer
    0,                                        // Shift Bit Ignored
    0,                                        // No Accumulation Buffer
    0, 0, 0, 0,                               // Accumulation Bits Ignored
    32,                                       // 32-bit Z-Buffer (Depth Buffer)
    0,                                        // No Stencil Buffer
    0,                                        // No Auxiliary Buffer
    PFD_MAIN_PLANE,                           // Main Drawing Layer
    0,                                        // Reserved
    0, 0, 0                                   // Layer Masks Ignored
  };
  // </SampleCode PixelFormatDescriptor Setup>

  // Did We Get A Device Context?
  if (!(m_hDC = GetDC(m_hWnd)))								
  {
    shutdown();											      
    MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
    return;
  }

  // Did Windows Find A Matching Pixel Format?
  if (!(pixelFormat = ChoosePixelFormat(m_hDC, &pfd)))	
  {
    shutdown();											     
    MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
    return;
  }

  // Are We Able To Set The Pixel Format?
  if (!SetPixelFormat(m_hDC, pixelFormat, &pfd))	
  {
    shutdown();											    
    MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
    return;
  }

  // Are We Able To Get A Rendering Context?
  if (!(m_hRC = wglCreateContext(m_hDC)))			
  {
    shutdown();											      
    MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK|MB_ICONEXCLAMATION);
    return;
  }

  // Try To Activate The Rendering Context.
  if (!wglMakeCurrent(m_hDC, m_hRC))								
  {
    shutdown();											     
    MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
    return;
  }

  ShowWindow(m_hWnd, SW_SHOW);                // Show The Window
  SetForegroundWindow(m_hWnd);                // Slightly Higher Priority
  SetFocus(m_hWnd);                           // Sets Keyboard Focus To The Window

  // Add this OpenGlWindow to the window map so that we can access the correct
  // OpenGlWindow (based on hWnd) from the static windowProc method.
  m_windowMap[m_hWnd] = this;
}


void OpenGlWindow::shutdown()
{
  // Do We Have A Rendering Context?
  if (m_hRC)													          
  {
    // Are We Able To Release The DC And RC Contexts?
    if (!wglMakeCurrent(NULL, NULL))						
      MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);

    // Are We Able To Delete The RC?
    if (!wglDeleteContext(m_hRC))								
      MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
    
    m_hRC = NULL;												        
  }

  // Are We Able To Release The DC?
  if (m_hDC)				
  {
    if (!ReleaseDC(m_hWnd, m_hDC))
      MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
    
    m_hDC = NULL;
  }

  // Are We Able To Destroy The Window?
  if (m_hWnd)					
  {
    if (!DestroyWindow(m_hWnd))
      MessageBox(NULL, "Could Not Release hWnd.","SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
      									      
    m_hWnd = NULL;	
  }

  // Are We Able To Unregister Class?
  if (m_hInstance)
  {
    if (!UnregisterClass(m_windowClassName.c_str(), m_hInstance))
      MessageBox(NULL, "Could Not Unregister Class.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
    
    m_hInstance = NULL;
  }
}


LRESULT OpenGlWindow::windowProcInternal(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
  case WM_CLOSE:
    {
      shutdown();
      return 0;
    }
  case WM_DESTROY:
    {
      PostQuitMessage(0);
      return 0;
    } 
  case WM_SHOWWINDOW:
  case WM_SIZE:
    {
      RECT rect;
      if (GetClientRect(hWnd, &rect))
      {
        m_width = rect.right-rect.left;
        m_height = rect.bottom-rect.top;
      }
      return 0;
    } 
  case WM_MOVE:
    {
      RECT rect;
      if (GetClientRect(hWnd, &rect))
      {
        m_width = rect.right-rect.left;
        m_height = rect.bottom-rect.top;
      }
      m_x = (int)(short)LOWORD(lParam);
      m_y = (int)(short)HIWORD(lParam);
      return 0;
    }
  case WM_KEYDOWN:
    {
      return 0;
    }
  }

  return DefWindowProc(hWnd, message, wParam, lParam);
}
