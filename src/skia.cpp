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

	Matrix3 parentMatrix;

	HashMap *nameTransformMap;
};

struct SpriteLayerProps {
	char *name;
	float alpha;
};

struct DrawSpriteCall;
struct SpriteTransform {
#define SPRITE_TRANSFORM_PATHS_MAX 64
	char *paths[SPRITE_TRANSFORM_PATHS_MAX];
	int pathsNum;

	SpriteLayerProps *layerProps;
	int layerPropsNum;

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
	Matrix3 outPreTransformMatrix;

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
	VDRAW_CLIP_PATH,
	VDRAW_DRAW_CACHED_PATH,
	VDRAW_CLIP_CACHED_PATH,
	VDRAW_SAVE,
	VDRAW_RESTORE,
	VDRAW_START_SHAPE,
	VDRAW_END_SHAPE,
	VDRAW_END_SPRITE,
	VDRAW_START_BLUR,
	VDRAW_END_BLUR,
	VDRAW_START_COLOR_MATRIX,
	VDRAW_END_COLOR_MATRIX,
	VDRAW_SET_BLEND_MODE,
	VDRAW_END_BLEND_MODE,
	VDRAW_SET_SOLID_FILL,
	VDRAW_SET_LINEAR_GRADIENT_FILL,
	VDRAW_SET_RADIAL_GRADIENT_FILL,
	VDRAW_SET_FOCAL_GRADIENT_FILL,
	VDRAW_SET_LINE_STYLE,
	VDRAW_BITMAP_FILL,
	VDRAW_TEXT,
};
char *vDrawCommandTypeStrings[] = {
	"VDRAW_SET_MATRIX",
	"VDRAW_CLIP_PATH",
	"VDRAW_DRAW_CACHED_PATH",
	"VDRAW_CLIP_CACHED_PATH",
	"VDRAW_SAVE",
	"VDRAW_RESTORE",
	"VDRAW_START_SHAPE",
	"VDRAW_END_SHAPE",
	"VDRAW_END_SPRITE",
	"VDRAW_START_BLUR",
	"VDRAW_END_BLUR",
	"VDRAW_START_COLOR_MATRIX",
	"VDRAW_END_COLOR_MATRIX",
	"VDRAW_SET_BLEND_MODE",
	"VDRAW_END_BLEND_MODE",
	"VDRAW_SET_SOLID_FILL",
	"VDRAW_SET_LINEAR_GRADIENT_FILL",
	"VDRAW_SET_RADIAL_GRADIENT_FILL",
	"VDRAW_SET_FOCAL_GRADIENT_FILL",
	"VDRAW_SET_LINE_STYLE",
	"VDRAW_BITMAP_FILL",
	"VDRAW_TEXT",
};
struct VDrawCommand {
	VDrawCommandType type;

	Matrix3 matrix;
	SwfBlendMode blendMode;
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

struct SkiaSystem {
	SkBitmap bitmap;
	SkCanvas *mainCanvas;
	SkCanvas *canvas;

	bool useSaveLayerBlur;
	bool useSaveLayerBlendMode;

	const GrGLInterface *grInterface;
	GrDirectContext *grDirectContext;
	SkSurface *gpuSurface;
	Texture *skiaTexture;

#define MATRIX_STACK_MAX 256
	Matrix3 matrixStack[MATRIX_STACK_MAX];
	int matrixStackNum;

	int width;
	int height;
	Vec2 scale;
	float superSampleScale;
	Vec2 subFrameCachedScale;
	Vec2 outputSize;

	bool useCpuAA;
	bool blurDisabled;

	VDrawCommandsList immVDrawCommandsList;

	bool recordFrame;
	bool debugDrawTextFieldRectsThisFrame;
};
SkiaSystem *skiaSys = NULL;

void resetSkia(Vec2 size, Vec2 scale=v2(1, 1), float superSampleScale=2);
void drawSprite(SwfSprite *sprite, SpriteTransform *transforms, int transformsNum, DrawSpriteRecurseData recurse={});
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
void startSkiaSubFrame(float scale);
void endSkiaSubFrame();

void drawSkiaFrameOnTexture(Texture *Texture, bool doClear=false);
Texture *getSkiaFrameAsTexture(Vec2 size);
void deinitSkiaFont(Font *font);

void initSpriteTransforms(SpriteTransform *transforms, int transformsNum);

SpriteTransform *getSpriteTransform(SpriteTransform *transforms, int *transformsNum, char *pathName);
SpriteTransform *getNewOrExistingSpriteTransform(SpriteTransform *transforms, int *transformsNum, char *pathName);

#define CreateSpriteTransforms(varName, varNumName, count) \
	SpriteTransform varName[count]; \
	initSpriteTransforms(varName, count); \
	int varNumName = 0;

void drawSwfAnalyzer(char *basePath, Vec2 screenSize);

/// FUNCTIONS ^

// static void* get_proc(fastuidraw::c_string proc_name) { return SDL_GL_GetProcAddress(proc_name); }

void resetSkia(Vec2 size, Vec2 scale, float superSampleScale) {
	printf("Creating surface %fx%f (%f) surface\n", size.x, size.y, superSampleScale);
	if (!skiaSys) {
		skiaSys = (SkiaSystem *)zalloc(sizeof(SkiaSystem));
		skiaSys->matrixStack[skiaSys->matrixStackNum++] = mat3();
		SkGraphics::Init();
	}

	skiaSys->superSampleScale = superSampleScale;

	if (size.x == -1) {
		size.x = skiaSys->width / skiaSys->superSampleScale;
		size.y = skiaSys->height / skiaSys->superSampleScale;
		scale = skiaSys->scale;
	}
	// logf("Skia reset to %.1f %.1f\n", size.x, size.y);

	skiaSys->width = size.x * skiaSys->superSampleScale;
	skiaSys->height = size.y * skiaSys->superSampleScale;
	skiaSys->scale = scale;

	skiaSys->outputSize = size;

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

	sk_sp<SkColorSpace> colorSpace = SkColorSpace::MakeSRGB();
	SkSurfaceProps surfaceProps(0, SkPixelGeometry::kUnknown_SkPixelGeometry);

	SkImageInfo imageInfo = SkImageInfo::MakeN32(skiaSys->width, skiaSys->height, SkAlphaType::kPremul_SkAlphaType);

	if (skiaSys->gpuSurface) {
		delete skiaSys->gpuSurface;
		skiaSys->gpuSurface = NULL;
	}

	sk_sp<SkSurface> gpuSurface = SkSurfaces::RenderTarget(
		skiaSys->grDirectContext, skgpu::Budgeted::kNo,
		imageInfo,
		0, kBottomLeft_GrSurfaceOrigin,
		&surfaceProps
	);

	if (!gpuSurface) {
		if (fabs(superSampleScale - 2) < 0.01) {
			resetSkia(size, scale, 1.5);
			return;
		} else if (fabs(superSampleScale - 1.5) < 0.01) {
			resetSkia(size, scale, 1);
			return;
		} else if (fabs(superSampleScale - 1) < 0.01) {
			resetSkia(size, scale, 0.75);
			return;
		} else if (fabs(superSampleScale - 0.75) < 0.01) {
			resetSkia(size, scale, 0.5);
			return;
		} else {
			logf("Failed to create gpu surface\n");
			return;
		}
	}

	GrGLTextureInfo textureInfo = {};
	GrBackendTexture backendTexture = SkSurfaces::GetBackendTexture(gpuSurface.get(), SkSurfaces::BackendHandleAccess::kFlushRead);
	GrBackendTextures::GetGLTextureInfo(backendTexture, &textureInfo);

	//@incomplete Skia also hacks the renderer api using Raylib
	skiaSys->skiaTexture = (Texture *)zalloc(sizeof(Texture));
	skiaSys->skiaTexture->width = imageInfo.width();
	skiaSys->skiaTexture->height = imageInfo.height();
#if defined(RAYLIB_MODE)
	skiaSys->skiaTexture->backendTexture.raylibTexture.width = imageInfo.width();
	skiaSys->skiaTexture->backendTexture.raylibTexture.height = imageInfo.height();
	skiaSys->skiaTexture->backendTexture.raylibTexture.mipmaps = 1;
	skiaSys->skiaTexture->backendTexture.raylibTexture.format = textureInfo.fFormat;
	skiaSys->skiaTexture->backendTexture.raylibTexture.id = textureInfo.fID;
#else
	skiaSys->skiaTexture->backendTexture.width = imageInfo.width();
	skiaSys->skiaTexture->backendTexture.height = imageInfo.height();
	skiaSys->skiaTexture->backendTexture.id = textureInfo.fID;
#endif
	setTextureSmooth(skiaSys->skiaTexture, true);

	skiaSys->gpuSurface = gpuSurface.release();

	skiaSys->mainCanvas = skiaSys->gpuSurface->getCanvas();
	if (!skiaSys->mainCanvas) logf("Failed to create skia canvas\n");

	skiaSys->canvas = skiaSys->mainCanvas;

	printf("Good surface!\n");
}

void drawSprite(SwfSprite *sprite, SpriteTransform *transforms, int transformsNum, DrawSpriteRecurseData recurse) {
	if (!sprite) logf("Drawing null sprite\n");
	genDrawSprite(sprite, transforms, transformsNum, recurse, &skiaSys->immVDrawCommandsList);
}

void drawShape(SwfShape *shape, Matrix3 matrix, DrawShapeProps props) {
	pushSpriteMatrix(&skiaSys->immVDrawCommandsList, matrix);
	genDrawShape(shape, props, &skiaSys->immVDrawCommandsList);
	popSpriteMatrix(&skiaSys->immVDrawCommandsList);
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
	// memset(cmd, 0, sizeof(VDrawCommand));
	cmd->type = type;
	cmd->gradientRatiosNum = 0;
	return cmd;
}

void pushSpriteMatrix(VDrawCommandsList *cmdList, Matrix3 mat) {
	if (skiaSys->matrixStackNum > MATRIX_STACK_MAX-1) {
		logf("Too many matrix stacks!\n");
		return;
	}

	Matrix3 newMatrix = skiaSys->matrixStack[skiaSys->matrixStackNum-1] * mat;
	skiaSys->matrixStack[skiaSys->matrixStackNum++] = newMatrix;

	VDrawCommand *cmd = createCommand(cmdList, VDRAW_SET_MATRIX);
	cmd->matrix = newMatrix;
}
void popSpriteMatrix(VDrawCommandsList *cmdList) {
	skiaSys->matrixStackNum--;
	Matrix3 newMatix = skiaSys->matrixStack[skiaSys->matrixStackNum-1];

	VDrawCommand *cmd = createCommand(cmdList, VDRAW_SET_MATRIX);
	cmd->matrix = newMatix;
}

DrawShapeProps newDrawShapeProps() {
	DrawShapeProps props = {};
	props.alpha = 1;
	props.colorTransform = makeColorTransform();
	return props;
}

void genDrawShape(SwfShape *shape, DrawShapeProps props, VDrawCommandsList *cmdList) {
	for (int i = 0; i < shape->subShapesNum; i++) {
		SwfSubShape *subShape = &shape->subShapes[i];
		// if (subShape->drawEdgesNum == 0) continue;
		createCommand(cmdList, VDRAW_START_SHAPE);

		{ /// Set line/fill style
			if (subShape->fillStyleIndex && subShape->lineStyleIndex) logf("nononono\n");

			VDrawCommand *paintCmd = NULL;
			if (subShape->fillStyleIndex != 0) {
				FillStyle *newFillStyle = &shape->fillStyles[subShape->fillStyleIndex - 1];
				// if (newFillStyle->gradient.spreadMode != 0) logf("Bad spread mode %d\n", newFillStyle->gradient.spreadMode);
				// if (newFillStyle->gradient.interpolationMode != 0) logf("Bad introplation mode %d\n", newFillStyle->gradient.interpolationMode);

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
					paintCmd->matrix = newFillStyle->matrix;

					float lastRatio = -1;
					for (int i = 0; i < newFillStyle->gradient.coordsNum; i++) {
						GradeCord *coord = &newFillStyle->gradient.coords[i];
						paintCmd->colors[paintCmd->gradientRatiosNum] = applyColorTransform(coord->color, props.colorTransform);

						float ratio = coord->ratio / 255.0;
						if (ratio == lastRatio) continue;
						lastRatio = ratio;

						paintCmd->gradientRatios[paintCmd->gradientRatiosNum] = ratio;
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
					paintCmd->matrix = newFillStyle->matrix;
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

				paintCmd->startCapStyle = (CapStyle)newLineStyle->startCapStyle;
				paintCmd->joinStyle = (JoinStyle)newLineStyle->joinStyle;
				paintCmd->miterLimitFactor = newLineStyle->miterLimitFactor;
			}

			if (paintCmd) {
				for (int i = 0; i < 16; i++) {
#if 1
					int a = (float)((paintCmd->colors[i] >> 24) & 0xFF) * props.alpha;
					paintCmd->colors[i] = (paintCmd->colors[i] & 0x00FFFFFF) | (a << 24);
#else
					int a, r, g, b;
					hexToArgb(paintCmd->colors[i], &a, &r, &g, &b);
					a = ((a/255.0) * Clamp01(props.alpha)) * 255.0;
					paintCmd->colors[i] = argbToHex(a, r, g, b);
#endif
				}
			}
		} ///

		VDrawCommand *cmd = createCommand(cmdList, props.useClip ? VDRAW_CLIP_CACHED_PATH : VDRAW_DRAW_CACHED_PATH);
		cmd->path = subShape->runtimeCachedPath;

		createCommand(cmdList, VDRAW_END_SHAPE);
	}
}

void genDrawSprite(SwfSprite *sprite, SpriteTransform *transforms, int transformsNum, DrawSpriteRecurseData recurse, VDrawCommandsList *cmdList, bool isNested) {
	if (streq(sprite->name, "__VDRAW_CMD_LIST__")) {
		VDrawCommandsList *newList = (VDrawCommandsList *)sprite->controlTags;
		for (int i = 0; i < newList->cmdsNum; i++) {
			VDrawCommand *newCmd = createCommand(cmdList, VDRAW_SET_MATRIX);
			*newCmd = newList->cmds[i];
		}
		return;
	}

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

		Allocator *allocator = (Allocator *)frameMalloc(sizeof(Allocator));
		allocator->type = ALLOCATOR_FRAME;
		recurse.nameTransformMap = createHashMap(sizeof(char *), sizeof(SpriteTransform *), 1024, allocator);
		recurse.nameTransformMap->usesStreq = true;
		for (int i = 0; i < transformsNum; i++) {
			SpriteTransform *transform = &transforms[i];
			for (int i = 0; i < transform->pathsNum; i++) {
				char *path = transform->paths[i];
				hashMapSet(recurse.nameTransformMap, &path, (int)stringHash32(path), &transform);
			}
		}

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
		recurse.path = frameSprintf(
			"%s%s%s", 
			recurse.path,
			strlen(recurse.path) != 0 ? "." : "",
			name
		);

		hashMapGet(recurse.nameTransformMap, &recurse.path, (int)stringHash32(recurse.path), &matchingTransform);
	}

	int frameIndex = 0;
	if (matchingTransform) {
		frameIndex = matchingTransform->frame;
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
	}
	Font *font = recurse.font;
	char *text = recurse.text;

	bool canDraw = true;
	if (!sprite->isTextField) {
		if (frameIndex > sprite->framesNum-1) frameIndex = sprite->framesNum-1;
		if (frameIndex < 0) frameIndex = 0;
	}

	if (matchingTransform) matchingTransform->outPreTransformMatrix = skiaSys->matrixStack[skiaSys->matrixStackNum-1];

	pushSpriteMatrix(cmdList, localMatrix);

	if (matchingTransform) matchingTransform->outMatrix = skiaSys->matrixStack[skiaSys->matrixStackNum-1];

	if (canDraw) {
		if (sprite->isTextField) {
			if (!font) font = getFontInternal(ARIAL_FONT, 24);
			if (!text) text = sprite->initialText;
			Rect textRect = sprite->bounds;

			if (text[0] != 0) {
				VDrawCommand *cmd = createCommand(cmdList, VDRAW_TEXT);
				cmd->font = font;
				cmd->text = text;
				cmd->position = getPosition(textRect);
				cmd->control = getSize(textRect);
				cmd->colors[0] = setAofArgb(sprite->textColor, 255*Clamp01(recurse.alpha));

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
		} else {
			SwfFrame *frame = &sprite->frames[frameIndex];
			int clippingTill = 0;
			char *currentLayerName = NULL;
			for (int i = 0; i < frame->depthsNum; i++) {
				SwfDrawable *drawable = &frame->depths[i];

				if (clippingTill != 0 && drawable->depth >= clippingTill) {
					clippingTill = 0;
					createCommand(cmdList, VDRAW_RESTORE);
				}

				float alphaMultiplier = 1;
				if (currentLayerName && matchingTransform) {
					for (int i = 0; i < matchingTransform->layerPropsNum; i++) {
						SpriteLayerProps *layerProps = &matchingTransform->layerProps[i];
						if (streq(currentLayerName, layerProps->name)) {
							alphaMultiplier *= layerProps->alpha;
						}
					}
				}

				if (drawable->name && stringStartsWith(drawable->name, "LAYERSTART_")) {
					currentLayerName = frameStringClone(drawable->name + strlen("LAYERSTART_"));
				}
				if (drawable->name && stringStartsWith(drawable->name, "LAYEREND_")) {
					char *layerToEnd = drawable->name + strlen("LAYEREND_");
					if (!streq(layerToEnd, currentLayerName)) {
						logf("Tried to end layer %s, but we're in layer %s (in %s)\n", layerToEnd, currentLayerName, recurse.path);
					}
					currentLayerName = NULL;
				}

				if (drawable->type == SWF_DRAWABLE_NONE) continue;
				if (drawable->type == SWF_DRAWABLE_SPRITE && drawable->sprite->name && stringStartsWith(drawable->sprite->name, "Invis_")) continue; //@incomplete @speed

				bool nextUseClip = recurse.useClip;

				if (drawable->clipDepth != 0) {
					if (clippingTill > 0) {
						logf("Trying to clip, but already clipping %d\n", clippingTill);
						createCommand(cmdList, VDRAW_RESTORE);
					}
					clippingTill = drawable->clipDepth;
					nextUseClip = true;
					createCommand(cmdList, VDRAW_SAVE);
				}

				pushSpriteMatrix(cmdList, mat3(drawable->matrix));
				if (drawable->type == SWF_DRAWABLE_SHAPE) {
					DrawShapeProps props = {};
					props.useClip = nextUseClip;
					props.alpha = Clamp01(recurse.alpha * alphaMultiplier);
					props.tint = recurse.tint;
					props.colorTransform = recurse.colorTransform;

					if (props.alpha > 0) genDrawShape(drawable->shape, props, cmdList);
				} else if (drawable->type == SWF_DRAWABLE_SPRITE) {
					DrawSpriteRecurseData newRecurse = recurse;
					newRecurse.useClip = nextUseClip;
					newRecurse.altName = drawable->name;
					newRecurse.alpha *= alphaMultiplier;
					newRecurse.alpha = Clamp01(newRecurse.alpha);
					if (drawable->colorTransform) newRecurse.colorTransform = applyColorTransform(newRecurse.colorTransform, *drawable->colorTransform);

					bool shouldStopBlur = false;
					bool shouldStopColorMatrix = false;
					for (int i = 0; i < drawable->filtersNum; i++) {
						SwfFilter *filter = &drawable->filters[i];
						if (filter->type == SWF_FILTER_BLUR) {
							if (!skiaSys->blurDisabled) {
								VDrawCommand *cmd = createCommand(cmdList, VDRAW_START_BLUR);
								cmd->position = v2(filter->blurFilter.blurX, filter->blurFilter.blurY);
								shouldStopBlur = true;
							}
						} else if (filter->type == SWF_FILTER_COLOR_MATRIX) {
							VDrawCommand *cmd = createCommand(cmdList, VDRAW_START_COLOR_MATRIX);
							memcpy(cmd->colors, filter->colorMatrixFilter.matrix, sizeof(float) * 20);
							shouldStopColorMatrix = true;
						}
					}

					bool shouldEndBlendMode = false;
					if (drawable->spriteBlendMode != SWF_BLEND_NONE && !nextUseClip) {
						VDrawCommand *cmd = createCommand(cmdList, VDRAW_SET_BLEND_MODE);
						cmd->blendMode = (SwfBlendMode)drawable->spriteBlendMode;
						shouldEndBlendMode = true;
					}

					genDrawSprite(drawable->sprite, transforms, transformsNum, newRecurse, cmdList);

					if (shouldEndBlendMode) createCommand(cmdList, VDRAW_END_BLEND_MODE);
					if (shouldStopBlur) createCommand(cmdList, VDRAW_END_BLUR);
					if (shouldStopColorMatrix) createCommand(cmdList, VDRAW_END_COLOR_MATRIX);
				} else {
					logf("Bad place object character\n");
				}
				popSpriteMatrix(cmdList);
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
			if (
				cmd->type != VDRAW_SET_SOLID_FILL &&
				cmd->type != VDRAW_SET_LINE_STYLE &&
				cmd->type != VDRAW_SET_LINEAR_GRADIENT_FILL && 
				cmd->type != VDRAW_SET_RADIAL_GRADIENT_FILL && 
				cmd->type != VDRAW_SET_FOCAL_GRADIENT_FILL
			) continue;

			for (int i = 0; i < recurse.swapsNum; i++) {
				VDrawPaletteSwap *swap = &recurse.swaps[i];

				int colorsToCheck = 16;

				//@speed You can only do colors[0] if it's a non-gradient, but lines can have gradients?
				if (cmd->type == VDRAW_SET_SOLID_FILL || cmd->type == VDRAW_SET_LINE_STYLE) colorsToCheck = 1;

				for (int i = 0; i < colorsToCheck; i++) {
					u8 alphaByte = getAofArgb(cmd->colors[i]);
					if ((cmd->colors[i] & 0x00FFFFFF) == (swap->from & 0x00FFFFFF)) {
						cmd->colors[i] = setAofArgb(swap->to, alphaByte);
					}
				}
			}
		}
	}

	auto copyCmdList = [](VDrawCommandsList *cmdList) {
		char *wholeStr = "";

		for (int i = 0; i < cmdList->cmdsNum; i++) {
			VDrawCommand *cmd = &cmdList->cmds[i];
			char *str = frameSprintf("%d: %s\n", i, vDrawCommandTypeStrings[cmd->type]);
			if (cmd->type == VDRAW_SET_MATRIX) {
				str = frameSprintf(
					"%d: %s (%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f)\n",
					i,
					vDrawCommandTypeStrings[cmd->type],
					cmd->matrix.data[0], cmd->matrix.data[1], cmd->matrix.data[2],
					cmd->matrix.data[3], cmd->matrix.data[4], cmd->matrix.data[5],
					cmd->matrix.data[6], cmd->matrix.data[7], cmd->matrix.data[8]
				);
			} else if (cmd->type == VDRAW_START_COLOR_MATRIX) {
				str = frameSprintf(
					"%d: %s (%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f)\n",
					i,
					vDrawCommandTypeStrings[cmd->type],
					cmd->colors+0, cmd->colors+1, cmd->colors+2, cmd->colors+3, cmd->colors+4,
					cmd->colors+5, cmd->colors+6, cmd->colors+7, cmd->colors+8, cmd->colors+9,
					cmd->colors+10, cmd->colors+11, cmd->colors+12, cmd->colors+13, cmd->colors+14,
					cmd->colors+15, cmd->colors+16, cmd->colors+17, cmd->colors+18, cmd->colors+19
				);
			} else if (cmd->type == VDRAW_SET_BLEND_MODE) {
				str = frameSprintf("%d: %s (%d)\n", i, vDrawCommandTypeStrings[cmd->type], cmd->blendMode);
			} else if (cmd->type == VDRAW_SET_SOLID_FILL) {
				str = frameSprintf("%d: %s (0x%X)\n", i, vDrawCommandTypeStrings[cmd->type], cmd->colors[0]);
			} else if (cmd->type == VDRAW_SET_LINE_STYLE) {
				str = frameSprintf("%d: %s (0x%X, %.2f)\n", i, vDrawCommandTypeStrings[cmd->type], cmd->colors[0], cmd->width);
			} else if (cmd->type == VDRAW_DRAW_CACHED_PATH) {
				str = frameSprintf("%d: %s (%d)\n", i, vDrawCommandTypeStrings[cmd->type], cmd->path->countVerbs());
			}

			wholeStr = frameSprintf("%s%s", wholeStr, str);
		}

		setClipboard(wholeStr);
	};

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

void execCommands(VDrawCommandsList *cmdList) {
	if (skiaSys->width == 0) {
		logf("You have to call resize at least once to draw\n");
		return;
	}

	SkPaint paint = SkPaint();
	// paint.setAntiAlias(true);

	SkPath path = SkPath();

	for (int i = 0; i < cmdList->cmdsNum; i++) {
		VDrawCommand *cmd = &cmdList->cmds[i];
		VDrawCommand *nextCmd = NULL;
		if (i < cmdList->cmdsNum-1) nextCmd = &cmdList->cmds[i+1];
		// logf("Cmd %d\n", i);
		if (cmd->type == VDRAW_SET_MATRIX) {
			if (!nextCmd || nextCmd->type != VDRAW_SET_MATRIX) skiaSys->canvas->setMatrix(toSkMatrix(cmd->matrix));
		} else if (cmd->type == VDRAW_CLIP_PATH) {
			skiaSys->canvas->clipPath(path);
		} else if (cmd->type == VDRAW_DRAW_CACHED_PATH) {
			// logf("Drawing path: %p\n", cmd->path);
			// logf("path length: %d %d\n", cmd->path->countVerbs(), cmd->path->countPoints());
			skiaSys->canvas->drawPath(*cmd->path, paint);
		} else if (cmd->type == VDRAW_CLIP_CACHED_PATH) {
			skiaSys->canvas->clipPath(*cmd->path, true);
		} else if (cmd->type == VDRAW_SAVE) {
			skiaSys->canvas->save();
		} else if (cmd->type == VDRAW_RESTORE) {
			skiaSys->canvas->restore();
		} else if (cmd->type == VDRAW_START_SHAPE) {
		} else if (cmd->type == VDRAW_END_SHAPE) {
			paint.setShader(NULL);
		} else if (cmd->type == VDRAW_END_SPRITE) {
		} else if (cmd->type == VDRAW_START_BLUR) {
			if (skiaSys->useSaveLayerBlur) {
				SkPaint blurPaint = SkPaint();
				sk_sp<SkImageFilter> filter = SkImageFilters::Blur(cmd->position.x, cmd->position.y, SkTileMode::kDecal, NULL);
				blurPaint.setImageFilter(filter);
				skiaSys->canvas->saveLayer(NULL, &blurPaint);
			} else {
				sk_sp<SkImageFilter> filter = SkImageFilters::Blur(cmd->position.x, cmd->position.y, SkTileMode::kDecal, NULL);
				paint.setImageFilter(filter);
			}
		} else if (cmd->type == VDRAW_END_BLUR) {
			if (skiaSys->useSaveLayerBlur) {
				skiaSys->canvas->restore();
			} else {
				paint.setImageFilter(NULL);
			}
		} else if (cmd->type == VDRAW_START_COLOR_MATRIX) {
			float *colorMatrix = (float *)frameMalloc(sizeof(float) * 20);
			memcpy(colorMatrix, cmd->colors, sizeof(float) * 20);
			colorMatrix[4] /= 255.0;
			colorMatrix[9] /= 255.0;
			colorMatrix[14] /= 255.0;
			colorMatrix[19] /= 255.0;
			sk_sp<SkColorFilter> skiaColorFilter = SkColorFilters::Matrix(colorMatrix);
			paint.setColorFilter(skiaColorFilter);
		} else if (cmd->type == VDRAW_END_COLOR_MATRIX) {
			paint.setColorFilter(NULL);
		} else if (cmd->type == VDRAW_SET_BLEND_MODE) {
			static int _usingInvalidBlendModeWarnings = 0;

			SkBlendMode blendMode = SkBlendMode::kSrcOver;
			if (cmd->blendMode == SWF_BLEND_NORMAL || cmd->blendMode == SWF_BLEND_NONE || cmd->blendMode == 0) {
				blendMode = SkBlendMode::kSrcOver;
			} else if (cmd->blendMode == SWF_BLEND_MULTIPLY) {
				blendMode = SkBlendMode::kMultiply;
			} else if (cmd->blendMode == SWF_BLEND_SCREEN) {
				blendMode = SkBlendMode::kScreen;
			} else if (cmd->blendMode == SWF_BLEND_DIFFERENCE) {
				blendMode = SkBlendMode::kDifference;
			} else if (cmd->blendMode == SWF_BLEND_ADD) {
				blendMode = SkBlendMode::kPlus;
			} else if (cmd->blendMode == SWF_BLEND_SUBTRACT) {
				blendMode = SkBlendMode::kDifference;
			} else {
				if (_usingInvalidBlendModeWarnings < 8) logf("Using invalid blend mode\n");
				_usingInvalidBlendModeWarnings++;
			}
			if (_usingInvalidBlendModeWarnings == 8) logf("Too many blend mode warnings, stopping reports.\n");

			if (skiaSys->useSaveLayerBlendMode) {
				SkPaint blendModePaint = SkPaint();
				blendModePaint.setBlendMode(blendMode);
				skiaSys->canvas->saveLayer(NULL, &blendModePaint);
			} else {
				paint.setBlendMode(blendMode);
			}

		} else if (cmd->type == VDRAW_END_BLEND_MODE) {
			if (skiaSys->useSaveLayerBlendMode) {
				skiaSys->canvas->restore();
			} else {
				paint.setBlendMode(SkBlendMode::kSrcOver);
			}
		} else if (cmd->type == VDRAW_SET_SOLID_FILL) {
			paint.setStyle(SkPaint::Style::kFill_Style);
			paint.setColor(cmd->colors[0]);
		} else if (cmd->type == VDRAW_SET_LINEAR_GRADIENT_FILL) {
			paint.setStyle(SkPaint::Style::kFill_Style);
			paint.setColor(0xFFFFFFFF);

			SkMatrix mat = toSkMatrix(cmd->matrix);
			SkPoint points[2] = {SkPoint::Make(-16384/20, 0), SkPoint::Make(16384/20, 0)};
			paint.setShader(SkGradientShader::MakeLinear(points, (SkColor *)cmd->colors, cmd->gradientRatios, cmd->gradientRatiosNum, SkTileMode::kClamp, 0, &mat));
		} else if (cmd->type == VDRAW_SET_RADIAL_GRADIENT_FILL) {
			paint.setStyle(SkPaint::Style::kFill_Style);
			paint.setColor(0xFFFFFFFF);

			SkMatrix mat = toSkMatrix(cmd->matrix);
			paint.setShader(SkGradientShader::MakeRadial(SkPoint::Make(0, 0), 16384.0/20, (SkColor *)cmd->colors, cmd->gradientRatios, cmd->gradientRatiosNum, SkTileMode::kClamp, 0, &mat));

			// for (int i = 0; i < cmd->gradientRatiosNum; i++) {
			// 	logf("Grad %d: 0x%X (%f)\n", i, cmd->colors[i], cmd->gradientRatios[i]);
			// }
		} else if (cmd->type == VDRAW_SET_FOCAL_GRADIENT_FILL) {
			paint.setStyle(SkPaint::Style::kFill_Style);
			paint.setColor(0xFFFFFFFF);

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
			paint.setStyle(SkPaint::Style::kFill_Style);
			paint.setColor(0xFFFFFFFF);
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
#if 0
			SkPaint fontPaint = SkPaint();
			fontPaint.setStyle(SkPaint::Style::kFill_Style);
			fontPaint.setColor(cmd->colors[0]);

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
			// float offsetY = skiaBounds.height();
			float offsetY = cmd->control.y;

			Matrix3 matrix = mat3();
			matrix.TRANSLATE(cmd->position);
			matrix.TRANSLATE(0, offsetY);

			skiaSys->canvas->save();
			skiaSys->canvas->concat(toSkMatrix(matrix));

			skiaSys->canvas->drawTextBlob(skiaTextBlob, 0, 0, fontPaint);

			skiaSys->canvas->restore();
#else
			SkPaint fontPaint = SkPaint();
			fontPaint.setStyle(SkPaint::Style::kFill_Style);
			fontPaint.setColor(cmd->colors[0]);

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

			skiaFont->measureText(cmd->text, strlen(cmd->text), SkTextEncoding::kUTF8, &skiaBounds, &fontPaint);
			Vec2 actualTextSize;
			actualTextSize.x = skiaBounds.width();
			actualTextSize.y = skiaBounds.height();

			Rect toFit = makeRect(cmd->position, cmd->control);

			Rect textRect = getInnerRectOfAspect(toFit, actualTextSize, cmd->gravity);

			Vec2 scaleRatio = getSize(textRect) / actualTextSize;

			textRect.y += textRect.height/2 + actualTextSize.y/2*scaleRatio.y;
			Matrix3 matrix = mat3();
			matrix.TRANSLATE(getPosition(textRect));
			matrix.SCALE(scaleRatio);

			skiaSys->canvas->save();
			skiaSys->canvas->concat(toSkMatrix(matrix));

			skiaSys->canvas->drawTextBlob(skiaTextBlob, 0, 0, fontPaint);

			skiaSys->canvas->restore();

			if (skiaSys->debugDrawTextFieldRectsThisFrame) {
				SkPaint outlineRed = SkPaint();
				outlineRed.setStyle(SkPaint::Style::kStroke_Style);
				outlineRed.setColor(0xFFFF0000);
				outlineRed.setStrokeWidth(1);

				SkPaint outlineBlue = SkPaint();
				outlineBlue.setStyle(SkPaint::Style::kStroke_Style);
				outlineBlue.setColor(0xFF0000FF);
				outlineBlue.setStrokeWidth(1);

				SkPaint outlineGreen = SkPaint();
				outlineGreen.setStyle(SkPaint::Style::kStroke_Style);
				outlineGreen.setColor(0xFF00FF00);
				outlineGreen.setStrokeWidth(1);

				// skiaSys->canvas->drawRect(skiaBounds, outlineRed);
				// skiaSys->canvas->drawRect(SkRect::MakeXYWH(textRect.x, textRect.y, textRect.width, textRect.height), outlineBlue);
				skiaSys->canvas->drawRect(SkRect::MakeXYWH(toFit.x, toFit.y, toFit.width, toFit.height), outlineGreen);
			}
#endif
		} else {
			logf("Unknown VDrawCommandType %d\n", cmd->type);
		}
	}
}

void clearSkia(int color) {
	skiaSys->canvas->clear(color);
}

SkPictureRecorder recorder;
void startSkiaFrame() {
	processBatchDraws();
	if (skiaSys->matrixStackNum != 1) logf("Matrix stack mismatch\n");

	if (skiaSys->recordFrame) {
		skiaSys->canvas = recorder.beginRecording(skiaSys->width, skiaSys->height);
	}

	skiaSys->canvas->clear(0);
}

void endSkiaFrame() {
	processBatchDraws();
	skiaSys->grDirectContext->resetContext();

	execCommands(&skiaSys->immVDrawCommandsList);
	skiaSys->immVDrawCommandsList.cmdsNum = 0;

	// skiaSys->grDirectContext->flushAndSubmit();
	skiaSys->grDirectContext->flush();

	resetRenderContext();
	clearScissor(); // This should be a part of resetRenderContext

	if (skiaSys->recordFrame) {
		skiaSys->recordFrame = false;
		skiaSys->canvas = skiaSys->mainCanvas;
		sk_sp<SkPicture> picture = recorder.finishRecordingAsPicture();
		sk_sp<SkData> skiaData = picture->serialize();
		void *data = (void *)skiaData->bytes();
		int size = skiaData->size();
		writeFile("assets/recording.skp", data, size);
		logf("Wrote recording\n");
		return;
	}
}

void startSkiaSubFrame(float scale) {
	skiaSys->subFrameCachedScale = skiaSys->scale;
	skiaSys->scale = v2(scale, scale);
	startSkiaFrame();
}

void endSkiaSubFrame() {
	endSkiaFrame();
	skiaSys->scale = skiaSys->subFrameCachedScale;
}

Texture *getSkiaFrameAsTexture(Vec2 size) {
	Texture *texture = createTexture(size.x, size.y);
	drawSkiaFrameOnTexture(texture);
	return texture;
}

void deinitSkiaFont(Font *font) {
	if (!font->skiaUserData) return;
	SkFont *skiaFont = (SkFont *)font->skiaUserData;
	delete skiaFont;
}

void drawSkiaFrameOnTexture(Texture *texture, bool doClear) {
	pushTargetTexture(texture);
	if (doClear) clearRenderer();

	Matrix3 matrix = mat3();
	matrix.SCALE(skiaSys->outputSize);
	drawSimpleTexture(skiaSys->skiaTexture, matrix);

	popTargetTexture();
}

void initSpriteTransforms(SpriteTransform *transforms, int transformsNum) {
	memset(transforms, 0, sizeof(SpriteTransform) * transformsNum);

	for (int i = 0; i < transformsNum; i++) {
		SpriteTransform *transform = &transforms[i];
		transform->matrix = mat3();
		transform->outMatrix = mat3();
		transform->alpha = 1;
	}
}

SpriteTransform *getSpriteTransform(SpriteTransform *transforms, int *transformsNum, char *pathName) {
	SpriteTransform *transform = &transforms[*transformsNum];
	*transformsNum = *transformsNum + 1;

	transform->paths[transform->pathsNum++] = pathName;
	return transform;
};

SpriteTransform *getNewOrExistingSpriteTransform(SpriteTransform *transforms, int *transformsNum, char *pathName) {
	for (int i = 0; i < *transformsNum; i++) {
		SpriteTransform *trans = &transforms[i];
		for (int i = 0; i < trans->pathsNum; i++) {
			if (streq(trans->paths[i], pathName)) return trans;
		}
	}

	return getSpriteTransform(transforms, transformsNum, pathName);
}

void drawSwfAnalyzer(char *basePath, Vec2 screenSize) {
	struct SwfASys {
		char inputPath[PATH_MAX_LEN];
		Swf *swf;
		char spriteFilter[PATH_MAX_LEN];
		int selectedSprite;
		int selectedSpriteFrame;
		Vec2 spriteOffset;
		float spriteScale;
		Texture *spriteTexture;
		int selectedDrawable;

		int selectedShape;
		Vec2 shapeOffset;
		float shapeScale;
		Texture *shapeTexture;

		bool showUnnamedSprites;
		bool recordFrame;
	};

	static SwfASys *aSys = NULL;
	if (!aSys) {
		aSys = (SwfASys *)zalloc(sizeof(SwfASys));
		aSys->spriteTexture = createTexture(512, 512);
		aSys->spriteScale = 1;

		aSys->shapeTexture = createTexture(512, 512);
		aSys->shapeScale = 1;

		if (!basePath) basePath = "assets/swf/Shared.swf";
		strcpy(aSys->inputPath, basePath);
	}

	ImGui::InputText("Input swf", aSys->inputPath, PATH_MAX_LEN);
	ImGui::SameLine();
	if (ImGui::Button("Analyze")) {
		swfFreeDrawEdges = false;
		aSys->swf = loadSwf(aSys->inputPath);
	}
	if (!aSys->swf) {
		// if (ImGui::Button("Shared.swf")) strcpy(aSys->inputPath, "assets/swf/Shared.swf");
	}

	Swf *swf = aSys->swf;
	if (swf && swf->allSpritesNum > 0) {

		ImGui::BeginChild("spritesListChild", ImVec2(screenSize.x*0.15, screenSize.y*0.5), true, 0); 
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
				aSys->selectedSpriteFrame = 0;
				SwfDrawable *drawable = sprite->frames[0].depths;

				if (drawable->type == SWF_DRAWABLE_SHAPE) {
					for (int i = 0; i < aSys->swf->allShapesNum; i++) {
						SwfShape *shape = aSys->swf->allShapes[i];
						if (shape == drawable->shape) {
							aSys->selectedShape = i;
						}
					}
				}
			}
		}

		ImGui::EndChild();
		ImGui::SameLine();

		ImGui::BeginChild("spriteChild", ImVec2(screenSize.x*0.2, screenSize.y*0.5), true, 0); 
		SwfSprite *sprite = swf->allSprites[aSys->selectedSprite];

		ImGui::InputInt("Frame", &aSys->selectedSpriteFrame);
		aSys->selectedSpriteFrame = MathClamp(aSys->selectedSpriteFrame, 0, sprite->framesNum-1);

		SkPictureRecorder recorder;
		if (aSys->recordFrame) {
			SkCanvas *pictureCanvas = recorder.beginRecording(skiaSys->width, skiaSys->height);
			skiaSys->canvas = pictureCanvas;
		}

		int frame = aSys->selectedSpriteFrame;
		Rect rect = getFrameBounds(sprite, frame);
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

		{
		pushTargetTexture(aSys->spriteTexture);
		clearRenderer();
		Matrix3 matrix = mat3();
		matrix.SCALE(skiaSys->outputSize);
		drawSimpleTexture(skiaSys->skiaTexture, matrix);
		popTargetTexture();
		}

		if (aSys->recordFrame) {
			aSys->recordFrame = false;
			skiaSys->canvas = skiaSys->mainCanvas;
			sk_sp<SkPicture> picture = recorder.finishRecordingAsPicture();
			sk_sp<SkData> skiaData = picture->serialize();
			void *data = (void *)skiaData->bytes();
			int size = skiaData->size();
			writeFile("assets/recording.skp", data, size);
			logf("Wrote recording\n");
		}

		imGuiTexture(aSys->spriteTexture);
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
		Rect frameBounds = getFrameBounds(sprite, frame);
		size = getSize(frameBounds);
		ImGui::Text(
			"FrameBounds: %.1f %.1f %.1f %.1f (%.1fx%.1f)",
			frameBounds.x,
			frameBounds.y,
			frameBounds.width,
			frameBounds.height,
			size.x,
			size.y
		);

		if (ImGui::Button("Dump")) {
			Vec2 size = getSize(sprite->bounds);
			size.x = ceilf(size.x);
			size.y = ceilf(size.y);
			resetSkia(size, v2(1, 1));
			// game->skiaFlagsDirty = true;

			Texture *texture = createTexture(size.x, size.y);
			if (directoryExists("c:/bin/dump")) removeDirectory("c:/bin/dump");
			if (!createDirectory("c:/bin/dump")) logf("Failed to create dump dir\n");

			for (int i = 0; i < sprite->framesNum; i++) {
				Rect rect = getFrameBounds(sprite, i);

				startSkiaFrame();

				Matrix3 mat = mat3();
				SpriteTransform trans;
				initSpriteTransforms(&trans, 1);
				trans.paths[trans.pathsNum++] = "_";
				trans.matrix.TRANSLATE(-rect.x, -rect.y);
				trans.frame = i;
				drawSprite(sprite, &trans, 1);

				endSkiaFrame();

				{
					pushTargetTexture(texture);
					clearRenderer();
					Matrix3 matrix = mat3();
					matrix.SCALE(skiaSys->outputSize);
					drawSimpleTexture(skiaSys->skiaTexture, matrix);
					popTargetTexture();
				}

				u8 *bitmapData = getTextureData(texture, _F_TD_FLIP_Y);

				char *path = frameSprintf("C:/bin/dump/frame%d.png", i);
				if (!stbi_write_png(path, texture->width, texture->height, 4, bitmapData, texture->width*4)) logf("Failed to dump to %s!\n", path);
			}
			destroyTexture(texture);
		}

		ImGui::SameLine();
		// if (ImGui::Button("Record skp")) aSys->recordFrame = true;

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
			ImGui::BeginChild("shapesChild", ImVec2(screenSize.x*0.2, screenSize.y*0.5), true, 0); 
			SwfShape *shape = aSys->swf->allShapes[aSys->selectedShape];
#if 0
			for (int i = 0; i < shape->subShapesNum; i++) {
				SwfSubShape *subShape = &shape->subShapes[i];
				ImGui::Text("Fill style index: %d", subShape->fillStyleIndex);
				ImGui::Text("Line style index: %d", subShape->lineStyleIndex);
				// ImGui::Text("Edges: %d", subShape->drawEdgesNum);
				ImGui::Separator();
			}
#else

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

			{
				pushTargetTexture(aSys->shapeTexture);
				clearRenderer();
				Matrix3 matrix = mat3();
				matrix.SCALE(skiaSys->outputSize);
				drawSimpleTexture(skiaSys->skiaTexture, matrix);
				popTargetTexture();
			}

			imGuiTexture(aSys->shapeTexture);
			Rect bounds = toRect(shape->shapeBounds);
			ImGui::DragFloat2("shapeOffset", &aSys->shapeOffset.x);
			ImGui::DragFloat("shapeScale", &aSys->shapeScale, 0.01);
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
#endif
			ImGui::EndChild();
		}
	}
}
