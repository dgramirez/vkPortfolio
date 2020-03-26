#version 450

layout (location = 0) in vec2 vsUV;
layout (location = 1) in vec2 vsActive
layout (binding = 0) uniform sampler2D uv_sampler;
layout (location = 0) out vec4 psColor;

void main() {
	psColor = texture(uv_sampler, vsUV);
}