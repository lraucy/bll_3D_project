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

void RayTracer::setShadowRadius(float _shadowRadius) {
	Scene * scene = Scene::getInstance();
	if(scene->getLights().size() > 0)
		scene->getLights()[0].setRadius(_shadowRadius);
}

float RayTracer::getShadowRadius() {
	Scene * scene = Scene::getInstance();
	if(scene->getLights().size() > 0)
		return scene->getLights()[0].getRadius();
	else
		return 0.0;
}


// POINT D'ENTREE DU PROJET.
// Le code suivant ray trace uniquement la boite englobante de la scene.
// Il faut remplacer ce code par une veritable raytracer
QImage RayTracer::render (const QImage &image_,
		const Vec3Df & camPos,
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
	QImage image;
	if(iterationPathTracingLoic == 0)
		image = QImage(QSize (screenWidth, screenHeight), QImage::Format_RGB888);
	else
		image = QImage(image_);
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

			if(tracing == PATH_TRACING_LOIC) {
				Vec3Df old_color(0,0,0);
				if(iterationPathTracingLoic != 0)
					old_color = Vec3Df(qRed(image.pixel(i,j)), qGreen(image.pixel(i,j)), qBlue(image.pixel(i,j)));
				c = (old_color*sqrt(iterationPathTracingLoic) + c)/sqrt(iterationPathTracingLoic+iterationPerTracingLoic);
			}

			image.setPixel (i, j, qRgb (clamp (c[0], 0, 255), clamp (c[1], 0, 255), clamp (c[2], 0, 255)));
		}
	}
	if(tracing == PATH_TRACING_LOIC)
		iterationPathTracingLoic += iterationPerTracingLoic;
	progressDialog.setValue (100);
	return image;
}

const Object * RayTracer::getObjectIntersected(Ray &ray,
							Vec3Df &intersectionPoint, Triangle &intersectionTriangle) const{

	Scene * scene = Scene::getInstance();

	float smallestIntersectionDistance = 1000000.f;
	const Object *objectIntersected = NULL;

	for (unsigned int k = 0; k < scene->getObjects().size (); k++) {
		const Object & o = scene->getObjects()[k];
		ray.translate(-o.getTrans());

		if(ray.intersect(o.getMesh(), o.getMesh().kdTree, intersectionPoint, smallestIntersectionDistance, intersectionTriangle))
			objectIntersected = &o;

		ray.translate(o.getTrans());
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

Vec3Df RayTracer::getPhongBRDFWithLights(const Ray &ray, const Object &o, const Vec3Df &intersectionPoint, const Vec3Df &normal, const std::vector<Light> lights) const{


	Vec3Df color(0.0f, 0.0f, 0.0f);
	for (unsigned int i = 0; i < lights.size(); i++) {
	  Light l = lights[i];
		Vec3Df toLight = l.getPos() - intersectionPoint;
		toLight.normalize();
		Vec3Df r = (ray.getOrigin() - intersectionPoint);
		r.normalize();
		color += l.getIntensity() * l.getColor() * getPhongBRDF(r, toLight, normal, o.getMaterial());
	}

	color = color * 255;
	return color;
}

Vec3Df RayTracer::getPhongBRDF(const Vec3Df &eyeVector, const Vec3Df &lightVector, const Vec3Df &normal, const Material &material) const {
	float shininess = 16.0f;

	Vec3Df ref = 2*Vec3Df::dotProduct(normal, lightVector)*normal - lightVector;
	ref.normalize();

	float diffuseTerm = material.getDiffuse() * std::max(Vec3Df::dotProduct(lightVector, normal), 0.0f);
	float specTerm = material.getSpecular() * pow(std::max(Vec3Df::dotProduct(ref, eyeVector), 0.0f), shininess);
	return (diffuseTerm + specTerm) * material.getColor();

}



Vec3Df RayTracer::getPhongBRDF(const Ray &ray, const Object &o,	const Vec3Df &intersectionPoint, const Vec3Df &normal) const{
  return getPhongBRDFWithLights(ray, o, intersectionPoint, normal, Scene::getInstance()->getLights());
}

#define MAX_DEPTH 2
#define NB_DIR 10

Vec3Df RayTracer::pathTracer(const Vec3Df &camPos, const Vec3Df &dir) const {
  Triangle intersectionTriangle;
  Vec3Df intersectionPoint;
  const Object * objectIntersected = NULL;
  
  Ray ray (camPos, dir);
  objectIntersected = getObjectIntersected(ray, intersectionPoint, intersectionTriangle);
  
  if(objectIntersected != NULL){
    Ray ray (camPos-objectIntersected->getTrans (), dir);
    Vec3Df normal = getNormalAtIntersection(*objectIntersected, intersectionPoint, intersectionTriangle);
    
    Vertex  intP(intersectionPoint, normal);
    return pathTracerRec(ray, *objectIntersected, intP, 0);
  }
  return Vec3Df(0.f, 0.f, 0.f);
}
  
Vec3Df RayTracer::pathTracerRec(const Ray & ray, const Object & intersectedObject, const Vertex & intP, unsigned int depth) const{ 
  Vec3Df color;
  std::vector<Light> pathTracingLights;
  float a = 1.0;
  float b=0.0;
  float c=0.5;
  float intensity;
  float distance;
  if(depth == pathTraceDepth){
    color =  getPhongBRDF(ray, intersectedObject, intP.getPos(), intP.getNormal());
    color /= (pow(2, depth));
    return color;
  }

  std::vector<Vec3Df> direction = getRandomDirections(intP, pathTraceAngle);
  Vec3Df pos = intP.getPos() + intersectedObject.getTrans();
  
  for(unsigned int i = 0; i<direction.size(); i++){
        const Object * intObject = NULL;
        Vec3Df intersectionPoint;
        Vec3Df lightColor;
        Vec3Df intPos;
	Triangle intersectionTriangle;

	Ray rayInter (pos, direction[i]);
	intObject = getObjectIntersected(rayInter,intersectionPoint,intersectionTriangle);
	
	if(intObject != NULL){
	  Ray newRay(pos, direction[i]);
	  Vec3Df normal = getNormalAtIntersection(*intObject,intersectionPoint,intersectionTriangle);
	  Vertex newIntVertex(intersectionPoint, normal);
	  lightColor = this->pathTracerRec(newRay, *intObject, newIntVertex, depth+1);
	  intPos = newIntVertex.getPos() + intObject->getTrans();

	  distance = Vec3Df::distance(intP.getPos(), intersectionPoint); 
	  intensity = 1/(a+b*distance+c*pow(distance,2));
	  pathTracingLights.push_back(Light(intPos, Vec3Df(1.0,1.0,1.0), intensity)); // !! Uses white lights
	}
	 

  }
  
  color = getPhongBRDFWithLights(ray, intersectedObject, intP.getPos(), intP.getNormal(), pathTracingLights)/pathTracingLights.size();
  color /= (pow(2, depth));;

  return color;
}

std::vector<Vec3Df> RayTracer::getRandomDirections(const Vertex & v, const float angle) const{

  std::vector<Vec3Df> directions;
  Vec3Df normal = v.getNormal();
  Vec3Df secondVec(-normal[1],normal[0],0.0); // compute second perpendicular vector
  Vec3Df thirdVec = Vec3Df::crossProduct(normal, secondVec);
  secondVec.normalize();
  thirdVec.normalize();

  for (unsigned int i=0; i<pathTraceNbRay; i++) {
    Ray *ray = Ray::getRandomRay(v.getPos(), normal, secondVec, thirdVec, angle);
    directions.push_back(ray->getDirection());
  }
  
  return directions;
}


Vec3Df RayTracer::getPhongBRDFReflectance(const Ray &ray, const Object &o, const Vec3Df &intersectionPoint,
					  const Vec3Df &normal) const{

    float reflectance = o.getMaterial().getReflectance();
    float epsilon = 0.001f;
    if(reflectance > 0.0f) {
	Vec3Df reflectedRay = 2 * normal * (Vec3Df::dotProduct(-ray.getDirection(), normal)) + ray.getDirection();//  tmp * dir;
  	reflectedRay.normalize();
	Vec3Df point = intersectionPoint + epsilon * reflectedRay;
	return reflectance * getColorFromRay(point, reflectedRay);
    }
    return Vec3Df(0.0f, 0.0f, 0.0f);
}

Vec3Df RayTracer::getColorFromRay(const Vec3Df &camPos, const Vec3Df &dir) const {
	switch(tracing) {
		case RAY_TRACING:
			return getColorFromRayWithRayTracing(camPos, dir);
			break;
		case PATH_TRACING:
			return pathTracer(camPos, dir);
			break;
		case PATH_TRACING_LOIC:
			Ray ray(camPos, dir);
			Vec3Df color(0.0,0.0,0.0);
			for (unsigned int i = 0; i < iterationPerTracingLoic; i++)
				color += tracePathLoic(ray, 0);
			return color;
			break;
	}
	return getColorFromRayWithRayTracing(camPos, dir);
}




Vec3Df RayTracer::getColorFromRayWithRayTracing(const Vec3Df &camPos, const Vec3Df &dir) const {
	Vec3Df c (backgroundColor);
	
	Triangle intersectionTriangle;
	Vec3Df intersectionPoint;
	const Object * objectIntersected = NULL;

	Ray ray (camPos, dir);
	objectIntersected = getObjectIntersected(ray, intersectionPoint,
			intersectionTriangle);
	if(objectIntersected != NULL) {
		Ray ray (camPos, dir);
		Vec3Df normal = getNormalAtIntersection(*objectIntersected, intersectionPoint,
				intersectionTriangle);
		Vec3Df intersectionPointGlobalMark = intersectionPoint + objectIntersected->getTrans();
		c = getPhongBRDF(ray, *objectIntersected, intersectionPointGlobalMark, normal);
		c += ambientOcclusion(intersectionPointGlobalMark, normal) * objectIntersected->getMaterial().getColor();

		normal.normalize();
		c += getPhongBRDFReflectance(ray, *objectIntersected, intersectionPointGlobalMark, normal);
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
  float epsilon = 0.001;
  float intersectionShadow = 0.0;

  for (unsigned int i = 0; i < scene->getLights().size(); i++)
  {
	  Vec3Df shadowRayDirection = scene->getLights()[i].getPos() - intersectionPoint;
	  for (unsigned int k = 0; k < scene->getObjects().size(); k++) {
		const Object &o = scene->getObjects()[k];
		Ray ray(intersectionPoint + epsilon * shadowRayDirection - o.getTrans(), shadowRayDirection);
		ray.isASegment = true;
		ray.intersectReverseTriangles = false;
		if(ray.intersect(o.getMesh(), o.getMesh().kdTree))
			intersectionShadow += 1.0;
	  }

  }
  return 1.0 - intersectionShadow / scene->getLights().size();
}

float RayTracer::softShadowRay(const Vec3Df &intersectionPoint, 
			       const unsigned int nbSamples) const{
  Scene * scene = Scene::getInstance();
  float epsilon = 0.001;
  float counter = 0.0;
  
  for (unsigned int i = 0; i < scene->getLights().size(); i++) {
	  for (unsigned int j = 0; j < nbSamples; j++) {
		  Vec3Df shadowRayDirection = scene->getLights()[i].getRandomPoint() - intersectionPoint;
		  for (unsigned int k = 0; k < scene->getObjects().size(); k++) {
			const Object &o = scene->getObjects()[k];
			Ray ray (intersectionPoint + epsilon * shadowRayDirection - o.getTrans (), shadowRayDirection);
			ray.isASegment = true;
			ray.intersectReverseTriangles = true;
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


Vec3Df RayTracer::tracePathLoic(Ray &ray, unsigned int depth) const {
	if (depth == pathTraceDepthLoic)
		return Vec3Df(0.0, 0.0, 0.0);

	Vec3Df intersectionPoint;
	Triangle intersectionTriangle;
	const Object *o = getObjectIntersected(ray, intersectionPoint, intersectionTriangle);
	if (o != NULL) {
		if((float)rand()/RAND_MAX < (float)depth/(pathTraceDepthLoic))
			return o->getMaterial().getEmitance();

		Vec3Df normal = getNormalAtIntersection(*o, intersectionPoint, intersectionTriangle);
		Vec3Df intersectionPointGlobalMark = intersectionPoint + o->getTrans();

		if(o->getMaterial().isTransparent()) {
			if((float)rand()/RAND_MAX < o->getMaterial().getProbaTransmission()) {
				float epsilon = 0.01;

				if (ray.reverseTriangleIntersected)
					normal = -normal;

				Vec3Df planVector = Vec3Df::crossProduct(normal, Vec3Df::crossProduct(-ray.getDirection(), normal));
				float sinTheta1 = planVector.getLength();
				float n = (ray.reverseTriangleIntersected) ? o->getMaterial().getRefractionIndex() : 1.0 / o->getMaterial().getRefractionIndex();
				float sinTheta2 = n * sinTheta1;
				Vec3Df newDir;
				if(sinTheta2 <= 1)
					newDir = sinTheta2 * (-planVector) + sqrt(1-sinTheta2*sinTheta2) * (-normal);
				else
					newDir = sinTheta1 * (-planVector) + sqrt(1-sinTheta1*sinTheta1) * normal;

				Ray newRay(intersectionPointGlobalMark + epsilon * newDir, newDir);
				newRay.intersectReverseTriangles = true;
				return tracePathLoic(newRay, depth);
			}
		}

		Ray * newRay = getRandomRay(intersectionPointGlobalMark, normal);

		Vec3Df colorReflected = tracePathLoic(*newRay, depth+1);
		float cosOmega = std::max(Vec3Df::dotProduct(newRay->getDirection(), normal), 0.0f);

		Vec3Df color = cosOmega * getPhongBRDF(-ray.getDirection(), newRay->getDirection(), normal, o->getMaterial()) * colorReflected;
		delete newRay;
		return color;

	}
	else
		return Vec3Df(0.0f, 0.0f, 0.0f);
}

Ray * RayTracer::getRandomRay(const Vec3Df &origin, const Vec3Df &normal) const {
	Vec3Df secondVec;
	Vec3Df thirdVec;

	if (normal[0] == 0.0) {
		secondVec[0] = 0.0f;
		secondVec[1] = - normal[2];
		secondVec[2] = normal[1];
	}
	else {
		secondVec[0] = - normal[1];
		secondVec[1] = normal[0];
		secondVec[2] = 0.0f;
	}
	thirdVec = Vec3Df::crossProduct(normal, secondVec);

	return Ray::getRandomRay(origin, normal, secondVec, thirdVec, 90.0);
}
