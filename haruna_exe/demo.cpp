// Ŭnicode please 
#include "exe_stdafx.h"
#include "demo.h"

#include <GL/glew.h>
#include <GL/glfw.h>

#include "sora/logger.h"
#include "haruna/debug_draw_manager.h"
#include "haruna/gl/debug_draw.h"
#include "haruna/gl/gl_env.h"
#include "haruna/gl/render_state.h"

using haruna::gl::RenderState;
using glm::vec2;
using glm::vec3;
using haruna::vec4ub;
using std::string;
using namespace OVR;

haruna::DebugDrawManager debug_draw_mgr;
haruna::gl::DebugDrawer2D debug_draw_2d;
haruna::gl::DebugDrawer3D debug_draw_3d;

Demo::Demo(float width, float height)
	: AbstractLogic(width, height), y_rot_(0)
{
}

Demo::~Demo()
{
	RemoveHandlerFromDevices();
}

bool Demo::InitOVR()
{
	int         detectionResult = IDCONTINUE;
    const char* detectionMessage;

	
	manager_ = *DeviceManager::Create();
	manager_->SetMessageHandler(this);
	
	HMDInfo hmd_info;

	do {
		// config ovr	
		hmd_device_ = manager_->EnumerateDevices<OVR::HMDDevice>().CreateDevice();

		if(hmd_device_) {
			sensor_ = hmd_device_->GetSensor();

			if(hmd_device_->GetDeviceInfo(&hmd_info)) {
				/*
				monitor_name = hmd_info.DisplayDeviceName;
				eye_distance = hmd_info.InterpupillaryDistance;
				for(int i = 0 ; i < 4 ; ++i) {
					distortion_k[i] = hmd_info.DistortionK[i];
				}
				*/
			}
		} else {            
			// If we didn't detect an HMD, try to create the sensor directly.
			// This is useful for debugging sensor interaction; it is not needed in
			// a shipping app.
			sensor_ = *manager_->EnumerateDevices<OVR::SensorDevice>().CreateDevice();
		}
		

		// If there was a problem detecting the Rift, display appropriate message.
        detectionResult  = IDCONTINUE;        

		if (!hmd_device_ && !sensor_) {
            detectionMessage = "Oculus Rift not detected.";
		} else if (!hmd_device_) {
            detectionMessage = "Oculus Sensor detected; HMD Display not detected.";
		} else if (!sensor_) {
            detectionMessage = "Oculus HMD Display detected; Sensor not detected.";
		} else if (hmd_info.DisplayDeviceName[0] == '\0') {
            detectionMessage = "Oculus Sensor detected; HMD display EDID not detected.";
		} else {
            detectionMessage = 0;
		}

        if (detectionMessage) {
            string messageText(detectionMessage);
            messageText += "\n\n"
                           "Press 'Try Again' to run retry detection.\n"
                           "Press 'Continue' to run full-screen anyway.";

            detectionResult = ::MessageBoxA(0, messageText.data(), "Oculus Rift Detection",
                                            MB_CANCELTRYCONTINUE|MB_ICONWARNING);

            if (detectionResult == IDCANCEL) {
                return false;
			}
        }

	} while (detectionResult != IDCONTINUE);

	if(sensor_) {
		sensor_fusion_.AttachToSensor(sensor_);
		sensor_fusion_.SetDelegateMessageHandler(this);
	}

	return true;
}

bool Demo::Init()
{
	bool ovr_result = InitOVR();
	if(ovr_result == false) {
		return false;
	}
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

// Initializes graphics, Rift input and creates world model.
int Demo::OnStartup(const char* args)
{
	LOGE("on startup");
	return 0;
}
// Called per frame to sample SensorFucion and render the world.
void Demo::OnIdle()
{
	LOGE("on idle");
}

// Installed for Oculus device messages. Optional.
void Demo::OnMessage(const OVR::Message& msg)
{
	if (msg.Type == OVR::Message_DeviceAdded && msg.pDevice == manager_) {
		OVR::LogText("DeviceManager reported device added.\n");
	} else if (msg.Type == OVR::Message_DeviceRemoved && msg.pDevice == manager_) {
		OVR::LogText("DeviceManager reported device removed.\n");
	} else if (msg.Type == OVR::Message_DeviceAdded && msg.pDevice == sensor_) {
		OVR::LogText("Sensor reported device added.\n");
	} else if (msg.Type == OVR::Message_DeviceRemoved && msg.pDevice == sensor_) {
		OVR::LogText("Sensor reported device removed.\n");
	}
}

// Handle input events for movement.
void Demo::OnGamepad(float padLx, float padLY, float padRx, float padRy)
{
	LOGE("on gamepad");
}
void Demo::OnMouseMove(int x, int y, int modifiers)
{
	LOGE("on mouse move");
}

void Demo::OnKey(unsigned vk, bool down)
{
	LOGE("on key");
}