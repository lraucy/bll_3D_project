#include "KDTreeNode.h"


void KDTreeNode::print(){
    std::cout << "Box min " << box.getMin() << " max " << box.getMax() << "\n";
    std::cout << "Left ";
    if(left != NULL)
	left->print();
    std::cout << "Right ";
    if(right != NULL)
	right->print();
}
