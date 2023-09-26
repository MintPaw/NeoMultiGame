enum EventType {
	EVENT_NONE,
};
struct Event {
	EventType type;
};

enum EffectType {
	EFFECT_NONE,
};
struct Effect {
	EffectType type;
	float time;
};

struct Globals {

#define CUSTOM_STYLE_STACKS_MAX 64
	NguiNamedStyleStack customStyleStacks[CUSTOM_STYLE_STACKS_MAX];
	int customStyleStacksNum;
};

enum GameState {
	GAME_STATE_MENU,
	GAME_STATE_PLAY,
};
struct Game {
	Font *defaultFont;
  Font *dialogFont;

	RenderTexture *gameTexture;
	RenderTexture *overlayTexture;
  Vec2 screenOverlayOffset;

	Globals globals;
	float timeScale;
	float time;
	Vec2 size;
	Vec2 realSize;
	Vec2 mouse;
	float sizeScale;

	GameState state;
	float stateTime;

	GameState nextState;
  float stateChangePerc;

	float eventTime;
#define EVENTS_MAX 128
  Event events[EVENTS_MAX];
  int eventsNum;

#define EFFECTS_MAX 128
  Effect effects[EFFECTS_MAX];
  int effectsNum;

  char *prevMusicPath;
  char *musicPath;
  Channel *musicChannel;

	/// Editor/debug
	bool inEditor;
	bool debugShowFrameTimes;
};
Game *game = NULL;

void runGame();
void updateGame();
Channel *playGameSound(char *path, bool loops=false);
Effect *createEffect(EffectType type);
Event *createEvent(EventType type);

void saveLoadGlobals(bool save);
/// FUNCTIONS ^

void runGame() {
#if defined(_WIN32)
#if !defined(FALLOW_INTERNAL) // This needs to be a macro
	snprintf(projectAssetDir, PATH_MAX_LEN, "%s", exeDir);
#else
	if (directoryExists("C:/Dropbox")) strcpy(projectAssetDir, "C:/Dropbox/MultiGame/multiGame/neoMinGameAssets");

  char *possibleDevPath = frameSprintf("%s/../runTree", exeDir);
	if (directoryExists(possibleDevPath)) strcpy(projectAssetDir, possibleDevPath);
#endif
#endif

	initFileOperations();

	Vec2 res = v2(1920, 1080);

	initPlatform(res.x, res.y, "neoMinGame");
	platform->sleepWait = true;
	initAudio();
	initRenderer(res.x, res.y);
  renderer->defaultSetSrcWidthAndSrcHeightTo1 = true;
	initTextureSystem();
	initFonts();
	nguiInit();

	platformUpdateLoop(updateGame);
}

void updateGame() {
	if (!game) {
		game = (Game *)zalloc(sizeof(Game));
    game->size = v2(1920, 1080);
		game->timeScale = 1;

		saveLoadGlobals(false);

		maximizeWindow();
	}

	Globals *globals = &game->globals;

	float elapsed = platform->elapsed * game->timeScale;
  if (platform->rightMouseDown) {
    elapsed *= 5;
    if (platform->mouseDown) elapsed *= 2;
  }

  {
    int newWidth = platform->windowWidth;
    int newHeight = platform->windowHeight;
		Vec2 dst = v2(newWidth, newHeight);
		Vec2 src = game->size;
    game->sizeScale = MinNum(dst.x/src.x, dst.y/src.y);
		newWidth = src.x * game->sizeScale;
		newHeight = src.y * game->sizeScale;
    if (newWidth != game->realSize.x || newHeight != game->realSize.y) {
      game->realSize = v2(newWidth, newHeight);

      game->screenOverlayOffset.x = (float)platform->windowWidth/2 - game->realSize.x/2;
      game->screenOverlayOffset.y = (float)platform->windowHeight/2 - game->realSize.y/2;

      if (game->gameTexture) destroyTexture(game->gameTexture);
      game->gameTexture = NULL;

      if (game->overlayTexture) destroyTexture(game->overlayTexture);
      game->overlayTexture = NULL;

      if (game->defaultFont) destroyFont(game->defaultFont);
      game->defaultFont = createFont("assets/common/arial.ttf", (int)(game->size.y * 0.2));
    }
  }

	game->mouse = (platform->mouse - game->screenOverlayOffset) / game->sizeScale;

	ngui->mouse = game->mouse;
	ngui->uiScale = v2(1, 1);
	ngui->screenSize = game->realSize;

  if (!game->overlayTexture) game->overlayTexture = createRenderTexture(game->realSize.x, game->realSize.y);
  pushTargetTexture(game->overlayTexture);
  clearRenderer();
  popTargetTexture();

  if (!game->gameTexture) game->gameTexture = createRenderTexture(game->realSize.x, game->realSize.y);
  pushTargetTexture(game->gameTexture);
	clearRenderer();

  {
    Matrix3 mat = mat3();
    mat.SCALE(game->sizeScale);
    pushCamera2d(mat);
  }

  auto changeState = [](GameState nextState) { //@todo make function?
		if (game->nextState == nextState) return;
    game->nextState = nextState;
    game->stateChangePerc = 0;
  };

	if (keyJustPressed(KEY_BACKTICK)) game->inEditor = !game->inEditor;
	// platform->disableGui = !game->inEditor;
	if (game->inEditor) {
		ImGui::Begin("Editor", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    if (ImGui::TreeNodeEx("Testing", ImGuiTreeNodeFlags_DefaultOpen)) {
      ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Globals")) {
      if (ImGui::Button("Save Globals")) saveLoadGlobals(true);
      ImGui::SameLine();
      if (ImGui::Button("Load Globals")) saveLoadGlobals(false);

			if (ImGui::TreeNode("Ngui Styles")) {
				nguiShowImGuiStylesEditor(globals->customStyleStacks, &globals->customStyleStacksNum, CUSTOM_STYLE_STACKS_MAX);
				ImGui::TreePop();
			}

      ImGui::TreePop();
    }

		ImGui::End();
	}

  if (game->state != game->nextState) {
    if (game->stateChangePerc >= 1) {
      game->state = game->nextState;
      game->stateTime = 0;
    } else {
      game->stateChangePerc += 0.05;
    }
  } else {
    if (game->stateChangePerc > 0) game->stateChangePerc -= 0.05;
  }
  game->stateChangePerc = Clamp01(game->stateChangePerc);

  if (game->state == GAME_STATE_MENU) {
		changeState(GAME_STATE_PLAY);
  } else if (game->state == GAME_STATE_PLAY) {
		drawRect(0, 0, 100, 100, 0xFFFF0000);
	}

	{ /// Update effects
		for (int i = 0; i < game->effectsNum; i++) {
			Effect *effect = &game->effects[i];

			bool effectDone = false;
			if (effect->type == EFFECT_NONE) {
				if (effect->time == 0) {
					logf("None effect?\n");
				}

				float maxTime = 5;
				if (effect->time > maxTime) effectDone = true;
			}

			if (effectDone) {
				arraySpliceIndex(game->effects, game->effectsNum, sizeof(Effect), i);
				game->effectsNum--;
				i--;
				continue;
			}
			effect->time += elapsed;
		}
	} ///

    { /// Update events
			if (game->eventsNum) {
				Event *event = &game->events[0];

				bool eventDone = false;

				if (event->type == EVENT_NONE) {
					if (game->eventTime == 0) {
						logf("None event?\n");
					}

					float maxTime = 5;
					if (game->eventTime > maxTime) eventDone = true;
				}

				game->eventTime += elapsed;

				if (eventDone) {
					arraySpliceIndex(game->events, game->eventsNum, sizeof(Event), 0);
					game->eventsNum--;
					game->eventTime = 0;
				}
			}
    } ///

  drawRect(makeRect(v2(), game->size), lerpColor(0x00000000, 0xFF000000, game->stateChangePerc));

  game->stateTime += elapsed;
  game->time += elapsed;

	nguiDraw(elapsed);

  popCamera2d();
  popTargetTexture();

	clearRenderer();
	{
		RenderTexture *texture = game->gameTexture;
		Matrix3 matrix = mat3();
		matrix.TRANSLATE(game->screenOverlayOffset);
		matrix.SCALE(texture->width, texture->height);

		drawSimpleTexture(texture, matrix);
		drawSimpleTexture(game->overlayTexture, matrix);
	}

	if (keyPressed(KEY_CTRL) && keyPressed(KEY_SHIFT) && keyJustPressed('F')) game->debugShowFrameTimes = !game->debugShowFrameTimes;
	if (game->debugShowFrameTimes) drawText(game->defaultFont, frameSprintf("%.1fms", platform->frameTimeAvg), v2(300, 0), 0xFF808080);

	guiDraw();
	drawOnScreenLog();

  { /// Update music
    float maxMusicVol = 0.2;

    if (game->prevMusicPath == game->musicPath) {
    		if (game->musicChannel) {
    			float curSample = game->musicChannel->secondPosition;
    			float minSample = 0;
    			float timeMax = game->musicChannel->sound->length;
    			float fadeTime = 1;

					// Something is not right here I think
    			float volPower = clampMap(curSample, 0, fadeTime, 0, 1);
    			volPower *= clampMap(curSample, timeMax-fadeTime, timeMax, 1, 0);
    			maxMusicVol *= volPower;

    			game->musicChannel->userVolume += 0.01;
    			game->musicChannel->userVolume = mathClamp(game->musicChannel->userVolume, 0, maxMusicVol);
    		}
    } else {
    	if (game->musicChannel) {
    		game->musicChannel->userVolume -= 0.01;
    		if (game->musicChannel->userVolume <= 0) {
    			stopChannel(game->musicChannel);
    			game->musicChannel = NULL;
    		}
    	} else {
    		game->musicChannel = playGameSound(game->musicPath, true);
    		game->musicChannel->userVolume = 0;
    		game->prevMusicPath = game->musicPath;
    	}
    }
  } ///

	game->time += elapsed;
}

Channel *playGameSound(char *path, bool loops) {
	char *exactPath = resolveFuzzyPath(path);
	if (!exactPath) {
		logf("No sound at %s\n", path);
		return NULL;
	}

	Sound *sound = getSound(exactPath);
	Channel *channel = playSound(sound, loops);
	channel->userVolume = 0.2;
	return channel;
}

Effect *createEffect(EffectType type) {
  if (game->effectsNum > EFFECTS_MAX-1) {
    logf("Too many effects\n");
    game->effectsNum--;
  }

  Effect *effect = &game->effects[game->effectsNum++];
  memset(effect, 0, sizeof(Effect));
  effect->type = type;

  return effect;
}

Event *createEvent(EventType type) {
  if (game->eventsNum > EFFECTS_MAX-1) {
    logf("Too many events\n");
    game->eventsNum--;
  }

  Event *event = &game->events[game->eventsNum++];
  memset(event, 0, sizeof(Event));
  event->type = type;

  return event;
}

void saveLoadGlobals(bool save) {
	char *path = "assets/info/globals.bin";

	int version = 0;

	DataStream *stream;
	if (save) {
		stream = newDataStream();
	} else {
		stream = loadDataStream(path);
		if (!stream) return;
	}

	Globals *globals = &game->globals;

	saveLoadInt(stream, save, version, &version, 0, 999);

	saveLoadNamedStyleStacks(stream, save, version, globals->customStyleStacks, &globals->customStyleStacksNum, CUSTOM_STYLE_STACKS_MAX, 0, 999);

	if (save) writeDataStream(path, stream);
	destroyDataStream(stream);

	if (save) logf("Done.\n");
}
