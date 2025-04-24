#include "Mesh.h"
#include "Group.h"
#include "Face.h"

Mesh::Mesh() { }

Mesh::~Mesh() { }

void Mesh::GenMinMax() {
    for (Group* g : groups) {
        for (Face* f : g->faces) {
            for (int i = 0; i < f->verts.size(); i++) {
                glm::vec3 v = vertices[f->verts[i]];
                
                min.x = glm::min(min.x, v.x);
                min.y = glm::min(min.y, v.y);
                min.z = glm::min(min.z, v.z);
                max.x = glm::max(max.x, v.x);
                max.y = glm::max(max.y, v.y);
                max.z = glm::max(max.z, v.z);
            }
        }
    }
}

float Mesh::GetDiameter() {
    return sqrt(pow((max.x - min.x), 2) + pow((max.y - min.y), 2) + pow((max.z - min.z), 2));
}

glm::vec3 Mesh::DetermineCenter() {
    float x = (max.x - min.x) / 2;
    float y = (max.y - min.y) / 2;
    float z = (max.z - min.z) / 2;

    return glm::vec3(x, y, z);
}