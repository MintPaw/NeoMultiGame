precision highp float;

#define TEXTURE2D texture
#define PI 3.1415926535897932384626433832795
#define DIFFUSE_INSTANCES_MAX 16
#define linearstep(edge0, edge1, x) clamp((x - (edge0)) / (edge1 - (edge0)), 0.0, 1.0)



vec4 hexToVec4(int value);
vec4 blur13(sampler2D image, vec2 uv, vec2 resolution, vec2 direction);
vec3 FxaaPixelShader(vec4 posPos, sampler2D tex, vec2 rcpFrame);
/// FUNCTIONS ^

vec4 hexToVec4(int value) {
	return vec4(
		float((value >> 16) & 0xFF) / 255.0, // r
		float((value >> 8) & 0xFF) / 255.0, // g
		float((value     ) & 0xFF) / 255.0, // b
		float((value >> 24) & 0xFF) / 255.0 // a
	);
}

float stroke(float x, float s, float w);
float stroke(float x, float s, float w) {
	float d = step(s, x+w * 0.5) - step(s, x-w * 0.5);
	return clamp(d, 0.0, 1.0);
}

float circleSDF(vec2 st);
float circleSDF(vec2 st) {
	return length(st - 0.5 * 2.0);
}
float rectSDF(vec2 st, vec2 s);
float rectSDF(vec2 st, vec2 s) {
	st = st*2.0-1.0;
	return max( abs(st.x/s.x), abs(st.y/s.y) );
}
float random(in vec2 st);
float random(in vec2 st) {
	return fract(sin(dot(st.xy, vec2(12.9898, 78.233)))* 43758.5453123);
}

float noise(in vec2 st);
float noise(in vec2 st) { // https://www.shadertoy.com/view/4dS3Wd
	vec2 i = floor(st);
	vec2 f = fract(st);

	// Four corners in 2D of a tile
	float a = random(i);
	float b = random(i + vec2(1.0, 0.0));
	float c = random(i + vec2(0.0, 1.0));
	float d = random(i + vec2(1.0, 1.0));

	vec2 u = f * f * (3.0 - 2.0 * f);

	return mix(a, b, u.x) +
		(c - a)* u.y * (1.0 - u.x) +
		(d - b) * u.x * u.y;
}

// #define FBM_OCTAVES 5
// float fbm(in vec2 st);
// float fbm(in vec2 st) {
// #if 0
// 	// Initial values
// 	float value = 0.0;
// 	float amplitude = .5;
// 	float frequency = 0.;

// 	// Loop of octaves
// 	for (int i = 0; i < FBM_OCTAVES; i++) {
// 		value += amplitude * noise(st);
// 		st *= 2.;
// 		amplitude *= .5;
// 	}
// 	return value;
// #else
// 	float v = 0.0;
// 	float a = 0.5;
// 	vec2 shift = vec2(100.0);
// 	// Rotate to reduce axial bias
// 	mat2 rot = mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.50));
// 	for (int i = 0; i < FBM_OCTAVES; ++i) {
// 		v += a * noise(st);
// 		st = rot * st * 2.0 + shift;
// 		a *= 0.5;
// 	}
// 	return v;
// #endif
// }


vec3 permute(vec3 x) { return mod(((x*34.0)+1.0)*x, 289.0); }

float snoise(vec2 v){
	const vec4 C = vec4(0.211324865405187, 0.366025403784439,
		-0.577350269189626, 0.024390243902439);
	vec2 i  = floor(v + dot(v, C.yy) );
	vec2 x0 = v -   i + dot(i, C.xx);
	vec2 i1;
	i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
	vec4 x12 = x0.xyxy + C.xxzz;
	x12.xy -= i1;
	i = mod(i, 289.0);
	vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
		+ i.x + vec3(0.0, i1.x, 1.0 ));
	vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy),
			dot(x12.zw,x12.zw)), 0.0);
	m = m*m ;
	m = m*m ;
	vec3 x = 2.0 * fract(p * C.www) - 1.0;
	vec3 h = abs(x) - 0.5;
	vec3 ox = floor(x + 0.5);
	vec3 a0 = x - ox;
	m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
	vec3 g;
	g.x  = a0.x  * x0.x  + h.x  * x0.y;
	g.yz = a0.yz * x12.xz + h.yz * x12.yw;
	return 130.0 * dot(m, g);
}

float sRGB(float x);
float sRGB(float x) {
    if (x <= 0.00031308)
        return 12.92 * x;
    else
        return 1.055*pow(x,(1.0 / 2.4) ) - 0.055;
}

vec3 FxaaPixelShader(
	vec4 posPos, // Output of FxaaVertexShader interpolated across screen.
	sampler2D tex, // Input texture.
	vec2 rcpFrame) // Constant {1.0/frameWidth, 1.0/frameHeight}.
{
	/*---------------------------------------------------------*/
#define FXAA_REDUCE_MIN   (1.0/128.0)
	//#define FXAA_REDUCE_MUL   (1.0/8.0)
	//#define u_fxaaSpanMax     8.0
	/*---------------------------------------------------------*/
	vec3 rgbNW = texture(tex, posPos.zw).xyz;
	vec3 rgbNE = textureOffset(tex, vec2(posPos.z, posPos.w), ivec2(1,0)).xyz;
	vec3 rgbSW = textureOffset(tex, posPos.zw, ivec2(0,1)).xyz;
	vec3 rgbSE = textureOffset(tex, posPos.zw, ivec2(1,1)).xyz;
	vec3 rgbM  = texture(tex, posPos.xy).xyz;
	/*---------------------------------------------------------*/
	vec3 luma = vec3(0.299, 0.587, 0.114);
	float lumaNW = dot(rgbNW, luma);
	float lumaNE = dot(rgbNE, luma);
	float lumaSW = dot(rgbSW, luma);
	float lumaSE = dot(rgbSE, luma);
	float lumaM  = dot(rgbM,  luma);
	/*---------------------------------------------------------*/
	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
	/*---------------------------------------------------------*/
	vec2 dir; 
	dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
	dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
	/*---------------------------------------------------------*/
	float fxaaReduceMul = 1.0/8.0;
	float fxaaSpanMax = 8.0;
	float dirReduce = max(
		(lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * fxaaReduceMul),
		FXAA_REDUCE_MIN);
	float rcpDirMin = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);
	dir = min(vec2(fxaaSpanMax, fxaaSpanMax), 
		max(vec2(-fxaaSpanMax, -fxaaSpanMax), 
			dir * rcpDirMin)) * rcpFrame.xy;
	/*--------------------------------------------------------*/
	vec3 rgbA = (1.0/2.0) * (
		texture(tex, posPos.xy + dir * (1.0/3.0 - 0.5)).xyz +
		texture(tex, posPos.xy + dir * (2.0/3.0 - 0.5)).xyz
	);

	vec3 rgbB = rgbA * (1.0/2.0) + (1.0/4.0) * (
		texture(tex, posPos.xy + dir * (0.0/3.0 - 0.5)).xyz +
		texture(tex, posPos.xy + dir * (3.0/3.0 - 0.5)).xyz
	);

	float lumaB = dot(rgbB, luma);
	if((lumaB < lumaMin) || (lumaB > lumaMax)) return rgbA;
	return rgbB;
}

vec3 hueShift(vec3 color, float hue) {
	const vec3 k = vec3(0.57735, 0.57735, 0.57735);
	float cosAngle = cos(hue);
	return vec3(color * cosAngle + cross(k, color) * sin(hue) + k * dot(k, color) * (1.0 - cosAngle));
}
