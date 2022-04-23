enum EntityType {
	ENTITY_NONE,
	ENTITY_PLAYER,
	ENTITY_STATIC_MODEL,
	ENTITY_WALL,
	ENTITY_CRYSTAL,
	ENTITY_EGG,
	ENTITY_LING,
	ENTITY_CRUMB,
	ENTITY_OMNI_SCREEN,
	ENTITY_WARP_ZONE,
};
char *entityTypeStrings[] = {
	"None",
	"Player",
	"Static model",
	"Wall",
	"Crystal",
	"Egg",
	"Ling",
	"Crumb",
	"Omni screen",
	"Warp zone",
};
struct Entity {
	EntityType type;
	int id;
	char path[PATH_MAX_LEN];
	float time;

	int hp;
	Vec3 position;
	Vec3 scale;
	float scaleMulti;
	Vec3 rotation;
	Matrix4 matrix;
	AABB localBounds;
	AABB globalBounds;

	bool isCollider;
	bool hasPhysics;
	bool unshootable;

	bool isOnGround;
	bool justWarped;

	Vec3 velo;
	Vec3 accel;

	int tint;

	bool markedForDestruction;

	int growth;
	// float maxHatchDistance;
	int maxEnemiesContained;
	bool broken;
	int enemiesToSpawn;
	float lingAccelSpeed;
};

struct Scene {
	int index;

	int ticksToProcess;
	int ticksTotal;
	int infectionTicks;
	int infectionTicksUsed;

	Entity *entities;
	int entitiesNum;
	int entitiesMax;
	int nextEntityId;

	Tile::Map *map;
	int *availTiles;
	int availTilesNum;
	Vec3 tileSize;
};

struct Warper {
	int entityId;
	int srcSceneIndex;
	int destSceneIndex;
	float time;
	float maxTime;
};

struct Globals {
};

struct Game {
	Framebuffer *gameFramebuffer;
	Texture *gameTexture;

	Framebuffer *omniScreenFramebuffer;
	Texture *omniScreenTexture;
	Vec2 omniScreenCursorUv;
	bool lookingAtOmniScreen;
	Font *omniScreenFont;
	int omniScreenSelectedScene;

	Globals globals;

	Vec3 cameraPosition;
	Vec3 cameraRotation;
	Matrix4 cameraMatrix;

	Vec3 playerRotation;
	float crystals;

#define SCENES_MAX 64
	Scene scenes[SCENES_MAX];
	int scenesNum;

#define WARPERS_MAX 64
	Warper warpers[WARPERS_MAX];
	int warpersNum;

	bool firstTickOfFrame;

	int currentSceneIndex;

	int selectedEntityId;
	bool debugMode;
	bool debugFreeCamera;
	float debugFreeCameraSpeed;
	int debugExtraTicksPerFrame;
};

Game *game = NULL;

void runGame();
void updateGame();
void updateScene(Scene *scene);
Vec3 getRayFromSceenSpace(Vec2 screenPosition, Vec3 cameraOffset);
Entity *createEntity(Scene *scene, EntityType type);
void destroyEntity(Scene *scene, Entity *entity);
Entity *getEntity(Scene *scene, int id);
Entity *getShotEntity(Vec3 rayStart, Vec3 rayDir, Vec3 *hitPos=NULL, bool excludePlayer=false);
void dealDamage(Entity *entity, int amount);
/// FUNCTIONS ^

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

	initPlatform(1280, 720, "A game?");
	platform->sleepWait = true;
	initAudio();
	initRenderer(1280, 720);
	initMesh();
	initModel();
	initSkeleton();
	initFonts();
	initTextureSystem();
	// if (!initNetworking()) logf("Failed to init networking\n");

	platformUpdateLoop(updateGame);
}

void updateGame() {
	if (!game) {
		game = (Game *)zalloc(sizeof(Game));

		// RegMem(Globals, );
		// loadStruct("Globals", "assets/info/globals.txt", &game->globals);

		game->debugFreeCameraSpeed = 0.5;
		game->cameraPosition = v3(0, -20, 0.5);
		game->cameraRotation.x = toRad(90);

		game->omniScreenFont = createFont("assets/common/arial.ttf", 42);
		game->omniScreenSelectedScene = -1;

#if 0
		Texture *canvas = createTexture(1000, 1000);

		pushTargetTexture(canvas);
		clearRenderer();

		for (int i = 0; i < 100000; i++) {
			Texture *note = getTexture("assets/eightNote.png");
			RenderProps props = newRenderProps();
			Vec2 off;
			off.x = rndFloat(-note->width, canvas->width);
			off.y = rndFloat(-note->height, canvas->height);
			props.matrix.TRANSLATE(off);
			props.matrix.SCALE(rndFloat(0.1, 1));
			props.tint = argbToHex(255, 0, 0, rndInt(0, 255));
			props.tint = lerpColor(props.tint, 0xFF00FF00, rndFloat(0.5, 1) * (off.x/canvas->width));
			props.tint = lerpColor(props.tint, 0xFFFF0000, rndFloat(0.5, 1) * (off.y/canvas->height));
			drawTexture(note, props);
		}

		popTargetTexture();

		Rect rect = makeRect(0, 0, canvas->width, canvas->height);
		u8 *data = getTextureData(canvas, _F_TD_FLIP_Y);

		char *filePath = "musicOutput.png";
		if (!stbi_write_png(filePath, rect.width, rect.height, 4, data, rect.width*4)) {
			logf("png is bad\n");
		}

		exit(1);
#endif
	}

	for (int i = 0; i < SCENES_MAX; i++) {
		Scene *scene = &game->scenes[i];
		scene->index = i;
		scene->ticksToProcess++;
		scene->ticksToProcess += game->debugExtraTicksPerFrame;
	}

	Scene *scene = &game->scenes[game->currentSceneIndex];
	Globals *globals = &game->globals;
	float elapsed = platform->elapsed;
	float secondPhase = timePhase(platform->time);

	clearRenderer();

	if (!game->gameFramebuffer) {
		game->gameFramebuffer = createFramebuffer();
		game->gameTexture = createTexture(platform->windowWidth, platform->windowHeight);

		setFramebuffer(game->gameFramebuffer);
		setColorAttachment(game->gameFramebuffer, game->gameTexture, 0);
		addDepthAttachment(game->gameTexture->width, game->gameTexture->height);
		setFramebuffer(NULL);
	}

	if (!game->omniScreenFramebuffer) {
		game->omniScreenFramebuffer = createFramebuffer();
		game->omniScreenTexture = createTexture(1600, 900);

		setFramebuffer(game->omniScreenFramebuffer);
		setColorAttachment(game->omniScreenFramebuffer, game->omniScreenTexture, 0);
		setFramebuffer(NULL);
	}

	setFramebuffer(game->omniScreenFramebuffer);
	{
		clearRenderer();

		bool mouseJustDown = platform->mouseJustDown;
		if (!game->lookingAtOmniScreen) mouseJustDown = false;

		Vec2 cursor = game->omniScreenCursorUv * v2(game->omniScreenTexture->width, game->omniScreenTexture->height);

		int scenesWide = 8;
		int scenesHigh = 8;
		int hoveredSceneIndex = -1;
		for (int y = 0; y < scenesHigh; y++) {
			for (int x = 0; x < scenesWide; x++) {
				Rect rect = makeRect(0, 0, 64, 64);
				rect.x = x * (rect.width + 10) + 10;
				rect.y = y * (rect.height + 10) + 10;

				int sceneIndex = y * scenesWide + x;
				if (rect.contains(cursor)) {
					hoveredSceneIndex = sceneIndex;
					if (mouseJustDown) game->omniScreenSelectedScene = sceneIndex;
				}

				drawRectOutline(rect, 5, 0xFFFF0000);
			}
		}

		if (hoveredSceneIndex != -1) {
			char *str = frameSprintf("Select dimension %d", hoveredSceneIndex);
			Vec2 size = getTextSize(game->omniScreenFont, str);
			Vec2 pos;
			pos.x = 600;
			pos.y = game->omniScreenTexture->height - size.y - 64;
			drawText(game->omniScreenFont, str, pos, 0xFFFFFFFF);
		}

		if (game->omniScreenSelectedScene != -1) {
			char *str = frameSprintf("Dimension %d", game->omniScreenSelectedScene);
			Vec2 textSize = getTextSize(game->omniScreenFont, str);
			Vec2 textPos;
			textPos.x = 600;
			textPos.y = 64;
			drawText(game->omniScreenFont, str, textPos, 0xFFFFFFFF);

			{
				Rect rect = makeRect(0, 0, 256, 64);
				rect.x = textPos.x;
				rect.y = textPos.y + textSize.y + 8;
				drawRectOutline(rect, 5, 0xFF00FF00);

				char *str = "Start warp";
				Vec2 size = getTextSize(game->omniScreenFont, str);
				Vec2 pos;
				pos.x = rect.x + rect.width/2 - size.x/2;
				pos.y = rect.y + rect.height/2 - size.y/2;
				drawText(game->omniScreenFont, str, pos, lerpColor(0x8000EE00, 0xFF00EE00, timePhase(platform->time*2)));

				if (rect.contains(cursor) && mouseJustDown) {
					Entity *warpZone = NULL;
					for (int i = 0; i < scene->entitiesNum; i++) {
						Entity *entity = &scene->entities[i];
						if (entity->type == ENTITY_WARP_ZONE) warpZone = entity;
					}

					if (warpZone) {
						for (int i = 0; i < scene->entitiesNum; i++) {
							Entity *entity = &scene->entities[i];
							if (entity->type == ENTITY_WARP_ZONE) continue;
							if (entity->type == ENTITY_WALL) continue;

							if (intersects(warpZone->globalBounds, entity->globalBounds)) {
								Warper *warper = &game->warpers[game->warpersNum++]; //@robustness
								memset(warper, 0, sizeof(Warper));
								warper->entityId = entity->id;
								warper->srcSceneIndex = game->currentSceneIndex;
								warper->destSceneIndex = game->omniScreenSelectedScene;
								warper->time = warper->maxTime = 3;
							}
						}
					} else {
						logf("Can't find a warpzone\n");
					}
				}
			}
		}

		{
			int color;
			if (game->lookingAtOmniScreen) {
				color = 0xFF3333FF;
			} else {
				color = lerpColor(0xFF3333FF, 0x003333FF, secondPhase);
			}
			drawCircle(cursor, 10, color);
		}
	}

	setFramebuffer(game->gameFramebuffer);
	clearRenderer(0xFF808080);

	{ /// Camera
		if (game->debugMode && !game->debugFreeCamera) {
			platform->useRelativeMouse = false;
		} else {
			platform->useRelativeMouse = true;
		}

		if (game->debugMode && game->debugFreeCamera) {
			Vec3 moveVec = v3();
			if (keyPressed('W')) moveVec.z--;
			if (keyPressed('S')) moveVec.z++;
			if (keyPressed('A')) moveVec.x--;
			if (keyPressed('D')) moveVec.x++;
			if (keyPressed(KEY_SHIFT)) moveVec.y++;
			if (keyPressed(KEY_CTRL)) moveVec.y--;

			moveVec *= game->debugFreeCameraSpeed;
			moveVec = game->cameraMatrix.multiplyAffine(moveVec);
			game->cameraPosition += moveVec;

			float sensitivity = 1;
			game->cameraRotation.z += -platform->relativeMouse.x * sensitivity*0.001;
			game->cameraRotation.x += -platform->relativeMouse.y * sensitivity*0.001;
			if (game->cameraRotation.x < 0) game->cameraRotation.x = 0;
			if (game->cameraRotation.x > M_PI) game->cameraRotation.x = M_PI;

			if (platform->mouseWheel < 0) {
				game->debugFreeCameraSpeed -= 0.01;
				logf("Speed: %f\n", game->debugFreeCameraSpeed);
			}

			if (platform->mouseWheel > 0) {
				game->debugFreeCameraSpeed += 0.01;
				logf("Speed: %f\n", game->debugFreeCameraSpeed);
			}
			if (game->debugFreeCameraSpeed < 0.01) game->debugFreeCameraSpeed = 0.01;
		}

		WorldProps *world = defaultWorld;

		game->cameraMatrix = mat4();
		game->cameraMatrix.TRANSLATE(game->cameraPosition);
		game->cameraMatrix.ROTATE_EULER(game->cameraRotation);

		world->viewMatrix = game->cameraMatrix.invert();
		world->projectionMatrix = getPerspectiveMatrix(60, platform->windowWidth/(float)platform->windowHeight, 0.1, FAR_PLANE);
	}

	game->firstTickOfFrame = true;
	while (scene->ticksToProcess > 0) {
		if (scene->ticksToProcess > 1) {
			renderer->disabled = true;
			if (scene->ticksToProcess % 100 == 0) {
				logf("%d(%.1fsec) left to process\n", scene->ticksToProcess, scene->ticksToProcess / 60.0);
			}
		}

		updateScene(scene);
		game->firstTickOfFrame = false;

		renderer->disabled = false;
		scene->ticksToProcess--;
		scene->ticksTotal++;
	}

	for (int i = 0; i < game->warpersNum; i++) {
		Warper *warper = &game->warpers[i];
		warper->time -= elapsed;

		if (warper->time <= 0) {
			Scene *srcScene = &game->scenes[warper->srcSceneIndex];
			Scene *destScene = &game->scenes[warper->destSceneIndex];

			Entity *toWarp = getEntity(srcScene, warper->entityId);
			if (toWarp) {
				Entity tempEntity = *toWarp;
				destroyEntity(srcScene, toWarp);
				Entity *newEntity = createEntity(destScene, ENTITY_NONE);
				*newEntity = tempEntity;

				if (newEntity->type == ENTITY_PLAYER) {
					game->currentSceneIndex = warper->destSceneIndex;
				}
				newEntity->justWarped = true;
			} else {
				logf("Entity not found, warper failed\n");
			}
			arraySpliceIndex(game->warpers, game->warpersNum, sizeof(Warper), i);
			i--;
			game->warpersNum--;
			continue;
		}
	}

	drawSphere(v3(15, 0, 0), 1, 0xFFFF0000);
	drawSphere(v3(0, 15, 0), 1, 0xFF00FF00);
	drawSphere(v3(0, 0, 15), 1, 0xFF0000FF);

	drawSphere(defaultWorld->sunPosition, 1, 0xFFFFFF00);
	// defaultWorld->sunPosition = v3(lerp(-10, 10, timePhase(platform->time*0.2)), -24, 28);

	process3dDrawQueue();
	drawOnScreenLog();

	setFramebuffer(NULL);

	Entity *player = NULL;
	for (int i = 0; i < scene->entitiesNum; i++) {
		Entity *entity = &scene->entities[i];
		if (entity->type == ENTITY_PLAYER) player = entity;
	}

	float playerWarpPerc = 0;
	if (player) {
		for (int i = 0; i < game->warpersNum; i++) {
			Warper *warper = &game->warpers[i];
			if (warper->entityId == player->id) {
				playerWarpPerc = 1 - warper->time/warper->maxTime;
			}
		}
	}

	Matrix3 mat = mat3();
	mat.SCALE(game->gameTexture->width, game->gameTexture->height);
	drawPost3dTexture(game->gameTexture, mat, lerpColor(0x00FF00FF, 0xFFFF00FF, playerWarpPerc));
}

int MAX_CRYSTAL_GROWTH = 3*60*60;
int MAX_EGG_GROWTH = 10*60*60;

void updateScene(Scene *scene) {
	Globals *globals = &game->globals;
	float elapsed = platform->elapsed;
	float secondPhase = timePhase(platform->time);

	if (!scene->map) {
		Tile::Map *map = NULL;
		scene->tileSize = v3(3, 3, 4);

		if (game->currentSceneIndex == 0) {
			scene->tileSize.z = 10;

			using namespace Tile;
			GeneratorProps props = Tile::newGeneratorProps();
			map = startMap(50, 50, props);

			int roomWidth = 5;
			int roomHeight = 5;
			int roomX = map->width/2 - roomWidth/2;
			int roomY = map->height/2 - roomHeight/2;
			Room room = getRoom(map, roomX, roomY, roomWidth, roomHeight);
			addRoom(map, room);

			finalizeMap(map);

			{
				Entity *entity = createEntity(scene, ENTITY_PLAYER);
				entity->position.x = map->width/2 * scene->tileSize.x;
				entity->position.y = map->height/2 * scene->tileSize.y;
				entity->position.z = 5;
				game->playerRotation.x = M_PI/2;
			}

			{
				Model *model = getModel("assets/models/Items/OmniScreen.model");
				Vec3 size = getSize(model->bounds);

				Entity *screen = createEntity(scene, ENTITY_OMNI_SCREEN);
				screen->position.x = map->width/2 * scene->tileSize.x;
				screen->position.y = map->height/2 * scene->tileSize.y;
				screen->position.z = size.z/2 + 0.3;
				screen->position.y += roomHeight/2 * scene->tileSize.y + scene->tileSize.y/2 - 0.01;
			}

			{
				Model *model = getModel("assets/models/Items/WarpZone.model");
				Vec3 size = getSize(model->bounds);

				Entity *warpZone = createEntity(scene, ENTITY_WARP_ZONE);
				warpZone->position.x = map->width/2 * scene->tileSize.x;
				warpZone->position.y = map->height/2 * scene->tileSize.y;
				// warpZone->position.y = roomY * scene->tileSize.y + (size.y - scene->tileSize.y);
				warpZone->position.z = 0;
				warpZone->position.y -= roomHeight/2 * scene->tileSize.y - (size.y/2 - scene->tileSize.y/2);
			}
		} else {
			Tile::GeneratorProps props = Tile::newGeneratorProps();
			props.roomsMin = 2;
			props.roomsMax = 3;
			props.hallwaySizeMin = 1;
			props.hallwaySizeMax = 4;
			map = Tile::generate(50, 50, props);
		}

		scene->map = map;
		scene->availTiles = (int *)zalloc(sizeof(int) * map->width * map->height);
		for (int i = 0; i < map->width * map->height; i++) {
			if (map->tiles[i] == 0) scene->availTiles[scene->availTilesNum++] = i;
		}

		if (game->currentSceneIndex > 0) {
			{ /// Crystal room
				int crystalsToSpawn = rndInt(2, 5);

				for (int i = 0; i < crystalsToSpawn; i++) {
					if (scene->availTilesNum == 0) {
						logf("No more room for crystals\n");
						break;
					}
					int availIndex = rndInt(0, scene->availTilesNum-1);
					int tileIndex = scene->availTiles[availIndex];

					Entity *entity = createEntity(scene, ENTITY_CRYSTAL);
					entity->position.x = (tileIndex % map->width) * scene->tileSize.x;
					entity->position.y = (tileIndex / map->width) * scene->tileSize.y;
					entity->position.z = 0;

					Model *model = getModel("assets/models/Items/Crystals.model");
					Vec3 size = getSize(model->bounds);
					entity->position.x += rndFloat(-size.x/2, size.x/2);
					entity->position.y += rndFloat(-size.y/2, size.y/2);
				}
			}
		}

		for (int y = 0; y < map->height; y++) {
			for (int x = 0; x < map->width; x++) {
				int tileIndex = y * map->width + x;
				if (map->tiles[tileIndex] == 1) {
					Entity *entity = createEntity(scene, ENTITY_WALL);
					entity->position.x = x * scene->tileSize.x;
					entity->position.y = y * scene->tileSize.y;
					entity->position.z = scene->tileSize.z/2;
					entity->scale = scene->tileSize;
					entity->tint = argbToHex(255, rndInt(0, 255), rndInt(0, 255), rndInt(0, 255));
				}
			}
		}

		{ /// Ground
			Entity *entity = createEntity(scene, ENTITY_WALL);
			entity->position.x = (map->width * scene->tileSize.x)/2;
			entity->position.y = (map->height * scene->tileSize.y)/2;
			entity->position.z = -0.5;
			entity->scale.x = (map->width * scene->tileSize.x);
			entity->scale.y = (map->height * scene->tileSize.y);
			entity->scale.z = 1;
			entity->tint = 0xFF808080;
		}
	}

	Tile::Map *map = scene->map;

	if (game->firstTickOfFrame) {
		if (keyJustPressed(KEY_BACKTICK)) game->debugMode = !game->debugMode;
		if (game->debugMode) {
			if (keyJustPressed(' ')) game->debugFreeCamera = !game->debugFreeCamera;

			{
				ImGui::Begin("Menu", NULL, ImGuiWindowFlags_AlwaysAutoResize);
				if (game->debugFreeCamera) ImGui::Text("In free camera, Space to stop");
				ImGui::SliderFloat("debugFreeCameraSpeed", &game->debugFreeCameraSpeed, 0.01, 5);
				ImGui::Checkbox("disableDiffuse", &meshSys->disableDiffuse);
				ImGui::Checkbox("disableNormals", &meshSys->disableNormals);
				ImGui::Checkbox("disableSpecular", &meshSys->disableSpecular);
				ImGui::Checkbox("disableCulling", &meshSys->disableCulling);
				ImGui::Checkbox("disableInstancing", &meshSys->disableInstancing);

				ImGui::SliderFloat3("ambientLight", &defaultWorld->ambientLight.x, 0, 1);
				ImGui::SliderFloat3("diffuseLight", &defaultWorld->diffuseLight.x, 0, 1);
				ImGui::SliderFloat3("specularLight", &defaultWorld->specularLight.x, 0, 1);

				ImGui::SliderInt("debugExtraTicksPerFrame", &game->debugExtraTicksPerFrame, 0, 60);

				ImGui::Text("Ticks so far: %d(%.1fsec)", scene->ticksTotal, scene->ticksTotal / 60.0);
				ImGui::SliderInt("currentSceneIndex", &game->currentSceneIndex, 0, SCENES_MAX-1);

				if (ImGui::TreeNode("Meshes")) {
					showMeshGui();
					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Models")) {
					showModelsGui();
					ImGui::TreePop();
				}

				ImGui::End();
			}

			{
				ImGui::SetNextWindowPos(ImVec2(platform->windowWidth, 0), ImGuiCond_Always, ImVec2(1, 0));
				ImGui::Begin("Entities", NULL, ImGuiWindowFlags_AlwaysAutoResize);

				ImGui::Text("Entities:");
				ImGui::BeginChild("Entities", ImVec2(0, 200), false, 0); 
				for (int i = 0; i < scene->entitiesNum; i++) {
					Entity *entity = &scene->entities[i];
					ImGui::PushID(i);

					char *name = entityTypeStrings[entity->type];
					if (ImGui::Selectable(name, game->selectedEntityId == entity->id)) {
						game->selectedEntityId = entity->id;
					}
					ImGui::PopID();
				}
				ImGui::EndChild();

				Entity *selectedEntity = NULL;
				for (int i = 0; i < scene->entitiesNum; i++) {
					Entity *entity = &scene->entities[i];
					if (entity->id == game->selectedEntityId) {
						selectedEntity = entity;
						break;
					}
				}

				ImGui::Separator();

				if (selectedEntity) {
					Entity *entity = selectedEntity;

					ImGui::Text("Id: %d", entity->id);
					ImGui::Combo("Type", (int *)&entity->type, entityTypeStrings, ArrayLength(entityTypeStrings));
					ImGui::InputInt("hp", &entity->hp);

					ImGui::Separator();
					ImGui::DragFloat3("Position", &entity->position.x, 0.01);
					ImGui::DragFloat3("Size", &entity->scale.x, 0.01);
					ImGui::DragFloat3("Rotation", &entity->rotation.x, 0.01);

					ImGui::Text("Acceleration: %.1f %.1f %.1f", entity->accel.x, entity->accel.y, entity->accel.z);
					ImGui::DragFloat3("Velocity", &entity->velo.x, 0.01);
					// guiInputArgb("Tint", &entity->tint);
					// ImGui::SliderFloat("Alpha", &entity->alpha, 0, 1);

					ImGui::Button("Hold to show bounds");
					if (ImGui::IsItemActive()) {
						drawBoundsOutline(entity->globalBounds, 0.01, 0xFFFF0000);
					}
					ImGui::Separator();
					if (entity->type == ENTITY_STATIC_MODEL) {
						ImGui::InputText("Model path", entity->path, PATH_MAX_LEN);
						ImGui::Checkbox("Is collider", &entity->isCollider);
					} else if (entity->type == ENTITY_EGG) {
						float growthPerc = entity->growth / (float)MAX_EGG_GROWTH;
						int toSpawn = ceil(growthPerc*entity->maxEnemiesContained);
						ImGui::Text("Egg growth perc: %f", growthPerc);
						ImGui::Text("Would spawn: %d/%d", toSpawn, entity->maxEnemiesContained);
					}
				}

				ImGui::End();
			}
		}
	}

	{
		if (scene->index != 0) scene->infectionTicks++;
		int infectionTicksPerEvent = 100;

		Entity **infectedEntities = (Entity **)frameMalloc(sizeof(Entity *) * scene->entitiesNum);
		int infectedEntitiesNum;

		while (scene->infectionTicks > infectionTicksPerEvent) {
			bool consumeInfectionTicks = true;

			infectedEntitiesNum = 0;
			for (int i = 0; i < scene->entitiesNum; i++) {
				Entity *entity = &scene->entities[i];
				if (entity->type == ENTITY_EGG) {
					infectedEntities[infectedEntitiesNum++] = entity;
				}
			}

			enum InfectionOption {
				INFECT_ADD_EGG,
				INFECT_GROW,
				INFECT_END,
			};

			int weights[INFECT_END] = {};
			weights[INFECT_ADD_EGG] = 1;
			weights[INFECT_GROW] = 4;
			InfectionOption choice = (InfectionOption)rndPick(weights, INFECT_END);

			if (choice == INFECT_ADD_EGG) {
				if (scene->availTilesNum > 0 && infectedEntitiesNum < 20) {
					int availIndex = rndInt(0, scene->availTilesNum-1);
					int tileIndex = scene->availTiles[availIndex];

					int x = tileIndex % map->width;
					int y = tileIndex / map->width;

					Entity *entity = createEntity(scene, ENTITY_EGG);
					entity->position.x = x * scene->tileSize.x;
					entity->position.y = y * scene->tileSize.y;
				} else {
					consumeInfectionTicks = false;
				}
			} else if (choice == INFECT_GROW) {
				if (infectedEntitiesNum > 0) {
					Entity *entity = infectedEntities[rndInt(0, infectedEntitiesNum-1)];
					entity->growth += 1000;
				} else {
					consumeInfectionTicks = false;
				}
			}

			if (consumeInfectionTicks) {
				scene->infectionTicks -= infectionTicksPerEvent;
				scene->infectionTicksUsed += infectionTicksPerEvent;
			}
		}
	}

	Entity *player = NULL;
	for (int i = 0; i < scene->entitiesNum; i++) { /// First iter
		Entity *entity = &scene->entities[i];

		entity->accel = v3();

		if (entity->type == ENTITY_PLAYER) {
			player = entity;
		}
	}

	Vec3 playerAimStart;
	Vec3 playerAimDir;
	if (player) {
		Entity *entity = player;
		entity->hasPhysics = true;
		entity->localBounds = makeAABB(v3(-0.2, -0.2, -1), v3(0.2, 0.2, 0.1));

		Vec3 moveVec = v3();
		bool shoot = false;
		if (game->firstTickOfFrame && !game->debugMode) {
			if (keyPressed('W')) moveVec.z--;
			if (keyPressed('S')) moveVec.z++;
			if (keyPressed('A')) moveVec.x--;
			if (keyPressed('D')) moveVec.x++;
			if (keyJustPressed(' ') && entity->isOnGround) entity->velo.z = 0.3;

			float sensitivity = 1;
			game->playerRotation.z += -platform->relativeMouse.x * sensitivity*0.001;
			game->playerRotation.x += -platform->relativeMouse.y * sensitivity*0.001;
			if (game->playerRotation.x < 0) game->playerRotation.x = 0;
			if (game->playerRotation.x > M_PI) game->playerRotation.x = M_PI;

			if (platform->mouseJustDown) shoot = true;
		}

		moveVec = game->cameraMatrix.multiplyAffine(moveVec);
		moveVec.z = 0;
		Vec2 moveVec2 = v2(moveVec.x, moveVec.y).normalize();
		moveVec.x = moveVec2.x;
		moveVec.y = moveVec2.y;

		entity->accel += moveVec * 0.05;

		if (!game->debugMode) {
			game->cameraPosition = entity->position;
			game->cameraRotation = game->playerRotation;
		} else {
			// drawBoundsOutline(bounds, lerp(0.01, 0.1, secondPhase), 0xFFFF00FF);
		}

		playerAimStart = entity->position;
		playerAimDir = getRayFromSceenSpace(v2(platform->windowWidth/2, platform->windowHeight/2), v3());

		if (shoot) {
			Vec3 hitPos;
			Entity *shotEntity = getShotEntity(playerAimStart, playerAimDir, &hitPos, true);
			if (shotEntity) {
				int crumbsToSpawn = rndInt(3, 5);
				for (int i = 0; i < crumbsToSpawn; i++) {
					Entity *crumb = createEntity(scene, ENTITY_CRUMB);
					crumb->position = hitPos;
					float mass = rndFloat(0.5, 1.0);
					crumb->scale *= mass * 0.25;
					crumb->velo = playerAimDir.normalize() * -1 * 0.5;
					crumb->velo.x += rndFloat(-0.1, 0.1);
					crumb->velo.y += rndFloat(-0.1, 0.1);
					crumb->velo.z += mass * 0.1;

					if (shotEntity->type == ENTITY_EGG) crumb->tint = lerpColor(0xFF00FF00, 0x8000FF00, rndFloat(0, 1));
				}

				dealDamage(shotEntity, 10);
			}
		}
	}

	for (int i = 0; i < scene->entitiesNum; i++) { /// Update and draw non-player
		Entity *entity = &scene->entities[i];

		Model *model = NULL;
		ModelProps props = newModelProps();

		if (entity->justWarped) {
			entity->justWarped = false;
			entity->position.x = map->width/2 * scene->tileSize.x;
			entity->position.y = map->height/2 * scene->tileSize.y;
			entity->position.z = 5;
		}

		if (entity->type == ENTITY_PLAYER) {
			// Handled above
		} else if (entity->type == ENTITY_STATIC_MODEL) {
			model = getModel(entity->path);
		} else if (entity->type == ENTITY_WALL) {
			entity->isCollider = true;
			model = getModel("assets/models/Wall/Wall.model");
			entity->localBounds = model->bounds;
		} else if (entity->type == ENTITY_CRYSTAL) {
			entity->growth++;
			if (entity->growth > MAX_CRYSTAL_GROWTH) entity->growth = MAX_CRYSTAL_GROWTH;

			model = getModel("assets/models/Items/Crystals.model");
			entity->scale.z = entity->growth / (float)MAX_CRYSTAL_GROWTH;
			entity->localBounds = model->bounds;
		} else if (entity->type == ENTITY_EGG) {
			if (entity->growth > MAX_EGG_GROWTH) entity->growth = MAX_EGG_GROWTH;

			model = getModel("assets/models/Items/Egg.model");
			float growthPerc = entity->growth / (float)MAX_EGG_GROWTH;
			float scale = clampMap(growthPerc, 0, 1, 0.25, 1);
			entity->scale = v3(scale, scale, scale);
			entity->localBounds = model->bounds;

			int hpBreakLimit = 100*growthPerc;
			if (entity->hp < hpBreakLimit && !entity->broken) {
				entity->broken = true;
				entity->enemiesToSpawn = ceil(growthPerc * entity->maxEnemiesContained);
			}

			if (entity->broken) {
				if (entity->enemiesToSpawn <= 0) {
					entity->markedForDestruction = true;
				} else {
					if (rndPerc(0.02)) {
						entity->enemiesToSpawn--;
						Entity *newEntity = createEntity(scene, ENTITY_LING);
						newEntity->position = entity->position;
						newEntity->position.z += 0.01;
					}
				}
			}
		} else if (entity->type == ENTITY_LING) {
			entity->hasPhysics = true;

			model = getModel("assets/models/Items/Ling.model");
			entity->localBounds = model->bounds;

			if (player) {
				Vec3 target = player->position;

				Vec3 dir = (target - entity->position).normalize();
				dir.z = 0;
				Vec2 dir2 = v2(dir).normalize();
				// drawBeam(entity->position, entity->position + dir*1, 0.1, 0xFFFF0000);

				entity->rotation.z = toRad(dir2) - toRad(90);

				entity->accel.x = dir2.x * entity->lingAccelSpeed;
				entity->accel.y = dir2.y * entity->lingAccelSpeed;

				if (intersects(player->globalBounds, entity->globalBounds)) {
					entity->markedForDestruction = true;
					dealDamage(player, 5);
				}
			}
		} else if (entity->type == ENTITY_CRUMB) {
			entity->hasPhysics = true;
			entity->unshootable = true;
			model = getModel("assets/models/Items/Crumb.model");

			float maxTime = 1;
			float perc = entity->time / maxTime;
			entity->scaleMulti = 1.0-perc;

			if (perc >= 1) {
				entity->markedForDestruction = true;
			}
		} else if (entity->type == ENTITY_OMNI_SCREEN) {
			model = getModel("assets/models/Items/OmniScreen.model");
			Model *nestedModel = &model->children[0];
			Mesh *mesh = nestedModel->mesh;
			if (mesh) {
				mesh->diffuseTexture = game->omniScreenTexture;
			} else {
				logf("No mesh for omni?\n");
			}
			entity->localBounds = model->bounds;

			game->lookingAtOmniScreen = false;
			if (player) {
				Vec3 start = entity->matrix.invert() * playerAimStart;
				Vec3 end = entity->matrix.invert() * (playerAimStart + playerAimDir*10);

				float dist;
				Vec2 baryUv;
				MeshTri *meshTri;
				if (intersectsLine(model, start, end, &dist, &baryUv, &meshTri)) {
					Vec3 bary;
					bary.x = 1 - baryUv.x - baryUv.y;
					bary.y = baryUv.x;
					bary.z = baryUv.y;

					Vec2 uv = v2();
					uv += meshTri->verts[0].uv * bary.x;
					uv += meshTri->verts[1].uv * bary.y;
					uv += meshTri->verts[2].uv * bary.z;
					uv.y = 1 - uv.y;
					game->omniScreenCursorUv = uv;
					game->lookingAtOmniScreen = true;

					// logf("%.2f %.2f %.2f | %.2f %.2f\n", bary.x, bary.y, bary.z, uv.x, uv.y);
				}
			}
		} else if (entity->type == ENTITY_WARP_ZONE) {
			entity->unshootable = true;
			model = getModel("assets/models/Items/WarpZone.model");
			entity->localBounds = model->bounds;
		}

		entity->matrix = mat4();
		entity->matrix.TRANSLATE(entity->position);
		entity->matrix.ROTATE_EULER(entity->rotation);
		entity->matrix.SCALE(entity->scale * entity->scaleMulti);

		entity->globalBounds = entity->matrix * entity->localBounds;

		if (entity->hasPhysics) {
			entity->isOnGround = false;
			entity->velo += entity->accel;
			entity->velo.z -= 0.02;

			entity->velo.x *= 0.75;
			entity->velo.y *= 0.75;
			entity->velo.z *= 0.95;
		}

		if (model) {
			props.matrix = entity->matrix;
			if (entity->tint != 0) {
				Vec4 tintVec = hexToArgbFloat(entity->tint);
				Vec3 tintVec3;
				tintVec3.x = tintVec.y * tintVec.x;
				tintVec3.y = tintVec.z * tintVec.x;
				tintVec3.z = tintVec.w * tintVec.x;

				props.diffuseLight *= tintVec3;
				props.ambientLight *= tintVec3;
			}
			drawModel(model, props);
		}

		entity->time += elapsed;
	}

	int iters = 1;
	for (int i = 0; i < iters; i++) {
		float perc = 1.0/iters;
		for (int i = 0; i < scene->entitiesNum; i++) { /// Phys step update
			Entity *entity = &scene->entities[i];
			if (!entity->hasPhysics) continue;

			Vec3 nextPosition = entity->position + entity->velo*perc;

			Vec3 nextPositionX = entity->position;
			nextPositionX.x = nextPosition.x;

			Vec3 nextPositionY = entity->position;
			nextPositionY.y = nextPosition.y;

			Vec3 nextPositionZ = entity->position;
			nextPositionZ.z = nextPosition.z;

			// entity->localBounds = makeAABB(v3(-0.2, -0.2, -1), v3(0.2, 0.2, 0.1));
			// drawBoundsOutline(entity->localBounds + entity->position, 0.01, 0xFF00FF00);

			AABB nextBounds = entity->localBounds + nextPosition;

			AABB nextBoundsX = entity->localBounds + nextPositionX;
			AABB nextBoundsY = entity->localBounds + nextPositionY;
			AABB nextBoundsZ = entity->localBounds + nextPositionZ;

			for (int i = 0; i < scene->entitiesNum; i++) {
				Entity *other = &scene->entities[i];
				if (!other->isCollider) continue;

				// AABB otherBounds = other->matrix * other->localBounds;
				AABB otherBounds = other->localBounds;
				otherBounds = otherBounds * other->scale;
				otherBounds = otherBounds + other->position;

				if (intersects(nextBounds, otherBounds)) {
					if (intersects(nextBoundsX, otherBounds)) {
						nextPosition.x = entity->position.x;
						entity->velo.x = 0;
					}
					if (intersects(nextBoundsY, otherBounds)) {
						nextPosition.y = entity->position.y;
						entity->velo.y = 0;
					}
					if (intersects(nextBoundsZ, otherBounds)) {
						if (nextPosition.z < entity->position.z && entity->velo.z < 0) {
							entity->velo.z = 0;
							entity->isOnGround = true;
						}
						nextPosition.z = entity->position.z;
					}
				}
			}

			entity->position = nextPosition;
		}
	}

	if (player && game->firstTickOfFrame) { /// Interaction
		float closestDist;
		Entity *closestEntity = NULL;
		for (int i = 0; i < scene->entitiesNum; i++) {
			Entity *entity = &scene->entities[i];
			if (entity->type == ENTITY_PLAYER) continue;
			Vec3 hit;
			if (!overlaps(entity->globalBounds, makeLine3(playerAimStart, playerAimStart + playerAimDir*2), &hit)) continue;
			float dist = playerAimStart.distance(hit);
			if (!closestEntity || closestDist > dist) {
				closestEntity = entity;
				closestDist = dist;
			}
		}

		if (closestEntity) {
			if (closestEntity->type == ENTITY_CRYSTAL) {
				char *str = "Collect crystals";
				Vec2 size = getTextSize(fontSys->defaultFont, str);
				Vec2 pos;
				pos.x = platform->windowWidth/2 + 10;
				pos.y = platform->windowHeight/2 + size.y/2;
				drawText(fontSys->defaultFont, str, pos, 0xFFFFFFFF);
				if (keyJustPressed('E')) {
					if (closestEntity->growth > MAX_CRYSTAL_GROWTH*0.25) {
						closestEntity->growth -= MAX_CRYSTAL_GROWTH*0.25;
						game->crystals += 1;
					} else {
						logf("Not enough grown\n");
					}
				}
			}
		}
	}

	/// Entity destruction
	for (int i = 0; i < scene->entitiesNum; i++) {
		Entity *entity = &scene->entities[i];
		if (!entity->markedForDestruction) continue;

		destroyEntity(scene, entity);
		continue;
	}

	defaultWorld->sunPosition.x = map->width/2 * scene->tileSize.x;
	defaultWorld->sunPosition.y = map->height/2 * scene->tileSize.y;
	defaultWorld->sunPosition.z = 20;

	process3dDrawQueue();

	{
		Texture *texture = renderer->whiteTexture;
		RenderProps props = newRenderProps();
		props.matrix.TRANSLATE(game->gameTexture->width/2, game->gameTexture->height/2);
		props.matrix.SCALE(50);
		props.matrix.TRANSLATE(-0.5, -0.5);
		props.flags |= _F_ARC;
		props.params.x = 0.2;
		props.params.y = 1;
		drawTexture(texture, props);
	}

	{
		if (player) {
			char *str = frameSprintf("%d hp (%.1fms (%.1ffps))", player->hp, platform->frameTimeAvg, 1.0/platform->frameTimeAvg * 1000.0);
			Vec2 size = getTextSize(fontSys->defaultFont, str);
			Vec2 pos = v2();
			drawText(fontSys->defaultFont, str, pos);
		}
	}
}

Entity *getShotEntity(Vec3 rayStart, Vec3 rayDir, Vec3 *hitPos, bool excludePlayer) {
	Scene *scene = &game->scenes[game->currentSceneIndex];

	Vec3 start = rayStart;
	Vec3 end = rayStart + rayDir*9999;

	float closestDist;
	Entity *closestEntity = NULL;
	for (int i = 0; i < scene->entitiesNum; i++) {
		Entity *entity = &scene->entities[i];
		if (excludePlayer && entity->type == ENTITY_PLAYER) continue;
		if (entity->unshootable) continue;

		Vec3 hit;
		if (!overlaps(entity->globalBounds, makeLine3(start, end), &hit)) continue;

		float dist = rayStart.distance(hit);
		if (!closestEntity || closestDist > dist) {
			closestEntity = entity;
			closestDist = dist;
			*hitPos = hit;
		}
	}

	return closestEntity;
}

Vec3 getRayFromSceenSpace(Vec2 screenPosition, Vec3 cameraOffset) {
	Vec3 start = game->cameraPosition + cameraOffset;

	Vec3 mouse = v3(screenPosition.x, screenPosition.y, 1.0);
	mouse.x /= platform->windowWidth;
	mouse.y /= platform->windowHeight;
	mouse.x = mouse.x*2.0 - 1.0;
	mouse.y = mouse.y*2.0 - 1.0;
	mouse.y *= -1;

	Vec4 rayClip = v4(mouse.x, mouse.y, -1, 1);
	Vec4 rayEye = defaultWorld->projectionMatrix.invert().multiply(rayClip);
	rayEye = v4(rayEye.x, rayEye.y, -1, 0);
	Vec4 rayWorld4 = defaultWorld->viewMatrix.invert().multiply(rayEye);
	Vec3 rayWorld = v3(rayWorld4.x, rayWorld4.y, rayWorld4.z).normalize();

	return rayWorld;
}

Entity *createEntity(Scene *scene, EntityType type) {
	if (scene->entitiesNum > scene->entitiesMax-1) {
		if (!scene->entities) {
			scene->entitiesMax = 8;
			scene->entities = (Entity *)zalloc(sizeof(Entity) * scene->entitiesMax);
		} else {
			scene->entities = (Entity *)resizeArray(scene->entities, sizeof(Entity), scene->entitiesMax, scene->entitiesMax*1.5);
			scene->entitiesMax *= 1.5;
		}
	}

	Entity *entity = &scene->entities[scene->entitiesNum++];
	memset(entity, 0, sizeof(Entity));
	entity->id = ++scene->nextEntityId;
	entity->scale = v3(1, 1, 1);
	entity->scaleMulti = 1;
	entity->type = type;
	entity->hp = 100;
	if (entity->type == ENTITY_EGG) {
		entity->maxEnemiesContained = rndInt(10, 20);
	} else if (entity->type == ENTITY_LING) {
		float mass = rndFloat(0.5, 1.5);
		entity->scale *= mass;
		entity->lingAccelSpeed = 0.015 / mass;
	}
	return entity;
}

void destroyEntity(Scene *scene, Entity *entity) {
	for (int i = 0; i < scene->entitiesNum; i++) {
		Entity *otherEntity = &scene->entities[i];
		if (entity != otherEntity) continue;

		arraySpliceIndex(scene->entities, scene->entitiesNum, sizeof(Entity), i);
		i--;
		scene->entitiesNum--;
		return;
	}
}

Entity *getEntity(Scene *scene, int id) {
	for (int i = 0; i < scene->entitiesNum; i++) {
		Entity *entity = &scene->entities[i];
		if (entity->id == id) return entity;
	}

	return NULL;
}

void dealDamage(Entity *entity, int amount) {
	entity->hp -= amount;
}
