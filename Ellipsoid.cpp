#include "stdafx.h"
#include "Ellipsoid.h"

Ellipsoid::Ellipsoid(glm::vec3 &axis, glm::vec3 &center, glm::vec3 &size) :
  size(size), center(center) {

    this->axis = glm::normalize(axis);
}

void Ellipsoid::InsetParameters() {
  Q = gluNewQuadric();
  gluQuadricDrawStyle( Q, GLU_FILL);
  gluQuadricNormals( Q, GLU_SMOOTH);
  gluQuadricOrientation( Q, GLU_OUTSIDE);
  gluQuadricTexture( Q, GL_TRUE);
  gluSphere(Q, 0.01, 10, 10);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

#ifdef ZSPACE_SDK
  glTranslatef(center.x, center.y, center.z);
 // glRotatef(M_PI * 2.0, this->axis.x, this->axis.y, this->axis.z);
 // glScalef(size.x * 0.01, size.y *0.01, size.z * 0.01);
  //
#else
  glTranslatef(center.x, center.y, center.z);
  glRotatef(M_PI * 2.0f, this->axis.x, this->axis.y, this->axis.z);
  glScalef(size.x * 0.01f, size.y *0.01f, size.z * 0.01f);
#endif

  glGetFloatv(GL_MODELVIEW_MATRIX, M);

  glPopMatrix();
}

void Ellipsoid::Render() {
  glPushMatrix();
  
  glMultMatrixf(M);
  gluSphere(Q, 0.01, 10, 10);
  
  glPopMatrix();
}

void Ellipsoid::Translate(glm::vec3 A) {
  this->center += A;
  for(int i = 0; i < V.size(); i+=3){
    V[i] = V[i] + A.x;
    V[i+1] = V[i+1] + A.y;
    V[i+2] = V[i+2] + A.z;
  }
}

void Ellipsoid::Scale(float A) {
  this->center *= A;
  for(int i = 0; i < V.size(); i+=3){
    V[i] = V[i] * A;
    V[i+1] = V[i+1] * A;
    V[i+2] = V[i+2] * A;
  }
}

glm::vec3 Ellipsoid::GetCenter() {
  return center;
}