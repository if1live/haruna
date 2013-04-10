﻿// Ŭnicode please 
#pragma once

namespace haruna {;
typedef enum {
	kRendererGL,
	kRendererDX,
} RendererType;

bool RenderDevice_Init(RendererType type, float w, float h);
bool RenderDevice_Deinit();

class RenderDevice {
public:
	RenderDevice();
	virtual ~RenderDevice();

	virtual void Set2D() = 0;
	virtual void Set3D() = 0;

	void SetWinSize(float w, float h) { win_width_ = w; win_height_ = h; }
	float win_width() const { return win_width_; }
	float win_height() const { return win_height_; }

	static RenderDevice *Get();

public:
	glm::mat4 view_mat;
	glm::mat4 model_mat;
	glm::mat4 proj_mat;

private:
	float win_width_;
	float win_height_;
};
}	// namespace haruna