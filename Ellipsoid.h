#pragma once

#include "stdafx.h"

class Ellipsoid {
private:
  glm::vec3 axis;
  glm::vec3 center;
  glm::vec3 size;

  std::vector<float> V;
  std::vector<float> N;
  std::vector<float> C;

  GLUquadricObj *Q;

  GLfloat M[16];


public:
  Ellipsoid(glm::vec3 &axis, glm::vec3 &center, glm::vec3 &size);
  void Render();
  void Translate(glm::vec3);
  void Scale(float k);
  void InsetParameters();

  glm::vec3 GetCenter();
};



  /*
std::vector<Ellipsoid *> loadHalosList(char *filename) {
  std::vector<Ellipsoid *> haloList;
  float centerX, centerY, centerZ;

  std::ifstream FP(filename);

  if(!FP.good()){
    std::cout << "error opening file\n";
    exit(0);
  }

  float x0, y0, z0;
  float xax, yax, zax;
  float virRadius;
  float bovera, covera;

  float crap;

  float maxX, maxY, maxZ = -10000000.0;
  float minX, minY, minZ =  10000000.0;


  maxX = maxY = maxZ;
  minX = minY = minZ;

  Ellipsoid *tmp;

  while(!FP.eof()){


    FP >> xax >> yax >> zax;
    FP >> x0 >> y0 >> z0;
    FP >> crap;
    FP >> virRadius >> bovera >> covera;
    FP >> crap;
    // FP >> crap >> crap >> crap >> crap >> crap >> crap >> crap;

    tmp = new Ellipsoid(glm::vec3(xax, yax, zax), glm::vec3(x0, y0, z0), 
      glm::vec3(fabs(virRadius), fabs(virRadius * bovera), fabs(virRadius * covera)));


    if(x0 > maxX) maxX = x0;
    if(y0 > maxY) maxY = y0;
    if(z0 > maxZ) maxZ = z0;
    if(x0 < minX) minX = x0;
    if(y0 < minY) minY = y0;
    if(z0 < minZ) minZ = z0;



    haloList.push_back(tmp);
  }

  centerX = (maxX + minX) / 2.0f;
  centerY = (maxY + minY) / 2.0f;
  centerZ = (maxZ + minZ) / 2.0f;

  float c = -100000000;
  glm::vec3 pos;

  // Center all the halos
  for(int i = 0; i < haloList.size(); i++){
    haloList[i]->Translate(glm::vec3(-centerX, -centerY, -centerZ));
    pos = glm::abs(haloList[i]->GetCenter());

    if(pos.x > c) c = pos.x;
    if(pos.y > c) c = pos.y;
    if(pos.z > c) c = pos.z;
  }

  // Normalize all the halos 
  c = 1.0f / c;
  for(int i = 0; i < haloList.size(); i++){
    haloList[i]->Scale(c);
  }




  FP.close();

  return haloList;
}
*/