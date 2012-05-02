// *********************************************************
// Scene Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "Scene.h"

using namespace std;

static Scene * instance = NULL;

Scene * Scene::getInstance () {
    if (instance == NULL)
        instance = new Scene ();
    return instance;
}

void Scene::destroyInstance () {
    if (instance != NULL) {
        delete instance;
        instance = NULL;
    }
}

Scene::Scene () {
    buildDefaultScene ();
    updateBoundingBox ();
}

Scene::~Scene () {
}

void Scene::updateBoundingBox () {
    if (objects.empty ())
        bbox = BoundingBox ();
    else {
        bbox = objects[0].getBoundingBox ();
        for (unsigned int i = 1; i < objects.size (); i++)
            bbox.extendTo (objects[i].getBoundingBox ());
    }
}

// Changer ce code pour creer des scenes originales
void Scene::buildDefaultScene () {

    Mesh groundMesh;
    groundMesh.loadOFF ("models/ground.off");
    Material groundMat(0.8f, 0.2f, Vec3Df(0.5f, 0.5f, 0.5f), 0.f) ;
    Object ground (groundMesh, groundMat, "Ground");
    ground.setTrans(Vec3Df(0.f, 0.f, 0.f));
    objects.push_back (ground);

    Mesh ceilingMesh;
    ceilingMesh.loadOFF ("models/ceiling.off");
    Material ceilingMat(0.8f, 0.2f, Vec3Df(0.5f, 0.5f, 0.5f), 0.f) ;
    Object ceiling (ceilingMesh, ceilingMat, "Ground");
    ceiling.setTrans(Vec3Df(0.f, 0.f, 4.f));
    objects.push_back (ceiling);

	Mesh wallRightMesh;
    wallRightMesh.loadOFF ("models/WallRight.off");
    Material wallRightMat(0.8f, 0.2f, Vec3Df(0.5f, 0.5f, 0.5f), 0.f) ;
    Object wallRight (wallRightMesh, wallRightMat, "WallRight");
    wallRight.setTrans(Vec3Df(2.f, 0.f, 2.f));
    objects.push_back (wallRight);

	Mesh wallLeftMesh;
    wallLeftMesh.loadOFF ("models/WallLeft.off");
    Material wallLeftMat(0.8f, 0.2f, Vec3Df(0.5f, 0.5f, 0.5f), 0.f) ;
    Object wallLeft (wallLeftMesh, wallLeftMat, "WallLeft");
    wallLeft.setTrans(Vec3Df(-2.f, 0.f, 2.f));
    objects.push_back (wallLeft);

	Mesh wallBackMesh;
    wallBackMesh.loadOFF ("models/WallBack.off");
    Material wallBackMat(0.8f, 0.2f, Vec3Df(0.5f, 0.5f, 0.5f), 0.f) ;
    Object wallBack (wallBackMesh, wallBackMat, "WallBack");
    wallBack.setTrans(Vec3Df(0.f, -2.f, 2.f));
    objects.push_back (wallBack);

	Mesh wallFrontMesh;
    wallFrontMesh.loadOFF ("models/WallFront.off");
    Material wallFrontMat(0.8f, 0.2f, Vec3Df(0.5f, 0.5f, 0.5f), 0.f) ;
    Object wallFront (wallFrontMesh, wallFrontMat, "WallFront");
    wallFront.setTrans(Vec3Df(0.f, 2.f, 2.f));
    objects.push_back (wallFront);

    Mesh ramMesh;
    ramMesh.loadOFF ("models/ram.off");
    Material ramMat (1.f, 1.f, Vec3Df (1.f, .6f, .2f));
    Object ram (ramMesh, ramMat, "ram");
    ram.setTrans (Vec3Df (1.f, 0.5f, 0.f));
    objects.push_back (ram);
    Mesh rhinoMesh;
    rhinoMesh.loadOFF ("models/rhino.off");
    Material rhinoMat (1.0f, 0.2f, Vec3Df (0.6f, 0.6f, 0.7f));
    Object rhino (rhinoMesh, rhinoMat, "rhino");
    rhino.setTrans (Vec3Df (-1.f, -1.0f, 0.4f));
    objects.push_back (rhino);
    Mesh gargMesh;
    gargMesh.loadOFF ("models/gargoyle.off");
    Material gargMat (0.7f, 0.4f, Vec3Df (0.5f, 0.8f, 0.5f));
    Object garg (gargMesh, gargMat, "gargoyle");
    garg.setTrans (Vec3Df (-2.f, 0.0f, 0.f));
    objects.push_back (garg);
	for (unsigned int i = 0; i < objects.size(); i++)
		objects[i].getMesh().buildKdTree();

    Light l1 (Vec3Df (0.0f, 0.0f, 3.6f), Vec3Df (1.0f, 1.0f, 1.0f), 1.0f, 1.0f, Vec3Df(0.f, 0.f, -3.6f));
    lights.push_back (l1);
    Light l2 (Vec3Df (0.0f, 5.0f, 5.0f), Vec3Df (1.0f, 1.0f, 1.0f), 1.0f, 1.0f, Vec3Df(0.0f, -5.0f, -5.0f));
    lights.push_back (l2);
}
