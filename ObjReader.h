#pragma once
#include<GL/glew.h>
#include<string>
#include"Mesh.h"

using namespace std;

class ObjReader {
public:
	ObjReader();
	~ObjReader();
	Mesh* ReadMesh(string content);
	glm::mat4 ScaleObj();
	string ReadObj(string path);
	void GenGroupObjects(Group* group, Mesh* mesh);
};