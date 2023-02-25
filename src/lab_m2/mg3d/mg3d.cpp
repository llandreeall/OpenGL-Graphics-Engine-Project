#include "lab_m2/mg3d/mg3d.h"
#include <vector>
#include <cmath>
#include <iostream>

#include "stb/stb_image.h"
#include <queue>


using namespace std;
using namespace m2;

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


MG3::MG3()
{
}


MG3::~MG3()
{
}

void MG3::InitCamera() {
    Camera* cam = new Camera();
    cam->SetPerspective(60, window->props.aspectRatio, 0.01f, 200);
    cam->m_transform->SetMoveSpeed(2);
    cam->m_transform->SetWorldPosition(glm::vec3(0, 1.6f, 2.5));
    cam->m_transform->SetWorldRotation(glm::vec3(-15, 0, 0));
    cam->Update();

    fpsCam = new FPSCamera(cam);
}

void MG3::DisplayHierarchy(GameObject* object) {
    std::string name = object->nameRef;
    ImVec4 color = (!object->selected) ? ImVec4(0.0f, 1.0f, 0.0f, 1.00f): ImVec4(1.0f, 0.0f, 0.0f, 1.00f);
    ImGui::PushStyleColor(ImGuiCol_Button, color);
    for (int i = 0; i < object->level; i++) {
        ImGui::Text("   ");
        ImGui::SameLine();
    }
    if(ImGui::Button(object->nameRef.c_str())) printf("%s\n", name.c_str());
    ImGui::PopStyleColor();
    //object->isColliding = false;
    if (!object->IsLeaf()) {
        for each (GameObject * aux in object->children) {
            DisplayHierarchy(aux);
        }
    }
}

void MG3::DisplayHierarchyInLogs(GameObject* root) {
    if (root == NULL)
        return;

    queue<GameObject*> q;  
    q.push(root); 
    while (!q.empty())
    {
        int n = q.size();

        while (n > 0)
        {
            GameObject* p = q.front();
            q.pop();
            for (int i = 0; i < p->level; i++) {
                printf("   ");
            }
            printf("%s\n", p->nameRef.c_str());

            for (int i = 0; i < p->children.size(); i++)
                q.push(p->children[i]);
            n--;
        }

        printf("\n");
    }
}

void MG3::RenderHierarchy(GameObject* object, Shader* shader) {
    int id_value = shader->GetUniformLocation("id");

    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix = glm::translate(modelMatrix, object->transform.position);
    modelMatrix = glm::scale(modelMatrix, object->transform.scale);
    
    glUniform1i(id_value, object->id);
    int coll_value = shader->GetUniformLocation("isColliding");
    glUniform1i(coll_value, object->GetIsColliding() ? 1 : 0);

    RenderSimpleMeshWithLight(object->mesh, shaders["LightShader"], modelMatrix, object->color, mapTextures["white"]);
    
    if (!object->IsLeaf()) {
        for each (GameObject * aux in object->children) {
            RenderHierarchy(aux, shader);
        }
    }
}

void MG3::FindObject(int searchID, std::vector<GameObject*> objects) {
    for each (GameObject * obj in objects) {
        if (obj->id == searchID) {
            obj->selected = true;
            //current_item = obj->nameRef.c_str();
            selected_item = obj->nameRef.c_str();
        }
        else {
            obj->selected = false;
            
        }
        if (!obj->IsLeaf()) {
            FindObject(searchID, obj->children);
        }
    }
    
    
}

void MG3::UpdateList(GameObject* object, GameObject* parent) {
    if (object == NULL)
        return;

    queue<GameObject*> q;
    q.push(object);
    while (!q.empty())
    {
        int n = q.size();

        while (n > 0)
        {
            GameObject* p = q.front();
            q.pop();
            
            if (p->nameRef.c_str() != selected_item ) 
                itemsForCombo.push_back(p->nameRef.c_str());
            for (int i = 0; i < p->children.size(); i++)
                q.push(p->children[i]);
            n--;
        }
    }

}

GameObject* MG3::FindObjectWithReturn(int searchID, GameObject* root){
    if (root == NULL)
        return nullptr;

    queue<GameObject*> q;
    q.push(root);
    while (!q.empty())
    {
        int n = q.size();

        while (n > 0)
        {
            GameObject* p = q.front();
            q.pop();
            if (p->id == searchID) return p;

            for (int i = 0; i < p->children.size(); i++)
                q.push(p->children[i]);
            n--;
        }
    }
    return nullptr;
}

GameObject* MG3::FindObjectBasedOnText(const char* text, GameObject* root) {//std::vector<GameObject*> objects) {
    if (root == NULL)
        return nullptr;

    queue<GameObject*> q;
    q.push(root);
    while (!q.empty())
    {
        int n = q.size();

        while (n > 0)
        {
            GameObject* p = q.front();
            q.pop();
            if (p->nameRef.c_str() == text) return p;

            for (int i = 0; i < p->children.size(); i++)
                q.push(p->children[i]);
            n--;
        }
    }
    return nullptr;
}

std::vector<GameObject*> MG3::DeleteFromVector(GameObject* value, std::vector<GameObject*> vect) {
   //std::cout << vect.size() << std::endl;
   int pos = -1;
   for (int i = 0; i < vect.size(); i++) {
       if (vect[i] == value) {
           pos = i;
           break;
       }
   }
   if(pos > -1) vect.erase(vect.begin() + pos);
   
   return vect;
}

void MG3::UpdateHierarchy(GameObject* parent, GameObject* child) {
    if (parent != nullptr) {
        if (child->parent == nullptr) {
            printf("%s\n", "set parent after null");
            child->SetParent(parent, maxLevel);
            parent->AddChild(child);
            firstLevel = DeleteFromVector(child, firstLevel);
        }
        else {
            printf("%s\n", "set parent after NOT null");
            GameObject* oldParent = child->parent;
            oldParent->children = DeleteFromVector(child, oldParent->children);
            child->SetParent(parent, maxLevel);
            parent->AddChild(child);
        }
    }
    else {
        if (child->parent != nullptr) {
            printf("%s\n", "set null parent after null");
            GameObject* oldParent = child->parent;
            oldParent->children = DeleteFromVector(child, oldParent->children);
            child->SetParent(parent, maxLevel);
            firstLevel.push_back(child);
        }
    }
}

bool MG3::IsColliding(GameObject* obj1, GameObject* obj2) 
{
    bool collX = obj1->transform.position.x + obj1->transform.scale.x >= obj2->transform.position.x &&
        obj2->transform.position.x + obj2->transform.scale.x >= obj1->transform.position.x;
   
    bool collY = obj1->transform.position.y + obj1->transform.scale.y >= obj2->transform.position.y &&
        obj2->transform.position.y + obj2->transform.scale.y >= obj1->transform.position.y;
    
    bool collZ = obj1->transform.position.z + obj1->transform.scale.z >= obj2->transform.position.z &&
        obj2->transform.position.z + obj2->transform.scale.z >= obj1->transform.position.z;

    return (collX && collY && collZ);
}

void MG3::CheckCollisions() {
    queue<GameObject*> q;
    vector<GameObject*> allObjects;
    for each (GameObject * obj in firstLevel) {
        q.push(obj);
        while (!q.empty())
        {
            int n = q.size();

            while (n > 0)
            {
                GameObject* p = q.front();
                q.pop();
                allObjects.push_back(p);
                for (int i = 0; i < p->children.size(); i++)
                    q.push(p->children[i]);
                n--;
               
            }
        }
    }
    
    for each (GameObject* obj1 in allObjects) {
        bool reset = true;
        for each(GameObject * obj2 in allObjects) {
            bool col = obj1 != obj2 ? IsColliding(obj1, obj2) : false;
            if (col == true) {
                obj1->SetIsColliding(true);
                obj2->SetIsColliding(true);
                reset = false;
                break;
            }
        }
        if (reset) {
            obj1->SetIsColliding(false);
        }
    }
}

const char* glsl_version = "#version 130";
void MG3::Init()
{
    InitCamera();

   
    auto camera = fpsCam->GetCamera();
    camera->SetPositionAndRotation(glm::vec3(0, 2, 4), glm::quat(glm::vec3(-30 * TO_RADIANS, 0, 0)));
    camera->Update();

    ToggleGroundPlane();

    
    std::string texturePath = PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES, "cube");
    std::string shaderPath = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "mg3d", "shaders");
    const string sourceTextureDir = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "mg3d", "textures");

    
    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "grey.png").c_str(), GL_REPEAT);
        mapTextures["grey"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "white.png").c_str(), GL_REPEAT);
        mapTextures["white"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "sun.png").c_str(), GL_REPEAT);
        mapTextures["sun"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "earth.png").c_str(), GL_REPEAT);
        mapTextures["earth"] = texture;
    }

    {
        TextureManager::LoadTexture(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::TEXTURES), "particle2.png");
    }

    {
        Mesh* mesh = new Mesh("cube");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        mesh->UseMaterials(false);
   
        meshes["cube"] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        mesh->UseMaterials(false);

        meshes["sphere"] = mesh;
    }

    {
        Mesh* mesh = new Mesh("quad");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "quad.obj");
        mesh->UseMaterials(false);
        meshes[mesh->GetMeshID()] = mesh;
    }
    cullFace = GL_BACK;
    polygonMode = GL_FILL;

    {
        vector<VertexFormat> vertices
        {
            VertexFormat(glm::vec3(0, 0,  1), glm::vec3(1, 0, 0)),
           
            VertexFormat(glm::vec3(1, 0,  1), glm::vec3(0.8, 0.2, 0)),
            VertexFormat(glm::vec3(0, 1,  1), glm::vec3(0.6, 0.4, 0)),
            VertexFormat(glm::vec3(1, 1,  1), glm::vec3(0.4, 0.6, 0)),
            VertexFormat(glm::vec3(0, 0,  0), glm::vec3(0, 0.6, 0.4)),
            VertexFormat(glm::vec3(1, 0,  0), glm::vec3(0, 0.4, 0.6)),
            VertexFormat(glm::vec3(0, 1,  0), glm::vec3(0, 0.2, 0.8)),
            VertexFormat(glm::vec3(1, 1,  0), glm::vec3(0, 0, 1))
        };

        vector<unsigned int> indices =
        {
            0, 1, 2,   
            1, 3, 2,    
            2, 3, 7,
            2, 7, 6,
            1, 7, 3,
            1, 5, 7,
            6, 7, 4,
            7, 5, 4,
            0, 4, 1,
            1, 4, 5,
            2, 6, 4,
            0, 2, 4
        };

        meshes["cube_A"] = new Mesh("cube_A");
        meshes["cube_A"]->InitFromData(vertices, indices);

        Mesh* mesh = meshes["cube_A"];
        //mesh->UseMaterials(false);
    }

    
    {
        Shader *shader = new Shader("ShaderNormal");
        shader->AddShader(PATH_JOIN(shaderPath, "Normal.VS.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(shaderPath, "Normal.FS.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader* shader = new Shader("LightShader");
        shader->AddShader(PATH_JOIN(shaderPath, "Light.VS.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(shaderPath, "Light.FS.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    
    ImGui_ImplGlfw_InitForOpenGL(window->GetHandle(), true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    {
        lightPos = glm::vec3(0, 2, 0);
        shininess = 50;
        matKd = 3;
        matKs = 0.5;

        timer = 0;
    }


}


void MG3::FrameStart()
{
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    
    glViewport(0, 0, resolution.x, resolution.y);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}



void MG3::Update(float deltaTimeSeconds)
{
    ClearScreen();
    auto camera = fpsCam->GetCamera();

    glm::ivec2 resolution = window->GetResolution();
    
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    {
        static float f = 0.0f;
        static int counter = 0;
        ImGui::SetNextWindowSize(ImVec2(resolution.x/5,resolution.y), ImGuiCond(0));
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond(0));
        ImGui::Begin("Scene Hierarchy");    
        for each(GameObject* obj in firstLevel)
            DisplayHierarchy(obj);

        ImGui::End();
    }
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, lightPos);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f));
        RenderSimpleMesh(meshes["sphere"], shaders["ShaderNormal"], modelMatrix, glm::vec3(1, 0, 0), mapTextures["sun"]);
    }
    {
        ImGui::SetNextWindowSize(ImVec2(resolution.x / 9, resolution.y), ImGuiCond(0));
        ImGui::SetNextWindowPos(ImVec2(resolution.x - resolution.x / 9, 0), ImGuiCond(0));
        ImGui::Begin("Object List");
        
        for each(std::pair<string,Mesh*> mesh in meshes) {
            const char* key = mesh.first.c_str();
            if (ImGui::Button(key)) {
                GameObject* obj = new GameObject(lastID, mesh.second, mesh.first, glm::vec3(lastID *2, 0, 0), glm::vec3(1, 1, 1), glm::vec3(0, 0, 0));
                
                obj->SetColor(glm::vec3((float)1/(lastID+0.01), 0.5f, 0.0f));
                firstLevel.push_back(obj);
                if (loadScene == true) loadScene = false;
            }
        }
        ImGui::End();
    }
    {
            ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond(0));
            
            itemsForCombo.clear();
            itemsForCombo.push_back("NULL");
            
            for each (GameObject * obj in firstLevel)
                UpdateList(obj, obj);
            ImGui::Begin("Edit Object");
            //find selected item
            GameObject* findSelected = nullptr;
            for each (GameObject * obj in firstLevel) {
                findSelected = FindObjectWithReturn(selectedID, obj);
                if (findSelected != nullptr) break;
            }

            if (ImGui::BeginCombo("Set Parent", current_item)) {
                for (int n = 0; n < itemsForCombo.size(); n++)
                {
                    bool is_selected = (current_item == itemsForCombo.at(n)); // You can store your selection however you want, outside or inside your objects
                    if (ImGui::Selectable(itemsForCombo.at(n), is_selected)) {
                        current_item = itemsForCombo.at(n);
                        //find parent
                        GameObject* parent = nullptr;
                        if (current_item != "NULL") {
                            for each (GameObject * obj in firstLevel) {
                                parent = FindObjectBasedOnText(current_item, obj);
                                if (parent != nullptr) break;
                            }
                        } else parent = nullptr;
                        
                        if (findSelected != nullptr) {
                            printf("%s parent:%s child:%s\n", "selection", parent->nameRef.c_str(), findSelected->nameRef.c_str());
                            UpdateHierarchy(parent, findSelected);
                            for each (GameObject * obj in firstLevel)
                                DisplayHierarchyInLogs(obj);
                        }
                    }
                    if (is_selected) {
                       ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
  
                    }
                }
                ImGui::EndCombo();
            }
            if (ImGui::Button("Save Scene")) {
                xmlSaveScene.WriteFile(firstLevel, path, lightPos);

            }
            if (loadScene == true) {
                ImGui::SameLine();
                if (ImGui::Button("LoadScene")) {
                    loadScene = false;
                    firstLevel = xmlSaveScene.LoadScene(path, lastID, meshes);
                }
            }
            ImGui::Button("<- Left", ImVec2(80, 45));
            if (ImGui::IsItemActive() && findSelected != nullptr) {
                findSelected->GoLeft(-0.1);
            }
            ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::Button("^ Up", ImVec2(60, 20));
            if (ImGui::IsItemActive() && findSelected != nullptr) {
                findSelected->GoTop(0.1);
            }
            ImGui::Button("v Down", ImVec2(60, 20));
            if (ImGui::IsItemActive() && findSelected != nullptr) {
                findSelected->GoTop(-0.1);
            }
            ImGui::EndGroup();
            ImGui::SameLine();
            ImGui::Button("Right ->", ImVec2(80, 45));
            if (ImGui::IsItemActive() && findSelected != nullptr) {
                findSelected->GoLeft(0.1);
            }
            ImGui::Button("o Front", ImVec2(90, 20));
            if (ImGui::IsItemActive() && findSelected != nullptr) {
                findSelected->GoFront(0.1);
            }
            ImGui::SameLine();
            ImGui::Button("x Back", ImVec2(90, 20));
            if (ImGui::IsItemActive() && findSelected != nullptr) {
                findSelected->GoFront(-0.1);
            }
            ImGui::Button("Scale Up", ImVec2(90, 20));
            if (ImGui::IsItemActive() && findSelected != nullptr) {
                findSelected->Scale(+0.01);
            }
            ImGui::SameLine();
            ImGui::Button("Scale Down", ImVec2(90, 20));
            if (ImGui::IsItemActive() && findSelected != nullptr) {
                findSelected->Scale(-0.01);
            }
            ImGui::End();
    }
    timer += deltaTimeSeconds;
    if (timer >= detectionPeriod) {
        timer -= detectionPeriod;
        CheckCollisions();
    }
    {
        Shader* shader = shaders["LightShader"];
        shader->Use();
        glm::mat4 modelMatrix;
        
        int id_value = shader->GetUniformLocation("id");
        int id_selected = shader->GetUniformLocation("idSelected");
        glUniform1i(id_selected, selectedID);
   
        for each (GameObject * obj in firstLevel) {
            RenderHierarchy(obj, shader);
        }
    }
}


void MG3::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::vec3 color, Texture2D* texture1)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    glUseProgram(shader->program);

   
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    auto camera = fpsCam->GetCamera();
   
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    glm::mat4 projectionMatrix = camera->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    {
        int loc_color = glGetUniformLocation(shader->program, "color");
        glUniform3fv(loc_color, 1, glm::value_ptr(color));
    }
    
    if (texture1)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
        glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);
    }

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

void MG3::RenderSimpleMeshWithLight(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::vec3 color, Texture2D* texture1)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    glUseProgram(shader->program);
    auto camera = fpsCam->GetCamera();

    //iluminare
    glUniform3fv(glGetUniformLocation(shader->program, "light_pos"), 1, glm::value_ptr(lightPos));
    glm::vec3 eyePosition = camera->m_transform->GetWorldPosition();
    glUniform3fv(glGetUniformLocation(shader->program, "eye_pos"), 1, glm::value_ptr(eyePosition));
    glUniform1f(glGetUniformLocation(shader->program, "mat_kd"), matKd);
    glUniform1f(glGetUniformLocation(shader->program, "mat_ks"), matKs);
    glUniform1i(glGetUniformLocation(shader->program, "mat_shininess"), shininess);
    //end iluminare 

    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glm::mat4 viewMatrix = camera->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    glm::mat4 projectionMatrix = camera->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    {
        int loc_color = glGetUniformLocation(shader->program, "color");
        glUniform3fv(loc_color, 1, glm::value_ptr(color));
    }

    if (texture1)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
        glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);
    }

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}



void MG3::CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices)
{
    unsigned int VAO = 0;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO = 0;
  
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    unsigned int IBO = 0;
    
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
    
    glBindVertexArray(0);
    
    if (GetOpenGLError() == GL_INVALID_OPERATION)
    {
        cout << "\t[NOTE] : For students : DON'T PANIC! This error should go away when completing the tasks." << std::endl;
        cout << "\t[NOTE] : For developers : This happens because OpenGL core spec >=3.1 forbids null VAOs." << std::endl;
    }
    cout << indices.size();
    
    meshes[name] = new Mesh(name);
    meshes[name]->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
    cout << meshes[name]->indices.size();
}

void MG3::LoadShader(const std::string& name, bool hasGeomtery)
{
    std::string shaderPath = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M2, "mg3d", "shaders");

    {
        Shader* shader = new Shader(name);
        shader->AddShader(PATH_JOIN(shaderPath, name + ".VS.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(shaderPath, name + ".FS.glsl"), GL_FRAGMENT_SHADER);
        if (hasGeomtery)
        {
            shader->AddShader(PATH_JOIN(shaderPath, name + ".GS.glsl"), GL_GEOMETRY_SHADER);
        }

        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }
}

void MG3::FrameEnd()
{
    DrawCoordinateSystem(fpsCam->GetCamera());
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void MG3::OnInputUpdate(float deltaTime, int mods)
{
    float speed = 2;

    if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        auto camera = fpsCam->GetCamera();
        glm::vec3 up = glm::vec3(0, 1, 0);
        glm::vec3 right = camera->m_transform->GetLocalOXVector();
        glm::vec3 forward = camera->m_transform->GetLocalOZVector();
        forward = glm::normalize(glm::vec3(forward.x, 0, forward.z));

        // Control light position using on W, A, S, D, E, Q
        if (window->KeyHold(GLFW_KEY_W)) lightPos -= forward * deltaTime * speed;
        if (window->KeyHold(GLFW_KEY_A)) lightPos -= right * deltaTime * speed;
        if (window->KeyHold(GLFW_KEY_S)) lightPos += forward * deltaTime * speed;
        if (window->KeyHold(GLFW_KEY_D)) lightPos += right * deltaTime * speed;
        if (window->KeyHold(GLFW_KEY_E)) lightPos += up * deltaTime * speed;
        if (window->KeyHold(GLFW_KEY_Q)) lightPos -= up * deltaTime * speed;
    }
}


void MG3::OnKeyPress(int key, int mods)
{
    
    if (key == GLFW_KEY_UP) 
    {
        //index++;
        //GameObject* obj = new GameObject(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), glm::vec3(0, 0, 0));
        //obj->AttachMesh(meshes["cube_A"]);
        //obj->SetID(lastID);
        //firstLevel.push_back(obj);
    }
    if (key == GLFW_KEY_DOWN && index > 0)
    {
        //index--;
    }

}


void MG3::OnKeyRelease(int key, int mods)
{
   
}


void MG3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
   
}


void MG3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    if (window->MouseHold(GLFW_MOUSE_BUTTON_LEFT)) {
        unsigned char data[4];

        mouseY = window->props.resolution.y - mouseY;

        glReadPixels(mouseX, mouseY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
        if (data[3] != 255) {
            selectedID = data[3];
            FindObject(selectedID, firstLevel);
            //if (aux != nullptr) aux->selected = true;
        }
        //printf("%d", selectedID);
    }
    
}


void MG3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
   
}


void MG3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
   
}


void MG3::OnWindowResize(int width, int height)
{
  
}
