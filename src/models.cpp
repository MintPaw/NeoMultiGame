struct Model {
	char version;
	char *name;

	Matrix4 modelMatrix;
	Matrix4 localMatrix;
	char *meshPath;

	Model *children;
	int childrenNum;

	/// Unserialized
	char *path;
	int childrenMax;
	Mesh *mesh;
	AABB bounds;
	Matrix4 invMatrix;
};

struct ModelSystem {
#define MODELS_MAX 512
	Model models[MODELS_MAX];
	int modelsNum;

	// Framebuffer *debugFramebuffer;
	Texture *debugModelTexture;
	bool debugShowModelBounds;
	int debugSelectedModel;
};

ModelSystem *modelSys = NULL;

void initModel();
Model *getModel(char *path);

void readModel(DataStream *stream, Model *model, char *modelDir);
void computeBounds(Model *model, Matrix4 matrix=mat4());
void writeModel(DataStream *stream, Model *model);

void drawModel(Model *model, Matrix4 matrix, Skeleton *skeleton=NULL, Model *parent=NULL);

// bool intersectsLine(Model *model, Vec3 start, Vec3 end, float *outDist, Vec2 *outUv, MeshTri **outMeshTri);
void showModelsGui();

// void reloadAllMeshesForModels(); // Actually declared in draw3d.cpp
void reloadMeshes(Model *model, char *modelDir=NULL);

void initModel() {
	modelSys = (ModelSystem *)zalloc(sizeof(ModelSystem));
}

Model *getModel(char *path) {
	if (!path) return NULL;
	if (path[0] == 0) return NULL;

	for (int i = 0; i < modelSys->modelsNum; i++) {
		Model *model = &modelSys->models[i];
		if (streq(model->path, path)) return model;
	}

	if (!fileExists(path)) {
		logf("No model at %s\n", path);
		return NULL;
	}

	if (modelSys->modelsNum > MODELS_MAX-1) {
		logf("Too many models\n");
		return NULL;
	}

	Model *model = &modelSys->models[modelSys->modelsNum++];
	memset(model, 0, sizeof(Model));
	model->path = stringClone(path);

	char *dir = frameMalloc(PATH_MAX_LEN);
	strcpy(dir, path);
	char *lastSlash = strrchr(dir, '/');
	if (lastSlash) *lastSlash = 0;

	DataStream *stream = loadDataStream(path);
	readModel(stream, model, dir);
	destroyDataStream(stream);

	computeBounds(model);
	return model;
}

void readModel(DataStream *stream, Model *model, char *modelDir) {
	model->version = readU8(stream);
	model->name = readString(stream);

	model->localMatrix = readMatrix4(stream);
	model->invMatrix = model->localMatrix.invert();
	model->meshPath = readString(stream);
	if (model->meshPath) {
		char *realMeshPath = frameSprintf("%s/%s.mesh", modelDir, model->meshPath);
		model->mesh = getMesh(realMeshPath);
		if (!model->mesh) logf("Failed to load mesh %s\n", realMeshPath);
	}

	model->childrenNum = readU32(stream);
	model->childrenMax = model->childrenNum;
	model->children = (Model *)zalloc(sizeof(Model) * model->childrenMax);
	for (int i = 0; i < model->childrenNum; i++) {
		Model *nextModel = &model->children[i];
		readModel(stream, nextModel, modelDir);
	}
}

void computeBounds(Model *model, Matrix4 matrix) {
	model->bounds.min = v3(99999, 99999, 99999);
	model->bounds.max = v3(-99999, -99999, -99999);

	matrix = model->localMatrix * matrix;
	model->modelMatrix = matrix;

	for (int i = 0; i < model->childrenNum; i++) {
		Model *child = &model->children[i];
		computeBounds(child, matrix);

		model->bounds = expand(model->bounds, child->bounds);
	}

	if (model->mesh) {
		Vec3 verts[8];
		getVerts(model->mesh->bounds, verts);
		for (int i = 0; i < 8; i++) verts[i] = matrix * verts[i];
		for (int i = 0; i < 8; i++) model->bounds = expand(model->bounds, verts[i]);
	}
}

void writeModel(DataStream *stream, Model *model) {
	writeU8(stream, model->version);
	writeString(stream, model->name);

	writeMatrix4(stream, model->localMatrix);
	writeString(stream, model->meshPath);

	writeU32(stream, model->childrenNum);
	for (int i = 0; i < model->childrenNum; i++) {
		Model *nextModel = &model->children[i];
		writeModel(stream, nextModel);
	}
}

void drawModel(Model *model, Matrix4 matrix, Skeleton *skeleton, Model *parent) {
	if (!model) {
		logf("Called drawModel with NULL model\n");
		return;
	}

	// if (!parent) matrix = matrix * modelProps.matrix;
	// matrix = matrix * model->localMatrix;

	if (model->mesh) {
		Matrix4 matrix = model->modelMatrix * matrix;
		drawMesh(model->mesh, matrix, skeleton);
	}

	for (int i = 0; i < model->childrenNum; i++) {
		Model *child = &model->children[i];
		drawModel(child, matrix, skeleton, model);
	}
}

// bool intersectsLine(Model *model, Vec3 start, Vec3 end, float *outDist, Vec2 *outUv, MeshTri **outMeshTri) {
// 	start = model->invMatrix * start;
// 	end = model->invMatrix * end;

// 	float closestHit = -1;
// 	Vec2 closestUv;
// 	MeshTri *closestMeshTri;

// 	if (model->mesh) {
// 		float rayDist;
// 		Vec2 uv;
// 		MeshTri *meshTri;
// 		if (intersectsLine(model->mesh, start, end, &rayDist, &uv, &meshTri)) {
// 			if (closestHit == -1 || rayDist < closestHit) {
// 				closestHit = rayDist;
// 				closestUv = uv;
// 				closestMeshTri = meshTri;
// 			}
// 		}
// 	}

// 	for (int i = 0; i < model->childrenNum; i++) {
// 		float rayDist;
// 		Vec2 uv;
// 		MeshTri *meshTri;
// 		if (intersectsLine(&model->children[i], start, end, &rayDist, &uv, &meshTri)) {
// 			if (closestHit == -1 || rayDist < closestHit) {
// 				closestHit = rayDist;
// 				closestUv = uv;
// 				closestMeshTri = meshTri;
// 			}
// 		}
// 	}

// 	if (closestHit == -1) {
// 		return false;
// 	} else {
// 		*outDist = closestHit;
// 		*outUv = closestUv;
// 		*outMeshTri = closestMeshTri;
// 		return true;
// 	}
// }

// void intersects(Model *model, Capsule cap, IntersectionResult *outResults, int *outResultsNum, int outResultsMax, Matrix4 matrix=mat4());
// void intersects(Model *model, Capsule cap, IntersectionResult *outResults, int *outResultsNum, int outResultsMax, Matrix4 matrix) {
// 	// cap = model->invMatrix * cap;
// 	matrix = model->matrix * matrix;
// 	Matrix4 inv = matrix.invert();
// 	cap.start = inv * cap.start;
// 	cap.end = inv * cap.end;

// 	float closestHit = -1;

// 	if (model->mesh) {
// 		int prevOutResultsNum = *outResultsNum;
// 		intersects(model->mesh, cap, outResults, outResultsNum, outResultsMax);
// 		for (int i = prevOutResultsNum; i < *outResultsNum; i++) {
// 			IntersectionResult *result = &outResults[i];
// 			result->penetrationNormal = inv * result->penetrationNormal;
// 		}
// 	}

// 	for (int i = 0; i < model->childrenNum; i++) {
// 		intersects(&model->children[i], cap, outResults, outResultsNum, outResultsMax);
// 	}
// }

void reloadAllMeshesForModels() {
	for (int i = 0; i < modelSys->modelsNum; i++) { // Maybe just set meshes to NULL and reload them during drawModel?
		Model *model = &modelSys->models[i];
		reloadMeshes(model);
	}
}

void reloadMeshes(Model *model, char *modelDir) {
	if (!modelDir) {
		modelDir = frameStringClone(model->path);
		char *lastSlash = strrchr(modelDir, '/');
		if (lastSlash) *lastSlash = 0;
	}
	if (model->mesh) {
		char *realMeshPath = frameSprintf("%s/%s.mesh", modelDir, model->meshPath);
		model->mesh = getMesh(realMeshPath);
	}

	for (int i = 0; i < model->childrenNum; i++) {
		reloadMeshes(&model->children[i], modelDir);
	}
}

// void showModelsGui() {
// 	if (!modelSys->debugFramebuffer) {
// 		modelSys->debugFramebuffer = createFramebuffer();

// 		setFramebuffer(modelSys->debugFramebuffer);
// 		addDepthAttachment(DEBUG_DEPTH_WIDTH, DEBUG_DEPTH_HEIGHT);
// 		setFramebuffer(NULL);
// 	}

// 	if (!modelSys->debugModelTexture) modelSys->debugModelTexture = createTexture(128, 128);

// 	ImGui::Checkbox("Show mesh bounds", &modelSys->debugShowModelBounds);

// 	ImGui::BeginChild("Model list child", ImVec2(400, 500), true, 0); 
// 	for (int i = 0; i < modelSys->modelsNum; i++) {
// 		Model *model = &modelSys->models[i];
// 		char *label = frameSprintf("%s###%d", model->name, i);
// 		if (ImGui::Selectable(label, modelSys->debugSelectedModel == i)) {
// 			modelSys->debugSelectedModel = i;
// 		}
// 	}
// 	ImGui::EndChild();

// 	ImGui::SameLine();

// 	ImGui::BeginChild("Model child", ImVec2(400, 500), true, 0); 
// 	Model *model = &modelSys->models[modelSys->debugSelectedModel];

// 	ImGui::Text("Name: %s", model->name);
// 	ImGui::Text("Path: %s", model->path);
// 	ImGui::Text(
// 		"Bounds: (%.1f,%.1f,%.1f) (%.1f,%.1f,%.1f)",
// 		model->bounds.min.x,
// 		model->bounds.min.y,
// 		model->bounds.min.z,
// 		model->bounds.max.x,
// 		model->bounds.max.y,
// 		model->bounds.max.z
// 	);

// 	{
// 		WorldProps oldWorld = *defaultWorld;

// 		float dist = getSize(model->bounds).length();
// 		Vec3 center = getCenter(model->bounds);

// 		Vec3 camPos;
// 		camPos.x = cos(platform->time) * dist;
// 		camPos.y = sin(platform->time) * dist;
// 		camPos.z = dist;
// 		defaultWorld->viewMatrix = lookAt(center + camPos, center).invert();

// 		float aspect = (float)modelSys->debugModelTexture->width/modelSys->debugModelTexture->height;
// 		defaultWorld->projectionMatrix = getPerspectiveMatrix(60, aspect, 0.1, FAR_PLANE);

// 		Framebuffer *oldFramebuffer = renderer->currentFramebuffer;
// 		setFramebuffer(modelSys->debugFramebuffer);
// 		setColorAttachment(modelSys->debugFramebuffer, modelSys->debugModelTexture, 0);

// 		clearRenderer();
// 		ModelProps props = newModelProps();
// 		drawModel(model, props);
// 		if (modelSys->debugShowModelBounds) drawBoundsOutline(model->bounds, 0.01*dist, 0xFFFF0000);
// 		process3dDrawQueue();

// 		*defaultWorld = oldWorld;
// 		setFramebuffer(oldFramebuffer);
// 	}

// 	guiTexture(modelSys->debugModelTexture);

// 	if (ImGui::Button("Destroy")) {
// 		// destroyModel(model);
// 	}

// 	ImGui::EndChild();
// }
