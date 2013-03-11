// Ŭnicode please 
#pragma once

#include "haruna/debug_draw_manager.h"

namespace haruna {;
namespace gl {
	class DebugDrawer3D : public haruna::AbstractDebugDrawer3D {
	public:
		DebugDrawer3D() {}
		virtual ~DebugDrawer3D() {}

		virtual void Draw(const DebugDrawManager &mgr);

	public:
		virtual void DrawElem(DebugDraw3D_Line *cmd);
		virtual void DrawElem(DebugDraw3D_Cross *cmd);
		virtual void DrawElem(DebugDraw3D_Sphere *cmd);
		virtual void DrawElem(DebugDraw3D_String *cmd);
		virtual void DrawElem(DebugDraw3D_Axis *cmd);
	};

	class DebugDrawer2D : public haruna::AbstractDebugDrawer2D {
	public:
		DebugDrawer2D() {}
		virtual ~DebugDrawer2D() {}

	public:
		virtual void Draw(const DebugDrawManager &mgr);

	public:
		virtual void DrawElem(DebugDraw2D_Line *cmd);
		virtual void DrawElem(DebugDraw2D_Cross *cmd);
		virtual void DrawElem(DebugDraw2D_String *cmd);
		virtual void DrawElem(DebugDraw2D_Sphere *cmd);
	};

}	// namespace gl
}	// namespace haruna