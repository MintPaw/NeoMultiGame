struct Texture {
	BackendTexture backendTexture;
	int width;
	int height;

	bool clampedX;
	bool clampedY;
	char *path;
};

struct RenderProps {
	bool disabled;

	Matrix3 matrix;
	float alpha;
	int tint;

	int srcWidth;
	int srcHeight;

#define _F_CIRCLE (1 << 6)
	u32 flags;

	Vec2 uv0;
	Vec2 uv1;
	Matrix3 uvMatrix;
};

struct Shader {
	BackendShader backendShader;
};

struct Renderer {
	bool disabled;

  Vec2 scissorScale;

	Shader *defaultShader;
	int defaultMvpLoc;
	int defaultTexture0Loc;
	int defaultColDiffuseLoc;

	Shader *outlineShader;
	int outlineShaderResolutionLoc;
	int outlineShaderOutlineSizeLoc;
	int outlineShaderOutlineColorLoc;
	int outlineShaderOutlineFadeOuterLoc;
	int outlineShaderOutlineFadeInnerLoc;
	int outlineShaderColDiffuse;

	Shader *currentShader;

#define TARGET_TEXTURE_LIMIT 16
	Texture *targetTextureStack[TARGET_TEXTURE_LIMIT];
	int targetTextureStackNum;

#define CAMERA_2D_STACK_MAX 128
	Matrix3 camera2dStack[CAMERA_2D_STACK_MAX];
	int camera2dStackNum;
	Matrix3 baseMatrix2d;

#define ALPHA_STACK_MAX 128
	float alphaStack[ALPHA_STACK_MAX];
	int alphaStackNum;
	float currentAlpha;

	Texture *whiteTexture;
	Texture *circleTexture1024;
	Texture *circleTexture32;
	Texture *linearGrad256;

#define FRAME_TEXTURES_MAX 512
	Texture *frameTextures[FRAME_TEXTURES_MAX];
	int frameTexturesNum;

	float current2dDrawDepth;
	int currentDrawCount;

  bool useBadSrcSize;
};

Renderer *renderer = NULL;

void initRenderer(int width, int height);
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

void destroyTexture(Texture *texture);

void drawTexture(Texture *texture, RenderProps props);
void drawSimpleTexture(Texture *texture);
void drawSimpleTexture(Texture *texture, Matrix3 matrix, Vec2 uv0=v2(0, 0), Vec2 uv1=v2(1, 1), float alpha=1);
void drawRect(Rect rect, int color, int flags=0);
void drawCircle(Circle circle, int color);
void drawQuadVerts(Texture *texture, Matrix3 matrix, Vec2 uv0, Vec2 uv1, Vec4i tints, float alpha, int flags);
void drawVerts(Vec2 *verts, Vec2 *uvs, int *colors, int vertsNum, Texture *texture);
void drawVerts(Vec3 *verts, Vec2 *uvs, int *colors, int vertsNum, Texture *texture);

void pushTargetTexture(Texture *renderTexture);
void popTargetTexture();

void refreshGlobalMatrices();
void pushCamera2d(Matrix3 mat);
void popCamera2d();

void setScissor(Rect rect);
void clearScissor();

void pushAlpha(float alpha); //@incomplete change to pushColor/popColor
void popAlpha();

void setRendererBlendMode(BlendMode blendMode);
void setDepthTest(bool enabled);
void setDepthMask(bool enabled);
void setBlending(bool enabled);
void setBackfaceCulling(bool enabled);

void setShader(Shader *shader);

void processBatchDraws();
void resetRenderContext();

void imGuiTexture(Texture *texture, Vec2 scale=v2(1, 1));
bool imGuiImageButton(Texture *texture);

#include "rendererUtils.cpp"

void initRenderer(int width, int height) {
	renderer = (Renderer *)zalloc(sizeof(Renderer));

	backendInit();

	pushCamera2d(mat3());
	pushAlpha(1);
	setRendererBlendMode(BLEND_NORMAL);
	setBackfaceCulling(false);
	renderer->scissorScale = v2(1, 1);

	{ /// Setup shaders
		renderer->defaultShader = loadShader(NULL, NULL);
		renderer->defaultMvpLoc = getUniformLocation(renderer->defaultShader, "mvp");
		renderer->defaultTexture0Loc = getUniformLocation(renderer->defaultShader, "texture0");
		renderer->defaultColDiffuseLoc = getUniformLocation(renderer->defaultShader, "colDiffuse");

		renderer->outlineShader = loadShader(NULL, "assets/common/shaders/outline.fs");
		renderer->outlineShaderResolutionLoc = getUniformLocation(renderer->outlineShader, "resolution");
		renderer->outlineShaderOutlineSizeLoc = getUniformLocation(renderer->outlineShader, "outlineSize");
		renderer->outlineShaderOutlineColorLoc = getUniformLocation(renderer->outlineShader, "outlineColor");
		renderer->outlineShaderOutlineFadeOuterLoc = getUniformLocation(renderer->outlineShader, "outlineFadeOuter");
		renderer->outlineShaderOutlineFadeInnerLoc = getUniformLocation(renderer->outlineShader, "outlineFadeInner");
		renderer->outlineShaderColDiffuse = getUniformLocation(renderer->outlineShader, "colDiffuse");
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
	processBatchDraws();
	backendClearRenderer(color);
}

Shader *loadShader(char *vsPath, char *fsPath) {
	if (vsPath == NULL) vsPath = "assets/common/shaders/base.vs";
	if (fsPath == NULL) fsPath = "assets/common/shaders/base.fs";

	Shader *shader = (Shader *)zalloc(sizeof(Shader));
	backendLoadShader(&shader->backendShader, (char *)frameReadFile(vsPath), (char *)frameReadFile(fsPath));

	return shader;
}

int getUniformLocation(Shader *shader, char *uniformName) {
	int loc = backendGetUniformLocation(&shader->backendShader, uniformName);
	if (loc == -1) logf("No uniform called %s\n", uniformName);
	return loc;
}

void setShaderUniform(Shader *shader, int loc, float value) {
	setShader(shader);
	return backendSetShaderUniform(&shader->backendShader, loc, &value, SHADER_UNIFORM_FLOAT, 1);
}
void setShaderUniform(Shader *shader, int loc, Vec2 value) {
	setShader(shader);
	return backendSetShaderUniform(&shader->backendShader, loc, &value, SHADER_UNIFORM_VEC2, 1);
}
void setShaderUniform(Shader *shader, int loc, Vec3 value) {
	setShader(shader);
	return backendSetShaderUniform(&shader->backendShader, loc, &value, SHADER_UNIFORM_VEC3, 1);
}
void setShaderUniform(Shader *shader, int loc, Vec4 value) {
	setShader(shader);
	return backendSetShaderUniform(&shader->backendShader, loc, &value, SHADER_UNIFORM_VEC4, 1);
}
void setShaderUniform(Shader *shader, int loc, Matrix4 value) {
	setShader(shader);
	return backendSetShaderUniform(&shader->backendShader, loc, &value, SHADER_UNIFORM_MATRIX4, 1);
}
void setShaderUniform(Shader *shader, int loc, Texture *texture) {
	setShader(shader);
	return backendSetShaderUniform(&shader->backendShader, loc, &texture->backendTexture, SHADER_UNIFORM_TEXTURE, 1);
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

	texture->backendTexture = backendCreateTexture(width, height, flags);

	if (data) setTextureData(texture, data, width, height, flags);
	setTextureSmooth(texture, true);
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

void setTextureSmooth(Texture *texture, bool smooth) { backendSetTextureSmooth(&texture->backendTexture, smooth); }
void setTextureClampedX(Texture *texture, bool clamped) { backendSetTextureClampedX(&texture->backendTexture, clamped); }
void setTextureClampedY(Texture *texture, bool clamped) { backendSetTextureClampedY(&texture->backendTexture, clamped); }
void setTextureData(Texture *texture, void *data, int width, int height, int flags) { backendSetTextureData(&texture->backendTexture, data, width, height, flags); }

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

void destroyTexture(Texture *texture) {
	if (texture->path) free(texture->path);
	backendDestroyTexture(&texture->backendTexture);
	// free(texture); //@incomplete Why can't I free this???
}

void drawTexture(Texture *texture, RenderProps props) {
	if (props.alpha == 0) return;
	if (props.disabled) return;
	if (renderer->disabled) return;
	if (!texture) Panic("drawTexture called with null texture!");

	if (renderer->useBadSrcSize) {
		if (props.srcWidth == 0) props.srcWidth = texture->width;
		if (props.srcHeight == 0) props.srcHeight = texture->height;
		props.matrix.SCALE(props.srcWidth, props.srcHeight);
	}

	Vec4i tints = v4i(props.tint, props.tint, props.tint, props.tint);
	float alpha = props.alpha;
	int flags = props.flags;
	Vec2 uv0 = props.uvMatrix * props.uv0;
	Vec2 uv1 = props.uvMatrix * props.uv1;
	drawQuadVerts(texture, props.matrix, uv0, uv1, tints, alpha, flags);
}

void drawSimpleTexture(Texture *texture) {
	Matrix3 matrix = mat3();
	matrix.SCALE(getSize(texture));
	Vec2 uv0 = v2(0, 0);
	Vec2 uv1 = v2(1, 1);
	Vec4i tints = v4i(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
	int alpha = 1;
	int flags = 0;
	drawQuadVerts(texture, matrix, uv0, uv1, tints, alpha, flags);
}
void drawSimpleTexture(Texture *texture, Matrix3 matrix, Vec2 uv0, Vec2 uv1, float alpha) {
	Vec4i tints = v4i(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF);
	int flags = 0;
	drawQuadVerts(texture, matrix, uv0, uv1, tints, alpha, flags);
}

void drawRect(Rect rect, int color, int flags) {
	Matrix3 matrix = mat3();
	matrix.TRANSLATE(rect.x, rect.y);
	matrix.SCALE(rect.width, rect.height);

	float alpha = 1;
	Vec4i tints = v4i(color, color, color, color);
	drawQuadVerts(renderer->whiteTexture, matrix, v2(0, 0), v2(1, 1), tints, alpha, flags);
}

void drawCircle(Circle circle, int color) {
	Matrix3 matrix = mat3();
	matrix.TRANSLATE(circle.position - circle.radius);
	matrix.SCALE(circle.radius*2);

	Matrix3 uvMatrix = mat3();
	float alpha = 1;
	int flags = 0;
	Vec4i tints = v4i(color, color, color, color);

	Texture *texture = renderer->circleTexture1024;
	if (circle.radius < 45) texture = renderer->circleTexture32;

	drawQuadVerts(texture, matrix, v2(0, 0), v2(1, 1), tints, alpha, flags);
}

void drawQuadVerts(Texture *texture, Matrix3 matrix, Vec2 uv0, Vec2 uv1, Vec4i tints, float alpha, int flags) {
	alpha *= renderer->currentAlpha;
	if (alpha == 0) return;

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

	int colors[4];
	for (int i = 0; i < 4; i++) {
		int tint = ((int *)&tints.x)[i];
		int a, r, g, b;
		hexToArgb(tint, &a, &r, &g, &b);
		a *= alpha;
		colors[i] = argbToHex(a, r, g, b);
	}

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
	Vec2 targetSize = v2(platform->windowWidth, platform->windowHeight);
	if (renderer->targetTextureStackNum > 0) targetSize = getSize(renderer->targetTextureStack[renderer->targetTextureStackNum-1]);
	Matrix3 matrix = getProjectionMatrix(targetSize.x, targetSize.y) * renderer->baseMatrix2d;

	Vec3 *verts3 = (Vec3 *)frameMalloc(sizeof(Vec3) * vertsNum);
	for (int i = 0; i < vertsNum; i++) verts3[i] = v3(matrix * verts[i], renderer->current2dDrawDepth);

	renderer->current2dDrawDepth += 1.0/RENDERER_BACKEND_MAX_DRAWS_PER_BATCH; // Should really be a fraction of the far-near clip plane
	drawVerts(verts3, uvs, colors, vertsNum, texture);
}

void drawVerts(Vec3 *verts, Vec2 *uvs, int *colors, int vertsNum, Texture *texture) {
	if (renderer->currentDrawCount > RENDERER_BACKEND_MAX_DRAWS_PER_BATCH-10) processBatchDraws(); // Magic -10 :/
	renderer->currentDrawCount++;
	if (renderer->currentShader == renderer->defaultShader) {
		Matrix4 matrix = mat4();
		setShaderUniform(renderer->defaultShader, renderer->defaultMvpLoc, matrix);
		// int tex = 0;
		// setShaderUniform(renderer->defaultShader, renderer->defaultTexture0Loc, tex);
		setShaderUniform(renderer->defaultShader, renderer->defaultColDiffuseLoc, v4(1, 1, 1, 1));
		//@incomplete I should set colDiffuse, but it's not used and I think I want to remove it
		// Also, other shaders need to set these two uniforms at least, Raylib sets them automatically
	}
	backendDrawVerts(verts, uvs, colors, vertsNum, &texture->backendTexture);
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

	renderer->targetTextureStackNum--;
	if (renderer->targetTextureStackNum > 0) {
		Texture *texture = renderer->targetTextureStack[renderer->targetTextureStackNum-1];
		backendSetTargetTexture(&texture->backendTexture);
	} else {
		backendSetTargetTexture(NULL);
	}
	refreshGlobalMatrices();
}

void refreshGlobalMatrices() {
	renderer->baseMatrix2d = mat3();
	for (int i = 0; i < renderer->camera2dStackNum; i++) renderer->baseMatrix2d *= renderer->camera2dStack[i];

	// glViewport(0, 0, targetSize.x, targetSize.y);
}

void pushCamera2d(Matrix3 mat) {
	if (renderer->camera2dStackNum > CAMERA_2D_STACK_MAX-1) Panic("camera2d overflow");
	renderer->camera2dStack[renderer->camera2dStackNum++] = mat;
	refreshGlobalMatrices();
}

void popCamera2d() {
	if (renderer->camera2dStackNum <= 1) Panic("camera2d underflow");
	renderer->camera2dStackNum--;
	refreshGlobalMatrices();
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

void pushAlpha(float value) {
	if (renderer->alphaStackNum > ALPHA_STACK_MAX-1) Panic("alpha overflow");
	renderer->alphaStack[renderer->alphaStackNum++] = value;
	renderer->currentAlpha = renderer->alphaStack[renderer->alphaStackNum-1];
}

void popAlpha() {
	if (renderer->alphaStackNum <= 1) Panic("alpha underflow");
	renderer->alphaStackNum--;
	renderer->currentAlpha = renderer->alphaStack[renderer->alphaStackNum-1];
}

void setRendererBlendMode(BlendMode blendMode) {
	processBatchDraws();
	backendSetBlendMode(blendMode);
}

void setDepthTest(bool enabled) {
	processBatchDraws();
	backendSetDepthTest(enabled);
}

void setDepthMask(bool enabled) {
	processBatchDraws();
	backendSetDepthMask(enabled);
}

void setBlending(bool enabled) {
	processBatchDraws();
	backendSetBlending(enabled);
}

void setBackfaceCulling(bool enabled) {
	processBatchDraws();
	backendSetBackfaceCulling(enabled);
}

void setShader(Shader *shader) {
	processBatchDraws();
	if (!shader) shader = renderer->defaultShader;

	renderer->currentShader = shader;
	backendSetShader(&shader->backendShader);
}

void processBatchDraws() {
	renderer->currentDrawCount = 0;
	renderer->current2dDrawDepth = -1;
	backendFlush();
}

void resetRenderContext() {
	processBatchDraws();
	setRendererBlendMode(BLEND_NORMAL);
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
