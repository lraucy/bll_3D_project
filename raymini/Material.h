// *********************************************************
// Material Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2010 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#ifndef MATERIAL_H
#define MATERIAL_H

#include <iostream>
#include <vector>

#include "Vec3D.h"

// Ce modèle suppose une couleur spéculaire blanche (1.0, 1.0, 1.0)

class Material {
public:
    inline Material () : diffuse (0.8f), specular (0.2f), color (0.5f, 0.5f, 0.5f) , reflectance (0.0f), emitance(Vec3Df(0,0,0)) {}
    inline Material (float diffuse, float specular, const Vec3Df & color)
        : diffuse (diffuse), specular (specular), color (color), reflectance (0.0f), emitance(Vec3Df(0,0,0)) {}
	inline Material (float diffuse, float specular, const Vec3Df & color, float reflectance)
        : diffuse (diffuse), specular (specular), color (color), reflectance (reflectance), emitance(Vec3Df(0,0,0)){}
	inline Material (float diffuse, float specular, const Vec3Df & color, Vec3Df emitance)
        : diffuse (diffuse), specular (specular), color (color), emitance(emitance){}
    virtual ~Material () {}

    inline float getDiffuse () const { return diffuse; }
    inline float getSpecular () const { return specular; }
    inline Vec3Df getColor () const { return color; }
	inline float getReflectance () const { return reflectance; }
	inline Vec3Df getEmitance () const { return emitance; }

    inline void setDiffuse (float d) { diffuse = d; }
    inline void setSpecular (float s) { specular = s; }
    inline void setColor (const Vec3Df & c) { color = c; }
    inline void setReflectance (float r) { reflectance = r; }

    
private:
    float diffuse;
    float specular;
    Vec3Df color;
    float reflectance;
	Vec3Df emitance;
};


#endif // MATERIAL_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
