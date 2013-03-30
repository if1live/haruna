// Ŭnicode please 
#include "exe_stdafx.h"

#include <GL/glew.h>
#include <GL/glfw.h>

#include <iostream>
#include <memory>
#include "renderer.h"

#include "demo.h"

#include "haruna/debug_draw_manager.h"
#include "haruna/gl/render_state.h"
#include "sora/io/filesystem.h"

using std::string;
using namespace OVR;

//const float kWidth = 640;
//const float kHeight = 480;
const float kWidth = 1280;
const float kHeight = 800;

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
//int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR inArgs, int)
{
	int exitCode = 0;

	InitWindow(static_cast<int>(kWidth), static_cast<int>(kHeight));
	std::unique_ptr<AbstractLogic> logic(new Demo(kWidth, kHeight));

	// engine initialize begin
	{
		using haruna::DebugDrawManager_Init;
		using haruna::gl::RenderState_Init;
		using sora::io::FS_Init;

		bool retval = false;
		retval = FS_Init();
		SR_ASSERT(retval);
		retval = DebugDrawManager_Init();
		SR_ASSERT(retval);
		retval = RenderState_Init(kWidth, kHeight);
		SR_ASSERT(retval);

		// Initializes LibOVR. This LogMask_All enables maximum logging.
		// Custom allocator can also be specified here.
		OVR::System::Init(OVR::Log::ConfigureDefaultLog(OVR::LogMask_All));
	}
	// engine initialize end
	
	bool running = true;
	bool init_result = logic->Init();
	if(init_result == false) {
		//getchar();
		running = false;
	}
	double old_time = glfwGetTime();
	while(running) {
		double current_time = glfwGetTime();

		float dt = static_cast<float>(current_time - old_time);
		running = logic->Update(dt);
		logic->Draw();

		glfwSwapBuffers();
		old_time = current_time;
	}
	logic.reset(NULL);

	// engine deinitialize begin
	{
		using haruna::DebugDrawManager_Deinit;
		using haruna::gl::RenderState_Deinit;
		using sora::io::FS_Deinit;

		bool retval = false;
		retval = RenderState_Deinit();
		SR_ASSERT(retval);
		retval = DebugDrawManager_Deinit();
		SR_ASSERT(retval);
		retval = FS_Deinit();
		SR_ASSERT(retval);
	}
	// engine deinitialize end
	glfwTerminate();
	
    // No OVR functions involving memory are allowed after this.
    OVR::System::Destroy();
    OVR_DEBUG_STATEMENT(_CrtDumpMemoryLeaks());
    return exitCode;

}	
