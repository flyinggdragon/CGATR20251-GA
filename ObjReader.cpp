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

Mesh* ObjReader::readMesh(string content) {
    Mesh* mesh = new Mesh;
    Group* group = new Group();
    bool firstGroup = true;

    stringstream data(content);
    string line;

    while (getline(data, line)) {
        stringstream sline(line);
        string token;
        sline >> token;

        if (token == "v") {
            float x, y, z;
            sline >> x >> y >> z;
            mesh->vertices.push_back(glm::vec3(x, y, z));
        }
        else if (token == "g") {
            if (!firstGroup) {
                mesh->groups.push_back(group);
                group = new Group();
            }
            firstGroup = false;
            sline >> group->name;
        }
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
    genGroupObjects(group, mesh);

    return mesh;
}

string ObjReader::readObj(string path) {
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

void ObjReader::genGroupObjects(Group* group, Mesh* mesh) {
    vector<GLfloat> vertices;

    for (Face* f : group->faces) {
        for (int i = 0; i < f->verts.size(); i++) {
            glm::vec3 v = mesh->vertices[f->verts[i]];
            vertices.push_back(v.x);
            vertices.push_back(v.y);
            vertices.push_back(v.z);
            group->numVertices++;
        }
    }

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, (vertices.size() * sizeof(GLfloat)),
                 vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    group->VAO = VAO;
    group->VBO = VBO;
}