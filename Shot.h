#pragma once
class Shot {
	
public:
	Obj3D* shotObj;
	float speed;
	glm::mat4 transform;
	glm::vec3 startPos;
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 target;
	glm::vec3 min, max;
	float shotLifetime;

	Shot(glm::vec3 cameraPosition, glm::vec3 cameraFront);
	~Shot();
	void Update(float deltaTime);
	void Reflect(Obj3D* obj);
	Obj3D* GenSphere();
};