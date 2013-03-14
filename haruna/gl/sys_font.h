// Ŭnicode please
#pragma once

#include "haruna/vertex.h"
#include <vector>
#include <string>

namespace haruna {;
namespace gl {
	class Font;
	class Label;

	class SysFont {
	public:
		// vincent 폰트를 기반으로 텍스쳐 만들기
		// 128글자, 8*8폰트니까
		// 256글자를 저장할수있는 영역을 할당하고
		// 16 * 16개의 글자를 배열할수있도록 텍스쳐의 크기를
		// 128 * 128의 텍스쳐 영역에 글자를 적절히 배치한다
		enum {
			kTextureWidth = 128,
			kTextureHeight = 64,
			kFontSize = 8,
			kRowCount = kTextureHeight / kFontSize,
			kColCount = kTextureWidth / kFontSize,
		};

	public:
		SysFont();
		~SysFont();

		bool Init();
		bool Deinit();

		void GetCharacterCoord(unsigned char ch, int *x, int *y) const;
		int GetCharacterPixelIndex(int base_x, int base_y, int x, int y) const;

		void GetCharacterTextureQuad(unsigned char ch,
			float *left, float *right, float *top, float *bottom) const;

		GLuint tex_id() const { return tex_id_; }
	private:
		GLuint tex_id_;
	};

	class Label {
	public:
		typedef Vertex_1P1UV Vertex;
		typedef std::vector<Vertex> VertexList;
		typedef std::vector<unsigned short> IndexList;
	public:
		Label(SysFont *font);
		Label(SysFont *font, const std::string &msg);
		~Label();

		void set_text(const std::string &text);
		void set_text(const char *text) { set_text(std::string(text)); }

		const Vertex *vertex_data() const;
		const void *index_data() const;
		const VertexList &vertex_list() const { return vert_list_; }
		const IndexList &index_list() const { return index_list_; }
		int vertex_count() const { return vert_list_.size(); }
		int index_count() const { return index_list_.size(); }

	private:
		void Init(const std::string &text);

	private:
		std::string text_;
		VertexList vert_list_;
		IndexList index_list_;
		SysFont *font_;
	};

}	// namespace gl
}	// namespace haruna
