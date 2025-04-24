#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <vector>
#include <cmath>

#include "Mesh.h"
#include "Group.h"
#include "Face.h"
#include "Obj3D.h"
#include "Shot.h"
#include "ObjReader.h"

Shot::Shot(glm::vec3 cameraPosition, glm::vec3 cameraFront) {
	shotObj = GenSphere();
	speed = 8.0f;

    transform = glm::mat4(1.0f);
    transform = glm::scale(transform, glm::vec3(0.5f));
    transform = glm::translate(transform, cameraPosition);

	startPos = cameraPosition;
    position = cameraPosition;
    
    direction = glm::normalize(cameraFront);

	target = cameraPosition + direction * 10.0f;

	shotLifetime = 6.0f;
}

Shot::~Shot() {
    delete shotObj;
    shotObj = nullptr;
}

void Shot::Update(float deltaTime) {
    position += direction * speed * deltaTime;

    transform = glm::mat4(1.0f);
    transform = glm::translate(transform, position);
    transform = glm::scale(transform, glm::vec3(0.5f));
}

void Shot::Reflect(Obj3D* obj) {
    glm::vec3 objCenter = glm::vec3(obj->transform * glm::vec4(0, 0, 0, 1));
    glm::vec3 shotCenter = glm::vec3(transform * glm::vec4(0, 0, 0, 1));
    glm::vec3 diff = shotCenter - objCenter;

    float absX = fabs(diff.x);
    float absY = fabs(diff.y);
    float absZ = fabs(diff.z);

    if (absX > absY && absX > absZ) {
        direction.x *= -1.0f;
    }
    else if (absY > absX && absY > absZ) {
        direction.y *= -1.0f;
    }
    else {
        direction.z *= -1.0f;
    }
}

Obj3D* Shot::GenSphere() {
    const int sectorCount = 24;
    const int stackCount = 16;
    const float radius = 0.1f;

    std::vector<float> vertices;
    std::vector<GLuint> indices;

    for (int i = 0; i <= stackCount; ++i) {
        float stackAngle = glm::pi<float>() / 2 - i * glm::pi<float>() / stackCount;
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = j * 2 * glm::pi<float>() / sectorCount;

            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }

    for (int i = 0; i < stackCount; ++i) {
        for (int j = 0; j < sectorCount; ++j) {
            int first = i * (sectorCount + 1) + j;
            int second = first + sectorCount + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    Group* group = new Group();
    group->numVertices = static_cast<int>(indices.size());

    glGenVertexArrays(1, &group->VAO);
    glBindVertexArray(group->VAO);

    GLuint VBO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    Mesh* mesh = new Mesh();
    mesh->groups.push_back(group);

    Obj3D* sphere = new Obj3D();
    sphere->mesh = mesh;

    mesh->GenMinMax();

    return sphere;
}