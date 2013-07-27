//////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2007-2013 zSpace, Inc.  All Rights Reserved.
//
//  File:       OpenGlWindow.h
//  Content:    The OpenGlWindow class for the zSpace OpenGl stereo samples.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __ZSPACE_OPEN_GL_WINDOW_H__
#define __ZSPACE_OPEN_GL_WINDOW_H__
#include "stdafx.h"
#include <Windows.h>

#include <map>
#include <string>

class OpenGlWindow
{
public:
  OpenGlWindow(const char* name, int x, int y, int width, int height);
  ~OpenGlWindow();

  int getX() const;
  int getY() const;
  int getWidth() const;
  int getHeight() const;

  HGLRC     getRenderingContext() const;
  HDC       getDeviceContext() const;
  HWND      getWindowHandle() const;
  HINSTANCE getApplicationInstance() const;

  static LRESULT CALLBACK windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
  static int  m_id;
  static std::map<HWND, OpenGlWindow*> m_windowMap;

  std::string m_windowName;
  std::string m_windowClassName;

  int         m_x;
  int         m_y;
  int         m_width;
  int         m_height;

  HGLRC       m_hRC;						// Permanent Rendering Context
  HDC         m_hDC;						// Private GDI Device Context
  HWND        m_hWnd;					  // Holds Our Window Handle
  HINSTANCE   m_hInstance;			// Holds The Instance Of The Application

private:
  void initialize();
  void shutdown();

  LRESULT windowProcInternal(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
};


#endif // __ZSPACE_OPEN_GL_WINDOW_H__