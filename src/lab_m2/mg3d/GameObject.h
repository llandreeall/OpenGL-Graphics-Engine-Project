#pragma once
#include "utils/glm_utils.h"
#include "../../src/components/simple_scene.h"
#include <string>

struct ObjectTransform {
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;
};
class GameObject
{
public:
	GameObject();
	GameObject(glm::vec3 pos, glm::vec3 scale, glm::vec3 rot);
	GameObject(int& id, Mesh* mesh, std::string key, glm::vec3 pos, glm::vec3 scale, glm::vec3 rot);
	void AttachMesh(Mesh* mesh, std::string key);
	void AddChild(GameObject* obj);
	void SetParent(GameObject* obj, int &maxLevel);
	void SetPosition(glm::vec3 pos);
	void SetScale(glm::vec3 scale);
	void SetRotation(glm::vec3 rot);
	glm::vec3 GetPosition();
	glm::vec3 GetScale();
	glm::vec3 GetRotation();
	void SetColor(glm::vec3 rot);
	glm::vec3 GetColor();
	void SetIsColliding(bool val);
	bool GetIsColliding();
	void SetID(int &id);
	bool IsLeaf();
	bool ContainsChild(const char* val);
	void GoLeft(float val);
	void GoTop(float val);
	void GoFront(float val);
	void Scale(float val);
	~GameObject();
public:
	GameObject* parent = nullptr;
	std::vector<GameObject*> children;
	ObjectTransform transform;
	Mesh* mesh;
	bool selected = false;
	int level = 0;
	int id;
	std::string meshKey;
	std::string nameRef;
	glm::vec3 color;
	bool isColliding = 0;
};

