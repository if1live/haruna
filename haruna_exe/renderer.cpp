// Ŭnicode please 
#include "renderer.h"

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
	prog_->Use();
	haruna::gl::ShaderLocation pos_loc = prog_->GetAttribLocation("a_position");

	float vertex_data[] = {
		0.0f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f, 
		0.5f, -0.5f, 0.0f
	};

	//draw
	glViewport(0, 0, (int)width_, (int)height_);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glVertexAttribPointer(pos_loc.handle(), 3, GL_FLOAT, GL_FALSE, 0, &vertex_data);
	glEnableVertexAttribArray(pos_loc.handle());

	glDrawArrays(GL_TRIANGLES, 0, 3);
}
