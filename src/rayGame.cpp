struct Globals {
};

enum GameState {
	GAME_NONE,
	GAME_PLAY,
};
struct Game {
	Font *defaultFont;
	// Font *hugeFont;
	RenderTexture *gameTexture;

	Globals globals;
	bool inEditor;
	float timeScale;
	float time;
	Vec2 size;
	float sizeScale;
	Vec2 mouse;

	Vec2 screenOverlayOffset;
	Vec2 screenOverlaySize;

	GameState state;
	GameState prevState;
	GameState nextState;
	float stateTransition_t;
	float stateTime;

	/// Editor/debug
	bool debugShowFrameTimes;
};
Game *game = NULL;

void runGame();
void updateGame();
/// FUNCTIONS ^

void runGame() {
#if defined(_WIN32)
#if !defined(FALLOW_INTERNAL) // This needs to be a macro
	snprintf(projectAssetDir, PATH_MAX_LEN, "%s", exeDir);
#else
	// if (directoryExists("C:/Dropbox")) strcpy(projectAssetDir, "C:/Dropbox/ZooBound/zooBoundGameAssets");
	// if (directoryExists("O:/Dropbox")) strcpy(projectAssetDir, "O:/Dropbox/ZooBound/zooBoundGameAssets");
#endif
#endif

	initFileOperations();

	Vec2 res = v2(1600, 900);

	initPlatform(res.x, res.y, "minGame");
	platform->sleepWait = true;
	initAudio();
	initRenderer(res.x, res.y);
	initTextureSystem();
	initFonts();

	platformUpdateLoop(updateGame);
}

void updateGame() {
	if (!game) {
		game = (Game *)zalloc(sizeof(Game));
		game->defaultFont = createFont("assets/common/arial.ttf", 20);

		// RegMem(Globals, useFXAA);
		// loadStruct("Globals", "assets/info/globals.txt", &game->globals);

		game->timeScale = 1;
		game->size = v2(platform->windowWidth, platform->windowHeight);

		maximizeWindow();
	}

	Globals *globals = &game->globals;

	float elapsed = platform->elapsed * game->timeScale;
	float secondPhase = (sin(game->time*M_PI*2-M_PI*0.5)/2)+0.5;

	game->mouse = (platform->mouse - game->screenOverlayOffset) * (game->size/game->screenOverlaySize);

	ImGui::Begin("Debug", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::End();

	{ /// Resizing
		Vec2 ratio = v2(1600.0, 900.0);
		game->sizeScale = MinNum(platform->windowWidth/ratio.x, platform->windowHeight/ratio.y);
		Vec2 newSize = ratio * game->sizeScale;

		if (!equal(game->size, newSize)) {
			game->size = newSize;

			if (game->gameTexture) destroyTexture(game->gameTexture);
			game->gameTexture = NULL;

			game->screenOverlaySize = game->size;
			game->screenOverlayOffset.x = (float)platform->windowWidth/2 - game->size.x/2;
			game->screenOverlayOffset.y = (float)platform->windowHeight/2 - game->size.y/2;
		}
	}

	if (!game->gameTexture) game->gameTexture = createRenderTexture(game->size.x, game->size.y);
	pushTargetTexture(game->gameTexture);
	clearRenderer(0xFF101010);

	if (keyJustPressed(KEY_BACKTICK)) game->inEditor = !game->inEditor;
	// platform->disableGui = !game->inEditor;

	if (game->state != game->nextState) {
		game->stateTransition_t += 0.05;
		if (game->stateTransition_t >= 1) game->state = game->nextState;
	} else {
		game->stateTransition_t -= 0.05;
	}
	game->stateTransition_t = Clamp01(game->stateTransition_t);

	if (game->prevState != game->state) {
		game->prevState = game->state;
		game->stateTime = 0;
	}

	if (game->state == GAME_NONE) {
		if (game->stateTime == 0) {
			game->nextState = GAME_PLAY;
		}
	} else if (game->state == GAME_PLAY) {
		Matrix3 cameraMatrix = mat3();
		// cameraMatrix.SCALE(lerp(1, 1.5, secondPhase));
		pushCamera2d(cameraMatrix);

		if (game->stateTime == 0) logf("Started\n");

#if 1
		{
			Texture *texture = getTexture("assets/images/tiger.png");
			drawSimpleTexture(texture);
		}

		{
			Texture *texture = getTexture("assets/images/alpha25.png");
			Matrix3 matrix = mat3();
			matrix.TRANSLATE(100, 200);
			matrix.SCALE(getSize(texture));
			drawSimpleTexture(texture, matrix);
		}

		{
			Texture *texture = getTexture("assets/images/alpha50.png");
			Matrix3 matrix = mat3();
			matrix.TRANSLATE(200, 200);
			matrix.SCALE(getSize(texture));
			drawSimpleTexture(texture, matrix);
		}

		{
			Texture *texture = getTexture("assets/images/alpha75.png");
			Matrix3 matrix = mat3();
			matrix.TRANSLATE(300, 200);
			matrix.SCALE(getSize(texture));
			drawSimpleTexture(texture, matrix);
		}

		{
			Texture *texture = getTexture("assets/images/alpha100.png");
			Matrix3 matrix = mat3();
			matrix.TRANSLATE(100, 300);
			matrix.SCALE(getSize(texture));
			drawSimpleTexture(texture, matrix, v2(0, 0), v2(1, 1), 0.25);
		}

		{
			Texture *texture = getTexture("assets/images/alpha100.png");
			Matrix3 matrix = mat3();
			matrix.TRANSLATE(200, 300);
			matrix.SCALE(getSize(texture));
			drawSimpleTexture(texture, matrix, v2(0, 0), v2(1, 1), 0.5);
		}

		{
			Texture *texture = getTexture("assets/images/alpha100.png");
			Matrix3 matrix = mat3();
			matrix.TRANSLATE(300, 300);
			matrix.SCALE(getSize(texture));
			drawSimpleTexture(texture, matrix, v2(0, 0), v2(1, 1), 0.75);
		}

		Rect rect = makeRect(100, 400, 100, 100);
		drawRect(rect, argbToHex(255*0.25, 0, 0, 0));
		rect.x += 100;
		drawRect(rect, argbToHex(255*0.5, 0, 0, 0));
		rect.x += 100;
		drawRect(rect, argbToHex(255*0.75, 0, 0, 0));
#endif

#if 0
		if (game->stateTime == 0) {
			clearRenderer(0xFFFF0000);
			{
				Texture *texture = getTexture("assets/images/alpha75.png");
				Matrix3 matrix = mat3();
				matrix.TRANSLATE(300, 200);
				matrix.SCALE(getSize(texture));
				drawSimpleTexture(texture, matrix);
			}

			{
				Texture *texture = getTexture("assets/images/alpha75.png");

				RenderTexture *cloneTexture = createRenderTexture(texture->width, texture->height);
				pushTargetTexture(cloneTexture);
				clearRenderer();
				drawSimpleTexture(texture);
				popTargetTexture();

				Matrix3 matrix = mat3();
				matrix.TRANSLATE(400, 200);
				matrix.SCALE(getSize(cloneTexture));
				drawSimpleTexture(cloneTexture, matrix);
			}
		}
#endif

		drawRect(0, 0, 100, 100, 0xFFFF0000);

		drawCircle(v2(), 100, 0xFF0000FF);

		Texture *texture = getTexture("assets/images/wn.png");

		Matrix3 matrix = mat3();
		Matrix3 uvMatrix = mat3();
#if 0
		matrix.TRANSLATE(game->time*5, game->time*5);
		matrix.ROTATE(game->time*5);
#else
		matrix.TRANSLATE(50, 50);
#endif
		matrix.SCALE(getSize(texture));
		// uvMatrix.TRANSLATE(0, game->time*0.5);

#if 0
		drawSimpleTexture(texture, matrix);
#else
		RenderProps props = newRenderProps();
		props.matrix = matrix;
		props.uvMatrix = uvMatrix;
		props.srcWidth = props.srcHeight = 1;
		props.tint = 0x000000FF;
		// props.tint = lerpColor(0x000000FF, 0xFF0000FF, secondPhase);
		drawTexture(texture, props);
#endif

		{
			// DrawTextProps textProps = newDrawTextProps();
			// textProps.font = fontSys->defaultFont;
			// textProps.color = lerpColor(0x00FFFFFF, 0xFFFFFFFF, secondPhase);
			// textProps.position = v2(500, 500);
			// drawText("Hello", textProps);
		}

		popCamera2d();
	}

	game->stateTime += elapsed;

	drawRect(makeRect(v2(0, 0), game->size), lerpColor(0x00000000, 0xFF000000, game->stateTransition_t));
	popTargetTexture(); // game->gameTexture

	clearRenderer();

	{
		RenderTexture *texture = game->gameTexture;
		Matrix3 matrix = mat3();
		matrix.TRANSLATE(game->screenOverlayOffset);
		matrix.SCALE(game->screenOverlaySize);

		drawSimpleTexture(texture, matrix);
	}

	if (keyPressed(KEY_CTRL) && keyPressed(KEY_SHIFT) && keyJustPressed('F')) game->debugShowFrameTimes = !game->debugShowFrameTimes;
	if (game->debugShowFrameTimes) {
		char *str = frameSprintf("%.5fms", platform->frameTimeAvg);
		drawText(game->defaultFont, str, v2(300, 0), 0xFF808080);
	}

	guiDraw();
	drawOnScreenLog();

	game->time += elapsed;
}
