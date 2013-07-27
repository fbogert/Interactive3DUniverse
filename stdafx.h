#pragma once

#define ZSPACE_SDK

#include <Windows.h>

#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include <GL/glew.h>

#pragma comment(lib, "glew32d.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_access.hpp"

#define _USE_MATH_DEFINES
#include <math.h>

#include <time.h>

#ifdef ZSPACE_SDK

#pragma comment(lib, "ZSCommon.lib")
#pragma comment(lib, "ZSCoreInterface.lib")
#pragma comment(lib, "ZSStereo.lib")
#pragma comment(lib, "ZSTracker.lib")

#include "ZSpace/Common/Math/MathConverterGl.h"
#include "ZSpace/Common/Math/Matrix4.h"

#include "ZSpace/Stereo/StereoFrustum.h"
#include "ZSpace/Stereo/StereoLeftRightDetect.h"
#include "ZSpace/Stereo/StereoViewport.h"
#include "ZSpace/Stereo/StereoWindow.h"

#include "ZSpace/Tracker/TrackerSystem.h"
#include "ZSpace/Tracker/TrackerTarget.h"
#include "ZSpace/Tracker/ITrackerButtonCapability.h"
#endif