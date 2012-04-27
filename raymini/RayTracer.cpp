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
#include "KdTreeElement.h"

static RayTracer * instance = NULL;

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
		unsigned int screenHeight) {

	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	this->upVector = upVector;
	this->rightVector = rightVector;
	QImage image (QSize (screenWidth, screenHeight), QImage::Format_RGB888);
	QProgressDialog progressDialog ("Raytracing...", "Cancel", 0, 100);
	progressDialog.show ();
	for (unsigned int i = 0; i < screenWidth; i++) {
		progressDialog.setValue ((100*i)/screenWidth);
		for (unsigned int j = 0; j < screenHeight; j++) {
			tanX = tan (fieldOfView)*aspectRatio;
			tanY = tan (fieldOfView);
			Vec3Df stepX = (float (i) - screenWidth/2.f)/screenWidth * tanX * rightVector;
			Vec3Df stepY = (float (j) - screenHeight/2.f)/screenHeight * tanY * upVector;
			Vec3Df step = stepX + stepY;
			Vec3Df dir = direction + step;
			Vec3Df intersectionPoint;
			dir.normalize ();

			Vec3Df c = getColor(camPos, dir, intersectionPoint);
			

			float coef = shadowRay(intersectionPoint, 10);
			c = c * Vec3Df(coef, coef, coef);
			
			image.setPixel (i, j, qRgb (clamp (c[0], 0, 255), clamp (c[1], 0, 255), clamp (c[2], 0, 255)));
		}
	}
	progressDialog.setValue (100);
	return image;
}

const Object * RayTracer::getObjectIntersected(const Vec3Df &camPos, const Vec3Df &dir,
							Vec3Df &intersectionPoint, Triangle &intersectionTriangle) const{

	Scene * scene = Scene::getInstance();

	float smallestIntersectionDistance = 1000000.f;
	const Object *objectIntersected = NULL;

	for (unsigned int k = 0; k < scene->getObjects().size (); k++) {
		const Object & o = scene->getObjects()[k];
		Ray ray (camPos-o.getTrans (), dir);

		if(ray.intersect(o.getMesh(), o.getMesh().kdTree, intersectionPoint, smallestIntersectionDistance, intersectionTriangle))
			objectIntersected = &o;
	}
	return objectIntersected;
}

Vec3Df RayTracer::getNormalAtIntersection(const Object &o, const Vec3Df &intersectionPoint,
												const Triangle &triangle) const{
	const Vertex & v0 = o.getMesh().getVertices()[triangle.getVertex(0)];
	const Vertex & v1 = o.getMesh().getVertices()[triangle.getVertex(1)];
	const Vertex & v2 = o.getMesh().getVertices()[triangle.getVertex(2)];
	Vec3Df triangleCoo[3] = {v0.getPos(), v1.getPos(), v2.getPos()};
	Vec3Df barycentricCooIntersection = Triangle::getBarycentricCoo(triangleCoo,
														intersectionPoint);

	Vec3Df normal = barycentricCooIntersection[0]*v0.getNormal() +
					barycentricCooIntersection[1]*v1.getNormal() +
					barycentricCooIntersection[2]*v2.getNormal();
	normal.normalize();
	return normal;
}

Vec3Df RayTracer::getPhongBRDF(const Ray &ray, const Object &o,	const Vec3Df &intersectionPoint,
					const Vec3Df &normal) const{

	Scene * scene = Scene::getInstance();

	Vec3Df toLight = scene->getLights()[0].getPos() - intersectionPoint;
	toLight.normalize();
	Vec3Df r = (ray.getOrigin() -intersectionPoint);
	r.normalize();
	Vec3Df ref = 2*Vec3Df::dotProduct(normal, toLight)*normal - toLight;
	ref.normalize();

	float diffuseRef = o.getMaterial().getDiffuse();
	float specRef = o.getMaterial().getSpecular();
	float shininess = 16.0f;

	Vec3Df colorVect = o.getMaterial().getColor();

	float color = specRef * pow(std::max(Vec3Df::dotProduct(ref, r), 0.0f), shininess) + diffuseRef*std::max(Vec3Df::dotProduct(toLight, normal), 0.0f);

	color = color*255;
	return Vec3Df(color*colorVect[0], color*colorVect[1], color*colorVect[2]);
}

Vec3Df RayTracer::getColorFromRay(const Vec3Df &camPos, const Vec3Df &dir, Vec3Df &intersectionPoint) const {
	Vec3Df c (backgroundColor);
	
	Triangle intersectionTriangle;
	const Object * objectIntersected = NULL;

	objectIntersected = getObjectIntersected(camPos, dir, intersectionPoint,
			intersectionTriangle);
	if(objectIntersected != NULL) {
		Ray ray (camPos - objectIntersected->getTrans(), dir);
		Vec3Df normal = getNormalAtIntersection(*objectIntersected, intersectionPoint,
				intersectionTriangle);
		c = getPhongBRDF(ray, *objectIntersected, intersectionPoint, normal);
	}
	return c;
}

Vec3Df RayTracer::getColorFromPixel(const Vec3Df &camPos, const Vec3Df &dir, Vec3Df &intersectionPoint) const {
  return getColorFromRay(camPos, dir, intersectionPoint);
}

Vec3Df RayTracer::getColorFromPixelWithAAx2(const Vec3Df &camPos, const Vec3Df &dir, Vec3Df &intersectionPoint) const {
	Vec3Df color = Vec3Df(0.0f, 0.0f, 0.0f);
	Vec3Df intersectionPointTemp;
	
	for (unsigned int i = 0; i < 2; i++) {
		for (unsigned int j = 0; j < 2; j++) {
			Vec3Df aaDir = dir + (0.5 - i)*tanX*rightVector/(2.0*screenWidth)
				+ (0.5 - j)*tanY*upVector/(2.0*screenHeight);
			color += getColorFromRay(camPos, aaDir, intersectionPoint);
			intersectionPoint += intersectionPointTemp;
		}
	}
	
	intersectionPoint /= 4;
	return color / 4;
}

Vec3Df RayTracer::getColorFromPixelWithAAx3(const Vec3Df &camPos, const Vec3Df &dir, Vec3Df &intersectionPoint) const {
	Vec3Df color = Vec3Df(0.0f, 0.0f, 0.0f);
	Vec3Df intersectionPointTemp;
	
	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			Vec3Df aaDir = dir + i*tanX*rightVector/(4.0*screenWidth)
				+ j*tanY*upVector/(4.0*screenHeight);
			color += getColorFromRay(camPos, aaDir, intersectionPointTemp);
			intersectionPoint += intersectionPointTemp;
		}
	}
	intersectionPoint /= 9;
	return color / 9;
}

Vec3Df RayTracer::getColor(const Vec3Df &camPos, const Vec3Df &dir, Vec3Df &intersectionPoint) const {
  switch(aaOpt){
  case RAYTRACER_NO_AA: 
    return getColorFromPixel(camPos, dir, intersectionPoint);
    break;
  case RAYTRACER_AAx2:
    return getColorFromPixelWithAAx2(camPos, dir, intersectionPoint);
    break;
  case RAYTRACER_AAx3:
    return getColorFromPixelWithAAx3(camPos, dir, intersectionPoint);
    break;
  }
  return getColorFromPixel(camPos, dir, intersectionPoint); 
}

float RayTracer::hardShadowRay(const Vec3Df &intersectionPoint) const{
  Scene * scene = Scene::getInstance();
  Triangle intersectionTriangleShadow;
  Vec3Df intersectionPointShadow;
  float epsilon = 0.1;
  float smallestIntersectionDistanceShadow = 1000000.f;
  bool hasIntersectionShadow = false;

  Vec3Df shadowRayDirection = scene->getLights()[0].getPos() - intersectionPoint;
  shadowRayDirection.normalize();

  for (unsigned int k = 0; k < scene->getObjects().size (); k++) {
    const Object & o = scene->getObjects()[k];
    Ray ray (intersectionPoint + epsilon * shadowRayDirection - o.getTrans (), shadowRayDirection);
    if(ray.intersect(o.getMesh(), o.getMesh().kdTree, intersectionPointShadow, smallestIntersectionDistanceShadow, intersectionTriangleShadow))
      hasIntersectionShadow = true;
  }
  return hasIntersectionShadow ? 0.0 : 1.1 ;
}

float RayTracer::softShadowRay(const Vec3Df &intersectionPoint, 
			       const unsigned int nbSamples) const{
  Scene * scene = Scene::getInstance();
  Triangle intersectionTriangleShadow;
  Vec3Df intersectionPointShadow;
  float epsilon = 0.1;
  float counter = 0;
  
  for(unsigned int i = 0; i < nbSamples; i++){
    float smallestIntersectionDistanceShadow = 1000000.f;
    Vec3Df shadowRayDirection = ((scene->getLights()[0].getPos() + 
				 Vec3Df((rand()/(double)RAND_MAX) * scene->getLights()[0].getRadius(),
					(rand()/(double)RAND_MAX) * scene->getLights()[0].getRadius(),
					(rand()/(double)RAND_MAX) * scene->getLights()[0].getRadius()))
				 - intersectionPoint);
    shadowRayDirection.normalize();
    for (unsigned int k = 0; k < scene->getObjects().size (); k++) {
      const Object & o = scene->getObjects()[k];
      Ray ray (intersectionPoint + epsilon * shadowRayDirection - o.getTrans (), shadowRayDirection);
      if(ray.intersect(o.getMesh(), o.getMesh().kdTree, intersectionPointShadow, smallestIntersectionDistanceShadow, intersectionTriangleShadow))
	counter += (1./nbSamples);
    }
  }

  return 1. - counter;
}

float RayTracer::shadowRay(const Vec3Df &intersectionPoint, 
			       const unsigned int nbSamples) const{
  switch(shadowOpt){
  case RAYTRACER_NO_SHADOW: 
    return 1.0;
    break;
  case RAYTRACER_HARD_SHADOW:
    return hardShadowRay(intersectionPoint);
    break;
  case RAYTRACER_SOFT_SHADOW:
    return softShadowRay(intersectionPoint, nbSamples);
    break;
  }
  return 1.0;
}

