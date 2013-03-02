// Ŭnicode please 
#include <GL/glew.h>
#include <GL/glfw.h>

#include <iostream>
#include "renderer.h"

#include "color_shader.h"
#include "texture_mapping.h"
#include "lighting.h"
#include "diffuse_specular_mapping.h"
#include "toon_shader.h"

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

	//AbstractLogic *logic = new ColorShader(kWidth, kHeight);
	//AbstractLogic *logic = new TextureMapping(kWidth, kHeight);
	//AbstractLogic *logic = new Lighting(kWidth, kHeight);
	//AbstractLogic *logic = new DiffuseSpecularMapping(kWidth, kHeight);
	AbstractLogic *logic = new ToonShader(kWidth, kHeight);
	
	bool running = true;
	double old_time = glfwGetTime();
	while(running) {
		double current_time = glfwGetTime();
		
		float dt = static_cast<float>(current_time - old_time);
		running = logic->Update(dt);
		logic->Draw();

		glfwSwapBuffers();
		old_time = current_time;
	}

	delete(logic);

	glfwTerminate();
	return 0;	
}	