struct Texture {
	BackendTexture backendTexture;
	int width;
	int height;

	bool smooth;
	bool clampedX;
	bool clampedY;
	char *path;
};

struct RenderProps {
	bool disabled;

	Matrix3 matrix;
	float alpha;
	int tint;

	// int srcWidth;
	// int srcHeight;

#define _F_CIRCLE (1 << 6)
	u32 flags;

	Vec2 uv0;
	Vec2 uv1;
	Matrix3 uvMatrix;
};

struct Shader {
	int mvpLoc;
	int resolutionLoc;
	BackendShader backendShader;
};

struct Renderer {
	bool disabled;

  Vec2 scissorScale;

	Shader *defaultShader;

	Shader *outlineShader;
	int outlineShaderResolutionLoc;
	int outlineShaderOutlineSizeLoc;
	int outlineShaderOutlineColorLoc;
	int outlineShaderOutlineFadeOuterLoc;
	int outlineShaderOutlineFadeInnerLoc;

	Shader *currentShader;
	BlendMode currentBlendMode;
	bool currentDepthTest;
	bool currentDepthMask;
	bool currentBlending;
	bool currentBackfaceCulling;
#define TEXTURE_SLOTS_MAX 8
	Texture *textureSlots[TEXTURE_SLOTS_MAX];

#define TARGET_TEXTURE_LIMIT 16
	Texture *targetTextureStack[TARGET_TEXTURE_LIMIT];
	int targetTextureStackNum;

#define CAMERA_2D_STACK_MAX 128
	Matrix3 camera2dStack[CAMERA_2D_STACK_MAX];
	int camera2dStackNum;
	Matrix3 baseMatrix2d;

#define COLOR_STACK_MAX 128
	int colorStack[COLOR_STACK_MAX];
	int colorStackNum;
	int currentColor;

	Texture *whiteTexture;
	Texture *circleTexture1024;
	Texture *circleTexture32;
	Texture *linearGrad256;

#define FRAME_TEXTURES_MAX 512
	Texture *frameTextures[FRAME_TEXTURES_MAX];
	int frameTexturesNum;

	float current2dDrawDepth;
	int currentDrawCount;

#define BATCH_VERTS_MAX 6000
	GpuVertex batchVerts[BATCH_VERTS_MAX];
	int batchVertsNum;
	int batchTextureSlotsNum;

	Allocator textureMapAllocator;
	HashMap *textureMap;
};

Renderer *renderer = NULL;

void initRenderer();
void startRenderingFrame();
void endRenderingFrame();
void clearRenderer(int color=0);

Shader *loadShader(char *vsPath, char *fsPath);
int getUniformLocation(Shader *shader, char *uniformName);
void setShaderUniform(Shader *shader, int loc, float value);
void setShaderUniform(Shader *shader, int loc, Vec2 value);
void setShaderUniform(Shader *shader, int loc, Vec3 value);
void setShaderUniform(Shader *shader, int loc, Vec4 value);
void setShaderUniform(Shader *shader, int loc, Matrix4 value);
void setShaderUniform(Shader *shader, int loc, Texture *texture);

Texture *createTexture(const char *path, int flags=0);
Texture *createTexture(int width, int height, void *data=NULL, int flags=0);
Texture *createFrameTexture(int width, int height, void *data=NULL, int flags=0);

void setTextureSmooth(Texture *texture, bool smooth);
void setTextureClampedX(Texture *texture, bool clamped);
void setTextureClampedY(Texture *texture, bool clamped);

void setTextureData(Texture *texture, void *data, int width, int height, int flags=0);
u8 *getTextureData(Texture *texture, int flags=0);
bool writeTextureToFile(Texture *texture, char *path);

Texture *getTexture(char *path, int flags=0);
void destroyTexture(Texture *texture);
void destroy(Texture *texture) { destroyTexture(texture); }
void clearTextureCache();

void drawTexture(Texture *texture, RenderProps props);
void drawSimpleTexture(Texture *texture);
void drawSimpleTexture(Texture *texture, Matrix3 matrix, Vec2 uv0=v2(0, 0), Vec2 uv1=v2(1, 1), float alpha=1);
void drawRect(Rect rect, int color);
void drawCircle(Circle circle, int color);

void drawQuadVerts(Texture *texture, Matrix3 matrix, Vec2 uv0=v2(0, 0), Vec2 uv1=v2(1, 1), int tint=0xFFFFFFFF, int flags=0);
void drawVerts(Vec2 *verts, Vec2 *uvs, int *colors, int vertsNum, Texture *texture);
Matrix3 getCurrentVertexMatrix();
void drawVerts(Vec3 *verts, Vec2 *uvs, int *colors, int vertsNum, Texture *texture);
void drawVerts(GpuVertex *verts, int vertsNum);

void processBatchDraws();

void pushTargetTexture(Texture *renderTexture);
void popTargetTexture();

void pushCamera2d(Matrix3 mat);
void popCamera2d();

void refreshGlobalMatrices();

void setScissor(Rect rect);
void clearScissor();

void pushColor(int color);
void pushColor(Vec4 color) { pushColor(argbToHex(color)); }
void popColor();

void setRendererBlendMode(BlendMode blendMode);
void setDepthTest(bool enabled);
void setDepthMask(bool enabled);
void setBlending(bool enabled);
void setBackfaceCulling(bool enabled);

void setShader(Shader *shader);
void setTexture(Texture *texture, int slot=0);

void resetRenderContext();

void imGuiTexture(Texture *texture, Vec2 scale=v2(1, 1));
bool imGuiImageButton(Texture *texture);

#include "rendererUtils.cpp"

void initRenderer() {
	renderer = (Renderer *)zalloc(sizeof(Renderer));

	backendInit();

	setBlending(true);
	setRendererBlendMode(BLEND_NORMAL);
	setDepthMask(true);
	pushColor(0xFFFFFFFF);
	renderer->scissorScale = v2(1, 1);

	{ /// Setup shaders
		renderer->defaultShader = loadShader(NULL, NULL);

		renderer->outlineShader = loadShader(NULL, "assets/common/shaders/outline.fs");
		renderer->outlineShaderResolutionLoc = getUniformLocation(renderer->outlineShader, "resolution");
		renderer->outlineShaderOutlineSizeLoc = getUniformLocation(renderer->outlineShader, "outlineSize");
		renderer->outlineShaderOutlineColorLoc = getUniformLocation(renderer->outlineShader, "outlineColor");
		renderer->outlineShaderOutlineFadeOuterLoc = getUniformLocation(renderer->outlineShader, "outlineFadeOuter");
		renderer->outlineShaderOutlineFadeInnerLoc = getUniformLocation(renderer->outlineShader, "outlineFadeInner");
	} ///
	setShader(NULL);

	u64 whiteData = 0xFFFFFFFF;
	renderer->whiteTexture = createTexture(1, 1, &whiteData);
	renderer->circleTexture1024 = createTexture("assets/common/images/circle1024.png");
	renderer->circleTexture32 = createTexture("assets/common/images/circle32.png");

	{
		char *gradData = (char *)frameMalloc(1 * 256 * 4);
		for (int y = 0; y < 256; y++) {
			Vec4 color = v4(y/255.0, 1, 1, 1);
			int colorInt = argbToHex(color);
			memcpy(&gradData[y*4], &colorInt, sizeof(int));
		}

		renderer->linearGrad256 = createTexture(1, 256, gradData);
	}
}

void startRenderingFrame() {
	backendStartFrame();
	refreshGlobalMatrices();
}

void endRenderingFrame() {
	processBatchDraws();
	for (int i = 0; i < renderer->frameTexturesNum; i++) destroyTexture(renderer->frameTextures[i]);
	renderer->frameTexturesNum = 0;
	backendEndFrame();
}

void clearRenderer(int color) {
	processBatchDraws(); // I probably don't really need to do this
	backendClearRenderer(color);
}

Shader *loadShader(char *vsPath, char *fsPath) {
	if (vsPath == NULL) vsPath = "assets/common/shaders/base.vs";
	if (fsPath == NULL) fsPath = "assets/common/shaders/base.fs";

	Shader *shader = (Shader *)zalloc(sizeof(Shader));
	backendLoadShader(&shader->backendShader, (char *)frameReadFile(vsPath), (char *)frameReadFile(fsPath));

	shader->mvpLoc = getUniformLocation(shader, "mvp");
	shader->resolutionLoc = getUniformLocation(shader, "resolution");

	return shader;
}

int getUniformLocation(Shader *shader, char *uniformName) {
	int loc = backendGetUniformLocation(&shader->backendShader, uniformName);
	if (loc == -1) logf("No uniform called %s\n", uniformName);
	return loc;
}

void setShaderUniform(Shader *shader, int loc, float value) {
	processBatchDraws();
	setShader(shader);
	backendSetShaderUniform(&shader->backendShader, loc, &value, SHADER_UNIFORM_FLOAT, 1);
}
void setShaderUniform(Shader *shader, int loc, Vec2 value) {
	processBatchDraws();
	setShader(shader);
	backendSetShaderUniform(&shader->backendShader, loc, &value, SHADER_UNIFORM_VEC2, 1);
}
void setShaderUniform(Shader *shader, int loc, Vec3 value) {
	processBatchDraws();
	setShader(shader);
	backendSetShaderUniform(&shader->backendShader, loc, &value, SHADER_UNIFORM_VEC3, 1);
}
void setShaderUniform(Shader *shader, int loc, Vec4 value) {
	processBatchDraws();
	setShader(shader);
	backendSetShaderUniform(&shader->backendShader, loc, &value, SHADER_UNIFORM_VEC4, 1);
}
void setShaderUniform(Shader *shader, int loc, Matrix4 value) {
	processBatchDraws();
	setShader(shader);
	backendSetShaderUniform(&shader->backendShader, loc, &value, SHADER_UNIFORM_MATRIX4, 1);
}
void setShaderUniform(Shader *shader, int loc, Texture *texture) {
	processBatchDraws();
	setShader(shader);
	backendSetShaderUniform(&shader->backendShader, loc, &texture->backendTexture, SHADER_UNIFORM_TEXTURE, 1);
}

Texture *createTexture(const char *path, int flags) {
	int pngSize;
	void *pngData = readFile(path, &pngSize);

	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	stbi_uc *img = stbi_load_from_memory((unsigned char *)pngData, pngSize, &width, &height, &channels, 4);

	if (!img) return NULL;

	Texture *texture = createTexture(width, height, img, flags);
	if (!texture) Panic(frameSprintf("Failed to load image %s\n", path));

	if (path) texture->path = stringClone(path);
	free(img);
	free(pngData);

	return texture;
}

Texture *createTexture(int width, int height, void *data, int flags) {
	Texture *texture = (Texture *)zalloc(sizeof(Texture));
	if (width == 0) width = 1;
	if (height == 0) height = 1;
	texture->width = width;
	texture->height = height;

	processBatchDraws();
	texture->backendTexture = backendCreateTexture(width, height, flags);
	setTexture(texture);

	if (data) setTextureData(texture, data, width, height, flags);
	texture->smooth = true;
	setTextureClampedX(texture, true);
	setTextureClampedY(texture, true);

	return texture;
}

Texture *createFrameTexture(int width, int height, void *data, int flags) {
	Texture *texture = createTexture(width, height, data, flags);
	if (renderer->frameTexturesNum > FRAME_TEXTURES_MAX-1) {
		logf("Too many frame textures, texture will leak\n");
		return texture;
	}

	renderer->frameTextures[renderer->frameTexturesNum++] = texture;
	return texture;
}

void setTextureSmooth(Texture *texture, bool smooth) {
	if (texture->smooth == smooth) return;
	texture->smooth = smooth;

	processBatchDraws();

	backendSetTexture(&texture->backendTexture);
	backendSetTextureSmooth(smooth);
}
void setTextureClampedX(Texture *texture, bool clamped) {
	if (texture->clampedX == clamped) return;
	texture->clampedX = clamped;

	processBatchDraws();

	backendSetTexture(&texture->backendTexture);
	backendSetTextureClampedX(clamped);
}
void setTextureClampedY(Texture *texture, bool clamped) {
	if (texture->clampedY == clamped) return;
	texture->clampedY = clamped;

	processBatchDraws();

	backendSetTexture(&texture->backendTexture);
	backendSetTextureClampedY(clamped);
}

void *_tempTextureBuffer;
int _tempTextureBufferSize;
void setTextureData(Texture *texture, void *data, int width, int height, int flags) {
	processBatchDraws();

	int neededTextureBufferSize = width * height * 4;
	if (neededTextureBufferSize > _tempTextureBufferSize) {
		if (_tempTextureBuffer) free(_tempTextureBuffer);
		_tempTextureBufferSize = neededTextureBufferSize;
		_tempTextureBuffer = malloc(_tempTextureBufferSize);
	}

	void *newData = _tempTextureBuffer;
	memcpy(newData, data, width * height * 4);
	data = newData;

	if ((flags & _F_TD_SKIP_PREMULTIPLY) == 0) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				float a = ((u8 *)data)[(y*width+x)*4 + 3] / 255.0;
				((u8 *)data)[(y*width+x)*4 + 0] *= a;
				((u8 *)data)[(y*width+x)*4 + 1] *= a;
				((u8 *)data)[(y*width+x)*4 + 2] *= a;
			}
		}
	}

	if (flags & _F_TD_FLIP_Y) {
		flipBitmapData((u8 *)data, width, height);
	}

	backendSetTexture(&texture->backendTexture);
	backendSetTextureData(data, width, height, flags);
}

u8 *getTextureData(Texture *texture, int flags) {
	processBatchDraws();
	return backendGetTextureData(&texture->backendTexture);
}

bool writeTextureToFile(Texture *texture, char *path) {
	u8 *bitmapData = getTextureData(texture);

	stbi_flip_vertically_on_write(true);
	if (!stbi_write_png(frameSprintf("%s%s", filePathPrefix, path), texture->width, texture->height, 4, bitmapData, texture->width*4)) {
		logf("Failed to write sprite sheet: %s\n", path);
		free(bitmapData);
		return false;
	}
	free(bitmapData);
	return true;
}

Texture *getTexture(char *path, int flags) {
  if (!renderer->textureMap) {
    renderer->textureMapAllocator.type = ALLOCATOR_DEFAULT;
    renderer->textureMap = createHashMap(sizeof(char *), sizeof(Texture *), 64, &renderer->textureMapAllocator);
    renderer->textureMap->usesStreq = true;
  }

	if (!path) return NULL;
	{
		Texture *texture;
		void *pathAsVoid = path;
		void *pathPtr = &pathAsVoid;
		if (hashMapGet(renderer->textureMap, pathPtr, stringHash32(path), &texture)) {
			return texture;
		}
	}

	if (!fileExists(path)) return NULL;

	Texture *texture = createTexture(path, flags);
	if (!texture) return NULL;

	void *pathAsVoid = texture->path;
	void *pathPtr = &pathAsVoid;
	hashMapSet(renderer->textureMap, pathPtr, stringHash32(texture->path), &texture);
	return texture;
}

void destroyTexture(Texture *texture) {
	processBatchDraws();
	if (texture->path) free(texture->path);
	backendDestroyTexture(&texture->backendTexture);
	// free(texture); //@incomplete Why can't I free this???
}

void clearTextureCache() {
  HashMapIterator iter;

  HashMapNode *node = init(&iter, renderer->textureMap);
  for (;;) {
    if (!node) break;

    Texture *texture = *(Texture **)node->value;
    destroyTexture(texture);

    node = next(&iter);
  }

  destroyHashMap(renderer->textureMap);
  renderer->textureMap = NULL;
}

void drawTexture(Texture *texture, RenderProps props) {
	if (props.alpha == 0) return;
	if (getAofArgb(props.tint) == 0) return;
	if (props.disabled) return;
	if (renderer->disabled) return;
	if (!texture) {
		Panic("drawTexture called with null texture!");
	}

	props.tint = setAofArgb(props.tint, getAofArgb(props.tint) * props.alpha);
	int flags = props.flags;
	Vec2 uv0 = props.uvMatrix * props.uv0;
	Vec2 uv1 = props.uvMatrix * props.uv1;
	drawQuadVerts(texture, props.matrix, uv0, uv1, props.tint, flags);
}

void drawSimpleTexture(Texture *texture) {
	Matrix3 matrix = mat3();
	matrix.SCALE(getSize(texture));
	drawQuadVerts(texture, matrix);
}
void drawSimpleTexture(Texture *texture, Matrix3 matrix, Vec2 uv0, Vec2 uv1, float alpha) {
	int tint = setAofArgb(0xFFFFFFFF, alpha * 255.0);
	drawQuadVerts(texture, matrix, uv0, uv1, tint);
}

void drawRect(Rect rect, int color) {
	if (getAofArgb(color) == 0) return;

	Matrix3 matrix = mat3();
	matrix.TRANSLATE(rect.x, rect.y);
	matrix.SCALE(rect.width, rect.height);
	drawQuadVerts(renderer->whiteTexture, matrix, v2(0, 0), v2(1, 1), color);
}

void drawCircle(Circle circle, int color) {
	if (getAofArgb(color) == 0) return;

	Matrix3 matrix = mat3();
	matrix.TRANSLATE(circle.position - circle.radius);
	matrix.SCALE(circle.radius*2);

	Texture *texture = renderer->circleTexture1024;
	if (circle.radius < 45) texture = renderer->circleTexture32;

	drawQuadVerts(texture, matrix, v2(0, 0), v2(1, 1), color);
}

void drawQuadVerts(Texture *texture, Matrix3 matrix, Vec2 uv0, Vec2 uv1, int tint, int flags) {
	tint = multiplyColors(tint, renderer->currentColor);
	if (getAofArgb(tint) == 0) return;

	Vec2 verts[] = {
		v2(0, 0),
		v2(0, 1),
		v2(1, 1),
		v2(1, 0),
	};

	for (int i = 0; i < ArrayLength(verts); i++) {
		verts[i] = matrix * verts[i];
	}

	Matrix3 flipMatrix = {
		1,  0,  0,
		0, -1,  0,
		0,  1,  1
	};
	uv0 = flipMatrix * uv0;
	uv1 = flipMatrix * uv1;

	Vec2 uvs[] = {
		v2(uv0.x, uv0.y),
		v2(uv0.x, uv1.y),
		v2(uv1.x, uv1.y),
		v2(uv1.x, uv0.y),
	};

	int colors[4] = {tint, tint, tint, tint};

	{
		Vec2 triVerts[6];
		Vec2 triUvs[6];
		int triColors[6];

		triVerts[0] = verts[0];
		triVerts[1] = verts[1];
		triVerts[2] = verts[2];
		triVerts[3] = verts[0];
		triVerts[4] = verts[2];
		triVerts[5] = verts[3];

		triUvs[0] = uvs[0];
		triUvs[1] = uvs[1];
		triUvs[2] = uvs[2];
		triUvs[3] = uvs[0];
		triUvs[4] = uvs[2];
		triUvs[5] = uvs[3];

		triColors[0] = colors[0];
		triColors[1] = colors[1];
		triColors[2] = colors[2];
		triColors[3] = colors[0];
		triColors[4] = colors[2];
		triColors[5] = colors[3];

		drawVerts(triVerts, triUvs, triColors, 6, texture);
	}
}

void drawVerts(Vec2 *verts, Vec2 *uvs, int *colors, int vertsNum, Texture *texture) {
	Matrix3 matrix = getCurrentVertexMatrix();

	Vec3 *verts3 = (Vec3 *)frameMalloc(sizeof(Vec3) * vertsNum);
	for (int i = 0; i < vertsNum; i++) verts3[i] = v3(matrix * verts[i], renderer->current2dDrawDepth);

	renderer->current2dDrawDepth += 1.0/RENDERER_BACKEND_MAX_DRAWS_PER_BATCH; // Should really be a fraction of the far-near clip plane
	drawVerts(verts3, uvs, colors, vertsNum, texture);
}

Matrix3 getCurrentVertexMatrix() {
	Vec2 targetSize = v2(platform->windowWidth, platform->windowHeight);
	if (renderer->targetTextureStackNum > 0) targetSize = getSize(renderer->targetTextureStack[renderer->targetTextureStackNum-1]);
	Matrix3 matrix = getProjectionMatrix(targetSize.x, targetSize.y) * renderer->baseMatrix2d;
	return matrix;
}

void drawVerts(Vec3 *verts, Vec2 *uvs, int *colors, int vertsNum, Texture *texture) {
	if (renderer->currentDrawCount > RENDERER_BACKEND_MAX_DRAWS_PER_BATCH-10) processBatchDraws(); // Magic -10 :/
	renderer->currentDrawCount++;

	setTexture(texture);

	GpuVertex *gpuVerts = (GpuVertex *)frameMalloc(sizeof(GpuVertex) * vertsNum);
	for (int i = 0; i < vertsNum; i++) {
		gpuVerts[i].position = verts[i];
		gpuVerts[i].uv = uvs[i];
		gpuVerts[i].color = hexToArgbFloat(colors[i]);
	}
	drawVerts(gpuVerts, vertsNum);
}

void drawVerts(GpuVertex *verts, int vertsNum) {
	if (vertsNum > BATCH_VERTS_MAX-1) {
		logf("Too many gpu verts!\n");
		return;
	}

	if (renderer->batchVertsNum + vertsNum > BATCH_VERTS_MAX-1) processBatchDraws();

	for (int i = 0; i < vertsNum; i++) {
		renderer->batchVerts[renderer->batchVertsNum++] = verts[i];
	}
}

void processBatchDraws() {
	if (renderer->batchVertsNum == 0) return;
	renderer->currentDrawCount = 0;
	renderer->current2dDrawDepth = -1;
	renderer->batchTextureSlotsNum = 0;

	backendDrawVerts(renderer->batchVerts, renderer->batchVertsNum);
	renderer->batchVertsNum = 0;
	backendFlush();
}

void pushTargetTexture(Texture *texture) {
	processBatchDraws();

	if (renderer->targetTextureStackNum >= TARGET_TEXTURE_LIMIT-1) Panic("Target texture overflow");
	renderer->targetTextureStack[renderer->targetTextureStackNum++] = texture;
  backendSetTargetTexture(&texture->backendTexture);
	refreshGlobalMatrices();
}

void popTargetTexture() {
	processBatchDraws();

	renderer->targetTextureStackNum--; //@robustness Why is there no error check for underflow?
	if (renderer->targetTextureStackNum > 0) {
		Texture *texture = renderer->targetTextureStack[renderer->targetTextureStackNum-1];
		backendSetTargetTexture(&texture->backendTexture);
	} else {
		backendSetTargetTexture(NULL);
	}
	refreshGlobalMatrices();
}

void pushCamera2d(Matrix3 mat) {
	if (renderer->camera2dStackNum > CAMERA_2D_STACK_MAX-1) Panic("camera2d overflow");
	renderer->camera2dStack[renderer->camera2dStackNum++] = mat;
	refreshGlobalMatrices();
}

void popCamera2d() {
	if (renderer->camera2dStackNum <= 0) {
		logf("camera2d underflow");
		return;
	}
	renderer->camera2dStackNum--;
	refreshGlobalMatrices();
}

void refreshGlobalMatrices() {
	renderer->baseMatrix2d = mat3();
	for (int i = 0; i < renderer->camera2dStackNum; i++) renderer->baseMatrix2d *= renderer->camera2dStack[i];
}

void setScissor(Rect rect) {
	processBatchDraws();

	rect.x *= renderer->scissorScale.x;
	rect.y *= renderer->scissorScale.y;
	rect.width *= renderer->scissorScale.x;
	rect.height *= renderer->scissorScale.y;
	Vec2 targetSize = v2(platform->windowWidth, platform->windowHeight);
	if (renderer->targetTextureStackNum > 0) targetSize = getSize(renderer->targetTextureStack[renderer->targetTextureStackNum-1]);
	rect.y = targetSize.y - rect.height - rect.y;
  backendSetScissor(rect);
}

void clearScissor() {
	processBatchDraws();
  backendEndScissor();
}

void pushColor(int color) {
	if (renderer->colorStackNum > COLOR_STACK_MAX-1) Panic("color overflow");
	renderer->colorStack[renderer->colorStackNum++] = color;
	renderer->currentColor = 0xFFFFFFFF;
	for (int i = 0; i < renderer->colorStackNum; i++) renderer->currentColor = multiplyColors(renderer->currentColor, renderer->colorStack[i]);
}

void popColor() {
	if (renderer->colorStackNum <= 0) Panic("color underflow");
	renderer->colorStackNum--;
	renderer->currentColor = 0xFFFFFFFF;
	for (int i = 0; i < renderer->colorStackNum; i++) renderer->currentColor = multiplyColors(renderer->currentColor, renderer->colorStack[i]);
}

void setRendererBlendMode(BlendMode blendMode) {
	if (renderer->currentBlendMode == blendMode) return;
	renderer->currentBlendMode = blendMode;

	processBatchDraws();
	backendSetBlendMode(blendMode);
}

void setDepthTest(bool enabled) {
	if (renderer->currentDepthTest == enabled) return;
	renderer->currentDepthTest = enabled;

	processBatchDraws();
	backendSetDepthTest(enabled);
}

void setDepthMask(bool enabled) {
	if (renderer->currentDepthMask == enabled) return;
	renderer->currentDepthMask = enabled;

	processBatchDraws();
	backendSetDepthMask(enabled);
}

void setBlending(bool enabled) {
	if (renderer->currentBlending == enabled) return;
	renderer->currentBlending = enabled;

	processBatchDraws();
	backendSetBlending(enabled);
}

void setBackfaceCulling(bool enabled) {
	if (renderer->currentBackfaceCulling == enabled) return;
	renderer->currentBackfaceCulling = enabled;

	processBatchDraws();
	backendSetBackfaceCulling(enabled);
}

void setShader(Shader *shader) {
	if (!shader) shader = renderer->defaultShader;

	if (renderer->currentShader == shader) return;

	processBatchDraws();

	renderer->currentShader = shader;

	backendSetShader(&shader->backendShader);

	setShaderUniform(shader, shader->mvpLoc, mat4());
}

void setTexture(Texture *texture, int slot) {
	if (renderer->textureSlots[slot] == texture) return;
	renderer->textureSlots[slot] = texture;

	Shader *shader = renderer->currentShader;
	if (shader->resolutionLoc && slot == 0) setShaderUniform(shader, shader->resolutionLoc, getSize(texture));

	processBatchDraws();
	backendSetTexture(&texture->backendTexture, slot);
}

void resetRenderContext() {
	processBatchDraws();

	Shader *currentShader = renderer->currentShader;
	renderer->currentShader = NULL;
	setShader(currentShader);

	BlendMode currentBlendMode = renderer->currentBlendMode;
	renderer->currentBlendMode = BLEND_INVALID;
	setRendererBlendMode(currentBlendMode);

	bool currentDepthTest = renderer->currentDepthTest;
	renderer->currentDepthTest ^= 1;
	setDepthTest(currentDepthTest);

	bool currentDepthMask = renderer->currentDepthMask;
	renderer->currentDepthMask ^= 1;
	setDepthMask(currentDepthMask);

	bool currentBlending = renderer->currentBlending;
	renderer->currentBlending ^= 1;
	setBlending(currentBlending);

	bool currentBackfaceCulling = renderer->currentBackfaceCulling;
	renderer->currentBackfaceCulling ^= 1;
	setBackfaceCulling(currentBackfaceCulling);

	for (int i = 0; i < TEXTURE_SLOTS_MAX; i++) {
		if (!renderer->textureSlots[i]) continue;
		Texture *currentTexture = renderer->textureSlots[i];
		renderer->textureSlots[i] = NULL;
		setTexture(currentTexture, i);
	}

	if (renderer->targetTextureStackNum > 0) {
		Texture *texture = renderer->targetTextureStack[renderer->targetTextureStackNum-1];
		backendSetTargetTexture(&texture->backendTexture);
	} else {
		backendSetTargetTexture(NULL);
	}
	refreshGlobalMatrices();

	backendResetRenderContext();
}

void imGuiTexture(Texture *texture, Vec2 scale) {
	if (!texture) return;
  backendImGuiTexture(&texture->backendTexture, scale);
}

bool imGuiImageButton(Texture *texture) {
	if (!texture) {
		logf("No texture for guiImageButton");
		return false;
	}
  return backendImGuiImageButton(&texture->backendTexture);
}
