#pragma once

#include "stdafx.h"
#include "ParticleSystem.h"
#include "OpenGlWindow.h"
#include "glsl.h"

class Window {
private:
  time_t timer;
  time_t prevTime;
  OpenGlWindow* g_applicationWindow;
  cwc::glShaderManager SM;
  cwc::glShader *shader;
  void write_img_file(std::string);
  int images;

#ifdef ZSPACE_SDK
  zspace::stereo::StereoWindow* g_stereoWindow;
  zspace::stereo::StereoViewport* g_stereoViewport;
  zspace::stereo::StereoFrustum* g_stereoFrustum;
  zspace::tracker::TrackerSystem* g_trackerSystem;

  bool                    m_isStylusEnabled;
  zspace::common::Matrix4 m_stylusTransform;

  bool                    m_isSecondaryEnabled;
  zspace::common::Matrix4 m_secondaryTransform;

  void renderStylus();

#endif

  glm::vec3 eyePos;
  glm::vec4  pointLightColor;
  glm::vec3  pointLightPos;
  glm::vec3  pointLightRot;
  float pointLightRadius;
  int   v_lastPenPush;
  int   v_isPenActive;

  float stylus[16];

  std::vector<ParticleSystem *> PT;

  int x, y, width, height;
  unsigned long t;


  int KJ;

  void RenderAll();

  void Resize();
  void ReorientCamera();
  void DrawUnitCube();
 

#ifdef ZSPACE_SDK
  void zSpaceInitialize();
  void zSpaceUpdate();
  void zSpaceShutdown();
  void renderFrame();
  void renderScene(zspace::stereo::StereoFrustum::StereoEye eye);
  void setupViewMatrix(zspace::stereo::StereoFrustum::StereoEye eye);
  void setupProjectionMatrix(zspace::stereo::StereoFrustum::StereoEye eye);

  int isStylusButtonPressed();
  void setStylusVisualizationEnabled(bool isStylusEnabled);
  void setStylusTransform(const zspace::common::Matrix4& stylusTransform);

  void setSecondaryVisualizationEnabled(bool isSecondaryEnabled);
  void setSecondaryTransform(const zspace::common::Matrix4& secondaryTransform);
  void convertPoseToWorldSpace(const zspace::common::Matrix4& pose, zspace::common::Matrix4& worldPose);

#endif

public:
  Window();
  void ProgramLoop();
  void AddParticleSystem(ParticleSystem *K);
};