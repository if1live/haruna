// Ŭnicode please 
#pragma once

#include "haruna/render_device.h"

namespace haruna {;
namespace gl {
	class GLRenderDevice : public RenderDevice {
	public:
		virtual void Set2D();
		virtual void Set3D();
	};
}	// namespace gl
}	// namespace haruna