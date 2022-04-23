in vec3 a_position;
in vec2 a_uv;
in vec3 a_normal;
in vec3 a_tangent;
in vec4 a_boneIndices;
in vec4 a_boneWeights;

struct VertexUniforms {
	mat4 u_model;
	mat4 u_viewProjection;
	mat4 u_boneTransforms[32]; // Must match BONES_MAX
	vec4 u_lightPos;
	vec4 u_viewPos;
	int u_usesNormalMap;
};
layout (std140) uniform DiffuseVertexUniformBlock {
	VertexUniforms uniforms[DIFFUSE_INSTANCES_MAX];
};

out vec2 v_uv;
out vec3 v_normal;
out vec3 v_fragPos;
out vec3 v_lightPos;
out vec3 v_viewPos;
out float v_usesNormalMap;
flat out int v_instanceID;

void main() {
	mat4 boneTrans = uniforms[gl_InstanceID].u_boneTransforms[int(a_boneIndices.x)] * a_boneWeights.x;
	boneTrans += uniforms[gl_InstanceID].u_boneTransforms[int(a_boneIndices.y)] * a_boneWeights.y;
	boneTrans += uniforms[gl_InstanceID].u_boneTransforms[int(a_boneIndices.z)] * a_boneWeights.z;
	boneTrans += uniforms[gl_InstanceID].u_boneTransforms[int(a_boneIndices.w)] * a_boneWeights.w;
	vec3 modelPos = (vec4(a_position, 1.0) * boneTrans).xyz;
	vec3 normalPos = a_normal * mat3(transpose(inverse(boneTrans)));

	gl_Position = uniforms[gl_InstanceID].u_viewProjection * uniforms[gl_InstanceID].u_model * vec4(modelPos, 1.0);
	v_uv = a_uv;

	v_lightPos = vec3(uniforms[gl_InstanceID].u_lightPos);
	v_viewPos = vec3(uniforms[gl_InstanceID].u_viewPos);
	v_fragPos = vec3(uniforms[gl_InstanceID].u_model * vec4(modelPos, 1.0));

	v_usesNormalMap = float(uniforms[gl_InstanceID].u_usesNormalMap);
	v_instanceID = gl_InstanceID;

	if (uniforms[gl_InstanceID].u_usesNormalMap == 1) {
		mat3 normalMatrix = transpose(inverse(mat3(uniforms[gl_InstanceID].u_model)));
		vec3 T = normalize(normalMatrix * a_tangent);
		vec3 N = normalize(normalMatrix * a_normal);
		T = normalize(T - dot(T, N) * N);
		vec3 B = cross(N, T);

		mat3 TBN = transpose(mat3(T, B, N));    
		v_lightPos = TBN * v_lightPos;
		v_viewPos = TBN * v_viewPos;
		v_fragPos = TBN * v_fragPos;
	} else {
		v_normal = mat3(transpose(inverse(uniforms[gl_InstanceID].u_model))) * normalPos;
	}

}

#VERTEX_END

precision highp float;

#define TEXTURE2D texture

#define _F_3D_BLOOM (1 << 0)
#define _F_3D_NO_GEOM (1 << 1)

in vec2 v_uv;
in vec3 v_normal;
in vec3 v_fragPos;
in vec3 v_lightPos;
in vec3 v_viewPos;
in float v_usesNormalMap;
flat in int v_instanceID;

struct FragmentUniforms {
	vec4 u_ambientLight;
	vec4 u_diffuseLight;
	vec4 u_specularLight;
	vec4 u_alpha;
	int u_flags;
};
layout (std140) uniform DiffuseFragmentUniformBlock {
	FragmentUniforms uniforms[DIFFUSE_INSTANCES_MAX];
};

uniform sampler2D u_diffuseTexture;
uniform sampler2D u_normalTexture;
uniform sampler2D u_specularTexture;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 bloomColor;

void main(void) { 
	vec3 diffuseColor = TEXTURE2D(u_diffuseTexture, v_uv).rgb;
	vec3 specularColor = TEXTURE2D(u_specularTexture, v_uv).rgb;

	vec3 normal;
	if (v_usesNormalMap > 0) {
		normal = texture(u_normalTexture, v_uv).rgb;
		// transform normal vector to range [-1,1]
		normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
	} else {
		normal = normalize(v_normal);
	}

	// ambient
	vec3 ambientTex = uniforms[v_instanceID].u_ambientLight.rgb * diffuseColor;

	// diffuse
	vec3 lightDir = normalize(v_lightPos - v_fragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuseTex = uniforms[v_instanceID].u_diffuseLight.rgb * diff * diffuseColor;

	// specular
	vec3 viewDir = normalize(v_viewPos - v_fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
	float specularStrength = 0.5;
	vec3 specularTex = uniforms[v_instanceID].u_specularLight.rgb * spec * specularColor;

	vec3 resultColor = ambientTex + diffuseTex + specularTex;
	fragColor = vec4(resultColor.rgb, uniforms[v_instanceID].u_alpha.r) * (~uniforms[v_instanceID].u_flags & _F_3D_NO_GEOM);

	float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0) {
		bloomColor = vec4(fragColor.rgb, 1.0);
	} else {
		bloomColor = vec4(0.0, 0.0, 0.0, 0.0);
	}
}
