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
void divideBoundingBox(unsigned int axis, const BoundingBox & b, BoundingBox & up, BoundingBox & down) {
    if(axis == 0) {
	down = BoundingBox(b.getMin(), b.getMin() + Vec3Df(b.getWidth()/2, b.getHeight(), b.getLength()));
	up =  BoundingBox((b.getMin() + Vec3Df(b.getWidth()/2, 0, 0)), b.getMax());
    }
    if(axis == 1) {
	down = BoundingBox(b.getMin(), b.getMin() + Vec3Df(b.getWidth(), b.getHeight()/2, b.getLength()));
	up =  BoundingBox((b.getMin() + Vec3Df(0, b.getHeight()/2, 0)), b.getMax());
    }
    if(axis == 2) {
	down = BoundingBox(b.getMin(), b.getMin() + Vec3Df(b.getWidth(), b.getHeight(), b.getLength()/2));
	up =  BoundingBox((b.getMin() + Vec3Df(0, 0, b.getLength()/2)), b.getMax());
    }
}

unsigned int testDivideBoundingBox(const unsigned int axis, const BoundingBox & b, const Mesh & mesh, const vector<unsigned int> &triangles) {
    BoundingBox up, down;
    divideBoundingBox(axis, b, up, down);
    //cout << "DOWN: Min : " << down.getMin() << "   Max : " << down.getMax() << "\n";
    //cout << "UP: Min : " << up.getMin() << "   Max : " << up.getMax() << "\n";
    
    unsigned int cpt_up = 0;
    unsigned int cpt_down = 0;
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


KDTreeNode KDTreeBuilder::buildKDTreeRec (const BoundingBox & b, const Mesh & mesh, const vector<unsigned int> &triangles) {
    KDTreeNode node = KDTreeNode ();
    node.data = triangles;
    node.box = b;
    unsigned int cpt = 0;
    cout << "\n\n\nNumber of triangles in current node: " << triangles.size() << " box is : min(" << b.getMin() << ") max(" << b.getMax() << ")\n";
    for (unsigned int i = 0; i<triangles.size(); i++) {
	std::cout << "   Triangle " << i << " : " << mesh.getVertices()[mesh.getTriangles()[i].getVertex(0)].getPos() << "\n";
	if(containsTriangle(b, mesh, triangles[i]))
	    cpt++;
    }

    std::cout << "Cpt = " << cpt << " (should be equal to number of triangles)\n";
    if(cpt<=KDTREEBUILDER_MAX_TRIANGLES_PER_LEAF) return node;
    

    // find best axis (x, y or z) to cut the bounding box
    unsigned int var[3];
    unsigned int best_axis = 0;
    var[0] = testDivideBoundingBox(0, b, mesh, triangles);
    var[1] = testDivideBoundingBox(1, b, mesh, triangles);
    var[2] = testDivideBoundingBox(2, b, mesh, triangles);
    
    if((var[0] <= var[1]) && (var[0] <= var[2])){
	best_axis = 0;
	cout << "Divide by X axis\n";
    }
    else{
	if(var[1] <= var[2]){ best_axis = 1; cout << "Divide by Y axis\n";}
	else {best_axis = 2; cout << "Divide by Z axis\n";};
    }

    BoundingBox up, down;
    divideBoundingBox(best_axis, b, up, down);
    
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
    KDTreeNode left, right;
    
    if(left_triangles.size() != 0) left = buildKDTreeRec (down, mesh, left_triangles);
    if(right_triangles.size() != 0) right = buildKDTreeRec (up, mesh, right_triangles);
    
    node.left = &left;
    node.right = &right;
    
    return node;
}

KDTreeNode KDTreeBuilder::buildKDTree (const Object & o) {
    // get the Mesh of o
    // get triangle list of mesh
    // get bounding box of o
    
    // call buildKDTreeRec with previous argument

    // return root node
}

