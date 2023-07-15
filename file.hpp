#pragma once

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <GLFW/glfw3.h>
#include "var.hpp"
using namespace std;


Mesh openFile(string model) {
	ifstream fin;
	Mesh mesh;
	string str;

	//the model file 
	// 
	//fin.open("Z:\\bench.d.txt", ios::in);
	//fin.open("Z:\\atc.d.txt", ios::in);

	fin.open(model, ios::in);

	if (!fin.is_open())
	{
		cout << "cannot locate file" << endl;
		return mesh;
	}


	fin >> str >> mesh.vecNum >>mesh.faceNum;

	cout << str << ' ' << mesh.vecNum<<' '<<mesh.faceNum << endl;

	for (int i = 0; i < mesh.vecNum; i++) {
		GLfloat x, y, z;
		
		GLfloat* vec = (GLfloat*)malloc(3 * sizeof(GLfloat));

		fin >> x >> y >> z;
		
		if (vec != NULL) {
			vec[0] = x;
			vec[1] = y;
			vec[2] = z;
		}

		mesh.polyTable.push_back(vec);
	}

	for (int i = 0; i < mesh.faceNum; i++) {
		int n;
		vector<int> Vset;

		fin >> n;

		mesh.num.push_back(n);
		
		for (int i = 0; i < n; i++) {
			int v;

			fin >> v;
			Vset.push_back(v);
			cout << v <<' '<< ends;
		}
		cout << endl;
		mesh.vnumset.push_back(Vset);
		
	}

	fin.close();
	
	return mesh;
}

vector<vector<Pixel>> readBMP(string source) {
	int height = 1080, width = 1920;
	
	vector<vector<Pixel>> texture;


	ifstream fs;
	fs.open(source, ios::in | ios::binary);

	if (!fs.is_open())
	{
		cout << "cannot locate file" << endl;
		return texture;
	}

	string s;
	for (int i = 0; i < 54; i++) {
		fs >> s;
	}


	for (int i = 0; i < height; i++) {
		vector<Pixel> line;

		for (int j = 0; j < width; j++) {
			Pixel pix;
			unsigned char color[4];
			fs.read((char*)color, 4);

			//cout << (float)color[0] << ',' << (float)color[1] << ',' << (float)color[2] << endl;

			pix.r = color[2] / 255.0f;
			pix.g = color[1] / 255.0f;
			pix.b = color[0] / 255.0f;

			line.push_back(pix);
		}
		texture.push_back(line);
	}
	fs.close();

	return texture;
}