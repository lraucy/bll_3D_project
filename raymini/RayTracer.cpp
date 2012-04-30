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

			Vec3Df c = getColor(camPos, dir);
			
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

	float diffuseRef = o.getMaterial().getDiffuse();
	float specRef = o.getMaterial().getSpecular();
	float shininess = 16.0f;
	Vec3Df colorVect = o.getMaterial().getColor();

	float diffuseTerm = 0.0;
	float specTerm = 0.0;

	for (unsigned int i = 0; i < scene->getLights().size(); i++) {
		Vec3Df toLight = scene->getLights()[i].getPos() - intersectionPoint;
		toLight.normalize();
		Vec3Df r = (ray.getOrigin() - intersectionPoint);
		r.normalize();
		Vec3Df ref = 2*Vec3Df::dotProduct(normal, toLight)*normal - toLight;
		ref.normalize();

		diffuseTerm += std::max(Vec3Df::dotProduct(toLight, normal), 0.0f);
		specTerm += pow(std::max(Vec3Df::dotProduct(ref, r), 0.0f), shininess);
	}

	float color = specRef * specTerm + diffuseRef*diffuseTerm;

	color = color*255;
	return Vec3Df(color*colorVect[0], color*colorVect[1], color*colorVect[2]);
}

Vec3Df RayTracer::getColorFromRay(const Vec3Df &camPos, const Vec3Df &dir) const {
	Vec3Df c (backgroundColor);
	
	Triangle intersectionTriangle;
	Vec3Df intersectionPoint;
	const Object * objectIntersected = NULL;

	objectIntersected = getObjectIntersected(camPos, dir, intersectionPoint,
			intersectionTriangle);
	if(objectIntersected != NULL) {
		Ray ray (camPos, dir);
		Vec3Df normal = getNormalAtIntersection(*objectIntersected, intersectionPoint,
				intersectionTriangle);
		Vec3Df intersectionPointGlobalMark = intersectionPoint + objectIntersected->getTrans();
		c = getPhongBRDF(ray, *objectIntersected, intersectionPointGlobalMark, normal);
		c += ambientOcclusion(intersectionPointGlobalMark, normal) * objectIntersected->getMaterial().getColor();

		float coef = shadowRay(intersectionPointGlobalMark);
		c = c * Vec3Df(coef, coef, coef);
	}
	return c;
}

Vec3Df RayTracer::getColorFromPixel(const Vec3Df &camPos, const Vec3Df &dir) const {
  return getColorFromRay(camPos, dir);
}

Vec3Df RayTracer::getColorFromPixelWithAAx2(const Vec3Df &camPos, const Vec3Df &dir) const {
	Vec3Df color = Vec3Df(0.0f, 0.0f, 0.0f);
	
	for (unsigned int i = 0; i < 2; i++) {
		for (unsigned int j = 0; j < 2; j++) {
			Vec3Df aaDir = dir + (0.5 - i)*tanX*rightVector/(2.0*screenWidth)
				+ (0.5 - j)*tanY*upVector/(2.0*screenHeight);
			color += getColorFromRay(camPos, aaDir);
		}
	}
	
	return color / 4;
}

Vec3Df RayTracer::getColorFromPixelWithAAx3(const Vec3Df &camPos, const Vec3Df &dir) const {
	Vec3Df color = Vec3Df(0.0f, 0.0f, 0.0f);
	
	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			Vec3Df aaDir = dir + i*tanX*rightVector/(4.0*screenWidth)
				+ j*tanY*upVector/(4.0*screenHeight);
			color += getColorFromRay(camPos, aaDir);
		}
	}
	return color / 9;
}

Vec3Df RayTracer::getColor(const Vec3Df &camPos, const Vec3Df &dir) const {
  switch(aaOpt){
  case RAYTRACER_NO_AA: 
    return getColorFromPixel(camPos, dir);
    break;
  case RAYTRACER_AAx2:
    return getColorFromPixelWithAAx2(camPos, dir);
    break;
  case RAYTRACER_AAx3:
    return getColorFromPixelWithAAx3(camPos, dir);
    break;
  }
  return getColorFromPixel(camPos, dir); 
}

float RayTracer::hardShadowRay(const Vec3Df &intersectionPoint) const{
  Scene * scene = Scene::getInstance();
  float epsilon = 0.1;
  float intersectionShadow = 0.0;

  for (unsigned int i = 0; i < scene->getLights().size(); i++)
  {
	  Vec3Df shadowRayDirection = scene->getLights()[i].getPos() - intersectionPoint;
	  shadowRayDirection.normalize();
	  for (unsigned int k = 0; k < scene->getObjects().size(); k++) {
		const Object &o = scene->getObjects()[k];
		Ray ray(intersectionPoint + epsilon * shadowRayDirection - o.getTrans(), shadowRayDirection);
		if(ray.intersect(o.getMesh(), o.getMesh().kdTree))
			intersectionShadow += 1.0;
	  }

  }
  return 1.0 - intersectionShadow / scene->getLights().size();
}

float RayTracer::softShadowRay(const Vec3Df &intersectionPoint, 
			       const unsigned int nbSamples) const{
  Scene * scene = Scene::getInstance();
  float epsilon = 0.1;
  float counter = 0;
  
  for (unsigned int i = 0; i < scene->getLights().size(); i++) {
	  for (unsigned int j = 0; j < nbSamples; j++) {
		  Vec3Df shadowRayDirection = ((scene->getLights()[i].getPos() +
					  Vec3Df((rand()/(double)RAND_MAX) * scene->getLights()[i].getRadius(),
						  (rand()/(double)RAND_MAX) * scene->getLights()[i].getRadius(),
						  (rand()/(double)RAND_MAX) * scene->getLights()[i].getRadius()))) - intersectionPoint;
		  shadowRayDirection.normalize();
		  for (unsigned int k = 0; k < scene->getObjects().size(); k++) {
			const Object &o = scene->getObjects()[k];
			Ray ray (intersectionPoint + epsilon * shadowRayDirection - o.getTrans (), shadowRayDirection);
			if(ray.intersect(o.getMesh(), o.getMesh().kdTree))
				counter += (1.0/nbSamples/scene->getLights().size());
		  }
	  }
  }
  return 1. - counter;
}

float RayTracer::shadowRay(const Vec3Df &intersectionPoint) const{
  switch(shadowOpt){
  case RAYTRACER_NO_SHADOW: 
    return 1.0;
    break;
  case RAYTRACER_HARD_SHADOW:
    return hardShadowRay(intersectionPoint);
    break;
  case RAYTRACER_SOFT_SHADOW:
    return softShadowRay(intersectionPoint, shadowNbRay);
    break;
  }
  return 1.0;
}


float RayTracer::ambientOcclusion(const Vec3Df &intersectionPoint, const Vec3Df &normal) const{
	if(aoOpt)
		return aoCoeff * 255 * computeAmbientOcclusion(intersectionPoint, normal, aoNbRay,
					(float)aoSphereRadius, (float)aoConeAngle*M_PI/180);
	else
		return 0.0;
}

float RayTracer::computeAmbientOcclusion(const Vec3Df &intersectionPoint,
		const Vec3Df &normal, unsigned int nbRay, float R, float coneAngle) const {
	Scene *scene = Scene::getInstance();

	R = scene->getBoundingBox().getSize() * R / 100;
	Vec3Df secondVec(-normal[1],normal[0],0.0); // compute second perpendicular vector
	Vec3Df thirdVec = Vec3Df::crossProduct(normal, secondVec);
	secondVec.normalize();
	thirdVec.normalize();

	Vec3Df intersectionOtherObject;
	float occlusionValue = 0;

	for (unsigned int i = 0; i < nbRay; i++) {
		bool intersect = false;
		Ray *ray = Ray::getRandomRay(intersectionPoint, normal, secondVec, thirdVec, coneAngle);

		for (unsigned int j = 0; j < scene->getObjects().size(); j++) {
			const Object &o = scene->getObjects()[j];
			ray->translate(-o.getTrans());
			if(ray->intersectInSphere(o.getMesh(), o.getMesh().kdTree,
						intersectionPoint - o.getTrans(), R)) {
				intersect = true;
			}
			ray->translate(o.getTrans());
			if (intersect)
				break;
		}
		if(!intersect)
			occlusionValue += Vec3Df::dotProduct(ray->getDirection(), normal);
		delete ray;
	}
	return occlusionValue / nbRay;
}

