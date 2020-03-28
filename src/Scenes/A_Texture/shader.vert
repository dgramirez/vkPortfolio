#version 450

//The Quad Vertices
vec4 quad[6] = {
	{-0.5f, -0.5f, 0.0f, 1.0f},
	{-0.5f,  0.5f, 0.0f, 1.0f},
	{ 0.5f,  0.5f, 0.0f, 1.0f},

	{ 0.5f,  0.5f, 0.0f, 1.0f},
	{ 0.5f, -0.5f, 0.0f, 1.0f},
	{-0.5f, -0.5f, 0.0f, 1.0f}
};

//The Quad UVs
vec2 uv[6] = {
	{0, 0},
	{0, 1},
	{1, 1},

	{1, 1},
	{1, 0},
	{0, 0}
};

//UVs to be sent to Fragment Shader
layout (location = 0) out vec2 vsUV;

void main() {
	//Send UV based on Vertex ID
	vsUV = uv[gl_VertexIndex];

	//Set Position based on Vertex ID
	gl_Position = quad[gl_VertexIndex];
}