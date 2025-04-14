#pragma once
#include<GL/glew.h>
#include<glm.hpp>
#include<vector>

#include"Mesh.h"
#include"Face.h"

using namespace std;

class Group {
public:
	string name;
	vector<Face*> faces;
	GLuint VBO;
	GLuint VAO;
	int numVertices;
};