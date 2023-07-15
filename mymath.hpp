#pragma once

#include <stdio.h>
#include <iostream>
#include <math.h>
#include "var.hpp"
#include <vector>
#include <GLFW/glfw3.h>

using namespace std;

//don't mix between initVec and initMat, which will cause heap error
//initialize 4*4 matrix
void initMat(GLfloat mat[]) {
	if (mat != NULL) {
		for (int i = 0; i < 16; i++) {
			mat[i] = 0;
		}
	}
}

//initialize vec4
void initVec(GLfloat vec[]) {
	if (vec != NULL) {
		for (int i = 0; i < 4; i++) {
			vec[i] = 0;
		}
	}

}

//return identity matrix
GLfloat* matI() {
	GLfloat* matrlt = (GLfloat*)malloc(16 * sizeof(GLfloat));

	if (matrlt != NULL) {
		initMat(matrlt);

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (i == j) {
					matrlt[i * 4 + j] = 1;
				}
			}
		}
	}

	return matrlt;
}

//mat4 transpose
void transpose(GLfloat f[16]) {
	GLfloat rlt[16];
	initMat(rlt);

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			rlt[j * 4 + i] = f[i * 4 + j];
		}
	}
	for (int i = 0; i < 16; i++) {
		f[i] = rlt[i];
	}

}

//vector addition
GLfloat* addVec(GLfloat vec1[], GLfloat vec2[]) {
	GLfloat* vecrlt = (GLfloat*)malloc(3 * sizeof(GLfloat));

	if (vecrlt != NULL) {
		for (int i = 0; i < 3; i++) {
			vecrlt[i] = vec1[i] + vec2[i];
		}
	}
	return vecrlt;
}

//vector substration
GLfloat* minusVec(GLfloat vec1[], GLfloat vec2[]) {
	GLfloat* vecrlt = (GLfloat*)malloc(3 * sizeof(GLfloat));

	if (vecrlt != NULL) {
		for (int i = 0; i < 3; i++) {
			vecrlt[i] = vec1[i] - vec2[i];
		}
	}
	return vecrlt;
}

GLfloat dotPdt(GLfloat vec1[], GLfloat vec2[]) {
	GLfloat rlt = 0;

	for (int i = 0; i < 3; i++) {
		rlt += vec1[i] * vec2[i];
	}

	return rlt;
}

GLfloat* scalarVec(GLfloat scalar, GLfloat vec[]) {
	GLfloat* vecrlt = (GLfloat*)malloc(3 * sizeof(GLfloat));

	if (vecrlt != NULL) {
		for (int i = 0; i < 3; i++) {
			vecrlt[i] = vec[i] * scalar;
		}
	}
	
	return vecrlt;
}

//takes 2 vec3 as input, output perpendicular vec3
GLfloat* crossPdt(GLfloat vec1[], GLfloat vec2[]) {
	GLfloat* vecrlt = (GLfloat*)malloc(3 * sizeof(GLfloat));

	if (vecrlt != NULL) {
		initVec(vecrlt);

		vecrlt[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
		vecrlt[1] = -(vec1[0] * vec2[2] - vec1[2] * vec2[0]);
		vecrlt[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];
	}

	return vecrlt;
}


//return the length of vector
GLfloat vecLength(GLfloat vec[]) {
	GLfloat rlt = 0;

	rlt = sqrt(pow(vec[0], 2) + pow(vec[1], 2) + pow(vec[2], 2));
	return rlt;
}

//vector normalization
GLfloat* vecNor(GLfloat vec[]) {
	GLfloat* vecrlt = (GLfloat*)malloc(3 * sizeof(GLfloat));
	GLfloat length = (vecLength(vec));

	if (length == 0) {
		return vec;
	}
	if (vecrlt != NULL) {
		for (int i = 0; i < 3; i++) {
			vecrlt[i] = vec[i] / length;
		}
	}
	return vecrlt;
}

//simple matrix addition
//works well version, all following should be like that
GLfloat* add(GLfloat mat1[], GLfloat mat2[]) {
	GLfloat* matrlt = (GLfloat*)malloc(16 * sizeof(GLfloat));

	if (matrlt != NULL) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				matrlt[4 * i + j] = mat1[4 * i + j] + mat2[4 * i + j];
			}
		}
	}

	return matrlt;
}

//simple matrix multiplication
GLfloat* multi(GLfloat mat1[], GLfloat mat2[]) {
	GLfloat* matrlt = (GLfloat*)malloc(16 * sizeof(GLfloat));

	if (matrlt != NULL) {
		initMat(matrlt);

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				for (int k = 0; k < 4; k++) {
					matrlt[(4 * i) + j] += mat1[(i * 4) + k] * mat2[(k * 4) + j];
				}
			}
		}
	}

	return matrlt;
}

//return c=Ax, where c is a vector4, x is a vector3
GLfloat* vecMatMulti(GLfloat mat[], GLfloat vec[]) {
	GLfloat* vecrlt = (GLfloat*)malloc(4 * sizeof(GLfloat));
	GLfloat* vect = (GLfloat*)malloc(4 * sizeof(GLfloat));

	if (vecrlt != NULL && vect != NULL) {
		initVec(vecrlt);
		initVec(vect);

		for (int i = 0; i < 3; i++) {
			vect[i] = vec[i];
		}
		vect[3] = 1;

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				vecrlt[i] += mat[i * 4 + j] * vect[j];

			}
		}

		//cout << vecrlt[0] << ' ' << vecrlt[1] << ' ' << vecrlt[2] << ' ' << vecrlt[3] << endl;
	}

	return vecrlt;
}


//normalize vector by w
GLfloat* vecWNor(GLfloat vec[]) {
	GLfloat* vecrlt = (GLfloat*)malloc(3 * sizeof(GLfloat));

	GLfloat x = vec[0], y = vec[1], z = vec[2], w = vec[3];

	if (vecrlt != NULL) {
		x /= w;
		y /= w;
		z /= w;
		w = 1;

		vecrlt[0] = x;
		vecrlt[1] = y;
		vecrlt[2] = z;
	}
	return vecrlt;

}

//return the slope of given point 1,2
GLfloat Slope(GLfloat pt1[], GLfloat pt2[]) {
	return (pt2[1] - pt1[1]) / (pt2[0] - pt1[0]);
}

GLfloat Slope(Ver p1, Ver p2) {
	return (p2.y - p1.y) / (p2.x - p1.x);
}//basic math linear interpolation
float lerp(GLfloat num1, GLfloat num2, float t) {
	if (t <= 1) {
		return (1 - t) * num1 + t * num2;
	}
	else return -1;

}

//distance between two 2d points
float dist2d(GLfloat* point1, GLfloat* point2) {
	return sqrt(pow(point2[1] - point1[1], 2) + pow(point2[0] - point1[0], 2));
}

float dist2d(Ver point1, Ver point2) {
	return sqrt(pow(point2.y - point1.y, 2) + pow(point2.x - point1.x, 2));
}

GLfloat vecLerp(GLfloat* point1, GLfloat* point2, GLfloat* point3) {
	float dist12 = dist2d(point1, point2);
	float dist13 = dist2d(point1, point3);

	return lerp(point1[2], point2[2], dist13 / dist12);
}

GLfloat vecLerp(Ver point1, Ver point2, Ver point3) {
	float dist12 = dist2d(point1, point2);
	float dist13 = dist2d(point1, point3);

	if (dist12 == 0 && dist13 == 0)
		return point1.z;
	else
		return lerp(point1.z, point2.z, dist13 / dist12);

}

GLfloat valueLerp(Ver point1, Ver point2, Ver point3, GLfloat v1, GLfloat v2) {
	float dist12 = dist2d(point1, point2);
	float dist13 = dist2d(point1, point3);

	if (dist12 == 0 && dist13 == 0)
		return v1;
	else
		return lerp(v1, v2, dist13 / dist12);
}

Ver	normalLerp(Ver point1, Ver point2, Ver point3, Ver normal1, Ver normal2) {
	float dist12 = dist2d(point1, point2);
	float dist13 = dist2d(point1, point3);
	Ver normalRlt;

	if (dist12 == 0 && dist13 == 0)
		return normal1;
	else
	{
		normalRlt.x = lerp(normal1.x, normal2.x, dist13 / dist12);
		normalRlt.y = lerp(normal1.y, normal2.y, dist13 / dist12);
		normalRlt.z = lerp(normal1.z, normal2.z, dist13 / dist12);

		normalRlt.normalize();

		return normalRlt;
	}
}