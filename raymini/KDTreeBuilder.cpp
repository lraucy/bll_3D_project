#include "KDTreeBuilder.h"

using namespace std;

bool containsTriangle (const BoundingBox & b, const Mesh & mesh, const unsigned int triangle) {
    if(b.contains(mesh.getVertices()[mesh.getTriangles()[triangle].getVertex(0)].getPos()) || 
       b.contains(mesh.getVertices()[mesh.getTriangles()[triangle].getVertex(1)].getPos()) || 
       b.contains(mesh.getVertices()[mesh.getTriangles()[triangle].getVertex(2)].getPos()) )
	return true;
    return false;
}


/* 0 for x, 1 for y and 2 for z */
void divideBoundingBox(unsigned int axis, float splitLeftCoef, const BoundingBox & b, BoundingBox & up, BoundingBox & down) {
    if(axis == 0) {
	down = BoundingBox(b.getMin(), b.getMin() + Vec3Df(b.getWidth()*splitLeftCoef, b.getHeight(), b.getLength()));
	up =  BoundingBox((b.getMin() + Vec3Df(b.getWidth()*splitLeftCoef, 0, 0)), b.getMax());
    }
    if(axis == 1) {
      down = BoundingBox(b.getMin(), b.getMin() + Vec3Df(b.getWidth(), b.getHeight()*splitLeftCoef, b.getLength()));
	up =  BoundingBox((b.getMin() + Vec3Df(0, b.getHeight()*splitLeftCoef, 0)), b.getMax());
    }
    if(axis == 2) {
	down = BoundingBox(b.getMin(), b.getMin() + Vec3Df(b.getWidth(), b.getHeight(), b.getLength()*splitLeftCoef));
	up =  BoundingBox((b.getMin() + Vec3Df(0, 0, b.getLength()*splitLeftCoef)), b.getMax());
    }
}

unsigned int testDivideBoundingBox(const unsigned int axis, float splitLeftCoef, const BoundingBox & b, const Mesh & mesh, const vector<unsigned int> &triangles, unsigned int & cpt_up, unsigned int & cpt_down) {
    BoundingBox up, down;
    divideBoundingBox(axis, splitLeftCoef, b, up, down);
    //cout << "DOWN: Min : " << down.getMin() << "   Max : " << down.getMax() << "\n";
    //cout << "UP: Min : " << up.getMin() << "   Max : " << up.getMax() << "\n";
    
    cpt_up = 0;
    cpt_down = 0;
    for (unsigned int i = 0; i<triangles.size(); i++) {

	if(containsTriangle(down, mesh, triangles[i])){
	    //	    cout << "triangle " << i << " is in down box\n";
	    cpt_down++;
	}
	if(containsTriangle(up, mesh, triangles[i])){
	    //cout << "triangle " << i << " is in up box\n"; 
	    cpt_up++;
	}
    }
    //std::cout << "DOWN: nb de triangle= " << cpt_down << "\n";
    //std::cout << "UP: nb de triangle= " << cpt_up << "\n";

    return abs(cpt_up - cpt_down);
}

float findBestSplit(const BoundingBox & b, const Mesh & mesh, const vector<unsigned int> &triangles, unsigned int & bestAxis) {
  unsigned int cpt_up, cpt_down;
  float bestCoef = 0.0;
  bestAxis=0;
  
  float bestcost = testDivideBoundingBox(bestAxis, bestCoef, b, mesh, triangles, cpt_up, cpt_down) + (cpt_up+cpt_down - triangles.size());
 
  for(unsigned int axis = 0; axis < 3; axis++)
    {
 
      for (float coef = 0.0; coef <= 1.0; coef = coef + 0.1)
	{
	  float cost = testDivideBoundingBox(axis, coef, b, mesh, triangles, cpt_up, cpt_down);
	  cost += (cpt_up+cpt_down - triangles.size()); 
	  
	  
	  if(cost < bestcost) {
	    //	    std::cout << "best cost is " << cost << " axis is " << axis << "\n";
	
	    bestcost = cost;
	    bestCoef = coef;
	    bestAxis = axis;
	  }
	}
    }
  return bestCoef;
}



KDTreeNode KDTreeBuilder::buildKDTreeRec (const BoundingBox & box, const Mesh & mesh, const vector<unsigned int> &triangles, unsigned int depth) {
    KDTreeNode node = KDTreeNode ();
    node.setTriangles(triangles);
    node.setBox(box);
    unsigned int cpt = 0;
    //cout << "\n\n\nNumber of triangles in current node (depth = "<<depth<<") : " << triangles.size() << " \nBox is : min(" << box.getMin() << ") max(" << box.getMax() << ")\n";
    for (unsigned int i = 0; i<triangles.size(); i++) {
      //std::cout << "   Triangle " << triangles[i] << " : " << mesh.getVertices()[mesh.getTriangles()[i].getVertex(0)].getPos() << "\n";
	if(containsTriangle(box, mesh, triangles[i]))
	    cpt++;
    }

    //std::cout << "Cpt = " << cpt << " (should be equal to number of triangles)\n";
    if(cpt<=KDTREEBUILDER_MIN_TRIANGLES_PER_LEAF || (depth >= KDTREEBUILDER_MAX_DEPTH)) return node;
    

    // find best axis (x, y or z) to cut the bounding box
    unsigned int var[3];
    unsigned int best_axis = 0;
    unsigned int cpt_down, cpt_up;
    BoundingBox up, down;
    float bestSplitCoef = findBestSplit(box, mesh, triangles, best_axis);

    //std::cout << "Best Split is " << bestSplitCoef << " with axis " << best_axis << " \n";
    divideBoundingBox(best_axis, bestSplitCoef, box, up, down);
    
    // cut triangles in two parts depending of axis
    vector<unsigned int> left_triangles, right_triangles;
    for (unsigned int i = 0; i<triangles.size(); i++) {
	if(containsTriangle(down, mesh, triangles[i])){
	        left_triangles.push_back(triangles[i]);
	}
	if(containsTriangle(up, mesh, triangles[i])){
	        right_triangles.push_back(triangles[i]);
	}
    }
   
     // call methode for left and right with divided BoudingBox and divided triangles lists (if lists not empty)
    depth++;
    
    
    if(left_triangles.size() != 0 ) node.pushBackLeaf(buildKDTreeRec (down, mesh, left_triangles, depth));
    if (right_triangles.size() != 0) node.pushBackLeaf(buildKDTreeRec (up, mesh, right_triangles, depth));
    
    return node;
}

KDTreeNode KDTreeBuilder::buildKDTree (const Object & o) {  
  vector<unsigned int> triangles;
  for(unsigned int i = 0; i < o.getMesh().getTriangles().size(); i++)
    triangles.push_back(i);
  KDTreeNode root = buildKDTreeRec (o.getBoundingBox(), o.getMesh(), triangles, 0);
  return root;
}

