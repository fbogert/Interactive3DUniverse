#include "stdafx.h"
#include "Window.h"

#define BITMASK 255

void Window::write_img_file(std::string eye){
	std::string filename = "images/test";
	filename += eye;
	filename += std::to_string(images);
	filename += ".bmp";

   //char * filename = new char[500];
   //char *chars = reinterpret_cast<char*>(&images);
   //strcat(filename,"test");
   //strcat(filename, chars); 
   //strcat(filename,".bmp\0");
   //Needs to be implemented in case the line width is not a multiple of 4!
   // Since width = 1920 for now we are good
   /*
   int line = width;
   unsigned int padding_size = 0;
   while((line&3) > 0) { ++line; ++padding_size; }  //Pad line size to nearest 4th byte
   */
   unsigned int offset = 26;
   unsigned int filesize = offset + (width * height * 3);
   char header[26] = {0x42,0x4d,                                                     // File type, always 4D42h ("BM") 
                      (filesize&BITMASK),            ((filesize&(BITMASK<<8))>>8),   // (0-1)
					  ((filesize&(BITMASK<<16))>>16),((filesize&(BITMASK<<24))>>24), // (2-3) Size of the file in bytes
                      0,   0,                                                        // Reserved 1 - Always 0
                      0,   0,                                                        // Reserved 2 - Always 0
                      0x1A,0,   0,   0,                                              // Bitmapoffset - From beg of file (26)
                      0x0C,0,   0,   0,                                              // Size of this header (12)
                      (width&BITMASK), ((width&(BITMASK<<8))>>8),                    // width
                      (height&BITMASK),((height&(BITMASK<<8))>>8),                   // height
                      0x01,0,                                                        // Color Planes (1)
                      0x18,0           };                                            // Bits per pixel (24)
    
   std::ofstream outFile(filename.c_str(), std::ios_base::binary | std::ios_base::out);
   if(!outFile.good()) throw std::runtime_error("File cannot open"); 
   
 
   char *pixels = new char[filesize - offset];
   //glBindBuffer(GL_PIXEL_PACK_BUFFER,pixels);
   glReadPixels(0,0,width,height,GL_BGR,GL_UNSIGNED_BYTE,pixels);
   //glBindBuffer(GL_PIXEL_PACK_BUFFER,0);
   outFile.write(header, 26);
   outFile.write(pixels,(filesize - offset));
   outFile.close();
   
   //wrote a file increment to next image
   delete pixels;


}

Window::Window() {
	v_lastPenPush = -1;
	v_isPenActive = -1;
	images = 0;
	width = 1920;
	height = 1080;
	x = 0;
	y = 0;
	t = 0;

#ifdef ZSPACE_SDK
	// Get the zSpace display
	zspace::common::DisplayInfo displayInfo;
	int index = 0;
	int numDisplays = displayInfo.getNumDisplays();
	while (index < numDisplays && !displayInfo.getDisplay(index)->isZSpaceDisplay)
		index++;

	// If a zSpace display was found, then position the window on the zSpace
	if (index < numDisplays)
	{
		const zspace::common::DisplayInfo::Display* display = displayInfo.getDisplay(index);
		x = display->displayPosition[0];
		y = display->displayPosition[1];
	}

#endif



	g_applicationWindow = new OpenGlWindow("OpenGL", x, y, width, height);
#ifdef ZSPACE_SDK
	zSpaceInitialize();
#endif

	//OpenGL settings
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND); 
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(1.0);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	//glEnable(GL_MULTISAMPLE);

	/*

	glEnable(GL_FOG);
	float FogCol[3] = {0.0f, 0.0f, 0.0f};
	glFogfv(GL_FOG_COLOR, FogCol);
	glFogi(GL_FOG_MODE, GL_EXP2);
	glFogf(GL_FOG_START, 1.0f);
	glFogf(GL_FOG_END, -1.0f);
	glFogf(GL_FOG_DENSITY, 1.0f);
	*/


#ifdef ZSPACE_SDK
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0f,   0.345f,   0.222f,   // Position
		0.0f,   0.0f,     0.0f,     // Lookat
		0.0f,   1.0f,     0.0f);    // Up 

#else
	ReorientCamera();
#endif



	shader = SM.loadfromFile("shaders/vertexshader.txt", "shaders/fragmentshader.txt");
	//shader = SM.loadfromFile("shaders/vertexshader.txt", "shaders/geomshader.txt", "shaders/fragmentshader.txt");
	if(shader==0){
		std::cout << "Error loading shaders\n";
		exit(0);
	}

	shader->begin();
	int cake = shader->GetUniformLocation("opacity");
	shader->end();
}

void Window::Resize() {
	int x = g_applicationWindow->getX();
	int y = g_applicationWindow->getY();
	int width = g_applicationWindow->getWidth();
	int height = g_applicationWindow->getHeight();

	if(this->x != x || this->y != y || this->width != width || this->height != height){
		//SIZE CHANGE DHUR
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;

		ReorientCamera();
	}
}

void Window::DrawUnitCube() {
	glBegin(GL_LINE_STRIP);
	glVertex3f(1, 1, 1);
	glVertex3f(1, 1, -1);
	glVertex3f(-1, 1, -1);
	glVertex3f(-1, 1, 1);
	glVertex3f(1, 1, 1);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex3f(1, -1, 1);
	glVertex3f(1, -1, -1);
	glVertex3f(-1, -1, -1);
	glVertex3f(-1, -1, 1);
	glVertex3f(1, -1, 1);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(1, 1, 1);
	glVertex3f(1, -1, 1);
	glVertex3f(-1, 1, 1);
	glVertex3f(-1, -1, 1);
	glVertex3f(-1, 1, -1);
	glVertex3f(-1, -1, -1);
	glVertex3f(1, 1, -1);
	glVertex3f(1, -1, -1);
	glEnd();
}

void Window::RenderAll() {
#ifndef ZSPACE_SDK
	wglMakeCurrent(g_applicationWindow->getDeviceContext(), g_applicationWindow->getRenderingContext());
#endif

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(shader) shader->begin();
	/*  BEGIN RENDERING  */

#ifdef ZSPACE_SDK
	if (m_isStylusEnabled)
		renderStylus();
#endif

	GLuint program = shader->GetProgramObject();
	for(int i = 0; i < PT.size(); i += 1){
		//PT[i]->Render(eyePos);
		PT[i]->Render(eyePos, pointLightPos, pointLightRot, v_isPenActive);
	}
	/*  END RENDERING  */
	if(shader) shader->end();

#ifndef ZSPACE_SDK
	SwapBuffers(g_applicationWindow->getDeviceContext());
	glPopMatrix();
#endif

	t = (t + 1) % 1000000;
}

void Window::ProgramLoop() {
	// KM = new HaloMST(&EC);
	KJ = 0;

	MSG msg;
	while (TRUE) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If 'escape' was pressed, exit the application.
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
			PostMessage(g_applicationWindow->getWindowHandle(), WM_DESTROY, 0, 0);
		}



#ifdef ZSPACE_SDK
		zSpaceUpdate();
		renderFrame();


#else
		Resize();
		RenderAll();
#endif

	}
#ifdef ZSPACE_SDK
	zSpaceShutdown();
#endif

	if (g_applicationWindow)
		delete g_applicationWindow;
}

#ifdef ZSPACE_SDK

void Window::renderStylus()
{
	// Convert the stylus world pose to OpenGl matrix format and apply it to
	// the virtual stylus beam's transform.
	GLfloat stylusTransformGl[16];
	zspace::common::MathConverterGl::convertMatrix4ToMatrixGl(m_stylusTransform, stylusTransformGl);
	//glMultMatrixf(stylusTransformGl);
	
	v_isPenActive = isStylusButtonPressed();


	if (v_isPenActive == -1) {
	    //nothing to be done
	}
	else {
		// Get the x,y,z position of the stylus from the viewing matrix
   	    float wscale = 1.0/g_stereoFrustum->getWorldScale();
		
		//pointLightPos.x = -wscale;
		//pointLightPos.y = wscale*(stylusTransformGl[13]/0.5);
	    //pointLightPos.x = wscale*(stylusTransformGl[12]/0.5);
		//pointLightPos.z = wscale*(stylusTransformGl[14]/0.5);
		pointLightPos.x = stylusTransformGl[12];
		pointLightPos.y = stylusTransformGl[13];
		pointLightPos.z = stylusTransformGl[14];

		//pointLightPos *= wscale;

		//calcuate the orientation of the pen
		float thetaS = glm::atan(pointLightPos.y,pointLightPos.z);
	

		//Rotation along x axis -- I think the x axis is up and down on a 2d screen
		float rS = sqrt(pointLightPos.y*pointLightPos.y  + pointLightPos.z*pointLightPos.z);
		//pointLightPos.z = wscale * (rS/* + 0.11*wscale*/)* cos(thetaS);
		//pointLightPos.y = wscale * (rS/* + 0.11*wscale*/) * sin(thetaS);
		//pointLightPos.z -= 0.8;



		//calcuate the orientation of the pen
		float thetaX = glm::atan(stylusTransformGl[9],stylusTransformGl[10]);
		float c2     = glm::sqrt(stylusTransformGl[0]*stylusTransformGl[0]  + stylusTransformGl[4]*stylusTransformGl[4]);
		float thetaY = glm::atan(-stylusTransformGl[8],c2);
		float s1     = glm::sin(thetaX);
		float c1     = glm::cos(thetaX);
		float thetaZ = glm::atan(s1*stylusTransformGl[2] - c1*stylusTransformGl[1], c1*stylusTransformGl[5] - s1*stylusTransformGl[6]);


		//The pen should now be pointing down the z axis
		pointLightRot.x = thetaX;
		pointLightRot.y = thetaY;
		pointLightRot.z = thetaZ;
		//The pens orientation and position are now available to be passed to Vertex
	}

}

int Window::isStylusButtonPressed() {
	int state = -1;
    zspace::tracker::TrackerTarget* primaryTarget = g_trackerSystem->getDefaultTrackerTarget(zspace::tracker::TrackerTarget::TYPE_PRIMARY);
	if (primaryTarget) {
	    // Get the primary tracker target's button capability if it exists.
        zspace::tracker::ITrackerButtonCapability* trackerButtonCapability =
        static_cast<zspace::tracker::ITrackerButtonCapability*>(primaryTarget->getCapability(zspace::tracker::ITrackerCapability::TYPE_BUTTON));

        if (!trackerButtonCapability) {
            printf("Tracker button capability: Not Found.\n");
			return state;
        }

        // Get the number of buttons associated with the stylus.
        unsigned int numButtons = trackerButtonCapability->getNumButtons();

        for (unsigned int buttonId = 0; buttonId < numButtons; ++buttonId) {
            // Check if the specified button is up or down.
            bool isButtonPressed = trackerButtonCapability->isButtonPressed(buttonId);

            // Detect any changes in state.
            if (isButtonPressed) {
				state = buttonId;
			}
        }
	}
	return state;
}

void Window::setStylusVisualizationEnabled(bool isStylusEnabled)
{
	m_isStylusEnabled = isStylusEnabled;
}


void Window::setStylusTransform(const zspace::common::Matrix4& stylusTransform)
{
	m_stylusTransform = stylusTransform;
}


void Window::setSecondaryVisualizationEnabled(bool isSecondaryEnabled)
{
	m_isSecondaryEnabled = isSecondaryEnabled;
}


void Window::setSecondaryTransform(const zspace::common::Matrix4& secondaryTransform)
{
	m_secondaryTransform = secondaryTransform;
}

void Window::zSpaceInitialize()
{
	// zSpace Stereo Initialization.
	g_stereoWindow   = new zspace::stereo::StereoWindow();
	g_stereoViewport = new zspace::stereo::StereoViewport();

	g_stereoWindow->addStereoViewport(g_stereoViewport);
	g_stereoViewport->setUsingWindowSize(true);

	g_stereoFrustum = g_stereoViewport->getStereoFrustum();
	g_stereoFrustum->setInterPupillaryDistance(0.04f);                                                                                                                                                 
	g_stereoFrustum->setFarClip(1.5f);
	g_stereoFrustum->setNearClip(0.01f);
	g_stereoFrustum->setWorldScale(4.0f);

	//g_stereoFrustum->setFieldOfViewScale(0.4);

	 g_stereoFrustum->setStereoLevel(1.0);

	// Initialize zSpace Left/Right Detection.
	zspace::stereo::StereoLeftRightDetect::initialize(g_stereoWindow, zspace::stereo::StereoLeftRightDetect::WINDOW_TYPE_GL);

	// <SampleCode zSpace Tracker Initialization>
	// zSpace Tracker Initialization.
	g_trackerSystem = new zspace::tracker::TrackerSystem();
	// </SampleCode zSpace Tracker Initialization>
}


void Window::zSpaceUpdate()
{
	// Update zSpace Left/Right Frame Detection.
	zspace::stereo::StereoLeftRightDetect::update();

	// Check if the application window has moved.  If so, update the 
	// StereoWindow's position to reflect the application window's new position.
	int x = g_applicationWindow->getX();
	int y = g_applicationWindow->getY();

	if (g_stereoWindow->getX() != x || g_stereoWindow->getY() != y)
		g_stereoWindow->move(x, y);

	// Check if the application window has been resized.  If so, update
	// the zSpace StereoWindow's dimensions to reflect the application 
	// window's new size.
	int width  = g_applicationWindow->getWidth();
	int height = g_applicationWindow->getHeight();

	if (g_stereoWindow->getWidth() != width || g_stereoWindow->getHeight() != height)
	{
		g_stereoWindow->resize(width, height);
		glViewport(0, 0, width, height);
	}

	// Update the tracking information.
	g_trackerSystem->captureTargets();

	// <SampleCode zSpace Head TrackerTarget>
	// Grab the latest 6DOF head pose (position and orientation) and pass it off
	// to the zSpace StereoFrustum.  This allows the StereoFrustum to take the head 
	// position/rotation into account when calculating the left and right eye
	// view/projection matrices.
	zspace::tracker::TrackerTarget* headTarget = g_trackerSystem->getDefaultTrackerTarget(zspace::tracker::TrackerTarget::TYPE_HEAD);

	// Check to make sure the head target is not NULL.  If the head target is NULL,
	// this signifies that the default head target cannot be found.
	if (headTarget) {
		zspace::common::Matrix4 headPose;
		headPose = zspace::common::Matrix4::IDENTITY();
		headTarget->getPose(headPose);

		zspace::common::Vector3 pos;

		pos = g_stereoFrustum->getEyePosition(zspace::stereo::StereoFrustum::STEREO_EYE_RIGHT);
		float wscale = g_stereoFrustum->getWorldScale();
		eyePos.x = pos.x/wscale;
		eyePos.y = pos.y/wscale;
		eyePos.z = pos.z/wscale;


		g_stereoFrustum->setHeadPose(headPose);

	}

	zspace::tracker::TrackerTarget* primaryTarget = g_trackerSystem->getDefaultTrackerTarget(zspace::tracker::TrackerTarget::TYPE_PRIMARY);
	if (primaryTarget) {
		zspace::common::Matrix4 primaryPose      = zspace::common::Matrix4::IDENTITY();
		zspace::common::Matrix4 primaryWorldPose = zspace::common::Matrix4::IDENTITY();

		primaryTarget->getPose(primaryPose);

		zspace::common::Vector3 pos = zspace::common::Vector3::ZERO();
	    zspace::common::Vector3 scale = zspace::common::Vector3::ZERO();
	    zspace::common::Quaternion ori = zspace::common::Quaternion::ZERO();
	    primaryPose.decomposition(pos,scale,ori);

	
	float wscale = g_stereoFrustum->getWorldScale();
	//pos.y *= wscale;
	//pos.z *= wscale;
	//pos.x *= wscale;
	pos *= wscale;
	scale.x *= wscale;
	scale.y *= wscale;
	scale.z *= wscale;
	//scale *= wscale;
	ori.w *= wscale;
	ori.x *= wscale;
	ori.y *= wscale;
	ori.z *= wscale;

	primaryPose = zspace::common::Matrix4(ori);
	primaryPose.setScale(scale);
	primaryPose.setTrans(pos);
	

		convertPoseToWorldSpace(primaryPose, primaryWorldPose);

		setStylusTransform(primaryWorldPose);
	}
}


void Window::zSpaceShutdown()
{
	// Shutdown zSpace Left/Right Frame Detection.
	zspace::stereo::StereoLeftRightDetect::shutdown();

	// Remove references to any zSpace objects that have been allocated.
	if (g_stereoWindow)
		g_stereoWindow->removeReference();

	if (g_stereoViewport)
		g_stereoViewport->removeReference();

	// <SampleCode zSpace Tracker Shutdown>
	if (g_trackerSystem)
		g_trackerSystem->removeReference();
	// </SampleCode zSpace Tracker Shutdown>
}

void Window::convertPoseToWorldSpace(const zspace::common::Matrix4& pose, zspace::common::Matrix4& worldPose)
{
	GLfloat monoModelViewGl[16];
	zspace::common::Matrix4 cameraLocalToWorld   = zspace::common::Matrix4::IDENTITY();
	zspace::common::Matrix4 trackerToCameraSpace = zspace::common::Matrix4::IDENTITY();
	zspace::common::Vector3 viewportOffset       = zspace::common::Vector3::ZERO();

	// Grab the mono (non-stereoscopic) model-view matrix in OpenGl format.
	glMatrixMode(GL_MODELVIEW);
	glGetFloatv(GL_MODELVIEW_MATRIX, monoModelViewGl);

	// Convert the OpenGl model-view matrix to the zSpace Matrix4 format and
	// invert it to obtain the non-stereo camera's matrix.
	zspace::common::MathConverterGl::convertMatrixGlToMatrix4(monoModelViewGl, cameraLocalToWorld);
	cameraLocalToWorld = cameraLocalToWorld.inverse();

	// Grab the display that the zSpace StereoWindow is currently on in order to 
	// calculate the tracker-to-camera space transform as well as the viewport offset.
	// Both the tracker-to-camera space transform and viewport offset are required
	// for transforming a tracker space pose into the application's world space.
	const zspace::common::DisplayInfo::Display* display = g_stereoWindow->getCurrentDisplay();

	if (display)
	{
		trackerToCameraSpace = zspace::common::DisplayInfo::getTrackerToCameraSpaceTransform(display);
		viewportOffset       = zspace::common::DisplayInfo::getViewportOffset(display,
			g_stereoWindow->getX(),
			g_stereoWindow->getY(),
			g_stereoWindow->getWidth(),
			g_stereoWindow->getHeight());
	}

	zspace::common::Vector3 pos = zspace::common::Vector3::ZERO();
	zspace::common::Vector3 scale = zspace::common::Vector3::ZERO();
	zspace::common::Quaternion ori = zspace::common::Quaternion::ZERO();
	cameraLocalToWorld.decomposition(pos,scale,ori);

	/*
	float wscale = g_stereoFrustum->getWorldScale();
	//pos.y *= wscale;
	//pos.z *= wscale;
	//pos.x *= wscale;
	pos *= wscale;
	scale.x *= wscale;
	scale.y *= wscale;
	scale.z *= wscale;
	//scale *= wscale;
	ori.w *= wscale;
	ori.x *= wscale;
	ori.y *= wscale;
	ori.z *= wscale;

	cameraLocalToWorld = zspace::common::Matrix4(ori);
	cameraLocalToWorld.setScale(scale);
	cameraLocalToWorld.setTrans(pos);
	*/
	worldPose = cameraLocalToWorld * trackerToCameraSpace * zspace::common::Matrix4::getTrans(-viewportOffset) * pose;
}


void Window::renderFrame()
{
	// Set the application window's rendering context as the current rendering context.
	wglMakeCurrent(g_applicationWindow->getDeviceContext(), g_applicationWindow->getRenderingContext());

	// Orbit the monoscopic camera about the center of the scene.
	//OpenGlScene::rotateScene();
	bool pressed = false;
	time(&timer);
	if(v_isPenActive == 0 && v_lastPenPush != v_isPenActive){
		prevTime = timer;
	}
	if(v_isPenActive == 0 && (((unsigned int)(difftime(timer,prevTime)*100))%60) == 0){
		//write_img_file();
		pressed = true;
	}
	v_lastPenPush = v_isPenActive;

	// Render the scene for each eye.
	renderScene(zspace::stereo::StereoFrustum::STEREO_EYE_LEFT);
	//if(pressed) write_img_file("left");
	renderScene(zspace::stereo::StereoFrustum::STEREO_EYE_RIGHT);
	//if(pressed) { write_img_file("right"); ++images; }

	// Flush the render buffers.
	SwapBuffers(g_applicationWindow->getDeviceContext());
}

void Window::renderScene(zspace::stereo::StereoFrustum::StereoEye eye)
{
	// Push the stereo view and projection matrices onto the OpenGl matrix stack
	// so that we can pop them off after we're done rendering the scene
	// for a specified eye.  This will allow us to restore the mono (non-
	// stereoscopic) model-view and projection matrices.
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	// Set up the view and projection matrices for the specified eye.
	setupViewMatrix(eye);
	setupProjectionMatrix(eye);

	// Select appropriate back buffer to render to based on the specified eye.
	if (eye == zspace::stereo::StereoFrustum::STEREO_EYE_LEFT)
		glDrawBuffer(GL_BACK_LEFT);
	else if (eye == zspace::stereo::StereoFrustum::STEREO_EYE_RIGHT)
		glDrawBuffer(GL_BACK_RIGHT);

	// Render the scene.
	//OpenGlScene::render();
	RenderAll();

	// Restore the mono (non-stereoscopic) model-view and projection matrices.
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}
void Window::setupViewMatrix(zspace::stereo::StereoFrustum::StereoEye eye)
{
	GLfloat viewMatrixGl[16];
	GLfloat monoModelViewGl[16];

	// Get the view matrix from the zSpace StereoFrustum for a specified eye
	// and convert it into OpenGl matrix format.
	zspace::common::Matrix4 viewMatrix;
	g_stereoFrustum->getViewMatrix(eye, viewMatrix);
	zspace::common::MathConverterGl::convertMatrix4ToMatrixGl(viewMatrix, viewMatrixGl);

	// Set the OpenGl MatrixMode to GL_MODELVIEW and get the mono (non-stereoscopic) 
	// model-view matrix.
	glMatrixMode(GL_MODELVIEW);
	glGetFloatv(GL_MODELVIEW_MATRIX, monoModelViewGl);

	// Set the model-view matrix for the specified eye and multiply it by the
	// mono (non-stereoscopic) model-view matrix.  This must be done because the
	// eye's view matrix only contains the eye offset (relative to the center of
	// the viewer's head) plus any rotation required for off-axis projection.
	glLoadMatrixf(viewMatrixGl);
	glMultMatrixf(monoModelViewGl);
}


void Window::setupProjectionMatrix(zspace::stereo::StereoFrustum::StereoEye eye)
{
	GLfloat projectionMatrixGl[16];

	// Get the projection matrix from the zSpace StereoFrustum for a specified eye
	// and convert it into OpenGl matrix format.
	zspace::common::Matrix4 projectionMatrix;
	g_stereoFrustum->getProjectionMatrix(eye, projectionMatrix);
	zspace::common::MathConverterGl::convertMatrix4ToMatrixGl(projectionMatrix, projectionMatrixGl);

	// Set OpenGl MatrixMode to GL_PROJECTION and set the projection matrix.
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(projectionMatrixGl);
}

#endif

/*
void Window::AddEllipsoid(Ellipsoid *K) {
K->InsetParameters();
EC.push_back(K);
}
*/

void Window::AddParticleSystem(ParticleSystem *K) {
	//PS = K;
	PT.push_back(K);
}

void Window::ReorientCamera() {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, ((float)width/(float)height), 0.01, 2.0);
	//glOrtho(-largestGenerated, largestGenerated, -largestGenerated, largestGenerated, 0.1, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(-0.01, -0.01, -0.01, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

