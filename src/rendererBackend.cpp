//@todo renderer->disabled?

#define _F_TD_FLIP_Y           (1 << 1)
#define _F_TD_SKIP_PREMULTIPLY (1 << 2)
// #define _F_TD_SRGB8            (1 << 3)
#define _F_TD_RGB16F           (1 << 4)
#define _F_TD_RGBA32           (1 << 5)

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
	SHADER_UNIFORM_SAMPLER2D
};

struct BackendShader {
	Raylib::Shader raylibShader;
};

struct BackendTexture {
	Raylib::Texture2D raylibTexture;
};

void backendClearRenderer(int color);
void backendLoadShader(BackendShader *backendShader, char *vs, char *fs);

int backendGetUniformLocation(BackendShader *backendShader, char *uniformName);
int backendGetVertextAttribLocation(BackendShader *backendShader, char *attribName);
bool backendSetShaderUniform(BackendShader *backendShader, int loc, void *ptr, ShaderUniformType type, int count);

void backendStartShader(BackendShader *backendShader);
void backendEndShader();
void backendFlush();

BackendTexture backendCreateTexture(int width, int height, int flags);
void backendSetTextureSmooth(BackendTexture *backendTexture, bool smooth);
void backendSetTextureClampedX(BackendTexture *backendTexture, bool clamped);
void backendSetTextureClampedY(BackendTexture *backendTexture, bool clamped);
void backendSetTextureData(BackendTexture *backendTexture, void *data, int width, int height, int flags);
u8 *backendGetTextureData(BackendTexture *backendTexture);
void backendDestroyTexture(BackendTexture *backendTexture);

void backendSetTargetTexture(BackendTexture *backendTexture);

void backendSetAlpha(float alpha);
void backendSetCamera2d(Matrix3 camera2d);
void backendSetScissor(Rect rect);
void backendEndScissor();

void backendDrawTexture(BackendTexture *backendTexture, Matrix3 matrix, Vec2 uv0, Vec2 uv1, Vec4i tints, float alpha, int flags);
void drawTexturedQuad(int textureId, Vec3 *verts, Vec2 *uvs, int *colors);

Raylib::Color toRaylibColor(int color) { return Raylib::GetColor(argbToRgba(color)); }
/// FUNCTIONS ^

float _current2dDrawDepth;
int _currentDrawCount;
#define RENDERER_BACKEND_MAX_DRAWS_PER_BATCH 20000

float _backendAlpha = 1;
Matrix3 _backendCamera2d = mat3();

int _renderTextureFbId;

void backendInit() {
	_renderTextureFbId = Raylib::rlLoadFramebuffer(0, 0); // The size here doesn't mater??
}

void backendClearRenderer(int color) {
	Raylib::ClearBackground(toRaylibColor(color));
}

void backendLoadShader(BackendShader *backendShader, char *vs, char *fs) {
	backendShader->raylibShader = Raylib::LoadShaderFromMemory(vs, fs);
}

int backendGetUniformLocation(BackendShader *backendShader, char *uniformName) {
	int loc = glGetUniformLocation(backendShader->raylibShader.id, uniformName);
	return loc;
}

int backendGetVertextAttribLocation(BackendShader *backendShader, char *attribName) {
	int loc = glGetAttribLocation(backendShader->raylibShader.id, attribName);
	return loc;
}

bool backendSetShaderUniform(BackendShader *backendShader, int loc, void *ptr, ShaderUniformType type, int count) {
	Raylib::rlEnableShader(backendShader->raylibShader.id);

	if (type == SHADER_UNIFORM_FLOAT) {
		Raylib::rlSetUniform(loc, ptr, Raylib::SHADER_UNIFORM_FLOAT, count);
	} else if (type == SHADER_UNIFORM_VEC2) {
		Raylib::rlSetUniform(loc, ptr, Raylib::SHADER_UNIFORM_VEC2, count);
	} else if (type == SHADER_UNIFORM_VEC3) {
		Raylib::rlSetUniform(loc, ptr, Raylib::SHADER_UNIFORM_VEC3, count);
	} else if (type == SHADER_UNIFORM_VEC4) {
		Raylib::rlSetUniform(loc, ptr, Raylib::SHADER_UNIFORM_VEC4, count);
	} else if (type == SHADER_UNIFORM_MATRIX4) {
		glUniformMatrix4fv(loc, count, true, (float *)ptr);
	} else if (type == SHADER_UNIFORM_INT) {
		Raylib::rlSetUniform(loc, ptr, Raylib::SHADER_UNIFORM_INT, count);
	} else if (type == SHADER_UNIFORM_IVEC2) {
		Raylib::rlSetUniform(loc, ptr, Raylib::SHADER_UNIFORM_IVEC2, count);
	} else if (type == SHADER_UNIFORM_IVEC3) {
		Raylib::rlSetUniform(loc, ptr, Raylib::SHADER_UNIFORM_IVEC3, count);
	} else if (type == SHADER_UNIFORM_IVEC4) {
		Raylib::rlSetUniform(loc, ptr, Raylib::SHADER_UNIFORM_IVEC4, count);
	} else if (type == SHADER_UNIFORM_SAMPLER2D) {
		Raylib::rlSetUniform(loc, ptr, Raylib::SHADER_UNIFORM_SAMPLER2D, count);
	} else {
		logf("Invalid shader uniform type\n");
	}
	return false;
}

void backendStartShader(BackendShader *backendShader) {
	backendFlush();
	Raylib::rlSetShader(backendShader->raylibShader.id, backendShader->raylibShader.locs);
}

void backendEndShader() {
	Raylib::rlSetShader(Raylib::rlGetShaderIdDefault(), Raylib::rlGetShaderLocsDefault());
}

void backendFlush() {
	_currentDrawCount = 0;
	_current2dDrawDepth = -1;
	Raylib::rlDrawRenderBatchActive();
}

BackendTexture backendCreateTexture(int width, int height, int flags) {
	Raylib::Image raylibImage = {};
	raylibImage.width = width;
	raylibImage.height = height;
	if (flags & _F_TD_RGBA32) {
		raylibImage.format = Raylib::PIXELFORMAT_UNCOMPRESSED_R32G32B32A32;
	} else {
		raylibImage.format = Raylib::PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
	}
	raylibImage.mipmaps = 1;
	raylibImage.data = (u8 *)zalloc(width * height * 4);

	BackendTexture backendTexture = {};
	backendTexture.raylibTexture = Raylib::LoadTextureFromImage(raylibImage);

	free(raylibImage.data);

	return backendTexture;
}

void backendSetTextureSmooth(BackendTexture *backendTexture, bool smooth) {
	Raylib::SetTextureFilter(backendTexture->raylibTexture, smooth ? Raylib::TEXTURE_FILTER_BILINEAR : Raylib::TEXTURE_FILTER_POINT);
}

void backendSetTextureClampedX(BackendTexture *backendTexture, bool clamped) {
  int mode = clamped ? RL_TEXTURE_WRAP_CLAMP : RL_TEXTURE_WRAP_REPEAT;
  Raylib::rlTextureParameters(backendTexture->raylibTexture.id, RL_TEXTURE_WRAP_S, mode);
}

void backendSetTextureClampedY(BackendTexture *backendTexture, bool clamped) {
  int mode = clamped ? RL_TEXTURE_WRAP_CLAMP : RL_TEXTURE_WRAP_REPEAT;
  Raylib::rlTextureParameters(backendTexture->raylibTexture.id, RL_TEXTURE_WRAP_T, mode);
}

void *_backendTempTextureBuffer;
int _backendTempTextureBufferSize;
void *_backendTempTextureRowBuffer;
int _backendTempTextureRowBufferSize;
void backendSetTextureDataByRaylibTexture(Raylib::Texture2D raylibTexture, void *data, int width, int height, int flags);
void backendSetTextureDataByRaylibTexture(Raylib::Texture2D raylibTexture, void *data, int width, int height, int flags) {
	int neededTextureBufferSize = width * height * 4;
	if (neededTextureBufferSize > _backendTempTextureBufferSize) {
		if (_backendTempTextureBuffer) free(_backendTempTextureBuffer);

		_backendTempTextureBufferSize = neededTextureBufferSize;
		_backendTempTextureBuffer = malloc(_backendTempTextureBufferSize);
	}

	void *newData = _backendTempTextureBuffer;
	memcpy(newData, data, width * height * 4);
	data = newData;

	if ((flags & _F_TD_SKIP_PREMULTIPLY) == 0) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				float a = ((u8 *)data)[(y*width+x)*4 + 3] / 255.0;
				((u8 *)data)[(y*width+x)*4 + 2] *= a;
				((u8 *)data)[(y*width+x)*4 + 1] *= a;
				((u8 *)data)[(y*width+x)*4 + 0] *= a;
			}
		}
	}

	if (flags & _F_TD_FLIP_Y) {
		int neededTextureRowBufferSize = width * 4;
		if (neededTextureRowBufferSize > _backendTempTextureRowBufferSize) {
			if (_backendTempTextureRowBuffer) free(_backendTempTextureRowBuffer);

			_backendTempTextureRowBufferSize = neededTextureRowBufferSize;
			_backendTempTextureRowBuffer = malloc(_backendTempTextureRowBufferSize);
		}

		u8 *tempRow = (u8 *)_backendTempTextureRowBuffer;
		for (int y = 0; y < height/2; y++) {
			int curTopRow = y;
			int curBottomRow = height - y - 1;
			u8 *topRowStart = (u8 *)data + curTopRow * width * 4;
			u8 *bottomRowStart = (u8 *)data + curBottomRow * width * 4;

			memcpy(tempRow, topRowStart, width * 4);
			memcpy(topRowStart, bottomRowStart, width * 4);
			memcpy(bottomRowStart, tempRow, width * 4);
		}
	}

	Raylib::UpdateTexture(raylibTexture, data);
}

void backendSetTextureData(BackendTexture *backendTexture, void *data, int width, int height, int flags) {
	backendSetTextureDataByRaylibTexture(backendTexture->raylibTexture, data, width, height, flags);
}

u8 *backendGetTextureDataByRaylibTexture(Raylib::Texture2D raylibTexture);
u8 *backendGetTextureDataByRaylibTexture(Raylib::Texture2D raylibTexture) {
	Raylib::Image raylibImage = Raylib::LoadImageFromTexture(raylibTexture);
	u8 *colors = (u8 *)Raylib::LoadImageColors(raylibImage);

	u8 *data = (u8 *)zalloc(raylibTexture.width * raylibTexture.height * 4);
	memcpy(data, colors, raylibTexture.width * raylibTexture.height * 4);

	RL_FREE(colors);

	Raylib::UnloadImage(raylibImage);
	return data;
}
u8 *backendGetTextureData(BackendTexture *backendTexture) {
	backendFlush();
	return backendGetTextureDataByRaylibTexture(backendTexture->raylibTexture);
}

void backendDestroyTexture(BackendTexture *backendTexture) {
	Raylib::UnloadTexture(backendTexture->raylibTexture);
	free(backendTexture);
}

void backendSetTargetTexture(BackendTexture *backendTexture) {
	backendFlush();

	if (backendTexture) {
		Raylib::rlEnableFramebuffer(_renderTextureFbId);

		Raylib::rlFramebufferAttach(
			_renderTextureFbId,
			backendTexture->raylibTexture.id,
			Raylib::RL_ATTACHMENT_COLOR_CHANNEL0,
			Raylib::RL_ATTACHMENT_TEXTURE2D,
			0
		);

#ifdef FALLOW_DEBUG
		if (!Raylib::rlFramebufferComplete(_renderTextureFbId)) TRACELOG(LOG_INFO, "Failed to complete FBO %d", _renderTextureFbId);
#endif

		Raylib::rlEnableFramebuffer(_renderTextureFbId);

		Raylib::rlViewport(0, 0, backendTexture->raylibTexture.width, backendTexture->raylibTexture.height);

		Raylib::rlMatrixMode(RL_PROJECTION);
		Raylib::rlLoadIdentity();

		Raylib::rlOrtho(0, backendTexture->raylibTexture.width, backendTexture->raylibTexture.height, 0, 0.0f, 1.0f);

		Raylib::rlMatrixMode(RL_MODELVIEW);
		Raylib::rlLoadIdentity();
	} else {
		Raylib::rlDisableFramebuffer();

		Raylib::rlViewport(0, 0, platform->windowWidth, platform->windowHeight);
		Raylib::rlMatrixMode(RL_PROJECTION);
		Raylib::rlLoadIdentity();

		Raylib::rlOrtho(0, platform->windowWidth, platform->windowHeight, 0, 0.0f, 1.0f);

		Raylib::rlMatrixMode(RL_MODELVIEW);
		Raylib::rlLoadIdentity();
	}
}

void backendSetAlpha(float alpha) {
	_backendAlpha = alpha;
}

void backendSetCamera2d(Matrix3 camera2d) {
	_backendCamera2d = camera2d;
}

void backendSetScissor(Rect rect) {
	Raylib::BeginScissorMode(rect.x, rect.y, rect.width, rect.height);
}

void backendEndScissor() {
	Raylib::EndScissorMode();
}

void backendDrawTexture(BackendTexture *backendTexture, Matrix3 matrix, Vec2 uv0, Vec2 uv1, Vec4i tints, float alpha, int flags) {
	alpha *= _backendAlpha;
	if (alpha == 0) return;

	Vec2 verts[] = {
		v2(0, 0),
		v2(0, 1),
		v2(1, 1),
		v2(1, 0),
	};

	matrix = _backendCamera2d * matrix;

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
		// r *= a/255.0; // Very bad!
		// g *= a/255.0;
		// b *= a/255.0;
		colors[i] = argbToHex(a, r, g, b);
	}

	{
		Vec3 verts3[4];
		verts3[0] = v3(verts[0], _current2dDrawDepth); //@incomplete This doesn't get reset upon flush
		verts3[1] = v3(verts[1], _current2dDrawDepth);
		verts3[2] = v3(verts[2], _current2dDrawDepth);
		verts3[3] = v3(verts[3], _current2dDrawDepth);
		drawTexturedQuad(backendTexture->raylibTexture.id, verts3, uvs, colors);
		_current2dDrawDepth += 1.0/RENDERER_BACKEND_MAX_DRAWS_PER_BATCH; // Should really be a fraction of the far-near clip plane
	}
}

void drawTexturedQuad(int textureId, Vec3 *verts, Vec2 *uvs, int *colors) {
	if (_currentDrawCount > RENDERER_BACKEND_MAX_DRAWS_PER_BATCH-10) backendFlush(); // Magic -10 :/
	_currentDrawCount++;

	Raylib::rlCheckRenderBatchLimit(4);

	Raylib::rlSetTexture(textureId);

	Raylib::rlBegin(RL_QUADS);

	for (int i = 0; i < 4; i++) {
		int color = colors[i];
		int a, r, g, b;
		hexToArgb(color, &a, &r, &g, &b);
		r *= a/255.0;
		g *= a/255.0;
		b *= a/255.0;
		color = argbToHex(a, r, g, b);
		Raylib::Color raylibColor = toRaylibColor(color);

		Raylib::rlColor4ub(raylibColor.r, raylibColor.g, raylibColor.b, raylibColor.a);
		Raylib::rlTexCoord2f(uvs[i].x, uvs[i].y);
		Raylib::rlVertex3f(verts[i].x, verts[i].y, verts[i].z);
	}

	Raylib::rlEnd();

	Raylib::rlSetTexture(0);
}
