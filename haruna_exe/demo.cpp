// Ŭnicode please 
#include "demo.h"

#include <GL/glew.h>
#include <GL/glfw.h>

#include "haruna/debug_draw_manager.h"
#include "haruna/gl/debug_draw.h"
#include "haruna/gl/gl_env.h"

haruna::DebugDrawManager debug_draw_mgr;
haruna::gl::DebugDrawer2D debug_draw_2d;
haruna::gl::DebugDrawer3D debug_draw_3d;

using glm::vec2;
using glm::vec3;
using haruna::vec4ub;

Demo::Demo(float width, float height)
	: AbstractLogic(width, height)
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
	debug_draw_mgr.Update(dt);

	debug_draw_mgr.AddLine(vec2(0, 0), vec2(100, 100), vec4ub(255, 255, 0, 255), 5.0f);
	debug_draw_mgr.AddCross(vec2(100, 200), vec4ub(255, 255, 255, 255), 10.0f);
	debug_draw_mgr.AddCircle(vec2(200, 200), 20, vec4ub(255, 0, 255, 255));

	bool running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
	return running;
}
void Demo::Draw()
{
	glViewport(0, 0, (int)width(), (int)height());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	debug_draw_2d.Draw(debug_draw_mgr);

	haruna::gl::GLEnv::CheckError("End Frame");
}