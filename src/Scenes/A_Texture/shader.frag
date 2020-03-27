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
	vec2 texSize = vec2(100, 100);
	vec2 tc = vsUV * texSize;
	tc -= ubo.swCenter;
	float dist = length(tc);
	if (dist < ubo.swRadius) {
		float p = (ubo.swRadius - dist) / ubo.swRadius;
		float theta = p * p * ubo.swAngle * 8.0f;
		float s = sin(theta);
		float c = cos(theta);
		tc = vec2(dot(tc, vec2(c, -s)), dot(tc, vec2(s, c)));
	}
	tc += ubo.swCenter;
	return texture(uv_sampler, tc / texSize);
}
vec4 Pixelate() {
	vec2 curUV = vsUV + ubo.offsetUV;
	int u = int(curUV.x * ubo.pxSize);
	int v = int(curUV.y * ubo.pxSize);
	return texture(uv_sampler, vec2(float(u)/ubo.pxSize, float(v)/ubo.pxSize));
}
vec4 EdgeDetection() {
	vec2 curUV = vsUV + ubo.offsetUV;
	vec4 c[9];
	c[0] = texture(uv_sampler, curUV + vec2(-ubo.edTexOffset.x, -ubo.edTexOffset.y));
	c[1] = texture(uv_sampler, curUV + vec2(              0.0f, -ubo.edTexOffset.y));
	c[2] = texture(uv_sampler, curUV + vec2( ubo.edTexOffset.x, -ubo.edTexOffset.y));
	
	c[3] = texture(uv_sampler, curUV + vec2(-ubo.edTexOffset.x, 0.0f));
	c[4] = texture(uv_sampler, curUV + vec2(              0.0f, 0.0f));
	c[5] = texture(uv_sampler, curUV + vec2( ubo.edTexOffset.x, 0.0f));
	
	c[6] = texture(uv_sampler, curUV + vec2(-ubo.edTexOffset.x, ubo.edTexOffset.y));
	c[7] = texture(uv_sampler, curUV + vec2(              0.0f, ubo.edTexOffset.y));
	c[8] = texture(uv_sampler, curUV + vec2( ubo.edTexOffset.x, ubo.edTexOffset.y));

	vec4 trueColor = 8.0f * c[4] - vec4( (c[0] + c[1] + c[2] + c[3] + c[5] + c[6] + c[7] + c[8]).rgb, c[4].w);
	return trueColor;
}
vec4 BlackAndWhite() {
	vec4 col = texture(uv_sampler, vsUV);
	float lum = dot(col, ubo.bwLumCoeff);
	
	if (ubo.bwGreyScaled)
		return vec4(lum, lum, lum, col.w);

	if (lum > 0.8f)
		return vec4(1, 1, 1, col.w);
	else
		return vec4(0, 0, 0, col.w); 
}
vec4 FishEye() {
	float apHalf = ubo.aperature * 0.5f * (3.1415927f / 180.0f);
	float maxFactor = sin(apHalf);

	vec2 uv = vsUV;
	vec2 xy = 2.0f * uv - 1.0f;
	float d = length(xy);
	if (d < 2.0f - maxFactor) {
		d = length(xy * maxFactor);
		float z = sqrt(1.0f - d * d);
		float r = atan(d, z) / 3.1415927f;
		float phi = atan(xy.y, xy.x);

		uv.x = r * cos(phi) + 0.5f;
		uv.y = r * sin(phi) + 0.5f;
	}

	return texture(uv_sampler, uv);
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