/*
LibBoubek/BoundingBox.cpp
Copyright (c) 2003-2008, Tamy Boubekeur

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* The name of the source code copyright owner cannot be used to endorse or
  promote products derived from this software without specific prior
  written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "BoundingBox.h"

/**
 * This function is used to create a bounding box large enough to include all triangles in vector triangles
 * from index begin to end
 */
BoundingBox::BoundingBox (const Mesh & mesh, std::vector<Triangle> & triangles) {
	const std::vector<Vertex> & vertices = mesh.getVertices();

	this->minBb = vertices[triangles[0].getVertex(0)].getPos();
	this->maxBb = this->minBb;
	for(unsigned int i = 0; i < triangles.size(); i++) {
		for (unsigned int j = 0; j < 3; j++) {
			this->extendTo(vertices[triangles[i].getVertex(j)].getPos());
		}
	}
}

unsigned int BoundingBox::biggerAxe() {
	Vec3Df axes = (this->maxBb - this->minBb);
	unsigned int axe = 0;
	if (axes[0] < axes[1])
		axe = 1;
	if (axes[axe] < axes[2])
		axe = 2;

	return axe;
}
