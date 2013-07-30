#ifndef __glStarLoader_H__
#define __glStarLoader_H__

class glStarLoader  {

public:
   typedef struct {
float x;
float y;
float z;
float time;
   } glStar;

  glStarLoader(char * filename);

  glStar* data      ();
  glStar& operator[](unsigned int i);

  unsigned int length ();
  unsigned int bytes  ();
protected:


   unsigned int f_size;
   glStar * mem;

   void loadFile(char* filename, unsigned int size);
};

#endif
