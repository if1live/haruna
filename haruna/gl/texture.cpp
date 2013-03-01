// Ŭnicode please 
#include "stdafx.h"
#include "texture.h"
#include "haruna/lodepng.h"

#include "sora/filesystem.h"

namespace haruna {;
namespace gl {

	Texture::Texture(const std::string &filename)
		: filename_(filename), tex_(0)
	{
	}
	Texture::~Texture()
	{
	}

	bool Texture::Init()
	{
		/* reference
		http://lodev.org/lodepng/example_opengl.cpp
		*/
		if(tex_ != 0) {
			return false;
		}

		std::string filename = sora::Filesystem::GetAppPath(filename_.data());
		std::vector<unsigned char> image;
		unsigned width, height;
		unsigned error = lodepng::decode(image, width, height, filename);
		if(error != 0) {
			std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
			return false;
		}

		// Texture size must be power of two for the primitive OpenGL version this is written for. Find next power of two.
		size_t u2 = 1; while(u2 < width) u2 *= 2;
		size_t v2 = 1; while(v2 < height) v2 *= 2;

		// Make power of two version of the image.
		std::vector<unsigned char> image2(u2 * v2 * 4);
		for(size_t y = 0; y < height; y++) {
			for(size_t x = 0; x < width; x++) {
				for(size_t c = 0; c < 4; c++) {
					image2[4 * u2 * y + 4 * x + c] = image[4 * width * y + 4 * x + c];
				}
			}
		}

		glGenTextures(1, &tex_);
		glEnable(GL_TEXTURE_2D);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //GL_NEAREST = no smoothing
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, 4, u2, v2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image2[0]);

		return true;
	}
	bool Texture::Deinit()
	{
		if(tex_ == 0) {
			return false;
		}

		glDeleteTextures(1, &tex_);
		tex_ = 0;
		return true;
	}
} // namespace gl
} // namespace haruna