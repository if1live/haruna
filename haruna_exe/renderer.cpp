// Ŭnicode please 
#include "renderer.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <GL/glew.h>
#include <GL/glfw.h>

#include <vector>
#include <memory>

#include "sora/assert_inc.h"
#include "sora/filesystem.h"
#include "sora/low_level_c_file.h"

#include "haruna/primitive_mesh.h"
#include "haruna/gl/shader.h"
#include "haruna/gl/texture.h"
#include "haruna/gl/gl_env.h"

GLenum ToDrawMode(haruna::DrawType type)
{
	if(type == haruna::kDrawTriangles) {
		return GL_TRIANGLES;
	} else {
		SR_ASSERT(!"what is?");
		return GL_TRIANGLES;
	}
}

Renderer::Renderer(float width, float height) 
	: width_(width), height_(height)
{
}

/////////////////////
SimpleRedRenderer::SimpleRedRenderer(float width, float height)
	: Renderer(width, height),
	y_rot_(0)
{
	Init();
}
SimpleRedRenderer::~SimpleRedRenderer() 
{
	prog_->Deinit();
}

bool SimpleRedRenderer::Init() 
{
	//쉐이더 
	std::string fs_path = sora::Filesystem::GetAppPath("shader/simple_red.fs");
	std::string vs_path = sora::Filesystem::GetAppPath("shader/simple_red.vs");
	sora::ReadonlyCFile fs_file = sora::ReadonlyCFile(fs_path);
	sora::ReadonlyCFile vs_file = sora::ReadonlyCFile(vs_path);
	bool fs_open_result = fs_file.Open();
	bool vs_open_result = vs_file.Open();
	SR_ASSERT(fs_open_result == true);
	SR_ASSERT(vs_open_result == true);
	
	std::string fs_src(static_cast<const char*>(fs_file.GetBuffer()));
	std::string vs_src(static_cast<const char*>(vs_file.GetBuffer()));
	
	haruna::gl::VertexShader vs(vs_src);
	haruna::gl::FragmentShader fs(fs_src);

	prog_.reset(new haruna::gl::ShaderProgram(vs, fs));
	bool prog_result = prog_->Init();
	SR_ASSERT(prog_result == true);

	return true;
}

bool SimpleRedRenderer::Update(float dt)
{
	y_rot_ += 4.0f * dt;
	float radius = 4;
	eye_ = glm::vec3(cos(y_rot_) * radius, 0, sin(y_rot_) * radius);

	bool running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
	return running;
}

void SimpleRedRenderer::Draw()
{
	//sample mesh
	haruna::SolidCubeFactory cube_factory(1, 1, 1);
	auto data = cube_factory.CreateNormalMesh();
	
	//haruna::SolidSphereFactory sphere_factory(1, 8, 8);
	//auto data = sphere_factory.CreateNormalMesh();

	prog_->Use();
	haruna::gl::ShaderLocation pos_loc = prog_->GetAttribLocation("a_position");
	haruna::gl::ShaderLocation mvp_loc = prog_->GetUniformLocation("u_mvp");

	//projection
	float aspect = width() / height();
	glm::mat4 proj_mat = glm::perspective(60.0f, aspect, 0.1f, 100.0f);

	glm::vec3 center(0, 0, 0);
	glm::vec3 up(0, 1, 0);
	glm::mat4 view_mat = glm::lookAt(eye_, center, up);
		

	//model
	glm::mat4 model_mat = glm::mat4();

	glm::mat4 mvp = proj_mat * view_mat * model_mat;

	//draw
	glViewport(0, 0, (int)width(), (int)height());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUniformMatrix4fv(mvp_loc.handle(), 1, GL_FALSE, glm::value_ptr(mvp));
	glEnableVertexAttribArray(pos_loc.handle());

	for(auto cmd : data) {
		std::vector<haruna::Vertex_1P1N1UV> &vert_list = cmd.vertex_list;
		std::vector<unsigned short> &index_list = cmd.index_list;

		int stride = sizeof(haruna::Vertex_1P1N1UV);
		glVertexAttribPointer(pos_loc.handle(), 3, GL_FLOAT, GL_FALSE, stride, &vert_list[0].p);

		GLenum draw_mode = ToDrawMode(cmd.draw_mode);
		glDrawElements(draw_mode, index_list.size(), GL_UNSIGNED_SHORT, &index_list[0]);
	}
}

TextureRenderer::TextureRenderer(float width, float height)
	: Renderer(width, height), y_rot_(0)
{
	Init();
}

TextureRenderer::~TextureRenderer()
{
	tex_->Deinit();
	prog_->Deinit();
}

bool TextureRenderer::Init()
{
	//쉐이더 
	std::string fs_path = sora::Filesystem::GetAppPath("shader/simple_tex.fs");
	std::string vs_path = sora::Filesystem::GetAppPath("shader/simple_tex.vs");
	sora::ReadonlyCFile fs_file = sora::ReadonlyCFile(fs_path);
	sora::ReadonlyCFile vs_file = sora::ReadonlyCFile(vs_path);
	bool fs_open_result = fs_file.Open();
	bool vs_open_result = vs_file.Open();
	SR_ASSERT(fs_open_result == true);
	SR_ASSERT(vs_open_result == true);
	
	std::string fs_src(static_cast<const char*>(fs_file.GetBuffer()));
	std::string vs_src(static_cast<const char*>(vs_file.GetBuffer()));
	
	haruna::gl::VertexShader vs(vs_src);
	haruna::gl::FragmentShader fs(fs_src);

	prog_.reset(new haruna::gl::ShaderProgram(vs, fs));
	bool prog_result = prog_->Init();
	SR_ASSERT(prog_result == true);

	//create texture
	std::string tex_path = sora::Filesystem::GetAppPath("texture/sora2.png");
	tex_.reset(new haruna::gl::Texture(tex_path));
	bool tex_init_result = tex_->Init();
	SR_ASSERT(tex_init_result == true);

	return true;
}
bool TextureRenderer::Update(float dt)
{
	y_rot_ += 4.0f * dt;
	float radius = 4;
	eye_ = glm::vec3(cos(y_rot_) * radius, 0, sin(y_rot_) * radius);

	bool running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
	return running;
}
void TextureRenderer::Draw()
{
	//sample mesh
	haruna::SolidCubeFactory cube_factory(1, 1, 1);
	auto data = cube_factory.CreateNormalMesh();

	//haruna::SolidSphereFactory sphere_factory(1, 16, 16);
	//auto data = sphere_factory.CreateNormalMesh();
	
	prog_->Use();
	haruna::gl::ShaderLocation pos_loc = prog_->GetAttribLocation("a_position");
	haruna::gl::ShaderLocation texcoord_loc = prog_->GetAttribLocation("a_texcoord");
	haruna::gl::ShaderLocation mvp_loc = prog_->GetUniformLocation("u_mvp");
	haruna::gl::ShaderLocation tex_loc = prog_->GetUniformLocation("s_tex");


	//projection
	float aspect = width() / height();
	glm::mat4 proj_mat = glm::perspective(60.0f, aspect, 0.1f, 100.0f);

	glm::vec3 center(0, 0, 0);
	glm::vec3 up(0, 1, 0);
	glm::mat4 view_mat = glm::lookAt(eye_, center, up);
		

	//model
	glm::mat4 model_mat = glm::mat4();

	glm::mat4 mvp = proj_mat * view_mat * model_mat;

	//draw
	glViewport(0, 0, (int)width(), (int)height());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);

	glUniformMatrix4fv(mvp_loc.handle(), 1, GL_FALSE, glm::value_ptr(mvp));
	glEnableVertexAttribArray(pos_loc.handle());

	//binding texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_->tex());
	glUniform1i(tex_loc.handle(), 0);

	for(auto cmd : data) {
		std::vector<haruna::Vertex_1P1N1UV> &vert_list = cmd.vertex_list;
		std::vector<unsigned short> &index_list = cmd.index_list;

		int stride = sizeof(haruna::Vertex_1P1N1UV);
		glVertexAttribPointer(pos_loc.handle(), 3, GL_FLOAT, GL_FALSE, stride, &vert_list[0].p);
		glVertexAttribPointer(texcoord_loc.handle(), 2, GL_FLOAT, GL_FALSE, stride, &vert_list[0].uv);

		GLenum draw_mode = ToDrawMode(cmd.draw_mode);
		glDrawElements(draw_mode, index_list.size(), GL_UNSIGNED_SHORT, &index_list[0]);
	}	

	haruna::gl::GLEnv::CheckError("End Frame");
}
