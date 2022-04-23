precision highp float;

in vec4 a_position;
uniform mat3 u_matrix;

out vec2 v_uv;

void main() {
	gl_Position = vec4(u_matrix * vec3(a_position.xy, 1.0), 1.0);
	mat3 flipMatrix = mat3(
		1,  0,  0,
		0, -1,  0,
		0,  1,  1
	);
	v_uv = vec3(flipMatrix * vec3(a_position.zw, 1.0)).xy;
}

##VERTEX_END

precision highp float;

in vec2 v_uv;
out vec4 fragColor;
uniform sampler2D u_texture;

vec2 uv_cstantos( vec2 uv, vec2 res ) {
	vec2 pixels = uv * res;

	// Updated to the final article
	vec2 alpha = 0.7 * fwidth(pixels);
	vec2 pixels_fract = fract(pixels);
	vec2 pixels_diff = clamp( .5 / alpha * pixels_fract, 0, .5 ) +
		clamp( .5 / alpha * (pixels_fract - 1) + .5, 0, .5 );
	pixels = floor(pixels) + pixels_diff;
	return pixels / res;
}

vec2 uv_nearest( vec2 uv, ivec2 texture_size ) {
	vec2 pixel = uv * texture_size;
	pixel = floor(pixel) + .5;

	return pixel / texture_size;
}

vec2 uv_iq( vec2 uv, ivec2 texture_size ) {
	vec2 pixel = uv * texture_size;

	vec2 seam = floor(pixel + 0.5);
	vec2 dudv = fwidth(pixel);
	pixel = seam + clamp( (pixel - seam) / dudv, -0.5, 0.5);

	return pixel / texture_size;
}

vec2 uv_aa_linear(vec2 uv, vec2 res) {
	uv = uv * res;
	vec2 uv_floor = floor(uv + 0.5);
	uv = uv_floor + clamp( (uv - uv_floor) / fwidth(uv) / 1, -0.5, 0.5);
	return uv / res;
}

void main(void) { 
	fragColor = texture(u_texture, uv_iq(v_uv, textureSize(u_texture, 0)));
	// fragColor = texture(u_texture, uv_nearest(v_uv, textureSize(u_texture, 0)));
}
