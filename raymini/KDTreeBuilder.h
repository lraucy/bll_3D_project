#ifndef KDTREEBUILDER_H
#define KDTREEBUILDER_H

#include <iostream>
#include <vector>

#include "Object.h"
#include "Vec3D.h"
#include "BoundingBox.h"
#include "Triangle.h"
#include "KDTreeNode.h"

#define KDTREEBUILDER_MAX_TRIANGLES_PER_LEAF 2

class KDTreeBuilder {
public:
    inline KDTreeBuilder () {}
    inline KDTreeNode buildKDTree (const Object & o);
    KDTreeNode buildKDTreeRec (const BoundingBox & b, const Mesh & mesh, const std::vector<unsigned int> &triangles);

private:
};


#endif // KDTREEBUILDER_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
