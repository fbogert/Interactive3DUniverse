#include "ArchHeader.h"
#include "HaloMST.h"



 
/* updateDistances(int target)
    should be called immediately after target is added to the tree;
    updates d so that the values are correct (goes through target's
    neighbours making sure that the distances between them and the tree
    are indeed minimum)
*/
void HaloMST::updateDistances(int target) {
    int i;
    for (i = 0; i < size; ++i)
        if ((AM[target][i] != 0) && (d[i] > AM[target][i])) {
            d[i] = AM[target][i];
            whoTo[i] = target;
        }
}
 
void HaloMST::MST(int start) {

  vertices.clear();
  int i, j;
 
    /* Initialise d with infinity */
    for (i = 0; i < size; ++i)
        d[i] = 100000;
 
    /* Mark all nodes as NOT beeing in the minimum spanning tree */
    for (i = 0; i < size; ++i)
        inTree[i] = 0;
 
    /* Add the first node to the tree */
    printf("Adding node %c\n", 0 + 'A');
    inTree[start] = 1;
    updateDistances(start);
 
    int total = 0;
    int treeSize;
    for (treeSize = 1; treeSize < size; ++treeSize) {
        /* Find the node with the smallest distance to the tree */
        int min = -1;
        for (i = 0; i < size; ++i)
            if (!inTree[i])
                if ((min == -1) || (d[min] > d[i]))
                    min = i;
 
        /* And add it */
        printf("Adding edge %c-%c\n", whoTo[min] + 'A', min + 'A');
        vertices.push_back((*list)[whoTo[min]]->GetCenter());
        vertices.push_back((*list)[min]->GetCenter());


        inTree[min] = 1;
        total += d[min];
 
        updateDistances(min);
    }
 
    printf("Total distance: %d\n", total);
 
}

HaloMST::HaloMST(std::vector<Ellipsoid *> *E) {
  size = (*E).size();

  // Construct an Adjacency matrix
  AM = new float *[size];
  for(int i = 0; i < size; i++){
    AM[i] = new float[size];

    for(int j = 0; j < size; j++){
      AM[i][j] = glm::distance((*E)[i]->GetCenter(), (*E)[j]->GetCenter());
    }
  }

  d = new int[size];
  inTree = new char[size];
  whoTo = new int[size];

  // Matrix is set up, ready for MSTing!
  list = E;

  MST(0);
}

void HaloMST::Render(int i) {

  glBegin(GL_LINES);
  if(i == -1){
    for(int i = 0; i < vertices.size(); i += 2){
      Render(i);   
    }
  }else{
    glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
    glVertex3f(vertices[i+1].x, vertices[i+1].y, vertices[i+1].z);
  }

  glEnd();

}

int HaloMST::GetCount() {
  return vertices.size() - 1;
}