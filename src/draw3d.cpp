#define FAR_PLANE 500
#define DIFFUSE_INSTANCES_MAX 16

struct Skybox {
	GLuint cubeMapId;
	GLuint vertexBuffer;
	GLuint indexBuffer;
};

Skybox *createSkybox(char *dir) { // right, left, bottom, top, front, back
	Skybox *skybox = (Skybox *)malloc(sizeof(Skybox));

	glGenTextures(1, &skybox->cubeMapId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->cubeMapId);

	for (int i = 0; i < 6; i++) {
		int texNum = i;

		char *path = NULL;
		if (i == 0) path = frameSprintf("%s/right.png", dir);
		if (i == 1) path = frameSprintf("%s/left.png", dir);
		if (i == 2) path = frameSprintf("%s/bottom.png", dir);
		if (i == 3) path = frameSprintf("%s/top.png", dir);
		if (i == 4) path = frameSprintf("%s/front.png", dir);
		if (i == 5) path = frameSprintf("%s/back.png", dir);

		int width, height, channels;
		int pngSize;
		void *pngData = readFile(path, &pngSize);

		bool oldFlip = stbi_get_flip_vertically_on_load();
		stbi_set_flip_vertically_on_load(true);
		u8 *data = stbi_load_from_memory((u8 *)pngData, pngSize, &width, &height, &channels, 0);
		stbi_set_flip_vertically_on_load(oldFlip);
		if (!data) {
			logf("Failed to load %s for skybox\n", dir);
			return NULL;
		}

		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0,
			GL_RGB,
			width,
			height,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			data
		);
		free(data);
		free(pngData);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	GLfloat verts[] = {
		1, 1, 1 ,  -1, 1, 1,  -1,-1, 1,   1,-1, 1,   // v0,v1,v2,v3 (front)
		1, 1, 1 ,   1,-1, 1,   1,-1,-1,   1, 1,-1,   // v0,v3,v4,v5 (right)
		1, 1, 1 ,   1, 1,-1,  -1, 1,-1,  -1, 1, 1,   // v0,v5,v6,v1 (top)
		-1, 1, 1,  -1, 1,-1,  -1,-1,-1,  -1,-1, 1,   // v1,v6,v7,v2 (left)
		-1,-1,-1,   1,-1,-1,   1,-1, 1,  -1,-1, 1,   // v7,v4,v3,v2 (bottom)
		1,-1,-1 ,  -1,-1,-1,  -1, 1,-1,   1, 1,-1    // v4,v7,v6,v5 (back)
	}; 

// index array of vertex array for glDrawElements() & glDrawRangeElement()
	u16 inds[]  = {
		0, 1, 2,   2, 3, 0,       // front
		4, 5, 6,   6, 7, 4,       // right
		8, 9, 10,  10,11, 8,      // top
		12,13,14,  14,15,12,      // left
		16,17,18,  18,19,16,      // bottom
		20,21,22,  22,23,20       // back
	};
	glGenBuffers(1, &skybox->vertexBuffer);
	glGenBuffers(1, &skybox->indexBuffer);

	bindVertexBuffer(skybox->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * ArrayLength(verts), verts, GL_STATIC_DRAW);

	bindElementBuffer(skybox->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16) * ArrayLength(inds), inds, GL_STATIC_DRAW);

	return skybox;
}

/// Texture End
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

struct MeshTri {
	// int triIndex;

	Tri tri;
	// u16 inds[3];
	MeshVertex verts[3];
};

struct TriOctree {
	AABB bounds;

#define TRI_OCTREE_TRIS_MAX 4
	MeshTri *meshTris[TRI_OCTREE_TRIS_MAX];
	int meshTrisNum;

	TriOctree *children;
	bool subdivided;
};

enum MaterialType {
	MATERIAL_DIFFUSE=0,
	MATERIAL_TERRAIN_MAP=1,
};

struct Mesh {
	char version;
#define MESH_NAME_MAX_LEN PATH_MAX_LEN
	char name[MESH_NAME_MAX_LEN];
	char path[PATH_MAX_LEN];

	char *boneNames[BONES_MAX];
	int boneNamesNum;

	MeshVertex *customVerts;
	int vertsIndex; // Unserialized
	int vertsNum;

	u16 *customInds;
	int indsIndex; // Unserialized
	int indsNum;

	MaterialType material;
	bool backFaceCulled;

	char *diffusePath;
	char *normalPath;
	char *specularPath;

	/// Unserialized
	int id;
	bool custom;
	AABB bounds;

	Texture *diffuseTexture;
	Texture *normalTexture;
	Texture *specularTexture;

	u16 globalArrayIndexPlusOne;

	GLuint indexBuffer; // Custom
	GLuint vertexBuffer; // Custom

	MeshTri *meshTris;
	int meshTrisNum;

	bool skipOctree;
	TriOctree *octree;
};

struct MeshProps {
	Matrix4 matrix;
	Matrix4 viewMatrix; // Don't set directly
	Matrix4 projectionMatrix; // Don't set directly
	Skeleton *skeleton;
	int tint;
	int layer;
	float alpha;
#define _F_3D_BLOOM (1 << 0)
#define _F_3D_NO_GEOM (1 << 1)
	int flags;

	Vec3 ambientLight;
	Vec3 diffuseLight;
	Vec3 specularLight;

	Texture *slopeTexture;
};

enum DrawQueueItemType {
	DRAW_QUEUE_NONE,
	DRAW_QUEUE_MESH,
};

struct DrawQueueItem {
	u32 sortKey;

	DrawQueueItemType type;
	int translucent;
	Mesh *mesh;
	MeshProps props;
	Matrix4 propsMatrix;

	Vec4 coords;
	int color;
	Texture *texture;
	Font *font;
	char *string;
};

#pragma pack(push, 1)
struct DiffuseVertexUniformBlock {
	Matrix4 u_model;
	Matrix4 u_viewProjection;
	Matrix4 u_boneTransforms[BONES_MAX];
	Vec4 u_lightPos;
	Vec4 u_viewPos;
	int u_usesNormalMap;
	int pad[3];
};
struct DiffuseFragmentUniformBlock {
	Vec4 u_ambientLight;
	Vec4 u_diffuseLight;
	Vec4 u_specularLight;
	Vec4 u_alpha;
	int u_flags;
	int pad[3];
};
#pragma pack(pop)

struct AnimatedMeshProgram {
	GLuint program;
	GLuint u_diffuseTexture;
	GLuint u_normalTexture;

	GLuint vertexUniform;
	GLuint fragmentUniform;

	void *uniformBlock;
	int vertexBlockSize;
	int fragmentBlockSize;
	int fragmentBlockPad;
	int uniformBlockSize;

	GLuint vao;
	GLuint vertexBuffer;
	GLuint indexBuffer;
	GLuint uniformBuffer;
};

struct SkyboxProgram {
	GLuint program;
	GLuint a_position;
	GLuint u_view;
	GLuint u_projection;
	GLuint u_cubeMap;
};

struct TerrainProgram {
	GLuint program;
	GLuint a_position;
	GLuint a_uv;
	GLuint a_normal;
	GLuint u_model;
	GLuint u_view;
	GLuint u_projection;
	GLuint u_lightPos;
	GLuint u_viewPos;
	GLuint u_ambientLight;
	GLuint u_diffuseLight;
	GLuint u_specularLight;
	GLuint u_diffuseTexture;
	GLuint u_slopeTexture;
};

struct MeshSystem {
	AnimatedMeshProgram animatedMeshProgram;
	SkyboxProgram skyboxProgram;
	TerrainProgram terrainProgram;

#define MESHES_MAX 512
	Mesh meshes[MESHES_MAX];
	int meshesNum;
	int nextMeshId;

	MeshVertex *verts;
	int vertsNum;
	int vertsMax;
	int gpuVertsNum;

	u16 *inds;
	int indsNum;
	int indsMax;
	int gpuIndsNum;

	bool gpuVertsDirty;

#define DRAW_QUEUE_MAX (8192*2*2)
	DrawQueueItem drawQueue[DRAW_QUEUE_MAX];
	int drawQueueNum;

	DrawQueueItem *sortedItems[DRAW_QUEUE_MAX];
	int sortedItemsNum;

	int lastDrawCalls;
	bool disableCulling;
	bool disableNormals;
	bool disableDiffuse;
	bool disableSpecular;
	bool disableInstancing;

	bool debugShowMeshBounds;
	Framebuffer *debugFramebuffer;
	Texture *debugMeshTexture;
	int debugSelectedMesh;
};

struct WorldProps {
	Vec3 cameraPosition;

	Vec3 sunPosition;
	Vec3 ambientLight;
	Vec3 diffuseLight;
	Vec3 specularLight;

	Matrix4 viewMatrix;
	Matrix4 projectionMatrix;
};

WorldProps *defaultWorld = NULL;
MeshSystem *meshSys = NULL;

void initMesh();
Mesh *getMesh(char *path);

Mesh *createCustomMesh(Vec3 *verts, Vec2 *uvs, Vec3 *normals, int vertsNum, u16 *inds, int indsNum, MaterialType material, char *diffusePath, bool skipOctree=false);
Mesh *createCustomMesh(Mesh *origMesh);

MeshProps newMeshProps();
Mesh *getMesh(int id);
void drawMesh(Mesh *mesh, MeshProps props);
void drawDiffuseMeshesImm(Mesh *mesh, MeshProps **instanceProps, int instancePropsNum);
void drawTerrainMeshImm(Mesh *mesh, MeshProps props);
void process3dDrawQueue();
void setBackFaceCulling(bool enabled);

void readMesh(DataStream *stream, char *meshDir, Mesh *mesh);
void writeMesh(char *path, Mesh *mesh);
void writeMesh(DataStream *stream, Mesh *mesh);

int getMeshVertices(int count);
int getMeshIndices(int count);

void destroyMesh(Mesh *mesh);

bool intersectsLine(Mesh *mesh, Vec3 start, Vec3 end, float *outDist, Vec2 *outUv, MeshTri **outMeshTri);
bool intersectsRay(TriOctree *octree, Vec3 start, Vec3 dir, float *outDist, Vec2 *outUv, MeshTri **outMeshTri);
void intersects(Mesh *mesh, Capsule3 cap, IntersectionResult *outResults, int *outResultsNum, int outResultsMax);
void getTriangles(TriOctree *octree, AABB bounds, MeshTri **outMeshTris, int *outMeshTrisNum, int outMeshTrisMax, int depth=0);

void finalizeMesh(Mesh *mesh);
void showMeshGui();

/// FUNCTIONS FOR MESH^

void drawBeam(Vec3 start, Vec3 end, float thickness=0.1, int color=0xFFFFFFFF);
void drawCubeoid(Vec3 center, float size=0.1, int color=0xFFFFFFFF);
void drawCubeoid(Vec3 center, Vec3 size, int color=0xFFFFFFFF);
void drawCubeoidOutline(Vec3 center, Vec3 size, float thickness=0.1, int color=0xFFFFFFFF);
void drawSphere(Vec3 center, float radius, int color=0xFFFFFFFF);
void drawBoundsOutline(AABB bounds, float thickness=0.1, int color=0xFFFFFFFF, Matrix4 matrix=mat4());
void drawBoundsOutline(Vec3 min, Vec3 max, float thickness=0.1, int color=0xFFFFFFFF, Matrix4 matrix=mat4());
/// Utils^

void reloadAllMeshesForModels(); // This is actually in models.cpp

void initMesh() {
	meshSys = (MeshSystem *)zalloc(sizeof(MeshSystem));
	logf("meshSys is %.1fmb\n", sizeof(MeshSystem)/(float)Megabytes(1));
	renderer->using3d = true;

	defaultWorld = (WorldProps *)zalloc(sizeof(WorldProps)); // I'm not sure why this happens here now, it was in initModelLoader, but I'm not sure who called it
	defaultWorld->sunPosition = v3(9, -0.8, 19);
	defaultWorld->ambientLight = v3(.1, .1, .1);
	defaultWorld->diffuseLight = v3(0.5, 0.5, 0.5);
	defaultWorld->specularLight = v3(0.1, 0.1, 0.1);

	meshSys->animatedMeshProgram.program = buildShader("assets/common/shaders/animatedMesh.shader");
	GetUniform(meshSys->animatedMeshProgram, u_diffuseTexture);
	GetUniform(meshSys->animatedMeshProgram, u_normalTexture);
	meshSys->animatedMeshProgram.vertexUniform = glGetUniformBlockIndex(meshSys->animatedMeshProgram.program, "DiffuseVertexUniformBlock");
	meshSys->animatedMeshProgram.fragmentUniform = glGetUniformBlockIndex(meshSys->animatedMeshProgram.program, "DiffuseFragmentUniformBlock");

	meshSys->terrainProgram.program = buildShader("assets/common/shaders/terrainShader.shader");
	GetAttrib(meshSys->terrainProgram, a_position);
	GetAttrib(meshSys->terrainProgram, a_uv);
	GetAttrib(meshSys->terrainProgram, a_normal);
	GetUniform(meshSys->terrainProgram, u_model);
	GetUniform(meshSys->terrainProgram, u_view);
	GetUniform(meshSys->terrainProgram, u_projection);
	GetUniform(meshSys->terrainProgram, u_lightPos);
	GetUniform(meshSys->terrainProgram, u_viewPos);
	GetUniform(meshSys->terrainProgram, u_ambientLight);
	GetUniform(meshSys->terrainProgram, u_diffuseLight);
	GetUniform(meshSys->terrainProgram, u_specularLight);
	GetUniform(meshSys->terrainProgram, u_diffuseTexture);
	GetUniform(meshSys->terrainProgram, u_slopeTexture);

	meshSys->skyboxProgram.program = buildShader("assets/common/shaders/skybox.shader");
	GetAttrib(meshSys->skyboxProgram, a_position);
	GetUniform(meshSys->skyboxProgram, u_view);
	GetUniform(meshSys->skyboxProgram, u_projection);
	GetUniform(meshSys->skyboxProgram, u_cubeMap);

	meshSys->vertsMax = 8;
	meshSys->verts = (MeshVertex *)zalloc(sizeof(MeshVertex) * meshSys->vertsMax);

	meshSys->indsMax = 8;
	meshSys->inds = (u16 *)zalloc(sizeof(u16) * meshSys->indsMax);

	{
		AnimatedMeshProgram *shader = &meshSys->animatedMeshProgram;

		glGenVertexArrays(1, &shader->vao);
		bindVertexArray(shader->vao);

		glGenBuffers(1, &shader->vertexBuffer);
		bindVertexBuffer(shader->vertexBuffer);

		glGenBuffers(1, &shader->indexBuffer);
		bindElementBuffer(shader->indexBuffer);

		int attrib;

		attrib = glGetAttribLocation(shader->program, "a_position");
		glEnableVertexAttribArray(attrib);
		glVertexAttribPointer(attrib, 3, GL_FLOAT, false, sizeof(MeshVertex), (void *)offsetof(MeshVertex, position));

		attrib = glGetAttribLocation(shader->program, "a_uv");
		glEnableVertexAttribArray(attrib);
		glVertexAttribPointer(attrib, 2, GL_FLOAT, false, sizeof(MeshVertex), (void *)offsetof(MeshVertex, uv));

		attrib = glGetAttribLocation(shader->program, "a_normal");
		glEnableVertexAttribArray(attrib);
		glVertexAttribPointer(attrib, 3, GL_FLOAT, false, sizeof(MeshVertex), (void *)offsetof(MeshVertex, normal));

		attrib = glGetAttribLocation(shader->program, "a_tangent");
		glEnableVertexAttribArray(attrib);
		glVertexAttribPointer(attrib, 3, GL_FLOAT, false, sizeof(MeshVertex), (void *)offsetof(MeshVertex, tangent));

		attrib = glGetAttribLocation(shader->program, "a_boneIndices");
		glEnableVertexAttribArray(attrib);
		glVertexAttribPointer(attrib, 4, GL_UNSIGNED_BYTE, false, sizeof(MeshVertex), (void *)offsetof(MeshVertex, boneIndices));

		attrib = glGetAttribLocation(shader->program, "a_boneWeights");
		glEnableVertexAttribArray(attrib);
		glVertexAttribPointer(attrib, 4, GL_FLOAT, false, sizeof(MeshVertex), (void *)offsetof(MeshVertex, boneWeights));

		shader->vertexBlockSize = sizeof(DiffuseVertexUniformBlock) * DIFFUSE_INSTANCES_MAX;
		shader->fragmentBlockSize = sizeof(DiffuseFragmentUniformBlock) * DIFFUSE_INSTANCES_MAX;
		int uniformBufferOffsetAlignment;
		glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uniformBufferOffsetAlignment);
		shader->fragmentBlockPad = shader->vertexBlockSize % uniformBufferOffsetAlignment;
		if (shader->fragmentBlockPad != 0) shader->fragmentBlockPad = uniformBufferOffsetAlignment - shader->fragmentBlockPad;

		shader->uniformBlockSize = shader->vertexBlockSize + shader->fragmentBlockSize + shader->fragmentBlockPad;
		shader->uniformBlock = (u8 *)malloc(shader->uniformBlockSize);

		glGenBuffers(1, &shader->uniformBuffer);
		bindUniformBuffer(shader->uniformBuffer);
		glBufferData(GL_UNIFORM_BUFFER, shader->uniformBlockSize, NULL, GL_DYNAMIC_DRAW);

		glUniformBlockBinding(shader->program, shader->vertexUniform, 0);
		glUniformBlockBinding(shader->program, shader->fragmentUniform, 1);

		glBindBufferRange(GL_UNIFORM_BUFFER, 0, shader->uniformBuffer, 0, shader->vertexBlockSize);
		glBindBufferRange(GL_UNIFORM_BUFFER, 1, shader->uniformBuffer, shader->vertexBlockSize + shader->fragmentBlockPad, shader->fragmentBlockSize);

		CheckGlErrorHigh();
		bindVertexArray(renderer->defaultVao);
	}
}

Mesh *getMesh(char *path) {
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
	mesh->id = ++meshSys->nextMeshId;
	strcpy(mesh->path, path);
	mesh->globalArrayIndexPlusOne = meshSys->meshesNum;

	char *dir = frameMalloc(PATH_MAX_LEN);
	strcpy(dir, path);
	char *lastSlash = strrchr(dir, '/');
	if (lastSlash) *lastSlash = 0;

	DataStream *stream = loadDataStream(path);
	readMesh(stream, dir, mesh);
	destroyDataStream(stream);

	return mesh;
}

Mesh *getMesh(int id) {
	for (int i = 0; i < meshSys->meshesNum; i++) {
		Mesh *mesh = &meshSys->meshes[i];
		if (mesh->id == id) return mesh;
	}

	return NULL;
}

Mesh *injectCustomMesh(Mesh *origMesh);
Mesh *injectCustomMesh(Mesh *origMesh) {
	if (!origMesh->custom) {
		logf("Injected mesh is not custom\n");
		return origMesh;
	}

	if (meshSys->meshesNum > MESHES_MAX-1) {
		logf("Too many meshes\n");
		return NULL;
	}

	Mesh *mesh = &meshSys->meshes[meshSys->meshesNum++];
	memcpy(mesh, origMesh, sizeof(Mesh));
	mesh->id = ++meshSys->nextMeshId;
	mesh->custom = false;

	mesh->vertsIndex = getMeshVertices(mesh->vertsNum);
	for (int i = 0; i < mesh->vertsNum; i++) {
		MeshVertex *vert = &meshSys->verts[mesh->vertsIndex+i];
		MeshVertex *origVert = &mesh->customVerts[i];
		memcpy(vert, origVert, sizeof(MeshVertex));
	}

	mesh->indsIndex = getMeshIndices(mesh->indsNum);
	for (int i = 0; i < mesh->indsNum; i++) {
		meshSys->inds[mesh->indsIndex+i] = mesh->customInds[i];
	}

	return mesh;
}

Mesh *createCustomMesh(Vec3 *verts, Vec2 *uvs, Vec3 *normals, int vertsNum, u16 *inds, int indsNum, MaterialType material, char *diffusePath, bool skipOctree) {
	Mesh *mesh = (Mesh *)zalloc(sizeof(Mesh));
	mesh->id = ++meshSys->nextMeshId;
	mesh->custom = true;
	mesh->material = material;
	mesh->diffusePath = diffusePath;
	mesh->skipOctree = skipOctree;

	mesh->diffuseTexture = getTexture(mesh->diffusePath);

	mesh->customVerts = (MeshVertex *)zalloc(sizeof(MeshVertex) * vertsNum);
	mesh->customInds = (u16 *)zalloc(sizeof(u16) * indsNum);

	for (int i = 0; i < vertsNum; i++) {
		MeshVertex *vert = &mesh->customVerts[mesh->vertsNum++];
		vert->position = verts[i];
		vert->uv = uvs[i];
		vert->normal = normals[i];
		vert->boneWeights[0] = 1;
	}

	for (int i = 0; i < indsNum; i++) {
		((u16 *)mesh->customInds)[mesh->indsNum++] = inds[i];
	}

	glGenBuffers(1, &mesh->vertexBuffer);
	bindVertexBuffer(mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MeshVertex) * mesh->vertsNum, mesh->customVerts, GL_STATIC_DRAW);

	glGenBuffers(1, &mesh->indexBuffer);
	bindElementBuffer(mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16) * mesh->indsNum, mesh->customInds, GL_STATIC_DRAW);

	finalizeMesh(mesh);

	return mesh;
}

void optimizeMesh(Mesh *mesh);
void optimizeMesh(Mesh *mesh) {
	MeshVertex *uniqueVerts = (MeshVertex *)malloc(sizeof(MeshVertex) * mesh->vertsNum);
	int uniqueVertsNum = 0;

	u16 *uniqueInds = (u16 *)malloc(sizeof(u16) * mesh->indsNum);
	int uniqueIndsNum = 0;

	for (int i = 0; i < mesh->indsNum; i++) {
		int currentInd = mesh->customInds[i];
		MeshVertex *currentVert = &mesh->customVerts[currentInd];

		int newInd = -1;
		for (int i = 0; i < uniqueVertsNum; i++) {
			MeshVertex *uniqueVert = &uniqueVerts[i];
			if (memcmp(uniqueVert, currentVert, sizeof(MeshVertex)) == 0) {
				newInd = i;
				break;
			}
		}

		if (newInd == -1) {
			uniqueVerts[uniqueVertsNum++] = *currentVert;
			newInd = uniqueVertsNum-1;
		}

		uniqueInds[uniqueIndsNum++] = newInd;
	}

	free(mesh->customVerts);
	mesh->customVerts = uniqueVerts;
	mesh->vertsNum = uniqueVertsNum;

	free(mesh->customInds);
	mesh->customInds = uniqueInds;
	mesh->indsNum = uniqueIndsNum;
}

void writeMesh(char *path, Mesh *mesh) {
	DataStream *stream = newDataStream();
	writeMesh(stream, mesh);
	writeFile(path, stream->data, stream->index);
	destroyDataStream(stream);
}
void writeMesh(DataStream *stream, Mesh *mesh) {
	writeU8(stream, mesh->version);
	writeString(stream, mesh->name);

	writeU32(stream, mesh->boneNamesNum);
	for (int i = 0; i < mesh->boneNamesNum; i++) {
		writeString(stream, mesh->boneNames[i]);
	}

	writeU32(stream, mesh->vertsNum);
	for (int i = 0; i < mesh->vertsNum; i++) {
		MeshVertex *vert = &mesh->customVerts[i];
		writeVec3(stream, vert->position);
		writeVec2(stream, vert->uv);
		writeVec3(stream, vert->normal);
		writeVec3(stream, vert->tangent);
		writeU8(stream, vert->boneIndices[0]);
		writeU8(stream, vert->boneIndices[1]);
		writeU8(stream, vert->boneIndices[2]);
		writeU8(stream, vert->boneIndices[3]);
		writeFloat(stream, vert->boneWeights[0]);
		writeFloat(stream, vert->boneWeights[1]);
		writeFloat(stream, vert->boneWeights[2]);
		writeFloat(stream, vert->boneWeights[3]);
	}

	writeU32(stream, mesh->indsNum);
	for (int i = 0; i < mesh->indsNum; i++) {
		writeU16(stream, mesh->customInds[i]);
	}

	writeU32(stream, mesh->material);
	writeU8(stream, mesh->backFaceCulled);

	writeString(stream, mesh->diffusePath);
	writeString(stream, mesh->normalPath);
	writeString(stream, mesh->specularPath);
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
	mesh->vertsIndex = getMeshVertices(mesh->vertsNum);
	for (int i = 0; i < mesh->vertsNum; i++) {
		MeshVertex *vert = &meshSys->verts[mesh->vertsIndex+i];
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
	mesh->indsIndex = getMeshIndices(mesh->indsNum);
	for (int i = 0; i < mesh->indsNum; i++) {
		meshSys->inds[mesh->indsIndex+i] = readU16(stream);
	}

	mesh->material = (MaterialType)readU32(stream);
	mesh->backFaceCulled = readU8(stream);

	mesh->diffusePath = readString(stream);
	mesh->normalPath = readString(stream);
	mesh->specularPath = readString(stream);

	if (mesh->diffusePath) {
		char *path = frameSprintf("%s/%s", meshDir, mesh->diffusePath);
		mesh->diffuseTexture = getTexture(path);
		if (!mesh->diffuseTexture) logf("Failed to load diffuse texture: %s\n", path);
	}

	finalizeMesh(mesh);
}

Mesh *createCustomMesh(Mesh *origMesh) {
	Mesh *mesh = (Mesh *)malloc(sizeof(Mesh));
	memcpy(mesh, origMesh, sizeof(Mesh));
	mesh->id = ++meshSys->nextMeshId;

	if (mesh->custom) {
		logf("It's already custom\n");
		return mesh;
	}

	mesh->custom = true;

	MeshVertex *newVerts = (MeshVertex *)malloc(sizeof(MeshVertex) * mesh->vertsNum);
	memcpy(newVerts, &meshSys->verts[mesh->vertsIndex], sizeof(MeshVertex) * mesh->vertsNum);
	mesh->customVerts = newVerts;

	u16 *newInds = (u16 *)malloc(sizeof(u16) * mesh->indsNum);
	memcpy(newInds, &meshSys->inds[mesh->indsIndex], sizeof(u16) * mesh->indsNum);
	mesh->customInds = newInds;

	return mesh;
}

void destroyMesh(Mesh *mesh) {
	process3dDrawQueue();

	if (mesh->custom) {
		free(mesh->customVerts);
		free(mesh->customInds);
		glDeleteBuffers(1, &mesh->indexBuffer);
		glDeleteBuffers(1, &mesh->vertexBuffer);
	} else {
		int indMoveSize = sizeof(u16) * (meshSys->indsNum - (mesh->indsIndex + mesh->indsNum));
		memmove(&meshSys->inds[mesh->indsIndex], &meshSys->inds[mesh->indsIndex+mesh->indsNum], indMoveSize);
		meshSys->indsNum -= mesh->indsNum;

		int vertMoveSize = sizeof(MeshVertex) * (meshSys->vertsNum - (mesh->vertsIndex + mesh->vertsNum));
		memmove(&meshSys->verts[mesh->vertsIndex], &meshSys->verts[mesh->vertsIndex+mesh->vertsNum], vertMoveSize);
		meshSys->vertsNum -= mesh->vertsNum;

		meshSys->gpuVertsDirty = true;

		for (int i = 0; i < meshSys->meshesNum; i++) {
			Mesh *other = &meshSys->meshes[i];
			if (other == mesh) continue;
			if (other->indsIndex > mesh->indsIndex) other->indsIndex -= mesh->indsNum;
			if (other->vertsIndex > mesh->vertsIndex) other->vertsIndex -= mesh->vertsNum;
		}

		if (mesh->diffuseTexture) releaseTexture(mesh->diffuseTexture);

		{
			int meshIndex = -1;
			for (int i = 0; i < meshSys->meshesNum; i++) {
				if (&meshSys->meshes[i] == mesh) meshIndex = i;
			}

			if (meshIndex != -1) {
				arraySpliceIndex(meshSys->meshes, meshSys->meshesNum, sizeof(Mesh), meshIndex);
				meshSys->meshesNum--;
			} else {
				logf("Tried to destoy non-custom mesh, but couldn't find it?\n");
				return;
			}
		}

		reloadAllMeshesForModels();
	}

	// destroyOctree(octree); //@leak
	// I don't think we free the textures, but should we?
}

MeshProps newMeshProps() {
	MeshProps props = {};
	props.alpha = 1;
	props.matrix = mat4();
	props.viewMatrix = defaultWorld->viewMatrix;
	props.projectionMatrix = defaultWorld->projectionMatrix;
	props.ambientLight = defaultWorld->ambientLight;
	props.diffuseLight = defaultWorld->diffuseLight;
	props.specularLight = defaultWorld->specularLight;
	return props;
}

#define RENDER_LAYER_2D 7
#define RENDER_LAYER_MAX 7
void drawMesh(Mesh *mesh, MeshProps props) {
	if (props.alpha == 0) return;
	if (renderer->disabled) return;

	if (props.layer > RENDER_LAYER_MAX) {
		logf("Layer too high\n");
		return;
	}

	if (meshSys->drawQueueNum > DRAW_QUEUE_MAX-1) {
		logf("Too many draw queue items (%d)\n", DRAW_QUEUE_MAX);
		return;
	}

	if (!mesh) {
		logf("Null mesh\n");
		return;
	}

	if (props.skeleton && props.skeleton->time == 0) {
		logf("Drawing un-updated skeleton\n");
	}

	DrawQueueItem *item = &meshSys->drawQueue[meshSys->drawQueueNum++];
	memset(item, 0, sizeof(DrawQueueItem));
	item->type = DRAW_QUEUE_MESH;
	item->mesh = mesh;
	item->sortKey |= props.layer << 29;
	if (props.alpha < 1) {
		item->sortKey |= 1 << 27;
			// printf("Trans key: ");
			// printBinary(item->sortKey);
			// printf("\n");
	}
	item->props = props;

	renderer->in3dPass = true;
}

void drawDiffuseMeshesImm(Mesh *mesh, MeshProps **instanceProps, int instancePropsNum) {
	WorldProps *world = defaultWorld;

	if (mesh->custom) {
		logf("Custom meshes must be drawn with a custom material\n");
		return;
	}


	AnimatedMeshProgram *shader = &meshSys->animatedMeshProgram;
	bindVertexArray(shader->vao);
	setShaderProgram(shader->program);

	if (meshSys->gpuVertsNum != meshSys->vertsNum) meshSys->gpuVertsDirty = true;
	if (meshSys->gpuIndsNum != meshSys->indsNum) meshSys->gpuVertsDirty = true;

	if (meshSys->gpuVertsDirty) {
		meshSys->gpuVertsDirty = false;

		meshSys->gpuVertsNum = meshSys->vertsNum;
		bindVertexBuffer(shader->vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(MeshVertex) * meshSys->vertsNum, meshSys->verts, GL_STATIC_DRAW);

		meshSys->gpuIndsNum = meshSys->indsNum;
		bindElementBuffer(shader->indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16) * meshSys->indsNum, meshSys->inds, GL_STATIC_DRAW);
	}

	if (mesh->diffuseTexture && !meshSys->disableDiffuse) {
		bindTextureSlot(0, mesh->diffuseTexture);
	} else {
		bindTextureSlot(0, renderer->whiteTexture);
	}

	bool usesNormalMap = false;
	if (mesh->normalTexture && !meshSys->disableNormals) {
		usesNormalMap = true;
		bindTextureSlot(1, mesh->normalTexture);
	} else {
		bindTextureSlot(1, renderer->whiteTexture);
	}

	if (mesh->specularTexture && !meshSys->disableSpecular) {
		bindTextureSlot(2, mesh->specularTexture);
	} else {
		bindTextureSlot(2, renderer->whiteTexture);
	}

	for (int i = 0; i < instancePropsNum; i++) {
		int instance = i;
		MeshProps *props = instanceProps[i];

		DiffuseVertexUniformBlock *vertBlock = (DiffuseVertexUniformBlock *)((u8 *)shader->uniformBlock + sizeof(DiffuseVertexUniformBlock)*instance);
		vertBlock->u_model = props->matrix;
		vertBlock->u_viewProjection = props->projectionMatrix * props->viewMatrix;

		Matrix4 shaderBoneMatrices[BONES_MAX];
		int shaderBoneMatricesCount = 1;
		{
			if (props->skeleton) {
				Skeleton *skeleton = props->skeleton;

				int meshToBoneMapping[BONES_MAX];
				for (int i = 0; i < mesh->boneNamesNum; i++) {
					int meshIndex = i;
					for (int i = 0; i < skeleton->base->bonesNum; i++) {
						Bone *bone = &skeleton->base->bones[i];
						int boneIndex = i;
						if (streq(bone->name, mesh->boneNames[meshIndex])) {
							meshToBoneMapping[meshIndex] = boneIndex;
						}
					}
				}

				for (int i = 0; i < skeleton->base->bonesNum; i++) {
					shaderBoneMatrices[i] = skeleton->meshTransforms[meshToBoneMapping[i]];
				}

				shaderBoneMatricesCount = skeleton->base->bonesNum;
			} else {
				if (shaderBoneMatricesCount < mesh->boneNamesNum) shaderBoneMatricesCount = mesh->boneNamesNum;

				for (int i = 0; i < shaderBoneMatricesCount; i++) {
					shaderBoneMatrices[i] = mat4();
				}
			}
		}
		for (int i = 0; i < shaderBoneMatricesCount; i++) vertBlock->u_boneTransforms[i] = shaderBoneMatrices[i].transpose();

		vertBlock->u_lightPos = v4(world->sunPosition);
		vertBlock->u_viewPos = v4(world->viewMatrix.getPosition());
		vertBlock->u_usesNormalMap = usesNormalMap;

		DiffuseFragmentUniformBlock *fragBlock = (DiffuseFragmentUniformBlock *)
			((u8 *)shader->uniformBlock + shader->vertexBlockSize + shader->fragmentBlockPad + sizeof(DiffuseFragmentUniformBlock)*instance);

		fragBlock->u_ambientLight = v4(props->ambientLight);
		fragBlock->u_diffuseLight = v4(props->diffuseLight);
		fragBlock->u_specularLight = v4(props->specularLight);
		fragBlock->u_alpha = v4(
			props->alpha * 0.5, //@todo I have no idea why this is halfed
			0,
			0,
			0
		);
		fragBlock->u_flags = props->flags;
	}

	bindUniformBuffer(shader->uniformBuffer);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, shader->uniformBlockSize, shader->uniformBlock);

	glUniform1i(shader->u_diffuseTexture, 0);
	glUniform1i(shader->u_normalTexture, 1);

	if (instanceProps[0]->alpha != 1) {
		setBackFaceCulling(true);
	} else {
		setBackFaceCulling(mesh->backFaceCulled);
	}

	CheckGlErrorHigh();

	int instanceCount = instancePropsNum;

	glDrawElementsInstancedBaseVertex(
		GL_TRIANGLES,
		mesh->indsNum,
		GL_UNSIGNED_SHORT, 
		(void *)(u64)(mesh->indsIndex * sizeof(u16)),
		instanceCount,
		mesh->vertsIndex
	);

	bindVertexArray(renderer->defaultVao);
	CheckGlErrorHigh();
}

void drawTerrainMeshImm(Mesh *mesh, MeshProps props) {
	WorldProps *world = defaultWorld;

	TerrainProgram *shader = &meshSys->terrainProgram;
	setShaderProgram(shader->program);

	bindVertexBuffer(mesh->vertexBuffer);
	bindElementBuffer(mesh->indexBuffer);

	int vertSize = sizeof(MeshVertex);

	glEnableVertexAttribArray(shader->a_position);
	glVertexAttribPointer(shader->a_position, 3, GL_FLOAT, false, vertSize, (void *)offsetof(MeshVertex, position));

	glEnableVertexAttribArray(shader->a_uv);
	glVertexAttribPointer(shader->a_uv, 2, GL_FLOAT, false, vertSize, (void *)offsetof(MeshVertex, uv));

	glEnableVertexAttribArray(shader->a_normal);
	glVertexAttribPointer(shader->a_normal, 3, GL_FLOAT, false, vertSize, (void *)offsetof(MeshVertex, normal));

	CheckGlErrorHigh();

	glUniformMatrix4fv(shader->u_model, 1, false, props.matrix.data);
	glUniformMatrix4fv(shader->u_view, 1, false, props.viewMatrix.data);
	glUniformMatrix4fv(shader->u_projection, 1, false, props.projectionMatrix.data);
	CheckGlErrorHigh();

	glUniform3f(shader->u_lightPos, world->sunPosition.x, world->sunPosition.y, -world->sunPosition.z);
	glUniform3f(shader->u_ambientLight, props.ambientLight.x, props.ambientLight.y, props.ambientLight.z);
	glUniform3f(shader->u_diffuseLight, props.diffuseLight.x, props.diffuseLight.y, props.diffuseLight.z);
	glUniform3f(shader->u_specularLight, props.specularLight.x, props.specularLight.y, props.specularLight.z);

	Vec3 camPos = world->viewMatrix.getPosition();
	glUniform3f(shader->u_viewPos, camPos.x, camPos.y, camPos.z);

	glUniform1i(shader->u_diffuseTexture, 0);
	bindTextureSlot(0, mesh->diffuseTexture);

	glUniform1i(shader->u_slopeTexture, 1);
	bindTextureSlot(1, props.slopeTexture);

	setBackFaceCulling(mesh->backFaceCulled);

	CheckGlErrorHigh();

	glDrawElements(GL_TRIANGLES, mesh->indsNum, GL_UNSIGNED_SHORT, 0);

	CheckGlErrorHigh();
}

void drawSkyboxImm(Skybox *skybox) {
	WorldProps *world = defaultWorld;

	setShaderProgram(meshSys->skyboxProgram.program);

	bindVertexBuffer(skybox->vertexBuffer);
	bindElementBuffer(skybox->indexBuffer);

	glEnableVertexAttribArray(meshSys->skyboxProgram.a_position);
	glVertexAttribPointer(meshSys->skyboxProgram.a_position, 3, GL_FLOAT, false, 0, 0);

	CheckGlErrorHigh();

	Matrix4 viewMatrix = mat4();
	viewMatrix.ROTATE_QUAT(world->viewMatrix.getQuaternion());
	viewMatrix.ROTATE_EULER(toRad(-90), 0, 0);
	glUniformMatrix4fv(meshSys->skyboxProgram.u_view, 1, false, viewMatrix.data);
	glUniformMatrix4fv(meshSys->skyboxProgram.u_projection, 1, false, world->projectionMatrix.data);

	CheckGlErrorHigh();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->cubeMapId);
	glUniform1i(meshSys->skyboxProgram.u_cubeMap, 0);
	CheckGlErrorHigh();

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
	CheckGlErrorHigh();
}

void setBackFaceCulling(bool enabled) {
	if (enabled && !meshSys->disableCulling) {
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CW);
	} else {
		glDisable(GL_CULL_FACE);
	}
}

int qsortDrawQueue(const void *a, const void *b) {
	DrawQueueItem *itemA = *(DrawQueueItem **)a;
	DrawQueueItem *itemB = *(DrawQueueItem **)b;
	return itemA->sortKey - itemB->sortKey;
}

void process3dDrawQueue() {
	Assert(meshSys);
	// Vec3 cameraPosition = defaultWorld->viewMatrix.invert().getPosition();

	// Was: Layer, Transparent mode, Distance: LLLT TDDD DDDD DDDD DDDD D___ ____ ____
	meshSys->sortedItemsNum = 0;
	for (int i = 0; i < meshSys->drawQueueNum; i++) {
		DrawQueueItem *item = &meshSys->drawQueue[i];
		if (item->mesh) {
			// Vec3 center = item->propsMatrix * getCenter(item->mesh->bounds);
			// float dist = center.distance(cameraPosition);
			// u16 shortDist;
			// if (item->props.alpha < 1) {
			// 	shortDist = clampMap(dist, 0, FAR_PLANE, pow(2, 16), 0);
			// } else {
			// 	shortDist = clampMap(dist, 0, FAR_PLANE, 0, pow(2, 16));
			// }
			// item->sortKey |= shortDist << 11;
			item->sortKey |= item->mesh->globalArrayIndexPlusOne << 11;
		}
		meshSys->sortedItems[meshSys->sortedItemsNum++] = item;
	}

	meshSys->lastDrawCalls = meshSys->sortedItemsNum;

	qsort(meshSys->sortedItems, meshSys->sortedItemsNum, sizeof(DrawQueueItem *), qsortDrawQueue);

	renderer->in3dPass = true;
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

	int sortedItemIndex = 0;
	int prevLayer = 0;
	int prevTransparentMode = 0;
	for (;;) {
		if (sortedItemIndex > meshSys->sortedItemsNum-1) break;

		DrawQueueItem *item = meshSys->sortedItems[sortedItemIndex++];

		u32 layer = item->sortKey >> 29;
		u32 transparentMode = (item->sortKey >> 27) & 0x00000003;
		if (prevLayer != layer) {
			prevLayer = layer;
			glClear(GL_DEPTH_BUFFER_BIT);
		}

		if (prevTransparentMode != transparentMode) {
			prevTransparentMode = transparentMode;

			if (transparentMode) {
				glEnable(GL_BLEND);
			} else {
				glDisable(GL_BLEND);
			}
		}

		if (item->type == DRAW_QUEUE_MESH) {
			if (item->mesh->material == MATERIAL_DIFFUSE && !item->mesh->custom) {
				int nextInstanceIndex = sortedItemIndex;
				MeshProps *instanceProps[DIFFUSE_INSTANCES_MAX];
				int instancePropsNum = 0;

				instanceProps[instancePropsNum++] = &item->props;
				for (;;) {
					if (nextInstanceIndex > meshSys->sortedItemsNum-1) break;
					if (instancePropsNum > DIFFUSE_INSTANCES_MAX-1) break;
					if (meshSys->disableInstancing && instancePropsNum > 0) break;
					DrawQueueItem *nextItem = meshSys->sortedItems[nextInstanceIndex++];
					if (nextItem->type == DRAW_QUEUE_MESH && nextItem->mesh->material == MATERIAL_DIFFUSE && nextItem->mesh == item->mesh) {
						instanceProps[instancePropsNum] = &nextItem->props;
						instancePropsNum++;
					} else {
						break;
					}
				}
				sortedItemIndex += instancePropsNum-1;
				// if (instancePropsNum > 1) logf("frame %d: %d insances of %s\n", platform->frameCount, instancePropsNum, item->mesh->path);
				drawDiffuseMeshesImm(item->mesh, instanceProps, instancePropsNum);
			} else if (item->mesh->material == MATERIAL_DIFFUSE && item->mesh->custom) {
				logf("No...\n");
			} else if (item->mesh->material == MATERIAL_TERRAIN_MAP) {
				drawTerrainMeshImm(item->mesh, item->props);
			} else {
				logf("Unknown material\n");
			}
		} else {
			logf("Unknown DrawQueueItemType\n");
		}
	}

	meshSys->drawQueueNum = 0;
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	renderer->in3dPass = false;
}

int getMeshVertices(int count) {
	int vertsNeeded = meshSys->vertsNum + count;
	if (vertsNeeded > meshSys->vertsMax-1) {
		meshSys->verts = (MeshVertex *)resizeArray(meshSys->verts, sizeof(MeshVertex), meshSys->vertsMax, vertsNeeded);
		meshSys->vertsMax = vertsNeeded;
	}

	int index = meshSys->vertsNum;
	meshSys->vertsNum += count;
	return index;
}

int getMeshIndices(int count) {
	int indsNeeded = meshSys->indsNum + count;
	if (indsNeeded > meshSys->indsMax-1) {
		meshSys->inds = (u16 *)resizeArray(meshSys->inds, sizeof(u16), meshSys->indsMax, indsNeeded);
		meshSys->indsMax = indsNeeded;
	}

	int index = meshSys->indsNum;
	meshSys->indsNum += count;
	return index;
}

void insert(TriOctree *octree, MeshTri *meshTri);
void insert(TriOctree *octree, MeshTri *meshTri) {
	bool isInside = false;
	Tri tri = meshTri->tri;
	if (contains(octree->bounds, tri.verts[0])) isInside = true;
	if (!isInside && contains(octree->bounds, tri.verts[1])) isInside = true;
	if (!isInside && contains(octree->bounds, tri.verts[2])) isInside = true;
	if (!isInside && intersects(octree->bounds, tri)) isInside = true;

	if (!isInside) return;

	if (octree->meshTrisNum > TRI_OCTREE_TRIS_MAX-1) {
		if (!octree->subdivided) {
			octree->subdivided = true;
			octree->children = (TriOctree *)zalloc(sizeof(TriOctree) * 8);

			AABB bounds = octree->bounds;
			Vec3 size = getSize(bounds);
			bounds.max = bounds.min + size/2;

			size = getSize(bounds);

			AABB firstBounds = bounds;
			octree->children[0].bounds = bounds;

			bounds.min.x += size.x;
			bounds.max.x += size.x;
			octree->children[1].bounds = bounds;

			bounds = firstBounds;
			bounds.min.y += size.y;
			bounds.max.y += size.y;
			octree->children[2].bounds = bounds;

			bounds = firstBounds;
			bounds.min.x += size.x;
			bounds.max.x += size.x;
			bounds.min.y += size.y;
			bounds.max.y += size.y;
			octree->children[3].bounds = bounds;

			bounds = firstBounds;
			bounds.min.z += size.z;
			bounds.max.z += size.z;
			octree->children[4].bounds = bounds;

			bounds = firstBounds;
			bounds.min.x += size.x;
			bounds.max.x += size.x;
			bounds.min.z += size.z;
			bounds.max.z += size.z;
			octree->children[5].bounds = bounds;

			bounds = firstBounds;
			bounds.min.y += size.y;
			bounds.max.y += size.y;
			bounds.min.z += size.z;
			bounds.max.z += size.z;
			octree->children[6].bounds = bounds;

			bounds = firstBounds;
			bounds.min.x += size.x;
			bounds.max.x += size.x;
			bounds.min.y += size.y;
			bounds.max.y += size.y;
			bounds.min.z += size.z;
			bounds.max.z += size.z;
			octree->children[7].bounds = bounds;
		}

		for (int i = 0; i < 8; i++) {
			insert(&octree->children[i], meshTri);
		}
	} else {
		octree->meshTris[octree->meshTrisNum++] = meshTri;
	}
}

void generateOctree(Mesh *mesh) {
	if (mesh->octree) {
		logf("Already has octree\n");
		return;
	}

	mesh->octree = (TriOctree *)zalloc(sizeof(TriOctree));
	mesh->octree->bounds = mesh->bounds;

	for (int i = 0; i < mesh->meshTrisNum; i++) {
		insert(mesh->octree, &mesh->meshTris[i]);
	}
}

void drawTriOctree(TriOctree *octree, Matrix4 matrix=mat4());
void drawTriOctree(TriOctree *octree, Matrix4 matrix) {
	drawBoundsOutline(octree->bounds, 0.01, 0xFFFFFFFF, matrix);
	if (octree->subdivided) {
		for (int i = 0; i < 8; i++) {
			drawTriOctree(&octree->children[i], matrix);
		}
	}
}

bool intersectsLine(Mesh *mesh, Vec3 start, Vec3 end, float *outDist, Vec2 *outUv, MeshTri **outMeshTri) {
	Vec3 dir = (end - start).normalize();
	bool ret = intersectsRay(mesh->octree, start, dir, outDist, outUv, outMeshTri);
	return ret;
}

bool intersectsRay(TriOctree *octree, Vec3 start, Vec3 dir, float *outDist, Vec2 *outUv, MeshTri **outMeshTri) {
	float closestHit = -1;
	Vec2 closestUv;
	MeshTri *closestMeshTri;

	Panic("AABBLineIntersects was renamed to something else");
	// if (!AABBLineIntersects(octree->bounds, start, start + dir*9999)) return false;

	for (int i = 0; i < octree->meshTrisNum; i++) {
		MeshTri *meshTri = octree->meshTris[i];
		float rayDist;
		Vec2 uv;
		if (rayIntersectsTriangle(start, dir, meshTri->tri, &rayDist, &uv)) {
			if (closestHit == -1 || rayDist < closestHit) {
				closestHit = rayDist;
				closestUv = uv;
				closestMeshTri = meshTri;
			}
		}
	}

	if (octree->subdivided) {
		for (int i = 0; i < 8; i++) {
			float rayDist;
			Vec2 uv;
			MeshTri *meshTri;
			if (intersectsRay(&octree->children[i], start, dir, &rayDist, &uv, &meshTri)) {
				if (closestHit == -1 || rayDist < closestHit) {
					closestHit = rayDist;
					closestUv = uv;
					closestMeshTri = meshTri;
				}
			}
		}
	}

	if (closestHit == -1) {
		return false;
	} else {
		*outDist = closestHit;
		*outUv = closestUv;
		*outMeshTri = closestMeshTri;
		return true;
	}
}

void intersects(Mesh *mesh, Capsule3 cap, IntersectionResult *outResults, int *outResultsNum, int outResultsMax) {
	AABB bounds = toAABB(cap);

	MeshTri **meshTris = (MeshTri **)frameMalloc(sizeof(MeshTri *) * outResultsMax);
	int meshTrisNum = 0;
	getTriangles(mesh->octree, bounds, meshTris, &meshTrisNum, outResultsMax);
	for (int i = 0; i < meshTrisNum; i++) {
		MeshTri *meshTri = meshTris[i];
		IntersectionResult result = intersects(cap, meshTri->tri);
		if (result.length == -1) continue;
		if (result.penetrationDepth <= 0) continue;
		if (*outResultsNum > outResultsMax-1) {
			logf("Too many outResults for mesh intersects capsule\n");
			return;
		}

		outResults[*outResultsNum] = result;
		*outResultsNum = *outResultsNum + 1;
	}
}

void getTriangles(TriOctree *octree, AABB bounds, MeshTri **outMeshTris, int *outMeshTrisNum, int outMeshTrisMax, int depth) {
	if (!intersects(bounds, octree->bounds)) return;

	for (int i = 0; i < octree->meshTrisNum; i++) {
		if (intersects(bounds, octree->meshTris[i]->tri)) {
			if (*outMeshTrisNum > outMeshTrisMax-1) {
				logf("Too many outMeshTris in getTriangles\n");
			} else {
				outMeshTris[*outMeshTrisNum] = octree->meshTris[i];
				*outMeshTrisNum = *outMeshTrisNum + 1;
			}
		}
	}

	if (octree->subdivided) {
		for (int i = 0; i < 8; i++) {
			getTriangles(&octree->children[i], bounds, outMeshTris, outMeshTrisNum, outMeshTrisMax, depth+1);
		}
	}

	if (depth == 0) {
		MeshTri **newOutMeshTris = (MeshTri **)zalloc(sizeof(MeshTri *) * outMeshTrisMax);
		int newOutMeshTrisNum = 0;

		for (int i = 0; i < *outMeshTrisNum; i++) {
			MeshTri *meshTri = outMeshTris[i];

			bool found = false;
			for (int i = 0; i < newOutMeshTrisNum; i++) {
				MeshTri *newMeshTri = newOutMeshTris[i];
				if (equal(meshTri->tri, newMeshTri->tri)) found = true;
			}

			if (!found) newOutMeshTris[newOutMeshTrisNum++] = meshTri;
		}

		for (int i = 0; i < newOutMeshTrisNum; i++) {
			outMeshTris[i] = newOutMeshTris[i];
		}

		*outMeshTrisNum = newOutMeshTrisNum;
	}
}

void finalizeMesh(Mesh *mesh) {
	mesh->bounds.min = v3(99999, 99999, 99999);
	mesh->bounds.max = v3(-99999, -99999, -99999);

	for (int i = 0; i < mesh->vertsNum; i++) {
		Vec3 pos;
		if (mesh->custom) {
			pos = mesh->customVerts[i].position;
		} else {
			pos = meshSys->verts[mesh->vertsIndex+i].position;
		}

		mesh->bounds = expand(mesh->bounds, pos);
	}

	if (mesh->meshTris) {
		logf("Finalizing mesh that already has meshTris!\n");
		return;
	}

	u16 *inds = NULL;
	int indsNum = mesh->indsNum;

	MeshVertex *verts = NULL;
	int vertsNum = mesh->vertsNum;
	if (mesh->custom) {
		inds = mesh->customInds;
		verts = mesh->customVerts;
	} else {
		inds = &meshSys->inds[mesh->indsIndex];
		verts = &meshSys->verts[mesh->vertsIndex];
	}


	mesh->meshTris = (MeshTri *)zalloc(sizeof(MeshTri) * mesh->indsNum/3);
	for (int i = 0; i < mesh->indsNum/3; i++) {
		MeshTri *meshTri = &mesh->meshTris[mesh->meshTrisNum++];
		// meshTri->triIndex = i;
		meshTri->verts[0] = verts[inds[i*3 + 0]];
		meshTri->verts[1] = verts[inds[i*3 + 1]];
		meshTri->verts[2] = verts[inds[i*3 + 2]];
		meshTri->tri.verts[0] = meshTri->verts[0].position;
		meshTri->tri.verts[1] = meshTri->verts[1].position;
		meshTri->tri.verts[2] = meshTri->verts[2].position;
	}

	if (!mesh->skipOctree) generateOctree(mesh);
}

#define DEBUG_DEPTH_WIDTH 128
#define DEBUG_DEPTH_HEIGHT 128
void showMeshGui() {
	if (!meshSys->debugFramebuffer) {
		meshSys->debugFramebuffer = createFramebuffer();

		setFramebuffer(meshSys->debugFramebuffer);
		addDepthAttachment(DEBUG_DEPTH_WIDTH, DEBUG_DEPTH_HEIGHT); // Does setting these too high even matter?
		setFramebuffer(NULL);
	}

	if (!meshSys->debugMeshTexture) meshSys->debugMeshTexture = createTexture(128, 128);

	int totalVerts = 0;
	int totalInds = 0;
	for (int i = 0; i < meshSys->meshesNum; i++) {
		Mesh *mesh = &meshSys->meshes[i];
		totalVerts += mesh->vertsNum;
		totalInds += mesh->indsNum;
	}
	ImGui::Checkbox("Show mesh bounds", &meshSys->debugShowMeshBounds);
	ImGui::Text("Total verts: %d", totalVerts);
	ImGui::Text("Total inds: %d", totalInds);
	if (ImGui::Button("Optimize All")) {
		for (int i = 0; i < meshSys->meshesNum; i++) {
			Mesh *mesh = &meshSys->meshes[i];
			logf("%s\n", mesh->path);

			Mesh *newMesh = createCustomMesh(mesh);

			optimizeMesh(newMesh);

			writeMesh(mesh->path, newMesh);

			destroyMesh(newMesh);
		}
	}

	ImGui::BeginChild("Mesh list child", ImVec2(400, 500), true, 0); 
	for (int i = 0; i < meshSys->meshesNum; i++) {
		Mesh *mesh = &meshSys->meshes[i];
		char *label = frameSprintf("%s###%d", mesh->name, i);
		if (ImGui::Selectable(label, meshSys->debugSelectedMesh == i)) {
			meshSys->debugSelectedMesh = i;
		}
	}
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("Mesh child", ImVec2(400, 500), true, 0); 
	Mesh *mesh = &meshSys->meshes[meshSys->debugSelectedMesh];

	ImGui::Text("Name: %s", mesh->name);
	ImGui::Text("Path: %s", mesh->path);
	ImGui::Text("vertsNum: %d (index: %d)", mesh->vertsNum, mesh->vertsIndex);
	ImGui::Text("indsNum: %d (index: %d)", mesh->indsNum, mesh->indsIndex);
	ImGui::Text("Diffuse texture: %s", mesh->diffusePath);
	ImGui::Text(
		"Bounds: (%.1f,%.1f,%.1f) (%.1f,%.1f,%.1f)",
		mesh->bounds.min.x,
		mesh->bounds.min.y,
		mesh->bounds.min.z,
		mesh->bounds.max.x,
		mesh->bounds.max.y,
		mesh->bounds.max.z
	);
	Vec3 size = getSize(mesh->bounds);
	Vec3 center = getCenter(mesh->bounds);
	ImGui::Text("Size: %.1f, %.1f, %.1f", size.x, size.y, size.z);
	ImGui::Text("Center: %.1f, %.1f, %.1f", center.x, center.y, center.z);

	{
		WorldProps oldWorld = *defaultWorld;

		float dist = getSize(mesh->bounds).length();
		Vec3 center = getCenter(mesh->bounds);

		Vec3 camPos;
		camPos.x = cos(platform->time) * dist;
		camPos.y = sin(platform->time) * dist;
		camPos.z = dist;
		defaultWorld->viewMatrix = lookAt(center + camPos, center).invert();

		float aspect = (float)meshSys->debugMeshTexture->width/meshSys->debugMeshTexture->height;
		defaultWorld->projectionMatrix = getPerspectiveMatrix(60, aspect, 0.1, FAR_PLANE);

		Framebuffer *oldFramebuffer = renderer->currentFramebuffer;
		setFramebuffer(meshSys->debugFramebuffer);
		setColorAttachment(meshSys->debugFramebuffer, meshSys->debugMeshTexture, 0);

		clearRenderer();
		MeshProps props = newMeshProps();
		drawMesh(mesh, props);
		if (meshSys->debugShowMeshBounds) drawBoundsOutline(mesh->bounds, 0.01*dist, 0xFFFF0000);
		process3dDrawQueue();

		*defaultWorld = oldWorld;
		setFramebuffer(oldFramebuffer);
	}

	guiTexture(meshSys->debugMeshTexture);

	if (ImGui::Button("Destroy")) {
		destroyMesh(mesh);
	}

	ImGui::EndChild();
}

/// Mesh end
#include "draw3dUtils.cpp"
