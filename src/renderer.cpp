#ifdef RENDERER_HEADER
#undef RENDERER_HEADER
#define TARGET_TEXTURE_LIMIT 16

#define DO_GL_LOG 0

#define GL_LOW_IMPACT_ERRORS 0 //@todo Figure how when to enable these
#define GL_HIGH_IMPACT_ERRORS 0

enum BlendMode {
	BLEND_NORMAL,
	BLEND_MULTIPLY,
	BLEND_SCREEN,
	BLEND_ADD,
	BLEND_INVERT,
	BLEND_SKIA,
};
const char *blendModeStrings[] = {
	"Normal",
	"Multiply",
	"Screen",
	"Add",
};

struct Texture {
	GLuint id;
	int width;
	int height;
	GLenum internalFormat;

	bool smooth;
	bool clamped;
	char *path;
};

struct RenderTexture {
	Texture *texture;
	int width;
	int height;
};

struct Framebuffer {
	GLuint id;
	Texture *colorAttachments[2];
	GLuint depthId;
};

struct RenderProps {
	bool disabled;
	Matrix3 matrix;
	float alpha;
	int tint;

	int srcWidth;
	int srcHeight;

#define _F_CIRCLE_V2 (1 << 0)
#define _F_THRESHOLD (1 << 1)
#define _F_ARC (1 << 2)
#define _F_BLUR13 (1 << 3)
#define _F_PERLIN (1 << 4)
#define _F_SANDSTORM (1 << 5)
#define _F_CIRCLE (1 << 6)
#define _F_HALO (1 << 7)
#define _F_INVERSE_SRGB (1 << 8)
#define _F_RECT_AA (1 << 9)
#define _F_ARC_V2 (1 << 10)
	u32 flags;

	BlendMode blendMode;
	Matrix3 uvMatrix;

	Vec4 params;
};

void initRenderer(int width, int height);

#define GetAttrib(programName, attribName) programName.attribName = glGetAttribLocation(programName.program, #attribName) 
#define GetUniform(programName, uniformName) programName.uniformName = glGetUniformLocation(programName.program, #uniformName) 
#define _F_TD_FLIP_Y           (1 << 1)
#define _F_TD_SKIP_PREMULTIPLY (1 << 2)
#define _F_TD_SRGB8            (1 << 3)
#define _F_TD_RGB16F           (1 << 4)

GLuint buildShader(const char *path);

void setShaderProgram(GLuint program);
void setViewport(float x, float y, float w, float h);
void setRendererBlendMode(BlendMode blendMode);
void bindVertexBuffer(GLuint buffer);
void bindElementBuffer(GLuint buffer);
void bindUniformBuffer(GLuint array);
void bindVertexArray(GLuint array);
void enableVertexAttribArray(int index);
void disableVertexAttribArray(int index);
void setUniform1i(GLint location, GLint v0) { if (location == -1) return; glUniform1i(location, v0); }
void setUniform1iv(GLint location, GLsizei count, const GLint *value) { if (location == -1) return; glUniform1iv(location, count, value); }
void setUniform2i(GLint location, GLint v0, GLint v1) { if (location == -1) return; glUniform2i(location, v0, v1); }
void setUniform1f(GLint location, GLfloat v0) { if (location == -1) return; glUniform1f(location, v0); }
void setUniform2f(GLint location, GLfloat v0, GLfloat v1) { if (location == -1) return; glUniform2f(location, v0, v1); }
void setUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2) { if (location == -1) return; glUniform3f(location, v0, v1, v2); }
void setUniform4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) { if (location == -1) return; glUniform4f(location, v0, v1, v2, v3); }
void setUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat *value) { if (location == -1) return; glUniformMatrix3fv(location, count, transpose, value); }
void bindTextureSlot(int slot, Texture *texture);
void bindTextureSlotId(int slot, int id);
void setFramebuffer(Framebuffer *framebuffer);

void checkGlError(int lineNum);

#if GL_LOW_IMPACT_ERRORS
#define CheckGlErrorLow() checkGlError(__LINE__);
#else
#define CheckGlErrorLow()
#endif

#if GL_HIGH_IMPACT_ERRORS
#define CheckGlErrorHigh() checkGlError(__LINE__);
#else
#define CheckGlErrorHigh()
#endif

Texture *createTexture(const char *path, int flags=0);
Texture *createTexture(int width, int height, void *data=NULL, int flags=0);
RenderTexture *createRenderTexture(int width, int height, void *data=NULL, int flags=0);
void setTextureData(Texture *texture, void *data, int width, int height, int flags=0);
u8 *getTextureData(Texture *texture, int flags=0);
u8 *getTextureData(RenderTexture *renderTexture, int flags) { return getTextureData(renderTexture->texture, flags); }
void destroyTexture(Texture *texture);
void destroyTexture(RenderTexture *renderTexture);

void clearRenderer(int color=0);

void pushTargetTexture(RenderTexture *renderTexture);
void popTargetTexture();
void setTargetTexture(RenderTexture *renderTexture);

Framebuffer *createFramebuffer();
void setColorAttachment(Framebuffer *framebuffer, Texture *texture, int slot=0);
void setColorAttachment(Framebuffer *framebuffer, RenderTexture *texture, int slot=0) { setColorAttachment(framebuffer, texture->texture, slot); }
void addDepthAttachment(float width, float height);

void drawCircle(Vec2 position, float radius, int color);
void drawRect(Rect rect, int color, int flags=0);
void drawTexture(Texture *texture, RenderProps props);
void drawOutline(Texture *texture, int color, float thickness=1);
void drawBlurred(Texture *texture, bool horizontal);
void drawSimpleTexture(Texture *texture);
void drawSimpleTexture(Texture *texture, Matrix3 matrix, Vec2 uv0=v2(0, 0), Vec2 uv1=v2(1, 1), float alpha=1);
void draw2dMesh(float *xyuvs, int xyuvsNum, u16 *inds, int indsNum, Matrix3 matrix, Texture *texture, float alpha=1);
void draw2dMesh(Vec2 *verts, Vec2 *uvs, u16 *inds, int indsNum, Matrix3 matrix, Texture *texture, float alpha=1);
void drawFxaaTexture(Texture *texture, Matrix3 matrix);
void drawPost3dTexture(Texture *texture, Matrix3 matrix, int tint=0);
void drawPixelArtFilterTexture(Texture *texture, Matrix3 matrix, Vec2 uv0=v2(0, 0), Vec2 uv1=v2(1, 1));

void pushCamera2d(Matrix3 mat);
void popCamera2d();
void refreshGlobalMatrices();
void setScissor(Rect rect);
void clearScissor();

void pushAlpha(float value);
void popAlpha();

void processBatchDraws();
void endRenderingFrame();

struct DefaultProgram {
	GLuint program;
	// GLuint u_textures;
	GLuint u_textures[16];
	GLuint u_time;
};
#pragma pack(push, 4)
struct DefaultProgramInstancedVertex {
	float matrixData[12];
	float alpha;
	u32 flags;
	Vec4 tintColorShiftHueTexture;
	Vec4 params;
};

struct DefaultProgramNonInstancedVertex {
	Vec4 position;
};
#pragma pack(pop)

struct Mesh2dProgram {
	GLuint program;
	GLuint a_position;
	GLuint u_matrix;
	GLuint u_texture;
	GLuint u_alpha;
};

struct FxaaProgram {
	GLuint program;
	GLuint a_position;
	GLuint u_matrix;
	GLuint u_texture;
	GLuint u_destSize;
};

struct Post3dProgram {
	GLuint program;
	GLuint a_position;
	GLuint u_matrix;
	GLuint u_texture;
	GLuint u_destSize;
	GLuint u_tint;
};

struct OutlineProgram {
	GLuint program;
	GLuint a_xyuv;
	GLuint u_textureSize;
	GLuint u_color;
	GLuint u_thickness;
	GLuint u_texture;
};

struct GaussianBlurProgram {
	GLuint program;
	GLuint a_position;
	GLuint u_matrix;
	GLuint u_texture;
	GLuint u_horizontal;
};

struct VignetteProgram {
	GLuint program;
	GLuint a_position;
	GLuint u_texture;
	GLuint u_matrix;
	GLuint u_vignettePos;
	GLuint u_softnessSizeAlpha;
	GLuint u_color;
};

struct RetroProgram {
	GLuint program;
	GLuint a_position;
	GLuint u_matrix;
	GLuint u_texture;
	GLuint u_alpha;
	GLuint u_gridResolution;
	GLuint u_gridAlpha;
};

struct PixelArtFilterProgram {
	GLuint program;
	GLuint a_position;
	GLuint u_matrix;
	GLuint u_texture;
};

struct Renderer {
	int errorCount;
	int width;
	int height;
	bool disabled;
	bool using3d;
	bool in3dPass;

	DefaultProgram defaultProgram;
	Mesh2dProgram mesh2dProgram;
	FxaaProgram fxaaProgram;
	Post3dProgram post3dProgram;

	OutlineProgram outlineProgram;
	GaussianBlurProgram gaussianBlurProgram;
	VignetteProgram vignetteProgram;
	RetroProgram retroProgram;
	PixelArtFilterProgram pixelArtFilterProgram;

	GLuint defaultVao;
	GLuint batchVao;

	GLuint tempVerts;
	GLuint tempIndices;
	GLuint tempTexCoords;
	GLuint textureFramebuffer;
	GLuint textureDepthbuffer;
	GLuint depthStencilBuffer;
	void *tempTextureBuffer;
	int tempTextureBufferSize;
	void *tempTextureRowBuffer;
	int tempTextureRowBufferSize;

	GLuint curShaderProgram;
	GLuint currentArrayBuffer;
	GLuint currentElementBuffer;
	GLuint currentUniformBuffer;
	GLuint currentVao;
	Framebuffer *currentFramebuffer;
	Framebuffer *prevFramebuffer;
	Rect currentViewport;
	Rect currentScissor;

	GLuint texture2dType;
	bool vertexAttribsEnabled[128];

	RenderTexture *currentTargetTexture;
	RenderTexture *targetTextureStack[TARGET_TEXTURE_LIMIT];
	int targetTextureStackNum;

	Texture *emptyTexture;
	Texture *whiteTexture;

	GLuint batchNonInstancedBuffer;
	GLuint batchInstancedBuffer;
	GLuint batchElementBuffer;
	u16 *batchIndices;
	int batchIndicesNum;
	int batchIndicesMax;

	DefaultProgramInstancedVertex *defaultBatchInstancedVerts;
	int defaultBatchInstancedVertsNum;
	int defaultBatchInstancedVertsMax;

	DefaultProgramNonInstancedVertex *defaultBatchNonInstancedVerts;
	int defaultBatchNonInstancedVertsNum;
	int defaultBatchNonInstancedVertsMax;
	bool batchVertexArraysSet;
#define BOUND_TEXTURES_MAX 128
	int boundTextures[BOUND_TEXTURES_MAX];

	BlendMode batchBlendMode;
	Texture **batchTextures;

	Matrix3 currentCameraMatrix;
	Matrix3 baseMatrix2d;
	Matrix3 flipMatrix;

#define CAMERA_2D_STACK_MAX 128
	Matrix3 camera2dStack[CAMERA_2D_STACK_MAX];
	int camera2dStackNum;

#define ALPHA_STACK_MAX 128
	float alphaStack[ALPHA_STACK_MAX];
	int alphaStackNum;

	GLint maxTextureUnits;
	GLint maxVertexAttribs;
	char *vendor;

	int drawCallsLastFrame;
	int drawCallsThisFrame;
	int prevPushesCulled;
	int pushesCulled;
	int prevPushesProcessed;
	int pushesProcessed;

	Texture *defaultMaskTexture;
	Matrix3 defaultMaskMatrix;

	int debugBatchTextureCountCounts[16];
};
bool usesAlphaDiscard = false;

Renderer *renderer;

bool disable3d = false;

#else // End header

#include "rendererUtils.cpp"

void initRenderer(int width, int height) {
	// logf("Initing renderer (%.2fmb)\n", sizeof(Renderer) / 1024.0 / 1024.0);
#if 0
	int value;
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &value);
	logf("Max vertex uniforms: %d\n", value);

	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &value);
	logf("Max fragment uniforms: %d\n", value);

	glGetIntegerv(GL_STENCIL_BITS, &value);
	logf("Stencil bits: %d\n", value);

	glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &value);
	logf("GL_MAX_UNIFORM_BLOCK_SIZE: %d\n", value);

	glGetIntegerv(GL_MAX_SAMPLES, &value);
	logf("GL_MAX_SAMPLES %d\n", value);

	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &value);
	logf("GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS %d\n", value);

	glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &value);
	logf("GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS %d\n", value);

	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &value);
	logf("GL_MAX_(fragment)TEXTURE_IMAGE_UNITS %d\n", value);

	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB, &value);
	logf("GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB %d\n", value);
#endif

	renderer = (Renderer *)malloc(sizeof(Renderer));
	memset(renderer, 0, sizeof(Renderer));
	renderer->width = width;
	renderer->height = height;

	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &renderer->maxVertexAttribs);
	// logf("Max is %d\n", renderer->maxVertexAttribs);
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &renderer->maxTextureUnits);
	renderer->vendor = (char *)glGetString(GL_VENDOR);
	if (renderer->maxTextureUnits > 8) renderer->maxTextureUnits = 8;

	renderer->texture2dType = GL_TEXTURE_2D;

	glEnable(GL_BLEND);
#if DO_GL_LOG
	glEnable(GL_DEBUG_OUTPUT);
#endif
	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_FRONT);
	// glEnable(GL_MULTISAMPLE);

	// if (renderer->texture2dType == GL_TEXTURE_2D_MULTISAMPLE) {
	// 	glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
	// 	glEnable(GL_SAMPLE_ALPHA_TO_ONE);
	// 	glEnable(GL_MULTISAMPLE);
	// }
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // For 3d?
	// glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE); // Old
	// glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // Recommended // lol
	setRendererBlendMode(BLEND_NORMAL);
	CheckGlErrorLow();

	glGenVertexArrays(1, &renderer->defaultVao);
	renderer->currentVao = -1;
	bindVertexArray(renderer->defaultVao);

	renderer->defaultProgram.program = buildShader("assets/common/shaders/default.shader");
	// GetUniform(renderer->defaultProgram, u_textures);
	renderer->defaultProgram.u_textures[0] = glGetUniformLocation(renderer->defaultProgram.program, "u_textures[0]");
	renderer->defaultProgram.u_textures[1] = glGetUniformLocation(renderer->defaultProgram.program, "u_textures[1]");
	renderer->defaultProgram.u_textures[2] = glGetUniformLocation(renderer->defaultProgram.program, "u_textures[2]");
	renderer->defaultProgram.u_textures[3] = glGetUniformLocation(renderer->defaultProgram.program, "u_textures[3]");
	renderer->defaultProgram.u_textures[4] = glGetUniformLocation(renderer->defaultProgram.program, "u_textures[4]");
	renderer->defaultProgram.u_textures[5] = glGetUniformLocation(renderer->defaultProgram.program, "u_textures[5]");
	renderer->defaultProgram.u_textures[6] = glGetUniformLocation(renderer->defaultProgram.program, "u_textures[6]");
	renderer->defaultProgram.u_textures[7] = glGetUniformLocation(renderer->defaultProgram.program, "u_textures[7]");
	GetUniform(renderer->defaultProgram, u_time);

	renderer->mesh2dProgram.program = buildShader("assets/common/shaders/mesh2d.shader");
	GetAttrib(renderer->mesh2dProgram, a_position);
	GetUniform(renderer->mesh2dProgram, u_matrix);
	GetUniform(renderer->mesh2dProgram, u_texture);
	GetUniform(renderer->mesh2dProgram, u_alpha);

	renderer->fxaaProgram.program = buildShader("assets/common/shaders/fxaa.shader");
	GetAttrib(renderer->fxaaProgram, a_position);
	GetUniform(renderer->fxaaProgram, u_matrix);
	GetUniform(renderer->fxaaProgram, u_texture);
	GetUniform(renderer->fxaaProgram, u_destSize);

	renderer->post3dProgram.program = buildShader("assets/common/shaders/post3d.shader");
	GetAttrib(renderer->post3dProgram, a_position);
	GetUniform(renderer->post3dProgram, u_matrix);
	GetUniform(renderer->post3dProgram, u_texture);
	GetUniform(renderer->post3dProgram, u_destSize);
	GetUniform(renderer->post3dProgram, u_tint);

	renderer->outlineProgram.program = buildShader("assets/common/shaders/outline.shader");
	GetAttrib(renderer->outlineProgram, a_xyuv);
	GetUniform(renderer->outlineProgram, u_textureSize);
	GetUniform(renderer->outlineProgram, u_color);
	GetUniform(renderer->outlineProgram, u_thickness);
	GetUniform(renderer->outlineProgram, u_texture);

	renderer->gaussianBlurProgram.program = buildShader("assets/common/shaders/gaussianBlur.shader");
	GetAttrib(renderer->gaussianBlurProgram, a_position);
	GetUniform(renderer->gaussianBlurProgram, u_matrix);
	GetUniform(renderer->gaussianBlurProgram, u_texture);
	GetUniform(renderer->gaussianBlurProgram, u_horizontal);

	renderer->vignetteProgram.program = buildShader("assets/common/shaders/vignette.shader");
	GetAttrib(renderer->vignetteProgram, a_position);
	GetUniform(renderer->vignetteProgram, u_matrix);
	GetUniform(renderer->vignetteProgram, u_texture);
	GetUniform(renderer->vignetteProgram, u_vignettePos);
	GetUniform(renderer->vignetteProgram, u_softnessSizeAlpha);
	GetUniform(renderer->vignetteProgram, u_color);

	renderer->retroProgram.program = buildShader("assets/common/shaders/retro.shader");
	GetAttrib(renderer->retroProgram, a_position);
	GetUniform(renderer->retroProgram, u_matrix);
	GetUniform(renderer->retroProgram, u_texture);
	GetUniform(renderer->retroProgram, u_alpha);
	GetUniform(renderer->retroProgram, u_gridResolution);
	GetUniform(renderer->retroProgram, u_gridAlpha);

	renderer->pixelArtFilterProgram.program = buildShader("assets/common/shaders/pixelArtFilter.shader");
	GetAttrib(renderer->pixelArtFilterProgram, a_position);
	GetUniform(renderer->pixelArtFilterProgram, u_matrix);
	GetUniform(renderer->pixelArtFilterProgram, u_texture);
	CheckGlErrorLow();

	glGenBuffers(1, &renderer->tempVerts);
	glGenBuffers(1, &renderer->tempIndices);
	glGenBuffers(1, &renderer->tempTexCoords);
	glGenBuffers(1, &renderer->batchElementBuffer);
	glGenFramebuffers(1, &renderer->textureFramebuffer);

	glGenRenderbuffers(1, &renderer->textureDepthbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderer->textureDepthbuffer);

	CheckGlErrorLow();

	renderer->currentArrayBuffer = -1;
	renderer->currentElementBuffer = -1;
	renderer->currentUniformBuffer = -1;
	renderer->currentVao = -1;
	renderer->emptyTexture = createTexture(1, 1);

	u64 whiteData = 0xFFFFFFFF;
	renderer->whiteTexture = createTexture(1, 1, &whiteData);
	{ /// Batch renderer
		// {
		// 	int vertsPerDraw = 4;
		// 	renderer->batchVertexSize = 27 * sizeof(GLfloat);
		// 	renderer->batchVertexDataSize = renderer->batchVertexSize * vertsPerDraw;

		// 	renderer->batchVertexDataMax = 2048;
		// 	renderer->batchVertexData = malloc(renderer->batchVertexDataSize * renderer->batchVertexDataMax);
		// 	renderer->batchIndices = malloc(sizeof(int) * 6 * renderer->batchVertexDataMax);
		// 	logf("Renderer is using %.1fmb for batch textures\n", (float)(renderer->batchVertexDataSize * renderer->batchVertexDataMax)/(float)Megabytes(1));

		// 	glBindBuffer(GL_ARRAY_BUFFER, renderer->tempVerts);
		// 	glBufferData(GL_ARRAY_BUFFER, renderer->batchVertexDataSize * renderer->batchVertexDataMax, NULL, GL_DYNAMIC_DRAW);
		// }

		renderer->defaultBatchNonInstancedVertsMax = 10000;
		renderer->defaultBatchInstancedVertsMax = 10000;
		renderer->defaultBatchNonInstancedVerts = (DefaultProgramNonInstancedVertex *)malloc(sizeof(DefaultProgramNonInstancedVertex) * renderer->defaultBatchNonInstancedVertsMax);
		renderer->defaultBatchInstancedVerts = (DefaultProgramInstancedVertex *)malloc(sizeof(DefaultProgramInstancedVertex) * renderer->defaultBatchInstancedVertsMax);

		// logf(
		// 	"New batch renderer is using %.1fmb\n",
		// 	(float)((sizeof(DefaultProgramNonInstancedVertex)+sizeof(DefaultProgramInstancedVertex)) * renderer->defaultBatchNonInstancedVertsMax) / (float)Megabytes(1)
		// );

		glGenBuffers(1, &renderer->batchNonInstancedBuffer);
		bindVertexBuffer(renderer->batchNonInstancedBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(DefaultProgramNonInstancedVertex) * renderer->defaultBatchNonInstancedVertsMax, NULL, GL_DYNAMIC_DRAW);

		glGenBuffers(1, &renderer->batchInstancedBuffer);
		bindVertexBuffer(renderer->batchInstancedBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(DefaultProgramInstancedVertex) * renderer->defaultBatchInstancedVertsMax, NULL, GL_DYNAMIC_DRAW);

		renderer->batchIndicesMax = ceilf((float)renderer->defaultBatchNonInstancedVertsMax * 6.0/4.0);
		renderer->batchIndices = (u16 *)malloc(sizeof(u16) * renderer->batchIndicesMax);
		if (renderer->batchIndicesMax > USHRT_MAX) {
			logf("Index buffer too big to use fast past\n");
			Panic("Can't continue");
		}

		renderer->batchTextures = (Texture **)zalloc(sizeof(Texture *) * renderer->maxTextureUnits);
		for (int i = 0; i < renderer->maxTextureUnits; i++) renderer->batchTextures[i] = NULL;

		bindElementBuffer(renderer->batchElementBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(u16) * renderer->defaultBatchNonInstancedVertsMax, NULL, GL_DYNAMIC_DRAW);

		glGenVertexArrays(1, &renderer->batchVao);
		{
			bindVertexArray(renderer->batchVao);

			int totalAttribs = 8;
			{
				for (int i = 0; i < totalAttribs; i++) enableVertexAttribArray(i);

				int niVertSize = sizeof(DefaultProgramNonInstancedVertex);
				int iVertSize = sizeof(DefaultProgramInstancedVertex);
				int attrib = 0;
				// logf("Size: %d\n", niVertSize);

				bindVertexBuffer(renderer->batchNonInstancedBuffer);
				glVertexAttribPointer(attrib++, 4, GL_FLOAT, false, niVertSize, (void *)offsetof(DefaultProgramNonInstancedVertex, position));

				bindVertexBuffer(renderer->batchInstancedBuffer);
				int instancedStarts = attrib;
				glVertexAttribPointer(attrib++, 4, GL_FLOAT, false, iVertSize, (void *)offsetof(DefaultProgramInstancedVertex, matrixData));
				glVertexAttribPointer(attrib++, 4, GL_FLOAT, false, iVertSize, (void *)(offsetof(DefaultProgramInstancedVertex, matrixData) + 4*sizeof(float)));
				glVertexAttribPointer(attrib++, 4, GL_FLOAT, false, iVertSize, (void *)(offsetof(DefaultProgramInstancedVertex, matrixData) + 8*sizeof(float)));
				glVertexAttribPointer(attrib++, 1, GL_FLOAT, false, iVertSize, (void *)offsetof(DefaultProgramInstancedVertex, alpha));
				glVertexAttribPointer(attrib++, 1, GL_INT, false, iVertSize, (void *)offsetof(DefaultProgramInstancedVertex, flags));
				glVertexAttribPointer(attrib++, 4, GL_FLOAT, false, iVertSize, (void *)offsetof(DefaultProgramInstancedVertex, tintColorShiftHueTexture));
				glVertexAttribPointer(attrib++, 4, GL_FLOAT, false, iVertSize, (void *)offsetof(DefaultProgramInstancedVertex, params));

				if (attrib != totalAttribs) {
					for (int i = 0; i < 20; i++) logf("Desynced attribs %d should be %d\n", totalAttribs, attrib);
				}

				for (int i = instancedStarts; i < totalAttribs; i++) {
					glVertexAttribDivisor(i, 1);
				}

				CheckGlErrorHigh();
			}

			bindVertexArray(renderer->defaultVao);
		}

	}
	CheckGlErrorLow();

	Matrix3 flipMatrix = {
		1,  0,  0,
		0, -1,  0,
		0,  1,  1
	};
	memcpy(&renderer->flipMatrix, &flipMatrix, sizeof(Matrix3));

	pushCamera2d(mat3());
	pushAlpha(1);

	initRendererUtils();

	CheckGlErrorLow();
}

GLuint buildShader(const char *path) {
	char *shaderSrc = (char *)readFile(path);
	char *librarySrc = (char *)readFile("assets/common/shaders/library.shader");

	const char *seperator = "#VERTEX_END";
	char *splitAt = strstr(shaderSrc, seperator);
	if (!splitAt) Panic(frameSprintf("Invalid shader %s", path));
	
	char *vertSrc = NULL;
	int vertSize;
	{
		vertSize = splitAt - shaderSrc;
		vertSrc = frameMalloc(vertSize);
		strncpy(vertSrc, shaderSrc, vertSize-1);
		vertSrc[vertSize] = 0;
	}

	char *fragSrc = NULL;
	{
		char *fragPos = shaderSrc + vertSize + strlen(seperator);
		int fragLen = strlen(fragPos);
		fragSrc = frameMalloc(fragLen+1);
		strncpy(fragSrc, fragPos, fragLen);
		fragSrc[fragLen] = 0;
	}

#ifdef GL_ES
	char *versionLine = frameSprintf("#version 300 es\n");
#else
	char *versionLine = frameSprintf("#version 330\n");
#endif

	GLuint vert = glCreateShader(GL_VERTEX_SHADER);
	const char *vertArray[] = {
		versionLine,
		librarySrc,
		vertSrc,
	};
	glShaderSource(vert, ArrayLength(vertArray), vertArray, NULL);
	glCompileShader(vert);
	CheckGlErrorLow();
	static const int errLogLimit = 1024;
	char errLog[errLogLimit];
	int errLogNum;

	int vertReturn;
	glGetShaderiv(vert, GL_COMPILE_STATUS, &vertReturn);
	if (!vertReturn) {
		glGetShaderInfoLog(vert, errLogLimit, &errLogNum, errLog);
#ifdef FALLOW_INTERNAL
		logf("Vertex shader: \n%s\n", vertSrc);
#endif
		logf("Vertex error:\n%s\n", errLog);

#if defined(_WIN32) && defined(FALLOW_INTERNAL)
	{
		int vertLen = 0;
		for (int i = 0; i < ArrayLength(vertArray); i++) vertLen += strlen(vertArray[i]);
		char *combinedVert = frameMalloc(sizeof(char) * vertLen);
		combinedVert[0] = 0;
		for (int i = 0; i < ArrayLength(vertArray); i++) strcat(combinedVert, vertArray[i]);

		static int vertexShaderCount = 0;
		char *fileName = frameSprintf("assets/common/__raw/vertexShader%d.txt", vertexShaderCount++);
		writeFile(fileName, combinedVert, vertLen);
	}
#endif

		return 0;
	}

	GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
	const char *fragArray[] = {
		versionLine,
		librarySrc,
		fragSrc,
	};
	glShaderSource(frag, ArrayLength(fragArray), fragArray, NULL);
	glCompileShader(frag);
	CheckGlErrorLow();

	int fragReturn;
	glGetShaderiv(frag, GL_COMPILE_STATUS, &fragReturn);
	if (!fragReturn) {
		glGetShaderInfoLog(frag, errLogLimit, &errLogNum, errLog);

#ifdef FALLOW_INTERNAL
		logf("Fragment shader: \n%s\n", fragSrc);
#endif

#ifdef _WIN32
		if (platform->isDebugVersion) {
			char *pos = errLog;
			for (;;) {
				char *nextNewLine = strchr(pos, '\n');
				if (!nextNewLine) nextNewLine = pos + strlen(pos-1);

				char *firstParen = strchr(pos, '(')+1;
				char *secondParen = strchr(pos, ')');
				int parensLen = secondParen - firstParen;
				char *numStr = frameMalloc(parensLen);
				strncpy(numStr, firstParen, parensLen);
				int errorLineNum = atoi(numStr);

				int lineLen = nextNewLine - pos;
				char *line = frameMalloc(lineLen);
				strncpy(line, pos, lineLen);

				int libraryLines = 0;
				for (int i = 0; i < strlen(librarySrc); i++) if (librarySrc[i] == '\n') libraryLines++;
				int vertLines = 0;
				for (int i = 0; i < strlen(vertSrc); i++) if (vertSrc[i] == '\n') vertLines++;
				int dotShaderLineNumber = errorLineNum + vertLines - libraryLines - 1;

				logf(".shader:%d ----> %s\n", dotShaderLineNumber, line);
				pos = nextNewLine+1;
				if (!pos[0]) break;
			}
		} else {
			logf("Fragment raw error:\n%s\n", errLog);
		}
#else
		logf("Fragment raw error:\n%s\n", errLog);
#endif

		return 0;
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, vert);
	glAttachShader(program, frag);
	glLinkProgram(program);
	CheckGlErrorLow();

	int progReturn;
	glGetProgramiv(program, GL_LINK_STATUS, &progReturn);
	if (!progReturn) {
		glGetProgramInfoLog(program, errLogLimit, &errLogNum, errLog);
		logf("Program error(%d):\n%s\n", errLogNum, errLog);
		logf("Shader program error\n");
	}

	CheckGlErrorLow();

	free(shaderSrc);
	free(librarySrc);

	return program;
}

void startFrame();
void startFrame() {
	if (renderer->targetTextureStackNum == 0 && !renderer->currentFramebuffer) {
		setViewport(0, 0, platform->windowWidth, platform->windowHeight);
	}
}
void clearRenderer(int color) {
	if (renderer->disabled) return;

	startFrame();
	int a, r, g, b;
	hexToArgb(color, &a, &r, &g, &b);
	glClearColor(r/255.0, g/255.0, b/255.0, a/255.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void pushTargetTexture(RenderTexture *renderTexture) {
	if (renderer->targetTextureStackNum >= TARGET_TEXTURE_LIMIT-1) Panic("Target texture overflow");
	if (renderer->currentFramebuffer) {
		if (!renderer->prevFramebuffer) {
			renderer->prevFramebuffer = renderer->currentFramebuffer;
			setFramebuffer(NULL); // Probably not necessary
		} else {
			Panic("Don't pushTargetTexture while using framebuffers (more than once...)\n");
		}
	}

	renderer->targetTextureStack[renderer->targetTextureStackNum++] = renderTexture;

	setTargetTexture(renderTexture);
}

void popTargetTexture() {
	renderer->targetTextureStackNum--;

	if (renderer->targetTextureStackNum > 0) {
		setTargetTexture(renderer->targetTextureStack[renderer->targetTextureStackNum-1]);
	} else {
		setTargetTexture(NULL);
		if (renderer->prevFramebuffer) {
			setFramebuffer(renderer->prevFramebuffer);
			renderer->prevFramebuffer = 0;
		}
	}
}

void setTargetTexture(RenderTexture *renderTexture) {
	if (renderer->disabled) return;
	processBatchDraws();
	renderer->currentTargetTexture = renderTexture;

	// for (int i = 0; i < renderer->maxTextureUnits; i++) { // Not sure what this is doing, maybe preventing binding a texture that you're drawing to?
	// 	glActiveTexture(GL_TEXTURE0+i);
	// 	glBindTexture(GL_TEXTURE_2D, 0);
	// }

	if (renderTexture == NULL) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		setViewport(0, 0, platform->windowWidth, platform->windowHeight);
	} else {
		glBindFramebuffer(GL_FRAMEBUFFER, renderer->textureFramebuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderer->texture2dType, renderTexture->texture->id, 0);
		setViewport(0, 0, renderTexture->texture->width, renderTexture->texture->height);
	}

	CheckGlErrorHigh();
}

void setTargetTextureEx(int index, Texture *texture);
void setTargetTextureEx(int index, Texture *texture) {
	GLint id = 0;
	if (texture) id = texture->id;

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+index, renderer->texture2dType, id, 0);
}

Framebuffer *createFramebuffer() {
	Framebuffer *framebuffer = (Framebuffer *)zalloc(sizeof(Framebuffer));
	glGenFramebuffers(1, &framebuffer->id);
	return framebuffer;
}

void setFramebuffer(Framebuffer *framebuffer) {
	processBatchDraws();

	renderer->currentFramebuffer = framebuffer;
	int id = 0;
	if (framebuffer) id = framebuffer->id;

	glBindFramebuffer(GL_FRAMEBUFFER, id);

	if (framebuffer && framebuffer->colorAttachments[0]) {
		setViewport(0, 0, framebuffer->colorAttachments[0]->width, framebuffer->colorAttachments[0]->height);
	} else {
		setViewport(0, 0, platform->windowWidth, platform->windowHeight);
	}
}

void setColorAttachment(Framebuffer *framebuffer, Texture *texture, int slot) {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);

	int textureId = 0;
	if (texture) textureId = texture->id;
	framebuffer->colorAttachments[slot] = texture;
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+slot, renderer->texture2dType, textureId, 0);

	if (framebuffer->colorAttachments[1]) {
		unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachments);
	}

	if (renderer->currentFramebuffer) {
		glBindFramebuffer(GL_FRAMEBUFFER, renderer->currentFramebuffer->id);
	} else {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

}

// void setColorAttachment(Texture *texture, int slot) {
// 	Framebuffer *framebuffer = renderer->currentFramebuffer;
// 	if (slot != 0 && slot != 1) Panic("Invalid framebuffer color attachment slot");

// 	int id = 0;
// 	if (texture) id = texture->id;
// 	framebuffer->colorAttachments[slot] = texture;
// 	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+slot, renderer->texture2dType, id, 0);

// 	if (slot == 1) {
// 		unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
// 		glDrawBuffers(2, attachments);
// 	}
// }

void addDepthAttachment(float width, float height) {
	Framebuffer *framebuffer = renderer->currentFramebuffer;

	GLuint depthId;

	glGenRenderbuffers(1, &depthId);
	glBindRenderbuffer(GL_RENDERBUFFER, depthId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthId);

	if (framebuffer) {
		framebuffer->depthId = depthId;
	} else {
		logf("No framebuffer attach, depth buffer leaked\n");
	}
}

void addDepthStencilAttachment(float width, float height);
void addDepthStencilAttachment(float width, float height) {
	Framebuffer *framebuffer = renderer->currentFramebuffer;

	GLuint depthId;
	glGenRenderbuffers(1, &depthId);
	glBindRenderbuffer(GL_RENDERBUFFER, depthId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthId);

	if (framebuffer) {
		framebuffer->depthId = depthId;
	} else {
		logf("No framebuffer attach, depth buffer leaked\n");
	}
}

void setTargetTexturesNum(int number);
void setTargetTexturesNum(int number) {
	if (number > 2) logf("Can't set target textures above 2\n");
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(number, attachments);
}

void pushBatchVertices(
	int tint,
	Matrix3 matrix,
	float alpha,
	Texture *texture,
	Matrix3 uvMatrix,
	int flags=0,
	BlendMode blendMode=BLEND_NORMAL,
	Vec4 params=v4()
);
void pushBatchVertices(int tint, Matrix3 matrix, float alpha, Texture *texture, Matrix3 uvMatrix, int flags, BlendMode blendMode, Vec4 params) {
	alpha *= renderer->alphaStack[renderer->alphaStackNum-1];
	if (alpha == 0) return;
	// if (renderer->using3d && renderer->in3dPass) {
	// 	logf("Don't push do 2d draws while in the 3d pass\n");
	// }

	// This is where culling should happen
	if (renderer->defaultBatchNonInstancedVertsNum >= renderer->defaultBatchNonInstancedVertsMax-1) processBatchDraws();
	if (renderer->defaultBatchInstancedVertsNum >= renderer->defaultBatchInstancedVertsMax-1) processBatchDraws();

	if (renderer->batchBlendMode != blendMode) {
		processBatchDraws();
		renderer->batchBlendMode = blendMode;
	}

	int textureIndex = -1;
	{
		int openingIndex = -1;
		for (int i = 0; i < renderer->maxTextureUnits; i++) {
			if (renderer->batchTextures[i] == texture) {
				textureIndex = i;
				break;
			}
			if (openingIndex == -1 && renderer->batchTextures[i] == NULL) openingIndex = i;
		}

		if (textureIndex == -1) {
			if (openingIndex == -1) {
				processBatchDraws();
				openingIndex = 0;
			}
			renderer->batchTextures[openingIndex] = texture;
			textureIndex = openingIndex;
		}
	}

	for (int i = 0; i < 4; i++) {
		DefaultProgramNonInstancedVertex *niVert = &renderer->defaultBatchNonInstancedVerts[renderer->defaultBatchNonInstancedVertsNum++];
		if (i == 0) {
			niVert->position = v4(0, 0, 0, 0);
		} else if (i == 1) {
			niVert->position = v4(1, 0, 1, 0);
		} else if (i == 2) {
			niVert->position = v4(1, 1, 1, 1);
		} else if (i == 3) {
			niVert->position = v4(0, 1, 0, 1);
		}
	}

	DefaultProgramInstancedVertex *iVert = &renderer->defaultBatchInstancedVerts[renderer->defaultBatchInstancedVertsNum++];
	uvMatrix = renderer->flipMatrix * uvMatrix;
	iVert->matrixData[0] = uvMatrix.data[0];
	iVert->matrixData[1] = uvMatrix.data[3];
	iVert->matrixData[2] = uvMatrix.data[6];
	iVert->matrixData[3] = uvMatrix.data[1];
	iVert->matrixData[4] = uvMatrix.data[4];
	iVert->matrixData[5] = uvMatrix.data[7];

	matrix = renderer->baseMatrix2d * matrix;
	iVert->matrixData[6] = matrix.data[0];
	iVert->matrixData[7] = matrix.data[3];
	iVert->matrixData[8] = matrix.data[6];
	iVert->matrixData[9] = matrix.data[1];
	iVert->matrixData[10] = matrix.data[4];
	iVert->matrixData[11] = matrix.data[7];

	memcpy(&iVert->tintColorShiftHueTexture.x, &tint, 4);
	iVert->tintColorShiftHueTexture.w = textureIndex;

	iVert->alpha = alpha;
	iVert->flags = flags;
	iVert->params = params;

	renderer->batchIndices[renderer->batchIndicesNum++] = (renderer->defaultBatchNonInstancedVertsNum - 4) + 0;
	renderer->batchIndices[renderer->batchIndicesNum++] = (renderer->defaultBatchNonInstancedVertsNum - 4) + 1;
	renderer->batchIndices[renderer->batchIndicesNum++] = (renderer->defaultBatchNonInstancedVertsNum - 4) + 2;
	renderer->batchIndices[renderer->batchIndicesNum++] = (renderer->defaultBatchNonInstancedVertsNum - 4) + 2;
	renderer->batchIndices[renderer->batchIndicesNum++] = (renderer->defaultBatchNonInstancedVertsNum - 4) + 3;
	renderer->batchIndices[renderer->batchIndicesNum++] = (renderer->defaultBatchNonInstancedVertsNum - 4) + 0;
}

void drawRect(Rect rect, int color, int flags) {
	if (renderer->disabled) return;

	// rect = rect.inflate(1); // Compensates for AA filtered edges, needs to be disabled in pixel-art mode // It's actually really really bad to do this, maybe 1%?

	Matrix3 matrix = mat3();
	matrix.TRANSLATE(rect.x, rect.y);
	matrix.SCALE(rect.width, rect.height);

	Matrix3 uvMatrix = mat3();

	pushBatchVertices(color, matrix, 1, renderer->whiteTexture, uvMatrix, flags, BLEND_NORMAL);
}

void drawCircle(Vec2 position, float radius, int color) {
	if (renderer->disabled) return;

	Rect rect = makeCenteredSquare(position, radius*2);

	Matrix3 matrix = mat3();
	matrix.TRANSLATE(rect.x, rect.y);
	matrix.SCALE(rect.width, rect.height);

	Matrix3 uvMatrix = mat3();

	pushBatchVertices(color, matrix, 1, renderer->whiteTexture, uvMatrix, _F_CIRCLE, BLEND_NORMAL);
}

void processBatchDraws() {
	if (renderer->defaultBatchInstancedVertsNum == 0 || !renderer->defaultBatchInstancedVerts) return;

	bindVertexArray(renderer->batchVao);

	DefaultProgram *shader = &renderer->defaultProgram;
	setShaderProgram(shader->program);
	CheckGlErrorHigh();

	bindVertexBuffer(renderer->batchNonInstancedBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(DefaultProgramNonInstancedVertex) * renderer->defaultBatchNonInstancedVertsNum, renderer->defaultBatchNonInstancedVerts);

	bindVertexBuffer(renderer->batchInstancedBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(DefaultProgramInstancedVertex) * renderer->defaultBatchInstancedVertsNum, renderer->defaultBatchInstancedVerts);
	CheckGlErrorHigh();

	int texturesCount = 0;
	GLint textureIds[128] = {};
	for (int i = 0; i < renderer->maxTextureUnits; i++) {
		Texture *texture = renderer->batchTextures[i];
		if (!texture) {
			renderer->batchTextures[i] = renderer->emptyTexture;
			texture = renderer->batchTextures[i];
		} else {
			texturesCount++;
		}
		bindTextureSlot(i, texture);
		textureIds[i] = i;
	}

	renderer->debugBatchTextureCountCounts[texturesCount]++;

	for (int i = 0; i < renderer->maxTextureUnits; i++) {
		setUniform1i(shader->u_textures[i], textureIds[i]);
	}
	// setUniform1iv(shader->u_textures, renderer->maxTextureUnits, textureIds);
	setUniform1f(shader->u_time, platform->time);

	setRendererBlendMode(renderer->batchBlendMode);

	bindElementBuffer(renderer->batchElementBuffer);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(u16) * renderer->batchIndicesNum, renderer->batchIndices);
	CheckGlErrorHigh();

	renderer->drawCallsThisFrame++;
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0, renderer->batchIndicesNum/6);
	CheckGlErrorHigh();

	for (int i = 0; i < renderer->maxTextureUnits; i++) renderer->batchTextures[i] = NULL;
	// memset(renderer->batchTextures, 0, sizeof(Texture *) * renderer->maxTextureUnits);
	renderer->defaultBatchNonInstancedVertsNum = 0;
	renderer->defaultBatchInstancedVertsNum = 0;
	renderer->batchIndicesNum = 0;

	bindVertexArray(renderer->defaultVao);
}

void drawTexture(RenderTexture *renderTexture, RenderProps props) { drawTexture(renderTexture->texture, props); }
void drawTexture(Texture *texture, RenderProps props) {
	if (renderer->disabled) return;
	if (props.alpha == 0) return;
	if (props.disabled) return;
	if (!texture) Panic("drawTexture called with null texture!");

	if (props.srcWidth == 0) props.srcWidth = texture->width;
	if (props.srcHeight == 0) props.srcHeight = texture->height;
	props.matrix.SCALE(props.srcWidth, props.srcHeight);

	pushBatchVertices(props.tint, props.matrix, props.alpha, texture, props.uvMatrix, props.flags, props.blendMode, props.params);
}

void drawOutline(Texture *texture, int color, float thickness) {
	if (renderer->disabled) return;
	processBatchDraws();
	if (!texture) Panic("drawOutline called with null texture!");

	Vec4 verts[4] = {
		v4(-1, -1, 0, 0),
		v4(1, -1 , 1, 0),
		v4(1, 1  , 1, 1),
		v4(-1, 1 , 0, 1)
	};

	u16 inds[6] = {0, 1, 2, 2, 3, 0};

	bindVertexBuffer(renderer->tempVerts);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4) * 4, verts, GL_DYNAMIC_DRAW);

	bindElementBuffer(renderer->tempIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16) * 6, inds, GL_DYNAMIC_DRAW);

	OutlineProgram *shader = &renderer->outlineProgram;
	setShaderProgram(shader->program);

	glEnableVertexAttribArray(shader->a_xyuv);
	glVertexAttribPointer(shader->a_xyuv, 4, GL_FLOAT, false, sizeof(Vec4), 0);

	setUniform2f(shader->u_textureSize, texture->width, texture->height);

	int a, r, g, b;
	hexToArgb(color, &a, &r, &g, &b);
	setUniform4f(shader->u_color, r/255.0, g/255.0, b/255.0, a/255.0);
	setUniform1f(shader->u_thickness, thickness);

	bindTextureSlot(0, texture);
	setUniform1i(shader->u_texture, 0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(shader->a_xyuv);

	CheckGlErrorHigh();
}

void drawBlurred(Texture *texture, Matrix3 matrix, bool horizontal) {
	if (renderer->disabled) return;
	processBatchDraws();

	Vec2 verts[4];
	verts[0] = v2(0, 0);
	verts[1] = v2(1, 0);
	verts[2] = v2(1, 1);
	verts[3] = v2(0, 1);

	Vec2 uvs[4];
	uvs[0] = v2(0, 0);
	uvs[1] = v2(1, 0);
	uvs[2] = v2(1, 1);
	uvs[3] = v2(0, 1);

	u16 inds[6] = {0, 1, 2, 2, 3, 0};
	int indsNum = 6;

	matrix = renderer->baseMatrix2d * matrix;

	int vertsNum = 0;
	for (int i = 0; i < indsNum; i++) {
		if (inds[i]+1 > vertsNum) vertsNum = inds[i]+1;
	}

	int vertSize = 4 * sizeof(float);
	int dataSize = vertsNum * vertSize;
	float *data = (float *)frameMalloc(dataSize);
	int dataNum = 0;
	for (int i = 0; i < vertsNum; i++) {
		data[dataNum++] = verts[i].x;
		data[dataNum++] = verts[i].y;
		data[dataNum++] = uvs[i].x;
		data[dataNum++] = uvs[i].y;
	}

	bindVertexBuffer(renderer->tempVerts);
	glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_DYNAMIC_DRAW);

	bindElementBuffer(renderer->tempIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16) * indsNum, inds, GL_DYNAMIC_DRAW);

	GaussianBlurProgram *shader = &renderer->gaussianBlurProgram;
	setShaderProgram(shader->program);
	CheckGlErrorHigh();

	glEnableVertexAttribArray(shader->a_position);
	glVertexAttribPointer(shader->a_position, 4, GL_FLOAT, false, vertSize, 0);
	CheckGlErrorHigh();

	setUniformMatrix3fv(shader->u_matrix, 1, false, (float *)matrix.data);
	setUniform1i(shader->u_horizontal, horizontal);

	bindTextureSlot(0, texture);
	if (texture) setUniform1i(shader->u_texture, 0);
	CheckGlErrorHigh();

	// glDrawArrays(GL_TRIANGLES, 0, vertsNum);
	glDrawElements(GL_TRIANGLES, indsNum, GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(shader->a_position);
	CheckGlErrorHigh();
	// logf("Drawn\n");
}

void drawRetroTexture(Texture *texture, Matrix3 matrix, Vec2 gridResolution=v2(100, 100), float gridAlpha=0.5);
void drawRetroTexture(Texture *texture, Matrix3 matrix, Vec2 gridResolution, float gridAlpha) {
	u16 inds[6] = {0, 1, 2, 2, 3, 0};
	Vec4 data[4] = {
		v4(0, 0, 0, 0), 
		v4(1, 0, 1, 0), 
		v4(1, 1, 1, 1), 
		v4(0, 1, 0, 1), 
	};
	// draw2dMesh((float *)data, 4, inds, 6, matrix, texture);
	{
		float alpha = 1;
		int vertsNum = 4;
		int indsNum = 6;
		matrix = renderer->baseMatrix2d * matrix;

		processBatchDraws();

		bindVertexBuffer(renderer->tempVerts);
		glBufferData(GL_ARRAY_BUFFER, vertsNum * sizeof(float) * 4, data, GL_DYNAMIC_DRAW);

		bindElementBuffer(renderer->tempIndices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16) * indsNum, inds, GL_DYNAMIC_DRAW);

		RetroProgram *shader = &renderer->retroProgram;
		setShaderProgram(shader->program);
		CheckGlErrorHigh();

		glEnableVertexAttribArray(shader->a_position);
		glVertexAttribPointer(shader->a_position, 4, GL_FLOAT, false, 0, 0);
		CheckGlErrorHigh();

		setUniformMatrix3fv(shader->u_matrix, 1, false, (float *)matrix.data);
		setUniform1f(shader->u_alpha, alpha);
		setUniform2f(shader->u_gridResolution, gridResolution.x, gridResolution.y);
		setUniform1f(shader->u_gridAlpha, gridAlpha);

		bindTextureSlot(0, texture);
		if (texture) setUniform1i(shader->u_texture, 0);
		CheckGlErrorHigh();

		glDrawElements(GL_TRIANGLES, indsNum, GL_UNSIGNED_SHORT, 0);

		glDisableVertexAttribArray(shader->a_position);
		CheckGlErrorHigh();
	}
}

void drawSimpleTexture(RenderTexture *texture) { drawSimpleTexture(texture->texture); };
void drawSimpleTexture(Texture *texture) {
	Matrix3 matrix = mat3();
	matrix.SCALE(texture->width, texture->height);
	drawSimpleTexture(texture, matrix);
}

void drawSimpleTexture(RenderTexture *texture, Matrix3 matrix, Vec2 uv0=v2(0, 0), Vec2 uv1=v2(1, 1), float alpha=1) { drawSimpleTexture(texture->texture, matrix, uv0, uv1); }
void drawSimpleTexture(Texture *texture, Matrix3 matrix, Vec2 uv0, Vec2 uv1, float alpha) {
	// if (platform->usingSkia) {
	// 	RenderProps props = newRenderProps();
	// 	props.matrix = matrix;
	// 	props.srcWidth = props.srcHeight = 1;
	// 	drawTexture(texture, props);
	// 	return;
	// }

	u16 inds[6] = {0, 1, 2, 2, 3, 0};
	Vec4 data[4] = {
		v4(0, 0, uv0.x, uv0.y), 
		v4(1, 0, uv1.x, uv0.y), 
		v4(1, 1, uv1.x, uv1.y), 
		v4(0, 1, uv0.x, uv1.y), 
	};
#if 1
	draw2dMesh((float *)data, 4, inds, 6, matrix, texture, alpha);
#else
	{
		int vertsNum = 4;
		int indsNum = 6;
		matrix = renderer->baseMatrix2d * matrix;

		processBatchDraws();

		bindVertexBuffer(renderer->tempVerts);
		glBufferData(GL_ARRAY_BUFFER, vertsNum * sizeof(float) * 4, data, GL_DYNAMIC_DRAW);

		bindElementBuffer(renderer->tempIndices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16) * indsNum, inds, GL_DYNAMIC_DRAW);

		Mesh2dProgram *shader = &renderer->mesh2dProgram;
		setShaderProgram(shader->program);
		CheckGlErrorHigh();

		glEnableVertexAttribArray(shader->a_position);
		glVertexAttribPointer(shader->a_position, 4, GL_FLOAT, false, 0, 0);
		CheckGlErrorHigh();

		setUniformMatrix3fv(shader->u_matrix, 1, false, (float *)matrix.data);
		setUniform1f(shader->u_alpha, alpha);

		bindTextureSlot(0, texture);
		if (texture) setUniform1i(shader->u_texture, 0);
		CheckGlErrorHigh();

		glDrawElements(GL_TRIANGLES, indsNum, GL_UNSIGNED_SHORT, 0);

		glDisableVertexAttribArray(shader->a_position);
		CheckGlErrorHigh();
	}
#endif
}

void draw2dMesh(Vec2 *verts, Vec2 *uvs, u16 *inds, int indsNum, Matrix3 matrix, Texture *texture, float alpha) {
	int vertsNum = 0;
	for (int i = 0; i < indsNum; i++) {
		if (inds[i]+1 > vertsNum) vertsNum = inds[i]+1;
	}

	int vertSize = 4 * sizeof(float);
	int dataSize = vertsNum * vertSize;
	float *data = (float *)frameMalloc(dataSize);
	int dataNum = 0;
	for (int i = 0; i < vertsNum; i++) {
		data[dataNum++] = verts[i].x;
		data[dataNum++] = verts[i].y;
		data[dataNum++] = uvs[i].x;
		data[dataNum++] = uvs[i].y;
	}

	draw2dMesh(data, vertsNum, inds, indsNum, matrix, texture);
}

void draw2dMesh(float *xyuvs, int xyuvsNum, u16 *inds, int indsNum, Matrix3 matrix, Texture *texture, float alpha) {
	matrix = renderer->baseMatrix2d * matrix;
	renderer->debugBatchTextureCountCounts[1]++;

	processBatchDraws();

	bindVertexBuffer(renderer->tempVerts);
	glBufferData(GL_ARRAY_BUFFER, xyuvsNum * sizeof(float) * 4, xyuvs, GL_DYNAMIC_DRAW);

	bindElementBuffer(renderer->tempIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16) * indsNum, inds, GL_DYNAMIC_DRAW);

	Mesh2dProgram *shader = &renderer->mesh2dProgram;
	setShaderProgram(shader->program);
	CheckGlErrorHigh();

	glEnableVertexAttribArray(shader->a_position);
	glVertexAttribPointer(shader->a_position, 4, GL_FLOAT, false, 0, 0);
	CheckGlErrorHigh();

	setUniformMatrix3fv(shader->u_matrix, 1, false, (float *)matrix.data);
	setUniform1f(shader->u_alpha, alpha);

	bindTextureSlot(0, texture);
	if (texture) setUniform1i(shader->u_texture, 0);
	CheckGlErrorHigh();

	glDrawElements(GL_TRIANGLES, indsNum, GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(shader->a_position);
	CheckGlErrorHigh();
}

void drawFxaaTexture(RenderTexture *texture, Matrix3 matrix) { drawFxaaTexture(texture->texture, matrix); }
void drawFxaaTexture(Texture *texture, Matrix3 matrix) {
	processBatchDraws();

	u16 inds[6] = {0, 1, 2, 2, 3, 0};
	Vec4 data[4] = {
		v4(0, 0, 0, 0), 
		v4(1, 0, 1, 0), 
		v4(1, 1, 1, 1), 
		v4(0, 1, 0, 1), 
	};

	matrix = renderer->baseMatrix2d * matrix;

	bindVertexBuffer(renderer->tempVerts);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4) * 4, data, GL_DYNAMIC_DRAW);

	bindElementBuffer(renderer->tempIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16) * 6, inds, GL_DYNAMIC_DRAW);

	FxaaProgram *shader = &renderer->fxaaProgram;
	setShaderProgram(shader->program);
	CheckGlErrorHigh();

	glEnableVertexAttribArray(shader->a_position);
	glVertexAttribPointer(shader->a_position, 4, GL_FLOAT, false, sizeof(Vec4), 0);
	CheckGlErrorHigh();

	setUniformMatrix3fv(shader->u_matrix, 1, false, (float *)matrix.data);
	setUniform2f(shader->u_destSize, texture->width, texture->height);

	bindTextureSlot(0, texture);
	if (texture) setUniform1i(shader->u_texture, 0);
	CheckGlErrorHigh();

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(shader->a_position);
	CheckGlErrorHigh();
}

void drawPost3dTexture(Texture *texture, Matrix3 matrix, int tint) {
	processBatchDraws();

	u16 inds[6] = {0, 1, 2, 2, 3, 0};
	Vec4 data[4] = {
		v4(0, 0, 0, 0), 
		v4(1, 0, 1, 0), 
		v4(1, 1, 1, 1), 
		v4(0, 1, 0, 1), 
	};

	matrix = renderer->baseMatrix2d * matrix;

	bindVertexBuffer(renderer->tempVerts);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4) * 4, data, GL_DYNAMIC_DRAW);

	bindElementBuffer(renderer->tempIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16) * 6, inds, GL_DYNAMIC_DRAW);

	Post3dProgram *shader = &renderer->post3dProgram;
	setShaderProgram(shader->program);
	CheckGlErrorHigh();

	glEnableVertexAttribArray(shader->a_position);
	glVertexAttribPointer(shader->a_position, 4, GL_FLOAT, false, sizeof(Vec4), 0);
	CheckGlErrorHigh();

	setUniformMatrix3fv(shader->u_matrix, 1, false, (float *)matrix.data);
	setUniform2f(shader->u_destSize, texture->width, texture->height);

	int a, r, g, b;
	hexToArgb(tint, &a, &r, &g, &b);
	setUniform4f(shader->u_tint, r/255.0, g/255.0, b/255.0, a/255.0);

	bindTextureSlot(0, texture);
	if (texture) setUniform1i(shader->u_texture, 0);
	CheckGlErrorHigh();

	// glDrawArrays(GL_TRIANGLES, 0, vertsNum);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(shader->a_position);
	CheckGlErrorHigh();
	// logf("Drawn\n");
}

void drawVignette(Vec2 vignettePos, float softness, float size, int color, float alpha);
void drawVignette(Vec2 vignettePos, float softness, float size, int color, float alpha) {
	// Vec2 vignettePos = v2(0.5, 0.5);
	// float softness = 0.6;
	// float size = 1;
	// int color = 0xFFFF0000;

	processBatchDraws();

	u16 inds[6] = {0, 1, 2, 2, 3, 0};
	Vec4 data[4] = {
		v4(0, 0, 0, 0), 
		v4(1, 0, 1, 0), 
		v4(1, 1, 1, 1), 
		v4(0, 1, 0, 1), 
	};

	bindVertexBuffer(renderer->tempVerts);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4) * 4, data, GL_DYNAMIC_DRAW);

	bindElementBuffer(renderer->tempIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16) * 6, inds, GL_DYNAMIC_DRAW);

	VignetteProgram *shader = &renderer->vignetteProgram;
	setShaderProgram(shader->program);
	CheckGlErrorHigh();

	glEnableVertexAttribArray(shader->a_position);
	glVertexAttribPointer(shader->a_position, 4, GL_FLOAT, false, sizeof(Vec4), 0);
	CheckGlErrorHigh();

	Matrix3 matrix = mat3();
	matrix.SCALE(platform->windowWidth, platform->windowHeight);
	matrix = renderer->baseMatrix2d * matrix;
	setUniformMatrix3fv(shader->u_matrix, 1, false, (float *)matrix.data);

	setUniform2f(shader->u_vignettePos, vignettePos.x, vignettePos.y);
	setUniform3f(shader->u_softnessSizeAlpha, softness, size, alpha);

	Texture *texture = renderer->whiteTexture;
	bindTextureSlot(0, texture);
	if (texture) setUniform1i(shader->u_texture, 0);
	CheckGlErrorHigh();

	int a, r, g, b;
	hexToArgb(color, &a, &r, &g, &b);
	setUniform4f(shader->u_color, r/255.0, g/255.0, b/255.0, a/255.0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(shader->a_position);
	CheckGlErrorHigh();
	// logf("Drawn\n");
}

void drawPixelArtFilterTexture(RenderTexture *texture, Matrix3 matrix, Vec2 uv0=v2(0, 0), Vec2 uv1=v2(1, 1)) { drawPixelArtFilterTexture(texture->texture, matrix, uv0, uv1); }
void drawPixelArtFilterTexture(Texture *texture, Matrix3 matrix, Vec2 uv0, Vec2 uv1) {
	processBatchDraws();

	u16 inds[6] = {0, 1, 2, 2, 3, 0};
	Vec4 data[4] = {
		v4(0, 0, uv0.x, uv0.y), 
		v4(1, 0, uv1.x, uv0.y), 
		v4(1, 1, uv1.x, uv1.y), 
		v4(0, 1, uv0.x, uv1.y), 
	};

	matrix = renderer->baseMatrix2d * matrix;

	bindVertexBuffer(renderer->tempVerts);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4) * 4, data, GL_DYNAMIC_DRAW);

	bindElementBuffer(renderer->tempIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u16) * 6, inds, GL_DYNAMIC_DRAW);

	PixelArtFilterProgram *shader = &renderer->pixelArtFilterProgram;
	setShaderProgram(shader->program);
	CheckGlErrorHigh();

	glEnableVertexAttribArray(shader->a_position);
	glVertexAttribPointer(shader->a_position, 4, GL_FLOAT, false, sizeof(Vec4), 0);
	CheckGlErrorHigh();

	setUniformMatrix3fv(shader->u_matrix, 1, false, (float *)matrix.data);

	bindTextureSlot(0, texture);
	if (texture) setUniform1i(shader->u_texture, 0);
	CheckGlErrorHigh();

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(shader->a_position);
	CheckGlErrorHigh();
}

void refreshGlobalMatrices() {
	renderer->baseMatrix2d = getProjectionMatrix(renderer->currentViewport.width, renderer->currentViewport.height);

	renderer->currentCameraMatrix = mat3();
	for (int i = 0; i < renderer->camera2dStackNum; i++) renderer->currentCameraMatrix *= renderer->camera2dStack[i];
	// renderer->currentCameraMatrix = renderer->camera2dStack[renderer->camera2dStackNum-1];

	renderer->baseMatrix2d *= renderer->currentCameraMatrix;
}

void setScissor(Rect rect) {
	processBatchDraws();

	if (rect.width == -1) {
		clearScissor();
		return;
	}

	glEnable(GL_SCISSOR_TEST);
	renderer->currentScissor = rect;
	glScissor(rect.x, renderer->currentViewport.height - rect.height - rect.y, rect.width, rect.height);
}

void clearScissor() {
	processBatchDraws();

	renderer->currentScissor = makeRect(0, 0, -1, -1);
	glDisable(GL_SCISSOR_TEST);
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

void pushAlpha(float value) {
	if (renderer->alphaStackNum > ALPHA_STACK_MAX-1) Panic("alpha overflow");
	renderer->alphaStack[renderer->alphaStackNum++] = value;
}

void popAlpha() {
	if (renderer->alphaStackNum <= 1) Panic("alpha underflow");
	renderer->alphaStackNum--;
}

void endRenderingFrame() {
	if (!renderer) return;

#if DO_GL_LOG
	for (;;) {
		GLint msgMaxLen = 0;
		glGetIntegerv(GL_MAX_DEBUG_MESSAGE_LENGTH, &msgMaxLen);

		char *msg = frameMalloc(msgMaxLen); 
		GLenum source;
		GLenum type;
		GLenum severity;
		GLenum id;
		GLsizei length;
		GLuint count = glGetDebugMessageLog(1, msgMaxLen, &source, &type, &id, &severity, &length, msg);

		if (count == 0) break;
		logf("Opengl msg: %s\n", msg);
	}
#endif

	processBatchDraws();
	CheckGlErrorLow();

	renderer->drawCallsLastFrame = renderer->drawCallsThisFrame;
	renderer->drawCallsThisFrame = 0;

	renderer->prevPushesCulled = renderer->pushesCulled;
	renderer->pushesCulled = 0;

	renderer->prevPushesProcessed = renderer->pushesProcessed;
	renderer->pushesProcessed = 0;

	renderer->tempTextureBufferSize = 0;
	if (renderer->tempTextureBuffer) {
		free(renderer->tempTextureBuffer);
		renderer->tempTextureBuffer = NULL;
	}

	renderer->tempTextureRowBufferSize = 0;
	if (renderer->tempTextureRowBuffer) {
		free(renderer->tempTextureRowBuffer);
		renderer->tempTextureRowBuffer = NULL;
	}

	//@robustness One day warn if this isn't actually being called every frame
}

RenderTexture *createRenderTexture(int width, int height, void *data, int flags) {
	RenderTexture *renderTexture = (RenderTexture *)zalloc(sizeof(RenderTexture));
	renderTexture->texture = createTexture(width, height, data, flags);
	renderTexture->width = width;
	renderTexture->height = height;
	return renderTexture;
}

Texture *createTexture(int width, int height, void *data, int flags) {
	Texture *texture = (Texture *)zalloc(sizeof(Texture));
	texture->width = width;
	texture->height = height;
	texture->internalFormat = GL_RGBA8;

	if (flags & _F_TD_RGB16F) {
		texture->internalFormat = GL_RGB16F;
	}

	if (flags & _F_TD_SRGB8) {
		texture->internalFormat = GL_SRGB8;
	}

	glGenTextures(1, &texture->id);

	bindTextureSlot(0, texture);

	if (renderer->texture2dType == GL_TEXTURE_2D) {
		glTexParameteri(renderer->texture2dType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(renderer->texture2dType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		texture->smooth = true;
	}
	glTexParameteri(renderer->texture2dType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(renderer->texture2dType, GL_TEXTURE_WRAP_T, GL_REPEAT); 
	glTexImage2D(renderer->texture2dType, 0, texture->internalFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	CheckGlErrorHigh();

	if (data) setTextureData(texture, data, width, height, flags);
	return texture;
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

void setTextureSmooth(Texture *texture, bool smooth);
void setTextureSmooth(RenderTexture *texture, bool smooth) { setTextureSmooth(texture->texture, smooth); }
void setTextureSmooth(Texture *texture, bool smooth) {
	if (texture->smooth == smooth) return;

	bindTextureSlot(0, texture);

	if (smooth) {
		glTexParameteri(renderer->texture2dType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(renderer->texture2dType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	} else {
		glTexParameteri(renderer->texture2dType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(renderer->texture2dType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	texture->smooth = smooth;
}

void setTextureClamped(Texture *texture, bool clamped);
void setTextureClamped(RenderTexture *texture, bool clamped) { setTextureClamped(texture->texture, clamped); }
void setTextureClamped(Texture *texture, bool clamped) {
	if (texture->clamped == clamped) return;

	bindTextureSlot(0, texture);

	if (clamped) {
		glTexParameteri(renderer->texture2dType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(renderer->texture2dType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
	} else {
		glTexParameteri(renderer->texture2dType, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(renderer->texture2dType, GL_TEXTURE_WRAP_T, GL_REPEAT); 
	}

	texture->clamped = clamped;
}

void setTextureData(RenderTexture *renderTexture, void *data, int width, int height, int flags);
void setTextureData(RenderTexture *renderTexture, void *data, int width, int height, int flags) { // I probably don't even need width and height here
	setTextureData(renderTexture->texture, data, width, height, flags);
}

void setTextureData(Texture *texture, void *data, int width, int height, int flags) { // I probably don't even need width and height here //@todo remove width/height params
	int neededTextureBufferSize = width * height * 4;
	if (neededTextureBufferSize > renderer->tempTextureBufferSize) {
		if (renderer->tempTextureBuffer) free(renderer->tempTextureBuffer);

		renderer->tempTextureBufferSize = neededTextureBufferSize;
		renderer->tempTextureBuffer = malloc(renderer->tempTextureBufferSize);
	}

	void *newData = renderer->tempTextureBuffer;
	memcpy(newData, data, width * height * 4);
	data = newData;

	if ((flags & _F_TD_SKIP_PREMULTIPLY) == 0) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				u8 a, r, g, b;
				int index = (y*width+x) * 4;
				a = ((u8 *)data)[index + 3];
				r = ((u8 *)data)[index + 2];
				g = ((u8 *)data)[index + 1];
				b = ((u8 *)data)[index + 0];

				r *= (float)a/255.0;
				g *= (float)a/255.0;
				b *= (float)a/255.0;

				((u8 *)data)[index + 3] = a;
				((u8 *)data)[index + 2] = r;
				((u8 *)data)[index + 1] = g;
				((u8 *)data)[index + 0] = b;
			}
		}
	}

	if (flags & _F_TD_FLIP_Y) {
		int neededTextureRowBufferSize = texture->width * 4;
		if (neededTextureRowBufferSize > renderer->tempTextureRowBufferSize) {
			if (renderer->tempTextureRowBuffer) free(renderer->tempTextureRowBuffer);

			renderer->tempTextureRowBufferSize = neededTextureRowBufferSize;
			renderer->tempTextureRowBuffer = malloc(renderer->tempTextureRowBufferSize);
		}

		u8 *tempRow = (u8 *)renderer->tempTextureRowBuffer;
		for (int y = 0; y < texture->height/2; y++) {
			int curTopRow = y;
			int curBottomRow = texture->height - y - 1;
			u8 *topRowStart = (u8 *)data + curTopRow * texture->width * 4;
			u8 *bottomRowStart = (u8 *)data + curBottomRow * texture->width * 4;

			memcpy(tempRow, topRowStart, texture->width * 4);
			memcpy(topRowStart, bottomRowStart, texture->width * 4);
			memcpy(bottomRowStart, tempRow, texture->width * 4);
		}
	}

	bindTextureSlot(0, texture);

	glTexSubImage2D(renderer->texture2dType, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	CheckGlErrorHigh();
}

u8 *getTextureData(Texture *texture, int flags) {
	if (!texture) return NULL;

	u8 *data = (u8 *)zalloc(texture->width * texture->height * 4);
	bindTextureSlot(0, texture);
	glGetTexImage(renderer->texture2dType, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	CheckGlErrorLow();

	if (flags & _F_TD_FLIP_Y) {
		u8 *tempRow = (u8 *)frameMalloc(texture->width * 4);
		for (int y = 0; y < texture->height/2; y++) {
			int curTopRow = y;
			int curBottomRow = texture->height - y - 1;
			u8 *topRowStart = (u8 *)data + curTopRow * texture->width * 4;
			u8 *bottomRowStart = (u8 *)data + curBottomRow * texture->width * 4;

			memcpy(tempRow, topRowStart, texture->width * 4);
			memcpy(topRowStart, bottomRowStart, texture->width * 4);
			memcpy(bottomRowStart, tempRow, texture->width * 4);
		}
	}

	return data;
}

u8 *getFramebufferData(Rect rect) {
	// glReadBuffer(GL_COLOR_ATTACHMENT0);
	u8 *data = (u8 *)zalloc(rect.width * rect.height * 4);
	glReadPixels(rect.x, rect.y, rect.width, rect.height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	CheckGlErrorLow();
	return data;
}

void destroyTexture(RenderTexture *renderTexture) {
	destroyTexture(renderTexture->texture);
	free(renderTexture);
}

void destroyTexture(Texture *texture) {
	if (texture->path) logf("Texture %s was deleted, but is still in the hashmap\n", texture->path);
	processBatchDraws();

	for (int i = 0; i < BOUND_TEXTURES_MAX; i++) {
		if (renderer->boundTextures[i] == texture->id) bindTextureSlot(i, NULL);
	}

	glDeleteTextures(1, &texture->id);
	free(texture);
}

void resetRenderContext();
void resetRenderContext() {
	bindVertexArray(renderer->defaultVao);
	renderer->curShaderProgram = NULL;
	renderer->currentViewport = makeRect();
	setRendererBlendMode(BLEND_NORMAL);
	glBindSampler(0, 0);
}

void setShaderProgram(GLuint program) {
	if (renderer->curShaderProgram == program) return;

	glUseProgram(program);
	renderer->curShaderProgram = program;
}

void bindVertexBuffer(GLuint buffer) {
	// if (renderer->currentArrayBuffer == buffer) return;
	renderer->currentArrayBuffer = buffer;
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
}

void bindElementBuffer(GLuint buffer) {
	// if (renderer->currentElementBuffer == buffer) return;
	renderer->currentElementBuffer = buffer;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
}

void bindUniformBuffer(GLuint buffer) {
	// if (renderer->currentUniformBuffer == buffer) return;
	renderer->currentUniformBuffer = buffer;
	glBindBuffer(GL_UNIFORM_BUFFER, buffer);
}

void bindVertexArray(GLuint array) {
	// if (renderer->currentVao == array) return;
	renderer->currentVao = array;
	glBindVertexArray(array);
}

void setViewport(float x, float y, float w, float h) {
	if (renderer->currentViewport.x == x && renderer->currentViewport.y == y && renderer->currentViewport.width == w && renderer->currentViewport.height == h) return;

	renderer->currentViewport = makeRect(x, y, w, h);
	glViewport(x, y, w, h);
	refreshGlobalMatrices();

	CheckGlErrorHigh();
}

void setRendererBlendMode(BlendMode blendMode) {
	///
	/// void glBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
	///
	if (blendMode == BLEND_NORMAL) {
		// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);
	} else if (blendMode == BLEND_MULTIPLY) {
		glBlendFuncSeparate(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA, GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
	} else if (blendMode == BLEND_SCREEN) {
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	} else if (blendMode == BLEND_ADD) {
		// glBlendFuncSeparate(GL_SRC_ALPHA, GL_DST_ALPHA, GL_ONE, GL_ONE);
		glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
		glBlendEquation(GL_FUNC_ADD);
	} else if (blendMode == BLEND_INVERT) {
		glBlendFuncSeparate(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	} else if (blendMode == BLEND_SKIA) {
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);
	}
}

void enableVertexAttribArray(int index) {
	// if (renderer->vertexAttribsEnabled[index]) return;
	renderer->vertexAttribsEnabled[index] = true;
	glEnableVertexAttribArray(index);
}

void disableVertexAttribArray(int index) {
	// if (!renderer->vertexAttribsEnabled[index]) return;
	renderer->vertexAttribsEnabled[index] = false;
	glDisableVertexAttribArray(index);
}

void bindTextureSlot(int slot, Texture *texture) {
	int id = 0;
	if (texture) id = texture->id;

	bindTextureSlotId(slot, id);
}

void bindTextureSlotId(int slot, int id) {
	// if (renderer->boundTextures[slot] != id) { // Doesn't work because of something to do with multi render targets
		renderer->boundTextures[slot] = id;
		glActiveTexture(GL_TEXTURE0+slot);
		glBindTexture(GL_TEXTURE_2D, id);
	// }
}

void checkGlError(int lineNum) {
	if (renderer->disabled) return;
	return;

	for (;;) {
		GLenum err = glGetError();
		if (err == GL_NO_ERROR) break;

		if (renderer->errorCount < 10) {
			logf("Gl error: 0x%x(%d) at line %d\n", err, err, lineNum);
			renderer->errorCount++;

			if (renderer->errorCount == 10) {
				logf("Max gl errors exceeded, no more will be shown\n");
				break;
			}
		}
	}
}
#endif
