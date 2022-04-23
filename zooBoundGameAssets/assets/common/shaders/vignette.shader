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
	vec2 uv = vec3(flipMatrix * vec3(a_position.zw, 1.0)).xy;
	v_uv = uv;
}

##VERTEX_END

precision highp float;

in vec2 v_uv;
// uniform sampler2D u_texture;
uniform vec2 u_vignettePos;
uniform vec3 u_softnessSizeAlpha;
uniform vec4 u_color;
uniform float u_alpha;

out vec4 fragColor;

void main(void) { 
	// vec4 tex = texture(u_texture, v_uv);
	vec2 coords = vec2(0.0, -1.0) + v_uv + u_vignettePos.xy*vec2(-1.0, 1.0);
	float v = length(coords * u_softnessSizeAlpha.x);
	v = smoothstep((1.0-u_softnessSizeAlpha.y)/2.0, 0.5, v);
	// tex.a = v;

	// fragColor = u_color * v;
	// fragColor = u_color * v * u_softnessSizeAlpha.z;

	// fragColor = vec4(u_color.rgb, v * u_softnessSizeAlpha.z);
	// fragColor.rgb *= fragColor.a;

	vec4 tex = vec4(1.0);
	tex.a = v;
	fragColor = mix(tex, u_color, u_color.a);

	fragColor.a *= u_softnessSizeAlpha.z;
	fragColor.rgb *= fragColor.a;
}
