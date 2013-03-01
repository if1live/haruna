// Ŭnicode please 
#pragma once

#include <memory>
#include <glm/glm.hpp>

namespace haruna {;
namespace gl {
	class ShaderProgram;
	class Texture;
}	// namespace haruna
}	// namespace gl

class Renderer {
public:
	Renderer(float width, float height);
	virtual ~Renderer() {}

	virtual bool Init() = 0;
	virtual bool Update(float dt) = 0;
	virtual void Draw() = 0;

	float width() const { return width_; }
	float height() const { return height_; }
private:
	float width_;
	float height_;
};

class SimpleRedRenderer : public Renderer {
public:
	SimpleRedRenderer(float width, float height);
	~SimpleRedRenderer();

	virtual bool Init();
	virtual bool Update(float dt);
	virtual void Draw();

private:
	std::unique_ptr<haruna::gl::ShaderProgram> prog_;
	
	float y_rot_;
	glm::vec3 eye_;
};

class TextureRenderer : public Renderer {
public:
	TextureRenderer(float width, float height);
	~TextureRenderer();

	virtual bool Init();
	virtual bool Update(float dt);
	virtual void Draw();

private:
	std::unique_ptr<haruna::gl::ShaderProgram> prog_;
	std::unique_ptr<haruna::gl::Texture> tex_;
	
	float y_rot_;
	glm::vec3 eye_;
};