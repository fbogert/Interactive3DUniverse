#include <iostream>
#include <fstream>

#include "glStarLoader.h"

int main(int argc, char** argv){
   glStarLoader verts(argv[1]);

   std::ofstream OUT(argv[2], std::ios_base::binary | std::ios_base::out);
   OUT.write((char *)verts.data(), verts.bytes());
   OUT.close();

   return 0;
}


