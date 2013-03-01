﻿// Ŭnicode please 
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

		std::vector<unsigned char> image;
		unsigned width, height;
		unsigned error = lodepng::decode(image, width, height, filename_);
		
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
		/*
		GLubyte pixels[] = {
			100, 0, 0,
			0, 100, 0,
			0, 0, 100,
			0, 0, 100,
		};
		*/
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		
		glGenTextures(1, &tex_);
		glBindTexture(GL_TEXTURE_2D, tex_);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, u2, v2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		

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