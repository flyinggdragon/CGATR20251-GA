#pragma once
#include"Obj3D.h"

class SceneReader {
	public:
		SceneReader();
		~SceneReader();
		string ReadScene(string path);
		vector<Obj3D*> GetObjects(string content);
};