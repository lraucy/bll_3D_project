#include "KDTreeNode.h"


void KDTreeNode::print(){
  std::cout << "Node : "; 
  std::cout << "Box min " << box.getMin() << " max " << box.getMax() << "first triangle is "<< triangles[0] << "\n";
  if(leaves.size() >= 1){
    std::cout << "Left ";
    leaves[0].print();
  }
  if(leaves.size() == 2)  {
      std::cout << "Right ";
      leaves[1].print();
  }

}
