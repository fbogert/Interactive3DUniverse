#ifndef __glVertexLoader_H__
#define __glVertexLoader_H__

class glVertexLoader  {

public:
   typedef struct {
float x;
float y;
float z;
float m;
int type;
   } glVertex;

  glVertexLoader(char * filename);

  glVertex* data      ();
  glVertex& operator[](unsigned int i);

  unsigned int length ();
  unsigned int bytes  ();
protected:


   unsigned int f_size;
   glVertex * mem;

   void loadFile(char* filename, unsigned int size);
};

#endif
