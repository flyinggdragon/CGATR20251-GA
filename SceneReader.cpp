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

string SceneReader::readScene(string path) {
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

vector<Obj3D*> SceneReader::getObjects(string content) {
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

        if (token == "obj") {
            if (!firstObj) {
                objects.push_back(obj);
                obj = new Obj3D(); // <-- corrige o escopo do ponteiro
            }

            firstObj = false;
            sline >> obj->name;
        }

        else if (token == "loc") {
            string path;
            sline >> path;

            string objData = objReader.readObj(path);

            if (objData.empty()) {
                std::cerr << "[ERRO] Arquivo OBJ vazio ou não encontrado: " << path << std::endl;
                continue; // pula essa iteração se o arquivo estiver inválido
            }

            obj->mesh = objReader.readMesh(objData);

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

            obj->deletable = (value == 1);
        }
    }

    // Adiciona o último objeto se for válido
    if (!firstObj) {
        objects.push_back(obj);
    }

    return objects;
}