// Ŭnicode please 
#include "renderer.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <GL/glew.h>

#include <vector>
#include <memory>

#include "sora/assert_inc.h"
#include "sora/filesystem.h"
#include "sora/low_level_c_file.h"

#include "haruna/primitive_mesh.h"
#include "haruna/gl/shader.h"

Renderer::Renderer(float width, float height) 
	: width_(width), height_(height)
{
	Init();
}
Renderer::~Renderer() 
{
	prog_->Deinit();

}

bool Renderer::Init() 
{
	//쉐이더 변수 잡고 인자 넘기기
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

void Renderer::Draw()
{
	//sample mesh
	haruna::SolidCubeFactory cube_factory(1, 1, 1);
	auto data = cube_factory.CreateNormalMesh();

	prog_->Use();
	haruna::gl::ShaderLocation pos_loc = prog_->GetAttribLocation("a_position");
	haruna::gl::ShaderLocation mvp_loc = prog_->GetUniformLocation("u_mvp");

	//projection
	float aspect = width_ / height_;
	glm::mat4 proj_mat = glm::perspective(60.0f, aspect, 0.1f, 100.0f);

	static float y_rot = 0;
	y_rot += 0.001f;
	float radius = 4;
	
	//view
	glm::vec3 eye(cos(y_rot) * radius, 0, sin(y_rot) * radius);
	glm::vec3 center(0, 0, 0);
	glm::vec3 up(0, 1, 0);

	glm::mat4 view_mat = glm::lookAt(eye, center, up);
		

	//model
	glm::mat4 model_mat = glm::mat4();

	glm::mat4 mvp = proj_mat * view_mat * model_mat;

	//draw
	glViewport(0, 0, (int)width_, (int)height_);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUniformMatrix4fv(mvp_loc.handle(), 1, GL_FALSE, glm::value_ptr(mvp));
	glEnableVertexAttribArray(pos_loc.handle());

	for(auto cmd : data) {
		std::vector<haruna::Vertex_1P1N1UV> &vert_list = cmd.vertex_list;
		std::vector<unsigned short> &index_list = cmd.index_list;

		int stride = sizeof(haruna::Vertex_1P1N1UV);
		glVertexAttribPointer(pos_loc.handle(), 3, GL_FLOAT, GL_FALSE, stride, &vert_list[0].p);

		GLenum draw_mode = 0;
		if(cmd.draw_mode == haruna::kDrawTriangles) {
			draw_mode = GL_TRIANGLES;
		} else {
			SR_ASSERT(!"what is?");
		}

		glDrawElements(draw_mode, index_list.size(), GL_UNSIGNED_SHORT, &index_list[0]);
	}
}
