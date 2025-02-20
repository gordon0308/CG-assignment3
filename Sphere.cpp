/*********************************************************
 Version 1.1
 
 Code provided by Michael Hemsley, Anthony Dick and Gustavo Carneiro
 for the course 
 COMP SCI 3014/7090 Computer Graphics
 School of Computer Science
 University of Adelaide
 
 Permission is granted for anyone to copy, use, modify, or distribute this
 program and accompanying programs and documents for any purpose, provided
 this copyright notice is retained and prominently displayed, along with
 a note saying that the original programs are available from the aforementioned 
 course web page. 
 
 The programs and documents are distributed without any warranty, express or
 implied.  As the programs were written for research purposes only, they have
 not been tested to the degree that would be advisable in any important
 application.  All use of these programs is entirely at the user's own risk.
 *********************************************************/

/*
 * sphere.cpp
 *
 * Using method described:
 * http://stackoverflow.com/questions/5988686/how-do-i-create-a-3d-sphere-in-opengl-using-visual-c
 * and various other places.
 */

#include <math.h>

#include "Sphere.hpp"

Sphere::Sphere(float radius, int vertDiv, int horzDiv)
{
	// Force minimum values
	if (vertDiv < 1) vertDiv = 1;
	if (horzDiv < 4) horzDiv = 4;

	vertDiv = vertDiv+2;	// Add 2 for top and bottom-cap 'divisions'
	float V = 1.0/(float)(vertDiv-1);
	float H = 1.0/(float)(horzDiv-1);
	int v, h;

	vertices = new float[vertDiv*horzDiv*3];
	normals = new float[vertDiv*horzDiv*3];
    texCoords = new float[vertDiv*horzDiv*2];
	indices = new unsigned int[vertDiv*horzDiv*6];

	vertCount = 0;
	normCount = 0;
    texCount = 0;
    indCount = 0;

	// Construct vertex, normal and tex-coords
	for (v=0; v<vertDiv; v++) {
        for (h=0; h<horzDiv; h++) {
            float x = cos(2*M_PI * h * H) * sin(M_PI * v * V);
            float y = sin(-M_PI_2 + M_PI * v * V);
            float z = sin(2*M_PI * h * H) * sin(M_PI * v * V);

            vertices[vertCount++] = x*radius;
            vertices[vertCount++] = y*radius;
            vertices[vertCount++] = z*radius;

            normals[normCount++] = x;
            normals[normCount++] = y;
            normals[normCount++] = z;
            
            texCoords[texCount++] = h*H;
            texCoords[texCount++] = v*V;
        }
    }
    
	// Construct indices
	// Start with special case of bottom sub-division where all vertices meet
	v = 0;
	for (h=0; h<horzDiv-1; h++) {
		indices[indCount++] = v * horzDiv + h;
		indices[indCount++] = (v+1) * horzDiv + h;
		indices[indCount++] = (v+1) * horzDiv + (h+1);
	}

	// Middle divisions
	for (v=1; v<vertDiv-2; v++) {
        for (h=0; h<horzDiv-1; h++) {

            indices[indCount++] = v * horzDiv + h;
            indices[indCount++] = (v+1) * horzDiv + (h+1);
            indices[indCount++] = v * horzDiv + (h+1);

            indices[indCount++] = v * horzDiv + h;
            indices[indCount++] = (v+1) * horzDiv + h;
            indices[indCount++] = (v+1) * horzDiv + (h+1);
        }
	}

	// Cap off the top of the sphere
	v = vertDiv-2;
	for (h=0; h<horzDiv-1; h++) {
		indices[indCount++] = v * horzDiv + h;
		indices[indCount++] = (v+1) * horzDiv + (h+1);
		indices[indCount++] = v * horzDiv + (h+1);
	}


}

