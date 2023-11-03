#include "rendererBackend.h"

BackendTexture *_textureSlots[8];
int _activeSlot = 0;

struct BackendShader {
	Raylib::Shader raylibShader;
};

struct BackendTexture {
	Raylib::Texture2D raylibTexture;
};

int _renderTextureFbId;

void backendInit() {
	_renderTextureFbId = Raylib::rlLoadFramebuffer(0, 0); // The size here doesn't mater??
	backendSetBackfaceCulling(false); // Conform to opengl defaults
}

void backendStartFrame() {
	Raylib::BeginDrawing();

	Raylib::rlMatrixMode(RL_PROJECTION);
	Raylib::rlLoadIdentity();
	Raylib::rlMatrixMode(RL_MODELVIEW);
	Raylib::rlLoadIdentity();
}

void backendEndFrame() {
	Raylib::EndDrawing();
}

void backendClearRenderer(int color) {
	int a, r, g, b;
	hexToArgb(color, &a, &r, &g, &b);
	Raylib::rlClearColor(r, g, b, a);
	Raylib::rlClearScreenBuffers();                             
}

void backendLoadShader(BackendShader *backendShader, char *vs, char *fs) {
#ifdef __EMSCRIPTEN__
	char *vsPreamble =
		"#version 100\n"
		"#define in attribute\n"
		"#define out varying\n";

	char *fsPreamble =
		"#version 100\n"
		"#define in varying\n"
		"#define out\n"
		"#define texture texture2D\n"
		"precision mediump float;\n";
#else
	char *vsPreamble = "#version 330\n";
	char *fsPreamble = "#version 330\n";
#endif

	vs = frameSprintf("%s\n%s", vsPreamble, vs);
	fs = frameSprintf("%s\n%s", fsPreamble, fs);

	backendShader->raylibShader = Raylib::LoadShaderFromMemory(vs, fs);

	// backendShader->raylibShader.locs[Raylib::RL_SHADER_LOC_VERTEX_POSITION] = Raylib::rlGetLocationAttrib(backendShader->raylibShader.id, "vertexPosition");
	// backendShader->raylibShader.locs[Raylib::RL_SHADER_LOC_VERTEX_TEXCOORD01] = Raylib::rlGetLocationAttrib(backendShader->raylibShader.id, "vertexTexCoord");
	// backendShader->raylibShader.locs[Raylib::RL_SHADER_LOC_VERTEX_NORMAL] = Raylib::rlGetLocationAttrib(backendShader->raylibShader.id, "vertexNormal");
	// backendShader->raylibShader.locs[Raylib::RL_SHADER_LOC_VERTEX_COLOR] = Raylib::rlGetLocationAttrib(backendShader->raylibShader.id, "vertexColor");
	// backendShader->raylibShader.locs[Raylib::RL_SHADER_LOC_MATRIX_MVP] = glGetUniformLocation(backendShader->raylibShader.id, "mvp");
	// backendShader->raylibShader.locs[Raylib::RL_SHADER_LOC_COLOR_DIFFUSE] = glGetUniformLocation(backendShader->raylibShader.id, "colDiffuse");
	// backendShader->raylibShader.locs[Raylib::RL_SHADER_LOC_MAP_DIFFUSE] = glGetUniformLocation(backendShader->raylibShader.id, "texture0");
}

int backendGetUniformLocation(BackendShader *backendShader, char *uniformName) {
	int loc = glGetUniformLocation(backendShader->raylibShader.id, uniformName);
	return loc;
}

void backendSetShaderUniform(BackendShader *backendShader, int loc, void *ptr, ShaderUniformType type, int count) {
	if (loc == -1) return;

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
	} else if (type == SHADER_UNIFORM_TEXTURE) {
		BackendTexture *backendTexture = (BackendTexture *)ptr;
		Raylib::rlSetUniform(loc, &backendTexture->raylibTexture.id, Raylib::SHADER_UNIFORM_SAMPLER2D, count);
	} else {
		logf("Invalid shader uniform type\n");
	}
}

void backendSetShader(BackendShader *backendShader) {
	Raylib::rlSetShader(backendShader->raylibShader.id, backendShader->raylibShader.locs);
}

void backendSetTexture(BackendTexture *backendTexture, int slot) {
	_textureSlots[slot] = backendTexture;
	_activeSlot = slot;
}

void backendFlush() {
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

	backendSetTexture(&backendTexture);
	backendSetTextureSmooth(true);
	backendSetTextureClampedX(false);
	backendSetTextureClampedY(false);

	return backendTexture;
}

void backendSetTextureSmooth(bool smooth) {
	Raylib::SetTextureFilter(
		_textureSlots[_activeSlot]->raylibTexture,
		smooth ? Raylib::TEXTURE_FILTER_BILINEAR : Raylib::TEXTURE_FILTER_POINT
	);
}

void backendSetTextureClampedX(bool clamped) {
  Raylib::rlTextureParameters(
		_textureSlots[_activeSlot]->raylibTexture.id,
		RL_TEXTURE_WRAP_S,
		clamped ? RL_TEXTURE_WRAP_CLAMP : RL_TEXTURE_WRAP_REPEAT
  );
}

void backendSetTextureClampedY(bool clamped) {
  Raylib::rlTextureParameters(
		_textureSlots[_activeSlot]->raylibTexture.id,
		RL_TEXTURE_WRAP_T,
		clamped ? RL_TEXTURE_WRAP_CLAMP : RL_TEXTURE_WRAP_REPEAT
  );
}

void backendSetTextureData(void *data, int width, int height, int flags) {
	Raylib::UpdateTexture(_textureSlots[_activeSlot]->raylibTexture, data);
}

u8 *backendGetTextureData(BackendTexture *backendTexture) {
	Raylib::Image raylibImage = Raylib::LoadImageFromTexture(backendTexture->raylibTexture);
	u8 *colors = (u8 *)Raylib::LoadImageColors(raylibImage);

	u8 *data = (u8 *)zalloc(backendTexture->raylibTexture.width * backendTexture->raylibTexture.height * 4);
	memcpy(data, colors, backendTexture->raylibTexture.width * backendTexture->raylibTexture.height * 4);

	RL_FREE(colors);
	Raylib::UnloadImage(raylibImage);

	return data;
}

void backendDestroyTexture(BackendTexture *backendTexture) {
	Raylib::UnloadTexture(backendTexture->raylibTexture);
	free(backendTexture);
}

void backendSetTargetTexture(BackendTexture *backendTexture) {
	if (backendTexture) {
		Raylib::rlEnableFramebuffer(_renderTextureFbId);

		Raylib::rlFramebufferAttach(_renderTextureFbId, backendTexture->raylibTexture.id, Raylib::RL_ATTACHMENT_COLOR_CHANNEL0, Raylib::RL_ATTACHMENT_TEXTURE2D, 0);

#ifdef FALLOW_DEBUG
		if (!Raylib::rlFramebufferComplete(_renderTextureFbId)) TRACELOG(LOG_INFO, "Failed to complete FBO %d", _renderTextureFbId);
#endif

		Raylib::rlEnableFramebuffer(_renderTextureFbId);

		Raylib::rlViewport(0, 0, backendTexture->raylibTexture.width, backendTexture->raylibTexture.height);

		Raylib::rlMatrixMode(RL_PROJECTION);
		Raylib::rlLoadIdentity();
		Raylib::rlMatrixMode(RL_MODELVIEW);
		Raylib::rlLoadIdentity();
	} else {
		Raylib::rlDisableFramebuffer();

		Raylib::rlViewport(0, 0, platform->windowWidth, platform->windowHeight);

		Raylib::rlMatrixMode(RL_PROJECTION);
		Raylib::rlLoadIdentity();
		Raylib::rlMatrixMode(RL_MODELVIEW);
		Raylib::rlLoadIdentity();
	}
}

void backendSetScissor(Rect rect) {
	Raylib::rlEnableScissorTest();
	Raylib::rlScissor(rect.x, rect.y, rect.width, rect.height);
}

void backendEndScissor() {
	Raylib::rlDisableScissorTest();
}

void backendSetBlendMode(BlendMode blendMode) {
	Raylib::rlSetBlendMode(Raylib::BLEND_ALPHA);
	Raylib::rlSetBlendMode(Raylib::BLEND_MULTIPLIED);

	if (blendMode == BLEND_NORMAL) {
		// void rlSetBlendFactors(int glSrcFactor, int glDstFactor, int glEquation);
		Raylib::rlSetBlendFactors(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_FUNC_ADD);
		Raylib::rlSetBlendMode(Raylib::BLEND_CUSTOM);
	} else if (blendMode == BLEND_MULTIPLY) {
		Raylib::rlSetBlendMode(Raylib::BLEND_MULTIPLIED);
	} else if (blendMode == BLEND_SCREEN) {
		logf("@todo no blendmodes in raylib\n");
	} else if (blendMode == BLEND_ADD) {
		Raylib::rlSetBlendMode(Raylib::BLEND_ADDITIVE);
	} else if (blendMode == BLEND_INVERT) {
		logf("@todo no blendmodes in raylib\n");
	} else if (blendMode == BLEND_RECOMMENDED) {
		Raylib::rlSetBlendMode(Raylib::BLEND_ALPHA);
	}
}

void backendSetDepthTest(bool enabled) {
	if (enabled) Raylib::rlEnableDepthTest(); else Raylib::rlDisableDepthTest();
}

void backendSetDepthMask(bool enabled) {
	if (enabled) Raylib::rlEnableDepthMask(); else Raylib::rlDisableDepthMask();
}

void backendSetBlending(bool enabled) {
	if (enabled) Raylib::rlEnableColorBlend(); else Raylib::rlDisableColorBlend();
}

void backendSetBackfaceCulling(bool enabled) {
	if (enabled) Raylib::rlEnableBackfaceCulling(); else Raylib::rlDisableBackfaceCulling();
}

void backendDrawVerts(GpuVertex *verts, int vertsNum) {
	Raylib::rlCheckRenderBatchLimit(vertsNum);
	if (_textureSlots[_activeSlot]) Raylib::rlSetTexture(_textureSlots[_activeSlot]->raylibTexture.id);
	Raylib::rlBegin(RL_QUADS);

	for (int i = 0; i < vertsNum; i++) {
		GpuVertex *gpuVert = &verts[i];
		Vec4 color = gpuVert->color;
		float a = color.x;
		float r = color.y;
		float g = color.z;
		float b = color.w;

		Raylib::rlColor4f(r*a, g*a, b*a, a);
		Raylib::rlTexCoord2f(gpuVert->uv.x, gpuVert->uv.y);
		Raylib::rlVertex3f(gpuVert->position.x, gpuVert->position.y, gpuVert->position.z);
		if ((i-1)%3 == 0) Raylib::rlVertex3f(gpuVert->position.x, gpuVert->position.y, gpuVert->position.z);
	}

	Raylib::rlEnd();
	Raylib::rlSetTexture(0);
}

void backendResetRenderContext() {
}

void backendImGuiTexture(BackendTexture *backendTexture, Vec2 scale) {
	ImGui::Image(
		(ImTextureID)(intptr_t)backendTexture->raylibTexture.id,
		ImVec2(backendTexture->raylibTexture.width * scale.x, backendTexture->raylibTexture.height * scale.y),
		ImVec2(0, 1),
		ImVec2(1, 0)
	);
}

bool backendImGuiImageButton(BackendTexture *backendTexture) {
	return ImGui::ImageButton(
		(ImTextureID)(intptr_t)backendTexture->raylibTexture.id,
		ImVec2(backendTexture->raylibTexture.width, backendTexture->raylibTexture.height),
		ImVec2(0, 1),
		ImVec2(1, 0)
	);
}
