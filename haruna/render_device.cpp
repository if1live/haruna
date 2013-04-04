// Ŭnicode please 
#include "stdafx.h"
#include "render_device.h"

#include "haruna/gl/gl_renderer.h"

namespace haruna {;
std::unique_ptr<RenderDevice> render_state;

bool RenderDevice_Init(RendererType type, float w, float h)
{
	if(type == kRendererGL) {
		render_state.reset(new gl::GLRenderDevice());
	}

	if(render_state.get() == nullptr) {
		return false;
	} else {
		render_state->SetWinSize(w, h);
		return true;
	}
}
bool RenderDevice_Deinit()
{
	if(render_state.get() == nullptr) {
		return false;
	}
	render_state.reset(nullptr);
	return true;
}

RenderDevice *RenderDevice::Get()
{
	return render_state.get();
}


RenderDevice::RenderDevice()
	: win_width_(0), win_height_(0)
{
}
RenderDevice::~RenderDevice()
{
}
}	// namespace haruna