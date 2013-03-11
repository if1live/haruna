// Ŭnicode please
#include "test_stdafx.h"

#include "haruna/gl/buffer_object.h"

using namespace std;

struct SampleVertex {
	glm::vec2 a;
	glm::vec2 b;
};

TEST(VertexBufferObject, vbo) 
{
	std::vector<SampleVertex> vert_list;
	SampleVertex v1;
	v1.a = glm::vec2(0, 0);	v1.b = glm::vec2(0, 0);

	SampleVertex v2;
	v2.a = glm::vec2(1, 0);	v2.b = glm::vec2(0, 1);

	SampleVertex v3;
	v3.a = glm::vec2(1, 1);	v3.b = glm::vec2(1, 0);

	vert_list.push_back(v1);
	vert_list.push_back(v2);
	vert_list.push_back(v3);

	haruna::gl::VertexBufferObjectT<SampleVertex> vbo;
	EXPECT_EQ(false, vbo.IsLoaded());

	vbo.Init(vert_list);
	EXPECT_EQ(true, vbo.IsLoaded());
	EXPECT_EQ(sizeof(SampleVertex), vbo.ElemSize());
	EXPECT_EQ(sizeof(SampleVertex), haruna::gl::VertexBufferObjectT<SampleVertex>::elem_size);
	EXPECT_EQ(3, vbo.Count());

	vbo.Deinit();
	EXPECT_EQ(false, vbo.IsLoaded());
}

TEST(IndexBufferObject, ibo) 
{
	std::vector<unsigned short> index_list;
	index_list.push_back(1);
	index_list.push_back(4);
	index_list.push_back(5);

	haruna::gl::IndexBufferObject ibo;
	EXPECT_EQ(false, ibo.IsLoaded());
	
	ibo.Init(index_list);
	EXPECT_EQ(true, ibo.IsLoaded());
	EXPECT_EQ(3, ibo.Count());
	EXPECT_EQ(sizeof(unsigned short), ibo.ElemSize());

	ibo.Deinit();
	EXPECT_EQ(false, ibo.IsLoaded());
}