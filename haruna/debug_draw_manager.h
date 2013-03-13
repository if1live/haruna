// Ŭnicode please
#pragma once

#include <memory>
#include <list>
#include "vertex.h"

namespace haruna {;

typedef enum {
	kDebugDraw2DNone,
	kDebugDraw2DLine,
	kDebugDraw2DCross,
	kDebugDraw2DString,
	kDebugDraw2DCircle,
} Draw2DType;

typedef enum {
	kDebugDraw3DNone,
	kDebugDraw3DLine,
	kDebugDraw3DCross,
	kDebugDraw3DSphere,
	kDebugDraw3DAxis,
	kDebugDraw3DString
} DebugDraw3DType;


struct DebugDraw3D;
struct DebugDraw3D_Line;
struct DebugDraw3D_Cross;
struct DebugDraw3D_Sphere;
struct DebugDraw3D_String;
struct DebugDraw3D_Axis;

struct DebugDraw2D;
struct DebugDraw2D_Line;
struct DebugDraw2D_Cross;
struct DebugDraw2D_String;
struct DebugDraw2D_Circle;

class DebugDrawManager;

class AbstractDebugDrawer3D {
public:
	AbstractDebugDrawer3D() {}
	virtual ~AbstractDebugDrawer3D() {}

	virtual void Draw(const DebugDrawManager &mgr) = 0;
	void DrawCmdList(const DebugDrawManager &mgr);
	void DrawElem(DebugDraw3D *cmd);

public:
	virtual void DrawElem(DebugDraw3D_Line *cmd) = 0;
	virtual void DrawElem(DebugDraw3D_Cross *cmd) = 0;
	virtual void DrawElem(DebugDraw3D_Sphere *cmd) = 0;
	virtual void DrawElem(DebugDraw3D_String *cmd) = 0;
	virtual void DrawElem(DebugDraw3D_Axis *cmd) = 0;
};

class AbstractDebugDrawer2D {
public:
	AbstractDebugDrawer2D() {}
	virtual ~AbstractDebugDrawer2D() {}

public:
	virtual void Draw(const DebugDrawManager &mgr) = 0;
	void DrawCmdList(const DebugDrawManager &mgr);
	void DrawElem(DebugDraw2D *cmd);

public:
	virtual void DrawElem(DebugDraw2D_Line *cmd) = 0;
	virtual void DrawElem(DebugDraw2D_Cross *cmd) = 0;
	virtual void DrawElem(DebugDraw2D_String *cmd) = 0;
	virtual void DrawElem(DebugDraw2D_Circle *cmd) = 0;
};

class DebugDrawManager {;
public:
	typedef std::list<std::unique_ptr<DebugDraw3D>> DebugDraw3DList;
	typedef std::list<std::unique_ptr<DebugDraw2D>> DebugDraw2DList;
public:
	DebugDrawManager();
	~DebugDrawManager();
	void Update(float dt);
	int CmdCount() const { return cmd_2d_list_.size() + cmd_3d_list_.size(); }
	void Clear();

	void AddLine(const glm::vec3 &p1, const glm::vec3 &p2,
		const haruna::vec4ub &color,
		float line_width = 1.0f,
		float duration = 0.0f,
		bool depth_enable = true);

	void AddCross(const glm::vec3 &pos, 
		const haruna::vec4ub &color,
		float size,
		float duration = 0.0f,
		bool depth_enable = true);

	void AddSphere(const glm::vec3 &pos, float radius,
		const haruna::vec4ub &color,
		float duration = 0.0f,
		bool depth_enable = true);

	void AddAxis(const glm::mat4 &xf,
		float size, 
		float duration = 0.0f,
		bool depth_enable = true);

	void AddString(const glm::vec3 &pos, const std::string &msg,
		const haruna::vec4ub &color,
		float scale = 1.0f,
		float duration = 0.0f,
		bool depth_enable = true);

	void AddLine(const glm::vec2 &p1, const glm::vec2 &p2,
		const haruna::vec4ub &color,
		float line_width = 1.0f,
		float duration = 0.0f);

	void AddCross(const glm::vec2 &pos, 
		const haruna::vec4ub &color,
		float size,
		float duration = 0.0f);

	void AddString(const glm::vec2 &pos, const std::string &msg,
		const haruna::vec4ub &color,
		float scale = 1.0f,
		float duration = 0.0f);

	void AddCircle(const glm::vec2 &pos, float radius,
		const haruna::vec4ub &color,
		float duration = 0.0f);

	DebugDraw3DList::const_iterator begin_3d() const { return cmd_3d_list_.begin(); }
	DebugDraw3DList::const_iterator end_3d() const { return cmd_3d_list_.end(); }
	DebugDraw2DList::const_iterator begin_2d() const { return cmd_2d_list_.begin(); }
	DebugDraw2DList::const_iterator end_2d() const { return cmd_2d_list_.end(); }

private:
	DebugDraw3DList cmd_3d_list_;
	DebugDraw2DList cmd_2d_list_;
};


struct DebugDraw2D {
	DebugDraw2D() : type(kDebugDraw2DNone), duration(0) {}
	DebugDraw2D(Draw2DType type)  : type(type), duration(0) {}
	virtual ~DebugDraw2D() {}
	Draw2DType type;
	vec4ub color;
	float duration;
};
struct DebugDraw2D_Line : public DebugDraw2D {
	DebugDraw2D_Line() : DebugDraw2D(kDebugDraw2DLine), line_width(1.0f) {}
	virtual ~DebugDraw2D_Line() {}
	glm::vec2 p1;
	glm::vec2 p2;
	float line_width;
};
struct DebugDraw2D_Cross : public DebugDraw2D {
	DebugDraw2D_Cross() : DebugDraw2D(kDebugDraw2DCross), size(5.0f) {}
	virtual ~DebugDraw2D_Cross() {}
	glm::vec2 pos;
	float size;
};
struct DebugDraw2D_String : public DebugDraw2D {
	DebugDraw2D_String() : DebugDraw2D(kDebugDraw2DString), scale(1.0f) {}
	virtual ~DebugDraw2D_String() {}
	glm::vec2 pos;
	std::string msg;
	float scale;
};
struct DebugDraw2D_Circle: public DebugDraw2D {
	DebugDraw2D_Circle() : DebugDraw2D(kDebugDraw2DCircle), radius(0) {}
	virtual ~DebugDraw2D_Circle() {}
	float radius;
	glm::vec2 pos;  //cross, sphere, string
};


struct DebugDraw3D {
	DebugDraw3D(DebugDraw3DType type);
	virtual ~DebugDraw3D() {}

	DebugDraw3DType type;

	//shared
	haruna::vec4ub color;
	float duration;
	bool depth_enable;

	glm::mat4 projection_mat;
	glm::mat4 view_mat;
	glm::mat4 model_mat;

	glm::mat4 GetMVPMatrix() const { return projection_mat * view_mat * model_mat; }
};

struct DebugDraw3D_Line : public DebugDraw3D {
	DebugDraw3D_Line() : DebugDraw3D(kDebugDraw3DLine), line_width(1.0f) {}
	float line_width;
	glm::vec3 p1, p2;   //line
};

struct DebugDraw3D_Cross : public DebugDraw3D {
	DebugDraw3D_Cross() : DebugDraw3D(kDebugDraw3DCross), size(1.0f) {}
	float size;
	glm::vec3 pos;  //cross
};
struct DebugDraw3D_Sphere : public DebugDraw3D {
	DebugDraw3D_Sphere() : DebugDraw3D(kDebugDraw3DSphere), radius(0) {}
	float radius;
	glm::vec3 pos;  //sphere
};
struct DebugDraw3D_String : public DebugDraw3D {
	DebugDraw3D_String() : DebugDraw3D(kDebugDraw3DString), scale(1.0f) {}
	std::string msg;
	float scale;
	glm::vec3 pos;  //string
};
struct DebugDraw3D_Axis : public DebugDraw3D {
	DebugDraw3D_Axis() : DebugDraw3D(kDebugDraw3DAxis), size(1.0f) {}
	float size;
	glm::mat4 xf;   //axis
};

} //namespace haruna