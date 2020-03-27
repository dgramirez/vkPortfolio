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

vec4 GaussianBlur() {
	vec2 uv = vsUV + ubo.offsetUV;
	vec4 tc = texture(uv_sampler, uv) * ubo.gbWeight[0];
	for (int i = 1; i < 3; ++i) {
		tc += texture(uv_sampler, uv + vec2(0.0f, ubo.gbOffset[i]) * 0.01f) * ubo.gbWeight[i];
		tc += texture(uv_sampler, uv - vec2(0.0f, ubo.gbOffset[i]) * 0.01f) * ubo.gbWeight[i];
	}

	return tc;
}
vec4 Swirling() {
	return texture(uv_sampler, vsUV);
}
vec4 Pixelate() {
	vec2 curUV = vsUV + ubo.offsetUV;
	int u = int(curUV.x * ubo.pxSize);
	int v = int(curUV.y * ubo.pxSize);
	return texture(uv_sampler, vec2(float(u)/ubo.pxSize, float(v)/ubo.pxSize));
}
vec4 EdgeDetection() {
	return texture(uv_sampler, vsUV);
}
vec4 BlackAndWhite() {
	return texture(uv_sampler, vsUV);
}
vec4 FishEye() {
	return texture(uv_sampler, vsUV);
}


void main() {
	//Setup Color and UV
	vec4 color = texture(uv_sampler, vsUV);

	//Case based on active effect
	switch(ubo.activeEffect) {
		case 0:
			psColor = color;
			break;
		case 1:
			psColor = GaussianBlur();
			break;
		case 2:
			psColor = Swirling();
			break;
		case 3:
			psColor = Pixelate();
			break;
		case 4:
			psColor = EdgeDetection();
			break;
		case 5:
			psColor = BlackAndWhite();
			break;
		case 6:
			psColor = FishEye();
			break;
	}
}