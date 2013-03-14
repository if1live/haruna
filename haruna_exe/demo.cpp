// Ŭnicode please 
#include "exe_stdafx.h"
#include "demo.h"

#include <GL/glew.h>
#include <GL/glfw.h>

#include "haruna/debug_draw_manager.h"
#include "haruna/gl/debug_draw.h"
#include "haruna/gl/gl_env.h"
#include "haruna/gl/render_state.h"

using haruna::gl::RenderState;
using glm::vec2;
using glm::vec3;
using haruna::vec4ub;

haruna::DebugDrawManager debug_draw_mgr;
haruna::gl::DebugDrawer2D debug_draw_2d;
haruna::gl::DebugDrawer3D debug_draw_3d;

Demo::Demo(float width, float height)
	: AbstractLogic(width, height), y_rot_(0)
{
}

Demo::~Demo()
{
}

bool Demo::Init()
{
	return true;
}
bool Demo::Update(float dt)
{
	y_rot_ += dt;

	debug_draw_mgr.Update(dt);

	//debug 2d 등록
	debug_draw_mgr.AddLine(vec2(0, 0), vec2(100, 100), vec4ub(255, 255, 0, 255), 5.0f);
	debug_draw_mgr.AddCross(vec2(100, 200), vec4ub(255, 255, 255, 255), 10.0f);
	debug_draw_mgr.AddCircle(vec2(200, 200), 20, vec4ub(255, 0, 255, 255));
	debug_draw_mgr.AddString(vec2(20, 20), "this is some msg", vec4ub(255, 0, 0, 255), 1.5f);

	//3d 환경 세팅
	RenderState *render_state = RenderState::Get();
	float aspect = width() / height();
	render_state->proj_mat = glm::perspective(60.0f, aspect, 0.1f, 100.0f);
	float radius = 4;
	glm::vec3 eye(cos(y_rot_) * radius, 0, sin(y_rot_) * radius);
	glm::vec3 center(0, 0, 0);
	glm::vec3 up(0, 1, 0);
	render_state->view_mat = glm::lookAt(eye, center, up);
	render_state->model_mat = glm::mat4();
	debug_draw_mgr.AddSphere(vec3(0, 0, 0), 1, vec4ub(0, 255, 255, 255));
	debug_draw_mgr.AddLine(vec3(0, 0, 0), vec3(100, 10, 0), vec4ub(128, 128, 128, 255));
	debug_draw_mgr.AddString(vec3(0, 0.6, 0.8), "pos", vec4ub(255, 255, 255, 255));
	debug_draw_mgr.AddCross(vec3(0, 0, 0), vec4ub(255, 255, 255, 255), 10);
	debug_draw_mgr.AddAxis(glm::mat4(), 2, 0);

	bool running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
	return running;
}
void Demo::Draw()
{
	glViewport(0, 0, (int)width(), (int)height());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	debug_draw_3d.Draw(debug_draw_mgr);
	debug_draw_2d.Draw(debug_draw_mgr);

	haruna::gl::GLEnv::CheckError("End Frame");
}