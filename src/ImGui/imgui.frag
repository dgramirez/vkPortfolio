#version 450 
layout (location = 0) in vec2 vsUV;
layout (binding = 0) uniform sampler2D uv_sampler;
layout (location = 0) out vec4 outColor;
void main() {
	outColor = texture(uv_sampler, vsUV);
}