//Headers
#include "misc/danmaku.cpp"
#include "dynaMenu.cpp"
#include "dynaMap.cpp"
#include "dynaBattle.cpp"
#include "dynaEditor.cpp"

enum GameState {
	GS_NONE,
	GS_MENU,
	GS_MAP,
	GS_BATTLE,
	GS_EDITOR,
};

struct Globals {
	int testInt;
	MapData debugMapData;
};

struct Game {
	Globals globals;
	bool debugMode;

	GameState currentState;

	u32 frameCount;
	Vec2 prevMouse;
	int playersNum;

	// --- Map Data ----------
	MapPlayer mapPlayers[4];
	int selectedPoint;
	Vec2 cursorPos;
	int mouseDisableCooldown;
	int currentPlayer;
	// -----------------------

	// --- Battle Data -------
	Player player;
	Rect playArea;
	bool victory;
	float levelProgress;
	bool focusToggle;
	bool tempBool;
	// -----------------------

#define DANMAKU_GRAPHIC_MAX DANMAKU_GRAPHIC_END
	DanmakuGraphicInfo danmakuGraphicInfos[DANMAKU_GRAPHIC_MAX];

#define DANMAKU_MAX 8192
	Danmaku danmakuList[DANMAKU_MAX];
	int danmakuListNum;
	int nextDanmakuId;
};

Game *game = NULL;

void runGame();
void updateGame();
/// FUNCTIONS ^

#define __DYNA_IMPL
#include "misc/danmaku.cpp"
#include "dynaMenu.cpp"
#include "dynaMap.cpp"
#include "dynaBattle.cpp"
#include "dynaEditor.cpp"

void runGame() {
#if defined(_WIN32)
	HMODULE hModule = GetModuleHandleW(NULL);
	GetModuleFileNameA(hModule, exeDir, PATH_MAX_LEN);

	char *lastSlash = strrchr(exeDir, '\\');
	if (!lastSlash) Panic("No last slash found in exe path");
	*lastSlash = 0;

#if defined(FALLOW_INTERNAL) // This needs to be a macro
		// if (directoryExists("C:/Dropbox")) strcpy(projectAssetDir, "C:/Dropbox/???");
#else
		snprintf(projectAssetDir, PATH_MAX_LEN, "%s", exeDir);
#endif

#endif

	initFileOperations();

	initPlatform(1280, 720, "Dyna");
	platform->sleepWait = true;
	initAudio();
	initRenderer(1280, 720);
	// initMesh();
	// initModel();
	// initSkeleton();
	initFonts();
	initTextureSystem();
	// if (!initNetworking()) logf("Failed to init networking\n");

	platformUpdateLoop(updateGame);
}

void updateGame() {
	if (!game) {
		game = (Game *)zalloc(sizeof(Game));

		RegMem(Globals, testInt);
		RegMem(Globals, debugMapData);

		loadStruct("Globals", "assets/info/globals.txt", &game->globals);

		game->currentState = GS_MENU;

		danmakuInit();
		menuInit();
		mapInit();
		battleInit();
	}

	float elapsed = platform->elapsed;
	float secondPhase = timePhase(platform->time);
	clearRenderer();

	if (keyJustPressed(KEY_BACKTICK)) game->debugMode = !game->debugMode;

	if (game->debugMode) {
		ImGui::Begin("Globals", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		if (ImGui::Button("Save globals")) saveStruct("Globals", "assets/info/globals.txt", &game->globals);
		ImGui::SameLine();
		if (ImGui::Button("Load globals")) loadStruct("Globals", "assets/info/globals.txt", &game->globals);
		ImGui::InputInt("testInt", &game->globals.testInt);
		mapGlobals();
		ImGui::End();
	}

	switch(game->currentState) {
		case GS_NONE:   logf("Current Game State: None"); break;
		case GS_MENU:   runMenu();   break;
		case GS_MAP:    runMap();    break;
		case GS_BATTLE: runBattle(); break;
		case GS_EDITOR: runEditor(); break;
	}

	game->frameCount++;

	game->prevMouse = platform->mouse;

	drawOnScreenLog();
}
