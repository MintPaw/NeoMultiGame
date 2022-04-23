in vec4 a_position;
uniform mat3 u_matrix;

out vec2 v_texCoord;

void main() {
	gl_Position = vec4(u_matrix * vec3(a_position.xy, 1.0), 1.0);
	mat3 flipMatrix = mat3(
		1,  0,  0,
		0, -1,  0,
		0,  1,  1
	);
	v_texCoord = vec3(flipMatrix * vec3(a_position.zw, 1.0)).xy;
}

##VERTEX_END

precision highp float;

in vec2 v_texCoord;
out vec4 fragColor;
uniform sampler2D u_texture;
uniform float u_alpha;
uniform vec2 u_gridResolution;
uniform float u_gridAlpha;

vec4 filteredTexture(sampler2D aSampler, vec2 uv) {
	vec2 res = textureSize(aSampler, 0);
	uv = uv*res + 0.5;

	vec2 fl = floor(uv);
	vec2 fr = fract(uv);
	vec2 aa = fwidth(uv)*0.75;
	fr = smoothstep( vec2(0.5)-aa, vec2(0.5)+aa, fr);

	uv = (fl+fr-0.5) / res;

	vec4 fragment = texture(aSampler, uv);

	/// Pixel filter the edges
	vec2 uvPixel = fwidth(uv);
	vec2 border = linearstep(vec2(0.0), vec2(uvPixel), uv) * linearstep(vec2(0.0), vec2(uvPixel), vec2(1.0) - uv);
	fragment.a *= border.x * border.y;

	return fragment;
}

vec4 scanLineIntensity(float uv, float resolution, float opacity) {
	float intensity = sin(uv * resolution * PI * 2.0);
	intensity = ((0.5 * intensity) + 0.5) * 0.9 + 0.1;
	return vec4(vec3(pow(intensity, opacity)), 1.0);
}

void main(void) { 
	// fragColor = texture(u_texture, v_texCoord);
	fragColor = filteredTexture(u_texture, v_texCoord);
	fragColor *= scanLineIntensity(v_texCoord.x, u_gridResolution.y, 1.0 - u_gridAlpha);
	fragColor *= scanLineIntensity(v_texCoord.y, u_gridResolution.x, 1.0 - u_gridAlpha);

	fragColor *= u_alpha;
}
