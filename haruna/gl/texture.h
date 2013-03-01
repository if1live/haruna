// Ŭnicode please 
#pragma once

namespace haruna {;
namespace gl {
	class Texture {
	public:
		Texture(const std::string &filename);
		~Texture();

		bool Init();
		bool Deinit();

		GLuint tex() const { return tex_; }
	private:
		GLuint tex_;
		std::string filename_;
	};
} // namespace gl
} // namespace haruna
