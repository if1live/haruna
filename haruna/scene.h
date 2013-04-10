// Ŭnicode please 
#pragma once

namespace haruna {;
class SceneNode;
typedef std::unique_ptr<SceneNode> SceneNodePtr;

class SceneNode {
public:
	SceneNode();
	virtual ~SceneNode();

	virtual void UpdateWorldTransform();

	virtual bool Add(SceneNode *node) { return false; }
	virtual int Size() const { return 0; }
	virtual SceneNode *Get(int i) { return nullptr; }
	virtual bool Remove(SceneNode *scene) { return false; }
	virtual bool Detach(SceneNode *scene) { return false; }

	//leaf node에서 사용할 행렬 얻기
	glm::mat4 GetMatrix();

public:
	SceneNode *parent;

	float local_scale;
	glm::mat3 local_rotate;
	glm::vec3 local_translate;

	float world_scale;
	glm::mat3 world_rotate;
	glm::vec3 world_translate;
};

class ContainerNode : public SceneNode {
public:
	ContainerNode();
	virtual ~ContainerNode();

	virtual void UpdateWorldTransform();

	bool Add(SceneNode *node);
	int Size() const { return children_.size(); }
	SceneNode *Get(int i) { return children_[i]; }
	bool Remove(SceneNode *scene);
	bool Detach(SceneNode *scene);

private:
	std::vector<SceneNode*> children_;
};

}	// namespace haruna
