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

void main(void) { 
	fragColor = texture(u_texture, v_texCoord);
	fragColor *= u_alpha;

	// fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
