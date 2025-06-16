#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "ObjReader.h"
#include "Mesh.h"
#include "Group.h"
#include "Face.h"

using namespace std;

ObjReader::ObjReader() {
}

ObjReader::~ObjReader() {
}

// Faz a leitura da malha.
Mesh* ObjReader::ReadMesh(string content) {
    Mesh* mesh = new Mesh;
    Group* group = new Group();
    bool firstGroup = true;

    stringstream data(content);
    string line;

    while (getline(data, line)) {
        stringstream sline(line);
        string token;
        sline >> token;

        // Grupo.
        if (token == "g") {
            if (!firstGroup) {
                mesh->groups.push_back(group);
                group = new Group();
            }
            firstGroup = false;
            sline >> group->name;
        }

        // Vértices.
        else if (token == "v") {
            float x, y, z;
            sline >> x >> y >> z;
            mesh->vertices.push_back(glm::vec3(x, y, z));
        }

        // Normais.
        else if (token == "vn") {
            float x, y, z;
            sline >> x >> y >> z;

            glm::vec3 vn = glm::vec3(x, y, z);
            mesh->normals.push_back(vn);
        }

        // Faces.
        else if (token == "f") {
            Face* face = new Face();
            string vertexData;
            while (sline >> vertexData) {
                stringstream ss(vertexData);
                string vertexIndex;
                getline(ss, vertexIndex, '/');
                int v = stoi(vertexIndex) - 1;
                face->verts.push_back(v);
            }
            group->faces.push_back(face);
        }
    }

    mesh->groups.push_back(group);

    // Gera os VAOs de cada grupo.
    GenGroupObjects(group, mesh);

    return mesh;
}

string ObjReader::ReadObj(string path) {
    ifstream inputFile;
    inputFile.open(path);

    if (!inputFile.is_open()) {
        return nullptr;
    }

    string content;
    string line;

    while (getline(inputFile, line)) {
        content += line + "\n";
    }

    inputFile.close();
    return content;
};

// Gera os VAOs de cada grupo.
void ObjReader::GenGroupObjects(Group* group, Mesh* mesh) {
    std::vector<GLfloat> vertices;

    for (Face* f : group->faces) {
        for (int i = 0; i < f->verts.size(); i++) {
            glm::vec3 v = mesh->vertices[f->verts[i]];
            vertices.push_back(v.x);
            vertices.push_back(v.y);
            vertices.push_back(v.z);

            // Se houver normal, adiciona
            if (i < f->norms.size()) {
                glm::vec3 n = mesh->normals[f->norms[i]];
                vertices.push_back(n.x);
                vertices.push_back(n.y);
                vertices.push_back(n.z);
            }
            else {
                // Se não houver normal, coloca uma normal fake para evitar erro
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
                vertices.push_back(1.0f);
            }

            // Cor opcional: definir uma cor fixa ou randômica por vértice
            vertices.push_back(0.9f); // R
            vertices.push_back(0.52f); // G
            vertices.push_back(0.33f); // B
        }
        group->numVertices += f->verts.size();
    }

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, (vertices.size() * sizeof(GLfloat)),
                 vertices.data(), GL_STATIC_DRAW);

    GLsizei stride = 9 * sizeof(float);

    // Posição
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Cor
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    group->VAO = VAO;
    group->VBO = VBO;
}