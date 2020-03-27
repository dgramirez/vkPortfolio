#version 450
layout (location = 0) in vec2 vsUV;
layout (location = 0) out vec4 psColor;

layout (binding = 0) uniform psUBO {
	//Align #1: Basics (and Black & White)
	vec2 offsetUV;
	int activeEffect;
	bool bwGreyScaled;

	//Align #2: Gauss Offset + Pixel Size
	vec3 gbOffset;
	float pxSize;

	//Align #3: Gauss Weight + Fisheye Aperature
	vec3 gbWeight;
	float aperature;

	//Align #4: Swirling
	vec2 swCenter;
	float swRadius;
	float swAngle;

	//Align #5: Black & White Luminence Coefficent
	vec4 bwLumCoeff;

	//Align #6: Edge Detection 
	vec4 edLumCoeff;
	vec4 edTexOffset;

} ubo;
layout (binding = 1) uniform sampler2D uv_sampler;

vec4 GaussianBlur(vec4 color) {
	vec2 uv = vsUV;
	vec3 tc = texture(uv_sampler, uv).rgb * ubo.gbWeight[0];
	for (int i = 1; i < 3; ++i) {
		tc += texture(uv_sampler, uv + vec2(0.0f, ubo.gbOffset[i]) * 0.01f).rgb * ubo.gbWeight[i];
		tc += texture(uv_sampler, uv - vec2(0.0f, ubo.gbOffset[i]) * 0.01f).rgb * ubo.gbWeight[i];
	}

	return vec4(tc, color.w);
}
vec4 Swirling(vec4 color) {
	return color;
}
vec4 Pixelate(vec4 color) {
	int u = int(vsUV.x * ubo.pxSize);
	int v = int(vsUV.y * ubo.pxSize);
	return texture(uv_sampler, vec2(float(u)/ubo.pxSize, float(v)/ubo.pxSize));
}
vec4 EdgeDetection(vec4 color) {
	return color;
}
vec4 BlackAndWhite(vec4 color) {
	return color;
}
vec4 FishEye(vec4 color) {
	return color;
}


void main() {
	//Setup Color and UV
	vec2 curUV = vsUV + ubo.offsetUV;
	vec4 color = texture(uv_sampler, curUV);

	//Case based on active effect
	switch(ubo.activeEffect) {
		case 0:
			psColor = color;
			break;
		case 1:
			psColor = GaussianBlur(color);
			break;
		case 2:
			psColor = Swirling(color);
			break;
		case 3:
			psColor = Pixelate(color);
			break;
		case 4:
			psColor = EdgeDetection(color);
			break;
		case 5:
			psColor = BlackAndWhite(color);
			break;
		case 6:
			psColor = FishEye(color);
			break;
	}
}