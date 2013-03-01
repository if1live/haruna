// Ŭnicode please 
#include <GL/glew.h>
#include <GL/glfw.h>

#include <iostream>
#include <vector>

#include "sora/filesystem.h"
#include "sora/low_level_c_file.h"

#include "haruna/primitive_mesh.h"
#include "haruna/gl/shader.h"

int main()
{
	const float width = 640;
	const float height = 480;

	if(!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	if(!glfwOpenWindow(width, height, 0, 0, 0, 0, 0, 0, GLFW_WINDOW)) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	GLenum err = glewInit();
	if(GLEW_OK != err) {
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	//쉐이더 변수 잡고 인자 넘기기
	std::string fs_path = sora::Filesystem::GetAppPath("shader/simple_red.fs");
	std::string vs_path = sora::Filesystem::GetAppPath("shader/simple_red.vs");
	sora::ReadonlyCFile fs_file = sora::ReadonlyCFile(fs_path);
	sora::ReadonlyCFile vs_file = sora::ReadonlyCFile(vs_path);
	fs_file.Open();
	vs_file.Open();
	
	std::string fs_src(static_cast<const char*>(fs_file.GetBuffer()));
	std::string vs_src(static_cast<const char*>(vs_file.GetBuffer()));
	
	haruna::gl::VertexShader vs(vs_src);
	haruna::gl::FragmentShader fs(fs_src);

	haruna::gl::ShaderProgram prog(vs, fs);
	bool init_result = prog.Init();

	//if(open_result) {
	//	const char *buffer = ;
	//	std::cout << buffer;
	//}

	haruna::SolidCubeFactory cube_factory(1, 1, 1);
	auto data = cube_factory.CreateNormalMesh();

	GLint pos_loc = glGetAttribLocation(prog.prog(), "a_position");

	float vertex_data[] = {
		0.0f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f, 
		0.5f, -0.5f, 0.0f
	};
	
	bool running = true;
	while(running) {
		//update
		running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);

		//draw
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(prog.prog());

		glVertexAttribPointer(pos_loc, 3, GL_FLOAT, GL_FALSE, 0, &vertex_data);
		glEnableVertexAttribArray(pos_loc);

		glDrawArrays(GL_TRIANGLES, 0, 3);


		glfwSwapBuffers();
	}

	glfwTerminate();
	return 0;	
}	