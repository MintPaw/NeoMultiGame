#pragma pack(push, 4)
struct MeshVertex {
	Vec3 position;
	Vec2 uv;
	Vec3 normal;
	Vec3 tangent;
	char boneIndices[4];
	float boneWeights[4];
};
#pragma pack(pop)

struct MaterialValue {
	Texture *texture;
	Vec4 color;
	float value;
};

struct Material {
	Shader *shader;
#define MAX_MATERIAL_MAPS 12
    // MATERIAL_MAP_DIFFUSE    = 0,
    // MATERIAL_MAP_SPECULAR,
    // MATERIAL_MAP_NORMAL,
    // MATERIAL_MAP_ROUGHNESS,
    // MATERIAL_MAP_OCCLUSION,
    // MATERIAL_MAP_EMISSION,
    // MATERIAL_MAP_HEIGHT,
    // MATERIAL_MAP_CUBEMAP,           // (NOTE: Uses GL_TEXTURE_CUBE_MAP)
    // MATERIAL_MAP_IRRADIANCE,        // (NOTE: Uses GL_TEXTURE_CUBE_MAP)
    // MATERIAL_MAP_PREFILTER,         // (NOTE: Uses GL_TEXTURE_CUBE_MAP)
    // MATERIAL_MAP_BRDF
	MaterialValue values[MAX_MATERIAL_MAPS];
	float params[4];
};

struct Mesh {
	char version;
#define MESH_NAME_MAX_LEN PATH_MAX_LEN
	char name[MESH_NAME_MAX_LEN];
	char path[PATH_MAX_LEN];

	char *boneNames[BONES_MAX];
	int boneNamesNum;

	MeshVertex *verts;
	int vertsNum;

	u16 *inds;
	int indsNum;

	int vaoId;

#define MAX_MESH_VERTEX_BUFFERS 7
	int vboIds[MAX_MESH_VERTEX_BUFFERS];

	/// Unserialized
	AABB bounds;
};

struct MeshSystem {
#define MESHES_MAX 512
	Mesh meshes[MESHES_MAX];
	int meshesNum;
};

MeshSystem *meshSys = NULL;

void initMesh();
Mesh *getMesh(char *path);
void readMesh(DataStream *stream, char *meshDir, Mesh *mesh);

Material createMaterial();
void uploadMesh(Mesh *mesh, bool hasWeights=false);
void drawMesh(Mesh *mesh, Matrix4 matrix=mat4(), Skeleton *skeleton=NULL, Material material=createMaterial());
/// FUNCTIONS ^

void initMesh() {
	meshSys = (MeshSystem *)zalloc(sizeof(MeshSystem));
}

Mesh *getMesh(char *path) {
	if (!meshSys) initMesh();

	if (!path) return NULL;
	if (path[0] == 0) return NULL;

	for (int i = 0; i < meshSys->meshesNum; i++) {
		Mesh *mesh = &meshSys->meshes[i];
		if (streq(mesh->path, path)) return mesh;
	}

	if (!fileExists(path)) {
		// logf("No mesh at %s\n", path);
		return NULL;
	}

	if (meshSys->meshesNum > MESHES_MAX-1) {
		logf("Too many meshes\n");
		return NULL;
	}

	Mesh *mesh = &meshSys->meshes[meshSys->meshesNum++];
	memset(mesh, 0, sizeof(Mesh));
	// mesh->id = ++meshSys->nextMeshId;
	strcpy(mesh->path, path);
	// mesh->globalArrayIndexPlusOne = meshSys->meshesNum;

	char *dir = frameMalloc(PATH_MAX_LEN);
	strcpy(dir, path);
	char *lastSlash = strrchr(dir, '/');
	if (lastSlash) *lastSlash = 0;

	DataStream *stream = loadDataStream(path);
	readMesh(stream, dir, mesh);
	destroyDataStream(stream);

	return mesh;
}

void readMesh(DataStream *stream, char *meshDir, Mesh *mesh) {
	mesh->version = readU8(stream);
	char *name = readString(stream);
	strcpy(mesh->name, name);
	free(name);

	mesh->boneNamesNum = readU32(stream);
	for (int i = 0; i < mesh->boneNamesNum; i++) {
		mesh->boneNames[i] = readString(stream);
	}

	mesh->vertsNum = readU32(stream);
	mesh->verts = (MeshVertex *)zalloc(sizeof(MeshVertex) * mesh->vertsNum);
	for (int i = 0; i < mesh->vertsNum; i++) {
		MeshVertex *vert = &mesh->verts[i];
		vert->position = readVec3(stream);
		vert->uv = readVec2(stream);
		vert->normal = readVec3(stream);
		vert->tangent = readVec3(stream);
		vert->boneIndices[0] = readU8(stream);
		vert->boneIndices[1] = readU8(stream);
		vert->boneIndices[2] = readU8(stream);
		vert->boneIndices[3] = readU8(stream);
		vert->boneWeights[0] = readFloat(stream);
		vert->boneWeights[1] = readFloat(stream);
		vert->boneWeights[2] = readFloat(stream);
		vert->boneWeights[3] = readFloat(stream);

		// logf("%d: %f, %f, %f\n", i, vert->position.x, vert->position.y, vert->position.z);

		float sum = 0;
		for (int i = 0; i < 4; i++) sum += vert->boneWeights[i];
		if (sum == 0) {
			vert->boneWeights[0] = 1;
		} else {
			for (int i = 0; i < 4; i++) vert->boneWeights[i] /= sum;
		}
	}

	mesh->indsNum = readU32(stream);
	mesh->inds = (u16 *)zalloc(sizeof(u16) * mesh->vertsNum);
	for (int i = 0; i < mesh->indsNum; i++) {
		mesh->inds[i] = readU16(stream);
	}

	readU32(stream); // Old material
	readU8(stream); // Old backFaceCulled

	readString(stream); // Old diffusePath
	readString(stream); // Old normalPath
	readString(stream); // Old specularPath

	{ /// Calculate bounds
		mesh->bounds.min = v3(99999, 99999, 99999);
		mesh->bounds.max = v3(-99999, -99999, -99999);

		for (int i = 0; i < mesh->vertsNum; i++) {
			Vec3 pos = mesh->verts[i].position;
			mesh->bounds = expand(mesh->bounds, pos);
		}
	} ///
}

Material createMaterial() {
	Material material = {};
	material.shader = renderer->lightingAnimatedShader;

	material.values[Raylib::MATERIAL_MAP_DIFFUSE].texture = renderer->whiteTexture;

	material.values[Raylib::MATERIAL_MAP_DIFFUSE].color = argbToRgbaFloat(0xFFFFFFFF);
	material.values[Raylib::MATERIAL_MAP_SPECULAR].color = argbToRgbaFloat(0xFFFFFFFF);

	return material;
}

void uploadMesh(Mesh *mesh, bool hasWeights) {
	if (mesh->vaoId) return;
	// This could be faster if it was global and resizable
	Vec3 *positions = (Vec3 *)frameMalloc(sizeof(Vec3) * mesh->vertsNum);
	Vec2 *uvs = (Vec2 *)frameMalloc(sizeof(Vec2) * mesh->vertsNum);
	Vec3 *normals = (Vec3 *)frameMalloc(sizeof(Vec3) * mesh->vertsNum);
	u8 *boneIndices = (u8 *)frameMalloc(sizeof(u8) * 4 * mesh->vertsNum);
	Vec4 *boneWeights = (Vec4 *)frameMalloc(sizeof(Vec4) * mesh->vertsNum);
	Vec2 *uvs2 = NULL;
	u16 *indices = mesh->inds;

	for (int i = 0; i < mesh->vertsNum; i++) {
		MeshVertex meshVert = mesh->verts[i];
		if (hasWeights) {
			boneIndices[i * 4 + 0] = meshVert.boneIndices[0];
			boneIndices[i * 4 + 1] = meshVert.boneIndices[1];
			boneIndices[i * 4 + 2] = meshVert.boneIndices[2];
			boneIndices[i * 4 + 3] = meshVert.boneIndices[3];
			boneWeights[i].x = meshVert.boneWeights[0];
			boneWeights[i].y = meshVert.boneWeights[1];
			boneWeights[i].z = meshVert.boneWeights[2];
			boneWeights[i].w = meshVert.boneWeights[3];
			positions[i] = meshVert.position;
			normals[i] = meshVert.normal;
		} else {
			boneWeights[i].x = 1;
			positions[i] = meshVert.position;
			normals[i] = meshVert.normal;
		}
		uvs[i] = meshVert.uv;
		normals[i] = meshVert.normal;
	}

	u8 *colors = boneIndices;
	Vec4 *tangents = boneWeights;
	int triangleCount = mesh->indsNum/3;
	int vertexCount = mesh->vertsNum;
	bool dynamic = false;

	if (mesh->vaoId) {
		logf("Trying to reupload mesh\n");
		return;
	}

	memset(mesh->vboIds, 0, sizeof(int) * MAX_MESH_VERTEX_BUFFERS);

	mesh->vaoId = Raylib::rlLoadVertexArray();
	Raylib::rlEnableVertexArray(mesh->vaoId);

	mesh->vboIds[0] = Raylib::rlLoadVertexBuffer(positions, vertexCount*3*sizeof(float), dynamic);
	Raylib::rlSetVertexAttribute(0, 3, RL_FLOAT, 0, 0, 0);
	Raylib::rlEnableVertexAttribute(0);

	mesh->vboIds[1] = Raylib::rlLoadVertexBuffer(uvs, vertexCount*2*sizeof(float), dynamic);
	Raylib::rlSetVertexAttribute(1, 2, RL_FLOAT, 0, 0, 0);
	Raylib::rlEnableVertexAttribute(1);

	if (normals) {
		mesh->vboIds[2] = Raylib::rlLoadVertexBuffer(normals, vertexCount*3*sizeof(float), dynamic);
		Raylib::rlSetVertexAttribute(2, 3, RL_FLOAT, 0, 0, 0);
		Raylib::rlEnableVertexAttribute(2);
	} else {
		float value[3] = { 1.0f, 1.0f, 1.0f };
		Raylib::rlSetVertexAttributeDefault(2, value, Raylib::SHADER_ATTRIB_VEC3, 3);
		Raylib::rlDisableVertexAttribute(2);
	}

	if (colors != NULL) {
		mesh->vboIds[3] = Raylib::rlLoadVertexBuffer(colors, vertexCount*4*sizeof(unsigned char), dynamic);
		Raylib::rlSetVertexAttribute(3, 4, RL_UNSIGNED_BYTE, 0, 0, 0);
		Raylib::rlEnableVertexAttribute(3);
	} else {
		float value[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		Raylib::rlSetVertexAttributeDefault(3, value, Raylib::SHADER_ATTRIB_VEC4, 4);
		Raylib::rlDisableVertexAttribute(3);
	}

	if (tangents != NULL) {
		mesh->vboIds[4] = Raylib::rlLoadVertexBuffer(tangents, vertexCount*4*sizeof(float), dynamic);
		Raylib::rlSetVertexAttribute(4, 4, RL_FLOAT, 0, 0, 0);
		Raylib::rlEnableVertexAttribute(4);
	} else {
		float value[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		Raylib::rlSetVertexAttributeDefault(4, value, Raylib::SHADER_ATTRIB_VEC4, 4);
		Raylib::rlDisableVertexAttribute(4);
	}

	if (uvs2 != NULL) {
		mesh->vboIds[5] = Raylib::rlLoadVertexBuffer(uvs2, vertexCount*2*sizeof(float), dynamic);
		Raylib::rlSetVertexAttribute(5, 2, RL_FLOAT, 0, 0, 0);
		Raylib::rlEnableVertexAttribute(5);
	} else {
		float value[2] = { 0.0f, 0.0f };
		Raylib::rlSetVertexAttributeDefault(5, value, Raylib::SHADER_ATTRIB_VEC2, 2);
		Raylib::rlDisableVertexAttribute(5);
	}

	if (indices != NULL) mesh->vboIds[6] = Raylib::rlLoadVertexBufferElement(indices, triangleCount*3*sizeof(u16), dynamic);

	if (!mesh->vaoId) logf("Failed to upload mesh (no vao support?)");

	Raylib::rlDisableVertexArray();
}

void unloadMesh(Mesh *mesh) {
	Raylib::rlUnloadVertexArray(mesh->vaoId);
	for (int i = 0; i < MAX_MESH_VERTEX_BUFFERS; i++) Raylib::rlUnloadVertexBuffer(mesh->vboIds[i]);
	mesh->vaoId = 0;
}

void drawMesh(Mesh *mesh, Matrix4 matrix, Skeleton *skeleton, Material material) {
	if (!renderer->in3dPass) {
		logf("Doing 3d draw call outside pass\n");
		return;
	}
	Matrix4 *boneTransforms = (Matrix4 *)frameMalloc(sizeof(Matrix4) * BONES_MAX);
	if (skeleton) {
		for (int i = 0; i < mesh->boneNamesNum; i++) {
			int meshIndex = i;
			for (int i = 0; i < skeleton->base->bonesNum; i++) {
				Bone *bone = &skeleton->base->bones[i];
				int boneIndex = i;
				if (streq(bone->name, mesh->boneNames[meshIndex])) {
					boneTransforms[meshIndex] = skeleton->meshTransforms[boneIndex];
				}
			}
		}
	} else {
		boneTransforms[0] = mat4();
	}

	bool hasWeights = false;
	if (skeleton) hasWeights = true;
	uploadMesh(mesh, hasWeights);

	Raylib::rlDisableBackfaceCulling(); //@hack Triangle winding is backwards for me, and Raylib doesn't have a way of changing it

	Raylib::Matrix raylibMatrix = toRaylib(matrix);
	{
		Raylib::rlEnableShader(material.shader->raylibShader.id);

		if (material.shader == renderer->lightingAnimatedShader) {
			glUniformMatrix4fv(renderer->lightingAnimatedShaderBoneTransformsLoc, BONES_MAX, true, (float *)boneTransforms); // Raylib can't set uniform matrix arrays
		}

		int *shaderLocs = material.shader->raylibShader.locs;
		if (shaderLocs[Raylib::SHADER_LOC_COLOR_DIFFUSE] != -1) {
			Vec4 color = argbToRgba(material.values[Raylib::MATERIAL_MAP_DIFFUSE].color);
			Raylib::rlSetUniform(shaderLocs[Raylib::SHADER_LOC_COLOR_DIFFUSE], &color.x, Raylib::SHADER_UNIFORM_VEC4, 1);
		}

		if (shaderLocs[Raylib::SHADER_LOC_COLOR_SPECULAR] != -1) {
			Vec4 color = argbToRgba(material.values[Raylib::MATERIAL_MAP_SPECULAR].color);
			Raylib::rlSetUniform(shaderLocs[Raylib::SHADER_LOC_COLOR_SPECULAR], &color.x, Raylib::SHADER_UNIFORM_VEC4, 1);
		}

		Raylib::Matrix matModel = MatrixMultiply(raylibMatrix, Raylib::rlGetMatrixTransform());
		Raylib::Matrix matView = Raylib::rlGetMatrixModelview();
		Raylib::Matrix matModelView = MatrixMultiply(matModel, matView);
		Raylib::Matrix matProjection = Raylib::rlGetMatrixProjection();

		if (shaderLocs[Raylib::SHADER_LOC_MATRIX_VIEW] != -1) Raylib::rlSetUniformMatrix(shaderLocs[Raylib::SHADER_LOC_MATRIX_VIEW], matView);
		if (shaderLocs[Raylib::SHADER_LOC_MATRIX_PROJECTION] != -1) Raylib::rlSetUniformMatrix(shaderLocs[Raylib::SHADER_LOC_MATRIX_PROJECTION], matProjection);
		if (shaderLocs[Raylib::SHADER_LOC_MATRIX_MODEL] != -1) Raylib::rlSetUniformMatrix(shaderLocs[Raylib::SHADER_LOC_MATRIX_MODEL], raylibMatrix);
		if (shaderLocs[Raylib::SHADER_LOC_MATRIX_NORMAL] != -1) Raylib::rlSetUniformMatrix(shaderLocs[Raylib::SHADER_LOC_MATRIX_NORMAL], MatrixTranspose(MatrixInvert(matModel)));

		for (int i = 0; i < MAX_MATERIAL_MAPS; i++) {
			if (material.values[i].texture) {
				Raylib::rlActiveTextureSlot(i);

				if (i == Raylib::MATERIAL_MAP_IRRADIANCE || i == Raylib::MATERIAL_MAP_PREFILTER || i == Raylib::MATERIAL_MAP_CUBEMAP) {
					Raylib::rlEnableTextureCubemap(material.values[i].texture->raylibTexture.id);
				} else {
					Raylib::rlEnableTexture(material.values[i].texture->raylibTexture.id);
				}

				Raylib::rlSetUniform(shaderLocs[Raylib::SHADER_LOC_MAP_DIFFUSE + i], &i, Raylib::SHADER_UNIFORM_INT, 1);
			}
		}

		if (!Raylib::rlEnableVertexArray(mesh->vaoId)) {
			logf("Couldn't bind vao (bad gpu support?)\n");
			return;
		}

		Raylib::Matrix matModelViewProjection = Raylib::MatrixMultiply(matModelView, matProjection);
		Raylib::rlSetUniformMatrix(shaderLocs[Raylib::SHADER_LOC_MATRIX_MVP], matModelViewProjection);

		if (mesh->inds) {
			Raylib::rlDrawVertexArrayElements(0, mesh->indsNum, 0);
		} else {
			Raylib::rlDrawVertexArray(0, mesh->vertsNum);
		}

		for (int i = 0; i < MAX_MATERIAL_MAPS; i++) {
			Raylib::rlActiveTextureSlot(i);

			if (i == Raylib::MATERIAL_MAP_IRRADIANCE || i == Raylib::MATERIAL_MAP_PREFILTER || i == Raylib::MATERIAL_MAP_CUBEMAP) {
				Raylib::rlDisableTextureCubemap();
			} else {
				Raylib::rlDisableTexture();
			}
		}

		Raylib::rlDisableVertexArray();
		Raylib::rlDisableVertexBuffer();
		Raylib::rlDisableVertexBufferElement();

		Raylib::rlDisableShader();

		Raylib::rlSetMatrixModelview(matView);
		Raylib::rlSetMatrixProjection(matProjection);
	}

	// unloadMesh(mesh);
}
