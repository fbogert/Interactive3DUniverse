#include <fstream>

#include "glLoader.h"
#include "glVertexLoader.h"

glVertexLoader::glVertexLoader(char * filename) {
    f_size = glLoader::getFileLength(filename)/sizeof(glVertex);
    loadFile(filename, f_size);
}

glVertexLoader::glVertex& glVertexLoader::operator[] (const unsigned int i) {
    return mem[i];
}

glVertexLoader::glVertex* glVertexLoader::data() {
    return mem;
}

unsigned int glVertexLoader::length() {
    return f_size;
}

unsigned int glVertexLoader::bytes() {
    return f_size * sizeof(glVertex);
}

void glVertexLoader::loadFile(char* filename, unsigned int size) {
    mem = new glVertex[size];
    std::ifstream IN(filename, std::ios_base::in);

    if(!IN.good()){
        f_size = 0;
    }

float x;
float y;
float z;
float m;
int type;
    int i = 0;

    while(IN >> x >> y >> z >> m >> type) {
       mem[i].x = x;
       mem[i].y = y;
       mem[i].z = z;
       mem[i].m = m;
       mem[i].type = type;
       i++;
    }
    IN.close();
}
