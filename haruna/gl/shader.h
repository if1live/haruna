// Ŭnicode please 
#pragma once

namespace haruna {;
namespace gl {
	class Shader {
	public:
		Shader(const std::string &src);
		~Shader();

		bool Init();
		bool Deinit();
		bool IsInit();

		virtual GLenum GetType() = 0;
		GLuint handle() const { return shader_; }
	protected:
		std::string src_;
		GLuint shader_;
	};

	class VertexShader : public Shader {
	public:
		VertexShader(const std::string &src) : Shader(src) {}
		virtual GLenum GetType() { return GL_VERTEX_SHADER; }
	};

	class FragmentShader : public Shader {
	public:
		FragmentShader(const std::string &src) : Shader(src) {}
		virtual GLenum GetType() { return GL_FRAGMENT_SHADER; }
	};

	class ShaderProgram {
	public:
		ShaderProgram(const VertexShader &vert_shader, const FragmentShader &frag_shader);
		~ShaderProgram();

		bool Init();
		bool Deinit();

		GLuint prog() const { return prog_; }

	private:
		GLuint prog_;

		VertexShader vert_shader_;
		FragmentShader frag_shader_;
	};
}
}
