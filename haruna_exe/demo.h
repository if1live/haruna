// Ŭnicode please 
#pragma once

#include "renderer.h"

class Demo : public AbstractLogic, public OVR::MessageHandler {
public:
	Demo(float width, float height);
	virtual ~Demo();

	virtual bool Init();
	virtual bool Update(float dt);
	virtual void Draw();

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

private:
	float y_rot_;

	OVR::Ptr<OVR::DeviceManager> manager_;
	OVR::Ptr<OVR::SensorDevice> sensor_;
	OVR::Ptr<OVR::HMDDevice> hmd_device_;
	
	OVR::HMDInfo hmd_info_;
	OVR::SensorFusion sensor_fusion_;
};