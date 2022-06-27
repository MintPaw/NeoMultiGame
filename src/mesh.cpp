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

Raylib::Matrix toRaylib(Matrix4 matrix) {
	Raylib::Matrix raylibMatrix = {};
	memcpy(&raylibMatrix.m0, matrix.transpose().data, sizeof(float) * 16);
	return raylibMatrix;
}

void drawMesh(Mesh *mesh, Matrix4 matrix, Skeleton *skeleton, int tint) {
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
	}

	// This could be much faster if it was global and resizable
	Vec3 *positions = (Vec3 *)frameMalloc(sizeof(Vec3) * mesh->vertsNum);
	Vec2 *uvs = (Vec2 *)frameMalloc(sizeof(Vec2) * mesh->vertsNum);
	Vec3 *normals = (Vec3 *)frameMalloc(sizeof(Vec3) * mesh->vertsNum);

	for (int i = 0; i < mesh->indsNum; i++) {
		MeshVertex meshVert = mesh->verts[mesh->inds[i]];
		if (skeleton) {
			Matrix4 boneTrans = boneTransforms[meshVert.boneIndices[0]] * meshVert.boneWeights[0];
			boneTrans += boneTransforms[meshVert.boneIndices[1]] * meshVert.boneWeights[1];
			boneTrans += boneTransforms[meshVert.boneIndices[2]] * meshVert.boneWeights[2];
			boneTrans += boneTransforms[meshVert.boneIndices[3]] * meshVert.boneWeights[3];
			positions[i] = boneTrans * meshVert.position;
			normals[i] = boneTrans.invert().transpose() * meshVert.normal;
		} else {
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
	raylibMesh.triangleCount = mesh->indsNum/3;
	raylibMesh.vertexCount = mesh->vertsNum;

	Raylib::UploadMesh(&raylibMesh, false);

	meshSys->raylibMaterial.shader = renderer->lightingShader;
	// meshSys->raylibMaterial.maps[MATERIAL_MAP_DIFFUSE] =
	meshSys->raylibMaterial.maps[Raylib::MATERIAL_MAP_DIFFUSE].color = toRaylibColor(tint);

	Raylib::rlDisableBackfaceCulling(); //@hack Triangle winding is backwards for me, and Raylib doesn't have a way of changing it

	Raylib::Matrix raylibMatrix = toRaylib(matrix);
	DrawMesh(raylibMesh, meshSys->raylibMaterial, raylibMatrix);

	// Unload mesh
	Raylib::rlUnloadVertexArray(raylibMesh.vaoId);
	const int MAX_MESH_VERTEX_BUFFERS = 7; // Copied from rmodels.c
	for (int i = 0; i < MAX_MESH_VERTEX_BUFFERS; i++) Raylib::rlUnloadVertexBuffer(raylibMesh.vboId[i]);
}
