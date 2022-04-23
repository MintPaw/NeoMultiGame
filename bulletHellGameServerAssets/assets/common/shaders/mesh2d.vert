#if __VERSION__ >= 300
# define IN in
# define OUT out
#elif __VERSION__ >= 100
# define IN attribute
# define OUT varying
#endif

IN vec2 a_position;
IN vec2 a_uv;
IN vec4 a_tint;

OUT vec2 v_uv;
OUT vec4 v_tint;

uniform mat3 u_projection;

void main() {
	gl_Position = vec4(u_projection * vec3(a_position.xy, 1.0), 1.0);
	v_uv = a_uv;
	v_tint = a_tint;
}
