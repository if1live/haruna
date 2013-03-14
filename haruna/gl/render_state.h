// Ŭnicode please 
#pragma once

namespace haruna {;
namespace gl {
	bool RenderState_Init(float w, float h);
	bool RenderState_Deinit();

	class RenderState {
	public:
		RenderState();
		~RenderState();

		void Set2D();
		void Set3D();

		void SetWinSize(int w, int h) { win_width_ = w; win_height_ = h; }
		int win_width() const { return win_width_; }
		int win_height() const { return win_height_; }

		static RenderState *Get();

	public:
		glm::mat4 view_mat;
		glm::mat4 model_mat;
		glm::mat4 proj_mat;

	private:
		int win_width_;
		int win_height_;
	};
}	// namespace gl
}	// namespace haruna