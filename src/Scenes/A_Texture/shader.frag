#version 450
layout (location = 0) in vec2 vsUV;
layout (location = 0) out vec4 psColor;

layout (binding = 0) uniform psUBO {
	vec2 offsetUV;
	int activeEffect;

	//Gaussian Blur
	vec3 gbOffset;
	vec3 gbWeight;

	//Swirling
	float swRadius;
	float swAngle;
	vec2 swCenter;

	//Pixelate
	float pxSize;

	//Edge Detection
	vec4 edLumCoeff;
	vec2 edTexOffset;

	//Black and White
	bool bwGreyScaled;
	vec4 bwLumCoeff;

	//Fish-Eye
	float aperature;
} ubo;
layout (binding = 1) uniform sampler2D uv_sampler;

void main() {
	vec2 curUV = vsUV + ubo.offsetUV;
	switch(ubo.activeEffect) {
		case 0:
			psColor = texture(uv_sampler, curUV);
			break;
		case 1:
			psColor = texture(uv_sampler, curUV);
			break;
		case 2:
			psColor = texture(uv_sampler, curUV);
			break;
		case 3:
			psColor = texture(uv_sampler, curUV);
			break;
		case 4:
			psColor = texture(uv_sampler, curUV);
			break;
		case 5:
			psColor = texture(uv_sampler, curUV);
			break;
		case 6:
			psColor = texture(uv_sampler, curUV);
			break;
	}
}