#include "GameObject.h"

GameObject::GameObject()
{
	level = 0;
	selected = false;
	parent = nullptr;
}

GameObject::GameObject(glm::vec3 pos, glm::vec3 scale, glm::vec3 rot)
{
	transform.position = pos;
	transform.scale = scale;
	transform.rotation = rot;
}

GameObject::GameObject(int &id, Mesh* mesh, std::string key, glm::vec3 pos, glm::vec3 scale, glm::vec3 rot) {

	this->mesh = mesh;
	this->meshKey = key;

	this->id = id;
	this->selected = false;
	this->nameRef = this->meshKey + "-" + std::to_string(this->id);

	id++;

	this->transform.position = pos;
	this->transform.scale = scale;
	this->transform.rotation = rot;
}

void GameObject::AttachMesh(Mesh* mesh, std::string key)
{
	this->mesh = mesh;
	this->meshKey = key;
	
}

void GameObject::AddChild(GameObject* obj)
{
	children.push_back(obj);
}

void GameObject::SetParent(GameObject* obj, int &maxLevel)
{
	if (obj != NULL) {
		level = obj->level + 1;
		if (level > maxLevel) maxLevel = level;
	}
	else {
		if (level >= maxLevel) maxLevel = parent->level;
		level = 0;
	}
	parent = obj;
}

void GameObject::SetPosition(glm::vec3 pos)
{
	transform.position = pos;
}

void GameObject::SetScale(glm::vec3 scale)
{
	transform.scale = scale;
}

void GameObject::SetRotation(glm::vec3 rot)
{
	transform.rotation = rot;
}

glm::vec3 GameObject::GetPosition()
{
	return transform.position;
}

glm::vec3 GameObject::GetScale()
{
	return transform.scale;
}

glm::vec3 GameObject::GetRotation()
{
	return transform.rotation;
}

void GameObject::SetColor(glm::vec3 rot)
{
	this->color = rot;
}

glm::vec3 GameObject::GetColor()
{
	return color;
}

void GameObject::SetIsColliding(bool val)
{
	this->isColliding = val;
}

bool GameObject::GetIsColliding()
{
	return isColliding;
}

void GameObject::SetID(int &id)
{
	this->id = id;
	selected = false;
	nameRef = this->meshKey + "-" + std::to_string(this->id);
	
	id++;
}

bool GameObject::ContainsChild(const char *val) {
	for each (GameObject *child in children) {
		if (child->nameRef.c_str() == val) {
			return true;
		}
	}
	return false;
}

bool GameObject::IsLeaf()
{
	return children.size() == 0;
}

void GameObject::GoLeft(float val) {
	transform.position.x += val;

	for each (GameObject * obj in children)
		obj->GoLeft(val);
}

void GameObject::GoTop(float val) {
	transform.position.y += val;

	for each (GameObject * obj in children)
		obj->GoTop(val);
}

void GameObject::GoFront(float val) {
	transform.position.z += val;

	for each (GameObject * obj in children)
		obj->GoFront(val);
}

void GameObject::Scale(float val) {
	transform.scale.x += val;
	transform.scale.y += val;
	transform.scale.z += val;
}


GameObject::~GameObject()
{
}
