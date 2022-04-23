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

		maximizeWindow();
	}

	Globals *globals = &game->globals;

	float elapsed = platform->elapsed * game->timeScale;
	float secondPhase = (sin(game->time*M_PI*2-M_PI*0.5)/2)+0.5;

	game->mouse = (platform->mouse - game->screenOverlayOffset) * (game->size/game->screenOverlaySize);

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
	} ///

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
		drawRect(0, 0, 100, 100, 0xFFFF0000);
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
		char *str = frameSprintf("%.1fms", platform->frameTimeAvg);
		drawText(game->defaultFont, str, v2(300, 0), 0xFF808080);
	}

	guiDraw();
	drawOnScreenLog();

	game->time += elapsed;
}
