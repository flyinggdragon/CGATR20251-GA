#pragma once
#include<GL/glew.h>
#include<string>
#include"Mesh.h"

using namespace std;

class ObjReader {
public:
	ObjReader();
	~ObjReader();
	Mesh* readMesh(string content);
	string readObj(string path);
	void genGroupObjects(Group* group, Mesh* mesh);
};