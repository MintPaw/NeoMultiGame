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

struct SoundInfo {
	char path[PATH_MAX_LEN];
	bool hardLoops;
	float volume;
};
enum SoundCategory {
	SOUND_CATEGORY_ALL=0,
	SOUND_CATEGORY_MUSIC=1,
	SOUND_CATEGORY_SFX=2,
	SOUND_CATEGORY_VOICES=3,
	SOUND_CATEGORY_UI=4,
	SOUND_CATEGORIES_MAX=5,
};
char *soundCategoryStrings[] = {
	"All",
	"Music",
	"SFX",
	"Voices",
	"UI",
};

struct Globals {

#define EMITTER_INFOS_MAX 32
	EmitterInfo emitterInfos[EMITTER_INFOS_MAX];
	int emitterInfosNum;

#define CUSTOM_STYLE_STACKS_MAX 64
	NguiNamedStyleStack customStyleStacks[CUSTOM_STYLE_STACKS_MAX];
	int customStyleStacksNum;

	float soundCategoryVolumes[SOUND_CATEGORIES_MAX];
#define SOUND_INFOS_MAX 64
	SoundInfo soundInfos[SOUND_INFOS_MAX];
	int soundInfosNum;

  EquationSet eqSet;
};

enum GameState {
	GAME_STATE_MENU,
	GAME_STATE_PLAY,
};
struct Game {
	Font *defaultFont;
  Font *dialogFont;

	Texture *gameTexture;
	Texture *overlayTexture;
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
  int musicChannel;

	/// Editor/debug
	bool inEditor;
	bool debugShowFrameTimes;

	Emitter *debugEmitter;
	EmitterInfo *debugTestingEmitterInfo;
	EmitterInfo debugCopiedEmitterInfo;
	bool debugShowNodeWindow;
};
Game *game = NULL;

void runGame();
void updateGame();
void stepGame(float elapsed, bool lastSubFrame);
SoundInfo *getSoundInfo(char *path);
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
	if (directoryExists("C:/Dropbox")) strcpy(projectAssetDir, "C:/Dropbox/MultiGame/multiGame/neoTestGameAssets");

  char *possibleDevPath = frameSprintf("%s/../runTree", exeDir);
	if (directoryExists(possibleDevPath)) strcpy(projectAssetDir, possibleDevPath);
#endif
#endif

	initFileOperations();

	Vec2 res = v2(1920, 1080);

	initPlatform(res.x, res.y, "neoMinGame");
	initAudio();
	initRenderer();
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

    checkEquationSystemInit();
    eqSys->eqSet = &game->globals.eqSet;

		// { /// Update sound infos
		// 	Globals *globals = &game->globals;

		// 	int pathsNum;
		// 	char **paths = getFrameDirectoryList("assets/audio", &pathsNum);

		// 	for (int i = 0; i < pathsNum; i++) {
		// 		char *path = paths[i];

		// 		bool shouldAdd = true;
		// 		for (int i = 0; i < globals->soundInfosNum; i++) {
		// 			if (streq(globals->soundInfos[i].path, path)) shouldAdd = false;
		// 		}

		// 		if (shouldAdd) {
		// 			if (globals->soundInfosNum < SOUND_INFOS_MAX-1) {
		// 				SoundInfo *info = &globals->soundInfos[globals->soundInfosNum++];
		// 				strcpy(info->path, path);
		// 				info->volume = 1;
		// 				logf("New SoundInfo created for %s\n", path);
		// 			} else {
		// 				logf("Too many sound infos!\n");
		// 			}
		// 		}
		// 	}

		// 	for (int i = 0; i < globals->soundInfosNum; i++) {
		// 		SoundInfo *info = &globals->soundInfos[i];

		// 		bool stillExists = false;
		// 		for (int i = 0; i < pathsNum; i++) {
		// 			char *path = paths[i];
		// 			if (streq(info->path, path)) {
		// 				stillExists = true;
		// 			}
		// 		}

		// 		if (!stillExists) {
		// 			logf("Sound %s is gone, deleting SoundInfo %s\n", info->path);
		// 			arraySpliceIndex(globals->soundInfos, globals->soundInfosNum, sizeof(SoundInfo), i);
		// 			globals->soundInfosNum--;
		// 			i--;
		// 			continue;
		// 		}
		// 	}
		// } ///

		maximizeWindow();
	}

  float timeScaleLeft = game->timeScale;
	if (keyPressed(MOUSE_RIGHT, true)) timeScaleLeft *= 5;
	if (keyPressed(MOUSE_RIGHT, true) && keyPressed(MOUSE_LEFT, true)) timeScaleLeft *= 2;
	if (keyPressed('Z')) timeScaleLeft *= 0.5;
	if (keyPressed('X')) timeScaleLeft *= 0.1;

  while (timeScaleLeft > 1) {
    stepGame(platform->elapsed, false);
    timeScaleLeft--;
  }

  stepGame(platform->elapsed * timeScaleLeft, true);
}

void stepGame(float elapsed, bool lastSubFrame) {
	Globals *globals = &game->globals;

  {
		Vec2 dst = v2(platform->windowWidth, platform->windowHeight);
		Vec2 src = game->size;
    game->sizeScale = MinNum(dst.x/src.x, dst.y/src.y);

		Vec2 newRealSize = src * game->sizeScale;
		Vec2 newScreenOverlayOffset = v2(platform->windowWidth, platform->windowHeight)/2 - newRealSize/2;
    if (!equal(game->realSize, newRealSize) || !equal(game->screenOverlayOffset, newScreenOverlayOffset)) {
      game->realSize = newRealSize;
      game->screenOverlayOffset = newScreenOverlayOffset;
			renderer->scissorScale = v2(game->sizeScale, game->sizeScale);

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

  if (!game->overlayTexture) game->overlayTexture = createTexture(game->realSize.x, game->realSize.y);
  pushTargetTexture(game->overlayTexture);
  clearRenderer();
  popTargetTexture();

  if (!game->gameTexture) game->gameTexture = createTexture(game->realSize.x, game->realSize.y);
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

	if (keyJustPressed(KEY_BACKTICK) || keyJustPressed(KEY_F1)) game->inEditor = !game->inEditor;
	// platform->disableGui = !game->inEditor;
	if (game->inEditor && lastSubFrame) {
		ImGui::Begin("Editor", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		if (ImGui::TreeNodeEx("Testing", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Globals")) {
			if (ImGui::Button("Save Globals")) saveLoadGlobals(true);
			ImGui::SameLine();
			if (ImGui::Button("Load Globals")) saveLoadGlobals(false);

			if (ImGui::TreeNode("Emitter infos")) {
				for (int i = 0; i < globals->emitterInfosNum; i++) {
					EmitterInfo *info = &globals->emitterInfos[i];
					ImGui::PushID(i);

					bool shouldSplice = false;
					guiPushStyleColor(ImGuiCol_Button, 0xFF900000);
					if (ImGui::Button("X")) shouldSplice = true;
					guiPopStyleColor();

					ImGui::SameLine();
					if (ImGui::Button("T")) {
						if (game->debugTestingEmitterInfo == info) {
							game->debugTestingEmitterInfo = NULL;
						} else {
							game->debugTestingEmitterInfo = info;
						}
					}

					ImGui::SameLine();
					char *isTestingStr = "";
					if (game->debugTestingEmitterInfo == info) isTestingStr = "[testing] ";
					if (ImGui::TreeNode(frameSprintf("%s%s###emitterInfo%d", isTestingStr, info->name, i))) {
						if (ImGui::Button("Copy EmitterInfo")) game->debugCopiedEmitterInfo = *info;
						ImGui::SameLine();
						if (ImGui::Button("Paste EmitterInfo")) *info = game->debugCopiedEmitterInfo;

						guiInputEmitterInfo(info);
						ImGui::TreePop();
					}

					if (shouldSplice) {
						if (game->debugTestingEmitterInfo == info) game->debugTestingEmitterInfo = NULL;

						arraySpliceIndex(globals->emitterInfos, globals->emitterInfosNum, sizeof(EmitterInfo), i);
						globals->emitterInfosNum--;
						i--;
					}

					ImGui::PopID();
				}

				if (ImGui::Button("Create emitter info")) {
					if (globals->emitterInfosNum > EMITTER_INFOS_MAX-1) {
						logf("Too many emitter infos!\n");
					} else {
						globals->emitterInfosNum++;
					}
				}
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Ngui Styles")) {
				nguiShowImGuiStylesEditor(globals->customStyleStacks, &globals->customStyleStacksNum, CUSTOM_STYLE_STACKS_MAX);
				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Sounds")) {
				ImGui::Text("Volumes:");
				for (int i = 0; i < SOUND_CATEGORIES_MAX; i++) {
					ImGui::SliderFloat(soundCategoryStrings[i], &globals->soundCategoryVolumes[i], 0, 1);
				}

				ImGui::Separator();
				ImGui::Text("Sound infos:");
				for (int i = 0; i < globals->soundInfosNum; i++) {
					SoundInfo *info = &globals->soundInfos[i];
					if (ImGui::TreeNode(info->path)) {
						ImGui::SliderFloat("Volume", &info->volume, 0, 2);
						ImGui::Checkbox("Hard loops", &info->hardLoops);
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		ImGui::End();
	}

	if (keyJustPressed(KEY_F2)) game->debugShowNodeWindow = !game->debugShowNodeWindow;
	if (keyJustPressed(KEY_F3)) {
    if (!game->debugShowNodeWindow) {
      game->debugShowNodeWindow = true;
    } else {
      if (fabs(eqSys->editorAlpha - 1) < 0.001) {
        eqSys->editorAlpha = 0.6;
      } else if (fabs(eqSys->editorAlpha - 0.6) < 0.001) {
        eqSys->editorAlpha = 0.3;
      } else if (fabs(eqSys->editorAlpha - 0.3) < 0.001) {
        eqSys->editorAlpha = 0.1;
      } else {
        eqSys->editorAlpha = 1;
      }
    }
  }

  updateEquationSystem(elapsed);

  if (game->debugShowNodeWindow && lastSubFrame) guiShowEquationNodeWindow();

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

	updateAndDrawHangingEmitters(elapsed);
  if (game->debugTestingEmitterInfo) {
    if (!game->debugEmitter) {
      game->debugEmitter = createEmitter();
      game->debugEmitter->position = game->size/2;
      game->debugEmitter->enabled = true;

      game->debugEmitter->rect = inflatePerc(makeRect(v2(), game->size), -0.75);
      game->debugEmitter->circleRadius = 100;
      game->debugEmitter->line.start = game->size*v2(0.25, 0.5);
      game->debugEmitter->line.end = game->size*v2(0.75, 0.5);
    }

    game->debugEmitter->info = *game->debugTestingEmitterInfo;
    updateEmitter(game->debugEmitter, elapsed);
    drawEmitter(game->debugEmitter);

    if (game->inEditor) guiInputEmitter(game->debugEmitter, elapsed);
  }

  drawRect(makeRect(v2(), game->size), lerpColor(0x00000000, 0xFF000000, game->stateChangePerc));

	{
		static SpriteSheet *_sheet;
		if (!_sheet) {
			_sheet = createSpriteSheet("assets/images/testCharacter");
		}
		SpriteSheetImage *image = getImage(_sheet, "bg.png");

		Rect rect = makeRect(0, 0, image->width, image->height);

		RenderProps props = newRenderProps();
		getUvs(_sheet, image, &props.uv0, &props.uv1);
		// translate(
		// 	*props.uvMatrix,
		// 	Vec2.{cast(float)image.srcX/cast(float)sheet.texture.width, cast(float)image.srcY/cast(float)sheet.texture.height}
		// );
		// scale(
		// 	*props.uvMatrix,
		// 	Vec2.{cast(float)image.srcWidth/cast(float)sheet.texture.width, cast(float)image.srcHeight/cast(float)sheet.texture.height}
		// );
		props.matrix.TRANSLATE(getPosition(rect));
		props.matrix.SCALE(getSize(rect));
		drawTexture(_sheet->texture, props);
		drawRectOutline(rect, 2, 0xFF0000FF);
	}


  game->stateTime += elapsed;
  game->time += elapsed;

	nguiDraw(elapsed);

  popCamera2d();
  popTargetTexture();

	clearRenderer();
	{
		Texture *texture = game->gameTexture;
		Matrix3 matrix = mat3();
		matrix.TRANSLATE(game->screenOverlayOffset);
		matrix.SCALE(texture->width, texture->height);

		drawSimpleTexture(texture, matrix);
		drawSimpleTexture(game->overlayTexture, matrix);
	}

	if (keyPressed(KEY_CTRL) && keyPressed(KEY_SHIFT) && keyJustPressed('F')) game->debugShowFrameTimes = !game->debugShowFrameTimes;
	if (game->debugShowFrameTimes) drawText(frameSprintf("%.1fms", platform->frameTimeAvg), newDrawTextProps(game->defaultFont, 0xFF808080, v2(300, 0)));

	if (lastSubFrame) imGuiDraw();
	drawOnScreenLog();

  { /// Update music
    float maxMusicVol = 0.2;
		SoundInfo *musicInfo = getSoundInfo(game->musicPath);
		if (musicInfo) maxMusicVol = musicInfo->volume;
		maxMusicVol *= globals->soundCategoryVolumes[SOUND_CATEGORY_ALL];
		maxMusicVol *= globals->soundCategoryVolumes[SOUND_CATEGORY_MUSIC];

    Channel *musicChannel = getChannel(game->musicChannel);

    if (game->prevMusicPath == game->musicPath) {
      if (musicChannel) {
        float musicTime = musicChannel->secondPosition;
        float timeMax = musicChannel->sound->length;
        float fadeTime = 1;

        float loopFadeVolPower = clampMap(musicTime, 0, fadeTime, 0, 1);
        loopFadeVolPower *= clampMap(musicTime, timeMax-fadeTime, timeMax, 1, 0);
				if (musicInfo && musicInfo->hardLoops) loopFadeVolPower = 1;
        maxMusicVol *= loopFadeVolPower;

        musicChannel->userVolume += elapsed / 0.5; // 0.5sec fade
        musicChannel->userVolume = mathClamp(musicChannel->userVolume, 0, maxMusicVol);
      }
    } else {
      if (musicChannel) {
        musicChannel->userVolume -= elapsed / 0.5; // 0.5sec fade
        if (musicChannel->userVolume <= 0) stopChannel(musicChannel);
      } else {
        musicChannel = playGameSound(game->musicPath, true);
        musicChannel->userVolume = 0;
        game->musicChannel = musicChannel->id;
        game->prevMusicPath = game->musicPath;
      }
    }
  } ///

	game->time += elapsed;
}

SoundInfo *getSoundInfo(char *path) {
	Globals *globals = &game->globals;

	for (int i = 0; i < globals->soundInfosNum; i++) {
		SoundInfo *info = &globals->soundInfos[i];
		if (streq(info->path, path)) {
			return info;
		}
	}

	return NULL;
}

Channel *playGameSound(char *path, bool loops) {
	Globals *globals = &game->globals;

	char *exactPath = resolveFuzzyPath(path);
	if (!exactPath) {
		logf("No sound at %s\n", path);
		return playSound(getSound("assets/common/audio/silence.ogg"));
	}

	Sound *sound = getSound(exactPath);
	Channel *channel = playSound(sound, loops);

	SoundInfo *info = getSoundInfo(exactPath);
	if (info) channel->userVolume *= info->volume;

	SoundCategory category = SOUND_CATEGORY_ALL;
	if (stringStartsWith(exactPath, "assets/audio/music")) category = SOUND_CATEGORY_MUSIC;
	else category = SOUND_CATEGORY_SFX;

	channel->userVolume *= globals->soundCategoryVolumes[SOUND_CATEGORY_ALL];
	if (category != SOUND_CATEGORY_ALL) channel->userVolume *= globals->soundCategoryVolumes[category];

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
	_referenceEmitterInfos = globals->emitterInfos;
	_referenceEmitterInfosNum = globals->emitterInfosNum;

	saveLoadInt(stream, save, version, &version, 0, 999);

  saveLoadInt(stream, save, version, &globals->emitterInfosNum, 0, 999);
  for (int i = 0; i < globals->emitterInfosNum; i++) {
    saveLoadEmitterInfo(stream, save, version, &globals->emitterInfos[i], 0, 999);
  }

	for (int i = 0; i < SOUND_CATEGORIES_MAX; i++) {
		saveLoadFloat(stream, save, version, &globals->soundCategoryVolumes[i], 0, 999);
	}

  saveLoadInt(stream, save, version, &globals->soundInfosNum, 0, 999);
	for (int i = 0; i < globals->soundInfosNum; i++) {
		SoundInfo *info = &globals->soundInfos[i];
		saveLoadStringInto(stream, save, version, info->path, PATH_MAX_LEN, 0, 999);
		saveLoadBool(stream, save, version, &info->hardLoops, 0, 999);
		saveLoadFloat(stream, save, version, &info->volume, 0, 999);
	}

  saveLoadEquationSet(stream, save, version, &globals->eqSet, 0, 999);

	saveLoadNamedStyleStacks(stream, save, version, globals->customStyleStacks, &globals->customStyleStacksNum, CUSTOM_STYLE_STACKS_MAX, 0, 999);

	if (save) writeDataStream(path, stream);
	destroyDataStream(stream);

	if (save) logf("Done.\n");
}
