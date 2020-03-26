#version 450

vec4 quad[6] = {
	{-0.5f, -0.5f, 0.0f, 1.0f},
	{-0.5f,  0.5f, 0.0f, 1.0f},
	{ 0.5f,  0.5f, 0.0f, 1.0f},

	{ 0.5f,  0.5f, 0.0f, 1.0f},
	{ 0.5f, -0.5f, 0.0f, 1.0f},
	{-0.5f, -0.5f, 0.0f, 1.0f}
};

vec2 uv[6] = {
	{0, 0},
	{0, 1},
	{1, 1},

	{1, 1},
	{1, 0},
	{0, 0}
};

layout (location = 0) out vec2 vsUV;
layout (location = 1) out float vsActive;

void main() {
	vsActive = 1;
	vsUV = uv[gl_VertexIndex];
	gl_Position = quad[gl_VertexIndex];
}