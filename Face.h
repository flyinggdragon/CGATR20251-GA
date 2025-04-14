#pragma once
#include<glm.hpp>
#include<vector>

using namespace std;

class Face {
public:
	vector<int> verts;
	vector<int> norms;
	vector<int> texts;
	int numVertices;
};