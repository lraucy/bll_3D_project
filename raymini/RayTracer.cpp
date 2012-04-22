// *********************************************************
// Ray Tracer Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2012 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "RayTracer.h"
#include "Ray.h"
#include "Scene.h"
#include <QProgressDialog>

#include "KDTreeBuilder.h"
static RayTracer * instance = NULL;

std::vector<unsigned int> *findTrianglesVectorRec(KDTreeNode &node, const Ray &ray ){
  Vec3Df intersectionPoint;
 
  if(node.leaves.size() == 1){
    if(ray.intersect(node.leaves[0].box, intersectionPoint))
      return findTrianglesVectorRec(node.leaves[0], ray);
  }  
  
  if(node.leaves.size() == 2){
    if(ray.intersect(node.leaves[0].box, intersectionPoint) && ray.intersect(node.leaves[1].box, intersectionPoint))
      return &(node.triangles);
    
    if(ray.intersect(node.leaves[0].box, intersectionPoint))
      return findTrianglesVectorRec(node.leaves[0], ray);
    
    
    if(ray.intersect(node.leaves[1].box, intersectionPoint)){
      return findTrianglesVectorRec(node.leaves[1], ray);
    }
  }

  return &(node.triangles);
}


RayTracer * RayTracer::getInstance () {
  if (instance == NULL)
    instance = new RayTracer ();
  return instance;
}

void RayTracer::destroyInstance () {
  if (instance != NULL) {
    delete instance;
    instance = NULL;
  }
}

inline int clamp (float f, int inf, int sup) {
  int v = static_cast<int> (f);
  return (v < inf ? inf : (v > sup ? sup : v));
}

// POINT D'ENTREE DU PROJET.
// Le code suivant ray trace uniquement la boite englobante de la scene.
// Il faut remplacer ce code par une veritable raytracer
QImage RayTracer::render (const Vec3Df & camPos,
                          const Vec3Df & direction,
                          const Vec3Df & upVector,
                          const Vec3Df & rightVector,
                          float fieldOfView,
                          float aspectRatio,
                          unsigned int screenWidth,
                          unsigned int screenHeight, 
			  unsigned int opt) {
  QImage image (QSize (screenWidth, screenHeight), QImage::Format_RGB888);
  Scene * scene = Scene::getInstance ();
  const BoundingBox & bbox = scene->getBoundingBox ();
  const Vec3Df & minBb = bbox.getMin ();
  const Vec3Df & maxBb = bbox.getMax ();
  const Vec3Df rangeBb = maxBb - minBb;
  QProgressDialog progressDialog ("Raytracing...", "Cancel", 0, 100);
  progressDialog.show ();

  
  KDTreeNode  root[scene->getObjects().size ()];
  for (unsigned int k = 0; k < scene->getObjects().size (); k++) {
    Object & o = scene->getObjects()[k];  
    root[k] = o.getTreeroot();
  }

  
  for (unsigned int i = 0; i < screenWidth; i++) {
    progressDialog.setValue ((100*i)/screenWidth);
    for (unsigned int j = 0; j < screenHeight; j++) {
      float tanX = tan (fieldOfView)*aspectRatio;
      float tanY = tan (fieldOfView);
      Vec3Df stepX = (float (i) - screenWidth/2.f)/screenWidth * tanX * rightVector;
      Vec3Df stepY = (float (j) - screenHeight/2.f)/screenHeight * tanY * upVector;
      Vec3Df step = stepX + stepY;
      Vec3Df dir = direction + step;
      dir.normalize ();
      Vec3Df intersectionPoint;
      Vec3Df baricentricCoo;
      float smallestIntersectionDistance = 1000000.f;
      Vec3Df c (backgroundColor);

	
      for (unsigned int k = 0; k < scene->getObjects().size (); k++) {
	Object & o = scene->getObjects()[k];
  	Ray ray (camPos-o.getTrans (), dir);

	bool hasBoxIntersection = ray.intersect (o.getBoundingBox (),
						 intersectionPoint);
	if(hasBoxIntersection) {
	  
	  std::vector<unsigned int> *pTriangles;
	  std::vector<unsigned int> triangles;
	  
	  if(opt == 1)
	    pTriangles = findTrianglesVectorRec(root[k], ray); 
	  else{
	    for(unsigned int i = 0; i<o.getMesh().getTriangles().size (); i++)
	      (triangles).push_back(i);
	    pTriangles = &triangles;
	  }
	  
	  std::cout << "Pixel "<< i << " " << j << "there is " << (*pTriangles).size() << " triangles\n";  
	    
	  for(unsigned int l = 0; l < (*pTriangles).size(); l++) {
	    const Triangle & t = o.getMesh().getTriangles()[(*pTriangles)[l]];
	    Vec3Df triangle[3] = {o.getMesh().getVertices()[t.getVertex(0)].getPos(), 
				  o.getMesh().getVertices()[t.getVertex(1)].getPos(),
				  o.getMesh().getVertices()[t.getVertex(2)].getPos()};
		    
	    bool hasIntersection =  ray.intersectTriangle (triangle, intersectionPoint, baricentricCoo);
	    if (hasIntersection) {
	      float distance = Vec3Df::distance(ray.getOrigin(), intersectionPoint);
	      if(distance < smallestIntersectionDistance){
		smallestIntersectionDistance = distance;
		Vec3Df normal = baricentricCoo[0]*o.getMesh().getVertices()[t.getVertex(0)].getNormal() + 
		  baricentricCoo[1]*o.getMesh().getVertices()[t.getVertex(1)].getNormal() +   
		  baricentricCoo[2]*o.getMesh().getVertices()[t.getVertex(2)].getNormal();
			
		normal.normalize();
		Vec3Df lightVector = intersectionPoint - scene->getLights()[0].getPos();
		lightVector.normalize();
		float w_i = std::max(Vec3Df::dotProduct(normal, lightVector), (float) 0);
		
		Vec3Df colorVect;
		float diffuseRef = o.getMaterial().getDiffuse();
		float specRef = o.getMaterial().getSpecular();
		float shininess = 16.0f;

		colorVect = o.getMaterial().getColor();
	
		float color = diffuseRef*w_i + specRef*pow(std::max(Vec3Df::dotProduct(Vec3Df::crossProduct(normal, lightVector), ray.getDirection()), (float)0.0), shininess);
			
		color = 255-color*255;
		c = Vec3Df(color*colorVect[0], color*colorVect[1], color*colorVect[2]);
	      }
	    }
	    
	  }
	}
      }
      image.setPixel (i, j, qRgb (clamp (c[0], 0, 255), clamp (c[1], 0, 255), clamp (c[2], 0, 255)));
    }
  }
  progressDialog.setValue (100);
  return image;
}
