// Ŭnicode please
#include "stdafx.h"
#include "debug_draw_manager.h"

#if SR_USE_GL
#include "gl/debug_draw.h"
#include "gl/render_state.h"
#endif

using namespace std;
using namespace glm;

namespace haruna {;

bool DebugDrawManager_Init()
{
	bool retval = false;
#if SR_USE_GL
	retval = gl::GLDebugDraw_Init();
	if(!retval) {
		return false;
	}
#endif

	return retval;
}
bool DebugDrawManager_Deinit()
{
	bool retval = false;
#if SR_USE_GL
	retval = gl::GLDebugDraw_Deinit();
	if(!retval) {
		return false;
	}
#endif
	return retval;
}

//디버깅 렌더링에서 쓰이는 쉐이더는 크게 2가지이다
//쉐이더 고정 단색 + 좌표정보만 잇는 버텍스를 처리할수 잇는 쉐이더. 이것은 선이나 뭐 그런거에 쓴다
//텍스쳐 + 쉐이더 고정 단색 + 좌표정보 잇는 버텍스. 문자출력에 사용한다

DebugDraw3D::DebugDraw3D(DebugDraw3DType type)
	: type(type),
	duration(0),
	depth_enable(true) 
{
	gl::RenderState *render_state = gl::RenderState::Get();
	proj_mat = render_state->proj_mat;
	view_mat = render_state->view_mat;
	model_mat = render_state->model_mat;
}

void DebugDrawManager::AddLine(const glm::vec3 &p1, const glm::vec3 &p2,
							   const haruna::vec4ub &color,
							   float line_width,
							   float duration,
							   bool depth_enable) 
{
	std::unique_ptr<DebugDraw3D_Line> cmd(new DebugDraw3D_Line());
	cmd->p1 = p1;
	cmd->p2 = p2;
	cmd->color = color;
	cmd->line_width = line_width;
	cmd->duration = duration;
	cmd->depth_enable = depth_enable;
	cmd_3d_list_.push_back(std::move(cmd));
}

void DebugDrawManager::AddCross(const glm::vec3 &pos, 
								const haruna::vec4ub &color,
								float size,
								float duration,
								bool depth_enable) 
{
	std::unique_ptr<DebugDraw3D_Cross> cmd(new DebugDraw3D_Cross());
	cmd->pos = pos;
	cmd->color = color;
	cmd->size = size;
	cmd->duration = duration;
	cmd->depth_enable = depth_enable;
	cmd_3d_list_.push_back(std::move(cmd));
}

void DebugDrawManager::AddSphere(const glm::vec3 &pos, float radius,
								 const haruna::vec4ub &color,
								 float duration,
								 bool depth_enable) 
{
	std::unique_ptr<DebugDraw3D_Sphere> cmd(new DebugDraw3D_Sphere());
	cmd->pos = pos;
	cmd->radius = radius;
	cmd->color = color;
	cmd->duration = duration;
	cmd->depth_enable = depth_enable;
	cmd_3d_list_.push_back(std::move(cmd));
}

void DebugDrawManager::AddAxis(const glm::mat4 &xf,
							   float size, 
							   float duration,
							   bool depth_enable) 
{
	std::unique_ptr<DebugDraw3D_Axis> cmd(new DebugDraw3D_Axis());
	cmd->xf = xf;
	cmd->size = size;
	cmd->duration = duration;
	cmd->depth_enable = depth_enable;
	cmd_3d_list_.push_back(std::move(cmd));
}

void DebugDrawManager::AddString(const glm::vec3 &pos, const std::string &msg,
								 const haruna::vec4ub &color,
								 float scale,
								 float duration,
								 bool depth_enable) 
{
	std::unique_ptr<DebugDraw3D_String> cmd(new DebugDraw3D_String());
	cmd->pos = pos;
	cmd->msg = msg;
	cmd->color = color;
	cmd->scale = scale;
	cmd->duration = duration;
	cmd->depth_enable = depth_enable;
	cmd_3d_list_.push_back(std::move(cmd));
}

void DebugDrawManager::AddLine(const glm::vec2 &p1, const glm::vec2 &p2,
							   const haruna::vec4ub &color,
							   float line_width,
							   float duration) 
{
	std::unique_ptr<DebugDraw2D_Line> cmd(new DebugDraw2D_Line());
	cmd->p1 = p1;
	cmd->p2 = p2;
	cmd->color = color;
	cmd->line_width = line_width;
	cmd->duration = duration;
	cmd_2d_list_.push_back(std::move(cmd));
}

void DebugDrawManager::AddCross(const glm::vec2 &pos, 
								const haruna::vec4ub &color,
								float size,
								float duration) 
{
	std::unique_ptr<DebugDraw2D_Cross> cmd(new DebugDraw2D_Cross());
	cmd->pos = pos;
	cmd->color = color;
	cmd->size = size;
	cmd->duration = duration;
	cmd_2d_list_.push_back(std::move(cmd));
}

void DebugDrawManager::AddString(const glm::vec2 &pos, const std::string &msg,
								 const haruna::vec4ub &color,
								 float scale,
								 float duration) 
{
	std::unique_ptr<DebugDraw2D_String> cmd(new DebugDraw2D_String());
	cmd->pos = pos;
	cmd->msg = msg;
	cmd->color = color;
	cmd->scale = scale;
	cmd->duration = duration;
	cmd_2d_list_.push_back(std::move(cmd));
}

void DebugDrawManager::AddCircle(const glm::vec2 &pos, float radius,
								 const haruna::vec4ub &color,
								 float duration) 
{
	std::unique_ptr<DebugDraw2D_Circle> cmd(new DebugDraw2D_Circle());
	cmd->pos = pos;
	cmd->radius = radius;
	cmd->color = color;
	cmd->duration = duration;
	cmd_2d_list_.push_back(std::move(cmd));
}


DebugDrawManager::DebugDrawManager() 
{

}
DebugDrawManager::~DebugDrawManager() 
{
	Clear();
}

void DebugDrawManager::Clear() 
{
	cmd_3d_list_.clear();
	cmd_2d_list_.clear();
}

void DebugDrawManager::Update(float dt) 
{
	struct RemoveFindFunctor {
		bool operator()(std::unique_ptr<DebugDraw3D> &cmd) {
			return (cmd->duration < 0);
		}
		bool operator()(std::unique_ptr<DebugDraw2D> &cmd) {
			return (cmd->duration < 0);
		}
	};
	RemoveFindFunctor functor;

	{
		auto it = cmd_3d_list_.begin();
		auto endit = cmd_3d_list_.end();
		for( ; it != endit ; ++it) {
			std::unique_ptr<DebugDraw3D> &cmd = *it;
			cmd->duration -= dt;
		}
		cmd_3d_list_.remove_if(functor);
	}
	{
		auto it = cmd_2d_list_.begin();
		auto endit = cmd_2d_list_.end();
		for( ; it != endit ; ++it) {
			std::unique_ptr<DebugDraw2D> &cmd = *it;
			cmd->duration -= dt;
		}
		cmd_2d_list_.remove_if(functor);
	}
}

void AbstractDebugDrawer2D::DrawElem(DebugDraw2D *cmd) 
{
	switch(cmd->type) {
	case kDebugDraw2DLine:
		DrawElem(static_cast<DebugDraw2D_Line*>(cmd));
		break;
	case kDebugDraw2DCross:
		DrawElem(static_cast<DebugDraw2D_Cross*>(cmd));
		break;
	case kDebugDraw2DString:
		DrawElem(static_cast<DebugDraw2D_String*>(cmd));
		break;
	case kDebugDraw2DCircle:
		DrawElem(static_cast<DebugDraw2D_Circle*>(cmd));
		break;
	default:
		break;
	}
}

void AbstractDebugDrawer3D::DrawElem(DebugDraw3D *cmd) 
{
	switch(cmd->type) {
	case kDebugDraw3DLine:
		DrawElem(static_cast<DebugDraw3D_Line*>(cmd));
		break;
	case kDebugDraw3DCross:
		DrawElem(static_cast<DebugDraw3D_Cross*>(cmd));
		break;
	case kDebugDraw3DSphere:
		DrawElem(static_cast<DebugDraw3D_Sphere*>(cmd));
		break;
	case kDebugDraw3DString:
		DrawElem(static_cast<DebugDraw3D_String*>(cmd));
		break;
	case kDebugDraw3DAxis:
		DrawElem(static_cast<DebugDraw3D_Axis*>(cmd));
		break;
	default:
		break;
	}
}

} //namespace sora