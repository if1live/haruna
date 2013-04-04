// Ŭnicode please 
#include "stdafx.h"
#include "gl_renderer.h"

namespace haruna {;
namespace gl {
	void GLRenderDevice::Set2D()
	{
		// set gl env
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_DEPTH_TEST);
	}

	void GLRenderDevice::Set3D()
	{
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}

}	// namespace gl
}	// namespace haruna