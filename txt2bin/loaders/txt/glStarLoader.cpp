#include <fstream>

#include "glLoader.h"
#include "glStarLoader.h"

glStarLoader::glStarLoader(char * filename) {
    f_size = glLoader::getFileLength(filename)/sizeof(glStar);
    loadFile(filename, f_size);
}

glStarLoader::glStar& glStarLoader::operator[] (const unsigned int i) {
    return mem[i];
}

glStarLoader::glStar* glStarLoader::data() {
    return mem;
}

unsigned int glStarLoader::length() {
    return f_size;
}

unsigned int glStarLoader::bytes() {
    return f_size * sizeof(glStar);
}

void glStarLoader::loadFile(char* filename, unsigned int size) {
    mem = new glStar[size];
    std::ifstream IN(filename, std::ios_base::in);

    if(!IN.good()){
        f_size = 0;
    }

float x;
float y;
float z;
float time;
    int i = 0;

    while(IN >> x >> y >> z >> time) {
       mem[i].x = x;
       mem[i].y = y;
       mem[i].z = z;
       mem[i].time = time;
       i++;
    }
    IN.close();
}
