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
};

MeshSystem *meshSys = NULL;

void initMesh();
Mesh *getMesh(char *path);
void readMesh(DataStream *stream, char *meshDir, Mesh *mesh);
void drawMesh(Mesh *mesh, Matrix4 matrix=mat4(), Skeleton *skeleton=NULL, int tint=0xFFFFFFFF);

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

	Raylib::Color raylibTint;
	{
		float alpha = 1;
		int a, r, g, b;
		hexToArgb(tint, &a, &r, &g, &b);
		a *= alpha;
		r *= a/255.0;
		g *= a/255.0;
		b *= a/255.0;
		tint = argbToHex(a, r, g, b);
		raylibTint = toRaylibColor(tint);

		Texture *texture = renderer->whiteTexture;
		Raylib::Texture raylibTexture = texture->raylibTexture;
		Raylib::rlCheckRenderBatchLimit((mesh->indsNum/3 - 1)*4);

		Raylib::rlSetTexture(raylibTexture.id);

		Raylib::rlBegin(RL_TRIANGLES);
	}

	for (int i = 0; i < mesh->indsNum/3; i++) {
		MeshVertex meshVerts[3] = {
			mesh->verts[mesh->inds[i * 3 + 0]],
			mesh->verts[mesh->inds[i * 3 + 1]],
			mesh->verts[mesh->inds[i * 3 + 2]],
		};

		Vec3 verts[3] = {
			meshVerts[0].position,
			meshVerts[1].position,
			meshVerts[2].position,
		};

		Vec2 uvs[3] = {
			meshVerts[0].uv,
			meshVerts[1].uv,
			meshVerts[2].uv,
		};

		for (int i = 0; i < 3; i++) {
			if (skeleton) {
				MeshVertex meshVert = meshVerts[i];
				Matrix4 boneTrans = boneTransforms[meshVert.boneIndices[0]] * meshVert.boneWeights[0];
				boneTrans += boneTransforms[meshVert.boneIndices[1]] * meshVert.boneWeights[1];
				boneTrans += boneTransforms[meshVert.boneIndices[2]] * meshVert.boneWeights[2];
				boneTrans += boneTransforms[meshVert.boneIndices[3]] * meshVert.boneWeights[3];
				verts[i] = boneTrans * verts[i];
				verts[i] = matrix * verts[i];

				// Vec4 vert4 = v4(verts[i], 1);
				// vert4 = boneTrans * vert4;
				// verts[i] = v3(vert4);
			} else {
				verts[i] = matrix * verts[i];
			}

			Raylib::rlColor4ub(raylibTint.r, raylibTint.g, raylibTint.b, raylibTint.a);
			Raylib::rlTexCoord2f(uvs[i].x, uvs[i].y);
			Raylib::rlVertex3f(verts[i].x, verts[i].y, verts[i].z);
		}

		// drawTriangle(renderer->whiteTexture, verts, uvs);
	}

	Raylib::rlEnd();

	Raylib::rlSetTexture(0);
}
