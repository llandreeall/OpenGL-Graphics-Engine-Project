#pragma once

#include "GameObject.h"
#include "lab_m2/mg3d/camera/fpscamera.h"
#include "../../src/components/transform.h"

#include <string>
#include <map>
#include "ui/imgui.h"
#include "ui/imgui_impl_glfw.h"
#include "ui/imgui_impl_opengl3.h"
#include "savescene/SaveScene.h"

namespace m2
{
    class MG3 : public gfxc::SimpleScene
    {
     public:
        MG3();
        ~MG3();

        void Init() override;
        void InitCamera() ;
 
     private:
        void CreateMesh(const char* name, const std::vector<VertexFormat>& vertices, const std::vector<unsigned int>& indices);

        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;
        void LoadShader(const std::string& name, bool hasGeomtery = true);
        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::vec3 color, Texture2D* texture1 = NULL);
        void RenderSimpleMeshWithLight(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::vec3 color, Texture2D* texture1 = NULL);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        
        std::unordered_map<std::string, Texture2D*> mapTextures;
        void DisplayHierarchy(GameObject* object);
        void DisplayHierarchyInLogs(GameObject* object);
        void FindObject(int searchID, std::vector<GameObject*> objects);
        void UpdateList(GameObject* object, GameObject* parent);
        GameObject* FindObjectWithReturn(int searchID, GameObject* root);//std::vector<GameObject*> objects);
        GameObject* FindObjectBasedOnText(const char* text, GameObject* root);//std::vector<GameObject*> objects);
        std::vector<GameObject*> DeleteFromVector(GameObject* value, std::vector<GameObject*> vect);
        void UpdateHierarchy(GameObject* parent, GameObject* child);
        void RenderHierarchy(GameObject* object, Shader* shader);
        bool IsColliding(GameObject* obj1, GameObject* obj2);
        void CheckCollisions();
    protected:
    
        GLenum cullFace;
        GLenum polygonMode;
        
        int index = 0;
        int selectedID = 255;
        int currentID;
        
        int maxLevel = 0;
        FPSCamera* fpsCam;
        std::vector<GameObject*> firstLevel;
        int lastID = 0;
        bool systemBusy;
        std::vector<const char*> itemsForCombo;
        const char* current_item = NULL;
        const char* selected_item = NULL;

        //iluminare
        glm::vec3 lightPos;
        unsigned int shininess;
        float matKd; //constanta de reflexie difuza a materialului
        float matKs; //constanta de reflexie speculara a materialului

        SaveScene xmlSaveScene;
        string path = "src/lab_m2/mg3d/savescene/";

        float timer;
        float detectionPeriod = 1.0f;
        bool loadScene = true;
    };
}   
