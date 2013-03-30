// Ŭnicode please 
#pragma once

#include "renderer.h"

// Post-processing type to apply to scene after rendering. PostProcess_Distortion
// applied distortion as described by DistortionConfig.
enum PostProcessType {
    kPostProcess_None,
    kPostProcess_Distortion
};

enum DisplayMode {
    kDisplay_Window     = 0,
    kDisplay_Fullscreen = 1
};
    

class Demo : public AbstractLogic, public OVR::MessageHandler {
public:
	Demo(float width, float height);
	virtual ~Demo();

	virtual bool Init();
	virtual bool Update(float dt);
	virtual void Draw();

	void Draw(const OVR::Util::Render::StereoEyeParams &stereo);
public:
	// Initializes graphics, Rift input and creates world model.
    virtual int OnStartup(const char* args);
    // Called per frame to sample SensorFucion and render the world.
    virtual void OnIdle();

    // Installed for Oculus device messages. Optional.
    virtual void OnMessage(const OVR::Message& msg);

    // Handle input events for movement.
    virtual void OnGamepad(float padLx, float padLY, float padRx, float padRy);  
    virtual void OnMouseMove(int x, int y, int modifiers);    
    virtual void OnKey(unsigned vk, bool down);

private:
	bool InitOVR();
	void ApplyStereoParams3D(const OVR::Util::Render::StereoEyeParams &stereo);
	void ApplyStereoParams2D(const OVR::Util::Render::StereoEyeParams &stereo);
	void SetDistortionConfig(const OVR::Util::Render::DistortionConfig& config, OVR::Util::Render::StereoEye eye = OVR::Util::Render::StereoEye_Left);

private:
	float y_rot_;

	OVR::Ptr<OVR::DeviceManager> manager_;
	OVR::Ptr<OVR::SensorDevice> sensor_;
	OVR::Ptr<OVR::HMDDevice> hmd_device_;
	
	//OVR::HMDInfo hmd_info_;
	OVR::SensorFusion sensor_fusion_;   

	// Stereo view parameters.
    OVR::Util::Render::StereoConfig stereo_config_;
    PostProcessType post_process_;

	OVR::Util::Render::DistortionConfig Distortion;    
};