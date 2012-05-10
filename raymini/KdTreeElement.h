// *********************************************************
// KdTreeElement Class
// Author : Loic Raucy (loic.raucy@gmail.com).
// *********************************************************
//

#ifndef KD_TREE_ELEMENT_H
#define KD_TREE_ELEMENT_H

#include "Triangle.h"

class Mesh;
class BoundingBox;

class KdTreeElement {
public:
	static KdTreeElement* getKdTreeElement(const Mesh &mesh, std::vector<Triangle> &triangles, unsigned int depth);
	~KdTreeElement();

	KdTreeElement *leftChild;
	KdTreeElement *rightChild;
	unsigned int axe; // axe dividing leftChild and rightChild
	std::vector<Triangle> triangles;
	BoundingBox *boundingBox;

	static inline void setMaxDepth(unsigned int maxDepth_) {maxDepth = maxDepth_;}
	static unsigned int maxDepth;
	static inline void setTrianglePerLeaf(unsigned int trianglePerLeaf_) {trianglePerLeaf = trianglePerLeaf_;}
	static unsigned int trianglePerLeaf;

private:
	inline KdTreeElement(KdTreeElement * leftChild_,
					KdTreeElement * rightChild_,
					unsigned int axe_,
					std::vector<Triangle> & triangles_,
					BoundingBox *boundingBox_):
			leftChild(leftChild_),
			rightChild(rightChild_),
			axe(axe_),
			triangles(triangles_),
			boundingBox(boundingBox_) {}

};

#endif
