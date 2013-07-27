
#include "stdafx.h"
#include "ParticleSystem.h"

#define BUFFER_OFFSET(i) ((void*)(i))



ParticleSystem::ParticleSystem(const char *d, unsigned long s){
  LoadMemory(d, s);
}

ParticleSystem::ParticleSystem(const char *filename) {
  std::ifstream FP(filename, std::ios_base::binary | std::ios_base::in);
  if(!FP.good()){
    std::cout << "error opening file\n";
    exit(0);
  }

  FP.seekg(0, FP.end);
  unsigned long len = FP.tellg();
  FP.seekg(0, FP.beg);



  unsigned long vertCount = len / (sizeof(float) * 4 + sizeof(int));


  char *buffer = new char[len+1];
  if(!buffer){
    exit(0);
  }else{
    FP.read(buffer, len);
  }

  FP.close();

  // File reading done
  LoadMemory(buffer, vertCount);

  delete buffer;
}

void ParticleSystem::quickSort(interleaved arr[], unsigned long left, unsigned long right) {
  unsigned long i = left, j = right;

  interleaved tmp;
  interleaved pivot = arr[(left + right) / 2];
  while (i <= j) {
    while (arr[i].m < pivot.m)
      i++;
    while (arr[j].m > pivot.m)
      j--;
    if (i <= j) {
      tmp = arr[i];
      arr[i] = arr[j];
      arr[j] = tmp;
      i++;
      j--;
    }
  };
  if (left < j)
    quickSort(arr, left, j);

  if (i < right)
    quickSort(arr, i, right);
}

void ParticleSystem::Connect(unsigned long i, unsigned long j) {
  connect.push_back(i);
  connect.push_back(j);
}



void ParticleSystem::LoadMemory(const char *d, unsigned long s){
	penScale = 1.0;
	saveScale = 1.0;
	lastPenPush = -1;
  // SIZE is number of vertices
  // MASSCOUNT 
  size = s;

  size = 500000;

  // Init calculations
  maxmass = 0;
  minmass = 1e99;
  minpos = glm::vec3(1e99, 1e99, 1e99);
  maxpos = glm::vec3(-1e99, -1e99, -1e99);

  ibuf = new interleaved[size];

  // Perform the copy operations and calculations
  const char *ptr = d;
  float buffer[4];
  int bufferint[4];
  //glm::vec4 tmpColor;
  for(unsigned long i = 0; i < size; i++){
    memcpy(buffer, ptr, sizeof(float) * 4);
    ptr += sizeof(float) * 4;

    ibuf[i].x = buffer[0];
    ibuf[i].y = buffer[1];
    ibuf[i].z = buffer[2];
    ibuf[i].m = buffer[3];
	//ibuf[i].type  = 1;
	
	memcpy(bufferint, ptr, sizeof(int));
	ptr += sizeof(int);

	ibuf[i].type = bufferint[0];

    // Update our min and max values
    if(ibuf[i].x > maxpos.x) maxpos.x = ibuf[i].x;
    if(ibuf[i].y > maxpos.y) maxpos.y = ibuf[i].y;
    if(ibuf[i].z > maxpos.z) maxpos.z = ibuf[i].z;
    if(ibuf[i].m > maxmass)  maxmass  = ibuf[i].m;

    if(ibuf[i].x < minpos.x) minpos.x = ibuf[i].x;
    if(ibuf[i].y < minpos.y) minpos.y = ibuf[i].y;
    if(ibuf[i].z < minpos.z) minpos.z = ibuf[i].z;
    if(ibuf[i].m < minmass)  minmass  = ibuf[i].m;
  }

  // Sort the buffer
  //quickSort(ibuf, 0, size - 1);

  // Evaluate colors and scale positions
  float sFactor = (2.0 / maxpos.x) * 1.00;
  float loc;
  for(unsigned long i = 0; i < size; i++){
    

    // Scale
    ibuf[i].x *= sFactor;   ibuf[i].y *= sFactor;   ibuf[i].z *= sFactor;

    ibuf[i].x -= 1.0;
    ibuf[i].y -= 1.0;
    ibuf[i].z -= 1.0;

    // Normals
    ibuf[i].nx = ibuf[i].nx;
    ibuf[i].ny = ibuf[i].ny;
    ibuf[i].nz = ibuf[i].nz;

    // Color
    loc = (float)i / size;
    ibuf[i].r = 1.0-loc;
    ibuf[i].g = 0.0;
    ibuf[i].b = loc;
    ibuf[i].a = 1.0;

  // if(i % 1000 == 1){
  //    Connect(i - i, i);
  //  }

  }

  glGenBuffers(1, &vboID[0]);
  glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(interleaved) * size, ibuf, GL_DYNAMIC_DRAW);



}



ParticleSystem::~ParticleSystem() {
  glDeleteBuffers(1, &vboID[0]);
  // glDeleteVertexArrays(3, &vaoID[0]);
  delete ibuf;
  //delete data;
}

int ParticleSystem::GetSize() {
  return size;
}

glm::vec3 ParticleSystem::GetPosition(unsigned long i) {
  return glm::vec3(ibuf[i].x, ibuf[i].y, ibuf[i].z);
}

void ParticleSystem::SetPosition(unsigned long i, glm::vec3 &pos) {
  ibuf[i].x = pos.x;
  ibuf[i].y = pos.y;
  ibuf[i].z = pos.z;
}

double ParticleSystem::GetMass(unsigned long i) {
  return ibuf[i].m;
}

void ParticleSystem::Render(glm::vec3 pos, glm::vec3 lightPos, glm::vec3 lightRot, int activatePen) {
  GLint ID    = glGetUniformLocation(1, "opacity");
  GLint IDp   = glGetUniformLocation(1, "eyePos");
  GLint IDpl  = glGetUniformLocation(1, "penLight");
  GLint IDpm  = glGetUniformLocation(1, "penMatrix");
//  GLint ID    = glGetUniformLocation(1, "opa");

  GLint IDattrib_type = glGetAttribLocation(1, "type");
  GLint IDattrib_mass = glGetAttribLocation(1, "mass");


  /*
  if (activatePen == 1 && lastPenPush == -1) {
	  penScaleSavePos = lightPos;
	  penScaleSaveRot = lightRot;
  }

  float op = 0.2;
    if (activatePen == 1 && lastPenPush == 1) {
	  penScaleSavePos = lightPos;
	  penScaleSaveRot = lightRot;
	  op = sqrt(pow(penScaleSavePos.x - lightPos.x,2) + pow(penScaleSavePos.y - lightPos.y,2) + pow(penScaleSavePos.z - lightPos.z,2));
  }

	lastPenPush = activatePen;
*/
  float Pl[7];
  Pl[0] = activatePen;
  Pl[1] = lightPos.x;
  Pl[2] = lightPos.y;
  Pl[3] = lightPos.z;
  Pl[4] = lightRot.x;
  Pl[5] = lightRot.y;
  Pl[6] = lightRot.z;




  float W[3];
  W[0] = pos.x;
  W[1] = pos.y;
  W[2] = pos.z;

  float scale = 1.0/sqrt(pow(W[0] - lightPos.x,2) + pow(W[1] - lightPos.y,2) + pow(W[2] - lightPos.z,2));
  if (scale < 0.1) scale = 0.1;
  
  if (activatePen == 0) {
	  penScale = scale*saveScale;
	  penSavePos = lightPos;
	  penSaveRot = lightRot;
  }
  else {
	  saveScale = penScale;
  }


  float Pm[7];
  Pm[0] = penScale;
  Pm[1] = penSavePos.x;
  Pm[2] = penSavePos.y;
  Pm[3] = penSavePos.z;
  Pm[4] = penSaveRot.x;
  Pm[5] = penSaveRot.y;
  Pm[6] = penSaveRot.z;

  for(int i = 0; i < size; i++){
	  ibuf[i].x *= 1.001;
	  ibuf[i].z *= 1.001;
ibuf[i].y *= 1.001;

  }

  glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, sizeof(interleaved), BUFFER_OFFSET(0));
  glEnableClientState(GL_COLOR_ARRAY);
  glColorPointer(4, GL_FLOAT, sizeof(interleaved), BUFFER_OFFSET(12));
  glEnableClientState(GL_NORMAL_ARRAY);
  glNormalPointer(GL_FLOAT, sizeof(interleaved), BUFFER_OFFSET(28));

  glEnableVertexAttribArray(IDattrib_mass);
  glEnableVertexAttribArray(IDattrib_type);
  glVertexAttribPointer(IDattrib_mass, 1, GL_FLOAT, GL_FALSE, sizeof(interleaved), BUFFER_OFFSET(40));
  glVertexAttribPointer(IDattrib_type, 1, GL_INT, GL_FALSE, sizeof(interleaved), BUFFER_OFFSET(44));
  

  glUniform1fv(IDp, 3, W);
  glUniform1fv(IDpl, 7, Pl);
  glUniform1fv(IDpm, 7, Pm);


  // set up our OpenGL state
	//glEnable(GL_TEXTURE_3D);	// enable 3d texturing
	//glEnable(GL_DEPTH_TEST);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);		// our texture colors will replace the untextured colors


	// VERY IMPORTANT:
	// this line loads the address of the glTexImage3D function into the function pointer of the same name.
	// glTexImage3D is not implemented in the standard GL libraries and must be loaded dynamically at run time,
	// the environment the program is being run in MAY OR MAY NOT support it, if not we'll get back a NULL pointer.
	// this is necessary to use any OpenGL function declared in the glext.h header file
	// the Pointer to FunctioN ... PROC types are declared in the same header file with a type appropriate to the function name
	//glTexImage3D = (PFNGLTEXIMAGE3DPROC) wglGetProcAddress("glTexImage3D");
	//if (glTexImage3D == NULL)
//		return;

	//BYTE *tex = BuildTexture();

	// tell GL we want it to use this texture when it textures our polygons
	// (this function has multiple uses, see BuildTexture())
//	glBindTexture(GL_TEXTURE_3D, texname);



 //  glEnable(GL_POINT_SPRITE);
 //  glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
 //  glPointSize(4);
   glEnable(GL_PROGRAM_POINT_SIZE);
 //  glActiveTexture(0);
 //  int texture_location = glGetUniformLocation(1, "VolumeTexture");  
  // glUniform1i(texture_location, 0);  
 //  glBindTexture(GL_TEXTURE_3D,texname);
   glUniform1f(ID, 0.5);
  //glDrawArrays(GL_POINTS, 0, size);
   glDrawArrays(GL_POINTS, 0, size);


  glUniform1f(ID, 0.4);
  glDrawElements(GL_LINES, connect.size(), GL_UNSIGNED_INT, connect.data());

  
}

BYTE * ParticleSystem::BuildTexture(void)
{
#define WIDTH	4
#define HEIGHT	4
#define DEPTH	4
#define BYTES_PER_TEXEL	3
#define LAYER(r)	(WIDTH * HEIGHT * r * BYTES_PER_TEXEL)
#define TEXEL2(s, t)	(BYTES_PER_TEXEL * (s * WIDTH + t))			// 2->1 dimension mapping function
#define TEXEL3(s, t, r) (TEXEL2(s, t) + LAYER(r))	// 3->1 dimension mapping function

	// ask for enough memory for the texels and make sure we got it before proceeding
	BYTE *texels = (BYTE *)malloc(WIDTH * HEIGHT * DEPTH * BYTES_PER_TEXEL);
	if (texels == NULL)
		return NULL;
	int s, t;

	// each of the following loops defines one layer of our 3d texture, there are 3 unsigned bytes (red, green, blue) for each texel so each iteration sets 3 bytes
	// the memory pointed to by texels is technically a single dimension (C++ won't allow more than one dimension to be of variable length), the 
	// work around is to use a mapping function like the one above that maps the 3 coordinates onto one dimension
	// layer 0 occupies the first (width * height * bytes per texel) bytes, followed by layer 1, etc...
	// define layer 0 as red
	for (s = 0; s < WIDTH; s++) {
		for (t = 0; t < HEIGHT; t++) {
			texels[TEXEL3(s, t, 0)] = 0x80;
			texels[TEXEL3(s, t, 0)+1] = 0x00;
			texels[TEXEL3(s, t, 0)+2] = 0x00;
		}
	}
	// define layer 1 as green
	for (s = 0; s < WIDTH; s++) {
		for (t = 0; t < HEIGHT; t++) {
			texels[TEXEL3(s, t, 1)] = 0x00;
			texels[TEXEL3(s, t, 1)+1] = 0x80;
			texels[TEXEL3(s, t, 1)+2] = 0x00;
		}
	}
	// define layer 2 as blue
	for (s = 0; s < WIDTH; s++) {
		for (t = 0; t < HEIGHT; t++) {
			texels[TEXEL3(s, t, 2)] = 0x00;
			texels[TEXEL3(s, t, 2)+1] = 0x00;
			texels[TEXEL3(s, t, 2)+2] = 0x80;
		}
	}

	// define layer 3 as grey
	for (s = 0; s < WIDTH; s++) {
		for (t = 0; t < HEIGHT; t++) {
			texels[TEXEL3(s, t, 3)] = 0x80;
			texels[TEXEL3(s, t, 3)+1] = 0x80;
			texels[TEXEL3(s, t, 3)+2] = 0x80;
		}
	}

	// request 1 texture name from OpenGL
	glGenTextures(1, &texname);	
	// tell OpenGL we're going to be setting up the texture name it gave us	
	glBindTexture(GL_TEXTURE_3D, texname);	
	// when this texture needs to be shrunk to fit on small polygons, use linear interpolation of the texels to determine the color
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// when this texture needs to be magnified to fit on a big polygon, use linear interpolation of the texels to determine the color
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// we want the texture to repeat over the S axis, so if we specify coordinates out of range we still get textured.
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	// same as above for T axis
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// same as above for R axis
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	// this is a 3d texture, level 0 (max detail), GL should store it in RGB8 format, its WIDTHxHEIGHTxDEPTH in size, 
	// it doesnt have a border, we're giving it to GL in RGB format as a series of unsigned bytes, and texels is where the texel data is.
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB8, WIDTH, HEIGHT, DEPTH, 0, GL_RGB, GL_UNSIGNED_BYTE, texels);

	return texels;
}

void ParticleSystem::Render(glm::vec4 pos) {

	
  // 
  GLint ID = glGetUniformLocation(1, "opacity");
  GLint IDp = glGetUniformLocation(1, "eyePos");


  float W[4];
  W[0] = pos.x;
  W[1] = pos.y;
  W[2] = pos.z;
  W[3] = pos.w;

  glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, sizeof(interleaved), BUFFER_OFFSET(0));
  glEnableClientState(GL_COLOR_ARRAY);
  glColorPointer(4, GL_FLOAT, sizeof(interleaved), BUFFER_OFFSET(12));
  glEnableClientState(GL_NORMAL_ARRAY);
  glNormalPointer(GL_FLOAT, sizeof(interleaved), BUFFER_OFFSET(27));

  glUniform1fv(IDp, 4, W);

  glUniform1f(ID, 0.5);
  glDrawArrays(GL_POINTS, 0, size);

  glUniform1f(ID, 0.4);
  glDrawElements(GL_LINES, connect.size(), GL_UNSIGNED_INT, connect.data());
}

/*
void Particle::SetMass(double m) {
mass = m;
}

double Particle::GetMass() {
return mass;
}

Particle::Particle() : color (COLOR_WHITE) {

}

void Particle::SetPosition(glm::vec3 &a) {
position[0] = a.x;
position[1] = a.y;
position[2] = a.z;
}

void Particle::SetColor(glm::vec4 a) {
color = a;
}

glm::vec4 Particle::GetColor() {
return color;
}

glm::vec3 Particle::GetPosition() {
return glm::vec3(position[0], position[1], position[2]);
}

void Particle::Render() {


glColor4f(color.r, color.g, color.b, color.a);
glVertex3f(position[0], position[1], position[2]);

//glEnd();

}

void Particle::SetNeighbors(int s) {
neighbors = s;
}


int Particle::GetNeighbors() {
return neighbors;
}
*/