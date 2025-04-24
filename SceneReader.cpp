#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "SceneReader.h"
#include "ObjReader.h"

using namespace std;

SceneReader::SceneReader() { }

SceneReader::~SceneReader() { }

string SceneReader::ReadScene(string path) {
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
}

vector<Obj3D*> SceneReader::GetObjects(string content) {
    vector<Obj3D*> objects;

    Obj3D* obj = new Obj3D();
    bool firstObj = true;

    ObjReader objReader;

    stringstream data(content);
    string line;

    while (getline(data, line)) {
        stringstream sline(line);
        string token;
        sline >> token;

        // -- == Comentários
        if (token == "--") {
            continue;
        }

        else if (token == "view") {
            float WIDTH, HEIGHT;
            sline >> WIDTH >> HEIGHT;

            glViewport(0, 0, WIDTH, HEIGHT);
        }

        else if (token == "obj") {
            if (!firstObj) {
                objects.push_back(obj);
                obj = new Obj3D();
            }

            firstObj = false;
            sline >> obj->name;
        }

        else if (token == "loc") {
            string path;
            sline >> path;

            string objData = objReader.ReadObj(path);

            if (objData.empty()) {
                std::cerr << "[ERRO] Arquivo OBJ vazio ou não encontrado: " << path << std::endl;
                continue;
            }

            obj->mesh = objReader.ReadMesh(objData);

            if (obj->mesh == nullptr) {
                std::cerr << "[ERRO] Falha ao criar mesh para o arquivo: " << path << std::endl;
                continue;
            }
        }

        else if (token == "sca") {
            float x, y, z;
            sline >> x >> y >> z;

            obj->transform = glm::scale(obj->transform, glm::vec3(x, y, z));
        }

        else if (token == "pos") {
            float x, y, z;
            sline >> x >> y >> z;

            obj->transform = glm::translate(obj->transform, glm::vec3(x, y, z));
        }

        else if (token == "del") {
            int value;
            sline >> value;

            obj->deletable = value == 1;
        }
    }

    if (!firstObj) {
        objects.push_back(obj);
    }

    return objects;
}