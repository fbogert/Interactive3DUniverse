#ifndef __glLoader_H__
#define __glLoader_H__

#include <fstream>
class glLoader  {
public:
  static unsigned int getFileLength(const char *filename) {
  std::ifstream FP(filename, std::ios_base::binary | std::ios_base::in);
  if(!FP.good()){
      return 0;
  }

  FP.seekg(0, FP.end);
  unsigned long len = FP.tellg();
  FP.seekg(0, FP.beg);
  FP.close();

  return len;
    }


};

#endif
