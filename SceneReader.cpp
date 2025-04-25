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

SceneReader::SceneReader() {
}

SceneReader::~SceneReader() {
}

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
    // Faz a leitura da cena.
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

        // Viewport
        else if (token == "view") {
            float WIDTH, HEIGHT;
            sline >> WIDTH >> HEIGHT;

            glViewport(0, 0, WIDTH, HEIGHT);
        }
        
        // Novo objeto.
        else if (token == "obj") {
            if (!firstObj) {
                objects.push_back(obj);
                obj = new Obj3D();
            }
        }

        // Localização do objeto.
        else if (token == "loc") {
            string path;
            sline >> path;

            string objData = objReader.ReadObj(path);

            if (objData.empty()) {
                std::cerr << "[ERRO] Arquivo OBJ vazio ou não encontrado: " << path << std::endl;
                continue;
            }
            
            // Lê o mesh.
            obj->mesh = objReader.ReadMesh(objData);

            if (obj->mesh == nullptr) {
                std::cerr << "[ERRO] Falha ao criar mesh para o arquivo: " << path << std::endl;
                continue;
            }

            obj->mesh->GenMinMax();

            // Normaliza para caber no frustum.
            obj->ScaleObj();

            obj->center = obj->mesh->DetermineCenter();
            obj->radius = obj->mesh->GetDiameter() / 2;

            firstObj = false;
            sline >> obj->name;
        }

        // Rotação do objeto.
        else if (token == "rot") {
            float angle, x, y, z;
            sline >> angle >> x >> y >> z;

            obj->transform = glm::rotate(obj->transform, glm::radians(angle), glm::vec3(x, y, z));
        }

        // Posição do objeto.
        else if (token == "pos") {
            float x, y, z;
            sline >> x >> y >> z;

            obj->transform = glm::translate(obj->transform, glm::vec3(x, y, z));
        }

        // Define se o objeto é apagável.
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