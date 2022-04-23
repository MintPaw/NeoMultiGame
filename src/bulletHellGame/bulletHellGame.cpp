#define INSTANT_START_PLAYING_LOCAL 0
#define INSTANT_START_TRAINING 1
#define INSTANT_START_CONNECTING 0

#define START_WITH_GOD_MODE 0
#define START_WITH_STARS 0

#define SAVES_FROM_LOG 0
#define SAVES_LOG 0

#define VN_SCRIPT_TO_TEST 0
#define DO_NEW_TREES 0

/*
- Bullet Model Specific notes:
	- Knife bullets should probably be thicker or bigger or something.
	- Players should have seperate bullet and bomb models.
	- Player can clip through their bullets. Maybe they should be underneath them.
	- I figured out how to distinguish player and enemy bullets. Player bullets can be thinner. Enemy bullets are very stout and obvious to dodge. Player bullets don't need that.
	- Also. Since players will have their own seperate bullet models. Bullet-to-bullet radius doesn't really need to exist anymore. I'll remove it later.
	- New bullet models that need to exist:
		BEAM(Shaped like a cylinder. 0.5x1.5x0.5)
		STAR(Similar to the rice bullet. But instead of two points there's five.)
		MISSILE2(Similar to missile but it's more slender. More pointed. And has fins.)
		ARROW(I'm thinking it should be two cones. A 1x2x1 cone inside of a 1x1x1 cone.)

- Notes:
	- Enemies need models and animations. M- Maybe they shouldn't be danmaku??
	- Does Vec2 use doubles? I got an error message that seemed to suggest that.
	- Why do you have to hold the start button on the pre-battle menu? And can it take less time to press?
	- Add a Character(Info?) struct
	- Raising the difficulty should play a special effect on the screen.
	- Francesca (The default character) should proably have a more interesting shot pattern. I have ideas. But I kinda wanna keep her simple.
	- We should make a better way to buy spells.
	- If the shop is empty and you press extra. I think the game will theoretically crash. Haven't tested it.
	- Pre battle menu is like all broken.
	- Seems like the player is being drawn a frame later than it should be. Execution order bug?
	- I'm starting to think the option-turret character (Now named "Farrow Drupe") is maybe a too strong. At least at full power.
	- Particles aren't visible.
	- game->phaseTime == 0 returns true for multiple frames. This needs to be investigated.
	- I still kinda think realGame and realGame2 should be an array.
	- Rock2 is insanely distracting. It looks like a bullet.
	- The items dropped from enemies blink.
	- Wind effects
	- More map props
	- Sounds should play when danmaku frame hits zero. Not when it spawns.
	- Bloom at the very top of the screen also shows up at the bottom. Perhaps it's because of bilinear. (Make it bigger)
	- Discuss particles and particle behaviours.
	- Option Turret should possibly be a mesh.
	- Hitboxes need to be reworked around meshes instead of textures.
	- Discuss sound design (player shots have no sound)
	- Maybe you should be able to buy bombs or perhaps they should be on cooldown.
	- I mean.. Somebody's gotta make the pre-battle menu more flexible
	- Interception laser needs a better effect
	- Make removeDirectory() not call system()
	- Time since last seen while in queue?

- Issues:
	- Bullets can clip through eachother?
	- Boss health bar flickers. (When firing bullets it seems.)
	- Models occasionally draw on top of 2d stuff. (Best examples being the danmaku debug info and the 2d focus area shown when holding shift)
	- Boss bag system
	- Convert to true 3d (both player different background)
	- Control bindings

- Done:
	- Laser Addendum doesn't skip around anymore. That's right. I fixed it. =3
	- Ugh. Shop rework 2.0 is now mostly done. It's looking pretty good but online stuff needs to be cleaned up.
	- Farrow's (The turret character) options have been slightly nerfed.
	- Penelope (The mirror character) has been tweaked. I'm not satisfied with it yet.
	- Rain enemies had their hp buffed.
	- Added two new enemies called "Seek" and "Omni". They don't spawn in actual matches yet. Adjustments still need to be made.

- Ashy's Notes:
	- DracoSofts medium pattern should have speedAccel on the bullets
	- enemyWaveI should be modified.
	- ull should probably be switched for like u64 or something.
	- I should probably find some way to distinguish spells that target players and enemies.
	- Make a spell for activating god mode and getting more bombs.
	- Remember to remove getModelFromGraphic
	- Remember to pre-caclulate the boss bag rng.
	- Maybe Tabitha shouldn't crash the game if you level up her attack too much..
	- Remember to buff the sun spell.
	- I wanna make a spell for lowering a players attack level.
	- Modifiers should be able to play sounds on activation.
	- Add a bool for disabling the hitbox.
	- Enemies spawn on screen.
	- Add a bool for making bullets silent when fired.

Shop items:
	- Boss (stage specific?)
	- Smoke screen
	- Wind
	- Gravity well bullets
	- Speed up/Slow down/Oscilate speed of player.
*/

#include "bulletHellGameCommon.cpp"
#include "Danmaku.cpp" // Header
#include "shopStuff.cpp" // Header
#include "vn.cpp" // Header

struct Globals {
	float playerHitRadius;
	int playerFireRate;
	int poc;
	int bombsPerLife;
	float invTimeOnDeath;
	int binsWide;
	int binsHigh;
	int grazePad;
	int spellDisplayTime;

	float backgroundDistance;
	float backgroundSpawnDepth;
	float backgroundFrontClip;
	float cameraFov;
	float backgroundSpeed;
	float backgroundTreeSpread;
	Vec2 treeSpawnTimeMinMax;

	Vec3 cameraPosition;
	Vec3 cameraRotation;
	Vec3 sunPosition;
	Vec3 ambientLight;
	Vec3 diffuseLight;
	Vec3 specularLight;

	bool showSyncString;
	float forestGroundFreq;
	int forestGroundDepth;
	float forestGroundMulti;
};

enum MapPropType {
	PROP_NONE,
	PROP_TERRAIN,
	PROP_TREE,
	PROP_ROCK,
};
struct MapProp {
	MapPropType type;
	int id;
	Vec3 position;
	Vec3 rotation;
	Vec3 scale;

	char modelPath[PATH_MAX_LEN];
	Mesh *customMesh;
};

enum AppState {
	APP_MENU,
	APP_IN_QUEUE,
	APP_PLAYING,
};

enum PlayMode {
	PLAY_NONE,
	PLAY_LOCAL,
	PLAY_ONLINE,
	PLAY_TRAINING,
};

struct App {
	// Texture *appTexture;

	AppState prevState;
	AppState state;
	float stateTime;

	PlayMode playMode;

#define MSGS_MAX 64
	NetMsg outMsgs[MSGS_MAX];
	int outMsgsNum;

	sockaddr_storage serverAddress;
	Socket *socket;
	int clientId;
	int enemyClientId;
	char serverIp[32];
	int startingSeed;

	Input *streamedInputs;
	int streamedInputsNum;
	int streamedInputsMax;

	int streamInputHash;
	int streamInputCount;
	char *streamSyncString;
	int streamSyncStringNum;
	int streamSyncStringMax;
	int lastTotalInputs;

	int ping;
	int enemyPing;

	Globals globals;
	Font *defaultFont;
	Font *smallFont;
	Font *bigFont;
	Font *hugeFont;
	Font *uiFont;
	bool online;
	float timeSinceLastSeenServer;
	float timeSinceLastPingedServer;

	ShopItem shopList[64];//The flags for shop items are a u64 thus 64 length array.
	int shopListNum;

	bool debugStopFrameAdvance;
	bool debugStepFrameAdvance;
	int debugLagSwitch;
	bool debugRandomMovement;
	int debugRandomMovementSeed;

#define DANMAKU_GRAPHIC_MAX DANMAKU_GRAPHIC_END
	DanmakuGraphicInfo danmakuGraphicInfos[DANMAKU_GRAPHIC_MAX];

	bool debugMode;
	int showingStats;
};

struct SpatialBin {
#define DANMAKU_MAX 8192
	Danmaku *danmaku[DANMAKU_MAX];
	int danmakuNum;
};

struct PatternData {
	int frame;
	int enemy;
	int shotPattern;
	int difficulty;
};

enum Character {
	CHARACTER_DYNAMIC,
	CHARACTER_TURRET,
	CHARACTER_DIRECTION_DRIFT,
	CHARACTER_SPRING,
	CHARACTER_PRETTY_WAVE,
	CHARACTER_MIRROR,
	CHARACTER_FINAL,
};
#define CHARACTERS_MAX (CHARACTER_FINAL-1)
char *characterStrings[] = {
	"Francesca Fiore",
	"Farrow Drupe",
	"Gabrielle Seldery",
	"Mischa Sobaka",
	"Tabitha Pachuli",
	"Penelope Fenn",
};
Vec2 characterSpeeds[] = {
	v2(5.0, 2.0),
	v2(6.5, 3.5),
	v2(4.5, 2.5),
	v2(6.5, 3.5),
	v2(4.5, 2.5),
	v2(5.0, 2.0),
};

struct Game {
	Texture *gameTexture;
	Texture *gameBloomTexture;
	Texture *gameBloomPingPongTexture;
	Framebuffer *gameFramebuffer;

#define PARTICLE_MAX 8192
	Particle particleList[PARTICLE_MAX];
	int particleListNum;

	Vec3 cameraPosition3d;
	Vec3 cameraRotation3d;
	Matrix4 cameraMatrix3d;

#define MAP_PROPS_MAX 1024
	MapProp mapProps[MAP_PROPS_MAX];
	int mapPropsNum;
	float timeTillNextTreeSpawn;
	int topGroundProp;
	int nextMapPropId;
	int groundCount;

	Skeleton *playerSkeleton;

	u32 seed;
	int pausedToCatchUp;

	int prevPhase;
	int phase;
	float phaseTime;

	float currentWheelRads;
	Character selectedCharacter;
	int prevSelectedMenuItem;
	int selectedMenuItem;
	int selectedMenuDifficulty;
	float menuReadyTime;
	bool menuReadyFinal;

	int danmakuWithHpBar;
	float healthBarTime;
	int prevHpBarPhase;
	int hpBarPhase;

	Danmaku danmakuList[DANMAKU_MAX];
	int danmakuListNum;
	int nextDanmakuId;
	int bulletSoundsThisFrame;

#define ITEM_MAX 4096
	Item itemList[ITEM_MAX];
	int itemListNum;

	int lives;
	float invTime;
	int focusLevel;
	int queuedShotFrames;
	int attackLevel;
	int attackPatternLevel;//I dont think this is necessary anymore
	int bombs;
	int bombCooldown;
	int graze;
	Vec2 playerPosition;
	Vec2 playerOptions[3];
	Vec2 playerMoveTrail;
	int mp;
	float playerScale;
	bool hasScreenWrap;
	bool hasBubbleShield;

	int shotAddendum;

	int playerNumber;

	Input prevInput;
	Input *inputs;
	int inputsMax;
	int inputsNum;
	int inputHash;

	float width;
	float height;

	int frameCount;

	int returnFireQuantity;
	int difficulty;

	int binsMax;
	SpatialBin *bins;

#define PATTERNS_MAX 64
	int patterns[PATTERNS_MAX];
	PatternData patternData;
	int patternIndex;
	int patternCount;
	int framesInPattern;

	int spellDisplayTime;
	int spellIndex;
	int spellHoldMp;
	int bossSpellLevel;

	u64 shopFlags;

#define INCOMING_SPELLS_MAX 64
	SpellCast incomingSpells[INCOMING_SPELLS_MAX];
	int incomingSpellsNum;

#define VN_COMMANDS_MAX 512
	VnCommand vnCommands[VN_COMMANDS_MAX];
	int vnCommandsNum;
	int vnCommandPrevIndex;
	int vnCommandIndex;
	float vnCommandTime;
	char vnImagePaths[2][PATH_MAX_LEN];
	int vnShownImage;
	char vnText[VN_TEXT_MAX_LEN];

	bool debugGodMode;
	bool debugShowBulletInfo;
	bool debugShowBinsInfo;
	bool debugLockDifficulty;
	bool debugFreeCamera;
	float debugFreeCameraSpeed;

	char *syncString;
	int syncStringNum;
	int syncStringMax;
};

Game *game = NULL;
Game *realGame = NULL;
Game *realGame2 = NULL;
App *app = NULL;

void runGame();
void updateApp();
void updateGames();
void stepGame(Input *input, int keyboardId, int joyId);
int *getBinsForCircle(Vec2 center, float radius, int *binsNum);
void respawnPlayer();
void playBulletSound();
NetMsg *newOutMsg(NetMsgType type);
void debugDrawDanmakuData(Danmaku *danmaku);
int getClosestEnemy(Vec2 searchPosition);
int getClosestEnemyShot(Vec2 searchPosition);
MapProp *newMapProp(MapPropType type);
MapProp *getMapProp(int id);
/// FUNCTIONS ^

#define __DANMAKU_IMPL
#include "Danmaku.cpp"
#define __SHOPSTUFF_IMPL
#include "shopStuff.cpp"
#include "patterns.cpp"
#define __VN_IMPL
#include "vn.cpp"
//#include "tempPatterns.cpp"

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

	initPlatform(1280, 720, "Bullet hell game");
	platform->sleepWait = true;
	initAudio();
	initRenderer(1280, 720);
	initMesh();
	initModel();
	initSkeleton();
	initFonts();
	initTextureSystem();

	bool good = initNetworking();
	if (!good) Panic("Failed to init networking");

	platformUpdateLoop(updateApp);
}

void updateApp() {
	if (!app) {
		app = (App *)zalloc(sizeof(App));
		logf("App is %.2fmb btw\n", sizeof(App)/(float)Megabytes(1));
		logf("Game is %.2fmb btw\n", sizeof(Game)/(float)Megabytes(1));
		app->defaultFont = createFont("assets/common/arial.ttf", 24);
		app->smallFont = createFont("assets/common/arial.ttf", 10);
		app->bigFont = createFont("assets/common/arial.ttf", 32);
		app->hugeFont = createFont("assets/common/arial.ttf", 96);
		app->uiFont = createFont("assets/fonts/constan.ttf", 12);
		strcpy(app->serverIp, "127.0.0.1");
		RegMem(Globals, playerHitRadius);
		RegMem(Globals, playerFireRate);
		RegMem(Globals, poc);
		RegMem(Globals, bombsPerLife);
		RegMem(Globals, invTimeOnDeath);
		RegMem(Globals, binsWide);
		RegMem(Globals, binsHigh);
		RegMem(Globals, grazePad);
		RegMem(Globals, spellDisplayTime);
		RegMem(Globals, backgroundDistance);
		RegMem(Globals, backgroundSpawnDepth);
		RegMem(Globals, backgroundFrontClip);
		RegMem(Globals, cameraFov);
		RegMem(Globals, backgroundSpeed);
		RegMem(Globals, backgroundTreeSpread);
		RegMem(Globals, treeSpawnTimeMinMax);
		RegMem(Globals, cameraPosition);
		RegMem(Globals, cameraRotation);
		RegMem(Globals, sunPosition);
		RegMem(Globals, ambientLight);
		RegMem(Globals, diffuseLight);
		RegMem(Globals, specularLight);
		RegMem(Globals, showSyncString);
		RegMem(Globals, forestGroundFreq);
		RegMem(Globals, forestGroundDepth);
		RegMem(Globals, forestGroundMulti);

		loadStruct("Globals", "assets/info/globals.txt", &app->globals);

		for (int i = 0; i < DANMAKU_GRAPHIC_MAX; i++) {
			DanmakuGraphicInfo *info = &app->danmakuGraphicInfos[i];
			char *path = frameSprintf("assets/images/bullets/%s.png", danmakuGraphicStrings[i]);
			Texture *texture = getTexture(path);
			info->size.x = texture->width;
			info->size.y = texture->height;
			float smallestRadius = min(info->size.x, info->size.y);
			info->radius = 8 + max(3, floor((smallestRadius - 12) / 3));
			info->bulletToBulletRadius = 8 + smallestRadius / 2;
			info->textureRadius = max(info->size.x, info->size.y);
			info->path = stringClone(path);
			info->name = danmakuGraphicStrings[i];
		}
		app->danmakuGraphicInfos[KOUMA].radius = 5;
		app->danmakuGraphicInfos[YOUMU].radius = 5;
		app->danmakuGraphicInfos[YINYANG_L].radius = 48;
		app->danmakuGraphicInfos[YINYANG_L].bulletToBulletRadius = 64;

		danmakuInitSys();
		fillUniversalShop(app->shopList, &app->shopListNum);

		app->startingSeed = rndInt(0, 99999);

		app->streamedInputsMax = 8;
		app->streamedInputs = (Input *)malloc(sizeof(Input) * app->streamedInputsMax);

		// app->cameraRotation3d.x = -1.19;

#if INSTANT_START_PLAYING_LOCAL
		app->playMode = PLAY_LOCAL;
		app->state = APP_PLAYING;
#endif

#if INSTANT_START_TRAINING
		app->playMode = PLAY_TRAINING;
		app->state = APP_PLAYING;
#endif
	}

	Globals *globals = &app->globals;
	float elapsed = platform->elapsed;
	float secondPhase = timePhase(platform->time);

	clearRenderer();

	{ /// App state
		if (app->prevState != app->state) {
			app->prevState = app->state;
			app->stateTime = 0;
		}

		if (app->state == APP_MENU) {
			ImGui::SetNextWindowPos(ImVec2(platform->windowWidth/2, platform->windowHeight/2), ImGuiCond_Once, ImVec2(0.5, 0.5));
			ImGui::Begin("Main menu", NULL, ImGuiWindowFlags_AlwaysAutoResize);

			{
				ImGui::Text("Build time: %s %s\n", __DATE__, __TIME__);
			}

			if (!app->socket) {
				ImGui::InputText("Server ip", app->serverIp, 32);
				if (ImGui::Button("192.168.2.168")) strcpy(app->serverIp, "192.168.2.168");
				ImGui::SameLine();
				if (ImGui::Button("127.0.0.1")) strcpy(app->serverIp, "127.0.0.1");
				ImGui::SameLine();

				if (ImGui::Button("184.72.9.156")) strcpy(app->serverIp, "184.72.9.156");

				bool forceConnect = false;
#if INSTANT_START_CONNECTING
				forceConnect = true;
#endif

				if (ImGui::Button("Connect") || forceConnect) {
					int port = 43594;
					app->serverAddress = createSockaddr(app->serverIp, port);
					app->socket = createTcpSocket(sizeof(NetMsg));
					if (!app->socket) {
						logf("Failed to create tcp socket for login\n");
						logLastOSError();
					}

					if (!socketConnect(app->socket, app->serverAddress)) {
						logf("Failed to connect tcp socket for login\n");
						socketClose(app->socket);
						app->socket = NULL;
						logLastOSError();
					}

					if (app->socket && !socketSetNonblock(app->socket)) {
						logf("Failed to make tcp socket nonblocking for login\n");

						socketClose(app->socket);
						app->socket = NULL;
						logLastOSError();
					}

					newOutMsg(NET_MSG_CtS_CONNECT);
					logf("Client connected to %s:%d\n", getSocketIp(app->socket), getSocketPort(app->socket));
				}
			}

			if (ImGui::Button("Play locally")) {
				app->playMode = PLAY_LOCAL;
				app->state = APP_PLAYING;
			}

			if (ImGui::Button("Training")) {
				app->playMode = PLAY_TRAINING;
				app->state = APP_PLAYING;
			}
			ImGui::End();
		} else if (app->state == APP_IN_QUEUE) {
			ImGui::SetNextWindowPos(ImVec2(platform->windowWidth/2, platform->windowHeight/2), ImGuiCond_Once, ImVec2(0.5, 0.5));
			ImGui::Begin("In queue", NULL, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::Text("You're in queue, please wait...\n");
			ImGui::End();
		} else if (app->state == APP_PLAYING) {
			if (app->stateTime == 0) {
				if (realGame) { //@cleanup Make this an array
					free(realGame->inputs);
					free(realGame);
					realGame = NULL;
				}
				if (realGame2) {
					free(realGame2->inputs);
					free(realGame2);
					realGame2 = NULL;
				}
			}
			updateGames();
		}
	}

	if (app->socket) { /// Receive
		for (;;) {
			if (app->debugLagSwitch) break;
			SocketlingStatus status = socketReceive(app->socket);
			if (status == SOCKETLING_ZERO_RECEIVE) {
				logf("The login server was disconnected\n");
				// exit(0);
			} else if (status == SOCKETLING_CONNECTION_RESET) {
				logf("The login server was destroyed\n");
				// exit(0);
			} else if (status == SOCKETLING_WOULD_BLOCK) {
				// Nothing...
			} else if (status == SOCKETLING_PENDING) {
				// Nothing...
			} else if (status == SOCKETLING_ERROR) {
				// if (lastSocketErrorWasConnReset()) {
				// 	logf("Server was ECONNRESET\n");
				// 	exit(0);
				// }
				logLastOSError();
			} else if (status == SOCKETLING_GOOD) {
				// logf("Good\n");
				app->timeSinceLastSeenServer = 0;

				NetMsg *msg = (NetMsg *)app->socket->receiveBuffer;
				if (msg->type == NET_MSG_CtC_INPUTS) {
					int newSize = app->streamedInputsNum + NET_MSG_INPUTS_MAX;
					if (newSize > app->streamedInputsMax) {
						app->streamedInputs = (Input *)resizeArray(app->streamedInputs, sizeof(Input), app->streamedInputsMax, newSize);
						app->streamedInputsMax = newSize;
					}

					int inputsSoFar = realGame2->frameCount + app->streamedInputsNum + NET_MSG_INPUTS_MAX;
					if (inputsSoFar != msg->unionInputs.inputsTotal) logf(
						"%d: No match %d %d\n",
						app->clientId,
						inputsSoFar,
						msg->unionInputs.inputsTotal
					);

					for (int i = 0; i < NET_MSG_INPUTS_MAX; i++) {
						Input *input = &msg->unionInputs.inputs[i];
						app->streamInputHash = incInputHash(app->streamInputHash, input, app->streamInputCount);

						char *str = frameSprintf(
							"%d(%d) | %d%d%d%d%d%d%d%d\n",
							app->streamInputCount,
							app->streamInputHash,
							input->up,
							input->down,
							input->left,
							input->right,
							input->focus,
							input->shoot,
							input->bomb,
							input->extra
						);
						app->streamInputCount++;
						if (!app->streamSyncString) {
							app->streamSyncStringMax = 1024;
							app->streamSyncStringNum = 0;
							app->streamSyncString = (char *)malloc(sizeof(char) * app->streamSyncStringMax);
							app->streamSyncString[0] = 0;
						}

						int len = strlen(str)+1;
						if (app->streamSyncStringNum + len > app->streamSyncStringMax-1) {
							app->streamSyncString = (char *)resizeArray(app->streamSyncString, sizeof(char), app->streamSyncStringMax, app->streamSyncStringMax * 1.5);
							app->streamSyncStringMax *= 1.5;
						}

						strcat(app->streamSyncString, str);
						app->streamSyncStringNum += len;

						int lineCount = countChar(app->streamSyncString, '\n');
						if (lineCount >= 600) {
#if SAVES_FROM_LOG
							appendFile(
								frameSprintf("assets/logs/inputsFrom%d.txt", app->enemyClientId),
								app->streamSyncString,
								strlen(app->streamSyncString)
							);
#endif
							free(app->streamSyncString);
							app->streamSyncString = NULL;
						}

						app->streamedInputs[app->streamedInputsNum++] = *input;
					}
				}

				if (msg->type == NET_MSG_PING) {
					// spliceMsg = true;
				}

				if (msg->type == NET_MSG_StC_CONNECT) {
					app->clientId = msg->id;
					logf("CLIENT: token is: %d\n", app->clientId);
				}

				if (msg->type == NET_MSG_StC_IN_QUEUE) {
					app->state = APP_IN_QUEUE;
				}

				if (msg->type == NET_MSG_StC_START_GAME) {
					app->playMode = PLAY_ONLINE;
					app->state = APP_PLAYING;
					app->enemyClientId = msg->unionStartGame.enemyClientId;
					app->startingSeed = msg->unionStartGame.seed;
				}

				if (msg->type == NET_MSG_CtC_SPELL_CAST) {
					Game *game = realGame;
					game = realGame;
					game = realGame;

					if (game->incomingSpellsNum < INCOMING_SPELLS_MAX-1) {
						SpellCast *cast = &game->incomingSpells[game->incomingSpellsNum++];
						memcpy(cast, &msg->unionSpellCast.cast, sizeof(SpellCast));
						if (cast->frame < game->frameCount) {
							logf("Spell too late! Desynced for sure 3:\n");
						}
					} else {
						logf("Too many spells, it's gone now\n");
					}
				}
			} else {
				logf("Unknown socketling status %d\n", status);
			}

			if (status != SOCKETLING_GOOD) break;
		}
	}

	if (app->socket) { /// Send
		for (int i = 0; i < app->outMsgsNum; i++) {
			if (app->debugLagSwitch) break;

			NetMsg *msg = &app->outMsgs[i];
			for (;;) {

				SocketlingStatus status = socketSend(app->socket, msg, app->serverAddress);
				if (status == SOCKETLING_WOULD_BLOCK || status == SOCKETLING_PENDING) {
					logf("Trying to send msg\n");
				} else if (status == SOCKETLING_ERROR) {
					logLastOSError();
				} else if (status == SOCKETLING_CONNECTION_RESET) {
					// logf("The server was destroyed\n");
					// exit(0);
				} else if (status == SOCKETLING_GOOD) {
					app->timeSinceLastPingedServer = 0;
					break;
				} else {
					logf("Unknown socketling status %d\n", status);
				}
			}
		}
		if (!app->debugLagSwitch) app->outMsgsNum = 0;
	}

	{ /// App debug
		if (keyJustPressed(KEY_BACKTICK)) app->debugMode = !app->debugMode;
		if (app->playMode == PLAY_ONLINE && app->clientId != 0) {
			app->timeSinceLastSeenServer += platform->elapsed;
			app->timeSinceLastPingedServer += platform->elapsed;

			if (app->timeSinceLastPingedServer > 1) {
				app->timeSinceLastPingedServer = 0;
				newOutMsg(NET_MSG_PING);
			}

			if (app->timeSinceLastSeenServer > 60) {
				app->state = APP_MENU;

				app->playMode = PLAY_NONE;
				if (app->socket) socketClose(app->socket);
				app->socket = NULL;
			}
		}

		if (app->debugLagSwitch > 0) app->debugLagSwitch--;
		if (keyPressed(KEY_CTRL) && keyJustPressed('L')) {
			if (keyPressed(KEY_SHIFT)) {
				platformSleep(3000);
			} else {
				logf("Lag switch for 60 frames\n");
				app->debugLagSwitch += 60;
			}
		}

		if (keyPressed(KEY_CTRL) && keyJustPressed('B')) {
			Game *games[2];
			int gamesNum = 0;
			if (realGame) games[gamesNum++] = realGame;
			if (realGame2) games[gamesNum++] = realGame2;
			Game *oldGame = game;
			for (int i = 0; i < gamesNum; i++) {
				game = games[i];
				for (int i = 0; i < DANMAKU_MAX; i++) {
					Vec2 pos = v2(rndFloat(0, game->width), rndFloat(0, game->height));
					Danmaku *danmaku = createShotA(
						pos,
						rndFloat(1, 5),
						rndFloat(0, 360),
						(DanmakuGraphic)rndInt(0, DANMAKU_GRAPHIC_MAX-1),
						(ColorName)rndInt(0, MAGENTA),
						0
					);
				}
			}
			game = oldGame;
		}

		if (keyPressed(KEY_CTRL) && keyJustPressed('F')) {
			app->showingStats++;
			if (app->showingStats > 2) app->showingStats = 0;
		}

		if (app->showingStats) {
			int bullets = 0;
			if (realGame) bullets += realGame->danmakuListNum;
			if (realGame2) bullets += realGame2->danmakuListNum;

			char *str = NULL;
			if (app->showingStats == 1) {
				str = frameSprintf(
					"%.1fms (%dms)\n%d bullets",
					platform->frameTimeAvg,
					platform->frameTime,
					bullets
				);
			} else if (app->showingStats == 2) { 
				str = frameSprintf(
					"%.1fms (%dms)\n%d bullets\n%d outMsgs\ninStream: %d/%d",
					platform->frameTimeAvg,
					platform->frameTime,
					bullets,
					app->outMsgsNum,
					app->streamedInputsNum,
					app->streamedInputsMax
				);
			}

			drawText(app->defaultFont, str, v2(), 0xFFFFFFFF);
		}
	}

	drawOnScreenLog();
	app->stateTime += elapsed;
}

void updateGames() {
	Globals *globals = &app->globals;

	if (app->debugMode && realGame) { /// Games debug
		if (keyJustPressed(' ')) realGame->debugFreeCamera = !realGame->debugFreeCamera;

		ImGui::Begin("Debug", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Checkbox("Free camera", &realGame->debugFreeCamera);
		platform->useRelativeMouse = realGame->debugFreeCamera;

		ImGui::Text("Frame time: %f (%f fps)", platform->frameTimeAvg, 1.0 / (platform->frameTimeAvg / 1000.0));
		ImGui::Text("Danmaku num: %d\n", realGame->danmakuListNum + realGame2->danmakuListNum);
		ImGui::Text("Items num: %d\n", realGame->itemListNum + realGame2->itemListNum);
		ImGui::Text("Particle num: %d\n", realGame->particleListNum + realGame2->particleListNum);
		ImGui::Text("Map props num: %d\n", realGame->mapPropsNum + realGame2->mapPropsNum);

		if (ImGui::TreeNode("Globals")) {
			if (ImGui::Button("Save globals")) saveStruct("Globals", "assets/info/globals.txt", globals);
			ImGui::SameLine();
			if (ImGui::Button("Load globals")) loadStruct("Globals", "assets/info/globals.txt", globals);

			ImGui::SliderFloat("playerHitRadius", &globals->playerHitRadius, 0.01, 10);
			ImGui::InputInt("playerFireRate", &globals->playerFireRate);
			ImGui::InputInt("pocLine", &globals->poc);
			ImGui::InputInt("bombsPerLife", &globals->bombsPerLife);
			ImGui::InputFloat("invTimeOnDeath", &globals->invTimeOnDeath);

			bool binsChanged = false;
			binsChanged = binsChanged || ImGui::SliderInt("binsWide", &globals->binsWide, 10, 50);
			binsChanged = binsChanged || ImGui::SliderInt("binsHigh", &globals->binsHigh, 10, 50);
			if (binsChanged) {
				free(realGame->bins);
				realGame->bins = NULL;
				free(realGame2->bins);
				realGame2->bins = NULL;
			}
			ImGui::Text("Bins are %.2fmb per player", (globals->binsWide * globals->binsHigh * sizeof(SpatialBin))/(float)Megabytes(1));

			ImGui::InputInt("grazePad", &globals->grazePad);
			ImGui::InputInt("spellDisplayTime", &globals->spellDisplayTime);

			ImGui::Separator();
			ImGui::DragFloat("Background distance", &globals->backgroundDistance, 0.1);
			ImGui::DragFloat("Background spawn depth", &globals->backgroundSpawnDepth, 0.1);
			ImGui::DragFloat("Background front clip", &globals->backgroundFrontClip, 0.1);
			ImGui::DragFloat("Camera fov", &globals->cameraFov, 0.1);
			ImGui::DragFloat("Background speed", &globals->backgroundSpeed, 0.1);
			ImGui::DragFloat("Background tree spread", &globals->backgroundTreeSpread, 0.1);
			ImGui::DragFloat2("Tree spawn time min/max", &globals->treeSpawnTimeMinMax.x, 0.1);
			ImGui::DragFloat("forestGroundFreq", &globals->forestGroundFreq, 0.01);
			ImGui::SliderInt("forestGroundDepth", &globals->forestGroundDepth, 1, 10);
			ImGui::DragFloat("forestGroundMulti", &globals->forestGroundMulti, 0.01);

			ImGui::Separator();

			ImGui::DragFloat3("Camera position", &globals->cameraPosition.x, 0.1);
			ImGui::DragFloat3("Camera rotation", &globals->cameraRotation.x, 0.01);
			ImGui::DragFloat3("sunPosition", &globals->sunPosition.x, 0.1);
			ImGui::DragFloat3("ambientLight", &globals->ambientLight.x, 0.1);
			ImGui::DragFloat3("diffuseLight", &globals->diffuseLight.x, 0.1);
			ImGui::DragFloat3("specularLight", &globals->specularLight.x, 0.1);

			ImGui::Separator();

			ImGui::Checkbox("showSyncString", &globals->showSyncString);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("realGame")) {
			ImGui::Checkbox("debugShowBulletInfo", &realGame->debugShowBulletInfo);
			ImGui::Checkbox("debugShowBinsInfo", &realGame->debugShowBinsInfo);
			ImGui::Checkbox("debugStopFrameAdvance", &app->debugStopFrameAdvance);
			ImGui::SameLine();
			app->debugStepFrameAdvance =  ImGui::Button("Step");

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Danmaku graphics")) {
			for (int i = 0; i < DANMAKU_GRAPHIC_MAX; i++) {
				DanmakuGraphicInfo *info = &app->danmakuGraphicInfos[i];

				if (ImGui::TreeNode(frameSprintf("%s###%d", info->name, i))) {
					ImGui::Text("Path: %s", info->path);
					ImGui::Text("Size: %f %f", info->size.x, info->size.y);
					ImGui::Text("Radius: %f", info->radius);
					ImGui::Text("Bullet to bullet radius: %f", info->bulletToBulletRadius);
					ImGui::Text("Texture radius: %f", info->textureRadius);

					ImGui::Text("Texture:");
					Texture *texture = getTexture(info->path);
					if (texture) {
						guiTexture(texture);
					} else {
						ImGui::Text("Texture missing");
					}
					ImGui::TreePop();
				}
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Models")) {
			showModelsGui();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Meshes")) {
			showMeshGui();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("ImGui demo")) {
			ImGui::ShowDemoWindow();
			ImGui::TreePop();
		}

		ImGui::End();
	}

	if (!realGame) {
		realGame = (Game *)zalloc(sizeof(Game));
		realGame->seed = app->startingSeed;
		realGame->playerNumber = 1;

		realGame->inputsMax = NET_MSG_INPUTS_MAX;
		realGame->inputs = (Input *)malloc(sizeof(Input) * realGame->inputsMax);
	}

	if (!realGame2) {
		realGame2 = (Game *)zalloc(sizeof(Game));
		realGame2->playerNumber = 2;
		realGame2->seed = app->startingSeed;

		realGame2->inputsMax = NET_MSG_INPUTS_MAX;
		realGame2->inputs = (Input *)malloc(sizeof(Input) * realGame2->inputsMax);
	}

	bool doFrame = true;
	if (app->debugStopFrameAdvance && !app->debugStepFrameAdvance) doFrame = false;

	if (doFrame) {
		bool player1Paused = false;
		if (realGame->frameCount - realGame2->frameCount > NET_MSG_INPUTS_MAX*2) {
			// player1Paused = true;
		}

		if (!player1Paused) { //P1
			game = realGame;

			Input input = {};
			stepGame(&input, 1, 1);

			if (app->playMode == PLAY_ONLINE) {
				game->inputs[game->inputsNum++] = input;
				if (game->inputsNum == NET_MSG_INPUTS_MAX) { /// Turn is over, send inputs
					NetMsg *msg = newOutMsg(NET_MSG_CtC_INPUTS);
					memcpy(msg->unionInputs.inputs, game->inputs, sizeof(Input) * NET_MSG_INPUTS_MAX);
					msg->unionInputs.inputsTotal = game->frameCount;
					game->inputsNum = 0;
				}
			}
		} else {
			char *str = frameSprintf("Network lag!");
			Vec2 size = getTextSize(app->bigFont, str);
			Vec2 pos;
			pos.x = realGame->width/2 - size.x/2;
			pos.y = realGame->height/2 - size.y/2;
			drawText(app->bigFont, str, pos, 0xFFFFFFFF);
		}

		{ //P2
			game = realGame2;

			if (app->playMode == PLAY_ONLINE) {
				int steps = 1;
				// if (realGame->frameCount - realGame2->frameCount > NET_MSG_INPUTS_MAX && app->streamedInputsNum > NET_MSG_INPUTS_MAX) steps = 2;
				for (int i = 0; i < steps; i++) {
					if (app->streamedInputsNum > 0) {
						Input input = app->streamedInputs[0];
						arraySpliceIndex(app->streamedInputs, app->streamedInputsNum, sizeof(Input), 0);
						app->streamedInputsNum--;
						stepGame(&input, 0, 0);
					} else {
						//@incomplete Network is paused, should probably display a message or something
					}
				}
			} else {
				Input input = {};
				stepGame(&input, 2, 2);
			}
		}
	}

	Game *games[2] = {realGame, realGame2};
	for (int i = 0; i < 2; i++) {
		Game *game = games[i];
		Matrix3 matrix = mat3();
		if (game->playerNumber == 2) matrix.TRANSLATE(game->width, 0);

		if (game->gameTexture) {
			matrix.SCALE(game->gameTexture->width, game->gameTexture->height);
			drawPost3dTexture(game->gameTexture, matrix);

			{ /// Bloom
				Matrix3 matrix = mat3();
				if (game->playerNumber == 2) matrix.TRANSLATE(game->width, 0);
				matrix.SCALE(game->gameBloomTexture->width, game->gameBloomTexture->height);
				drawBlurred(game->gameBloomTexture, matrix, true);
				drawBlurred(game->gameBloomTexture, matrix, false);
			}
		}
	}
}

void stepGame(Input *input, int keyboardId, int joyId) {
	pushRndSeed(game->seed);
	Globals *globals = &app->globals;
	float elapsed = platform->elapsed;
	float secondPhase = timePhase(platform->time);

	game->width = 1280/2;
	game->height = 720;
	if (!game->gameTexture) game->gameTexture = createTexture(game->width, game->height);
	if (!game->gameBloomTexture) game->gameBloomTexture = createTexture(game->width, game->height);

	if (!game->gameFramebuffer) {
		game->gameFramebuffer = createFramebuffer();
		setFramebuffer(game->gameFramebuffer);
		setColorAttachment(game->gameFramebuffer, game->gameTexture, 0);
		setColorAttachment(game->gameFramebuffer, game->gameBloomTexture, 1);
		addDepthAttachment(game->gameTexture->width, game->gameTexture->height);
	}

	setFramebuffer(game->gameFramebuffer);
	clearRenderer();
	drawRect(0, 0, game->gameTexture->width, game->gameTexture->height, 0xFF808080); //@todo Why does this need to be here?
	processBatchDraws();

	{ /// Input
		if (keyboardId == 1) {
			if (keyPressed(KEY_UP)) input->up = true;
			if (keyPressed(KEY_DOWN)) input->down = true;
			if (keyPressed(KEY_LEFT)) input->left = true;
			if (keyPressed(KEY_RIGHT)) input->right = true;
			if (keyPressed(KEY_SHIFT)) input->focus = true;
			if (keyPressed('Z')) input->shoot = true;
			if (keyPressed('X')) input->bomb = true;
			if (keyPressed('C')) input->extra = true;
		} else if (keyboardId == 2) {
			if (keyPressed('W')) input->up = true;
			if (keyPressed('S')) input->down = true;
			if (keyPressed('A')) input->left = true;
			if (keyPressed('D')) input->right = true;
			if (keyPressed('[')) input->focus = true;
			if (keyPressed(']')) input->shoot = true;
			if (keyPressed('\\')) input->bomb = true;
			if (keyPressed('P')) input->extra = true;
		}

		if (joyId > 0) {
			Vec2 leftStick = joyLeftStick(joyId);
			if (joyButtonPressed(joyId, 11) || leftStick.y < -0.24) input->up = true;
			if (joyButtonPressed(joyId, 12) || leftStick.y > 0.24) input->down = true;
			if (joyButtonPressed(joyId, 13) || leftStick.x < -0.24) input->left = true;
			if (joyButtonPressed(joyId, 14) || leftStick.x > 0.24) input->right = true;
			if (joyButtonPressed(joyId, 10)) input->focus = true;
			if (joyButtonPressed(joyId, 0)) input->shoot = true;
			if (joyButtonPressed(joyId, 1)) input->bomb = true;
			if (joyButtonPressed(joyId, 2)) input->extra = true;
		}

		if (game->playerNumber == 1) {
			if (keyPressed(KEY_CTRL) && keyJustPressed('R')) app->debugRandomMovement = !app->debugRandomMovement;
			if (app->debugRandomMovement) {
				pushRndSeed(app->debugRandomMovementSeed);
				input->up = rndBool();
				input->down = rndBool();
				input->left = rndBool();
				input->focus = rndBool();
				input->right = rndBool();
				input->shoot = rndBool();
				input->extra = rndBool();
				app->debugRandomMovementSeed = popRndSeed();
			}
		}

		if (game->frameCount < NET_MSG_INPUTS_MAX) memset(input, 0, sizeof(Input));

		game->inputHash = incInputHash(game->inputHash, input, game->frameCount);
	}

	{ /// 3d draw
		{ /// Camera
			if (game->debugFreeCamera) {
				Vec3 moveVec = v3();
				if (keyPressed('W')) moveVec.z--;
				if (keyPressed('S')) moveVec.z++;
				if (keyPressed('A')) moveVec.x--;
				if (keyPressed('D')) moveVec.x++;
				if (keyPressed(KEY_SHIFT)) moveVec.y++;
				if (keyPressed(KEY_CTRL)) moveVec.y--;

				moveVec *= game->debugFreeCameraSpeed;
				moveVec = game->cameraMatrix3d.multiplyAffine(moveVec);
				game->cameraPosition3d += moveVec;

				float sensitivity = 1;
				game->cameraRotation3d.z += -platform->relativeMouse.x * sensitivity*0.001;
				game->cameraRotation3d.x += -platform->relativeMouse.y * sensitivity*0.001;
				if (game->cameraRotation3d.x < 0) game->cameraRotation3d.x = 0;
				if (game->cameraRotation3d.x > M_PI) game->cameraRotation3d.x = M_PI;

				if (platform->mouseWheel < 0) {
					game->debugFreeCameraSpeed -= 0.01;
					logf("Speed: %f\n", game->debugFreeCameraSpeed);
				}

				if (platform->mouseWheel > 0) {
					game->debugFreeCameraSpeed += 0.01;
					logf("Speed: %f\n", game->debugFreeCameraSpeed);
				}
				if (game->debugFreeCameraSpeed < 0.01) game->debugFreeCameraSpeed = 0.01;
			} else {
				game->cameraPosition3d = globals->cameraPosition;
				game->cameraRotation3d = globals->cameraRotation;
			}

			WorldProps *world = defaultWorld;
			world->sunPosition = globals->sunPosition;
			world->ambientLight = globals->ambientLight;
			world->diffuseLight = globals->diffuseLight;
			world->specularLight = globals->specularLight;

			game->cameraMatrix3d = mat4();
			game->cameraMatrix3d.TRANSLATE(game->cameraPosition3d);
			game->cameraMatrix3d.ROTATE_EULER(game->cameraRotation3d);

			world->viewMatrix = game->cameraMatrix3d.invert();
			world->projectionMatrix = getPerspectiveMatrix(globals->cameraFov, game->width/(float)game->height, 0.1, FAR_PLANE);
		}

		{ /// Map props
			float groundSize = 40;
			float freq = globals->forestGroundFreq;
			int depth = globals->forestGroundDepth;
			float multi = globals->forestGroundMulti;

			MapProp *topGroundProp = getMapProp(game->topGroundProp);
			float groundY = 0;
			if (topGroundProp) {
				groundY = -(globals->backgroundSpawnDepth - (topGroundProp->position.y + groundSize/2));
				groundY /= groundSize;
				groundY = 1.0 - groundY;
				// logf("%f\n", groundY);
			}

			{ /// Background
				game->timeTillNextTreeSpawn -= elapsed;
				if (game->timeTillNextTreeSpawn < 0) {
					MapProp *mapProp = newMapProp(PROP_TREE);
					float treeX = rndFloat(0, groundSize);
					float height = perlin2d(treeX/groundSize, groundY + game->groundCount, freq, depth);
#if DO_NEW_TREES
					strcpy(mapProp->modelPath, "assets/models/Trees/Tree1.model");
#else
					strcpy(mapProp->modelPath, "assets/models/tree/Tree.model");
#endif

					Model *model = getModel(mapProp->modelPath);

					if (mapProp) {
						mapProp->position = v3(
							treeX - groundSize/2,
							globals->backgroundSpawnDepth + getSize(model->bounds).y/2,
							-(globals->backgroundDistance - height*multi)
						);

						mapProp->scale = v3(1, 1, 1) * rndFloat(0.9, 1.1);
						mapProp->rotation.z = rndFloat(0, 1) * 2*M_PI;
					}

					game->timeTillNextTreeSpawn = rndFloat(globals->treeSpawnTimeMinMax.x, globals->treeSpawnTimeMinMax.y);
				}

				if (game->frameCount % 2 == 0) {
					MapProp *mapProp = newMapProp(PROP_ROCK);
					float x = rndFloat(0, groundSize);
					float height = perlin2d(x/groundSize, groundY + game->groundCount, freq, depth);
					char *path = frameSprintf("assets/models/Rocks/Rock%d.model", rndInt(1, 2));
					strcpy(mapProp->modelPath, path);

					Model *model = getModel(mapProp->modelPath);

					if (mapProp) {
						mapProp->position = v3(
							x - groundSize/2,
							globals->backgroundSpawnDepth + getSize(model->bounds).y/2,
							-(globals->backgroundDistance - height*multi)
						);

						mapProp->scale = v3(1, 1, 1) * rndFloat(0.9, 1.1);
						mapProp->rotation.z = rndFloat(0, 1) * 2*M_PI;
					}

				}

				bool spawnNewGround = false;
				if (!topGroundProp) {
					spawnNewGround = true;
				} else {
					if (topGroundProp->position.y + getSize(topGroundProp->customMesh->bounds).y/2 < globals->backgroundSpawnDepth) spawnNewGround = true;
				}

				if (spawnNewGround) {
					game->groundCount++;

					MapProp *mapProp = newMapProp(PROP_TERRAIN);
					float off = 0;
					if (topGroundProp) off = globals->backgroundSpawnDepth - (topGroundProp->position.y + groundSize/2);

					game->topGroundProp = mapProp->id;

					int res = 30;

					Vec3 *verts = (Vec3 *)frameMalloc(sizeof(Vec3) * res*res);
					int vertsNum = 0;

					Vec3 *normals = (Vec3 *)frameMalloc(sizeof(Vec3) * res*res);
					int normalsNum = 0;

					Vec2 *uvs = (Vec2 *)frameMalloc(sizeof(Vec2) * res*res);
					int uvsNum = 0;

					u16 *inds = (u16 *)frameMalloc(sizeof(u16) * (res-1) * (res-1) * 6);
					int indsNum = 0;

					float step = 1.0/(res-1.0);

					for (int y = 0; y < res; y++) {
						for (int x = 0; x < res; x++) {
							Vec2 fract = v2(x*step, y*step);

							float up = perlin2d(fract.x, fract.y + step + game->groundCount, freq, depth);
							float down = perlin2d(fract.x, fract.y - step + game->groundCount, freq, depth);
							float left = perlin2d(fract.x - step, fract.y + game->groundCount, freq, depth);
							float right = perlin2d(fract.x + step, fract.y + game->groundCount, freq, depth);
							float noise = perlin2d(fract.x, fract.y + game->groundCount, freq, depth);

							verts[vertsNum++] = v3(fract.x, fract.y, noise * multi);
							//uvs[uvsNum++] = v2(x%2, y%2);
							uvs[uvsNum++] = fract;
							normals[normalsNum++] = v3(2*(right-left), 2*(down-up), -4).normalize();
						}
					}

					int triCount = 0;
					for (int i = 0; i < (res-1)*(res-1); i++) {
						if (triCount % res == res-1) triCount++;
						inds[indsNum++] = 1 + triCount;
						inds[indsNum++] = 0 + triCount;
						inds[indsNum++] = res + triCount;
						inds[indsNum++] = res + triCount;
						inds[indsNum++] = res+1 + triCount;
						inds[indsNum++] = 1 + triCount;
						triCount++;
					}

					for (int i = 0; i < vertsNum; i++) {
						Vec3 vert = verts[i];
						vert.x -= 0.5;
						vert.y -= 0.5;
						vert.x *= groundSize;
						vert.y *= groundSize;
						verts[i] = vert;
					}

					mapProp->customMesh = createCustomMesh(verts, uvs, normals, vertsNum, inds, indsNum, MATERIAL_TERRAIN_MAP, "assets/images/grassTexture.png", true);

					mapProp->position = v3(
						0,
						globals->backgroundSpawnDepth + getSize(mapProp->customMesh->bounds).y/2 - off,
						-globals->backgroundDistance
					);
				}

				drawSphere(defaultWorld->sunPosition, 1, 0xFFFFFF00);
			}

			for (int i = 0; i < game->mapPropsNum; i++) {
				MapProp *mapProp = &game->mapProps[i];
				mapProp->position.y -= globals->backgroundSpeed;

				Matrix4 mat = mat4();
				mat.TRANSLATE(mapProp->position);
				mat.ROTATE_EULER(mapProp->rotation);
				mat.SCALE(mapProp->scale);

				AABB bounds = {};

				if (mapProp->type == PROP_TERRAIN) {
					MeshProps props = newMeshProps();
					props.matrix = mat;
					props.slopeTexture = getTexture("assets/images/rockTexture.png");
					props.diffuseLight = v3(1.5, 1.5, 1.5);
					drawMesh(mapProp->customMesh, props);

					bounds = mapProp->customMesh->bounds;
				} else if (mapProp->type == PROP_TREE || mapProp->type == PROP_ROCK) {
					Model *model = getModel(mapProp->modelPath);
					ModelProps props = newModelProps();
					props.specularLight = v3(0.1, 0.1, 0.1);
					props.diffuseLight = v3(0.4, 0.4, 0.4);
					props.ambientLight = v3(0.2, 0.2, 0.2);
					props.matrix = mat;
					// drawModel(model, props);

					bounds = model->bounds;
				}

				if (isZero(bounds)) {
					logf("Map prop type %d didn't set bounds\n", mapProp->type);
				}

				// drawSphere(mapProp->position, 1, 0xFFFFFFFF);

				if (mapProp->position.y + getSize(bounds).y/2 < globals->backgroundFrontClip) {
					arraySpliceIndex(game->mapProps, game->mapPropsNum, sizeof(MapProp), i);
					game->mapPropsNum--;
					i--;
					continue;
				}
			}

			{
				Vec3 start = v3();
				start.y = globals->backgroundSpawnDepth;
				start.x = -groundSize;
				start.z = lerp(-5, 5, secondPhase);

				Vec3 end = start;
				end.x = groundSize;
				drawBeam(start, end, 1, 0xFFFFFFFF);
			}

			{
				Vec3 start = v3();
				start.y = globals->backgroundFrontClip;
				start.x = -groundSize;
				start.z = lerp(-5, 5, secondPhase);

				Vec3 end = start;
				end.x = groundSize;
				drawBeam(start, end, 1, 0xFFFFFFFF);
			}

#if 0 /// Frustum stuff
			Vec3 cube[8] = {
				v3(-1, -1, 1),
				v3(-1, 1, 1),
				v3(1, 1, 1),
				v3(1, -1, 1),
				v3(-1, -1, -1),
				v3(-1, 1, -1),
				v3(1, 1, -1),
				v3(1, -1, -1),
			};

			static Matrix4 frust = mat4();
			if (game->playerNumber == 1 && keyJustPressed(' ')) {
				logf("Captured\n");
				frust = defaultWorld->projectionMatrix * defaultWorld->viewMatrix;
			}

			for (int i = 0; i < 8; i++) {
				cube[i] = frust.invert() * cube[i];
			}

			Line3 cubeLines[12];
			cubeLines[0].start = cube[0];
			cubeLines[0].end = cube[1];
			cubeLines[1].start = cube[1];
			cubeLines[1].end = cube[2];
			cubeLines[2].start = cube[2];
			cubeLines[2].end = cube[3];
			cubeLines[3].start = cube[3];
			cubeLines[3].end = cube[0];
			cubeLines[4].start = cube[4];
			cubeLines[4].end = cube[5];
			cubeLines[5].start = cube[5];
			cubeLines[5].end = cube[6];
			cubeLines[6].start = cube[6];
			cubeLines[6].end = cube[7];
			cubeLines[7].start = cube[7];
			cubeLines[7].end = cube[4];
			cubeLines[8].start = cube[0];
			cubeLines[8].end = cube[4];
			cubeLines[9].start = cube[1];
			cubeLines[9].end = cube[5];
			cubeLines[10].start = cube[2];
			cubeLines[10].end = cube[6];
			cubeLines[11].start = cube[3];
			cubeLines[11].end = cube[7];

			for (int i = 0; i < 12; i++) {
				Line3 line = cubeLines[i];
				drawBeam(line.start, line.end, 0.01, 0xFFFF0000);
			}
#endif
		}

		{ /// Ortho world
			Matrix4 cameraMatrix = mat4();
			cameraMatrix.SCALE(game->width/2.0, -game->height/2.0, -1);
			cameraMatrix.TRANSLATE(1, -1, 0);
			defaultWorld->viewMatrix = cameraMatrix.invert();

			Matrix4 oldProj = defaultWorld->projectionMatrix;
			defaultWorld->projectionMatrix = mat4();

			{ /// Player
				if (!game->playerSkeleton) {
					game->playerSkeleton = deriveSkeleton("assets/models/apfel/Apfel.skele");
					createSkeletonBlend(game->playerSkeleton, "root", SKELETON_BLEND_MANUAL_BONES);
					createSkeletonBlend(game->playerSkeleton, "main", SKELETON_BLEND_ANIMATION);
				}

				SkeletonBlend *root = getSkeletonBlend(game->playerSkeleton, "root");
				if (root) {
					Xform xform = newXform();
					xform.translation = v3(game->playerPosition.x, game->playerPosition.y, 0);
					xform.scale = v3(50 * game->playerScale, 50 * game->playerScale, 1);
					// xform.rotation = eulerToQuaternion(v3(0, 0, toRad(180)));
					int boneIndex = 0;
					root->controlMask[boneIndex] = true;
					root->poseXforms[boneIndex] = xform;
				}

				SkeletonBlend *main = getSkeletonBlend(game->playerSkeleton, "main");
				if (main) {
					main->animation = getAnimation(game->playerSkeleton, "ArmatureAction");
					main->loops = true;
					main->time = platform->time;
				}

				updateSkeleton(game->playerSkeleton, platform->elapsed);

				Matrix4 matrix = mat4();
				Model *model = getModel("assets/models/apfel/Apfel.model");
				ModelProps props = newModelProps();
				props.skeleton = game->playerSkeleton;
				props.ambientLight = v3(0.4, 0.4, 0.4);
				props.layer = 1;
				if (game->invTime > 0) {
					if (game->frameCount % 4 < 2) drawModel(model, props);
				} else {
					drawModel(model, props);
				}
			}

			for (int i = 0; i < game->danmakuListNum; i++) {
				Danmaku *danmaku = &game->danmakuList[i];
				DanmakuGraphicInfo *info = &app->danmakuGraphicInfos[danmaku->graphic];

				{ /// Draw
					bool shouldDraw = true;
					if (danmaku->dead) shouldDraw = false;
					if (danmaku->flags & _F_DF_INVIS) shouldDraw = false;

					if (shouldDraw) {
						Vec2 position = danmaku->position;
						float rotation = danmaku->spin + danmaku->angle;
						float scale = 1;
						bool addBlend = false;

						if (danmaku->frame < 0) {
							scale = 1 + danmaku->frame*-0.05;
							if (scale > 4) scale = 4;
							addBlend = true;
						} else {
							scale = danmaku->scale;
							if (danmaku->flags & _F_DF_BLEND_ADD) addBlend = true;
						}

						Matrix4 matrix = mat4();
						matrix.TRANSLATE(v3(position.x, position.y, 0));
						matrix.SCALE(scale*8, scale*8, 1);
						matrix.ROTATE_EULER(toRad(-rotation), toRad(90), 0);

						Model *model = getModel(getModelFromGraphic(danmaku->graphic));
						ModelProps props = newModelProps();
						props.matrix = matrix;
						Vec3 color = hueShift(v3(0.8, 0.3, 0.3), danmaku->color * 30);
						props.ambientLight = color * 0.6;
						props.diffuseLight = color;
						props.layer = 1;
						// props.specularLight = v3();
						props.alpha = danmaku->alpha;
						drawModel(model, props);
					}
				}

				bool canGraze = true;
				if (propDanmakuIsAlly(danmaku->type)) canGraze = false;
				if (propDanmakuIsEnemy(danmaku->type)) canGraze = false;
				if (danmaku->frame < 0) canGraze = false;

				if (canGraze) {
					float playerDist = danmaku->position.distance(game->playerPosition);
					float grazeRadius = info->radius*danmaku->scale + globals->grazePad;
					float grazeDist = playerDist - grazeRadius;

					float alpha = clampMap(grazeDist, 10, 0, 0, 0.5);
					drawCircle(danmaku->position, grazeRadius, argbFloatToHex(alpha, 0, 1, 0));
					drawCircle(danmaku->position, info->radius*danmaku->scale, argbFloatToHex(alpha, 1, 0, 0));
				}
			}

			defaultWorld->viewMatrix = game->cameraMatrix3d.invert();
			defaultWorld->projectionMatrix = oldProj;
		}

		process3dDrawQueue();
	}

#if (INSTANT_START_PLAYING_LOCAL || INSTANT_START_TRAINING || INSTANT_START_CONNECTING)
	if (game->phase == 0) {
		game->phase = 4;
		game->difficulty = 1;
	}
#endif

	if (game->prevPhase != game->phase) {
		game->prevPhase = game->phase;
		game->phaseTime = 0;
	}

	if (game->phase == 0) {
		if (game->phaseTime == 0) {
			game->selectedMenuDifficulty = 1;
		}

		game->phase++;
	} else if (game->phase == 1) {

		if (game->prevSelectedMenuItem != game->selectedMenuItem) game->prevSelectedMenuItem = game->selectedMenuItem;

		{
			bool selected = game->selectedMenuItem == 0 && game->prevSelectedMenuItem == game->selectedMenuItem;
			if (selected) {
				if (!game->prevInput.shoot && input->shoot) game->selectedMenuItem++;

				if (!game->prevInput.up && input->up) game->selectedCharacter = (Character)((int)game->selectedCharacter-1);
				if (!game->prevInput.down && input->down) game->selectedCharacter = (Character)((int)game->selectedCharacter-1);
				if (game->selectedCharacter > CHARACTERS_MAX-1) game->selectedCharacter = (Character)0;
				if (game->selectedCharacter < 0) game->selectedCharacter = (Character)(CHARACTERS_MAX-1);
			}

			Vec2 center = v2(game->width, game->height/2);
			center.x += 50;

			float radsPerSlice = 2.0*M_PI / (float)CHARACTERS_MAX;
			float targetWheelRads = game->selectedCharacter * radsPerSlice;
			game->currentWheelRads = lerpRad(game->currentWheelRads, targetWheelRads, 0.1);

			float wheelOffset = toRad(180) - radsPerSlice/2;
			wheelOffset += game->currentWheelRads;

			float radius = 500;
			drawCircle(center, radius, 0xFF220000);
			for (int i = 0; i < CHARACTERS_MAX; i++) {
				int posIndex = CHARACTERS_MAX - i;

				Vec2 end = radToVec2(radsPerSlice*posIndex + wheelOffset) * radius + center;
				drawLine(center, end, 2, 0xFFCCCCCC);

				Vec2 sliceCenter = center + radToVec2((radsPerSlice*posIndex) + radsPerSlice/2 + wheelOffset) * (radius/2);
				Rect rect = makeCenteredSquare(sliceCenter, 20);
				drawRect(rect, 0xFF00FF00);
				drawCircle(sliceCenter, 20, 0xFF0000FF);
				drawText(app->defaultFont, frameSprintf("%d: %s\n", i, characterStrings[i]), sliceCenter);
			}
		}

		Rect lastRect = {};

		{
			bool selected = game->selectedMenuItem == 1 && game->prevSelectedMenuItem == game->selectedMenuItem;
			int bgColor = 0xFFFF0000;

			if (selected) {
				if (!game->prevInput.shoot && input->shoot) game->selectedMenuItem++;
				if (!game->prevInput.bomb && input->bomb) game->selectedMenuItem--;

				if (!game->prevInput.right && input->right) game->selectedMenuDifficulty++;
				if (!game->prevInput.left && input->left) game->selectedMenuDifficulty--;
				if (game->selectedMenuDifficulty < 1) game->selectedMenuDifficulty = 8;
				if (game->selectedMenuDifficulty > 8) game->selectedMenuDifficulty = 1;
			} else {
				bgColor = lerpColor(bgColor, 0xFF000000, 0.5);
			}

			Rect rect = makeRect(0, 0, 128, 64);
			rect.x = game->width * 0.05;
			rect.y = game->height * 0.35;

			drawRect(rect, bgColor);

			char *str = frameSprintf("Difficulty: %d", game->selectedMenuDifficulty);
			Vec2 size = getTextSize(app->defaultFont, str);
			Vec2 pos;
			pos.x = rect.x + rect.width/2 - size.x/2;
			pos.y = rect.y + rect.height/2 - size.y/2;
			drawText(app->defaultFont, str, pos, 0xFF000000);

			lastRect = rect;
		}

		{
			bool selected = game->selectedMenuItem == 2 && game->prevSelectedMenuItem == game->selectedMenuItem;
			int bgColor = 0xFFFF0000;
			if (selected) {
				if (!game->menuReadyFinal) {
					if (!game->prevInput.bomb && input->bomb) game->selectedMenuItem--;
				}

				float menuReadyTimeMax = 1;
				if (input->shoot) {
					game->menuReadyTime += elapsed;
					if (game->menuReadyTime >= menuReadyTimeMax) game->menuReadyFinal = true;
				} else {
					game->menuReadyTime = 0;
				}

				bgColor = lerpColor(bgColor, 0xFFFF8000, game->menuReadyTime/menuReadyTimeMax);

				if (game->menuReadyFinal) bgColor = 0xFF008000;
			} else {
				bgColor = lerpColor(bgColor, 0xFF000000, 0.5);
			}

			Rect rect = makeRect(0, 0, 128, 64);
			rect.x = game->width * 0.05;
			rect.y = lastRect.y + lastRect.height + 5;

			drawRect(rect, bgColor);

			char *str = frameSprintf("Ready");
			Vec2 size = getTextSize(app->defaultFont, str);
			Vec2 pos;
			pos.x = rect.x + rect.width/2 - size.x/2;
			pos.y = rect.y + rect.height/2 - size.y/2;

			drawText(app->defaultFont, str, pos, 0xFF000000);
		}

		Game *otherGame = game == realGame ? realGame2 : realGame;
		if (game->menuReadyFinal && otherGame->menuReadyFinal) {
			game->difficulty = (game->selectedMenuDifficulty + otherGame->selectedMenuDifficulty)/2;
			game->phase++;
		}

	} else if (game->phase == 2) { /// Vn
		if (game->phaseTime == 0 && game->playerNumber == 1) {
			playScript(VN_SCRIPT_TO_TEST);
		}

		if (game->vnCommandPrevIndex != game->vnCommandIndex) {
			game->vnCommandPrevIndex = game->vnCommandIndex;
			game->vnCommandTime = 0;
		}

		if (game->vnCommandIndex < game->vnCommandsNum) {
			VnCommand *command = &game->vnCommands[game->vnCommandIndex];
			bool commandDone = false;

			if (command->type == VN_SET_IMAGE) {
				if (game->vnCommandTime == 0) strcpy(game->vnImagePaths[command->imageIndex], command->path);
				if (game->vnCommandTime > 0.5) commandDone = true;
			} else if (command->type == VN_SHOW_IMAGE) {
				game->vnShownImage = command->imageIndex;
				commandDone = true;
			} else if (command->type == VN_IMAGE_SAY) {
				strcpy(game->vnText, command->text);
				if (!game->prevInput.shoot && input->shoot) commandDone = true;
			} else if (command->type == VN_SET_WAVES) {
				for (int i = 0; i < PATTERNS_MAX; i++) {
					game->patterns[i] = (int)command->text[i];
				}
				commandDone = true;
			} else {
				logf("Unknown VnCommand\n");
			}

			for (int i = 0; i < 2; i++) {
				Texture *texture = getTexture(game->vnImagePaths[i]);
				if (texture) {
					RenderProps props = newRenderProps();
					float pad = 10;
					if (i == 0) {
						props.matrix.TRANSLATE(pad, game->height/2 - texture->height/2);
					} else {
						props.matrix.TRANSLATE(game->width - texture->width - pad, game->height/2 - texture->height/2);
					}

					if (game->vnShownImage == i) {
						props.matrix.TRANSLATE(0, lerp(0, -20, secondPhase));
					} else {
						props.tint = 0x40000000;
					}
					drawTexture(texture, props);
				}
			}

			if (game->vnText[0]) {
				char *str = game->vnText;
				Vec2 size = getTextSize(app->defaultFont, str, game->width*0.7);
				Vec2 pos;
				pos.x = game->width/2 - size.x/2;
				pos.y = game->height - size.y - 30;

				Rect rect = makeRect(pos, size);
				rect = rect.inflate(lerp(0, 20, secondPhase));
				drawRect(rect, 0x80000000);
				drawText(app->defaultFont, str, pos, 0xFFFFFFFF, game->width*0.7);
			}

			if (commandDone) {
				game->vnCommandIndex++;
			}
		} else {
			game->phase++;
		}
		game->vnCommandTime += elapsed;
	} else if (game->phase == 3) {
		float countdownMax = 1;
		float timeLeft = countdownMax - game->phaseTime;
		char *str = frameSprintf("Game starting %.1f", timeLeft);
		Vec2 size = getTextSize(app->defaultFont, str);
		Vec2 pos;
		pos.x = game->width/2 - size.x/2;
		pos.y = game->height/2 - size.y/2;
		drawText(app->defaultFont, str, pos, 0xFFFFFFFF);
		if (timeLeft < 0) game->phase++;
	} else if (game->phase == 4) {
		if (game->phaseTime == 0) {
			game->debugFreeCameraSpeed = 1;

			respawnPlayer();

#if (START_WITH_GOD_MODE == 1)
			game->debugGodMode = true;
#endif
#if (START_WITH_STARS == 1)
			game->mp = 100000;
#endif
			game->lives = 5;
			game->attackLevel = 1;

			if (app->playMode != PLAY_TRAINING) {
				for (int i = 0; i < PATTERNS_MAX; i++) {
					if (game->patterns[i] == 0) game->patterns[i] = rndInt(1, 20);
				}
			}

			fillPlayerShop(game == realGame);

#if SAVES_LOG
			if (directoryExists("assets/logs")) removeDirectory("assets/logs");
			createDirectory("assets/logs");
#endif
		}

		float playerRadius = globals->playerHitRadius*game->playerScale;

		{ /// Pattern Code
			PatternData *data = &game->patternData;

			data->difficulty = game->difficulty;
			int pattern = game->patterns[game->patternIndex];
			data->frame = game->framesInPattern;
			updatePattern(pattern, data);

			int framesTillNextPattern = 10*60;
			game->framesInPattern++;
			if (game->framesInPattern >= framesTillNextPattern) {
				game->framesInPattern = 0;
				game->patternCount++;

				game->patternIndex = game->patternCount % PATTERNS_MAX;
				if (game->patternIndex > PATTERNS_MAX-1) game->patternIndex = 0;
				if (game->patterns[game->patternIndex] == 0) {
					game->patternCount = 0;
					game->patternIndex = 0;
				}

				if (game->patternCount % 5 == 4 && !game->debugLockDifficulty) game->difficulty = mathClamp(game->difficulty, (game->difficulty + 1) / 5, 8);
				game->framesInPattern = 0;
				if (game->patternIndex > PATTERNS_MAX-1) game->patternIndex = 0;
			}
		}

		if (game->returnFireQuantity > 0) {
			char *str = frameSprintf("%d", game->returnFireQuantity);
			Vec2 size = getTextSize(app->hugeFont, str);
			drawText(app->hugeFont, str, v2((game->width / 2) - (size.x / 2), (game->height / 2) - (size.y / 2)), 0x30FFFFFF);
		}

		if (app->playMode == PLAY_TRAINING && game->playerNumber == 1) { // Training menu
			game->mp = 999;
			game->lives = 5;
			game->bombs = 3;

			ImGui::SetNextWindowPos(ImVec2(), ImGuiCond_Once);
			ImGui::Begin("Training menu", NULL, ImGuiWindowFlags_AlwaysAutoResize);

			ImGui::InputInt("Character", (int *)&game->selectedCharacter);
			ImGui::InputInt("ShotAddendum", &game->shotAddendum);
			ImGui::Separator();

			static int staticPatternToInject = 0;
			ImGui::Text("Pattern:");
			ImGui::SameLine();
			ImGui::InputInt("###staticPatternToInject", &staticPatternToInject);
			ImGui::SameLine();
			if (ImGui::Button("Inject")) {
				for (int i = 0; i < PATTERNS_MAX; i++) {
					if (game->patterns[i] == 0) {
						game->patterns[i] = staticPatternToInject;
						logf("Injected pattern %d\n", staticPatternToInject);
						break;
					}
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Reset")) {
				game->danmakuListNum = 0;
				game->itemListNum = 0;
				game->particleListNum = 0;
				game->framesInPattern = 0;
			}

			ImGui::SliderInt("Difficulty", &game->difficulty, 1, 8);
			ImGui::SameLine();
			ImGui::Checkbox("Lock", &game->debugLockDifficulty);

			if (ImGui::TreeNode("Create a shot")) {
				static DanmakuGraphic sGraphic;
				static ColorName sColor;
				static int sDelay;
				ImGui::Combo("Graphic", (int *)&sGraphic, danmakuGraphicStrings, ArrayLength(danmakuGraphicStrings));
				ImGui::Combo("Color", (int *)&sColor, colorNamesStrings, ArrayLength(colorNamesStrings));
				ImGui::InputInt("Delay", &sDelay);

				if (ImGui::Button("Shoot")) {
					Danmaku *danmaku = createShotA(
						v2(game->width/2, game->height/2),
						rndFloat(1, 5),
						rndFloat(0, 360),
						sGraphic,
						sColor,
						sDelay
					);
				}
				ImGui::TreePop();
			}

			ImGui::Separator();

			ImGui::Checkbox("God mode", &game->debugGodMode);
			if (ImGui::Button("Increase Return Fire")) game->returnFireQuantity++;

			ImGui::Separator();
			ImGui::Text("Current pattern: ");
			for (int i = 0; i < PATTERNS_MAX; i++) {
				if (game->patterns[i] != 0) {
					ImGui::SameLine();
					if (i == game->patternIndex) ImGui::PushStyleColor(ImGuiCol_Text, argbToRgba(0xFFFF0000));
					ImGui::Text("%d ", game->patterns[i]);
					if (i == game->patternIndex) ImGui::PopStyleColor();
				}
			}

			if (ImGui::Button("Clear patterns")) {
				for (int i = 0; i < PATTERNS_MAX; i++) game->patterns[i] = 0;
				game->patternIndex = 0;
			}

			ImGui::End();
		}

		{ /// Update player
			Vec2 inputVec = v2();
			if (input->up) inputVec.y -= 1;
			if (input->down) inputVec.y += 1;
			if (input->left) inputVec.x -= 1;
			if (input->right) inputVec.x += 1;
			inputVec = inputVec.normalize();

			if (input->focus) {
				game->focusLevel++;
			} else {
				game->focusLevel--;
				if (inputVec.isZero()) {
					game->playerMoveTrail *= 0.9;
				} else {
					game->playerMoveTrail += inputVec / 20.0;
					game->playerMoveTrail = v2(min(max(-1, game->playerMoveTrail.x), 1), min(max(-1, game->playerMoveTrail.y), 1));
				}
			}
			game->focusLevel = mathClamp(game->focusLevel, 0, 20);

			if (input->shoot && game->queuedShotFrames < globals->playerFireRate * 3) game->queuedShotFrames += globals->playerFireRate;
			if (game->queuedShotFrames % globals->playerFireRate == globals->playerFireRate - 1) {
				playerShoot(game->selectedCharacter, game->attackLevel, true);
				playerShotAddendums(game->shotAddendum, game->attackLevel, true);
			} else {
				playerShoot(game->selectedCharacter, game->attackLevel, false);
				playerShotAddendums(game->shotAddendum, game->attackLevel, false);
			}
			if (game->queuedShotFrames > 0) game->queuedShotFrames--;

			if (input->bomb && game->bombs > 0 && game->bombCooldown < 0) {
				Sound *sound = getSound("assets/audio/effects/bomb.ogg");
				Channel *channel = playSound(sound);
				channel->userVolume = 0.75;

				playerBomb();
				game->bombCooldown = 60;
				game->bombs--;
				game->invTime = globals->invTimeOnDeath;

				for (int i = 0; i < game->itemListNum; i++) game->itemList[i].claimed = true;
			}

			game->bombCooldown--;

			float speed = characterSpeeds[game->selectedCharacter].x;
			if (input->focus) speed = characterSpeeds[game->selectedCharacter].y;
			game->playerPosition += inputVec * speed;
			if (game->playerPosition.x < 0) {
				if (game->hasScreenWrap) game->playerPosition.x = game->width + game->playerPosition.x;
				else game->playerPosition.x = 0;
			}
			if (game->playerPosition.x > game->width) {
				if (game->hasScreenWrap) game->playerPosition.x = game->width - game->playerPosition.x;
				else game->playerPosition.x = game->width;
			}
			if (game->playerPosition.y < 0) game->playerPosition.y = 0;
			if (game->playerPosition.y > game->height) game->playerPosition.y = game->height;

			if (game->hasBubbleShield) drawCircle(game->playerPosition, 30, 0x80FFFF00);
			if (game->hasScreenWrap) drawCircle(game->playerPosition, 30, 0x8000FFFF);

			{
				float alpha = game->focusLevel / 20.0;

				Texture *texture = getTexture("assets/images/FocusArea.png");
				RenderProps props = newRenderProps();
				props.matrix.TRANSLATE(game->playerPosition);
				props.matrix.ROTATE(game->frameCount * 5);
				props.matrix.TRANSLATE(-texture->width/2, -texture->height/2);
				props.alpha = alpha;
				drawTexture(texture, props);

				drawCircle(game->playerPosition, playerRadius, lerpColor(0x00FFFFFF, 0xFFFFFFFF, alpha));
			}
		}

		{ /// Clears bins
			int binWidth = game->width/globals->binsWide;
			int binHeight = game->height/globals->binsHigh;
			game->binsMax = globals->binsWide * globals->binsHigh;
			if (!game->bins) game->bins = (SpatialBin *)zalloc(sizeof(SpatialBin) * game->binsMax);

			for (int i = 0; i < game->binsMax; i++) {
				SpatialBin *bin = &game->bins[i];
				bin->danmakuNum = 0;
			}
		}

		for (int i = 0; i < game->danmakuListNum; i++) { /// Update and add to bins
			Danmaku *danmaku = &game->danmakuList[i];
			DanmakuGraphicInfo *info = &app->danmakuGraphicInfos[danmaku->graphic];
			preformActions(danmaku);

			if (danmaku->dead) {
				game->danmakuList[i] = game->danmakuList[game->danmakuListNum-1];
				game->danmakuListNum--;
				i--;
				continue;
			}

			int relevantBinsNum = 0;
			int *relevantBins = getBinsForCircle(danmaku->position, info->radius*danmaku->scale, &relevantBinsNum);
			for (int i = 0; i < relevantBinsNum; i++) {
				int binIndex = relevantBins[i];
				SpatialBin *bin = &game->bins[binIndex];
				bin->danmaku[bin->danmakuNum++] = danmaku;
				if (game->debugShowBulletInfo) debugDrawDanmakuData(danmaku);
			}
		}

		for (int i = 0; i < game->danmakuListNum; i++) { /// Update ally bullets
			Danmaku *danmaku = &game->danmakuList[i];
			if (!propDanmakuIsAlly(danmaku->type)) continue;
			DanmakuGraphicInfo *info = &app->danmakuGraphicInfos[danmaku->graphic];

			int relevantBinsNum = 0;
			int *relevantBins = getBinsForCircle(danmaku->position, info->radius*danmaku->scale, &relevantBinsNum);
			for (int i = 0; i < relevantBinsNum; i++) {
				int binIndex = relevantBins[i];
				SpatialBin *bin = &game->bins[binIndex];

				for (int i = 0; i < bin->danmakuNum; i++) {
					Danmaku *otherDanmaku = bin->danmaku[i];
					if (propDanmakuIsAlly(otherDanmaku->type)) continue;
					if (!propDanmakuIsEnemy(otherDanmaku->type) && danmaku->type != DTYPE_BOMB) continue;

					DanmakuGraphicInfo *otherInfo = &app->danmakuGraphicInfos[otherDanmaku->graphic];
					float dist = danmaku->position.distance(otherDanmaku->position);
					if (dist <= info->bulletToBulletRadius*danmaku->scale + otherInfo->bulletToBulletRadius*otherDanmaku->scale) {
						int damageDealt = min(danmaku->health, otherDanmaku->health) + 1;
						if (damageDealt > danmaku->health) {
							eDanmakuKill(danmaku);
							otherDanmaku->health -= damageDealt;
						} else {
							danmaku->health -= damageDealt;
							eDanmakuKill(otherDanmaku);
						}
					}
				}
			}
		}

		{ /// Player collision
			int relevantBinsNum = 0;
			int *relevantBins = getBinsForCircle(game->playerPosition, playerRadius+globals->grazePad, &relevantBinsNum);
			for (int i = 0; i < relevantBinsNum; i++) {
				int binIndex = relevantBins[i];
				SpatialBin *bin = &game->bins[binIndex];

				for (int i = 0; i < bin->danmakuNum; i++) {
					Danmaku *danmaku = bin->danmaku[i];
					if (propDanmakuIsAlly(danmaku->type)) continue;
					if (danmaku->frame < 0) continue;
					DanmakuGraphicInfo *info = &app->danmakuGraphicInfos[danmaku->graphic];

					float playerDist = danmaku->position.distance(game->playerPosition);
					float grazeRadius = info->radius*danmaku->scale + globals->grazePad;
					if (playerDist < grazeRadius+playerRadius) {
						if (danmaku->grazable) {
							game->graze++;
							game->mp++;
							danmaku->grazable = false;

							Sound *sound = getSound("assets/audio/effects/graze.ogg");
							playSound(sound);
						}
						bool canHit = true;
						if (game->invTime > 0) canHit = false;
						if (game->debugGodMode) canHit = false;

						if (canHit && playerDist < info->radius*danmaku->scale + playerRadius) {
							if (!propDanmakuIsEnemy(danmaku->type)) danmaku->dead = true;
							game->invTime = globals->invTimeOnDeath;
							if (game->hasBubbleShield) {
								game->hasBubbleShield = false;
							} else {
								game->lives--;
								Sound *sound = getSound("assets/audio/effects/playerDie.ogg");
								Channel *channel = playSound(sound);
								channel->userVolume = 0.5;
								respawnPlayer();
							}
						}
					}
				}
			}
		}
		game->invTime -= elapsed;

		for (int i = 0; i < game->itemListNum; i++) {
			Item *item = &game->itemList[i];

			if (game->playerPosition.y < globals->poc) item->claimed = true;

			{ /// Preform Actions
				if (item->falling > 0) {
					item->position.y -= 3.6;
					item->angle += 10;
					item->falling--;
				}
				else {
					if (item->claimed) item->position = moveTowards(item->position, game->playerPosition, 8);
					else item->position.y += 1;
					if (item->position.y > game->height + 8) item->dead = true;
				}
			}

			{ /// Draw
				bool offScreen = item->position.y < 0;
				Texture *texture = getTexture(offScreen ? (char *)"assets/images/ItemIndicator.png" : (char *)itemGraphicStrings[item->type]);
				RenderProps props = newRenderProps();
				props.matrix.TRANSLATE(offScreen ? v2(item->position.x, 8) : item->position);
				if (!offScreen) {
					props.matrix.ROTATE(item->angle);
				}
				else {
					switch (item->type) {
						case LIFE: props.hueShift = VIOLET * toRad(30); break;
						case BOMB: props.hueShift = GREEN * toRad(30); break;
						case FULL: props.hueShift = YELLOW * toRad(30); break;
						case B_POWER: props.hueShift = RED * toRad(30); break;
						case POWER: props.hueShift = RED * toRad(30); break;
						case MANA: props.hueShift = BLUE * toRad(30); break;
						case S_MANA: props.hueShift = BLUE * toRad(30); break;
						case STAR: props.alpha = 0; break;
					}
				}
				props.matrix.TRANSLATE(-texture->width/2, -texture->height/2);
				drawTexture(texture, props);
			}

			float itemRadius = 15;
			float playerDist = item->position.distance(game->playerPosition);
			if (playerDist < itemRadius+playerRadius) {
				int mpToGain = 0;
				switch (item->type) {
					case LIFE: game->lives++; break;
					case BOMB: game->bombs++; break;
					case FULL: mpToGain += 30; break;
					case B_POWER: game->attackLevel = 3; break;
					case POWER: min(++game->attackLevel, 3); break;
					case MANA: mpToGain += 10; break;
					case S_MANA: mpToGain += 5; break;
					case STAR: mpToGain += 1; break;
				}

				if (mpToGain) {
					Sound *sound = getSound("assets/audio/effects/gainMp.ogg");
					Channel *channel = playSound(sound);
					channel->userVolume = 0.75;
					game->mp += mpToGain;
				}
				item->dead = true;
			}
			else if (playerDist < itemRadius+playerRadius + 15) {
				item->position = moveTowards(item->position, game->playerPosition, 2);
			}

			if (item->dead) {
				arraySpliceIndex(game->itemList, game->itemListNum, sizeof(Item), i);
				game->itemListNum--;
				i--;
				continue;
			}
		}

		for (int i = 0; i < game->particleListNum; i++) {
			Particle *particle = &game->particleList[i];

			float transitionPerc = (float)(game->frameCount - particle->spawnFrame) / PARTICLE_TIME_MAX;

			if (transitionPerc > 1) {
				arraySpliceIndex(game->particleList, game->particleListNum, sizeof(Particle), i);
				game->particleListNum--;
				i--;
				continue;
			}

			Texture *texture = getTexture("assets/images/DelayParticle.png");
			RenderProps props = newRenderProps();
			props.matrix.TRANSLATE(particle->position);
			props.matrix.SCALE(0.4);
			props.matrix.TRANSLATE(-texture->width/2, -texture->height/2);
			props.blendMode = BLEND_ADD;
			props.hueShift = particle->color * toRad(30);
			drawTexture(texture, props);

			particle->position.x += cos(toRad(particle->angle)) * particle->speed;
			particle->position.y += sin(toRad(particle->angle)) * particle->speed;
		}

		{ /// Boss ui
			Rect rect;
			rect.width = game->width * 0.75;
			rect.height = 16;
			rect.x = game->width/2 - rect.width/2;
			rect.y = game->height * 0.15;

			int outlineColor = 0xFF888822;
			int barColor = 0xFF00FF00;

			Danmaku *danmaku = getDanmaku(game->danmakuWithHpBar);
			if (game->prevHpBarPhase != game->hpBarPhase) {
				game->prevHpBarPhase = game->hpBarPhase;
				game->healthBarTime = 0;
			}

			if (game->hpBarPhase == 0) {
				if (danmaku) game->hpBarPhase++;
			} else if (game->hpBarPhase == 1) {
				int maxTime = 1;
				float perc = game->healthBarTime/maxTime;

				barColor = lerpColor(setAofArgb(barColor, 0), barColor, perc);
				outlineColor = lerpColor(setAofArgb(outlineColor, 0), outlineColor, perc);

				Rect barRect = rect;
				barRect.width *= perc;
				drawRect(barRect, barColor);
				drawRectOutline(rect, 2, outlineColor);
				if (perc >= 1) game->hpBarPhase++;

			} else if (game->hpBarPhase == 2) {
				if (danmaku) {
					int maxTime = 1;
					float perc = game->healthBarTime/maxTime;
					Rect barRect = rect;
					barRect.width *= danmaku->health / (float)propDanmakuGetHealth(danmaku->type);
					drawRect(barRect, barColor);
					drawRectOutline(rect, 2, outlineColor);
				} else {
					game->danmakuWithHpBar = 0;
					game->hpBarPhase++;
				}
			} else if (game->hpBarPhase == 3) {
				int maxTime = 1;
				float perc = game->healthBarTime/maxTime;

				barColor = lerpColor(barColor, setAofArgb(barColor, 0), perc);
				outlineColor = lerpColor(outlineColor, setAofArgb(outlineColor, 0), perc);

				drawRect(rect, lerpColor(0xFFFF0000, 0xFF880000, timePhase(game->healthBarTime*20)));
				drawRectOutline(rect, 2, outlineColor);
				if (perc >= 1) game->hpBarPhase = 0;
			}

			game->healthBarTime += elapsed;
		}

		{ /// Draw UI
			Texture *texture = getTexture("assets/images/ICON_LIFE.png");
			RenderProps props = newRenderProps();
			props.matrix.TRANSLATE(game->playerNumber == 2 ? v2((game->width - 5) - (texture->width * 0.4), 5) : v2(5, 5));
			props.matrix.SCALE(0.4);

			for (int i = 0; i < game->lives; i++) {
				drawTexture(texture, props);
				props.matrix.TRANSLATE(v2(game->playerNumber == 2 ? -texture->width : texture->width, 0));
			}

			props.matrix.TRANSLATE(v2((game->playerNumber == 2 ? texture->width : -texture->width) * game->lives, texture->width));
			texture = getTexture("assets/images/ICON_BOMB.png");

			for (int i = 0; i < game->bombs; i++) {
				drawTexture(texture, props);
				props.matrix.TRANSLATE(v2(game->playerNumber == 2 ? -texture->width : texture->width, 0));
			}

			{
				char *str = frameSprintf("Wave %d(%d)", game->patternCount, game->difficulty);
				Vec2 size = getTextSize(app->hugeFont, str);
				Vec2 pos;
				pos.x = 5;
				pos.y = game->height - size.y - 5;
				drawText(app->hugeFont, str, pos, 0x40FFFFFF);
			}

			if (app->debugMode) {
				char *str = frameSprintf("Rnd: %d\n", lcgSeed);
				drawText(app->defaultFont, str, v2(0, 500), 0xFFFFFFFF);
			}

			{
				char *str = NULL;
				if (app->playMode == PLAY_ONLINE) {
					if (game->playerNumber == 1) {
						str = frameSprintf("%d Online id: %d", game->frameCount, app->clientId);
					} else {
						str = frameSprintf("%d Online id: %d (delay %d)", game->frameCount, app->enemyClientId, app->streamedInputsNum);
					}
				} else {
					str = frameSprintf("Local player: %d", game->playerNumber);
				}
				Vec2 size = getTextSize(app->defaultFont, str);
				Vec2 pos;
				pos.x = game->width/2 - size.x/2;
				pos.y = 5;
				drawText(app->defaultFont, str, pos, 0x80FFFFFF);
			}

			{
				if (game->spellDisplayTime > 0) {
					float upTime = min(1, 4 * ((float)game->spellDisplayTime / globals->spellDisplayTime));

					char *str = app->shopList[game->spellIndex].name;
					Vec2 pos = getTextSize(app->uiFont, str) * -0.5;
					drawText(app->uiFont, str, game->playerPosition + pos + v2(0, -60 + (20 * upTime)), setAofArgb(0xFFEEAAFF, 255 * upTime));
					game->spellDisplayTime--;
				}

				{
					Texture *texture = getTexture("assets/images/FocusArea.png");
					RenderProps props = newRenderProps();
					props.matrix.TRANSLATE(game->playerPosition);
					props.matrix.ROTATE((float) game->frameCount * -3 * ((float) game->spellHoldMp / app->shopList[game->spellIndex].price));
					props.matrix.SCALE(3, 3);
					props.matrix.TRANSLATE(-texture->width/2, -texture->height/2);
					props.alpha = (float) game->spellHoldMp / app->shopList[game->spellIndex].price;
					props.tint = 0xFFA0C0E0;
					drawTexture(texture, props);
				}

				{
					Texture *texture = renderer->whiteTexture;
					RenderProps props = newRenderProps();
					props.matrix.TRANSLATE(game->playerPosition);
					props.matrix.SCALE(100, 100);
					props.matrix.TRANSLATE(-0.5, -0.5);
					props.flags |= _F_ARC;
					props.params.x = 0.2;
					props.params.y = (float) game->spellHoldMp / app->shopList[game->spellIndex].price;
					drawTexture(texture, props);
				}
			}

			{
				drawLine(v2(game->playerNumber == 2 ? 0 : game->width, 0), v2(game->playerNumber == 2 ? 0 : game->width, game->height), 2, 0xFFFFFFFF);
			}

			if (game->debugShowBulletInfo) drawLine(v2(0, globals->poc), v2(game->width, globals->poc), 1, 0xFFFF0000);
		}

		{ /// Shoping spells
			{
				char *str = frameSprintf("Graze: %d", game->graze);
				Vec2 size = getTextSize(app->smallFont, str);
				Vec2 pos;
				if (game->playerNumber == 1) {
					pos.x = game->width - size.x - 10;
				} else {
					pos.x = 10;
				}
				pos.y = 1;
				drawText(app->smallFont, str, pos, 0xFFEEAAFF);
			}

			{
				char *str = frameSprintf("Mp: %d\n", game->mp);
				Vec2 size = getTextSize(app->defaultFont, str);
				Vec2 pos;
				if (game->playerNumber == 1) {
					pos.x = game->width - size.x - 10;
				} else {
					pos.x = 10;
				}
				pos.y = 5;
				drawText(app->defaultFont, str, pos, 0xFFEEAAFF);
			}

			if (input->extra) {
				if (input->focus) {
					game->spellHoldMp += 2;
				} else if (!game->prevInput.extra) {
					Sound *sound = getSound("assets/audio/effects/spellChange.ogg");
					Channel *channel = playSound(sound);
					channel->userVolume = 0.15;

					//What a weird bad hack... But it works~! And removing it would be a pain. =P
					game->spellIndex = (game->spellIndex + 1) % 64;
					while (!(game->shopFlags & (1ull << game->spellIndex))) {game->spellIndex = (game->spellIndex + 1) % 64;}

					game->spellDisplayTime = globals->spellDisplayTime;
					if (game->spellIndex > app->shopListNum-1) game->spellIndex = 0;
					game->spellHoldMp = 0;
				}
			} else {
				game->spellHoldMp--;
				if (game->spellHoldMp < 0) game->spellHoldMp = 0;
			}

			Vec2 cursor = v2();
			if (game->playerNumber == 1) {cursor.x = game->width - 5;} else {cursor.x = 5;}
			cursor.y = 40;

			for (int i = 0; i < app->shopListNum; i++) {
				if (!(game->shopFlags & (1ull << i))) continue;
				SpellType spellType = app->shopList[i].effect;
				char *spellName = app->shopList[i].name;
				int price = app->shopList[i].price;
				bool selected = game->spellIndex == i;

				char *spellLabel = frameSprintf("%s (%d)", spellName, price);
				Vec2 textSize = getTextSize(app->defaultFont, spellLabel);

				Rect rect;
				rect.width = textSize.x;
				rect.height = textSize.y;
				rect.x = cursor.x;
				if (game->playerNumber == 1) rect.x -= rect.width;
				rect.y = cursor.y;

				float castPerc = 0;
				if (selected) {
					if (game->spellHoldMp > game->mp) game->spellHoldMp = game->mp;
					castPerc = (float)game->spellHoldMp / price;

					if (getSpellBoss(spellType) != -1 && castPerc >= 1) {
						Game *otherGame = game == realGame ? realGame2 : realGame;
						bool otherHasBoss = false;
						if (otherGame->danmakuWithHpBar) otherHasBoss = true;
						for (int i = 0; i < otherGame->incomingSpellsNum; i++) {
							SpellCast *cast = &otherGame->incomingSpells[i];
							if (getSpellBoss(cast->type) != -1) otherHasBoss = true;
						}

						if (otherHasBoss) {
							game->spellHoldMp = price-1;
							castPerc = 0.999;
						}
					}

					if (castPerc >= 1) {
						Sound *sound = getSound("assets/audio/effects/spellBuy.ogg");
						Channel *channel = playSound(sound);
						channel->userVolume = 0.75;

						game->spellHoldMp = 0;
						game->mp -= app->shopList[game->spellIndex].price;

						bool sendToOpponent = false;
						if (getSpellBoss(spellType) != -1) sendToOpponent = true;
						if (spellType == SPELL_RETURN_FIRE) sendToOpponent = true;
						if (spellType == SPELL_GROW_ENEMY) sendToOpponent = true;
						if (getSpellAttackPattern(spellType) != -1) sendToOpponent = true;

						castSpell(game == realGame, spellType, sendToOpponent);
					}
				}

				drawRect(rect, 0xFF444444);

				{
					Rect foreRect = rect;
					foreRect.width *= castPerc;
					if (game->playerNumber == 2) foreRect.x += rect.width - foreRect.width;
					drawRect(foreRect, 0xFFFF0000);
				}

				Vec2 pos = v2(rect.x, rect.y);
				int textColor = 0xFFCCCCCC;

				if (selected) {
					textColor = 0xFFFF0044;
				} else {
					if (game->mp < price) textColor = 0xFF000000;
				}

				drawText(app->defaultFont, spellLabel, pos, textColor);

				cursor.y += rect.height + 5;
			}
		}

		{ /// Incoming spells
			Vec2 cursor = v2(game->width/2, 32);
			for (int i = 0; i < game->incomingSpellsNum; i++) {
				SpellCast *cast = &game->incomingSpells[i];

				float secLeft = (cast->frame - game->frameCount) / 60.0;
				char *str = frameSprintf("Incoming \"%s\" in: %.2f (%d)", spellTypeStrings[cast->type], secLeft, cast->frame);
				Vec2 size = getTextSize(app->defaultFont, str);
				Vec2 pos = cursor - size/2;
				drawText(app->defaultFont, str, pos, 0xFFFFFFFF);

				cursor.y += size.y + 10;

				if (game->frameCount == cast->frame) {
					castSpell(game == realGame, cast->type, false);
					arraySpliceIndex(game->incomingSpells, game->incomingSpellsNum, sizeof(SpellCast), i);
					game->incomingSpellsNum--;
					i--;
					continue;
				}
			}
		}

		if (game->lives <= 0) {
			int playerNumber = game->playerNumber == 1 ? 2 : 1;
			for (int i = 0; i < 10; i++) infof("Player %d won!\n", playerNumber);
			app->state = APP_MENU;
		}
	}

	{ /// Sync string
		char *str = frameSprintf(
			"%d(%d) | %d%d%d%d%d%d%d%d\n",
			game->frameCount,
			game->inputHash,
			input->up,
			input->down,
			input->left,
			input->right,
			input->focus,
			input->shoot,
			input->bomb,
			input->extra
		);

		if (!game->syncString) {
			game->syncStringMax = 1024;
			game->syncStringNum = 0;
			game->syncString = (char *)malloc(sizeof(char) * game->syncStringMax);
			game->syncString[0] = 0;
		}

		int len = strlen(str)+1;
		if (game->syncStringNum + len > game->syncStringMax-1) {
			game->syncString = (char *)resizeArray(game->syncString, sizeof(char), game->syncStringMax, game->syncStringMax * 1.5);
			game->syncStringMax *= 1.5;
		}

		// strcpy(game->syncString + game->syncStringNum, str); // ???
		strcat(game->syncString, str);
		game->syncStringNum += len;

		int lineCount = countChar(game->syncString, '\n');

		if (globals->showSyncString) {
			int linesToShow = 5;

			if (lineCount > linesToShow) {
				char *linesStart = game->syncString;
				for (int i = 0; i < lineCount-linesToShow; i++) {
					linesStart = strchr(linesStart, '\n')+1;
				}

				{
					char *str = linesStart;
					Vec2 size = getTextSize(app->defaultFont, str);
					Vec2 pos;
					pos.x = game->width - size.x - 5;
					pos.y = game->height - size.y - 5 + 10;
					drawText(app->defaultFont, str, pos, 0x80FFFFFF);
				}
			}
		}

		if (lineCount >= 600) {
			char *path = NULL;
			if (app->playMode == PLAY_ONLINE) {
				int onlineId;
				if (game->playerNumber == 1) onlineId = app->clientId;
				else onlineId = app->enemyClientId;
				path = frameSprintf("assets/logs/inputsOnline%d.txt", onlineId);
			} else {
				path = frameSprintf("assets/logs/inputsLocal%d.txt", game->playerNumber);
			}
#if SAVES_LOG
			appendFile(path, game->syncString, strlen(game->syncString));
#endif
			free(game->syncString);
			game->syncString = NULL;
		}
	}

	if (game->debugShowBinsInfo) {
		int binWidth = game->width/globals->binsWide;
		int binHeight = game->height/globals->binsHigh;

		for (int i = 0; i < game->binsMax; i++) {
			int row = i % globals->binsWide;
			int col = i / globals->binsWide;
			Vec2 binPos = v2(row * binWidth, col * binHeight);
			Vec2 binCenter;
			binCenter.x = binPos.x + binWidth/2;
			binCenter.y = binPos.y + binHeight/2;
			SpatialBin *bin = &game->bins[i];
			char *str = frameSprintf("%d", bin->danmakuNum);
			Vec2 size = getTextSize(app->defaultFont, str);

			Vec2 pos = binCenter - size/2;
			drawText(app->defaultFont, str, pos, 0xFFFFFFFF);
			Rect rect;
			rect.x = binPos.x;
			rect.y = binPos.y;
			rect.width = binWidth;
			rect.height = binHeight;
			drawRectOutline(rect, 1, 0xFFFFFFFF);
			// drawCircle(binCenter, 5, 0xFF0000FF);
		}
	}

	game->prevInput = *input;
	game->bulletSoundsThisFrame = 0;
	game->frameCount++;
	game->phaseTime += elapsed;
	game->seed = popRndSeed();

	setFramebuffer(NULL);
}

int *getBinsForCircle(Vec2 center, float radius, int *binsNum) {
	Globals *globals = &app->globals;

	int binWidth = game->width/globals->binsWide;
	int binHeight = game->height/globals->binsHigh;

	int minX = floor((center.x - radius) / (float)binWidth);
	int minY = floor((center.y - radius) / (float)binHeight);
	int maxX = ceil((center.x + radius) / (float)binWidth);
	int maxY = ceil((center.y + radius) / (float)binHeight);
	// drawCircle(center, radius, 0x8000FFFF);
	// Rect rect = makeRect(center.x - radius, center.y - radius, radius*2, radius*2);

	int binsMax = 64;
	int *binIndices = (int *)frameMalloc(sizeof(int) * binsMax);
	int binIndicesNum = 0;

	for (int y = minY; y < maxY; y++) {
		for (int x = minX; x < maxX; x++) {
			if (x < 0 || x > globals->binsWide-1 || y < 0 || y > globals->binsHigh-1) continue;

			int binIndex = y * globals->binsWide + x;
			bool shouldAdd = true;
			for (int i = 0; i < binIndicesNum; i++) {
				int existingIndex = binIndices[i];
				if (existingIndex == binIndex) {
					shouldAdd = false;
					break;
				}
			}

			if (shouldAdd) {
				if (binIndicesNum > binsMax-1) logf("Danmaku is in too many bins!\n");
				binIndices[binIndicesNum++] = binIndex;
			}
		}
	}

	*binsNum = binIndicesNum;
	return binIndices;
}

void respawnPlayer() {
	game->playerPosition.x = game->width / 2;
	game->playerPosition.y = game->height * 0.75;
	game->bombs = app->globals.bombsPerLife;
	game->hasScreenWrap = false;
	game->playerScale = 1;
	game->playerOptions[0] = game->playerPosition;
	game->playerOptions[1] = game->playerPosition;
	game->playerOptions[2] = game->playerPosition;
}

void playBulletSound() {
	if (game->bulletSoundsThisFrame >= 3) return;

	Sound *sound = getSound("assets/audio/effects/shoot.ogg");
	Channel *channel = playSound(sound);
	channel->userVolume = 0.25;
	game->bulletSoundsThisFrame++;
}

NetMsg *newOutMsg(NetMsgType type) {
	if (app->outMsgsNum > MSGS_MAX-1) {
		logf("Too many outMsgs\n");
		return NULL;
	}

	NetMsg *msg = &app->outMsgs[app->outMsgsNum++];
	memset(msg, 0, sizeof(NetMsg));
	msg->id = app->clientId;
	msg->type = type;
	return msg;
}

void debugDrawDanmakuData(Danmaku *danmaku) {
	DanmakuGraphicInfo *info = &app->danmakuGraphicInfos[danmaku->graphic];
	if (!propDanmakuIsAlly(danmaku->type)) {
		if (info->bulletToBulletRadius > info->radius) {
			drawCircle(danmaku->position, info->bulletToBulletRadius*danmaku->scale, 0xFF0000FF);
			drawCircle(danmaku->position, info->radius*danmaku->scale, 0xFFFF0000);
		}
		else if (info->bulletToBulletRadius == info->radius) {
			drawCircle(danmaku->position, info->radius*danmaku->scale, 0xFF800080);
		}
		else {
			drawCircle(danmaku->position, info->radius*danmaku->scale, 0xFFFF0000);
			drawCircle(danmaku->position, info->bulletToBulletRadius*danmaku->scale, 0xFF0000FF);
		}
	}
	else {
		drawCircle(danmaku->position, info->radius*danmaku->scale, 0xFFFF0000);
	}
	char *str = frameSprintf("health: %d", danmaku->health);
	drawText(app->smallFont, str, danmaku->position, 0xFFFFFFFF);
}

int getClosestEnemy(Vec2 searchPosition) {
	int target = -1;
	float targetScore = 99999.0;
	for (int i = 0; i < game->danmakuListNum; i++) {
		Danmaku *danmaku = &game->danmakuList[i];
		if (!propDanmakuIsEnemy(danmaku->type)) continue;

		float getScore = searchPosition.distance(danmaku->position);
		if (getScore < targetScore) {
			target = i;
			targetScore = getScore;
		}
	}
	return target;
}

int getClosestEnemyShot(Vec2 searchPosition) {
	int target = -1;
	float targetScore = 99999.0;
	for (int i = 0; i < game->danmakuListNum; i++) {
		Danmaku *danmaku = &game->danmakuList[i];
		if (propDanmakuIsEnemy(danmaku->type)) continue;
		if (propDanmakuIsAlly(danmaku->type)) continue;

		float getScore = searchPosition.distance(danmaku->position);
		if (getScore < targetScore) {
			target = i;
			targetScore = getScore;
		}
	}
	return target;
}

MapProp *newMapProp(MapPropType type) {
	if (game->mapPropsNum > MAP_PROPS_MAX-1) { // This shouldn't crash
		logf("Too many MapProps!\n");
		return NULL;
	}

	MapProp *mapProp = &game->mapProps[game->mapPropsNum++];
	memset(mapProp, 0, sizeof(MapProp));
	mapProp->id = ++game->nextMapPropId;
	mapProp->type = type;
	mapProp->scale = v3(1, 1, 1);
	return mapProp;
}

MapProp *getMapProp(int id) {
	for (int i = 0; i < game->mapPropsNum; i++) {
		MapProp *mapProp = &game->mapProps[i];
		if (mapProp->id == id) return mapProp;
	}

	// logf("Failed to find MapProp %d\n", id);
	return NULL;
}
