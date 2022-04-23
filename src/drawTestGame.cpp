struct Game {
	Font *defaultFont;
	Texture *gameTexture;

	Swf *swf;

	int debugShowDefinedShape;
	bool debugIterateDefinedShapes;
	bool debugStepAllShapes;

	int debugShowSprite;
	bool debugIterateSprites;
#define SPRITE_CLASS_NAME_MAX_LEN 64
	char debugSpriteClassName[SPRITE_CLASS_NAME_MAX_LEN];
	int debugSpriteFrame;
	Vec2 debugSpriteOffset;

	bool debugRecordNextSkiaFrame;
	bool debugRecordingSkiaFrame;
};
Game *game = NULL;

void runGame();
void updateGame();

/// FUNCTIONS ^

void runGame() {
#if defined(_WIN32) && !defined(FALLOW_INTERNAL) // This needs to be a macro
	HMODULE hModule = GetModuleHandleW(NULL);
	GetModuleFileNameA(hModule, exeDir, PATH_MAX_LEN);

	char *lastSlash = strrchr(exeDir, '\\');
	if (!lastSlash) Panic("No last slash found in exe path");
	*lastSlash = 0;
	snprintf(projectAssetDir, PATH_MAX_LEN, "%s", exeDir);
#endif

	initFileOperations();

	initPlatform(1280, 720, "Swf Skia");
	platform->sleepWait = false;
	initAudio();
	initRenderer(1280, 720);
	initTextureSystem();
	initFonts();

	platformUpdateLoop(updateGame);
}

void updateGame() {
	if (!game) {
		game = (Game *)zalloc(sizeof(Game));
		game->defaultFont = createFont("assets/fonts/arial.ttf", 24);
		game->gameTexture = createTexture(platform->windowWidth, platform->windowHeight);

		initSkia();

		// game->swf = loadSwf("assets/swf/SofiAnim.swf");
		// game->debugShowSprite = 213;
		// game->debugSpriteFrame = 250;

		// game->swf = loadSwf("assets/swf/Shared.swf");
		// game->debugShowSprite = 0;
		// game->debugSpriteFrame = 0;
		// strcpy(game->debugSpriteClassName, "ZaraPussy");

		// game->swf = loadSwf("assets/swf/test.swf");
		// game->debugShowSprite = 0;
		// game->debugSpriteFrame = 0;
		// strcpy(game->debugSpriteClassName, "BitmapFilled");

		// game->swf = loadSwf("assets/swf/ZaraAnim.swf");
		// game->debugShowSprite = 97;
		// game->debugSpriteFrame = 230;

		game->swf = loadSwf("assets/swf/ThereSheIs.swf");

		game->debugSpriteOffset = v2(300, 300);
	}

	pushTargetTexture(game->gameTexture);
	clearRenderer();
	// startFrame();
	// clearRenderer();
	// drawText(game->defaultFont, "Hi", v2(), 0xFFFFFFFF);
	Swf *swf = game->swf;

	SkPictureRecorder recorder;
	if (game->debugRecordNextSkiaFrame) {
		game->debugRecordNextSkiaFrame = false;
		game->debugRecordingSkiaFrame = true;
		skiaSys->canvas = recorder.beginRecording(platform->windowWidth, platform->windowHeight);
	}
	skiaSys->useAA = true;
	startSkiaFrame();

	ImGui::Begin("Debug", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("frame time: %02dms (%03.0ffps)\n", platform->frameTime, 1.0/(float)platform->frameTime * 1000.0);
	// if (ImGui::TreeNode("ImGui demo")) {
	// 	ImGui::ShowDemoWindow();
	// 	ImGui::TreePop();
	// }
	if (ImGui::Button("Record .skp")) game->debugRecordNextSkiaFrame = true;
	if (ImGui::TreeNode("allShapes")) {
		int maxRows = 4;
		int maxCols = 4;

		int stepSize = 1;
		if (game->debugStepAllShapes) stepSize = maxRows * maxCols;

		ImGui::InputInt("Shape", &game->debugShowDefinedShape, stepSize);
		ImGui::SameLine();
		ImGui::Text("(max %d)\n", swf->allShapesNum);
		ImGui::Checkbox("Iterate", &game->debugIterateDefinedShapes);
		ImGui::Checkbox("Step all shapes", &game->debugStepAllShapes);

		if (game->debugIterateDefinedShapes) game->debugShowDefinedShape += stepSize;
		if (game->debugShowDefinedShape < 0) game->debugShowDefinedShape = swf->allShapesNum-1;
		if (game->debugShowDefinedShape > swf->allShapesNum-1) game->debugShowDefinedShape = 0;
		int shapeOffset = 0;
		for (int col = 0; col < maxCols; col++) {
			for (int row = 0; row < maxRows; row++) {
				int nextShapeIndex = game->debugShowDefinedShape + (shapeOffset++);
				if (nextShapeIndex > swf->allShapesNum-1) continue;

				Rect rect;
				rect.width = (float)platform->windowWidth / (float)maxRows;
				rect.height = (float)platform->windowHeight / (float)maxCols;
				rect.x = row * rect.width;
				rect.y = col * rect.height;

				skiaSys->canvas->save();
				Vec2 center = rect.center();
				skiaSys->canvas->translate(center.x, center.y);

				SwfShape *shape = swf->allShapes[nextShapeIndex];
				drawShape(skiaSys->canvas, shape);
				skiaSys->canvas->restore();
			}
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("allSprites", ImGuiTreeNodeFlags_DefaultOpen)) {
		// if (game->debugShowSprite == 0) game->debugShowSprite = 6;
		ImGui::InputInt("Sprite", &game->debugShowSprite);
		ImGui::InputInt("Frame", &game->debugSpriteFrame);
		ImGui::DragFloat2("Offset", &game->debugSpriteOffset.x);
		ImGui::Checkbox("Iterate", &game->debugIterateSprites);
		ImGui::InputText("By class", game->debugSpriteClassName, SPRITE_CLASS_NAME_MAX_LEN);
		ImGui::SameLine();
		if (ImGui::Button("Find")) {
			for (int i = 0; i < swf->allSpritesNum; i++) {
				SwfSprite *sprite = swf->allSprites[i];
				if (streq(sprite->name, game->debugSpriteClassName)) {
					game->debugShowSprite = i;
					break;
				}
			}
		}

		if (game->debugIterateSprites) game->debugShowSprite++;
		if (game->debugShowSprite < 0) game->debugShowSprite = swf->allSpritesNum-1;
		if (game->debugShowSprite > swf->allSpritesNum-1) game->debugShowSprite = 0;

		SwfSprite *sprite = swf->allSprites[game->debugShowSprite];

		SpriteTransform *transforms = (SpriteTransform *)frameMalloc(sizeof(SpriteTransform) * 1);
		init(transforms, 1);
		int transformsNum = 0;

		SpriteTransform *trans = &transforms[transformsNum++];
		trans->paths[trans->pathsNum++] = "_";
		trans->frame = game->debugSpriteFrame;
		trans->matrix = mat3();
		// trans->matrix = trans->matrix.translate(platform->windowWidth/2, platform->windowHeight/2);
		trans->matrix = trans->matrix.translate(game->debugSpriteOffset);

		drawSprite(sprite, transforms, transformsNum, {}, true);

		// logf(
		// 	"AABB: %.2f %.2f %.2f %.2f (%.2f x %.2f)\n",
		// 	sprite->bounds.min.x,
		// 	sprite->bounds.min.y,
		// 	sprite->bounds.max.x,
		// 	sprite->bounds.max.y,
		// 	sprite->bounds.max.x - sprite->bounds.min.x,
		// 	sprite->bounds.max.y - sprite->bounds.min.y
		// );
		// sprite->bounds.print("Bounds");
		Rect rect;
		rect.x = sprite->bounds.min.x;
		rect.y = sprite->bounds.min.y;
		rect.width = sprite->bounds.max.x - sprite->bounds.min.x;
		rect.height = sprite->bounds.max.y - sprite->bounds.min.y;
		// rect.print("Rect");
		// trans->matrix.multiply(rect).print("Transed");
		drawRectOutline(trans->matrix.multiply(rect), 5, 0xFFFF0000);
		// drawRect(trans->matrix.multiply(rect), 0xFFFF0000);
		ImGui::TreePop();
	}

	ImGui::End();

	endSkiaFrame();

	if (game->debugRecordingSkiaFrame) {
		game->debugRecordingSkiaFrame = false;
		sk_sp<SkPicture> picture = recorder.finishRecordingAsPicture();
		sk_sp<SkData> picData = picture->serialize();
		writeFile("assets/trace.skp", (void *)picData->data(), picData->size());
		logf("Recorded\n");
		skiaSys->canvas = skiaSys->mainCanvas;
	}

	popTargetTexture();
	clearRenderer(0xFFFFFFFF);

	{
		Texture *skiaTexture = getSkiaTexture();
		RenderProps props = newRenderProps();

		props.matrix = props.matrix.scale(skiaTexture->width, skiaTexture->height);
		props.matrix = props.matrix.scale(1, -1);
		// props.matrix = props.matrix.scale(game->gameTexture->width/(float)skiaTexture->width, game->gameTexture->height/(float)skiaTexture->height);
		props.matrix = props.matrix.translate(0, -1);
		props.srcWidth = props.srcHeight = 1;
		drawTexture(skiaTexture, props);
	}

	{
		RenderProps props = newRenderProps();
		drawTexture(game->gameTexture, props);
	}

	drawOnScreenLog();
}
