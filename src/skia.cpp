#define DO_SKIA_MSAA 0

#define DO_LAYER_BLUR 1
#define DO_DRAW_AFTER_CONTEXT_SWITCH 0
#define USE_MSAA_RENDERBUFFER 1

#define USING_CAIROVG 0

#if USING_CAIROVG
#include "cairo/cairo.h"
#endif

struct VDrawPaletteSwap {
	int from;
	int to;
};

struct DrawSpriteRecurseData {
	bool inited;
#define SPRITE_PATH_MAX_LEN 256
	char *path;
	bool useClip;
	char *altName;
	float alpha;
	int tint;
	char *currentLayerName;
	ColorTransform colorTransform;
	SwfBlendMode blendMode;
	Font *font;
	char *text;
	VDrawPaletteSwap *swaps;
	int swapsNum;
};

struct DrawSpriteCall;
struct SpriteTransform {
#define SPRITE_TRANSFORM_PATHS_MAX 32
	char *paths[SPRITE_TRANSFORM_PATHS_MAX];
	int pathsNum;
#define LAYERS_TO_HIDE_MAX 8
	char *layersToHide[LAYERS_TO_HIDE_MAX];
	int layersToHideNum;
	bool isInstanceName;
	int frame;
	Matrix3 matrix;
	float alpha;
	int tint;
	bool touched;

	char *text;
	Font *font;

	Matrix3 outMatrix;

	DrawSpriteCall *drawSpriteCalls;
	int drawSpriteCallsNum;

	VDrawPaletteSwap *swaps;
	int swapsNum;
};

struct DrawSpriteCall {
	SwfSprite *sprite;
#define DRAW_SPRITE_CALL_TRANSFORMS_MAX 8
	SpriteTransform transforms[DRAW_SPRITE_CALL_TRANSFORMS_MAX];
	int transformsNum;
};

struct DrawShapeProps {
	bool useClip;
	float alpha;
	int tint;
	ColorTransform colorTransform;
};

enum VDrawCommandType {
	VDRAW_SET_MATRIX,
	VDRAW_MOVE_TO,
	VDRAW_LINE_TO,
	VDRAW_QUAD_TO,
	VDRAW_DRAW_PATH,
	VDRAW_CLIP_PATH,
	VDRAW_RESET_PATH,
	VDRAW_DRAW_CACHED_PATH,
	VDRAW_CLIP_CACHED_PATH,
	VDRAW_SAVE,
	VDRAW_RESTORE,
	VDRAW_START_SHAPE,
	VDRAW_END_SHAPE,
	VDRAW_END_SPRITE,
	VDRAW_START_BLUR,
	VDRAW_END_BLUR,
	VDRAW_SET_BLEND_MODE,
	VDRAW_SET_SOLID_FILL,
	VDRAW_SET_LINEAR_GRADIENT_FILL,
	VDRAW_SET_RADIAL_GRADIENT_FILL,
	VDRAW_SET_FOCAL_GRADIENT_FILL,
	VDRAW_SET_LINE_STYLE,
	VDRAW_BITMAP_FILL,
	VDRAW_TEXT,
};
struct VDrawCommand {
	VDrawCommandType type;

	Matrix3 matrix;
	BlendMode blendMode;
	Vec2 control;
	Vec2 position;
	float width;

	int colors[16];
	float gradientRatios[16];
	int gradientRatiosNum;

	SwfBitmap *swfBitmap;
	SkTileMode tileMode;

	CapStyle startCapStyle;
	JoinStyle joinStyle;
	float miterLimitFactor;

	SkPath *path;

	Font *font;
	char *text;
	Vec2 gravity;
};

struct VDrawCommandsList {
	VDrawCommand *cmds;
	int cmdsNum;
	int cmdsMax;
};

struct SkiaSys {
#if USING_CAIROVG
	cairo_surface_t *cairoSurface;
	cairo_t *cairoContext;
#endif

	SkBitmap bitmap;
	SkCanvas *mainCanvas;
	SkCanvas *canvas;

	const GrGLInterface *grInterface;
	GrDirectContext *grDirectContext;
	SkSurface *gpuSurface;
#if DO_SKIA_MSAA
	u32 multiSampleFramebufferId;
	u32 flatFramebufferId;

	u32 msaaColorRenderbufferId;
	u32 msaaDepthRenderbufferId;
#else
	Texture *skiaTexture;
#endif
	bool useGpu;
	int msaaSamples;

	RenderTexture *backTexture;

#define MATRIX_STACK_MAX 256
	Matrix3 matrixStack[MATRIX_STACK_MAX];
	int matrixStackNum;

	int width;
	int height;
	u8 *cpuFramePixels;
	Vec2 scale;
	float superSampleScale;
	bool useCpuAA;
	bool blurEnabled;

	Vec2 currentBlur;

	VDrawCommandsList immVDrawCommandsList;
};
SkiaSys *skiaSys = NULL;

void resetSkia(Vec2 size, Vec2 scale=v2(1, 1), bool useGpu=true, int msaaSamples=16);
void drawSprite(SwfSprite *sprite, SpriteTransform *transforms=NULL, int transformsNum=0, DrawSpriteRecurseData recurse={});
void drawShape(SwfShape *shape, Matrix3 matrix, DrawShapeProps props);

VDrawCommand *createCommand(VDrawCommandsList *cmdList, VDrawCommandType type);
void pushSpriteMatrix(VDrawCommandsList *cmdList, Matrix3 mat);
void popSpriteMatrix(VDrawCommandsList *cmdList);
DrawShapeProps newDrawShapeProps();
void genDrawShape(SwfShape *shape, DrawShapeProps props, VDrawCommandsList *cmdList);
void genDrawSprite(SwfSprite *sprite, SpriteTransform *transforms, int transformsNum, DrawSpriteRecurseData recurse, VDrawCommandsList *cmdList, bool isNested=false);
void execCommands(VDrawCommandsList *cmdList);

void clearSkia(int color=0);
void startSkiaFrame();
void endSkiaFrame();

void initSpriteTransforms(SpriteTransform *transforms, int transformsNum);

SpriteTransform *getSpriteTransform(SpriteTransform *transforms, int *transformsNum, char *pathName);

#define CreateSpriteTransforms(varName, varNumName, count) \
	SpriteTransform varName[count]; \
	initSpriteTransforms(varName, count); \
	int varNumName = 0;

void drawSwfAnalyzer();

/// FUNCTIONS ^

// static void* get_proc(fastuidraw::c_string proc_name) { return SDL_GL_GetProcAddress(proc_name); }

void resetSkia(Vec2 size, Vec2 scale, bool useGpu, int msaaSamples) {
	if (!skiaSys) {
		platform->usingSkia = true;

		skiaSys = (SkiaSys *)zalloc(sizeof(SkiaSys));
		skiaSys->matrixStack[skiaSys->matrixStackNum++] = mat3();
		skiaSys->superSampleScale = 2;

#if !defined(__EMSCRIPTEN__)
		skiaSys->blurEnabled = true;
#endif

#if USING_CAIROVG
#else
		SkGraphics::Init();
#endif
	}

	if (size.x == -1) {
		size.x = skiaSys->width / skiaSys->superSampleScale;
		size.y = skiaSys->height / skiaSys->superSampleScale;
		scale = skiaSys->scale;
		useGpu = skiaSys->useGpu;
		msaaSamples = skiaSys->msaaSamples;
	}
	// logf("Skia reset to %.1f %.1f\n", size.x, size.y);

	// skiaSys->backTextureWidth = size.x;
	// skiaSys->backTextureHeight = size.y;
	skiaSys->width = size.x * skiaSys->superSampleScale;
	skiaSys->height = size.y * skiaSys->superSampleScale;
	skiaSys->scale = scale;
	skiaSys->msaaSamples = msaaSamples;

	if (skiaSys->backTexture) destroyTexture(skiaSys->backTexture);
	skiaSys->backTexture = createRenderTexture(size.x, size.y);

#if USING_CAIROVG
	if (skiaSys->cairoSurface) cairo_surface_destroy(skiaSys->cairoSurface);
	skiaSys->cairoSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, size.x, size.y);

	if (skiaSys->cairoContext) cairo_destroy(skiaSys->cairoContext);
	skiaSys->cairoContext = cairo_create(skiaSys->cairoSurface);

	cairo_set_antialias(skiaSys->cairoContext, CAIRO_ANTIALIAS_NONE);
	cairo_set_fill_rule(skiaSys->cairoContext, CAIRO_FILL_RULE_EVEN_ODD);
#else
	bool oldUseGpu = skiaSys->useGpu;
	skiaSys->useGpu = useGpu;
	if (!oldUseGpu && skiaSys->mainCanvas) {
		delete skiaSys->mainCanvas;
		skiaSys->mainCanvas = NULL;
	}

	if (skiaSys->gpuSurface) {
		delete skiaSys->gpuSurface;
		skiaSys->gpuSurface = NULL;
	}

	if (skiaSys->useGpu) {
		if (!skiaSys->grInterface) {
			sk_sp<const GrGLInterface> grInterface = GrGLMakeNativeInterface();
			if (!grInterface || !grInterface.get()) logf("NULL native interface\n");
			skiaSys->grInterface = grInterface.release();
		}

		if (!skiaSys->grDirectContext) {
			sk_sp<GrDirectContext> grDirectContext = GrDirectContext::MakeGL();
			if (!grDirectContext || !grDirectContext.get()) logf("Failed to create grDirectContext\n");
			skiaSys->grDirectContext = grDirectContext.release();
		}

#if DO_SKIA_MSAA
		if (!skiaSys->flatFramebufferId) glGenFramebuffers(1, &skiaSys->flatFramebufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, skiaSys->flatFramebufferId);

#if RAYLIB_MODE
		u32 backTextureId = skiaSys->backTexture->raylibRenderTexture.texture.id;
#else
		u32 backTextureId = skiaSys->backTexture->texture->id;
#endif
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, backTextureId, 0);

		if (!skiaSys->multiSampleFramebufferId) glGenFramebuffers(1, &skiaSys->multiSampleFramebufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, skiaSys->multiSampleFramebufferId);

#if USE_MSAA_RENDERBUFFER
		if (skiaSys->msaaColorRenderbufferId) glDeleteRenderbuffers(1, &skiaSys->msaaColorRenderbufferId);
		glGenRenderbuffers(1, &skiaSys->msaaColorRenderbufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, skiaSys->msaaColorRenderbufferId);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaaSamples, GL_RGBA8, skiaSys->width, skiaSys->height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, skiaSys->msaaColorRenderbufferId);
#else
		if (skiaSys->msaaColorRenderbufferId) glDeleteTextures(1, &skiaSys->msaaColorRenderbufferId);
		glGenTextures(1, &skiaSys->msaaColorRenderbufferId);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, skiaSys->msaaColorRenderbufferId);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, msaaSamples, GL_RGBA8, skiaSys->width, skiaSys->height, true);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, skiaSys->msaaColorRenderbufferId, 0);
#endif

		if (skiaSys->msaaDepthRenderbufferId) glDeleteRenderbuffers(1, &skiaSys->msaaDepthRenderbufferId);
		glGenRenderbuffers(1, &skiaSys->msaaDepthRenderbufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, skiaSys->msaaDepthRenderbufferId);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaaSamples, GL_DEPTH24_STENCIL8, skiaSys->width, skiaSys->height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, skiaSys->msaaDepthRenderbufferId);
		u32 status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) logf("Bad fb\n");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		GrGLFramebufferInfo framebufferInfo;
		framebufferInfo.fFBOID = skiaSys->multiSampleFramebufferId;
		framebufferInfo.fFormat = GL_RGBA8;

		GrBackendRenderTarget backendRenderTarget(
			skiaSys->width,
			skiaSys->height,
			msaaSamples, // sample count
			8, // stencil bits
			framebufferInfo
		);

		SkSurfaceProps surfaceProps(0, SkPixelGeometry::kUnknown_SkPixelGeometry);

		skiaSys->gpuSurface = SkSurface::MakeFromBackendRenderTarget(
			skiaSys->grDirectContext,
			backendRenderTarget,
			kBottomLeft_GrSurfaceOrigin,
			kRGBA_8888_SkColorType,
			NULL,
			&surfaceProps
		).release();
#else

	if (skiaSys->skiaTexture) destroyTexture(skiaSys->skiaTexture);
	skiaSys->skiaTexture = createTexture(skiaSys->width, skiaSys->height);
	setTextureSmooth(skiaSys->skiaTexture, true);

	sk_sp<SkColorSpace> colorSpace = SkColorSpace::MakeSRGB();
	SkSurfaceProps surfaceProps(0, SkPixelGeometry::kUnknown_SkPixelGeometry);

	GrGLTextureInfo textureInfo = {};
#if RAYLIB_MODE
	int skiaTextureId = skiaSys->skiaTexture->raylibTexture.id;
#else
	int skiaTextureId = skiaSys->skiaTexture->id;
#endif
	textureInfo.fTarget = GL_TEXTURE_2D;
	textureInfo.fID = skiaTextureId;
	textureInfo.fFormat = GL_RGBA8;

	GrBackendTexture backendTexture(skiaSys->skiaTexture->width, skiaSys->skiaTexture->height, GrMipmapped::kNo, textureInfo);

	skiaSys->gpuSurface = SkSurface::MakeFromBackendTexture(
		skiaSys->grDirectContext,
		backendTexture,
		kBottomLeft_GrSurfaceOrigin,
		0,
		kRGBA_8888_SkColorType,
		colorSpace,
		&surfaceProps
	).release();

#endif

		if (!skiaSys->gpuSurface) {
			if (msaaSamples == 16) {
				logf("Failed to create skia 16x MSAA surface, falling back\n");
				resetSkia(size, scale, true, 8);
			} else if (msaaSamples == 8) {
				logf("Failed to create skia 8x MSAA surface, falling back\n");
				resetSkia(size, scale, true, 4);
			} else if (msaaSamples == 4) {
				logf("Failed to create skia 4x MSAA surface, falling back\n");
				resetSkia(size, scale, true, 2);
			} else if (msaaSamples == 2) {
				logf("Failed to create skia 2x MSAA surface, falling back\n");
				resetSkia(size, scale, true, 0);
			} else {
				logf("Failed to create skia GPU surface, falling back\n");
				resetSkia(size, scale, false, 0);
			}
			return;
		}

		skiaSys->mainCanvas = skiaSys->gpuSurface->getCanvas();

		if (!skiaSys->mainCanvas) logf("Failed to create skia canvas\n");
		// bindVertexArray(renderer->defaultVao);

		// glPopClientAttrib();
	} else {
		SkImageInfo info = SkImageInfo::Make(skiaSys->width, skiaSys->height, kRGBA_8888_SkColorType, kPremul_SkAlphaType, NULL);
		skiaSys->bitmap.allocPixels(info);

		skiaSys->mainCanvas = new SkCanvas(skiaSys->bitmap);
	}
	skiaSys->canvas = skiaSys->mainCanvas;
#endif

	if (skiaSys->cpuFramePixels) free(skiaSys->cpuFramePixels);
	skiaSys->cpuFramePixels = (u8 *)malloc(skiaSys->width * skiaSys->height * 4);
}

void drawSprite(SwfSprite *sprite, SpriteTransform *transforms, int transformsNum, DrawSpriteRecurseData recurse) {
	genDrawSprite(sprite, transforms, transformsNum, recurse, &skiaSys->immVDrawCommandsList);
#if !DO_DRAW_AFTER_CONTEXT_SWITCH
	execCommands(&skiaSys->immVDrawCommandsList);
	skiaSys->immVDrawCommandsList.cmdsNum = 0;
#endif
}

void drawShape(SwfShape *shape, Matrix3 matrix, DrawShapeProps props) {
	pushSpriteMatrix(&skiaSys->immVDrawCommandsList, matrix);
	genDrawShape(shape, props, &skiaSys->immVDrawCommandsList);
	popSpriteMatrix(&skiaSys->immVDrawCommandsList);
#if !DO_DRAW_AFTER_CONTEXT_SWITCH
	execCommands(&skiaSys->immVDrawCommandsList);
	skiaSys->immVDrawCommandsList.cmdsNum = 0;
#endif
}

VDrawCommand *createCommand(VDrawCommandsList *cmdList, VDrawCommandType type) {
	if (cmdList->cmdsMax == 0) {
		cmdList->cmdsMax = 32;
		cmdList->cmds = (VDrawCommand *)zalloc(sizeof(VDrawCommand) * cmdList->cmdsMax);
	}

	if (cmdList->cmdsNum > cmdList->cmdsMax-1) {
		cmdList->cmds = (VDrawCommand *)resizeArray(cmdList->cmds, sizeof(VDrawCommand), cmdList->cmdsNum, cmdList->cmdsMax * 1.5);
		cmdList->cmdsMax *= 1.5;
	}

	VDrawCommand *cmd = &cmdList->cmds[cmdList->cmdsNum++];
	memset(cmd, 0, sizeof(VDrawCommand));
	cmd->type = type;
	return cmd;
}

void pushSpriteMatrix(VDrawCommandsList *cmdList, Matrix3 mat) {
	if (skiaSys->matrixStackNum > MATRIX_STACK_MAX-1) {
		logf("Too many matrix stacks!\n");
		return;
	}

	Matrix3 topMatrix = skiaSys->matrixStack[skiaSys->matrixStackNum-1];
	topMatrix = topMatrix * mat;
	skiaSys->matrixStack[skiaSys->matrixStackNum++] = topMatrix;

	VDrawCommand *cmd = createCommand(cmdList, VDRAW_SET_MATRIX);
	cmd->matrix = topMatrix;
}
void popSpriteMatrix(VDrawCommandsList *cmdList) {
	skiaSys->matrixStackNum--;
	Matrix3 prevMatrix = skiaSys->matrixStack[skiaSys->matrixStackNum-1];

	VDrawCommand *cmd = createCommand(cmdList, VDRAW_SET_MATRIX);
	cmd->matrix = prevMatrix;
}

DrawShapeProps newDrawShapeProps() {
	DrawShapeProps props = {};
	props.alpha = 1;
	props.colorTransform = makeColorTransform();
	return props;
}

void genDrawShape(SwfShape *shape, DrawShapeProps props, VDrawCommandsList *cmdList) {
#if 1 /// Draw sub shapes with cached paths
	for (int i = 0; i < shape->subShapesNum; i++) {
		SwfSubShape *subShape = &shape->subShapes[i];
		if (subShape->drawEdgesNum == 0) continue;
		createCommand(cmdList, VDRAW_START_SHAPE);

		{ /// Set line/fill style
			if (subShape->fillStyleIndex && subShape->lineStyleIndex) logf("nononono\n");

			VDrawCommand *paintCmd = NULL;
			if (subShape->fillStyleIndex != 0) {
				FillStyle *newFillStyle = &shape->fillStyles[subShape->fillStyleIndex - 1];
				// if (newFillStyle->gradient.spreadMode != 0) logf("Bad spread mode\n");
				// if (newFillStyle->gradient.interpolationMode != 0) logf("Bad introplation mode\n");

				if (newFillStyle->fillStyleType == FILL_STYLE_SOLID) {
					int color = newFillStyle->color;
					color = applyColorTransform(color, props.colorTransform);

					paintCmd = createCommand(cmdList, VDRAW_SET_SOLID_FILL);
					paintCmd->colors[0] = color;
				} else if (
					newFillStyle->fillStyleType == FILL_STYLE_LINEAR_GRADIENT ||
					newFillStyle->fillStyleType == FILL_STYLE_RADIAL_GRADIENT ||
					newFillStyle->fillStyleType == FILL_STYLE_FOCAL_RADIAL_GRADIENT
				) {
					if (newFillStyle->fillStyleType == FILL_STYLE_LINEAR_GRADIENT) {
						paintCmd = createCommand(cmdList, VDRAW_SET_LINEAR_GRADIENT_FILL);
					} else if (newFillStyle->fillStyleType == FILL_STYLE_RADIAL_GRADIENT) {
						paintCmd = createCommand(cmdList, VDRAW_SET_RADIAL_GRADIENT_FILL);
					} else if (newFillStyle->fillStyleType == FILL_STYLE_FOCAL_RADIAL_GRADIENT) {
						paintCmd = createCommand(cmdList, VDRAW_SET_FOCAL_GRADIENT_FILL);
						paintCmd->position.x = newFillStyle->gradient.focalPoint;
					}
					paintCmd->matrix = newFillStyle->gradientMatrix;
					for (int i = 0; i < newFillStyle->gradient.coordsNum; i++) {
						GradeCord *coord = &newFillStyle->gradient.coords[i];
						paintCmd->colors[i] = applyColorTransform(coord->color, props.colorTransform);
						paintCmd->gradientRatios[i] = (float)coord->ratio / 255.0;
						paintCmd->gradientRatiosNum++;
					}
				} else if (newFillStyle->fillStyleType == FILL_STYLE_CLIPPED_BITMAP || newFillStyle->fillStyleType == FILL_STYLE_REPEATING_BITMAP) {
					paintCmd = createCommand(cmdList, VDRAW_BITMAP_FILL);

					if (newFillStyle->fillStyleType == FILL_STYLE_CLIPPED_BITMAP) {
						paintCmd->tileMode = SkTileMode::kClamp;
					} else {
						paintCmd->tileMode = SkTileMode::kRepeat;
					}

					paintCmd->swfBitmap = newFillStyle->bitmap;
					paintCmd->matrix = newFillStyle->bitmapMatrix;
					paintCmd->matrix.SCALE(1/20.0, 1/20.0);
				} else {
					Panic("Bad fill");
				}
			}

			if (subShape->lineStyleIndex != 0) {
				LineStyle *newLineStyle = &shape->lineStyles[subShape->lineStyleIndex - 1];
				int color = newLineStyle->color;
				color = applyColorTransform(color, props.colorTransform);

				paintCmd = createCommand(cmdList, VDRAW_SET_LINE_STYLE);
				paintCmd->colors[0] = color;
				paintCmd->width = newLineStyle->width;

				paintCmd->startCapStyle = newLineStyle->startCapStyle;
				paintCmd->joinStyle = newLineStyle->joinStyle;
				paintCmd->miterLimitFactor = newLineStyle->miterLimitFactor;
			}

			if (paintCmd) {
				for (int i = 0; i < 16; i++) {
					int a, r, g, b;
					hexToArgb(paintCmd->colors[i], &a, &r, &g, &b);
					a = ((a/255.0) * props.alpha) * 255.0;
					paintCmd->colors[i] = argbToHex(a, r, g, b);
				}
			}
		}

		VDrawCommand *cmd = createCommand(cmdList, props.useClip ? VDRAW_CLIP_CACHED_PATH : VDRAW_DRAW_CACHED_PATH);
		cmd->path = (SkPath *)subShape->runtimeCachedPath;

		createCommand(cmdList, VDRAW_END_SHAPE);
	}
#endif

#if 0 /// Draw sub shapes imm
	{
		VDrawCommand *cmd = createCommand(cmdList, VDRAW_MOVE_TO);
		cmd->position = v2();
	}

	Vec2 cursor = v2();

	for (int i = 0; i < shape->subShapesNum; i++) {
		SwfSubShape *subShape = &shape->subShapes[i];

		{ /// Set line/fill style
			if (subShape->fillStyleIndex && subShape->lineStyleIndex) logf("nononono\n");

			VDrawCommand *paintCmd = NULL;
			if (subShape->fillStyleIndex != 0) {
				FillStyle *newFillStyle = &shape->fillStyles[subShape->fillStyleIndex - 1];
				// if (newFillStyle->gradient.spreadMode != 0) logf("Bad spread mode\n");
				// if (newFillStyle->gradient.interpolationMode != 0) logf("Bad introplation mode\n");

				if (newFillStyle->fillStyleType == FILL_STYLE_SOLID) {
					int color = newFillStyle->color;
					color = applyColorTransform(color, props.colorTransform);

					paintCmd = createCommand(cmdList, VDRAW_SET_SOLID_FILL);
					paintCmd->colors[0] = color;
				} else if (
					newFillStyle->fillStyleType == FILL_STYLE_LINEAR_GRADIENT ||
					newFillStyle->fillStyleType == FILL_STYLE_RADIAL_GRADIENT ||
					newFillStyle->fillStyleType == FILL_STYLE_FOCAL_RADIAL_GRADIENT
				) {
					if (newFillStyle->fillStyleType == FILL_STYLE_LINEAR_GRADIENT) {
						paintCmd = createCommand(cmdList, VDRAW_SET_LINEAR_GRADIENT_FILL);
					} else if (newFillStyle->fillStyleType == FILL_STYLE_RADIAL_GRADIENT) {
						paintCmd = createCommand(cmdList, VDRAW_SET_RADIAL_GRADIENT_FILL);
					} else if (newFillStyle->fillStyleType == FILL_STYLE_FOCAL_RADIAL_GRADIENT) {
						paintCmd = createCommand(cmdList, VDRAW_SET_FOCAL_GRADIENT_FILL);
						paintCmd->position.x = newFillStyle->gradient.focalPoint;
					}
					paintCmd->matrix = newFillStyle->gradientMatrix;
					for (int i = 0; i < newFillStyle->gradient.coordsNum; i++) {
						GradeCord *coord = &newFillStyle->gradient.coords[i];
						paintCmd->colors[i] = applyColorTransform(coord->color, props.colorTransform);
						paintCmd->gradientRatios[i] = (float)coord->ratio / 255.0;
						paintCmd->gradientRatiosNum++;
					}
				} else if (newFillStyle->fillStyleType == FILL_STYLE_CLIPPED_BITMAP || newFillStyle->fillStyleType == FILL_STYLE_REPEATING_BITMAP) {
					paintCmd = createCommand(cmdList, VDRAW_BITMAP_FILL);

					if (newFillStyle->fillStyleType == FILL_STYLE_CLIPPED_BITMAP) {
						paintCmd->tileMode = SkTileMode::kClamp;
					} else {
						paintCmd->tileMode = SkTileMode::kRepeat;
					}

					paintCmd->swfBitmap = newFillStyle->bitmap;
					paintCmd->matrix = newFillStyle->bitmapMatrix;
					paintCmd->matrix.SCALE(1/20.0, 1/20.0);
				} else {
					Panic("Bad fill");
				}
			}

			if (subShape->lineStyleIndex != 0) {
				LineStyle *newLineStyle = &shape->lineStyles[subShape->lineStyleIndex - 1];
				int color = newLineStyle->color;
				color = applyColorTransform(color, props.colorTransform);

				paintCmd = createCommand(cmdList, VDRAW_SET_LINE_STYLE);
				paintCmd->colors[0] = color;
				paintCmd->width = newLineStyle->width;

				paintCmd->startCapStyle = newLineStyle->startCapStyle;
				paintCmd->joinStyle = newLineStyle->joinStyle;
				paintCmd->miterLimitFactor = newLineStyle->miterLimitFactor;
			}

			if (paintCmd) {
				for (int i = 0; i < 16; i++) {
					int a, r, g, b;
					hexToArgb(paintCmd->colors[i], &a, &r, &g, &b);
					a = ((a/255.0) * props.alpha) * 255.0;
					paintCmd->colors[i] = argbToHex(a, r, g, b);
				}
			}
		}

		int pathOperations = 0;
		for (int i = 0; i < subShape->drawEdgesNum; i++) {
			DrawEdgeRecord *edge = &subShape->drawEdges[i];

			if (!cursor.equal(edge->start)) {
				cursor = edge->start;
				VDrawCommand *cmd = createCommand(cmdList, VDRAW_MOVE_TO);
				cmd->position = edge->start;
				pathOperations++;
			}
			if (edge->type == DRAW_EDGE_STRAIGHT_EDGE) {
				cursor = edge->control;
				VDrawCommand *cmd = createCommand(cmdList, VDRAW_LINE_TO);
				cmd->position = edge->control;
				pathOperations++;
			} else if (edge->type == DRAW_EDGE_CURVED_EDGE) {
				cursor = edge->anchor;
				VDrawCommand *cmd = createCommand(cmdList, VDRAW_QUAD_TO);
				cmd->control = edge->control;
				cmd->position = edge->anchor;
				pathOperations++;
			}
		}

		if (pathOperations > 0) {
			if (props.useClip) {
				createCommand(cmdList, VDRAW_CLIP_PATH);
			} else {
				createCommand(cmdList, VDRAW_DRAW_PATH);
			}
		}

		createCommand(cmdList, VDRAW_RESET_PATH);
		createCommand(cmdList, VDRAW_END_SHAPE);
		cursor = v2();
	}
#endif

#if 0 /// Draw shape imm
	{
		VDrawCommand *cmd = createCommand(cmdList, VDRAW_MOVE_TO);
		cmd->position = v2();
	}

	Vec2 cursor = v2();
	int pathOperations = 0;

	int lineStyleIndex = 0;
	int fillStyleIndex = 0;

	for (int i = 0; i < shape->drawEdgesNum; i++) {
		DrawEdgeRecord *edge = &shape->drawEdges[i];

		if (edge->fillStyleIndex != fillStyleIndex || edge->lineStyleIndex != lineStyleIndex) {
			if (pathOperations > 0) {
				if (props.useClip) {
					createCommand(cmdList, VDRAW_CLIP_PATH);
				} else {
					createCommand(cmdList, VDRAW_DRAW_PATH);
				}
				pathOperations = 0;
				createCommand(cmdList, VDRAW_RESET_PATH);
				createCommand(cmdList, VDRAW_END_SHAPE);
				cursor = v2();
			}

			lineStyleIndex = edge->lineStyleIndex;
			fillStyleIndex = edge->fillStyleIndex;

			if (fillStyleIndex && lineStyleIndex) logf("nononono\n");

			VDrawCommand *paintCmd = NULL;
			if (fillStyleIndex != 0) {
				FillStyle *newFillStyle = &shape->fillStyles[fillStyleIndex - 1];
				// if (newFillStyle->gradient.spreadMode != 0) logf("Bad spread mode\n");
				// if (newFillStyle->gradient.interpolationMode != 0) logf("Bad introplation mode\n");

				if (newFillStyle->fillStyleType == FILL_STYLE_SOLID) {
					int color = newFillStyle->color;
					color = applyColorTransform(color, props.colorTransform);

					paintCmd = createCommand(cmdList, VDRAW_SET_SOLID_FILL);
					paintCmd->colors[0] = color;
				} else if (
					newFillStyle->fillStyleType == FILL_STYLE_LINEAR_GRADIENT ||
					newFillStyle->fillStyleType == FILL_STYLE_RADIAL_GRADIENT ||
					newFillStyle->fillStyleType == FILL_STYLE_FOCAL_RADIAL_GRADIENT
				) {
					if (newFillStyle->fillStyleType == FILL_STYLE_LINEAR_GRADIENT) {
						paintCmd = createCommand(cmdList, VDRAW_SET_LINEAR_GRADIENT_FILL);
					} else if (newFillStyle->fillStyleType == FILL_STYLE_RADIAL_GRADIENT) {
						paintCmd = createCommand(cmdList, VDRAW_SET_RADIAL_GRADIENT_FILL);
					} else if (newFillStyle->fillStyleType == FILL_STYLE_FOCAL_RADIAL_GRADIENT) {
						paintCmd = createCommand(cmdList, VDRAW_SET_FOCAL_GRADIENT_FILL);
						paintCmd->position.x = newFillStyle->gradient.focalPoint;
					}
					paintCmd->matrix = newFillStyle->gradientMatrix;
					for (int i = 0; i < newFillStyle->gradient.coordsNum; i++) {
						GradeCord *coord = &newFillStyle->gradient.coords[i];
						paintCmd->colors[i] = applyColorTransform(coord->color, props.colorTransform);
						paintCmd->gradientRatios[i] = (float)coord->ratio / 255.0;
						paintCmd->gradientRatiosNum++;
					}
				} else if (newFillStyle->fillStyleType == FILL_STYLE_CLIPPED_BITMAP || newFillStyle->fillStyleType == FILL_STYLE_REPEATING_BITMAP) {
					paintCmd = createCommand(cmdList, VDRAW_BITMAP_FILL);

					if (newFillStyle->fillStyleType == FILL_STYLE_CLIPPED_BITMAP) {
						paintCmd->tileMode = SkTileMode::kClamp;
					} else {
						paintCmd->tileMode = SkTileMode::kRepeat;
					}

					paintCmd->swfBitmap = newFillStyle->bitmap;
					paintCmd->matrix = newFillStyle->bitmapMatrix;
					paintCmd->matrix.SCALE(1/20.0, 1/20.0);
				} else {
					Panic("Bad fill");
				}
			}

			if (lineStyleIndex != 0) {
				LineStyle *newLineStyle = &shape->lineStyles[lineStyleIndex - 1];
				int color = newLineStyle->color;
				color = applyColorTransform(color, props.colorTransform);

				paintCmd = createCommand(cmdList, VDRAW_SET_LINE_STYLE);
				paintCmd->colors[0] = color;
				paintCmd->width = newLineStyle->width;

				paintCmd->startCapStyle = newLineStyle->startCapStyle;
				paintCmd->joinStyle = newLineStyle->joinStyle;
				paintCmd->miterLimitFactor = newLineStyle->miterLimitFactor;
			}

			if (paintCmd) {
				for (int i = 0; i < 16; i++) {
					int a, r, g, b;
					hexToArgb(paintCmd->colors[i], &a, &r, &g, &b);
					a = ((a/255.0) * props.alpha) * 255.0;
					paintCmd->colors[i] = argbToHex(a, r, g, b);
				}
			}
		}

		if (!cursor.equal(edge->start)) {
			cursor = edge->start;
			VDrawCommand *cmd = createCommand(cmdList, VDRAW_MOVE_TO);
			cmd->position = edge->start;
			pathOperations++;
		}
		if (edge->type == DRAW_EDGE_STRAIGHT_EDGE) {
			cursor = edge->control;
			VDrawCommand *cmd = createCommand(cmdList, VDRAW_LINE_TO);
			cmd->position = edge->control;
			pathOperations++;
		} else if (edge->type == DRAW_EDGE_CURVED_EDGE) {
			cursor = edge->anchor;
			VDrawCommand *cmd = createCommand(cmdList, VDRAW_QUAD_TO);
			cmd->control = edge->control;
			cmd->position = edge->anchor;
			pathOperations++;
		}
	}

	if (pathOperations > 0) {
		if (props.useClip) {
			createCommand(cmdList, VDRAW_CLIP_PATH);
		} else {
			createCommand(cmdList, VDRAW_DRAW_PATH);
		}
		createCommand(cmdList, VDRAW_RESET_PATH);
		createCommand(cmdList, VDRAW_END_SHAPE);
		cursor = v2();
		pathOperations = 0;
	}
#endif
}

int usingInvalidBlendModeWarnings = 0;
void genDrawSprite(SwfSprite *sprite, SpriteTransform *transforms, int transformsNum, DrawSpriteRecurseData recurse, VDrawCommandsList *cmdList, bool isNested) {
	int startingCmdIndex = cmdList->cmdsNum;
	Matrix3 localMatrix = mat3();
	SpriteTransform *matchingTransform = NULL;

	bool firstSprite = false;
	if (!recurse.inited) {
		Matrix3 matrix = mat3();
		if (!isNested) matrix.SCALE(skiaSys->superSampleScale);
		pushSpriteMatrix(cmdList, matrix);

		recurse.inited = true;
		recurse.path = frameMalloc(SPRITE_PATH_MAX_LEN);
		recurse.alpha = 1;
		recurse.tint = 0;
		recurse.colorTransform = makeColorTransform();
		recurse.blendMode = SWF_BLEND_NORMAL;

		if (skiaSys->canvas == skiaSys->mainCanvas && !isNested) localMatrix.SCALE(skiaSys->scale);

		firstSprite = true;
		if (transformsNum > 0 && transforms[0].pathsNum > 0 && streq(transforms[0].paths[0], "_")) {
			matchingTransform = &transforms[0];
		}
	}

	char *name = sprite->name;
	if (recurse.altName) {
		name = recurse.altName;
		recurse.altName = NULL;
	}

	int charsAddedToPath = 0;
	if (name) {
		charsAddedToPath += strlen(name) + 1;
		if (strlen(recurse.path) != 0) strcat(recurse.path, ".");
		strcat(recurse.path, name);

		for (int i = 0; i < transformsNum; i++) {
			SpriteTransform *trans = &transforms[i];

			for (int i = 0; i < trans->pathsNum; i++) {
				if (streq(trans->paths[i], recurse.path)) {
					matchingTransform = trans;
					break;
				}
			}

			if (matchingTransform) break;
		}
	}

	int frame = 0;
	if (matchingTransform) {
		frame = matchingTransform->frame;
		localMatrix = localMatrix * matchingTransform->matrix;
		recurse.font = matchingTransform->font;
		recurse.text = matchingTransform->text;
		recurse.alpha *= matchingTransform->alpha;
		recurse.tint = matchingTransform->tint;
		if (matchingTransform->swapsNum > 0) {
			recurse.swaps = matchingTransform->swaps;
			recurse.swapsNum = matchingTransform->swapsNum;
		}
		matchingTransform->touched = true;
		// if (keyJustPressed('A')) logf("Drawing %s frame %d\n", recurse.path, frame);
	}
	Font *font = recurse.font;
	char *text = recurse.text;

	bool canDraw = true;
	if (!sprite->isTextField && frame > sprite->framesNum-1) frame = sprite->framesNum-1;

	pushSpriteMatrix(cmdList, localMatrix);

	if (matchingTransform) matchingTransform->outMatrix = skiaSys->matrixStack[skiaSys->matrixStackNum-1];

	if (canDraw) {
		if (sprite->isTextField) {
			if (!font) font = fontSys->defaultFont;
			if (!text) text = sprite->initialText;
			Rect textRect = sprite->bounds;

#if 1
			if (text[0] != 0) {
				VDrawCommand *cmd = createCommand(cmdList, VDRAW_TEXT);
				cmd->font = font;
				cmd->text = text;
				cmd->position = getPosition(textRect);
				cmd->control = getSize(textRect);
				cmd->colors[0] = setAofArgb(sprite->textColor, 255*recurse.alpha);

				if (sprite->textAlign == TEXT_ALIGN_LEFT) {
					cmd->gravity = v2(0, 0);
				} else if (sprite->textAlign == TEXT_ALIGN_RIGHT) {
					cmd->gravity = v2(1, 0);
				} else if (sprite->textAlign == TEXT_ALIGN_CENTER) {
					cmd->gravity = v2(0.5, 0);
				} else {
					logf("Bad text align\n");
				}
			}
#endif

#if 0
			Rect rect = inflatePerc(textRect, -0.15);
			pushCamera2d(skiaSys->matrixStack[skiaSys->matrixStackNum-1]);
			DrawTextProps props = newDrawTextProps();
			props.font = font;
			props.color = setAofArgb(sprite->textColor, 255*recurse.alpha);
			drawTextInRect(text, props, rect);
			drawRectOutline(textRect, 2, 0xFF0000FF);
			popCamera2d();
#endif

#if 0
			textRect = topMatrix.multiply(textRect);
			Vec2 pos;
			pos.x = textRect.x;
			pos.y = textRect.y;
			float width = textRect.width;
			// drawRectOutline(textRect, 2, 0xFF0000FF);
			Vec2 size = getTextSize(font, text, width);
			if (sprite->textAlign == TEXT_ALIGN_LEFT) {
				// Nothing...
			} else if (sprite->textAlign == TEXT_ALIGN_RIGHT) {
				pos.x += textRect.width - size.x;
			} else if (sprite->textAlign == TEXT_ALIGN_CENTER) {
				pos.x += textRect.width/2.0 - size.x/2.0;
			} else {
				logf("Bad text align\n");
			}
			drawRectOutline(textRect, 2, 0xFF0000FF);
			drawText(font, text, pos, sprite->textColor, width); //@incomplte
#endif
		} else {
			SwfDrawable *depths = sprite->frames[frame].depths;
			int depthsNum = sprite->frames[frame].depthsNum;
			int clippingTill = 0;
			char *currentLayerName = NULL;
			for (int i = 0; i < depthsNum; i++) {
				bool canSubDraw = true;

				SwfDrawable *drawable = &depths[i];
				if (drawable->type == SWF_DRAWABLE_NONE) canSubDraw = false;

				if (currentLayerName && matchingTransform) {
					for (int i = 0; i < matchingTransform->layersToHideNum; i++) {
						if (streq(currentLayerName, matchingTransform->layersToHide[i])) {
							canSubDraw = false;
						}
					}
				}

				if (drawable->name && stringStartsWith(drawable->name, "LAYERSTART_")) {
					currentLayerName = frameStringClone(drawable->name + strlen("LAYERSTART_"));
				}
				if (drawable->name && stringStartsWith(drawable->name, "LAYEREND_")) {
					char *layerToEnd = drawable->name + strlen("LAYEREND_");
					if (!streq(layerToEnd, currentLayerName)) logf("Tried to end layer %s, but we're in layer %s\n", layerToEnd, currentLayerName);
					currentLayerName = NULL;
				}

				if (canSubDraw) {
					bool nextUseClip = recurse.useClip;

					if (drawable->clipDepth != 0) {
						if (clippingTill > 0) logf("Trying to clip, but already clipping %d\n", clippingTill);
						clippingTill = drawable->clipDepth;
						nextUseClip = true;
						createCommand(cmdList, VDRAW_SAVE);
					}

					pushSpriteMatrix(cmdList, toMatrix3(drawable->matrix));
					if (drawable->type == SWF_DRAWABLE_SHAPE) {
						DrawShapeProps props = {};
						props.useClip = nextUseClip;
						props.alpha = recurse.alpha;
						props.tint = recurse.tint;
						props.colorTransform = recurse.colorTransform;

						VDrawCommand *cmd = createCommand(cmdList, VDRAW_SET_BLEND_MODE); //@todo This could be a bit faster if you didn't set blend mode every shape
						if (recurse.blendMode == SWF_BLEND_NORMAL) {
							cmd->blendMode = BLEND_NORMAL;
						} else if (recurse.blendMode == SWF_BLEND_MULTIPLY) {
							cmd->blendMode = BLEND_MULTIPLY;
						} else if (recurse.blendMode == SWF_BLEND_SCREEN) {
							cmd->blendMode = BLEND_SCREEN;
						} else if (recurse.blendMode == SWF_BLEND_LIGHTEN) {
							if (usingInvalidBlendModeWarnings < 8) logf("Using invalid blend mode\n");
							usingInvalidBlendModeWarnings++;
						} else if (recurse.blendMode == SWF_BLEND_DARKEN) {
							if (usingInvalidBlendModeWarnings < 8) logf("Using invalid blend mode\n");
							usingInvalidBlendModeWarnings++;
						} else if (recurse.blendMode == SWF_BLEND_DIFFERENCE) {
							if (usingInvalidBlendModeWarnings < 8) logf("Using invalid blend mode\n");
							usingInvalidBlendModeWarnings++;
						} else if (recurse.blendMode == SWF_BLEND_ADD) {
							if (usingInvalidBlendModeWarnings < 8) logf("Using invalid blend mode\n");
							usingInvalidBlendModeWarnings++;
						} else if (recurse.blendMode == SWF_BLEND_SUBTRACT) {
							if (usingInvalidBlendModeWarnings < 8) logf("Using invalid blend mode\n");
							usingInvalidBlendModeWarnings++;
						} else if (recurse.blendMode == SWF_BLEND_INVERT) {
							if (usingInvalidBlendModeWarnings < 8) logf("Using invalid blend mode invert\n");
							usingInvalidBlendModeWarnings++;
						} else if (recurse.blendMode == SWF_BLEND_ALPHA) {
							if (usingInvalidBlendModeWarnings < 8) logf("Using invalid blend mode alpha\n");
							usingInvalidBlendModeWarnings++;
						} else if (recurse.blendMode == SWF_BLEND_ERASE) {
							if (usingInvalidBlendModeWarnings < 8) logf("Using invalid blend mode erase\n");
							usingInvalidBlendModeWarnings++;
						} else if (recurse.blendMode == SWF_BLEND_OVERLAY) {
							if (usingInvalidBlendModeWarnings < 8) logf("Using invalid blend mode\n");
							usingInvalidBlendModeWarnings++;
						} else if (recurse.blendMode == SWF_BLEND_HARDLIGHT) {
							if (usingInvalidBlendModeWarnings < 8) logf("Using invalid blend mode\n");
							usingInvalidBlendModeWarnings++;
						}
						if (usingInvalidBlendModeWarnings == 8) logf("Too many blend mode warnings, stopping reports.\n");

						genDrawShape(drawable->shape, props, cmdList);
					} else if (drawable->type == SWF_DRAWABLE_SPRITE) {
						DrawSpriteRecurseData newRecurse = recurse;
						newRecurse.useClip = nextUseClip;
						newRecurse.altName = drawable->name;
						if (drawable->colorTransform) newRecurse.colorTransform = applyColorTransform(newRecurse.colorTransform, *drawable->colorTransform);

						bool shouldStopBlur = false;
						for (int i = 0; i < drawable->filtersNum; i++) {
							SwfFilter *filter = &drawable->filters[i];
							if (filter->type == SWF_FILTER_BLUR) {
								if (skiaSys->blurEnabled) {
									VDrawCommand *cmd = createCommand(cmdList, VDRAW_START_BLUR);
									cmd->position = v2(filter->blurFilter.blurX, filter->blurFilter.blurY);
									shouldStopBlur = true;
								}
							}
						}
						if (drawable->spriteBlendMode != SWF_BLEND_NORMAL) newRecurse.blendMode = (SwfBlendMode)drawable->spriteBlendMode;
						genDrawSprite(drawable->sprite, transforms, transformsNum, newRecurse, cmdList);
						if (shouldStopBlur) createCommand(cmdList, VDRAW_END_BLUR);
					} else {
						logf("Bad place object character\n");
					}
					popSpriteMatrix(cmdList);
				}

				if (clippingTill != 0 && i >= clippingTill-1) {
					clippingTill = 0;
					createCommand(cmdList, VDRAW_RESTORE);
				}
			}

			if (clippingTill != 0) {
				// logf("Clipping mismatch %d (but only %d)\n", clippingTill, depthsNum); // Apperently this just happens sometimes
				createCommand(cmdList, VDRAW_RESTORE);
			}
		}
	}

	if (recurse.swapsNum) {
		for (int i = startingCmdIndex; i < cmdList->cmdsNum; i++) {
			VDrawCommand *cmd = &cmdList->cmds[i];
			if (cmd->type != VDRAW_SET_LINE_STYLE && cmd->type != VDRAW_SET_SOLID_FILL) continue; //@todo allow gradients here
			for (int i = 0; i < recurse.swapsNum; i++) {
				VDrawPaletteSwap *swap = &recurse.swaps[i];
				for (int i = 0; i < 16; i++) { //@speed You can only do colors[0] if it's a non-gradient
					if (cmd->colors[i] == swap->from) {
						cmd->colors[i] = swap->to;
					}
				}
			}
		}
	}
	if (matchingTransform) {
		for (int i = 0; i < matchingTransform->drawSpriteCallsNum; i++) {
			genDrawSprite(
				matchingTransform->drawSpriteCalls[i].sprite,
				matchingTransform->drawSpriteCalls[i].transforms,
				matchingTransform->drawSpriteCalls[i].transformsNum,
				{},
				cmdList,
				true
			);
		}
	}

	popSpriteMatrix(cmdList);

	int zeroIndex = strlen(recurse.path) - charsAddedToPath;
	if (zeroIndex < 0) zeroIndex = 0;
	recurse.path[zeroIndex] = 0;

	if (firstSprite) {
		popSpriteMatrix(cmdList);
	}

	createCommand(cmdList, VDRAW_END_SPRITE);
}

#if USING_CAIROVG
void cairo_quadratic_to (cairo_t *cr, double x1, double y1, double x2, double y2);
void cairo_quadratic_to (cairo_t *cr, double x1, double y1, double x2, double y2) {
	double x0, y0;
	cairo_get_current_point (cr, &x0, &y0);
	cairo_curve_to (cr,
		2.0 / 3.0 * x1 + 1.0 / 3.0 * x0,
		2.0 / 3.0 * y1 + 1.0 / 3.0 * y0,
		2.0 / 3.0 * x1 + 1.0 / 3.0 * x2,
		2.0 / 3.0 * y1 + 1.0 / 3.0 * y2,
		x2, y2);
}
#endif

void execCommands(VDrawCommandsList *cmdList) {
	if (skiaSys->width == 0) {
		logf("You have to call resize at least once to draw\n");
		return;
	}

#if USING_CAIROVG
	cairo_t *cr = skiaSys->cairoContext;

	bool doStroke = false;

	for (int i = 0; i < cmdList->cmdsNum; i++) {
		VDrawCommand *cmd = &cmdList->cmds[i];
		if (cmd->type == VDRAW_SET_MATRIX) {
			float sx = cmd->matrix.data[0];
			float sy = cmd->matrix.data[4];
			float kx = cmd->matrix.data[1];
			float ky = cmd->matrix.data[3];
			float tx = cmd->matrix.data[6];
			float ty = cmd->matrix.data[7];

			cairo_matrix_t cairoMatrix;
			cairo_matrix_init(&cairoMatrix, sx, kx, ky, sy, tx, ty);
			cairo_set_matrix(cr, &cairoMatrix);
		} else if (cmd->type == VDRAW_MOVE_TO) {
			cairo_move_to(cr, cmd->position.x, cmd->position.y);
		} else if (cmd->type == VDRAW_LINE_TO) {
			cairo_line_to(cr, cmd->position.x, cmd->position.y);
		} else if (cmd->type == VDRAW_QUAD_TO) {
			cairo_quadratic_to(cr, cmd->control.x, cmd->control.y, cmd->position.x, cmd->position.y);
			// cairo_quadratic_to(cr, cmd->position.x, cmd->position.y, cmd->control.x, cmd->control.y);
		} else if (cmd->type == VDRAW_DRAW_PATH) {
			if (doStroke) {
				cairo_stroke(cr);
			} else {
				cairo_fill(cr);
			}
		} else if (cmd->type == VDRAW_CLIP_PATH) {
			cairo_clip(cr);
			cairo_new_path(cr);
		} else if (cmd->type == VDRAW_RESET_PATH) {
			cairo_new_path(cr);
		} else if (cmd->type == VDRAW_SAVE) {
			cairo_save(cr);
		} else if (cmd->type == VDRAW_RESTORE) {
			cairo_restore(cr);
		} else if (cmd->type == VDRAW_END_SHAPE) {
		} else if (cmd->type == VDRAW_SET_BLEND_MODE) {
		} else if (cmd->type == VDRAW_SET_SOLID_FILL) {
			doStroke = false;
			Vec4 color = hexToArgbFloat(cmd->colors[0]);
			float a = color.x;
			float r = color.y;
			float g = color.z;
			float b = color.w;
			cairo_set_source_rgba(cr, r, g, b, a);
		} else if (cmd->type == VDRAW_SET_LINEAR_GRADIENT_FILL) {
			doStroke = false;
			Vec4 color = hexToArgbFloat(cmd->colors[0]);
			float a = color.x;
			float r = color.y;
			float g = color.z;
			float b = color.w;
			cairo_set_source_rgba(cr, r, g, b, a);
		} else if (cmd->type == VDRAW_SET_RADIAL_GRADIENT_FILL) {
			doStroke = false;
			Vec4 color = hexToArgbFloat(cmd->colors[0]);
			float a = color.x;
			float r = color.y;
			float g = color.z;
			float b = color.w;
			cairo_set_source_rgba(cr, r, g, b, a);
		} else if (cmd->type == VDRAW_SET_FOCAL_GRADIENT_FILL) {
			doStroke = false;
			Vec4 color = hexToArgbFloat(cmd->colors[0]);
			float a = color.x;
			float r = color.y;
			float g = color.z;
			float b = color.w;
			cairo_set_source_rgba(cr, r, g, b, a);
		} else if (cmd->type == VDRAW_SET_LINE_STYLE) {
			doStroke = true;

			cairo_line_cap_t capStyle = CAIRO_LINE_CAP_ROUND;
			if (cmd->startCapStyle == CAP_STYLE_NONE) capStyle = CAIRO_LINE_CAP_BUTT;
			if (cmd->startCapStyle == CAP_STYLE_SQUARE) capStyle = CAIRO_LINE_CAP_SQUARE;

			cairo_set_line_cap(cr, capStyle);
			cairo_set_miter_limit(cr, cmd->miterLimitFactor);

			cairo_line_join_t joinStyle = CAIRO_LINE_JOIN_MITER;
			if (cmd->joinStyle == JOIN_STYLE_ROUND) joinStyle = CAIRO_LINE_JOIN_ROUND;
			if (cmd->joinStyle == JOIN_STYLE_BEVEL) joinStyle = CAIRO_LINE_JOIN_BEVEL;

			cairo_set_line_join(cr, joinStyle);

			float width = cmd->width;
			if (cmd->width == 0.0) logf("Why 0 width??\n");
			cairo_set_line_width(cr, cmd->width);

			Vec4 color = hexToArgbFloat(cmd->colors[0]);
			float a = color.x;
			float r = color.y;
			float g = color.z;
			float b = color.w;
			cairo_set_source_rgba(cr, r, g, b, a);
		} else if (cmd->type == VDRAW_BITMAP_FILL) {
		} else if (cmd->type == VDRAW_TEXT) {
		} else {
			logf("Unknown VDrawCommandType %d\n", cmd->type);
		}
	}
#else
	SkPaint paint = SkPaint();
	if (skiaSys->useGpu) {
		// if (skiaSys->msaaSamples > 0) paint.setAntiAlias(true);
	} else {
		if (skiaSys->useCpuAA) paint.setAntiAlias(true);
	}

	SkPath path = SkPath();

	for (int i = 0; i < cmdList->cmdsNum; i++) {
		VDrawCommand *cmd = &cmdList->cmds[i];
		if (cmd->type == VDRAW_SET_MATRIX) {
			skiaSys->canvas->setMatrix(toSkMatrix(cmd->matrix));
		} else if (cmd->type == VDRAW_MOVE_TO) {
			path.moveTo(cmd->position.x, cmd->position.y);
		} else if (cmd->type == VDRAW_LINE_TO) {
			path.lineTo(cmd->position.x, cmd->position.y);
		} else if (cmd->type == VDRAW_QUAD_TO) {
			path.quadTo(cmd->control.x, cmd->control.y, cmd->position.x, cmd->position.y);
		} else if (cmd->type == VDRAW_DRAW_PATH) {
			skiaSys->canvas->drawPath(path, paint);
		} else if (cmd->type == VDRAW_CLIP_PATH) {
			skiaSys->canvas->clipPath(path);
		} else if (cmd->type == VDRAW_RESET_PATH) {
			path.reset();
		} else if (cmd->type == VDRAW_DRAW_CACHED_PATH) {
			skiaSys->canvas->drawPath(*cmd->path, paint);
		} else if (cmd->type == VDRAW_CLIP_CACHED_PATH) {
			skiaSys->canvas->clipPath(*cmd->path, true);
		} else if (cmd->type == VDRAW_SAVE) {
			skiaSys->canvas->save();
		} else if (cmd->type == VDRAW_RESTORE) {
			skiaSys->canvas->restore();
		} else if (cmd->type == VDRAW_START_SHAPE) {
#if DO_LAYER_BLUR
#else
			if (!isZero(skiaSys->currentBlur)) {
				sk_sp<SkImageFilter> filter = SkImageFilters::Blur(skiaSys->currentBlur.x, skiaSys->currentBlur.y, SkTileMode::kDecal, NULL);
				paint.setImageFilter(filter);
			}
#endif
		} else if (cmd->type == VDRAW_END_SHAPE) {
			paint.setShader(NULL);
			paint.setImageFilter(NULL);
		} else if (cmd->type == VDRAW_END_SPRITE) {
		} else if (cmd->type == VDRAW_START_BLUR) {
#if DO_LAYER_BLUR
			SkPaint blurPaint = SkPaint();
			sk_sp<SkImageFilter> filter = SkImageFilters::Blur(cmd->position.x, cmd->position.y, SkTileMode::kDecal, NULL);
			blurPaint.setImageFilter(filter);
			skiaSys->canvas->saveLayer(NULL, &blurPaint);
#else
			skiaSys->currentBlur = cmd->position;
#endif
		} else if (cmd->type == VDRAW_END_BLUR) {
#if DO_LAYER_BLUR
			skiaSys->canvas->restore();
#else
			skiaSys->currentBlur = v2();
#endif
		} else if (cmd->type == VDRAW_SET_BLEND_MODE) {
			if (cmd->blendMode == BLEND_NORMAL) {
				paint.setBlendMode(SkBlendMode::kSrcOver);
			} else if (cmd->blendMode == BLEND_MULTIPLY) {
				paint.setBlendMode(SkBlendMode::kMultiply);
			} else if (cmd->blendMode == BLEND_SCREEN) {
				paint.setBlendMode(SkBlendMode::kScreen);
			} else {
				logf("Unknown blend mode\n");
			}
		} else if (cmd->type == VDRAW_SET_SOLID_FILL) {
			paint.setStyle(SkPaint::Style::kFill_Style);
			paint.setColor(cmd->colors[0]);
		} else if (cmd->type == VDRAW_SET_LINEAR_GRADIENT_FILL) {
			paint.setStyle(SkPaint::Style::kFill_Style);

			SkMatrix mat = toSkMatrix(cmd->matrix);
			SkPoint points[2] = {SkPoint::Make(-16384/20, 0), SkPoint::Make(16384/20, 0)};
			paint.setShader(SkGradientShader::MakeLinear(points, (SkColor *)cmd->colors, cmd->gradientRatios, cmd->gradientRatiosNum, SkTileMode::kClamp, 0, &mat));
		} else if (cmd->type == VDRAW_SET_RADIAL_GRADIENT_FILL) {
			paint.setStyle(SkPaint::Style::kFill_Style);

			SkMatrix mat = toSkMatrix(cmd->matrix);
			paint.setShader(SkGradientShader::MakeRadial( SkPoint::Make(0, 0), 16384.0/20, (SkColor *)cmd->colors, cmd->gradientRatios, cmd->gradientRatiosNum, SkTileMode::kClamp, 0, &mat));
		} else if (cmd->type == VDRAW_SET_FOCAL_GRADIENT_FILL) {
			paint.setStyle(SkPaint::Style::kFill_Style);
			SkMatrix mat = toSkMatrix(cmd->matrix);
			paint.setShader(SkGradientShader::MakeTwoPointConical(
				SkPoint::Make(16384/20.0 * cmd->position.x, 0),
				0,
				SkPoint::Make(0, 0),
				16384/20.0,
				(SkColor *)cmd->colors,
				cmd->gradientRatios,
				cmd->gradientRatiosNum,
				SkTileMode::kClamp,
				0,
				&mat
			));
		} else if (cmd->type == VDRAW_SET_LINE_STYLE) {
			paint.setStyle(SkPaint::Style::kStroke_Style);
			paint.setColor(cmd->colors[0]);
			paint.setStrokeWidth(cmd->width);
			if (cmd->startCapStyle == CAP_STYLE_NONE) paint.setStrokeCap(SkPaint::Cap::kButt_Cap);
			if (cmd->startCapStyle == CAP_STYLE_ROUND) paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
			if (cmd->startCapStyle == CAP_STYLE_SQUARE) paint.setStrokeCap(SkPaint::Cap::kSquare_Cap);

			if (cmd->joinStyle == JOIN_STYLE_ROUND) paint.setStrokeJoin(SkPaint::Join::kRound_Join);
			if (cmd->joinStyle == JOIN_STYLE_BEVEL) paint.setStrokeJoin(SkPaint::Join::kBevel_Join);
			if (cmd->joinStyle == JOIN_STYLE_MITER) {
				paint.setStrokeJoin(SkPaint::Join::kMiter_Join);
				paint.setStrokeMiter(cmd->miterLimitFactor);
			}

		} else if (cmd->type == VDRAW_BITMAP_FILL) {
			SwfBitmap *bitmap = cmd->swfBitmap;
			if (bitmap) {
				if (!bitmap->bitmapRuntimePointer) {
					SkBitmap *skiaBitmap = new SkBitmap();
					skiaBitmap->allocN32Pixels(bitmap->width, bitmap->height);
					skiaBitmap->setInfo(skiaBitmap->info().makeColorType(SkColorType::kRGBA_8888_SkColorType));
					skiaBitmap->setPixels(bitmap->pixels);
					bitmap->bitmapRuntimePointer = skiaBitmap;
				}
				//@todo I have no idea how to apply colorTransforms to bitmaps
				SkBitmap *skiaBitmap = (SkBitmap *)bitmap->bitmapRuntimePointer;
				paint.setShader(skiaBitmap->makeShader(cmd->tileMode, cmd->tileMode, SkSamplingOptions(), toSkMatrix(cmd->matrix)));
			} else {
				logf("Missing bitmap\n");
			}
		} else if (cmd->type == VDRAW_TEXT) {
			SkPaint outlinePaint = SkPaint();
			outlinePaint.setStyle(SkPaint::Style::kStroke_Style);
			outlinePaint.setColor(0xFFFF0000);
			outlinePaint.setStrokeWidth(1);

			SkPaint outlinePaint2 = SkPaint();
			outlinePaint2.setStyle(SkPaint::Style::kStroke_Style);
			outlinePaint2.setColor(0xFF0000FF);
			outlinePaint2.setStrokeWidth(1);

			SkPaint outlinePaint3 = SkPaint();
			outlinePaint3.setStyle(SkPaint::Style::kStroke_Style);
			outlinePaint3.setColor(0xFF00FF00);
			outlinePaint3.setStrokeWidth(1);

			if (!cmd->font->skiaUserData) {
				int fontDataSize;
				void *fontData = readFile(cmd->font->path, &fontDataSize);
				sk_sp<SkData> skiaData = SkData::MakeWithCopy(fontData, fontDataSize);
				free(fontData);
				sk_sp<SkTypeface> skiaTypeface = SkTypeface::MakeFromData(skiaData);
				SkFont *skiaFont = new SkFont(skiaTypeface, cmd->font->fontSize);
				cmd->font->skiaUserData = skiaFont;
			}

			SkFont *skiaFont = (SkFont *)cmd->font->skiaUserData;
			sk_sp<SkTextBlob> skiaTextBlob = SkTextBlob::MakeFromString(cmd->text, *skiaFont);
			SkRect skiaBounds = skiaTextBlob->bounds();

			Vec2 actualTextSize;
			actualTextSize.x = skiaBounds.width();
			actualTextSize.y = skiaBounds.height();

			Rect toFit = makeRect(cmd->position, cmd->control);

			Rect textRect = getCenteredRectOfAspect(toFit, actualTextSize, cmd->gravity);

			// skiaSys->canvas->drawRect(skiaBounds, outlinePaint);
			// skiaSys->canvas->drawRect(SkRect::MakeXYWH(toFit.x, toFit.y, toFit.width, toFit.height), outlinePaint2);

			Matrix3 matrix = mat3();
			matrix.SCALE(getSize(textRect) / actualTextSize);
			matrix.TRANSLATE(getPosition(textRect) - v2(skiaBounds.x(), skiaBounds.y()));

			skiaSys->canvas->save();
			skiaSys->canvas->concat(toSkMatrix(matrix));
			// skiaSys->canvas->drawRect(skiaBounds, outlinePaint3);

			SkPaint fontPaint = SkPaint();
			fontPaint.setStyle(SkPaint::Style::kFill_Style);
			fontPaint.setColor(cmd->colors[0]);
			skiaSys->canvas->drawTextBlob(skiaTextBlob, 0, 0, fontPaint);

			skiaSys->canvas->restore();
		} else {
			logf("Unknown VDrawCommandType %d\n", cmd->type);
		}
	}
#endif
}

void clearSkia(int color) {
#if USING_CAIROVG
	cairo_save(skiaSys->cairoContext);
	cairo_set_source_rgba(skiaSys->cairoContext, 1, 0, 0, 1);
	cairo_set_operator(skiaSys->cairoContext, CAIRO_OPERATOR_SOURCE);
	cairo_paint(skiaSys->cairoContext);
	cairo_restore(skiaSys->cairoContext);
#else
	skiaSys->canvas->clear(color);
#endif
}

void startSkiaFrame() {
	if (skiaSys->matrixStackNum != 1) logf("Matrix stack mismatch\n");

#if USING_CAIROVG
	clearSkia(0);
#else
	skiaSys->canvas->clear(0);
#endif
}

void endSkiaFrame() {
#if USING_CAIROVG
	cairo_t *cr = skiaSys->cairoContext;
	// cairo_select_font_face (cr, "serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	// cairo_set_font_size (cr, 32.0);
	// cairo_set_source_rgb (cr, 0.0, 0.0, 1.0);
	// cairo_move_to (cr, 10.0, 50.0);
	// cairo_show_text (cr, "Hello, world");

	// clearSkia();
	// cairo_new_path(cr);
	// cairo_rectangle(cr, 100, 100, 200, 200);
	// cairo_set_source_rgba(cr, 0, 1, 0, 1);
	// cairo_fill(cr);

	u8 *pixels = cairo_image_surface_get_data(skiaSys->cairoSurface);
	for (int y = 0; y < skiaSys->height; y++) {
		for (int x = 0; x < skiaSys->width; x++) {
			u8 b = pixels[(y * skiaSys->width + x)*4 + 0];
			u8 g = pixels[(y * skiaSys->width + x)*4 + 1];
			u8 r = pixels[(y * skiaSys->width + x)*4 + 2];
			u8 a = pixels[(y * skiaSys->width + x)*4 + 3];

			skiaSys->cpuFramePixels[(y * skiaSys->width + x)*4 + 0] = r;
			skiaSys->cpuFramePixels[(y * skiaSys->width + x)*4 + 1] = g;
			skiaSys->cpuFramePixels[(y * skiaSys->width + x)*4 + 2] = b;
			skiaSys->cpuFramePixels[(y * skiaSys->width + x)*4 + 3] = a;
		}
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, skiaSys->backTexture->id);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, skiaSys->width, skiaSys->height, GL_RGBA8, GL_UNSIGNED_BYTE, skiaSys->cpuFramePixels);
#else // USING_CAIROVG == 0
	if (skiaSys->useGpu) {
		setRendererBlendMode(BLEND_SKIA);

		skiaSys->grDirectContext->resetContext();
#if DO_DRAW_AFTER_CONTEXT_SWITCH
		execCommands(&skiaSys->immVDrawCommandsList);
		skiaSys->immVDrawCommandsList.cmdsNum = 0;
#endif

		skiaSys->grDirectContext->flushAndSubmit();

		clearScissor();
		resetRenderContext();

#if RAYLIB_MODE
		glBindVertexArray(0);
		glVertexAttribDivisor(0, 0);
		glVertexAttribDivisor(1, 0);
		glVertexAttribDivisor(2, 0);
#endif

#if DO_SKIA_MSAA
		glBindFramebuffer(GL_READ_FRAMEBUFFER, skiaSys->multiSampleFramebufferId);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, skiaSys->flatFramebufferId);
#if RAYLIB_MODE 
#else
		glBlitFramebuffer(0, 0, skiaSys->width, skiaSys->height, 0, 0, skiaSys->width, skiaSys->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
#endif

#if RAYLIB_MODE 
		//@todo
		if (renderer->targetTextureStackNum > 0) {
			glBindFramebuffer(GL_FRAMEBUFFER, renderer->targetTextureStack[renderer->targetTextureStackNum-1]->raylibRenderTexture.id);
		} else {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
#else
		if (renderer->currentFramebuffer) {
			glBindFramebuffer(GL_FRAMEBUFFER, renderer->currentFramebuffer->id);
		} else {
			if (renderer->currentTargetTexture) {
				setTargetTexture(renderer->currentTargetTexture);
			} else {
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
		}
#endif

#else
		pushTargetTexture(skiaSys->backTexture);
		clearRenderer();

		Matrix3 matrix = mat3();
		matrix.SCALE(getSize(skiaSys->backTexture));
		drawSimpleTexture(skiaSys->skiaTexture, matrix);
		popTargetTexture();

#endif
	} else {
#if DO_DRAW_AFTER_CONTEXT_SWITCH
		execCommands(&skiaSys->immVDrawCommandsList);
		skiaSys->immVDrawCommandsList.cmdsNum = 0;
#endif

		SkImageInfo info = {};
		size_t rowBytes = 0;

		u8 *inPixels = (u8 *)skiaSys->mainCanvas->accessTopLayerPixels(&info, &rowBytes);
		u8 *outPixels = skiaSys->cpuFramePixels;
		int w = skiaSys->width;
		int h = skiaSys->height;
		for (int y = 0; y < h; y++) {
			int y1 = (h-1)-y;
			memcpy(outPixels + (y1*w)*4, inPixels + (y*w)*4, w * 4);
		}

#if RAYLIB_MODE
		setTextureData(skiaSys->backTexture, outPixels, skiaSys->width, skiaSys->height);
#else
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, skiaSys->backTexture->texture->id);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, skiaSys->width, skiaSys->height, GL_RGBA8, GL_UNSIGNED_BYTE, outPixels);
#endif
	}
#endif
}

void initSpriteTransforms(SpriteTransform *transforms, int transformsNum) {
	memset(transforms, 0, sizeof(SpriteTransform) * transformsNum);

	for (int i = 0; i < transformsNum; i++) {
		//
		/// RcAnim inits sprite transforms manually!
		//
		SpriteTransform *transform = &transforms[i];
		transform->matrix = mat3();
		transform->alpha = 1;
	}
}

SpriteTransform *getSpriteTransform(SpriteTransform *transforms, int *transformsNum, char *pathName) {
	SpriteTransform *transform = &transforms[*transformsNum];
	*transformsNum = *transformsNum + 1;

	transform->paths[transform->pathsNum++] = pathName;
	return transform;
};

void drawSwfAnalyzer() {
	struct SwfASys {
		char inputPath[PATH_MAX_LEN];
		Swf *swf;
		char spriteFilter[PATH_MAX_LEN];
		int selectedSprite;
		int selectedSpriteFrame;
		Vec2 spriteOffset;
		float spriteScale;
		RenderTexture *spriteTexture;
		int selectedDrawable;

		int selectedShape;
		Vec2 shapeOffset;
		float shapeScale;
		RenderTexture *shapeTexture;

		bool showUnnamedSprites;
	};

	static SwfASys *aSys = NULL;
	if (!aSys) {
		aSys = (SwfASys *)zalloc(sizeof(SwfASys));
		aSys->spriteTexture = createRenderTexture(256, 256);
		aSys->spriteScale = 1;

		aSys->shapeTexture = createRenderTexture(256, 256);
		aSys->shapeScale = 1;
		strcpy(aSys->inputPath, "assets/swf/Shared.swf");
	}

	ImGui::InputText("Input swf", aSys->inputPath, PATH_MAX_LEN);
	ImGui::SameLine();
	if (ImGui::Button("Analyze")) aSys->swf = loadSwf(aSys->inputPath);
	if (!aSys->swf) {
		// if (ImGui::Button("Shared.swf")) strcpy(aSys->inputPath, "assets/swf/Shared.swf");
		// if (ImGui::Button("SofiAnim.swf")) strcpy(aSys->inputPath, "assets/swf/SofiAnim.swf");
	}

	Swf *swf = aSys->swf;
	if (swf && swf->allSpritesNum > 0) {

		ImGui::BeginChild("spritesListChild", ImVec2(200, 500), true, 0); 
		ImGui::InputText("Filter", aSys->spriteFilter, PATH_MAX_LEN);
		ImGui::Checkbox("Show unnamed", &aSys->showUnnamedSprites);
		for (int i = 0; i < swf->allSpritesNum; i++) {
			SwfSprite *sprite = swf->allSprites[i];
			char *name = sprite->name;
			if (!name) name = frameSprintf("Unnamed sprite %d\n", i);
			if (name == NULL && !aSys->showUnnamedSprites) continue;
			if (aSys->spriteFilter[0]) {
				if (!strContains(name, aSys->spriteFilter)) continue;
			}
			if (ImGui::Selectable(name, aSys->selectedSprite == i)) {
				aSys->selectedSprite = i;
			}
		}

		ImGui::EndChild();
		ImGui::SameLine();

		ImGui::BeginChild("spriteChild", ImVec2(400, 500), true, 0); 
		SwfSprite *sprite = swf->allSprites[aSys->selectedSprite];

		ImGui::InputInt("Frame", &aSys->selectedSpriteFrame);
		aSys->selectedSpriteFrame = MathClamp(aSys->selectedSpriteFrame, 0, sprite->framesNum-1);

		// ImGui::InputInt("Frame", &aSys->selectedSpriteFrame);
		// aSys->selectedSpriteFrame = MathClamp(aSys->selectedSpriteFrame, 0, sprite->framesNum-1);

		int frame = aSys->selectedSpriteFrame;
		Rect rect = sprite->frameBounds[frame];
#if 0
		pushSkiaCanvas(getSize(aSys->spriteTexture), true);

		clearSkia(0xFFFFFFFF);
		SpriteTransform trans;
		initSpriteTransforms(&trans, 1);
		trans.paths[trans.pathsNum++] = "_";
		trans.matrix.SCALE(aSys->spriteScale);
		trans.matrix.TRANSLATE(-rect.x, -rect.y);
		trans.matrix.TRANSLATE(aSys->spriteOffset);
		trans.frame = frame;
		drawSprite(sprite, &trans, 1);

		altCanvasToTexture(aSys->spriteTexture);
		popSkiaCanvas();
#else
		Vec2 oldScale = skiaSys->scale;
		skiaSys->scale = v2(1, 1);
		startSkiaFrame();

		Matrix3 mat = mat3();
		SpriteTransform trans;
		initSpriteTransforms(&trans, 1);
		trans.paths[trans.pathsNum++] = "_";
		trans.matrix.SCALE(aSys->spriteScale);
		trans.matrix.TRANSLATE(-rect.x, -rect.y);
		trans.matrix.TRANSLATE(aSys->spriteOffset);
		trans.frame = frame;
		drawSprite(sprite, &trans, 1);

		endSkiaFrame();
		skiaSys->scale = oldScale;

		pushTargetTexture(aSys->spriteTexture);
		clearRenderer();
		drawSimpleTexture(skiaSys->backTexture);
		popTargetTexture();

#endif

		guiTexture(aSys->spriteTexture);
		ImGui::DragFloat2("spriteOffset", &aSys->spriteOffset.x);
		ImGui::DragFloat("spriteScale", &aSys->spriteScale, 0.01);
		Vec2 size = getSize(sprite->bounds);
		ImGui::Text(
			"Bounds: %.1f %.1f %.1f %.1f (%.1fx%.1f)",
			sprite->bounds.x,
			sprite->bounds.y,
			sprite->bounds.width,
			sprite->bounds.height,
			size.x,
			size.y
		);
		size = getSize(sprite->frameBounds[frame]);
		ImGui::Text(
			"FrameBounds: %.1f %.1f %.1f %.1f (%.1fx%.1f)",
			sprite->frameBounds[frame].x,
			sprite->frameBounds[frame].y,
			sprite->frameBounds[frame].width,
			sprite->frameBounds[frame].height,
			size.x,
			size.y
		);

		if (ImGui::Button("Dump")) {
			Vec2 size = getSize(sprite->bounds);
			size.x = ceilf(size.x);
			size.y = ceilf(size.y);
			resetSkia(size, v2(1, 1), true, 16);
			// game->skiaFlagsDirty = true;

			RenderTexture *texture = createRenderTexture(size.x, size.y);
			if (directoryExists("c:/bin/dump")) removeDirectory("c:/bin/dump");
			if (!createDirectory("c:/bin/dump")) logf("Failed to create dump dir\n");

			for (int i = 0; i < sprite->framesNum; i++) {
				Rect rect = sprite->frameBounds[i];

				startSkiaFrame();

				Matrix3 mat = mat3();
				SpriteTransform trans;
				initSpriteTransforms(&trans, 1);
				trans.paths[trans.pathsNum++] = "_";
				trans.matrix.TRANSLATE(-rect.x, -rect.y);
				trans.frame = i;
				drawSprite(sprite, &trans, 1);

				endSkiaFrame();

				pushTargetTexture(texture);
				clearRenderer();
				drawSimpleTexture(skiaSys->backTexture);
				popTargetTexture();

				u8 *bitmapData = getTextureData(texture, _F_TD_FLIP_Y);

				char *path = frameSprintf("C:/bin/dump/frame%d.png", i);
				if (!stbi_write_png(path, texture->width, texture->height, 4, bitmapData, texture->width*4)) logf("Failed to dump to %s!\n", path);
			}
			destroyTexture(texture);
		}

		{ /// Drawables
			SwfDrawable *drawables = NULL;
			int drawablesNum = 0;
			if (sprite->framesNum > 0) {
				drawables = sprite->frames[aSys->selectedSpriteFrame].depths;
				drawablesNum = sprite->frames[aSys->selectedSpriteFrame].depthsNum;
			}

			char *label = NULL;
			if (aSys->selectedDrawable < drawablesNum) {
				SwfDrawable *drawable = &drawables[aSys->selectedDrawable];
				if (drawable->type == SWF_DRAWABLE_SPRITE) label = drawable->sprite->name;
				else if (drawable->type == SWF_DRAWABLE_SHAPE) label = frameSprintf("%d: Shape", aSys->selectedDrawable);
				else label = frameSprintf("%d: ???", aSys->selectedDrawable);
			}
			if (ImGui::BeginCombo("Drawables", label, ImGuiComboFlags_None)) {
				for (int i = 0; i < drawablesNum; i++) {
					SwfDrawable *drawable = &drawables[i];
					char *label = frameSprintf("%d: ???", i);
					if (drawable->type == SWF_DRAWABLE_SPRITE) label = frameSprintf("%d: %s", i, drawable->sprite->name);
					else if (drawable->type == SWF_DRAWABLE_SHAPE) label = frameSprintf("%d: Shape", i);
					else label = frameSprintf("%d: ???", i);
					if (ImGui::Selectable(label, aSys->selectedDrawable == i)) {
						aSys->selectedDrawable = i;
					}
				}

				ImGui::EndCombo();
			}

			SwfDrawable *drawable = &drawables[aSys->selectedDrawable];
			if (drawable->type == SWF_DRAWABLE_SPRITE) {
				if (ImGui::Button("(^) Jump into sprite")) {
					for (int i = 0; i < aSys->swf->allSpritesNum; i++) {
						SwfSprite *sprite = aSys->swf->allSprites[i];
						if (sprite == drawable->sprite) {
							aSys->selectedSprite = i;
							logf("Jumped to sprite\n");
							break;
						}
					}
				}
			} else if (drawable->type == SWF_DRAWABLE_SHAPE) {
				if (ImGui::Button("(>) Analyze shape")) {
					for (int i = 0; i < aSys->swf->allShapesNum; i++) {
						SwfShape *shape = aSys->swf->allShapes[i];
						if (shape == drawable->shape) {
							aSys->selectedShape = i;
							logf("Jumped to shape\n");
							break;
						}
					}
				}
			} else {
				ImGui::Text("Unknown drawable type");
			}
		}

		ImGui::EndChild();

		ImGui::SameLine();
		{ /// Shapes
			ImGui::BeginChild("shapesChild", ImVec2(400, 500), true, 0); 
			SwfShape *shape = aSys->swf->allShapes[aSys->selectedShape];

			Vec2 oldScale = skiaSys->scale;
			skiaSys->scale = v2(1, 1);
			startSkiaFrame();

			Matrix3 matrix = mat3();
			matrix.SCALE(aSys->shapeScale);
			matrix.TRANSLATE(-rect.x, -rect.y);
			matrix.TRANSLATE(aSys->shapeOffset);

			DrawShapeProps props = newDrawShapeProps();
			drawShape(shape, matrix, props);

			endSkiaFrame();
			skiaSys->scale = oldScale;

			pushTargetTexture(aSys->shapeTexture);
			clearRenderer();
			drawSimpleTexture(skiaSys->backTexture);
			popTargetTexture();

			guiTexture(aSys->shapeTexture);
			Rect bounds = toRect(shape->shapeBounds);
			ImGui::Text("Bounds: %.1f %.1f %.1f %.1f", bounds.x, bounds.y, bounds.width, bounds.height);
			ImGui::Text("ShapeId: %d", shape->shapeId);
			if (ImGui::TreeNode("Line styles")) {
				for (int i = 0; i < shape->lineStylesNum; i++) {
					LineStyle *lineStyle = &shape->lineStyles[i];
					guiPushStyleColor(ImGuiCol_Text, lineStyle->color);
					ImGui::Text("lineStyle %d, width: %.1f, color: 0x%X", i, lineStyle->width, lineStyle->color);
					guiPopStyleColor();
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Fill styles")) {
				for (int i = 0; i < shape->fillStylesNum; i++) {
					FillStyle *fillStyle = &shape->fillStyles[i];
					guiPushStyleColor(ImGuiCol_Text, fillStyle->color);
					if (fillStyle->fillStyleType != FILL_STYLE_SOLID) {
						ImGui::Text("[Not showing non-solid fill]");
						ImGui::SameLine();
					}
					ImGui::Text("fillStyle %d, color: 0x%X", i, fillStyle->color);
					guiPopStyleColor();
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Edge records")) {
				for (int i = 0; i < shape->drawEdgesNum; i++) {
					DrawEdgeRecord *record = &shape->drawEdges[i];
					if (record->type == DRAW_EDGE_STRAIGHT_EDGE) {
						ImGui::Text(
							"lineTo (%d %d) %.1f %.1f -> %.1f %.1f",
							record->lineStyleIndex,
							record->fillStyleIndex,
							record->start.x,
							record->start.y,
							record->control.x,
							record->control.y
						);
					} else if (record->type == DRAW_EDGE_CURVED_EDGE) {
						ImGui::Text(
							"curveTo (%d %d) %.1f %.1f -> %.1f %.1f (%.1f %.1f)",
							record->lineStyleIndex,
							record->fillStyleIndex,
							record->start.x,
							record->start.y,
							record->control.x,
							record->control.y,
							record->anchor.x,
							record->anchor.y
						);
					} else {
						ImGui::Text("Invalid draw edge type");
					}
				}

				ImGui::TreePop();
			}
			ImGui::EndChild();
		}
	}
}
