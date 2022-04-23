#if __VERSION__ >= 300
# define IN in
# define OUT out
#elif __VERSION__ >= 100
# define IN attribute
# define OUT varying
#endif

IN vec3 a_position;
IN vec2 a_uv;
IN vec3 a_normal;

OUT vec2 v_uv;
OUT vec3 v_normal;
OUT vec3 v_fragPos;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main() {
	gl_Position = u_projection * u_view * u_model * vec4(a_position.xyz, 1.0);
	v_uv = a_uv;

	v_fragPos = vec3(u_model * vec4(a_position.xyz, 1.0));
#if __VERSION__ >= 300
	v_normal = mat3(transpose(inverse(u_model))) * a_normal;
#else
	v_normal = mat3(u_model) * a_normal;
#endif
}
