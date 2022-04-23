#define IN in
#define OUT out

IN vec3 a_position;
IN vec2 a_uv;
IN vec3 a_normal;
IN vec4 a_boneIndices;
IN vec4 a_boneWeights;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform mat4 u_boneTransforms[64]; // Must match BONES_PER_ANIMATION_LIMIT

OUT vec2 v_uv;
OUT vec3 v_normal;
OUT vec3 v_fragPos;

void main() {
	mat4 boneTrans = u_boneTransforms[int(a_boneIndices.x)] * a_boneWeights.x;
	boneTrans += u_boneTransforms[int(a_boneIndices.y)] * a_boneWeights.y;
	boneTrans += u_boneTransforms[int(a_boneIndices.z)] * a_boneWeights.z;
	boneTrans += u_boneTransforms[int(a_boneIndices.w)] * a_boneWeights.w;
	vec3 modelPos = (vec4(a_position, 1.0) * boneTrans).xyz;
	vec3 normalPos = a_normal * mat3(transpose(inverse(boneTrans)));

	gl_Position = u_projection * u_view * u_model * vec4(modelPos, 1.0);
	v_uv = a_uv;

	v_fragPos = vec3(u_model * vec4(modelPos, 1.0));
	// v_fragPos = modelPos;
	// v_normal = mat3(transpose(inverse(u_model))) * normalPos;
	v_normal = normalPos;
}
