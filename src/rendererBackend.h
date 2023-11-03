//@todo renderer->disabled?
#define RENDERER_BACKEND_MAX_DRAWS_PER_BATCH 20000

#define _F_TD_FLIP_Y           (1 << 1)
#define _F_TD_SKIP_PREMULTIPLY (1 << 2)
// #define _F_TD_SRGB8            (1 << 3)
#define _F_TD_RGB16F           (1 << 4)
#define _F_TD_RGBA32           (1 << 5)

enum BlendMode {
	BLEND_INVALID=0,
	BLEND_NORMAL,
	BLEND_MULTIPLY,
	BLEND_SCREEN,
	BLEND_ADD,
	BLEND_INVERT,
	BLEND_RECOMMENDED,
};

enum ShaderUniformType {
	SHADER_UNIFORM_FLOAT,
	SHADER_UNIFORM_VEC2,
	SHADER_UNIFORM_VEC3,
	SHADER_UNIFORM_VEC4,
	SHADER_UNIFORM_MATRIX4,
	SHADER_UNIFORM_INT,
	SHADER_UNIFORM_IVEC2,
	SHADER_UNIFORM_IVEC3,
	SHADER_UNIFORM_IVEC4,
	SHADER_UNIFORM_TEXTURE,
};

struct BackendShader;
struct BackendTexture;

#pragma pack(push, 1)
struct GpuVertex {
	Vec3 position;
	Vec2 uv;
	Vec3 normal;
	Vec4 color;
};
#pragma pack(pop)

void backendInit();
void backendStartFrame();
void backendEndFrame();

void backendClearRenderer(int color);
void backendLoadShader(BackendShader *backendShader, char *vs, char *fs);

int backendGetVertexAttribLocation(BackendShader *backendShader, char *attribName);
int backendGetUniformLocation(BackendShader *backendShader, char *uniformName);
void backendSetShaderUniform(BackendShader *backendShader, int loc, void *ptr, ShaderUniformType type, int count);

void backendSetShader(BackendShader *backendShader);
void backendSetTexture(BackendTexture *backendTexture, int slot=0);
void backendFlush();

BackendTexture backendCreateTexture(int width, int height, int flags);
void backendSetTextureSmooth(bool smooth);
void backendSetTextureClampedX(bool clamped);
void backendSetTextureClampedY(bool clamped);
void backendSetTextureData(void *data, int width, int height, int flags);
u8 *backendGetTextureData(BackendTexture *backendTexture);
void backendDestroyTexture(BackendTexture *backendTexture);

void backendSetTargetTexture(BackendTexture *backendTexture);

void backendSetScissor(Rect rect);
void backendEndScissor();
void backendSetBlendMode(BlendMode blendMode);
void backendSetDepthTest(bool enabled);
void backendSetDepthMask(bool enabled);
void backendSetBlending(bool enabled);
void backendSetBackfaceCulling(bool enabled);

void backendDrawVerts(GpuVertex *verts, int vertsNum);

void backendResetRenderContext();

void backendImGuiTexture(BackendTexture *backendTexture, Vec2 scale);
bool backendImGuiImageButton(BackendTexture *backendTexture);
