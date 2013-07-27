#pragma once
#include "stdafx.h"


class ParticleSystem {
protected:

  typedef struct{
	 //0
    float x, y, z;
	//12
    float r, g, b, a;
	//28
    float nx, ny, nz;
	//40
	float m;
	//44
	int type;
    
  } interleaved;
  
  interleaved *ibuf;
  int size;

  std::vector<unsigned long> connect;
  
  unsigned int texname;
  float maxmass;
  float minmass;
  glm::vec3 maxpos;
  glm::vec3 minpos;
  float penScale;
  float saveScale;
  glm::vec3 penSavePos;
  glm::vec3 penSaveRot;

    glm::vec3 penScaleSavePos;
  glm::vec3 penScaleSaveRot;
  int lastPenPush;
  // VBO STUFF
  unsigned int vboID[1];  // vertex buffer object
  unsigned int vaoID[1];
  BYTE * BuildTexture(void);
  void quickSort(interleaved arr[], unsigned long left, unsigned long right);
  void LoadMemory(const char *, unsigned long);

public:
  ParticleSystem(const char *, unsigned long);
  ParticleSystem(const char *);
  ~ParticleSystem();
  int GetSize();
  glm::vec3 GetPosition(unsigned long i);
  void SetPosition(unsigned long i, glm::vec3 &pos);
  double GetMass(unsigned long i);
  void Connect(unsigned long i, unsigned long j);

  void ReadTree(const char *);


  void Render(glm::vec4);
  void Render(glm::vec3 pos, glm::vec3 lightPos, glm::vec3 lightRot, int activatePen);
};

/*
class Particle {

protected:
  //glm::vec3 position;
  float position[3];
  glm::vec4 color;
  int neighbors;
  double mass;

public:
  Particle();
  void SetMass(double m);
  double GetMass();
  void SetPosition(glm::vec3 &);
  void SetColor(glm::vec4);
  void SetNeighbors(int);
  int GetNeighbors();
  glm::vec4 GetColor();
  glm::vec3 GetPosition();

  void Render();
};
*/