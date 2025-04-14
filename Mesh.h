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
	vector<Group*> groups;
};