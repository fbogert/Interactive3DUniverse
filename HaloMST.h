#pragma once

#include "ArchHeader.h"
#include "Ellipsoid.h"

class HaloMST {
private:

  // Adjacency matrix
  float **AM;
  int size;


  char *inTree; /* inTree[i] is 1 if the node i is already in the minimum
            spanning tree; 0 otherwise*/
 
int *d; /* d[i] is the distance between node i and the minimum spanning
        tree; this is initially infinity (100000); if i is already in
        the tree, then d[i] is undefined;
        this is just a temporary variable. It's not necessary but speeds
        up execution considerably (by a factor of n) */
 
int *whoTo; /* whoTo[i] holds the index of the node i would have to be
            linked to in order to get a distance of d[i] */

  std::vector<Ellipsoid *> *list;

  std::vector<glm::vec3> vertices;

  
  void updateDistances(int target);

public:
  HaloMST(std::vector<Ellipsoid *> *E);
  void Render(int i = 0);
  void MST(int i);
  int GetCount();

};