#include "KdTreeElement.h"
#include "BoundingBox.h"

#define NO_SUBDIVISION_AXE 3

unsigned int KdTreeElement::trianglePerLeaf = 50;
unsigned int KdTreeElement::maxDepth = 10;

KdTreeElement* KdTreeElement::getKdTreeElement(const Mesh &mesh, std::vector<Triangle> &triangles, unsigned int depth) {
	BoundingBox *boundingBox_ = new BoundingBox(mesh, triangles);


	if (triangles.size() <= trianglePerLeaf || depth >= maxDepth)
		return new KdTreeElement(NULL, NULL, NO_SUBDIVISION_AXE, triangles, boundingBox_);

	else {
		BoundingBox bbLeft, bbRight;
		std::vector<Triangle> leftTriangles;
		std::vector<Triangle> rightTriangles;
		std::vector<Triangle> leafTriangles;

		unsigned int axe_ = boundingBox_->biggerAxe();
		float median = mesh.getMedian(axe_, *boundingBox_);
		boundingBox_->split(axe_, median, bbLeft, bbRight);

		for (unsigned int i = 0; i < triangles.size(); i++) {
			const Vec3Df & v0 = mesh.getVertices()[triangles[i].getVertex(0)].getPos();
			const Vec3Df & v1 = mesh.getVertices()[triangles[i].getVertex(1)].getPos();
			const Vec3Df & v2 = mesh.getVertices()[triangles[i].getVertex(2)].getPos();

			if(bbLeft.contains(v0) || bbLeft.contains(v1) || bbLeft.contains(v2))
				leftTriangles.push_back(triangles[i]);
			if(bbRight.contains(v0) || bbRight.contains(v1) || bbRight.contains(v2))
				rightTriangles.push_back(triangles[i]);
		}
		
		//creation of left child and right child
		KdTreeElement * leftChild_;
		KdTreeElement * rightChild_;

		if(leftTriangles.size() == 0)
			leftChild_ = NULL;
		else
			leftChild_ = KdTreeElement::getKdTreeElement(mesh, leftTriangles, depth+1);

		if(rightTriangles.size() == 0)
			rightChild_ = NULL;
		else
			rightChild_ = KdTreeElement::getKdTreeElement(mesh, rightTriangles, depth+1);

		return new KdTreeElement(leftChild_, rightChild_, axe_, leafTriangles, boundingBox_);
	}
}

KdTreeElement::~KdTreeElement() {
	if (this->leftChild != NULL)
		delete this->leftChild;
	if (this->rightChild != NULL)
		delete this->rightChild;
	delete this->boundingBox;
}


