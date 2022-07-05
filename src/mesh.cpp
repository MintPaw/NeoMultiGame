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

	int material;
	bool backFaceCulled;

	char *diffusePath;
	char *normalPath;
	char *specularPath;

	/// Unserialized
	AABB bounds;
	Texture *diffuseTexture;
};

struct MeshSystem {
#define MESHES_MAX 512
	Mesh meshes[MESHES_MAX];
	int meshesNum;

	Raylib::Material raylibMaterial;
};

MeshSystem *meshSys = NULL;

void initMesh();
Mesh *getMesh(char *path);
void readMesh(DataStream *stream, char *meshDir, Mesh *mesh);
void drawMesh(Mesh *mesh, Matrix4 matrix=mat4(), Skeleton *skeleton=NULL, int tint=0xFFFFFFFF);
namespace Raylib {
	void MyDrawMesh(Mesh mesh, Material material, Matrix transform) {
    const int MAX_MATERIAL_MAPS = 12;
#if defined(GRAPHICS_API_OPENGL_33) || defined(GRAPHICS_API_OPENGL_ES2)
		rlEnableShader(material.shader.id);

		if (material.shader.locs[SHADER_LOC_COLOR_DIFFUSE] != -1) {
			float values[4] = {
				(float)material.maps[MATERIAL_MAP_DIFFUSE].color.r/255.0f,
				(float)material.maps[MATERIAL_MAP_DIFFUSE].color.g/255.0f,
				(float)material.maps[MATERIAL_MAP_DIFFUSE].color.b/255.0f,
				(float)material.maps[MATERIAL_MAP_DIFFUSE].color.a/255.0f
			};

			rlSetUniform(material.shader.locs[SHADER_LOC_COLOR_DIFFUSE], values, SHADER_UNIFORM_VEC4, 1);
		}

		if (material.shader.locs[SHADER_LOC_COLOR_SPECULAR] != -1) {
			float values[4] = {
				(float)material.maps[SHADER_LOC_COLOR_SPECULAR].color.r/255.0f,
				(float)material.maps[SHADER_LOC_COLOR_SPECULAR].color.g/255.0f,
				(float)material.maps[SHADER_LOC_COLOR_SPECULAR].color.b/255.0f,
				(float)material.maps[SHADER_LOC_COLOR_SPECULAR].color.a/255.0f
			};

			rlSetUniform(material.shader.locs[SHADER_LOC_COLOR_SPECULAR], values, SHADER_UNIFORM_VEC4, 1);
		}

		Matrix matModel = MatrixIdentity();
		Matrix matView = rlGetMatrixModelview();
		Matrix matModelView = MatrixIdentity();
		Matrix matProjection = rlGetMatrixProjection();

		if (material.shader.locs[SHADER_LOC_MATRIX_VIEW] != -1) rlSetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_VIEW], matView);
		if (material.shader.locs[SHADER_LOC_MATRIX_PROJECTION] != -1) rlSetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_PROJECTION], matProjection);
		if (material.shader.locs[SHADER_LOC_MATRIX_MODEL] != -1) rlSetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_MODEL], transform);

		matModel = MatrixMultiply(transform, rlGetMatrixTransform());

		matModelView = MatrixMultiply(matModel, matView);

		if (material.shader.locs[SHADER_LOC_MATRIX_NORMAL] != -1) rlSetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_NORMAL], MatrixTranspose(MatrixInvert(matModel)));

		for (int i = 0; i < MAX_MATERIAL_MAPS; i++) {
			if (material.maps[i].texture.id > 0) {
				rlActiveTextureSlot(i);

				if ((i == MATERIAL_MAP_IRRADIANCE) ||
					(i == MATERIAL_MAP_PREFILTER) ||
					(i == MATERIAL_MAP_CUBEMAP)) rlEnableTextureCubemap(material.maps[i].texture.id);
				else rlEnableTexture(material.maps[i].texture.id);

				rlSetUniform(material.shader.locs[SHADER_LOC_MAP_DIFFUSE + i], &i, SHADER_UNIFORM_INT, 1);
			}
		}

		if (!rlEnableVertexArray(mesh.vaoId)) {
			logf("Couldn't bind vao (bad gpu support?)\n");
			return;
		}

		Matrix matModelViewProjection = MatrixMultiply(matModelView, matProjection);

		rlSetUniformMatrix(material.shader.locs[SHADER_LOC_MATRIX_MVP], matModelViewProjection);

		if (mesh.indices != NULL) rlDrawVertexArrayElements(0, mesh.triangleCount*3, 0);
		else rlDrawVertexArray(0, mesh.vertexCount);

		for (int i = 0; i < MAX_MATERIAL_MAPS; i++) {
			rlActiveTextureSlot(i);

			if ((i == MATERIAL_MAP_IRRADIANCE) ||
				(i == MATERIAL_MAP_PREFILTER) ||
				(i == MATERIAL_MAP_CUBEMAP)) rlDisableTextureCubemap();
			else rlDisableTexture();
		}

		rlDisableVertexArray();
		rlDisableVertexBuffer();
		rlDisableVertexBufferElement();

		rlDisableShader();

		rlSetMatrixModelview(matView);
		rlSetMatrixProjection(matProjection);
#else
		logf("Can't use meshes in gles!\n");
#endif
	}

	void MyUploadMesh(Mesh *mesh, bool dynamic) {
		if (mesh->vaoId > 0) {
			TRACELOG(LOG_WARNING, "VAO: [ID %i] Trying to re-load an already loaded mesh", mesh->vaoId);
			return;
		}

		const int MAX_MESH_VERTEX_BUFFERS = 7; // Copied from rmodels.c
		mesh->vboId = (unsigned int *)RL_CALLOC(MAX_MESH_VERTEX_BUFFERS, sizeof(unsigned int));

		mesh->vaoId = 0;        // Vertex Array Object
		mesh->vboId[0] = 0;     // Vertex buffer: positions
		mesh->vboId[1] = 0;     // Vertex buffer: texcoords
		mesh->vboId[2] = 0;     // Vertex buffer: normals
		mesh->vboId[3] = 0;     // Vertex buffer: colors
		mesh->vboId[4] = 0;     // Vertex buffer: tangents
		mesh->vboId[5] = 0;     // Vertex buffer: texcoords2
		mesh->vboId[6] = 0;     // Vertex buffer: indices

		mesh->vaoId = rlLoadVertexArray();
		rlEnableVertexArray(mesh->vaoId);

		void *vertices = mesh->animVertices != NULL ? mesh->animVertices : mesh->vertices;
		mesh->vboId[0] = rlLoadVertexBuffer(vertices, mesh->vertexCount*3*sizeof(float), dynamic);
		rlSetVertexAttribute(0, 3, RL_FLOAT, 0, 0, 0);
		rlEnableVertexAttribute(0);

		mesh->vboId[1] = rlLoadVertexBuffer(mesh->texcoords, mesh->vertexCount*2*sizeof(float), dynamic);
		rlSetVertexAttribute(1, 2, RL_FLOAT, 0, 0, 0);
		rlEnableVertexAttribute(1);

		if (mesh->normals != NULL) {
			void *normals = mesh->animNormals != NULL ? mesh->animNormals : mesh->normals;
			mesh->vboId[2] = rlLoadVertexBuffer(normals, mesh->vertexCount*3*sizeof(float), dynamic);
			rlSetVertexAttribute(2, 3, RL_FLOAT, 0, 0, 0);
			rlEnableVertexAttribute(2);
		} else {
			float value[3] = { 1.0f, 1.0f, 1.0f };
			rlSetVertexAttributeDefault(2, value, SHADER_ATTRIB_VEC3, 3);
			rlDisableVertexAttribute(2);
		}

		if (mesh->colors != NULL) {
			mesh->vboId[3] = rlLoadVertexBuffer(mesh->colors, mesh->vertexCount*4*sizeof(unsigned char), dynamic);
			rlSetVertexAttribute(3, 4, RL_UNSIGNED_BYTE, 0, 0, 0);
			rlEnableVertexAttribute(3);
		} else {
			float value[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
			rlSetVertexAttributeDefault(3, value, SHADER_ATTRIB_VEC4, 4);
			rlDisableVertexAttribute(3);
		}

		if (mesh->tangents != NULL) {
			mesh->vboId[4] = rlLoadVertexBuffer(mesh->tangents, mesh->vertexCount*4*sizeof(float), dynamic);
			rlSetVertexAttribute(4, 4, RL_FLOAT, 0, 0, 0);
			rlEnableVertexAttribute(4);
		} else {
			float value[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			rlSetVertexAttributeDefault(4, value, SHADER_ATTRIB_VEC4, 4);
			rlDisableVertexAttribute(4);
		}

		if (mesh->texcoords2 != NULL) {
			mesh->vboId[5] = rlLoadVertexBuffer(mesh->texcoords2, mesh->vertexCount*2*sizeof(float), dynamic);
			rlSetVertexAttribute(5, 2, RL_FLOAT, 0, 0, 0);
			rlEnableVertexAttribute(5);
		} else {
			float value[2] = { 0.0f, 0.0f };
			rlSetVertexAttributeDefault(5, value, SHADER_ATTRIB_VEC2, 2);
			rlDisableVertexAttribute(5);
		}

		if (mesh->indices != NULL) mesh->vboId[6] = rlLoadVertexBufferElement(mesh->indices, mesh->triangleCount*3*sizeof(unsigned short), dynamic);

		if (!mesh->vaoId) logf("Failed to upload mesh (no vao support?)");

		rlDisableVertexArray();
	}
}

/// FUNCTIONS ^

void initMesh() {
	meshSys = (MeshSystem *)zalloc(sizeof(MeshSystem));

	meshSys->raylibMaterial = Raylib::LoadMaterialDefault();
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

	mesh->material = readU32(stream);
	mesh->backFaceCulled = readU8(stream);

	mesh->diffusePath = readString(stream);
	mesh->normalPath = readString(stream);
	mesh->specularPath = readString(stream);

	if (mesh->diffusePath) {
		char *path = frameSprintf("%s/%s", meshDir, mesh->diffusePath);
		mesh->diffuseTexture = getTexture(path);
		if (!mesh->diffuseTexture) logf("Failed to load diffuse texture: %s\n", path);
	}

	{ /// Calculate bounds
		mesh->bounds.min = v3(99999, 99999, 99999);
		mesh->bounds.max = v3(-99999, -99999, -99999);

		for (int i = 0; i < mesh->vertsNum; i++) {
			Vec3 pos = mesh->verts[i].position;
			mesh->bounds = expand(mesh->bounds, pos);
		}
	} ///
}

void drawMesh(Mesh *mesh, Matrix4 matrix, Skeleton *skeleton, int tint) {
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

	// This could be much faster if it was global and resizable
	Vec3 *positions = (Vec3 *)frameMalloc(sizeof(Vec3) * mesh->vertsNum);
	Vec2 *uvs = (Vec2 *)frameMalloc(sizeof(Vec2) * mesh->vertsNum);
	Vec3 *normals = (Vec3 *)frameMalloc(sizeof(Vec3) * mesh->vertsNum);
	u8 *boneIndices = (u8 *)frameMalloc(sizeof(u8) * 4 * mesh->vertsNum);
	Vec4 *boneWeights = (Vec4 *)frameMalloc(sizeof(Vec4) * mesh->vertsNum);

	for (int i = 0; i < mesh->vertsNum; i++) {
		MeshVertex meshVert = mesh->verts[i];
		if (skeleton) {
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

	Raylib::Mesh raylibMesh = {};
	raylibMesh.vertices = &positions[0].x;
	raylibMesh.texcoords = &uvs[0].x;
	raylibMesh.normals = &normals[0].x;
	raylibMesh.indices = mesh->inds;
	raylibMesh.colors = boneIndices;
	raylibMesh.tangents = &boneWeights[0].x;
	raylibMesh.triangleCount = mesh->indsNum/3;
	raylibMesh.vertexCount = mesh->vertsNum;

	Raylib::rlEnableShader(renderer->lightingAnimatedShader.id);
	glUniformMatrix4fv(renderer->lightingAnimatedShaderBoneTransformsLoc, BONES_MAX, true, (float *)boneTransforms); // Raylib can't set uniform matrix arrays
	// glUniformMatrix4fv(shader->u_projection, 1, false, props.projectionMatrix.data);
	// Raylib::SetShaderValueV(renderer->lightingAnimatedShader, renderer->lightingAnimatedShaderBoneTransformsLoc, boneTransforms, int uniformType, int count)

	// Raylib::UploadMesh(&raylibMesh, false);
	Raylib::MyUploadMesh(&raylibMesh, false);

	// meshSys->raylibMaterial.shader = renderer->lightingShader;
	meshSys->raylibMaterial.shader = renderer->lightingAnimatedShader;
	// meshSys->raylibMaterial.maps[MATERIAL_MAP_DIFFUSE] =
	meshSys->raylibMaterial.maps[Raylib::MATERIAL_MAP_DIFFUSE].color = toRaylibColor(tint);

	Raylib::rlDisableBackfaceCulling(); //@hack Triangle winding is backwards for me, and Raylib doesn't have a way of changing it

	Raylib::Matrix raylibMatrix = toRaylib(matrix);
	// DrawMesh(raylibMesh, meshSys->raylibMaterial, raylibMatrix);
	Raylib::MyDrawMesh(raylibMesh, meshSys->raylibMaterial, raylibMatrix);

	// Unload mesh
	Raylib::rlUnloadVertexArray(raylibMesh.vaoId);
	const int MAX_MESH_VERTEX_BUFFERS = 7; // Copied from rmodels.c
	for (int i = 0; i < MAX_MESH_VERTEX_BUFFERS; i++) Raylib::rlUnloadVertexBuffer(raylibMesh.vboId[i]);
}
