#define DO_DRAW_AFTER_CONTEXT_SWITCH 1
#define USE_MSAA_RENDERBUFFER 1

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

struct SpriteLayerProps {
	char *name;
	float alpha;
};

struct DrawSpriteCall;
struct SpriteTransform {
#define SPRITE_TRANSFORM_PATHS_MAX 32
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

	Vec2 subFrameCachedScale;

	VDrawCommandsList immVDrawCommandsList;
};
SkiaSystem *skiaSys = NULL;

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
void startSkiaSubFrame(float scale);
void endSkiaSubFrame();

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

		skiaSys = (SkiaSystem *)zalloc(sizeof(SkiaSystem));
		skiaSys->matrixStack[skiaSys->matrixStackNum++] = mat3();
		skiaSys->superSampleScale = 2;

#if !defined(__EMSCRIPTEN__)
		skiaSys->blurEnabled = true;
#endif

		SkGraphics::Init();
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
	for (int i = 0; i < shape->subShapesNum; i++) {
		SwfSubShape *subShape = &shape->subShapes[i];
		if (subShape->drawEdgesNum == 0) continue;
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
					paintCmd->matrix = newFillStyle->gradientMatrix;

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
		} ///

		VDrawCommand *cmd = createCommand(cmdList, props.useClip ? VDRAW_CLIP_CACHED_PATH : VDRAW_DRAW_CACHED_PATH);
		cmd->path = (SkPath *)subShape->runtimeCachedPath;

		createCommand(cmdList, VDRAW_END_SHAPE);
	}
}

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
				if (streq(trans->paths[i], "*") || streq(trans->paths[i], recurse.path)) {
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
		} else {
			SwfDrawable *depths = sprite->frames[frame].depths;
			int depthsNum = sprite->frames[frame].depthsNum;
			int clippingTill = 0;
			char *currentLayerName = NULL;
			for (int i = 0; i < depthsNum; i++) {
				bool canSubDraw = true;

				SwfDrawable *drawable = &depths[i];
				if (drawable->type == SWF_DRAWABLE_NONE) canSubDraw = false;

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
						props.alpha = recurse.alpha * alphaMultiplier;
						props.tint = recurse.tint;
						props.colorTransform = recurse.colorTransform;

						if (props.alpha > 0) genDrawShape(drawable->shape, props, cmdList);
					} else if (drawable->type == SWF_DRAWABLE_SPRITE) {
						DrawSpriteRecurseData newRecurse = recurse;
						newRecurse.useClip = nextUseClip;
						newRecurse.altName = drawable->name;
						newRecurse.alpha *= alphaMultiplier;
						if (drawable->colorTransform) newRecurse.colorTransform = applyColorTransform(newRecurse.colorTransform, *drawable->colorTransform);

						bool shouldStopBlur = false;
						bool shouldStopColorMatrix = false;
						for (int i = 0; i < drawable->filtersNum; i++) {
							SwfFilter *filter = &drawable->filters[i];
							if (filter->type == SWF_FILTER_BLUR) {
								if (skiaSys->blurEnabled) {
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
			if (
				cmd->type != VDRAW_SET_SOLID_FILL &&
				cmd->type != VDRAW_SET_LINE_STYLE &&
				cmd->type != VDRAW_SET_LINEAR_GRADIENT_FILL && 
				cmd->type != VDRAW_SET_RADIAL_GRADIENT_FILL && 
				cmd->type != VDRAW_SET_FOCAL_GRADIENT_FILL
			) continue;

			for (int i = 0; i < recurse.swapsNum; i++) {
				VDrawPaletteSwap *swap = &recurse.swaps[i];
				for (int i = 0; i < 16; i++) { //@speed You can only do colors[0] if it's a non-gradient
					u8 alphaByte = getAofArgb(cmd->colors[i]);
					if ((cmd->colors[i] & 0x00FFFFFF) == (swap->from & 0x00FFFFFF)) {
						cmd->colors[i] = setAofArgb(swap->to, alphaByte);
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

int usingInvalidBlendModeWarnings = 0;
void execCommands(VDrawCommandsList *cmdList) {
	if (skiaSys->width == 0) {
		logf("You have to call resize at least once to draw\n");
		return;
	}

	SkPaint paint = SkPaint();
	if (!skiaSys->useGpu && skiaSys->useCpuAA) paint.setAntiAlias(true);

	SkPath path = SkPath();

	for (int i = 0; i < cmdList->cmdsNum; i++) {
		VDrawCommand *cmd = &cmdList->cmds[i];
		if (cmd->type == VDRAW_SET_MATRIX) {
			skiaSys->canvas->setMatrix(toSkMatrix(cmd->matrix));
		} else if (cmd->type == VDRAW_CLIP_PATH) {
			skiaSys->canvas->clipPath(path);
		} else if (cmd->type == VDRAW_DRAW_CACHED_PATH) {
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
			SkBlendMode blendMode = SkBlendMode::kSrcOver;
			if (cmd->blendMode == SWF_BLEND_NORMAL || cmd->blendMode == SWF_BLEND_NONE) {
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
				if (usingInvalidBlendModeWarnings < 8) logf("Using invalid blend mode\n");
				usingInvalidBlendModeWarnings++;
			}
			if (usingInvalidBlendModeWarnings == 8) logf("Too many blend mode warnings, stopping reports.\n");

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
			paint.setShader(SkGradientShader::MakeRadial( SkPoint::Make(0, 0), 16384.0/20, (SkColor *)cmd->colors, cmd->gradientRatios, cmd->gradientRatiosNum, SkTileMode::kClamp, 0, &mat));

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

			Rect textRect = getInnerRectOfAspect(toFit, actualTextSize, cmd->gravity);

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
}

void clearSkia(int color) {
	skiaSys->canvas->clear(color);
}

void startSkiaFrame() {
	if (skiaSys->matrixStackNum != 1) logf("Matrix stack mismatch\n");

	skiaSys->canvas->clear(0);
}

void endSkiaFrame() {
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

		pushTargetTexture(skiaSys->backTexture);
		clearRenderer();

		Matrix3 matrix = mat3();
		matrix.SCALE(getSize(skiaSys->backTexture));
		drawSimpleTexture(skiaSys->skiaTexture, matrix);
		popTargetTexture();

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
	RenderTexture *renderTexture = createRenderTexture(size.x, size.y);

	pushTargetTexture(renderTexture);
	drawSimpleTexture(skiaSys->backTexture);
	popTargetTexture();

	return renderTextureToTexture(renderTexture);
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
		bool recordFrame;
	};

	static SwfASys *aSys = NULL;
	if (!aSys) {
		aSys = (SwfASys *)zalloc(sizeof(SwfASys));
		aSys->spriteTexture = createRenderTexture(512, 512);
		aSys->spriteScale = 1;

		aSys->shapeTexture = createRenderTexture(512, 512);
		aSys->shapeScale = 1;
		strcpy(aSys->inputPath, "assets/swf/Shared.swf");
	}

	ImGui::InputText("Input swf", aSys->inputPath, PATH_MAX_LEN);
	ImGui::SameLine();
	if (ImGui::Button("Analyze")) aSys->swf = loadSwf(aSys->inputPath);
	if (!aSys->swf) {
		// if (ImGui::Button("Shared.swf")) strcpy(aSys->inputPath, "assets/swf/Shared.swf");
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

		SkPictureRecorder recorder;
		if (aSys->recordFrame) {
			SkCanvas *pictureCanvas = recorder.beginRecording(skiaSys->width, skiaSys->height);
			skiaSys->canvas = pictureCanvas;
		}


		int frame = aSys->selectedSpriteFrame;
		Rect rect = sprite->frameBounds[frame];
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

		ImGui::SameLine();
		if (ImGui::Button("Record skp")) aSys->recordFrame = true;

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
#if 1
			for (int i = 0; i < shape->subShapesNum; i++) {
				SwfSubShape *subShape = &shape->subShapes[i];
				ImGui::Text("Fill style index: %d", subShape->fillStyleIndex);
				ImGui::Text("Line style index: %d", subShape->lineStyleIndex);
				ImGui::Text("Edges: %d", subShape->drawEdgesNum);
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
#endif
			ImGui::EndChild();
		}
	}
}
