struct Globals {
};

#define MAP_VERSION 1
enum ActorType {
	ACTOR_NONE=0,
	ACTOR_SPAWNER=1,
	ACTOR_PLAYER=2,
	ACTOR_TYPES_MAX,
};
char *actorTypeStrings[] = {
	"None",
	"Spawner",
	"Player",
};

struct Actor {
	ActorType type;
#define ACTOR_NAME_MAX_LEN 128
	char name[ACTOR_NAME_MAX_LEN];
	int id;
	Vec2 position;
	Vec2 scale;

	/// Unserialized
	bool markedForDeletion;
};

struct Map {
#define MAP_NAME_MAX_LEN 128
	char name[MAP_NAME_MAX_LEN];

#define ACTORS_MAX 512
	Actor actors[ACTORS_MAX];
	int actorsNum;
	int nextActorId;

	/// Unserialized
};

enum GameState {
	GAME_NONE,
	GAME_PLAY,
};
struct Game {
	Font *defaultFont;
	RenderTexture *gameTexture;
	RenderTexture *debugTexture;

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

#define MAPS_MAX 128
	Map maps[MAPS_MAX];
	int prevMap;
	int currentMap;
	int nextMap;
	float mapTransition_t;

	Vec2 cameraTarget;

	/// Editor/debug
	int selectedActor;
	bool draggingActor;
	Vec2 draggingActorOffset;
	int debugChangeMap;

	bool debugShowFrameTimes;
};
Game *game = NULL;

void runGame();
void updateGame();

Actor *createActor();
void deleteActor(Actor *actor);
Actor *getActor(int id);
Actor *getActor(char *name);
bool doesActorContain(Actor *actor, Vec2 point);

void saveMap(Map *map, int mapFileIndex);
void loadMap(Map *map, int mapFileIndex);
/// FUNCTIONS ^

void runGame() {
#if defined(_WIN32)
#if !defined(FALLOW_INTERNAL) // This needs to be a macro
	snprintf(projectAssetDir, PATH_MAX_LEN, "%s", exeDir);
#else
#endif
#endif

	initFileOperations();

	initPlatform(1600, 900, "Roller game");
	initAudio();
	initRenderer(1600, 900);
	initTextureSystem();
	initFonts();

	platformUpdateLoop(updateGame);
}

void updateGame() {
	if (!game) {
		logf("Game struct is %.2fmb btw\n", (float)sizeof(Game) / (float)Megabytes(1));

		game = (Game *)zalloc(sizeof(Game));
		game->defaultFont = createFont("assets/common/arial.ttf", 20);

		for (int i = 0; i < MAPS_MAX; i++) loadMap(&game->maps[i], i);

		game->timeScale = 1;
		game->nextMap = -1;

		maximizeWindow();
	}

	Globals *globals = &game->globals;

	float elapsed = platform->elapsed * game->timeScale;
	float secondPhase = (sin(game->time*M_PI*2-M_PI*0.5)/2)+0.5;

	game->mouse = (platform->mouse - game->screenOverlayOffset) * (game->size/game->screenOverlaySize);

	{ /// Resizing
		Vec2 ratio = v2(16, 9);
		game->sizeScale = MinNum(platform->windowWidth/ratio.x, platform->windowHeight/ratio.y);
		Vec2 newSize = ratio * game->sizeScale;

		if (!equal(game->size, newSize)) {
			game->size = newSize;

			if (game->gameTexture) destroyTexture(game->gameTexture);
			game->gameTexture = NULL;
			if (game->debugTexture) destroyTexture(game->debugTexture);
			game->debugTexture = NULL;

			game->screenOverlaySize = game->size;
			game->screenOverlayOffset.x = (float)platform->windowWidth/2 - game->size.x/2;
			game->screenOverlayOffset.y = (float)platform->windowHeight/2 - game->size.y/2;
		}
	} ///

	if (!game->gameTexture) game->gameTexture = createRenderTexture(game->size.x, game->size.y);
	if (!game->debugTexture) game->debugTexture = createRenderTexture(game->size.x, game->size.y);

	pushTargetTexture(game->debugTexture);
	clearRenderer();
	popTargetTexture();

	pushTargetTexture(game->gameTexture);
	clearRenderer(0xFF101010);

	if (game->state != game->nextState) {
		game->stateTransition_t += 0.05;
		if (game->stateTransition_t >= 1) {
			game->state = game->nextState;
		}
	} else {
		game->stateTransition_t -= 0.05;
	}
	game->stateTransition_t = Clamp01(game->stateTransition_t);

	if (game->prevState != game->state) {
		game->prevState = game->state;
		game->stateTime = 0;
	}

	if (keyJustPressed(KEY_BACKTICK)) game->inEditor = !game->inEditor;
	// platform->disableGui = !game->inEditor;

	if (game->state == GAME_NONE) {
		if (game->stateTime == 0) game->nextState = GAME_PLAY;
	} else if (game->state == GAME_PLAY) {
		Map *map = &game->maps[game->currentMap];

		float elapsed = platform->elapsed * game->timeScale;
		float secondPhase = (sin(game->time*M_PI*2-M_PI*0.5)/2)+0.5;

		if (game->prevMap != game->currentMap) {
			game->prevMap = game->currentMap;
		}

		Matrix3 cameraMatrix = mat3();
		pushCamera2d(cameraMatrix);

		game->mouse = cameraMatrix.invert() * game->mouse;

		if (game->inEditor) {
			ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_Once);
			ImGui::Begin("Editor", NULL, ImGuiWindowFlags_AlwaysAutoResize);
			if (ImGui::TreeNodeEx("Map", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::InputInt("Change map", &game->debugChangeMap);
				game->debugChangeMap = mathClamp(game->debugChangeMap, 0, MAPS_MAX-1);
				if (game->currentMap != game->debugChangeMap) {
					ImGui::SameLine();
					if (ImGui::Button("Go")) {
						game->nextMap = game->debugChangeMap;
					}
				}

				if (ImGui::Button("Save map")) saveMap(map, game->currentMap);
				ImGui::SameLine();
				if (ImGui::Button("Load map")) {
					loadMap(map, game->currentMap);
					game->nextMap = game->currentMap;
				}
				ImGui::InputText("Name", map->name, MAP_NAME_MAX_LEN);

				ImGui::TreePop();
			}

			ImGui::End();

			ImGui::SetNextWindowPos(ImVec2(platform->windowWidth, 0), ImGuiCond_Always, ImVec2(1, 0));
			ImGui::Begin("Actors", NULL, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::BeginChild("ActorListChild", ImVec2(400, 200));

			for (int i = 0; i < map->actorsNum; i++) {
				ImGui::PushID(i);
				Actor *actor = &map->actors[i];

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
				if (ImGui::ArrowButton("upButton", ImGuiDir_Up)) {
					if (i > 0) arraySwap(map->actors, map->actorsNum, sizeof(Actor), i, i-1);
				}
				ImGui::SameLine();
				if (ImGui::ArrowButton("downButton", ImGuiDir_Down)) {
					if (i < map->actorsNum-1) arraySwap(map->actors, map->actorsNum, sizeof(Actor), i, i+1);
				}
				ImGui::PopStyleVar();

				char *name = frameSprintf("%s###%d", actorTypeStrings[actor->type], i);
				ImGui::SameLine();
				if (ImGui::Selectable(name, game->selectedActor == actor->id, 0, ImVec2(320, 0))) {
					game->selectedActor = actor->id;
				}

				ImGui::PopID();
			}
			ImGui::EndChild();
			if (ImGui::Button("Create actor")) {
				Actor *actor = createActor();
				if (actor) {
					actor->position = game->cameraTarget;
					game->selectedActor = actor->id;
				}
			}

			ImGui::Separator();

			Actor *actor = getActor(game->selectedActor);
			if (actor) {
				if (ImGui::Button("Delete actor")) actor->markedForDeletion = true;
				ImGui::SameLine();
				if (ImGui::Button("Duplicate actor")) {
					Actor *newActor = createActor();
					if (newActor) {
						int id = newActor->id;
						*newActor = *actor;
						newActor->id = id;
						newActor->position += v2(10, 10);
						game->selectedActor = newActor->id;
					}
				}

				ImGui::Combo("Actor type", (int *)&actor->type, actorTypeStrings, ArrayLength(actorTypeStrings));
				ImGui::InputText("Name", actor->name, ACTOR_NAME_MAX_LEN);
				ImGui::SameLine();
				ImGui::Text("Id: %d", actor->id);
				ImGui::DragFloat2("Position", &actor->position.x);
				ImGui::DragFloat2("Scale", &actor->scale.x, 0.005);

				ImGui::Separator();
			}
			ImGui::End();
		}

		Actor *player = NULL;
		for (int i = 0; i < map->actorsNum; i++) {
			Actor *actor = &map->actors[i];
			if (actor->type == ACTOR_PLAYER) player = actor;
		}

		if (!player) {
			Vec2 spawnPosition = v2();
			player = createActor();
			player->type = ACTOR_PLAYER;
			player->position = spawnPosition;
		}

		/// Actors first iteration
		for (int i = 0; i < map->actorsNum; i++) {
			Actor *actor = &map->actors[i];
		}

		/// Update actors
		for (int i = 0; i < map->actorsNum; i++) {
			Actor *actor = &map->actors[i];
		}

		/// Draw actors
		for (int i = 0; i < map->actorsNum; i++) {
			Actor *actor = &map->actors[i];
		}

		if (game->inEditor) {
			drawCircle(game->mouse, 3, 0xFFFF0000);

			bool selectedSomething = false;

			for (int i = 0; i < map->actorsNum; i++) {
				Actor *actor = &map->actors[i];

				if (platform->mouseJustUp && !game->draggingActor && doesActorContain(actor, game->mouse)) {
					game->selectedActor = actor->id;
					selectedSomething = true;
				}
			}

			if (platform->mouseJustUp && !game->draggingActor && !selectedSomething) {
				game->selectedActor = 0;
			}

			Actor *actor = getActor(game->selectedActor);
			if (actor) {
				if (platform->mouseDown && doesActorContain(actor, game->mouse)) {
					if (!game->draggingActor) game->draggingActorOffset = game->mouse - actor->position;
					game->draggingActor = true;
				}

				if (!platform->mouseDown) game->draggingActor = false;

				if (game->draggingActor) {
					actor->position = game->mouse - game->draggingActorOffset;
				}
			}
		}

		/// Delete actors
		for (int i = 0; i < map->actorsNum; i++) {
			Actor *actor = &map->actors[i];
			if (actor->markedForDeletion) {
				deleteActor(actor);
			}
		}

		if (game->nextMap != -1) {
			game->mapTransition_t += 0.1;
		} else {
			game->mapTransition_t -= 0.1;
		}
		game->mapTransition_t = Clamp01(game->mapTransition_t);

		if (game->mapTransition_t >= 1) {
			game->currentMap = game->nextMap;

			game->nextMap = -1;
		}

		popCamera2d();


		drawRect(makeRect(v2(0, 0), game->size), lerpColor(0x00000000, 0xFF000000, game->mapTransition_t)); // Not far enough
	}

	game->stateTime += elapsed;

	drawRect(makeRect(v2(0, 0), game->size), lerpColor(0x00000000, 0xFF000000, game->stateTransition_t)); // Not far enough
	popTargetTexture(); // game->gameTexture

	clearRenderer();

	{
		Matrix3 matrix = mat3();
		matrix.TRANSLATE(game->screenOverlayOffset);
		matrix.SCALE(game->screenOverlaySize);

		drawSimpleTexture(game->gameTexture, matrix);
		drawSimpleTexture(game->debugTexture, matrix);
	}

	if (keyPressed(KEY_CTRL) && keyPressed(KEY_SHIFT) && keyJustPressed('F')) game->debugShowFrameTimes = !game->debugShowFrameTimes;
	if (game->debugShowFrameTimes) {
		char *str = frameSprintf("%.2fms", platform->frameTimeAvg);
		drawText(game->defaultFont, str, v2(300, 0), 0xFF808080);
	}

	guiDraw();
	drawOnScreenLog();

	game->time += elapsed;
}


Actor *createActor() {
	Map *map = &game->maps[game->currentMap];

	if (map->actorsNum > ACTORS_MAX-1) {
		logf("Out of actors!!!\n");
		return NULL;
	}

	Actor *actor = &map->actors[map->actorsNum++];
	memset(actor, 0, sizeof(Actor));
	actor->id = ++map->nextActorId;
	actor->scale = v2(1, 1);
	return actor;
}

void deleteActor(Actor *actor) {
	Map *map = &game->maps[game->currentMap];

	for (int i = 0; i < map->actorsNum; i++) {
		if (&map->actors[i] == actor) {
			arraySpliceIndex(map->actors, map->actorsNum, sizeof(Actor), i);
			map->actorsNum--;
			i--;
			return;
		}
	}

	logf("Couldn't delete actor, because it's not on this map...\n");
};

Actor *getActor(int id) {
	if (id == 0) return NULL;

	Map *map = &game->maps[game->currentMap];
	for (int i = 0; i < map->actorsNum; i++) {
		Actor *actor = &map->actors[i];
		if (actor->id == id) return actor;
	}
	return NULL;
}

Actor *getActor(char *name) {
	if (!name || !name[0]) return NULL;

	Map *map = &game->maps[game->currentMap];
	for (int i = 0; i < map->actorsNum; i++) {
		Actor *actor = &map->actors[i];
		if (streq(actor->name, name)) return actor;
	} 

	return NULL;
}

bool doesActorContain(Actor *actor, Vec2 point) {
	return false;
}

void saveMap(Map *map, int mapFileIndex) {
	DataStream *stream = newDataStream();

	writeU32(stream, MAP_VERSION);

	writeString(stream, map->name);

	int actorsToSave = 0;
	for (int i = 0; i < map->actorsNum; i++) {
		Actor *actor = &map->actors[i];
		bool willSave = true;
		if (actor->type == ACTOR_PLAYER) willSave = false;
		if (willSave) actorsToSave++;
	}

	writeU32(stream, actorsToSave);
	for (int i = 0; i < map->actorsNum; i++) {
		Actor *actor = &map->actors[i];
		bool willSave = true;
		if (actor->type == ACTOR_PLAYER) willSave = false;
		if (!willSave) continue;

		writeU32(stream, actor->type);
		writeString(stream, actor->name);
		writeU32(stream, actor->id);
		writeVec2(stream, actor->position);
		writeVec2(stream, actor->scale);
	}
	writeU32(stream, map->nextActorId);

	writeDataStream(frameSprintf("assets/maps/map%d.bin", mapFileIndex), stream);
	destroyDataStream(stream);
}

void loadMap(Map *map, int mapFileIndex) {
	memset(map, 0, sizeof(Map));

	DataStream *stream = loadDataStream(frameSprintf("assets/maps/map%d.bin", mapFileIndex));
	if (!stream) {
		logf("No map at index %d\n", mapFileIndex);
		return;
	}

	int version = readU32(stream);

	readStringInto(stream, map->name, MAP_NAME_MAX_LEN);

	map->actorsNum = readU32(stream);
	memset(map->actors, 0, sizeof(Actor) * ACTORS_MAX);
	for (int i = 0; i < map->actorsNum; i++) {
		Actor *actor = &map->actors[i];
		actor->type = (ActorType)readU32(stream);
		readStringInto(stream, actor->name, ACTOR_NAME_MAX_LEN);
		actor->id = readU32(stream);
		actor->position = readVec2(stream);
		actor->scale = readVec2(stream);
	}

	map->nextActorId = readU32(stream);

	destroyDataStream(stream);
}
