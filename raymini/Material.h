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
    inline Material (float diffuse=0.8f,
			float specular=0.2f,
			const Vec3Df & color=Vec3Df(0.5f,0.5f,0.5f),
			float reflectance=0.0f,
			const Vec3Df &emitance=Vec3Df(0,0,0),
			bool isTransparent=false,
			float probaTransmission=0.0f,
			float probaReflection=1.0f,
			float refractionIndex=1.0f)
        : diffuse (diffuse),
		specular (specular),
		color (color),
		reflectance (reflectance),
		emitance(emitance),
		transparent(isTransparent),
		probaTransmission(probaTransmission),
		probaReflection(probaReflection),
		refractionIndex(refractionIndex){}
    virtual ~Material () {}

    inline float getDiffuse () const { return diffuse; }
    inline float getSpecular () const { return specular; }
    inline Vec3Df getColor () const { return color; }
	inline float getReflectance () const { return reflectance; }
	inline const Vec3Df & getEmitance() const {return emitance;}
	inline bool isTransparent() const { return transparent;}
	inline float getProbaTransmission() const {return probaTransmission;}
	inline float getProbaReflection() const {return probaReflection;}
	inline float getRefractionIndex() const {return refractionIndex;}

    inline void setDiffuse (float d) { diffuse = d; }
    inline void setSpecular (float s) { specular = s; }
    inline void setColor (const Vec3Df & c) { color = c; }
    inline void setReflectance (float r) { reflectance = r; }
	inline void setEmitance (const Vec3Df &emitance_) {emitance = emitance_;}
    
private:
    float diffuse;
    float specular;
    Vec3Df color;
    float reflectance;
	Vec3Df emitance;
	bool transparent;
	float probaTransmission;
	float probaReflection;
	float refractionIndex;

	
};


#endif // MATERIAL_H

// Some Emacs-Hints -- please don't remove:
//
//  Local Variables:
//  mode:C++
//  tab-width:4
//  End:
