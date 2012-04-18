#ifndef KDTREENODE_H
#define KDTREENODE_H
#include <iostream>
#include <vector>

#include "Vec3D.h"
#include "BoundingBox.h"
#include "Triangle.h"

class KDTreeNode {
public:
    inline KDTreeNode () {}
	void print();
    std::vector<unsigned int> data;
    BoundingBox box;
    KDTreeNode *left;
    KDTreeNode *right; 
};

/*
std::ostream& operator<< (std::ostream & output, KDTreeNode const& n) {
    //    output << n.box << "\n" << n.getLeft() << "\n" << n.getRight();
    output << "node";
    return output;
}
*/

#endif // KDTREENODE_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
