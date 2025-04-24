#pragma once
#include<glm.hpp>
#include<vector>

#include"Mesh.h"

class Obj3D {
public:
	string name;
	Mesh* mesh;
	glm::mat4 transform;
	int status;
	bool deletable;
	glm::vec3 direction;
	glm::vec3 center;
	float radius;

	Obj3D();
	~Obj3D();
	void ScaleObj();
};