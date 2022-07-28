layout (location = 0) in vec3 a_position;

uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 v_uv;

void main() {
	v_uv = a_position;
	vec4 pos = u_projection * u_view * vec4(a_position, 1.0);
	gl_Position = pos.xyww;
}

/// FRAGMENT
##VERTEX_END

precision highp float;

in vec3 v_uv;

uniform samplerCube u_cubeMap;

out vec4 fragColor;

void main(void) {
	fragColor = texture(u_cubeMap, v_uv);
}
