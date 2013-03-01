// Ŭnicode please 
#include "stdafx.h"

#include "sora/filesystem.h"
#include "sora/low_level_c_file.h"

int main()
{
	std::string path = sora::Filesystem::GetAppPath("shader/simple_red.fs");
	sora::ReadonlyCFile file = sora::ReadonlyCFile(path);
	file.Open();
	const char *buffer = static_cast<const char*>(file.GetBuffer());
	std::cout << buffer;

	if(!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	if(!glfwOpenWindow(640, 480, 0, 0, 0, 0, 0, 0, GLFW_WINDOW))
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	GLenum err = glewInit();
	if(GLEW_OK != err) 
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	
	bool running = true;
	while(running)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);

		glfwSwapBuffers();
	}

	glfwTerminate();
	return 0;	
}	