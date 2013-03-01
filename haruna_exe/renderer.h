// Ŭnicode please 
#pragma once

#include <memory>

namespace haruna {;
namespace gl {
	class ShaderProgram;
}
}

class Renderer {
public:
	Renderer(float width, float height);
	~Renderer();

	bool Init();

	void Draw();

private:
	std::unique_ptr<haruna::gl::ShaderProgram> prog_;
	float width_;
	float height_;
};
