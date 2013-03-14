// Ŭnicode please 
#include "stdafx.h"
#include "render_state.h"

namespace haruna {;
namespace gl {
	std::unique_ptr<RenderState> render_state;

	bool RenderState_Init(float w, float h)
	{
		render_state.reset(new RenderState());
		render_state->SetWinSize(w, h);
		return true;
	}
	bool RenderState_Deinit()
	{
		if(render_state.get() == nullptr) {
			return false;
		}
		render_state.reset(nullptr);
		return true;
	}

	RenderState *RenderState::Get()
	{
		return render_state.get();
	}


	RenderState::RenderState()
		: win_width_(0), win_height_(0)
	{
	}
	RenderState::~RenderState()
	{
	}

	void RenderState::Set2D()
	{
		// set gl env
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
	}
	void RenderState::Set3D()
	{
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}
}	// namespace gl
}	// namespace haruna