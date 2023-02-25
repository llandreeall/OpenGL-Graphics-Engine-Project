#include "SaveScene.h"
#include <queue>

void SaveScene::WriteFile(std::vector<GameObject*> roots, string path, glm::vec3 light)
{
	ptree pt;
    ptree objects;
	ptree child;
    ptree childpos;
    
    for each (GameObject * root in roots) {
        std::queue<GameObject*> q;
        q.push(root);
        while (!q.empty())
        {
            int n = q.size();

            while (n > 0)
            {
                child.clear();
                childpos.clear();

                GameObject* p = q.front();
                q.pop();

                //save p game object
                child.add("id", p->id);
                child.add("meshName", p->meshKey);
                if(p->parent == nullptr) child.add("parentName", "NULL");
                else child.add("parentName", p->parent->id);
                childpos.add("x", p->transform.position.x);
                childpos.add("y", p->transform.position.y);
                childpos.add("z", p->transform.position.z);
                child.add_child("position", childpos);
                childpos.clear();
                childpos.add("x", p->transform.scale.x);
                childpos.add("y", p->transform.scale.y);
                childpos.add("z", p->transform.scale.z);
                child.add_child("scale", childpos);
                childpos.clear();
                childpos.add("x", p->transform.rotation.x);
                childpos.add("y", p->transform.rotation.y);
                childpos.add("z", p->transform.rotation.z);
                child.add_child("rotation", childpos);
                childpos.clear();
                childpos.add("x", p->color.x);
                childpos.add("y", p->color.y);
                childpos.add("z", p->color.z);
                child.add_child("color", childpos);
                childpos.clear();

                objects.add_child("objects.object", child);
                for (int i = 0; i < p->children.size(); i++)
                    q.push(p->children[i]);
                n--;
            }
        }
    }
    pt.add_child("scene", objects);
    childpos.add("x", light.x);
    childpos.add("y", light.y);
    childpos.add("z", light.z);
    pt.add_child("scene.light", childpos);
    write_xml(path + "saved_scene.xml", pt);
}

std::vector<GameObject*> SaveScene::LoadScene(string path, int& id, std::unordered_map<std::string, Mesh*> meshes) {
    ptree pt;

    try {
        read_xml(path + "saved_scene.xml", pt);
    }
    catch (...) {
        std::cout << "LOAD SCENR FAILED";
        
    }
    std::vector<GameObject*> firstLevell;
    for (auto& p : pt.get_child("scene.objects")) {
        
        Mesh* mesh = meshes["cube_A"];
        string meshName = "sphere";
        glm::vec3 pos = glm::vec3(0,0,0);
        glm::vec3 rot = glm::vec3(0, 0, 0);
        glm::vec3 sc = glm::vec3(1, 1, 1);
        glm::vec3 col = glm::vec3(0, 1, 0);
        for (auto& c : p.second) {
            
            if (strcmp(c.first.data(), "meshName") == 0) {
                for each (std::pair<string, Mesh*> mesh1 in meshes) {
                    const char* key = mesh1.first.c_str();
                    if (key == c.second.data()) {
                        mesh = mesh1.second;
                        meshName = mesh1.first;
                    }
                }
            } 
            float x = 0, y = 0, z = 0;
            if (strcmp(c.first.data(),"position") == 0) {
                for (auto& d : c.second) {
                    if (strcmp(d.first.data(), "x") == 0) x = std::stof(d.second.data());
                    if (strcmp(d.first.data(), "y") == 0) y = std::stof(d.second.data());
                    if (strcmp(d.first.data(), "z") == 0) z = std::stof(d.second.data());
                }
                std::cout << "x " << x << " y " << y << " z " << z << std::endl;
                pos = glm::vec3(x, y, z);
            }
            if (strcmp(c.first.data(), "scale") == 0) {
                for (auto& d : c.second) {
                    if (strcmp(d.first.data(), "x") == 0) x = std::stof(d.second.data());
                    if (strcmp(d.first.data(), "y") == 0) y = std::stof(d.second.data());
                    if (strcmp(d.first.data(), "z") == 0) z = std::stof(d.second.data());
                }
                sc = glm::vec3(x, y, z);
            }
            if (strcmp(c.first.data(), "rotation") == 0) {
                for (auto& d : c.second) {
                    if (strcmp(d.first.data(), "x") == 0) x = std::stof(d.second.data());
                    if (strcmp(d.first.data(), "y") == 0) y = std::stof(d.second.data());
                    if (strcmp(d.first.data(), "z") == 0) z = std::stof(d.second.data());
                }
                rot = glm::vec3(x, y, z);
            }
            if (strcmp(c.first.data(), "color") == 0) {
                for (auto& d : c.second) {
                    if (strcmp(d.first.data(), "x") == 0) x = std::stof(d.second.data());
                    if (strcmp(d.first.data(), "y") == 0) y = std::stof(d.second.data());
                    if (strcmp(d.first.data(), "z") == 0) z = std::stof(d.second.data());
                }
                col = glm::vec3(x, y, z);
            }
            
        }

        GameObject* obj = new GameObject(id, mesh, meshName, pos, sc, rot);

        obj->SetColor(col);
        firstLevell.push_back(obj);
        //create object
        //set pos
        //set scale
        //set rot
        //set color
    }

    return firstLevell;
}
