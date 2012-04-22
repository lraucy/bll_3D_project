#ifndef KDTREENODE_H
#define KDTREENODE_H
#include <iostream>
#include <vector>

#include "Vec3D.h"
#include "BoundingBox.h"
#include "Triangle.h"

class KDTreeNode {
public:
  KDTreeNode () {
	triangles = std::vector<unsigned int>();
	box = BoundingBox() ;
	leaves = std::vector<KDTreeNode> ();
  };
  /*  inline KDTreeNode& operator= (const KDTreeNode & node) {
	triangles = std::vector<unsigned int>(node.triangles);
	box = node.box;
	std::cout << "Box " << node.box.getMin() << "\n";

	if(node.leaves.size() >= 1){
	  leaves.push_back(node.leaves[0]);
	  leaves[0] = node.leaves[0];
	}
	if(node.leaves.size() == 2){
	  leaves.push_back(node.leaves[1]);
	  leaves[1] = node.leaves[1];
	}
	return (*this);
	};*/

  void setNode(KDTreeNode root){
	setTriangles(root.triangles);
	setBox(root.box);
	if(root.leaves.size() == 1)
	  {
		pushBackLeaf(root.leaves[0]);
		leaves[0] = KDTreeNode();
		leaves[0].setNode(root.leaves[0]);
	  }
	if(root.leaves.size() == 2)
	  {
		pushBackLeaf(root.leaves[0]);
		leaves[0] = KDTreeNode();
		leaves[0].setNode(root.leaves[0]);
		pushBackLeaf(root.leaves[1]);
		leaves[1] = KDTreeNode();
		leaves[1].setNode(root.leaves[1]);
	  }
  }
  void setTriangles(std::vector<unsigned int> _triangles){triangles = _triangles;}
  void setBox(BoundingBox _box){box = _box;}
  void pushBackLeaf(KDTreeNode _leaf) { leaves.push_back(_leaf);}

  void print();
  std::vector<unsigned int> triangles;
  BoundingBox box;
  std::vector<KDTreeNode> leaves;
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
