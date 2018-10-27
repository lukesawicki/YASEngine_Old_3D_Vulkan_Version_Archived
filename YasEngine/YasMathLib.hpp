#ifndef YASMATHLIB_HPP
#define YASMATHLIB_HPP

namespace YasMathLib
{

	struct vec2
	{
		float x;
		float y;
	};

	struct vec3
	{
		float x;
		float y;
		float z;
	};
	
	struct vec4
	{
		float x;
		float y;
		float z;
		float w;
	};

	struct mat4x4
	{
		float x0;
		float y0;
		float z0;
		float w0;

		float x1;
		float y1;
		float z1;
		float w1;

		float x2;
		float y2;
		float z2;
		float w2;

		float x3;
		float y3;
		float z3;
		float w3;
	};

}

struct UniformBufferObject
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

#endif

