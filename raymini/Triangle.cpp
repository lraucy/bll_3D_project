// *********************************************************
// Triangle Class
// Author : Tamy Boubekeur (boubek@gmail.com).
// Copyright (C) 2008 Tamy Boubekeur.
// All rights reserved.
// *********************************************************

#include "Triangle.h"

using namespace std;

ostream & operator<< (ostream & output, const Triangle & t) {
    output << t.getVertex (0) << " " << t.getVertex (1) << " " << t.getVertex (2);
    return output;
}


float air(const Vec3Df t[3]){
    float det1, det2, det3;
    det1 = t[0][0] * (t[1][1] - t[2][1]) - t[0][1] * (t[1][0] - t[2][0]) + (t[1][0]*t[2][1] - t[2][0]*t[1][1]);
    det2 = t[0][1] * (t[1][2] - t[2][2]) - t[0][2] * (t[1][1] - t[2][1]) + (t[1][1]*t[2][2] - t[2][1]*t[1][2]);
    det3 = t[0][2] * (t[1][0] - t[2][0]) - t[0][0] * (t[1][2] - t[2][2]) + (t[1][2]*t[2][0] - t[2][2]*t[1][0]);
    return (sqrt(det1*det1 + det2*det2 + det3*det3)/2);
}

Vec3Df Triangle::getBaricentricCoo(const Vec3Df triangle[3], const Vec3Df &point)
{
	// calculate barycentric coordinates
    Vec3Df PA, PB, PC;
    PA = triangle[0] - point;
    PB = triangle[1] - point;
    PC = triangle[2] - point;
    
    
    float t_air = air(triangle);
    Vec3Df t_1[3] = {point, triangle[1], triangle[2]};
    float alpha = air(t_1) / t_air;
    Vec3Df t_2[3] = {triangle[0], point, triangle[2]};
    float beta = air(t_2) / t_air;
    Vec3Df t_3[3] = {triangle[0], triangle[1], point};
    float gama = air(t_3) / t_air;

    return Vec3Df(alpha, beta, gama);
}
