// *********************************************************
// Light Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "Light.h"
#include <stdlib.h>

void Light::setNormal(const Vec3Df &normal) {
	this->normal = normal;
	this->normal.normalize();
	if (normal[0] != 0)
		secondVector = Vec3Df(-normal[1], normal[0], 0);
	else
		secondVector = Vec3Df(0, -normal[2], normal[1]);
	secondVector.normalize();
	thirdVector = Vec3Df::crossProduct(normal, secondVector);
	secondVector.normalize();
}

Vec3Df Light::getRandomPoint() const {
	float randomAngle = ((float)rand()/(float)RAND_MAX)*2*M_PI;
	float randomDistance = ((float)rand()/(float)RAND_MAX);
	Vec3Df randomVector = cosf(randomAngle)*secondVector + sinf(randomAngle)*thirdVector;
	randomVector *= radius*sqrt(randomDistance);
	randomVector += pos;
	return randomVector;
}
