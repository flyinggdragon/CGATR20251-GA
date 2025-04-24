#pragma once
#include<glm.hpp>
#include<string>
#include<vector>

using namespace std;

class Group;

class Mesh {
public:
	string mtllib;
	glm::vec3 min, max;
	vector<glm::vec3> vertices;
	vector<glm::vec3> normals;
	vector<Group*> groups;

	Mesh();
	~Mesh();
	void GenMinMax();
	float GetDiameter();
	glm::vec3 DetermineCenter();
};