#pragma once

#include <stdio.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <GLFW/glfw3.h>
#include "var.hpp"
#include "mymath.hpp"

using namespace std;


//generate perspective matrix according to given arguments
GLfloat* persMat(GLfloat aspect, GLfloat near, GLfloat far, GLfloat h) {
	GLfloat* matrlt = (GLfloat*)malloc(16 * sizeof(GLfloat));

	GLfloat scale = near / h;

	GLfloat mat[16] = { scale / aspect,0,0,0,
						0,scale,0,0,
						0,0,far / (far - near),-near * far / (far - near),
						0,0,1,0 };
	
	if (matrlt != NULL) {
		for (int i = 0; i < 16; i++) {
			matrlt[i] = mat[i];
		}
	}
	return matrlt;
}

//simple translate matrix
GLfloat* transMat(GLfloat Cx, GLfloat Cy, GLfloat Cz) {
	GLfloat* matrlt = (GLfloat*)malloc(16 * sizeof(GLfloat));

	if (matrlt != NULL) {
		matrlt = matI();
		
		matrlt[3]	= -Cx;
		matrlt[7]	= -Cy;
		matrlt[11]	= -Cz;
	}
	
	return matrlt;
}

//generate View matrix based on camera properties
GLfloat* viewMat(GLfloat* Pref, GLfloat* C, GLfloat* Vp) {
	GLfloat* matrlt = (GLfloat*)malloc(16 * sizeof(GLfloat));
	
		initMat(matrlt);

		GLfloat* N =  vecNor(minusVec(Pref, C));
		GLfloat* U = vecNor(crossPdt(N, Vp));
		GLfloat* V = crossPdt(U, N);

		if (matrlt != NULL) {
			for (int i = 0; i < 3; i++) {
				matrlt[i] = U[i];
			}
			for (int i = 0; i < 3; i++) {
				matrlt[4 + i] = V[i];
			}
			for (int i = 0; i < 3; i++) {
				matrlt[8 + i] = N[i];
			}
			matrlt[15] = 1;

			matrlt = multi(matrlt, transMat(C[0], C[1], C[2]));
		}	
	
	return matrlt;
}

//generate MVP matrix via input matrices
void generateMVP(GLfloat pers[], GLfloat view[], GLfloat trans[]) {
	GLfloat *matrlt1 = (GLfloat*)malloc(16 * sizeof(GLfloat));
	GLfloat *matrlt2 = (GLfloat*)malloc(16 * sizeof(GLfloat));

	if (matrlt1 != NULL&& matrlt2 != NULL) {
		initMat(matrlt1);
		initMat(matrlt2);

		matrlt1 = multi(view, trans);
		matrlt2	= multi(pers, matrlt1);

		for (int i = 0; i < 16; i++) {
			matMVP[i] = matrlt2[i];
			//cout << matMVP[i] << ' ' << ends;
		}
	}
}


//DON'T put in loop
//**SHOULD KEEP THE Z VALUE FOR FUTURE USE**
//get all vertices transformed and divided by w, get a vec3 as input, return a vec3
vector<GLfloat*> verteciesTrans(vector<GLfloat*> face) {

	GLfloat* vec = (GLfloat*)malloc(4 * sizeof(GLfloat));
	GLfloat* vec1 = (GLfloat*)malloc(3 * sizeof(GLfloat));
	GLfloat* vec2 = (GLfloat*)malloc(3 * sizeof(GLfloat));
	vector<GLfloat*> newface;

	if (vec != NULL &&vec1!=NULL&&vec2!=NULL) {
		initVec(vec);	


		//apply MVP to each vertex

		for (int i = 0; i < face.size(); i++) {
			vec1 = face[i];
			vec=vecMatMulti(matMVP, vec1);			
			vec2 = vecWNor(vec);

			newface.push_back(vec2);

		}
	}
	return newface;
}

//take a mesh, get each vertex transformed by MVP
vector<GLfloat*> setPolygon(Mesh mesh) {
	vector<GLfloat*> newPolyTable;

	newPolyTable= verteciesTrans(mesh.polyTable);

	return newPolyTable;
}


//create list of mesh according to vertex info, with each facet seperated
vector<vector<GLfloat*>> deployPoly(Mesh mesh, vector<GLfloat*> polyTable) {
	
	vector<vector<GLfloat*>> poly;

	for (int i = 0; i < mesh.faceNum; i++) {
		vector<GLfloat*> facet;

		for (int j = 0; j < mesh.num[i]; j++) {
			GLfloat* vec = (GLfloat*)malloc(3 * sizeof(GLfloat));

			if (vec != NULL) {
				vec = polyTable[mesh.vnumset[i][j] - 1];

				//********************************************************************************
				//cout << vec[0] << ' ' << vec[1] << ' ' << vec[2] << endl;
				facet.push_back(vec);
			}

		}
		poly.push_back(facet);
	}	
	
	return poly;
}

//split a facet into a set of vertecies and deploy them
void deployFace(vector<GLfloat*> face, int n) {

	for (int i = 0; i < n; i++) {
		glVertex2fv(face[i]);
		//cout << face[i][0] << ',' << face[i][1] << endl;
	}
}

//take a mesh, split it and draw each facet
void drawPoly(Mesh mesh, vector<vector<GLfloat*>> poly) {
	for (int i = 0; i < mesh.faceNum; i++) {
		//cout << mesh.culling[i];
		//drawing facet
		if (mesh.culling[i] == true) {

			glBegin(GL_LINE_LOOP);

			deployFace(poly[i], mesh.num[i]);

			glEnd();
		}
		//cout << "--------------" << endl;
	}
}

//generate facial normal, make dot product with lookat vector,then decide culling
void backCulling(Mesh& mesh, vector<vector<GLfloat*>> poly, GLfloat* Pref, GLfloat* Camera) {
	bool flag = false;

	//generate normal
	for (int i = 0; i < mesh.faceNum; i++) {
		/*
		GLfloat* lookat = minusVec(poly[i][0], Camera);

		GLfloat* vec1 = minusVec(poly[i][1], poly[i][0]);
		GLfloat* vec2 = minusVec(poly[i][2], poly[i][1]);
		
		GLfloat* normal = crossPdt(vec1, vec2);
		//bool flag = dotPdt(lookat, normal) > 0 ? true : false;

		//decide culling or not
		if (dotPdt(lookat, normal) >= 0) {
			flag = true;
		}
		else { 
			flag = false; 
		}
		*/
		

		//mesh.culling.push_back(flag);
		mesh.culling.push_back(true);
	}
	cout << mesh.culling.size() << endl;
	//mesh.printCulling();
	
}


//************************************************************************
//lab2 below

//generate list of edgetable
vector<edgeTable> generateEdgeTable(vector<Ver> facet) {
	

	//facet
	vector<edgeTable> tableList;

	for (int i = 0; i < facet.size(); i++) {
		//edge
		edgeTable etable;

		if (i < facet.size() - 1) {

			etable.Ymax = max(facet[i].y, facet[i + 1].y);
			etable.Xmin = min(facet[i].x, facet[i + 1].x);
			
			etable.ivrsSlope = 1 / Slope(facet[i], facet[i + 1]);

			if (facet[i].y <= facet[i + 1].y) {
				etable.vec1 = facet[i];
				etable.vec2 = facet[i + 1];

				etable.vec1Index = i;
				etable.vec2Index = i + 1;
			}
			else {
				etable.vec2 = facet[i];
				etable.vec1 = facet[i + 1];

				etable.vec2Index = i;
				etable.vec1Index = i + 1;
			}		

			tableList.push_back(etable);
		}

		//situation for the last vertex, loop back
		else if (i == (facet.size() - 1)) {
			etable.Ymax = max(facet[i].y, facet[0].y);
			etable.Xmin = min(facet[i].x, facet[0].x);

			etable.ivrsSlope = 1 / Slope(facet[i], facet[0]);

			if (facet[i].y <= facet[0].y) {
				etable.vec1 = facet[i];
				etable.vec2 = facet[0];

				etable.vec1Index = i;
				etable.vec2Index = 0;
			}
			else {
				etable.vec2 = facet[i];
				etable.vec1 = facet[0];

				etable.vec2Index = i;
				etable.vec1Index = 0;
			}

			tableList.push_back(etable);

		}
	}
	for (int i = 0; i < facet.size(); i++) {
		if (tableList[i].vec1.x == tableList[i].vec2.x)
			tableList[i].isPH = 1;
		else if(tableList[i].vec1.y == tableList[i].vec2.y)
			tableList[i].isPH = 2;

		/*
		if (tableList[i].isPH != 0)
			cout << "is P/H" << endl;
		*/
		
	}
	
	for (int i = 0; i < facet.size(); i++) {
		tableList[i].printTable();
	}
	cout << "------------------" << endl;

	return tableList;
}

//generate edge table list for the whole mesh
vector<vector<edgeTable>> generateMeshList(Mesh mesh, vector<vector<Ver>> poly) {
	vector<edgeTable> faceList;
	vector<vector<edgeTable>> meshList;

	for (int i = 0; i < poly.size(); i++) {
		faceList = generateEdgeTable(poly[i]);
		meshList.push_back(faceList);
		
	}

	return meshList;
}



//Extend the poly into pixel size, 1920*1080
//Include backface culling
vector<vector<Ver>> pixelize(Mesh mesh, vector<vector<GLfloat*>> poly) {

	
	vector<vector<Ver>> polygon;

	for (int i = 0; i < mesh.faceNum; i++) {

		//considering backface culling
		if (mesh.culling[i] == true) {
			vector<Ver> facet;

			for (int j = 0; j < mesh.num[i]; j++) {
				Ver vec;

				vec.x = round(WIDTH * (poly[i][j][0] + 1) / 2);
				vec.y = round(HEIGHT * (poly[i][j][1] + 1) / 2);
				vec.z = poly[i][j][2];

				cout << "*" << vec.x << ',' << vec.y << ',' << vec.z << endl;
				facet.push_back(vec);
			}


			polygon.push_back(facet);
			cout << "facet" << endl;
		}
		
		
	}

	cout << "----------------------------------------" << endl;

	return polygon;

}

//Initialize z-buffer as all FAR=1000
vector<vector<GLfloat>> initZBuff(int height, int width) {
	vector<vector<GLfloat>> zbuffer;

	for (int i = 0; i < height; i++) {
		vector<GLfloat> line;
		for (int j = 0; j < width; j++) {
			
			line.push_back(FAR);
		}
		zbuffer.push_back(line);
	}
	return zbuffer;
}

//Initialize image buffer as all black
vector<vector<Pixel>> initImgBuff(int height, int width) {
	vector<vector<Pixel>> imgbuffer;

	for (int i = 0; i < height; i++) {
		vector<Pixel> line;
		for (int j = 0; j < width; j++) {
			Pixel pix;
			line.push_back(pix);
		}
		imgbuffer.push_back(line);
	}
	return imgbuffer;
}

vector<vector<GLfloat>> initShadingMap(int height, int width) {
	vector<vector<GLfloat>> shadingMap;

	for (int i = 0; i < height; i++) {
		vector<GLfloat> line;
		for (int j = 0; j < width; j++) {

			line.push_back(0);
		}
		shadingMap.push_back(line);
	}
	return shadingMap;
}

vector<vector<Ver>> initNormalMap(int height, int width) {
	vector<vector<Ver>> normalMap;

	for (int i = 0; i < height;i++) {
		vector<Ver> line;

		for (int j = 0; j < width; j++) {
			Ver vertex;
			line.push_back(vertex);
		}
		normalMap.push_back(line);
	}
	return normalMap;
}

//Overlap z-buffer with new generated depth value; when z-buffer updated, fill image buffer with assigned color
bufferSet refreshZBuff
(bufferSet buffSet, vector<vector<GLfloat>> newBuff, Pixel color, GLfloat shadingInfo) {

	for (int y = 0; y < buffSet.zbuffer.size(); y++) {
		for (int x = 0; x < buffSet.zbuffer[y].size(); x++) {
			if (newBuff[y][x]< buffSet.zbuffer[y][x]) {

				buffSet.zbuffer[y][x] = newBuff[y][x];

				//refresh image-buffer here
				buffSet.imgBuffer[y][x] = color;

				buffSet.shadingMap[y][x] = shadingInfo;
			}
		}
	}
	return buffSet;
}

bufferSet refreshZBuffGourand
(bufferSet buffSet, vector<vector<GLfloat>> newBuff, Pixel color, vector<vector<GLfloat>> newShadingMap) {

	for (int y = 0; y < buffSet.zbuffer.size(); y++) {
		for (int x = 0; x < buffSet.zbuffer[y].size(); x++) {
			if (newBuff[y][x] < buffSet.zbuffer[y][x]) {

				buffSet.zbuffer[y][x] = newBuff[y][x];

				//refresh image-buffer here
				buffSet.imgBuffer[y][x] = color;

				buffSet.shadingMap[y][x] = newShadingMap[y][x];
			}
		}
	}
	return buffSet;
}

bufferSet refreshZBuffPhong
(bufferSet buffSet, vector<vector<GLfloat>> newBuff, Pixel color, vector<vector<Ver>> newNormalMap) {

	for (int y = 0; y < buffSet.zbuffer.size(); y++) {
		for (int x = 0; x < buffSet.zbuffer[y].size(); x++) {
			if (newBuff[y][x] < buffSet.zbuffer[y][x]) {

				buffSet.zbuffer[y][x] = newBuff[y][x];

				//refresh image-buffer here
				buffSet.imgBuffer[y][x] = color;

				buffSet.normalMap[y][x] = newNormalMap[y][x];
			}
		}
	}
	return buffSet;
}

//Scan converste per facet, fill the interior pixels, return the zbuffer with updated vertices
//Shading Mode: 0: const shading, 1: Gourand shading, 2: Phong shading
bufferSet scanlineConst(vector<edgeTable> facetET, bufferSet buffSet, int index, GLfloat shadingInfo) {
	GLfloat Ymin = HEIGHT;
	int start = 0;
	vector<activeEdgeTable> basicAET;
	vector<vector<GLfloat>> localzbuffer = initZBuff(HEIGHT, WIDTH);

	//initialize a set of random color
	vector<Pixel> ranColor;

	srand(time(NULL));
	for (int r = 0; r < 20; r++) {
		Pixel pix;
		pix.randPixel();
		ranColor.push_back(pix);
	}

	//get the start position
	for (int i = 0; i < facetET.size(); i++) {
		if (facetET[i].vec1.y <= Ymin) {
			Ymin = facetET[i].vec1.y;
			start = i;
		}
	}

	//generate AET for each ET for future selection
	for (int k = 0; k < facetET.size(); k++) {

		activeEdgeTable aet(facetET[k]);
		aet.Xcurr = aet.et.vec1.x;

		basicAET.push_back(aet);
	}

	

	//scan conversion for each line
	for (int y = Ymin; y < HEIGHT; y++) {
		
		Ver verL, verR;
		//verL.y = verR.y = v.y = y;
		vector<Ver> vec;

		//refresh and discard edges
		for (int k = 0; k < basicAET.size(); k++) {
			if (basicAET[k].et.vec1.y <= y && basicAET[k].et.vec2.y >= y) {
				basicAET[k].activate();
			}
			else {
				basicAET[k].disactivate();
			}
		}
		
		//sort x values for active edges
		for (int k = 0; k < basicAET.size(); k++) {
			if (basicAET[k].act==true) {
			
				Ver vt((int)basicAET[k].Xcurr, y);
				vt.z = vecLerp(basicAET[k].et.vec1, basicAET[k].et.vec2, vt);

				vec.push_back(vt);
				//update z value here

			}
		}

		//if no intersection, quit loop
		if (vec.size() == 0) {
			break;
		}

		//determine joint point
		switch (vec.size())
		{
		case 2:
			if (vec[0].x < vec[1].x) {
				verL = vec[0];
				verR = vec[1];
			}
			else {
				verL = vec[1];
				verR = vec[0];
			}
			break;

		case 3:
			//if two vertices are very close but not identity because of accuracy
			if (dist2d(vec[0],vec[1])< dist2d(vec[1], vec[2])) {
				
				vec.erase(vec.begin());
			}
			else {
				vec.erase(vec.begin() + 2);
			}
			

			if (vec[0].x < vec[1].x) {
				verL = vec[0];
				verR = vec[1];
			}
			else {
				verL = vec[1];
				verR = vec[0];
			}
			break;

		default:
			verL = vec[0];
			verR = vec[0];

			for (int l = 0; l < vec.size(); l++) {
				if (vec[l].x < verL.x)
					verL = vec[l];
				if (vec[l].x > verR.x)
					verR = vec[l];
			}
			break;
		}



		//update local z-buffer
		for (int x = verL.x; x <= verR.x; x++) {
			Ver v(x, y, 0);
			
			GLfloat z = vecLerp(verL, verR, v);
			localzbuffer[y][x] = z;

			//add new features here

		}

		//update x value for next line
		for (int k = 0; k < basicAET.size(); k++) {
			if (basicAET[k].act) {
				if(basicAET[k].et.isPH==0)
				basicAET[k].Xcurr += basicAET[k].et.ivrsSlope;

			}
		}
		//cout << y << endl;
	}
	Pixel color(1, 0, 0);
	
	buffSet = refreshZBuff(buffSet, localzbuffer, color,shadingInfo);
	//buffSet = refreshZBuff(buffSet, localzbuffer, ranColor[index%20],shadingInfo);

	return buffSet;
}

bufferSet scanlineGourand(vector<edgeTable> facetET, bufferSet buffSet, int index, vector<GLfloat> shadingInfo) {
	GLfloat Ymin = HEIGHT;
	int start = 0;
	vector<activeEdgeTable> basicAET;
	vector<vector<GLfloat>> localzbuffer = initZBuff(HEIGHT, WIDTH);
	vector<vector<GLfloat>> localShadingMap = initShadingMap(HEIGHT, WIDTH);

	//initialize a set of random color
	vector<Pixel> ranColor;

	srand(time(NULL));
	for (int r = 0; r < 20; r++) {
		Pixel pix;
		pix.randPixel();
		ranColor.push_back(pix);
	}

	//get the start position
	for (int i = 0; i < facetET.size(); i++) {
		if (facetET[i].vec1.y <= Ymin) {
			Ymin = facetET[i].vec1.y;
			start = i;
		}
	}

	//generate AET for each ET for future selection
	for (int k = 0; k < facetET.size(); k++) {

		activeEdgeTable aet(facetET[k]);
		aet.Xcurr = aet.et.vec1.x;

		basicAET.push_back(aet);
	}



	//scan conversion for each line
	for (int y = Ymin; y < HEIGHT; y++) {
		
		Ver verL, verR;
		GLfloat shadingL, shadingR;
		//verL.y = verR.y = v.y = y;
		vector<Ver> vec;
		vector<GLfloat> shad;

		//refresh and discard edges
		for (int k = 0; k < basicAET.size(); k++) {
			if (basicAET[k].et.vec1.y <= y && basicAET[k].et.vec2.y >= y) {
				basicAET[k].activate();
			}
			else {
				basicAET[k].disactivate();
			}
		}

		//sort x values for active edges
		for (int k = 0; k < basicAET.size(); k++) {
			if (basicAET[k].act == true) {

				Ver vt((int)basicAET[k].Xcurr, y);
				vt.z = vecLerp(basicAET[k].et.vec1, basicAET[k].et.vec2, vt);

				vec.push_back(vt);
				//update z value here

				GLfloat fl = valueLerp(basicAET[k].et.vec1, basicAET[k].et.vec2, vt, shadingInfo[basicAET[k].et.vec1Index], shadingInfo[basicAET[k].et.vec2Index]);
				shad.push_back(fl);
			}
		}

		//if no intersection, quit loop
		if (vec.size() == 0) {
			break;
		}

		//determine joint point
		
		switch (vec.size())
		{
		case 2:
			if (vec[0].x < vec[1].x) {
				verL = vec[0];
				verR = vec[1];

				shadingL = shad[0];
				shadingR = shad[1];
			}
			else {
				verL = vec[1];
				verR = vec[0];

				shadingL = shad[1];
				shadingR = shad[0];
			}
			break;

		case 3:
			//if two vertices are very close but not identity because of accuracy
			if (dist2d(vec[0], vec[1]) < dist2d(vec[1], vec[2])) {

				vec.erase(vec.begin());
				shad.erase(shad.begin());
			}
			else {
				vec.erase(vec.begin() + 2);
				shad.erase(shad.begin() + 2);
			}


			if (vec[0].x < vec[1].x) {
				verL = vec[0];
				verR = vec[1];

				shadingL = shad[0];
				shadingR = shad[1];
			}
			else {
				verL = vec[1];
				verR = vec[0];

				shadingL = shad[1];
				shadingR = shad[0];
			}
			break;

		default:
			verL = vec[0];
			verR = vec[0];

			shadingL = shad[0];
			shadingR = shad[0];

			for (int l = 0; l < vec.size(); l++) {
				if (vec[l].x < verL.x) {
					verL = vec[l];
					shadingL = shad[l];
				}
					

				if (vec[l].x > verR.x) {
					verR = vec[l];
					shadingR = shad[l];
				}
					
			}
			break;
		}



		//update local z-buffer
		for (int x = verL.x; x <= verR.x; x++) {
			Ver v(x, y, 0);

			GLfloat z= vecLerp(verL, verR, v);
			localzbuffer[y][x] = z;

			//add new features here
			//cout << valueLerp(verL, verR, v, shadingL, shadingR) << endl;

			GLfloat t= valueLerp(verL, verR, v, shadingL, shadingR);
			localShadingMap[y][x] = t;
		}

		//update x value for next line
		for (int k = 0; k < basicAET.size(); k++) {
			if (basicAET[k].act) {
				if (basicAET[k].et.isPH == 0)
					basicAET[k].Xcurr += basicAET[k].et.ivrsSlope;

			}
		}
		//cout << y << endl;
	}
	Pixel color(1, 0, 0);

	buffSet = refreshZBuffGourand(buffSet, localzbuffer, color, localShadingMap);
	//buffSet = refreshZBuff(buffSet, localzbuffer, ranColor[index%20],shadingInfo);

	return buffSet;
}

bufferSet scanlinePhong(vector<edgeTable> facetET, bufferSet buffSet, int index, vector<Ver> facetVertexNormal) {
	GLfloat Ymin = HEIGHT;
	int start = 0;
	vector<activeEdgeTable> basicAET;
	vector<vector<GLfloat>> localzbuffer = initZBuff(HEIGHT, WIDTH);
	//vector<vector<GLfloat>> localShadingMap = initShadingMap(HEIGHT, WIDTH);
	vector<vector<Ver>> localNormalMap = initNormalMap(HEIGHT, WIDTH);

	//initialize a set of random color
	vector<Pixel> ranColor;

	srand(time(NULL));
	for (int r = 0; r < 20; r++) {
		Pixel pix;
		pix.randPixel();
		ranColor.push_back(pix);
	}

	//get the start position
	for (int i = 0; i < facetET.size(); i++) {
		if (facetET[i].vec1.y <= Ymin) {
			Ymin = facetET[i].vec1.y;
			start = i;
		}
	}

	//generate AET for each ET for future selection
	for (int k = 0; k < facetET.size(); k++) {

		activeEdgeTable aet(facetET[k]);
		aet.Xcurr = aet.et.vec1.x;

		basicAET.push_back(aet);
	}



	//scan conversion for each line
	for (int y = Ymin; y < HEIGHT; y++) {

		Ver verL, verR;
		//GLfloat shadingL, shadingR;
		Ver normalL, normalR;
		//verL.y = verR.y = v.y = y;
		vector<Ver> vec;
		vector<Ver> normal;

		//refresh and discard edges
		for (int k = 0; k < basicAET.size(); k++) {
			if (basicAET[k].et.vec1.y <= y && basicAET[k].et.vec2.y >= y) {
				basicAET[k].activate();
			}
			else {
				basicAET[k].disactivate();
			}
		}

		//sort x values for active edges
		for (int k = 0; k < basicAET.size(); k++) {
			if (basicAET[k].act == true) {

				Ver vt((int)basicAET[k].Xcurr, y);
				vt.z = vecLerp(basicAET[k].et.vec1, basicAET[k].et.vec2, vt);

				vec.push_back(vt);
				//update z value here

				Ver nor = normalLerp(basicAET[k].et.vec1, basicAET[k].et.vec2, vt, facetVertexNormal[basicAET[k].et.vec1Index], facetVertexNormal[basicAET[k].et.vec2Index]);
				normal.push_back(nor);
			}
		}

		//if no intersection, quit loop
		if (vec.size() == 0) {
			break;
		}

		//determine joint point

		switch (vec.size())
		{
		case 2:
			if (vec[0].x < vec[1].x) {
				verL = vec[0];
				verR = vec[1];

				normalL = normal[0];
				normalR = normal[1];
				
			}
			else {
				verL = vec[1];
				verR = vec[0];

				normalL = normal[1];
				normalR = normal[0];
			}
			break;

		case 3:
			//if two vertices are very close but not identity because of accuracy
			if (dist2d(vec[0], vec[1]) < dist2d(vec[1], vec[2])) {

				vec.erase(vec.begin());
				normal.erase(normal.begin());
			}
			else {
				vec.erase(vec.begin() + 2);
				normal.erase(normal.begin() + 2);
			}


			if (vec[0].x < vec[1].x) {
				verL = vec[0];
				verR = vec[1];

				normalL = normal[0];
				normalR = normal[1];
			}
			else {
				verL = vec[1];
				verR = vec[0];

				normalL = normal[1];
				normalR = normal[0];
			}
			break;

		default:
			verL = vec[0];
			verR = vec[0];

			normalL = normal[0];
			normalR = normal[0];

			for (int l = 0; l < vec.size(); l++) {
				if (vec[l].x < verL.x) {
					verL = vec[l];
					normalL = normal[l];
				}


				if (vec[l].x > verR.x) {
					verR = vec[l];
					normalR = normal[l];
				}

			}
			break;
		}



		//update local z-buffer
		for (int x = verL.x; x <= verR.x; x++) {
			Ver v(x, y, 0);

			GLfloat z = vecLerp(verL, verR, v);
			localzbuffer[y][x] = z;

			//add new features here
			//cout << valueLerp(verL, verR, v, shadingL, shadingR) << endl;

			Ver ver = normalLerp(verL, verR, v, normalL, normalR);
			ver.normalize();
			localNormalMap[y][x] = ver;
		}

		//update x value for next line
		for (int k = 0; k < basicAET.size(); k++) {
			if (basicAET[k].act) {
				if (basicAET[k].et.isPH == 0)
					basicAET[k].Xcurr += basicAET[k].et.ivrsSlope;

			}
		}
		//cout << y << endl;
	}
	Pixel color(1, 1, 1);

	buffSet = refreshZBuffPhong(buffSet, localzbuffer, color, localNormalMap);
	//buffSet = refreshZBuff(buffSet, localzbuffer, ranColor[index%20],shadingInfo);

	return buffSet;
}

/*
//Scan converste per facet, fill the interior pixels, return the zbuffer with updated vertices
//Shading Mode: 0: const shading, 1: Gourand shading, 2: Phong shading
bufferSet scanline(vector<edgeTable> facetET, bufferSet buffSet, int index) {
	GLfloat Ymin = HEIGHT;
	int start = 0;
	vector<activeEdgeTable> basicAET;
	vector<vector<GLfloat>> localzbuffer = initZBuff(HEIGHT, WIDTH);

	//initialize a set of random color
	vector<Pixel> ranColor;

	srand(time(NULL));
	for (int r = 0; r < 20; r++) {
		Pixel pix;
		pix.randPixel();
		ranColor.push_back(pix);
	}

	//get the start position
	for (int i = 0; i < facetET.size(); i++) {
		if (facetET[i].vec1.y <= Ymin) {
			Ymin = facetET[i].vec1.y;
			start = i;
		}
	}

	//generate AET for each ET for future selection
	for (int k = 0; k < facetET.size(); k++) {

		activeEdgeTable aet(facetET[k]);
		aet.Xcurr = aet.et.vec1.x;

		basicAET.push_back(aet);
	}



	//scan conversion for each line
	for (int y = Ymin; y < HEIGHT; y++) {

		Ver verL, verR;
		//verL.y = verR.y = v.y = y;
		vector<Ver> vec;

		//refresh and discard edges
		for (int k = 0; k < basicAET.size(); k++) {
			if (basicAET[k].et.vec1.y <= y && basicAET[k].et.vec2.y >= y) {
				basicAET[k].activate();
			}
			else {
				basicAET[k].disactivate();
			}
		}

		//sort x values for active edges
		for (int k = 0; k < basicAET.size(); k++) {
			if (basicAET[k].act==true) {

				Ver vt((int)basicAET[k].Xcurr, y);
				vt.z = vecLerp(basicAET[k].et.vec1, basicAET[k].et.vec2, vt);

				vec.push_back(vt);
				//update z value here

			}
		}

		//if no intersection, quit loop
		if (vec.size() == 0) {
			break;
		}

		//determine joint point
		switch (vec.size())
		{
		case 2:
			if (vec[0].x < vec[1].x) {
				verL = vec[0];
				verR = vec[1];
			}
			else {
				verL = vec[1];
				verR = vec[0];
			}
			break;

		case 3:
			//if two vertices are very close but not identity because of accuracy
			if (dist2d(vec[0],vec[1])< dist2d(vec[1], vec[2])) {

				vec.erase(vec.begin());
			}
			else {
				vec.erase(vec.begin() + 2);
			}


			if (vec[0].x < vec[1].x) {
				verL = vec[0];
				verR = vec[1];
			}
			else {
				verL = vec[1];
				verR = vec[0];
			}
			break;

		default:
			verL = vec[0];
			verR = vec[0];

			for (int l = 0; l < vec.size(); l++) {
				if (vec[l].x < verL.x)
					verL = vec[l];
				if (vec[l].x > verR.x)
					verR = vec[l];
			}
			break;
		}



		//update local z-buffer
		for (int x = verL.x; x <= verR.x; x++) {
			Ver v(x, y, 0);

			GLfloat z = vecLerp(verL, verR, v);
			localzbuffer[y][x] = z;

			//add new features here

		}

		//update x value for next line
		for (int k = 0; k < basicAET.size(); k++) {
			if (basicAET[k].act) {
				if(basicAET[k].et.isPH==0)
				basicAET[k].Xcurr += basicAET[k].et.ivrsSlope;

			}
		}
		//cout << y << endl;
	}

	buffSet = refreshZBuff(buffSet, localzbuffer, ranColor[index%20]);

	return buffSet;
}
*/

//generate z-buffer for whole polygon
bufferSet polyZBuff(vector<vector<edgeTable>> meshList, Mesh mesh) {
	
	bufferSet buffSet;

	buffSet.zbuffer = initZBuff(HEIGHT, WIDTH);
	buffSet.imgBuffer = initImgBuff(HEIGHT, WIDTH);
	buffSet.shadingMap = initShadingMap(HEIGHT, WIDTH);
	buffSet.normalMap = initNormalMap(HEIGHT, WIDTH);

	//vector<vector<GLfloat>> vertexShading;
	
	vector<vector<Ver>> vertexNormal;

	/*
	for (int i = 0; i < mesh.faceNum; i++) {
		vector<GLfloat> vertexShadingFacet;

		if (mesh.culling[i] == true) {
			for (int j = 0; j < mesh.vnumset[i].size(); j++) {
				vertexShadingFacet.push_back(mesh.vertexShading[mesh.vnumset[i][j]-1]);
			}
			vertexShading.push_back(vertexShadingFacet);
		}
	}
	*/
	for (int i = 0; i < mesh.faceNum; i++) {
		vector<Ver> vertexNormalFacet;

		if (mesh.culling[i] == true) {
			for (int j = 0; j < mesh.vnumset[i].size(); j++) {
				vertexNormalFacet.push_back(mesh.vertexNormal[mesh.vnumset[i][j] - 1]);
			}
			vertexNormal.push_back(vertexNormalFacet);
		}
	}
	
	
	for (int i = 0; i < meshList.size(); i++) {

		//buffSet = scanlineConst(meshList[i], buffSet, i, mesh.facetShading[i]);
		//buffSet = scanlineGourand(meshList[i], buffSet, i, vertexShading[i]);
		buffSet = scanlinePhong(meshList[i], buffSet, i, vertexNormal[i]);

	}
	
	return buffSet;
}


//Compare zbuffer between different mesh and refresh it
bufferSet updateZBuffer
(bufferSet buff1, bufferSet buff2) {
	

	for (int y = 0; y < buff1.zbuffer.size(); y++) {
		for (int x = 0; x < buff1.zbuffer[y].size(); x++) {

			if (buff2.zbuffer[y][x] < buff1.zbuffer[y][x]) {
				buff1.zbuffer[y][x] = buff2.zbuffer[y][x];
				buff1.imgBuffer[y][x] = buff2.imgBuffer[y][x];
			}
		}
	}
	return buff1;
}

//Draw image buffer
void drawPixel(vector<vector<Pixel>> imgbuffer) {
	
	glBegin(GL_POINTS);

	for (int y = 0; y < imgbuffer.size(); y++) {
		for (int x = 0; x < imgbuffer[y].size(); x++) {
			Ver ver((2 * ((float)x / WIDTH)) - 1, (2 * ((float)y / HEIGHT)) - 1);

			glColor3f(imgbuffer[y][x].r, imgbuffer[y][x].g, imgbuffer[y][x].b);
			glVertex2f(ver.x, ver.y);
		}
	}
	glEnd();
}


//************************************************************************
//Lab3 bwlow

//Transform mesh from screen space back to world space
vector<vector<GLfloat*>> inversePoly(vector<vector<GLfloat*>> poly) {
	vector<vector<GLfloat*>> inversedPoly;

	for (int i = 0; i < poly.size(); i++) {
		vector<GLfloat*> facet;

		for (int j = 0; j < poly[i].size(); j++) {
			
			GLfloat* vec = (GLfloat*)malloc(3 * sizeof(GLfloat));
			GLfloat* vectmp = (GLfloat*)malloc(4 * sizeof(GLfloat));
			GLfloat* vecrlt = (GLfloat*)malloc(3 * sizeof(GLfloat));

			

			if (vec!= NULL&& vecrlt != NULL&& vectmp!=NULL) {
				initVec(vec);
				initVec(vecrlt);
				initVec(vectmp);
				vec = poly[i][j];
				
				
				vectmp = vecMatMulti(ivsMVP, vec);
				
				
				vecrlt = vecWNor(vectmp);
				cout << vecrlt[0] << ' ' << vecrlt[1] << ' ' << vecrlt[2] << endl;

				facet.push_back(vecrlt);
				
				/*
				free(vecrlt);
				free(vec);
				free(vectmp);
				*/
				
			}
			
			
		}
		inversedPoly.push_back(facet);
	}
	return inversedPoly;
}

vector<Ver> generateFacetNormal(vector<vector<GLfloat*>> worldPoly) {
	vector<Ver> normalList;
	for (int i = 0; i < worldPoly.size(); i++) {
		GLfloat* vec1 = (GLfloat*)malloc(3 * sizeof(GLfloat));
		GLfloat* vec2 = (GLfloat*)malloc(3 * sizeof(GLfloat));
		GLfloat* vec3 = (GLfloat*)malloc(3 * sizeof(GLfloat));

		if (vec1 != NULL && vec2 != NULL && vec3 != NULL) {
			vec1 = minusVec(worldPoly[i][1], worldPoly[i][0]);
			vec2 = minusVec(worldPoly[i][2], worldPoly[i][1]);

			vec3 = crossPdt(vec1, vec2);
			vec3 = vecNor(vec3);
			
			Ver nrl(vec3[0], vec3[1], vec3[2]);
			normalList.push_back(nrl);
		}
	}
	return normalList;
}

vector<GLfloat> constShading(Ver light,Ver view, vector<Ver> facetNormal, Mesh mesh) {
	GLfloat* vecR = (GLfloat*)malloc(3 * sizeof(GLfloat));
	GLfloat* vecLight = (GLfloat*)malloc(3 * sizeof(GLfloat));
	GLfloat* vecView = (GLfloat*)malloc(3 * sizeof(GLfloat));
	GLfloat* vecNormal = (GLfloat*)malloc(3 * sizeof(GLfloat));

	vector<GLfloat> meshShading;

	if (vecR != NULL && vecLight != NULL && vecView != NULL&&vecNormal!=NULL) {
		//initialize vecLight
		{
			vecLight[0] = light.x;
			vecLight[1] = light.y;
			vecLight[2] = light.z;
		}
		
		//initialize vecView
		{
			vecView[0] = view.x;
			vecView[1] = view.y;
			vecView[2] = view.z;
		}
		
		for (int i = 0; i < facetNormal.size(); i++) {
			if (mesh.culling[i]== true) {
				//initialize vecNormal
				{
					vecNormal[0] = facetNormal[i].x;
					vecNormal[1] = facetNormal[i].y;
					vecNormal[2] = facetNormal[i].z;
				}

				vecR = minusVec(vecLight, scalarVec((2 * vecLength(vecLight) * dotPdt(vecNor(vecNormal), vecNor(vecLight))), vecNor(vecNormal)));

				GLfloat facetShading = dotPdt(vecNor(vecR), vecNor(vecView));

				if (facetShading < 0)
					facetShading = 0;

				meshShading.push_back(facetShading);
			}
		}
	}
	return meshShading;
}

vector<GLfloat> GourandShading(Ver light, Ver view, vector<Ver> vertexNormal, Mesh mesh) {
	GLfloat* vecR = (GLfloat*)malloc(3 * sizeof(GLfloat));
	GLfloat* vecLight = (GLfloat*)malloc(3 * sizeof(GLfloat));
	GLfloat* vecView = (GLfloat*)malloc(3 * sizeof(GLfloat));
	GLfloat* vecNormal = (GLfloat*)malloc(3 * sizeof(GLfloat));

	vector<GLfloat> meshShading;

	if (vecR != NULL && vecLight != NULL && vecView != NULL && vecNormal != NULL) {
		//initialize vecLight
		{
			vecLight[0] = light.x;
			vecLight[1] = light.y;
			vecLight[2] = light.z;
		}

		//initialize vecView
		{
			vecView[0] = view.x;
			vecView[1] = view.y;
			vecView[2] = view.z;
		}

		for (int i = 0; i < vertexNormal.size(); i++) {
			{
				//initialize vecNormal
				{
					vecNormal[0] = vertexNormal[i].x;
					vecNormal[1] = vertexNormal[i].y;
					vecNormal[2] = vertexNormal[i].z;
				}

				vecR = minusVec(vecLight, scalarVec((2 * vecLength(vecLight) * dotPdt(vecNor(vecNormal), vecNor(vecLight))), vecNor(vecNormal)));

				GLfloat vertexShading = dotPdt(vecNor(vecR), vecNor(vecView));

				if (vertexShading < 0)
					vertexShading = 0;

				meshShading.push_back(vertexShading);
			}
		}
	}
	return meshShading;
}

bufferSet applyShadingMap(bufferSet buffset) {
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			buffset.imgBuffer[y][x].r *= buffset.shadingMap[y][x];
			buffset.imgBuffer[y][x].g *= buffset.shadingMap[y][x];
			buffset.imgBuffer[y][x].b *= buffset.shadingMap[y][x];
		}
	}
	return buffset;
}

vector<Ver> generatePolyNormal(Mesh mesh) {
	for (int i = 0; i < mesh.vnumset.size(); i++) {
		GLfloat* nor = (GLfloat*)malloc(3 * sizeof(GLfloat));

		if (nor != NULL) {
 			nor = crossPdt(minusVec(mesh.polyTable[mesh.vnumset[i][1]-1], mesh.polyTable[mesh.vnumset[i][0]-1]),
				minusVec(mesh.polyTable[mesh.vnumset[i][2]-1], mesh.polyTable[mesh.vnumset[i][1]-1]));
			nor = vecNor(nor);

			Ver vernor(nor[0], nor[1], nor[2]);
			mesh.polyTableNormal.push_back(vernor);
		}
	}
	return mesh.polyTableNormal;
}

bufferSet PhongShading(Ver light, Ver view, bufferSet buffset) {
	GLfloat* vecR = (GLfloat*)malloc(3 * sizeof(GLfloat));
	GLfloat* vecLight = (GLfloat*)malloc(3 * sizeof(GLfloat));
	GLfloat* vecView = (GLfloat*)malloc(3 * sizeof(GLfloat));
	GLfloat* vecNormal = (GLfloat*)malloc(3 * sizeof(GLfloat));

	for (int y = 0; y < HEIGHT; y++) {
		

		for (int x = 0; x < WIDTH; x++) {
			if (vecR != NULL && vecLight != NULL && vecView != NULL && vecNormal != NULL) {

				//initialize vecLight
				{
					vecLight[0] = light.x;
					vecLight[1] = light.y;
					vecLight[2] = light.z;
				}

				//initialize vecView
				{
					vecView[0] = view.x;
					vecView[1] = view.y;
					vecView[2] = view.z;
				}

				//initialize normal
				{
					vecNormal[0] = buffset.normalMap[y][x].x;
					vecNormal[1] = buffset.normalMap[y][x].y;
					vecNormal[2] = buffset.normalMap[y][x].z;
				}

				vecR = minusVec(vecLight, scalarVec((2 * vecLength(vecLight) * dotPdt(vecNor(vecNormal), vecNor(vecLight))), vecNor(vecNormal)));

				GLfloat vertexShading = dotPdt(vecNor(vecR), vecNor(vecView));

				if (vertexShading < 0)
					vertexShading = 0;
				buffset.shadingMap[y][x] = vertexShading;
				
			}
		}
		
	}
	return buffset;
}

bufferSet textureMapping(bufferSet normal, bufferSet texture) {
	int h = 10;
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			if (normal.normalMap[y][x].x != 0 && normal.normalMap[y][x].z != 0) {
				float xp = normal.normalMap[y][x].x / sqrt(pow(normal.normalMap[y][x].x, 2) + pow(normal.normalMap[y][x].z, 2));
				float yp = normal.normalMap[y][x].y / sqrt(pow(normal.normalMap[y][x].x, 2) + pow(normal.normalMap[y][x].z, 2));
				float zp = normal.normalMap[y][x].z / sqrt(pow(normal.normalMap[y][x].x, 2) + pow(normal.normalMap[y][x].z, 2));

				float u = abs(asin(xp) / (2 * PI));
				float v = (yp / h + 1) / 2;
				//cout << "u:" << u << "v:" << v<<endl;

				//do not operate if normal is out of range, saying too close to y axis
				if (v >= 1 || v < 0) {
					break;
				}
				normal.imgBuffer[y][x].r *= texture.texture[floor(v * HEIGHT)][floor(u * WIDTH)].r;
				normal.imgBuffer[y][x].g *= texture.texture[floor(v * HEIGHT)][floor(u * WIDTH)].g;
				normal.imgBuffer[y][x].b *= texture.texture[floor(v * HEIGHT)][floor(u * WIDTH)].b;
			}
		}
	}
	return normal;
}