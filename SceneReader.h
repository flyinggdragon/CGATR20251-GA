#pragma once
#include"Obj3D.h"

class SceneReader {
	public:
		SceneReader();
		~SceneReader();
		string readScene(string path);
		vector<Obj3D*> getObjects(string content);
};