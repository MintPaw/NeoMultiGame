#include "rendererBackend.h"

struct BackendShader {
	u32 id;
};

struct BackendTexture {
	u32 id;
	int width;
	int height;
};

u32 _vertexBuffer;
u32 _currentFramebuffer;
int _currentActiveTextureSlot;
u32 _renderTextureFbId;
u32 _glVao;
Rect _viewportRect;
int _glErrorCount;

GLuint compileShader(char *text, GLint shaderType);

void checkGlError(int lineNum);
#define CheckGlErrorLow() checkGlError(__LINE__);
#define CheckGlErrorHigh() checkGlError(__LINE__);

void backendInit() {
	glGenVertexArrays(1, &_glVao);
	glGenBuffers(1, &_vertexBuffer);
	glGenFramebuffers(1, &_renderTextureFbId);
	glDepthMask(false); // zii!

	logf("Opengl inited\n");

	int value;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &value);
	logf("Max texture size: %d\n", value);

	backendResetRenderContext();
}

void backendStartFrame() {
}

void backendEndFrame() {
}

void backendClearRenderer(int color) {
	int a, r, g, b;
	hexToArgb(color, &a, &r, &g, &b);
	glClearColor(r/255.0, g/255.0, b/255.0, a/255.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GLuint compileShader(char *text, GLint shaderType) {
	char *preamble;
	if (shaderType == GL_VERTEX_SHADER) {
		preamble =
			"#version 300 es\n";
	} else {
		preamble =
			"#version 300 es\n"
			"precision mediump float;\n";
	}

	char *shaderText[] = {
		preamble,
		text,
	};

	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, ArrayLength(shaderText), shaderText, NULL);
	glCompileShader(shader);
	CheckGlErrorLow();

	int shaderStatusValue;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderStatusValue);
	if (!shaderStatusValue) {
		char *errLog = frameMalloc(4096);
		glGetShaderInfoLog(shader, 4096, NULL, errLog);
		logf("%s shader error:\n%s\n", shaderType == GL_VERTEX_SHADER ? "Vertex" : "Fragment", errLog);
		return 0;
	}

	return shader;
}

void backendLoadShader(BackendShader *backendShader, char *vs, char *fs) {
	GLuint vertexShader = compileShader(vs, GL_VERTEX_SHADER);
	if (vertexShader == 0) return;
	GLuint fragmentShader = compileShader(fs, GL_FRAGMENT_SHADER);
	if (fragmentShader == 0) return;

	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	CheckGlErrorLow();

	int linkStatusValue;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatusValue);
	if (!linkStatusValue) {
		char *errLog = frameMalloc(4096);
		glGetProgramInfoLog(program, 4096, NULL, errLog);
		logf("Shader program error:\n%s\n", errLog);
	}

	backendShader->id = program;
}

int backendGetUniformLocation(BackendShader *backendShader, char *uniformName) {
	return glGetUniformLocation(backendShader->id, uniformName);
}

void backendSetShaderUniform(BackendShader *backendShader, int loc, void *ptr, ShaderUniformType type, int count) {
	if (loc == -1) return;

	if (type == SHADER_UNIFORM_FLOAT) {
		glUniform1fv(loc, count, (float *)ptr);
	} else if (type == SHADER_UNIFORM_VEC2) {
		glUniform2fv(loc, count, (float *)ptr);
	} else if (type == SHADER_UNIFORM_VEC3) {
		glUniform3fv(loc, count, (float *)ptr);
	} else if (type == SHADER_UNIFORM_VEC4) {
		glUniform4fv(loc, count, (float *)ptr);
	} else if (type == SHADER_UNIFORM_MATRIX4) {
		glUniformMatrix4fv(loc, count, true, (float *)ptr);
	} else if (type == SHADER_UNIFORM_INT) {
		glUniform1iv(loc, count, (int *)ptr);
	} else if (type == SHADER_UNIFORM_IVEC2) {
		glUniform2iv(loc, count, (int *)ptr);
	} else if (type == SHADER_UNIFORM_IVEC3) {
		glUniform3iv(loc, count, (int *)ptr);
	} else if (type == SHADER_UNIFORM_IVEC4) {
		glUniform4iv(loc, count, (int *)ptr);
	} else if (type == SHADER_UNIFORM_TEXTURE) {
		BackendTexture *backendTexture = (BackendTexture *)ptr;
		glUniform1iv(loc, count, (int *)&backendTexture->id);
	} else {
		logf("Invalid shader uniform type\n");
	}
}

void backendSetShader(BackendShader *backendShader) {
	glUseProgram(backendShader->id);
}

void backendSetTexture(BackendTexture *backendTexture, int slot) {
	// if (_currentActiveTextureSlot != slot) {
	// 	_currentActiveTextureSlot = slot;
	// 	glActiveTexture(GL_TEXTURE0 + _currentActiveTextureSlot);
	// }
	glActiveTexture(GL_TEXTURE0 + _currentActiveTextureSlot);

	glBindTexture(GL_TEXTURE_2D, backendTexture ? backendTexture->id : 0);
}

void backendFlush() {
}

BackendTexture backendCreateTexture(int width, int height, int flags) {
	if (flags & _F_TD_RGBA32) logf("HTML doesn't support weird texture formats!\n");
	BackendTexture backendTexture = {};
	backendTexture.width = width;
	backendTexture.height = height;
	glGenTextures(1, &backendTexture.id);

	backendSetTexture(&backendTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	CheckGlErrorHigh();
	return backendTexture;
}

void backendSetTextureSmooth(bool smooth) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, smooth ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, smooth ? GL_LINEAR : GL_NEAREST);
}

void backendSetTextureClampedX(bool clamped) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamped ? GL_CLAMP_TO_EDGE : GL_REPEAT);
}

void backendSetTextureClampedY(bool clamped) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamped ? GL_CLAMP_TO_EDGE : GL_REPEAT);
}

void backendSetTextureData(void *data, int width, int height, int flags) {
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

u8 *backendGetTextureData(BackendTexture *backendTexture) {
	u32 fboId = 0;
	glGenFramebuffers(1, &fboId);
	glBindFramebuffer(GL_FRAMEBUFFER, fboId);

	glBindTexture(GL_TEXTURE_2D, 0); // I don't know why, this was in rlReadTexturePixels

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, backendTexture->id, 0);

	u8 *pixels = (u8 *)malloc(backendTexture->width * backendTexture->height * 4);
	glReadPixels(0, 0, backendTexture->width, backendTexture->height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	glBindFramebuffer(GL_FRAMEBUFFER, _currentFramebuffer);

	glDeleteFramebuffers(1, &fboId);

	return pixels;
}

void backendDestroyTexture(BackendTexture *backendTexture) {
	glDeleteTextures(1, &backendTexture->id);
}

void backendSetTargetTexture(BackendTexture *backendTexture) {
	if (backendTexture) {
		_currentFramebuffer = _renderTextureFbId;
		glBindFramebuffer(GL_FRAMEBUFFER, _currentFramebuffer);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, backendTexture->id, 0);

#ifdef FALLOW_DEBUG
		GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) logf("Framebuffer incomplete error %d (0x%x)\n", framebufferStatus, framebufferStatus);
#endif

		_viewportRect = makeRect(0, 0, backendTexture->width, backendTexture->height);
	} else {
		_currentFramebuffer = 0;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		_viewportRect = makeRect(0, 0, platform->windowWidth, platform->windowHeight);
	}

	glViewport(_viewportRect.x, _viewportRect.x, _viewportRect.width, _viewportRect.height);
}

void backendSetScissor(Rect rect) {
	glEnable(GL_SCISSOR_TEST);
	glScissor(rect.x, rect.y, rect.width, rect.height);
}

void backendEndScissor() {
	glDisable(GL_SCISSOR_TEST);
}

void backendSetBlendMode(BlendMode blendMode) {
	glEnable(GL_BLEND);

	if (blendMode == BLEND_NORMAL) {
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);
	} else if (blendMode == BLEND_MULTIPLY) {
		glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);
	} else if (blendMode == BLEND_SCREEN) {
		logf("@todo no screen in webgl\n");
	} else if (blendMode == BLEND_ADD) {
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);
	} else if (blendMode == BLEND_INVERT) {
		logf("@todo no invert in webgl\n");
	} else if (blendMode == BLEND_RECOMMENDED) {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);
	}
}

void backendSetDepthTest(bool enabled) {
	if (enabled) {
		glEnable(GL_DEPTH_TEST);
	} else {
		glDisable(GL_DEPTH_TEST);
	}
}

void backendSetDepthMask(bool enabled) {
	glDepthMask(enabled);
}

void backendSetBlending(bool enabled) {
	if (enabled) {
		glEnable(GL_BLEND);
	} else {
		glDisable(GL_BLEND);
	}
}

void backendSetBackfaceCulling(bool enabled) {
	if (enabled) {
		glEnable(GL_CULL_FACE);
	} else {
		glDisable(GL_CULL_FACE);
	}
}

void backendDrawVerts(GpuVertex *verts, int vertsNum) {
	for (int i = 0; i < vertsNum; i++) {
		GpuVertex *gpuVert = &verts[i];
		Vec4 colorVec = argbToRgba(gpuVert->color);
		colorVec.x *= colorVec.w;
		colorVec.y *= colorVec.w;
		colorVec.z *= colorVec.w;
		gpuVert->color = colorVec;
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(GpuVertex) * vertsNum, verts, GL_DYNAMIC_DRAW);
	glDrawArrays(GL_TRIANGLES, 0, vertsNum);
}

void backendResetRenderContext() {
	glBindVertexArray(_glVao);

	glActiveTexture(GL_TEXTURE0 + _currentActiveTextureSlot);

	glViewport(_viewportRect.x, _viewportRect.x, _viewportRect.width, _viewportRect.height);

	glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(GpuVertex), (void *)(offsetof(GpuVertex, position)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(GpuVertex), (void *)(offsetof(GpuVertex, uv)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(GpuVertex), (void *)(offsetof(GpuVertex, normal)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, false, sizeof(GpuVertex), (void *)(offsetof(GpuVertex, color)));

	// glCullFace(GL_BACK);
	// glFrontFace(GL_CCW);

	glBindSampler(0, 0); // 100% needed...
}


void backendImGuiTexture(BackendTexture *backendTexture, Vec2 scale) {
	ImGui::Image(
		(ImTextureID)(intptr_t)backendTexture->id,
		ImVec2(backendTexture->width * scale.x, backendTexture->height * scale.y),
		ImVec2(0, 1),
		ImVec2(1, 0)
	);
}

bool backendImGuiImageButton(BackendTexture *backendTexture) {
	return ImGui::ImageButton(
		(ImTextureID)(intptr_t)backendTexture->id,
		ImVec2(backendTexture->width, backendTexture->height),
		ImVec2(0, 1),
		ImVec2(1, 0)
	);
}

void checkGlError(int lineNum) {
	if (_glErrorCount >= 10) return;

	for (;;) {
		GLenum err = glGetError();
		if (err == GL_NO_ERROR) break;

		_glErrorCount++;
		logf("Gl error: 0x%x(%d) at line %d\n", err, err, lineNum);
		if (_glErrorCount == 10) logf("Max gl errors exceeded, no more will be shown\n");
	}
}
