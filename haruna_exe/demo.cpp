// Ŭnicode please 
#include "exe_stdafx.h"
#include "demo.h"

#include <GL/glew.h>
#include <GL/glfw.h>

#include "sora/logger.h"
#include "haruna/debug_draw_manager.h"
#include "haruna/render_device.h"
#include "haruna/gl/debug_draw.h"
#include "haruna/gl/gl_env.h"

using haruna::RenderDevice;
using glm::vec2;
using glm::vec3;
using haruna::vec4ub;
using std::string;

using namespace OVR;
using namespace OVR::Util::Render;

haruna::DebugDrawManager debug_draw_mgr;
haruna::gl::DebugDrawer2D debug_draw_2d;
haruna::gl::DebugDrawer3D debug_draw_3d;

Demo::Demo(float width, float height)
	: AbstractLogic(width, height), 
	y_rot_(0),
	post_process_(kPostProcess_Distortion),
	stereo_config_()
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
			// This will initialize HMDInfo with information about configured IPD,
			// screen size and other variables needed for correct projection.
			// We pass HMD DisplayDeviceName into the renderer to select the
			// correct monitor in full-screen mode.
			if (hmd_device_->GetDeviceInfo(&hmd_info))
			{            
				//RenderParams.MonitorName = hmd_info.DisplayDeviceName;
				//RenderParams.DisplayId = hmd_info.DisplayId;
				stereo_config_.SetHMDInfo(hmd_info);
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

	// *** Configure Stereo settings.
	stereo_config_.SetFullViewport(Viewport(0,0, width(), height()));
	stereo_config_.SetStereoMode(Stereo_LeftRight_Multipass);

	// Configure proper Distortion Fit.
	// For 7" screen, fit to touch left side of the view, leaving a bit of invisible
	// screen on the top (saves on rendering cost).
	// For smaller screens (5.5"), fit to the top.
	if (hmd_info.HScreenSize > 0.0f) {
		if (hmd_info.HScreenSize > 0.140f) {
			// 7"
			stereo_config_.SetDistortionFitPointVP(-1.0f, 0.0f);
		} else {
			stereo_config_.SetDistortionFitPointVP(0.0f, 1.0f);
		}
	}

	//XXX 적절히 구현하기
	//pRender->SetSceneRenderScale(stereo_config_.GetDistortionScale());
	stereo_config_.Set2DAreaFov(DegreeToRad(85.0f));

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
	
	RenderDevice *render_state = RenderDevice::Get();
	/*
	float aspect = width() / height();
	render_state->proj_mat = glm::perspective(60.0f, aspect, 0.1f, 100.0f);
	*/
	float radius = 4;
	glm::vec3 eye(cos(y_rot_) * radius, 0, sin(y_rot_) * radius);
	glm::vec3 center(0, 0, 0);
	glm::vec3 up(0, 1, 0);
	render_state->view_mat = glm::lookAt(eye, center, up);
	render_state->model_mat = glm::mat4();
	debug_draw_mgr.AddSphere(vec3(2, 0, 0), 1, vec4ub(0, 255, 255, 255));
	debug_draw_mgr.AddLine(vec3(2, 0, 0), vec3(100, 10, 0), vec4ub(128, 128, 128, 255));
	debug_draw_mgr.AddString(vec3(2, 0.6, 0.8), "pos", vec4ub(255, 255, 255, 255));
	debug_draw_mgr.AddCross(vec3(2, 0, 0), vec4ub(255, 255, 255, 255), 10);
	debug_draw_mgr.AddAxis(glm::translate(vec3(2, 0, 0)), 2, 0);

	bool running = !glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED);
	return running;
}

void Demo::ApplyStereoParams3D(const OVR::Util::Render::StereoEyeParams &stereo)
{
	const Viewport &vp = stereo.VP;
	glViewport(vp.x, vp.y, vp.w, vp.h);

	//gl좌표계와 oculus vr은 반대
	RenderDevice *render_state = RenderDevice::Get();
	Matrix4f proj_mat = stereo.Projection.Transposed();
	float *m = (float*)proj_mat.M;
	render_state->proj_mat = glm::mat4(m[0], m[1], m[2], m[3], 
		m[4], m[5], m[6], m[7], 
		m[8], m[9], m[10], m[11], 
		m[12], m[13], m[14], m[15]);

    if (stereo.pDistortion) {
        SetDistortionConfig(*stereo.pDistortion, stereo.Eye);
	}
}

void Demo::ApplyStereoParams2D(const OVR::Util::Render::StereoEyeParams &stereo)
{
	const Viewport &vp = stereo.VP;
	glViewport(vp.x, vp.y, vp.w, vp.h);

	//gl좌표계와 oculus vr은 반대
	RenderDevice *render_state = RenderDevice::Get();
	Matrix4f proj_mat = stereo.OrthoProjection.Transposed();
	float *m = (float*)proj_mat.M;
	render_state->proj_mat = glm::mat4(m[0], m[1], m[2], m[3], 
		m[4], m[5], m[6], m[7], 
		m[8], m[9], m[10], m[11], 
		m[12], m[13], m[14], m[15]);

    if (stereo.pDistortion) {
        SetDistortionConfig(*stereo.pDistortion, stereo.Eye);
	}
}
void Demo::SetDistortionConfig(const DistortionConfig& config, StereoEye eye)
{
    Distortion = config;
    if (eye == StereoEye_Right) {
        Distortion.XCenterOffset = -Distortion.XCenterOffset;
	}
}

void Demo::Draw(const OVR::Util::Render::StereoEyeParams &stereo)
{
	glClear(GL_DEPTH_BUFFER_BIT);

	ApplyStereoParams3D(stereo);
	debug_draw_3d.Draw(debug_draw_mgr);

	ApplyStereoParams2D(stereo);
	debug_draw_2d.Draw(debug_draw_mgr);

	haruna::gl::GLEnv::CheckError("End Frame");
}

void Demo::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);


	switch(stereo_config_.GetStereoMode())
    {
    case Stereo_None:
        Draw(stereo_config_.GetEyeRenderParams(StereoEye_Center));
        break;

    case Stereo_LeftRight_Multipass:
		//여기에서 2번 렌더링 처리
        Draw(stereo_config_.GetEyeRenderParams(StereoEye_Left));
        Draw(stereo_config_.GetEyeRenderParams(StereoEye_Right));
        break;
    }
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