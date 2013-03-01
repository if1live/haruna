// Ŭnicode please 
#include <GL/glew.h>
#include <GL/glfw.h>

#include <iostream>
#include "renderer.h"

const float kWidth = 640;
const float kHeight = 480;

bool InitWindow(int width, int height) 
{
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
	return true;
}

int main()
{
	InitWindow(kWidth, kHeight);

	Renderer *renderer = new SimpleRedRenderer(kWidth, kHeight);
	
	bool running = true;
	double old_time = glfwGetTime();
	while(running) {
		double current_time = glfwGetTime();
		
		float dt = static_cast<float>(current_time - old_time);
		running = renderer->Update(dt);
		renderer->Draw();

		glfwSwapBuffers();
		old_time = current_time;
	}

	delete(renderer);

	glfwTerminate();
	return 0;	
}	