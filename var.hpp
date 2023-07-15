#pragma once

#include <stdio.h>
#include <vector>
#include <iostream>
#include <GLFW/glfw3.h>
#include <time.h> 
#include <map>
#include "mylib.hpp"


using namespace std;

#define FAR 1000
#define HEIGHT 1080
#define WIDTH 1920
#define PI 3.1415926

GLfloat matMVP[16];
GLfloat ivsMVP[16];

class Ver {
public:
	GLfloat x, y, z;

	Ver(void) {
		x = 0;
		y = 0;
		z = 0;
	}
	Ver(GLfloat xp, GLfloat yp, GLfloat zp) {
		x = xp;
		y = yp;
		z = zp;
	}
	Ver(GLfloat xp, GLfloat yp) {
		x = xp;
		y = yp;
		z = 0;
	}

	void makeVertex(GLfloat xp, GLfloat yp, GLfloat zp) {
		x = xp;
		y = yp;
		z = zp;
	}

	void normalize() {
		GLfloat length = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
		x /= length;
		y /= length;
		z /= length;
	}
};

class Pixel {
public:
	GLfloat r, g, b;

	Pixel() {
		r = 0;
		g = 0;
		b = 0;
	}
	Pixel(GLfloat r, GLfloat g, GLfloat b) {
		this->r = r;
		this->g = g;
		this->b = b;
	}

	//Assign given RGB to pixel
	void makePixel(GLfloat r, GLfloat g, GLfloat b) {
		this->r = r;
		this->g = g;
		this->b = b;
	}

	//Assign random RGB to pixel
	void randPixel() {
		//srand(time(NULL));

		this->r = (rand() % 100) / static_cast<GLfloat>(100);
		this->g = (rand() % 100) / static_cast<GLfloat>(100);
		this->b = (rand() % 100) / static_cast<GLfloat>(100);

	}

	//Set pixel as white
	void white() {
		this->r = 1;
		this->g = 1;
		this->b = 1;
	}

	//Set pixel as black
	void black() {
		this->r = 0;
		this->g = 0;
		this->b = 0;
	}

	//Print content of pixel
	void show() {
		cout << "R:" << r << ' ' << "G:" << g << ' ' << "B:" << b << endl;
	}
};

class Mesh {
public:
	//Table of vertex
	vector<GLfloat*> polyTable;

	//List of vertices index seperately for each facet
	vector<vector<int>> vnumset;

	//Let of number of vertices for each facet
	vector<int> num;

	//List of culling flag for each facet
	vector<bool> culling;

	//Number of vertices, initialized as 0
	int vecNum = 0;

	//Number of facets, initialized as 0
	int faceNum = 0;

	//Points info in screen space
	vector<vector<GLfloat*>> persMeshVertex;

	//Points info in world space
	vector<vector<GLfloat*>> worldMehsVertex;

	//Vertex associated facet, use by Gourand shading and Phong shading
	map<int, vector<int>> vertexAsoList;

	//List of facet normal
	vector<Ver> facetNormal;

	//List of vertex normal
	vector<Ver> vertexNormal;

	//List of facet normal under the order of polyTable
	vector<Ver> polyTableNormal;

	//Shading information for each facet, use by constant shading. Associated with specific light and view direction
	vector<GLfloat> facetShading;

	//Shading information for each facet, use by Gourand shading
	vector<GLfloat> vertexShading;

	void printPoly() {
		for (int i = 0; i < vecNum; i++) {
			cout << polyTable[i][0] << ' ' << polyTable[i][1] << ' ' << polyTable[i][2] << endl;
		}
	}

	void printVnumset() {
		for (int i = 0; i < faceNum; i++) {


			for (int j = 0; j < num[i]; j++) {
				cout << vnumset[i][j] << ends;
			}
			cout << endl;
		}
	}

	void getVecNum() {
		cout << vecNum << endl;
	}

	void getFaceNum() {
		cout << faceNum << endl;
	}

	void printCulling() {
		for (int i = 0; i < culling.size(); i++) {
			cout << culling[i] << ' ' << ends;
		}
	}

	void generateVAL() {
		for (int k = 0; k < vnumset.size(); k++) {
			for (int i = 0; i < vnumset[k].size(); i++) {
				vertexAsoList[vnumset[k][i]-1].push_back(k);
			}
		}
	}

	void generateVertexNormal() {
		for (int i = 0; i < this->vertexAsoList.size(); i++) {
			Ver vertexNormal(0, 0, 0);

			for (int j = 0; j < vertexAsoList[i].size(); j++) {
				

				vertexNormal.x += polyTableNormal[vertexAsoList[i][j]].x;
				vertexNormal.y += polyTableNormal[vertexAsoList[i][j]].y;
				vertexNormal.z += polyTableNormal[vertexAsoList[i][j]].z;
			}
			vertexNormal.normalize();
			this->vertexNormal.push_back(vertexNormal);
		}
	}

	void imptPersMesh(vector<vector<GLfloat*>> persMeshVertex) {
		this->persMeshVertex = persMeshVertex;
	}

	void imptWorldMesh(vector<vector<GLfloat*>> worldMehsVertex) {
		this->worldMehsVertex = worldMehsVertex;
	}

	void imptFacetNormal(vector<Ver> facetNormal) {
		this->facetNormal = facetNormal;
	}

};

class edgeTable {
public:
	int Ymax;
	int Xmin;
	GLfloat ivrsSlope;

	Ver vec1;
	Ver vec2;

	int vec1Index;
	int vec2Index;

	//mark the edge as perpendicular or horizontial, 0 for normal, 1 for perpendicular, 2 for horizontial
	int isPH;

	edgeTable* next;

	
	edgeTable() {
		Ymax = 0;
		Xmin = 0;
		ivrsSlope = 0;
		next = NULL;
		vec1;
		vec2;
		isPH = 0;
	}

	void printTable() {
		cout << Ymax << ' ' << Xmin << ' ' << ivrsSlope << endl;
	}
};

class activeEdgeTable {

public:
	bool act;
	
	edgeTable et;
	GLfloat Xcurr;

	activeEdgeTable() {
		act = false;
		Xcurr = 0;
	}

	activeEdgeTable(edgeTable etable) {
		act = false;
		Xcurr = 0;
		this->et = etable;
	}

	void activate() {
		this->act = true;
	}

	void disactivate() {
		this->act = false;
	}
};

class bufferSet {
public:
	vector<vector<GLfloat>> zbuffer;
	vector<vector<Pixel>> imgBuffer;
	vector<vector<GLfloat>> shadingMap;
	vector<vector<Ver>> normalMap;
	vector<vector<Pixel>> texture;
	

	void assignZBuffer(vector<vector<GLfloat>> zbuffer) {
		this->zbuffer = zbuffer;
	}
	void assignImgBuffer(vector<vector<Pixel>> imgBuffer) {
		this->imgBuffer = imgBuffer;
	}
	void assignshadingMap(vector<vector<GLfloat>> shadingMap) {
		this->shadingMap = shadingMap;
	}
};