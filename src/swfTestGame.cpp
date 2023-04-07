struct Game {
	RenderTexture *gameTexture;

	float time;
	Vec2 size;
	float sizeScale;
	Vec2 mouse;

	Vec2 screenOverlayOffset;
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
#endif
#endif

	initFileOperations();

	Vec2 res = v2(1600, 900);

	initPlatform(res.x, res.y, "SwfTestGame");
	platform->sleepWait = true;
	initRenderer(res.x, res.y);
	initTextureSystem();
	initFonts();

	platformUpdateLoop(updateGame);
}

void updateGame() {
	if (!game) {
		game = (Game *)zalloc(sizeof(Game));
		maximizeWindow();
	}

	float elapsed = platform->elapsed;
	game->mouse = (platform->mouse - game->screenOverlayOffset);

	{ /// Resizing
		Vec2 ratio = v2(1600.0, 900.0);
		game->sizeScale = MinNum(platform->windowWidth/ratio.x, platform->windowHeight/ratio.y);
		Vec2 newSize = ratio * game->sizeScale;

		if (!equal(game->size, newSize)) {
			game->size = newSize;

			if (game->gameTexture) destroyTexture(game->gameTexture);
			game->gameTexture = NULL;

			game->screenOverlayOffset.x = (float)platform->windowWidth/2 - game->size.x/2;
			game->screenOverlayOffset.y = (float)platform->windowHeight/2 - game->size.y/2;

			resetSkia(game->size, v2(1, 1), true, 16);
		}
	} ///

	if (!game->gameTexture) game->gameTexture = createRenderTexture(game->size.x, game->size.y);
	pushTargetTexture(game->gameTexture);
	clearRenderer(0xFF101010);
	popTargetTexture(); // game->gameTexture

	clearRenderer();

	ImGui::Begin("Swf Browser", NULL, ImGuiWindowFlags_AlwaysAutoResize);
	drawSwfAnalyzer("581168_Isaac_demo_Secure.swf", game->size);
	ImGui::End();

	{
		RenderTexture *texture = game->gameTexture;
		Matrix3 matrix = mat3();
		matrix.TRANSLATE(game->screenOverlayOffset);
		matrix.SCALE(game->size);

		drawSimpleTexture(texture, matrix);
	}

	guiDraw();
	drawOnScreenLog();

	game->time += elapsed;
}
