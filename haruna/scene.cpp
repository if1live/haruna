// Ŭnicode please 
#include "stdafx.h"
#include "scene.h"

namespace haruna {;
SceneNode::SceneNode()
	: parent(nullptr),
	local_scale(1),
	world_scale(1)
{
}
SceneNode::~SceneNode()
{
}
void SceneNode::UpdateWorldTransform()
{
	if(parent) {
		world_scale = parent->world_scale * local_scale;
		world_rotate = parent->world_rotate * local_rotate;
		world_translate = parent->world_translate + parent->world_scale * parent->world_rotate * local_translate;
	} else {
		world_scale = local_scale;
		world_rotate = local_rotate;
		world_translate = local_translate;
	}
}

glm::mat4 SceneNode::GetMatrix()
{
	glm::mat4 m(world_rotate);
	for(int i = 0 ; i < 3 ; ++i) {
		for(int j = 0 ; j < 3 ; ++j) {
			m[i][j] *= world_scale;
		}
	}
	m[3][0] = world_translate.x;
	m[3][1] = world_translate.y;
	m[3][2] = world_translate.z;
	return m;
}


////////////////////////////////

ContainerNode::ContainerNode()
{
}
ContainerNode::~ContainerNode()
{
}
void ContainerNode::UpdateWorldTransform()
{
	SceneNode::UpdateWorldTransform();
	for(SceneNode *child : children_) {
		child->UpdateWorldTransform();
	}
}

bool ContainerNode::Add(SceneNode *node)
{
	//중복 추가 불가능
	auto it = std::find(children_.begin(), children_.end(), node);
	if(it != children_.end()) {
		return false;
	}
	//자기 자신 넣기 불가능
	if(node == this) {
		return false;
	}
	if(node == parent) {
		return false;
	}

	node->parent = this;
	children_.push_back(node);
	return true;
}
bool ContainerNode::Detach(SceneNode *scene)
{
	auto it = std::find(children_.begin(), children_.end(), scene);
	if(it == children_.end()) {
		return false;
	} else {
		children_.erase(it);
		return true;
	}
}
bool ContainerNode::Remove(SceneNode *node)
{
	bool detach_result = Detach(node);
	if(detach_result) {
		delete(node);
	}
	return detach_result;
}


}	// namespace haruna
