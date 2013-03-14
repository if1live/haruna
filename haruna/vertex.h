// Ŭnicode please 
#pragma once

#include <glm/glm.hpp>

namespace haruna {;

typedef glm::detail::tvec2<unsigned char> vec2ub;
typedef glm::detail::tvec3<unsigned char> vec3ub;
typedef glm::detail::tvec4<unsigned char> vec4ub;

class Vertex_1P {
public:
	glm::vec3 p;
};

class Vertex_1P1UV {
public:
	glm::vec3 p;
	glm::vec2 uv;
};

class Vertex_1P1N {
public:
	glm::vec3 p;
	glm::vec3 n;
};

class Vertex_1P1N1UV {
public:
	glm::vec3 p;
	glm::vec3 n;
	glm::vec2 uv;
};

class Vertex_1P1N1UV1T {
public:
	glm::vec3 p;
	glm::vec3 n;
	glm::vec2 uv;
	glm::vec3 t;	//tangent
};

}	// namespace haruna