#if 0
Players have 10000hp and 1000mp
- Player 1:
	- Attack, 5000 damage
	- Quick attack, 20mp, 2 hits, 4000 damage, never misses (better at enemy dodge rate >= 20%)
	- Wide strike, 20mp, 1500 damage, hits adjacent
	- Combo, 20mp, starts at one hit, increases each time cast consecutively, 2000 damage
	- Earthquake, 100mp, hit all enemies, 500 damage, reduce enemy physical atk/def by 50% for 2 turns
	- Phys up, 100mp, double physical damage for both players for 2 turns
	- Bodyblock, all attacks are directed to this player
	- Draw:
		- Default, no stats
		- Vampire, steals 1% hp, makes everyone bleed when drawn
		- ?, can't be healed when holding, deals 2x physical damage
		- ?, +25% magic resist for both players, drawing costs 100 mana for both players
		- ?, does more damage at lower hp, both players gain 100hp when drawn
		- ?, gains +10% dodge chance, attacks for 1000 damage in 2 hits when draw
		- ?, poisons enemy and costs 10mp every hit, become sick when drawn
		- Glass, does double damage once, then breaks (must attack upon draw?)

- Player 2 (starts asleep and with re life?)
	- Attack, 1500 damage in 2 hits
	- Add attack buff:
		- Poison, next attack poison
		- ?, next attack steal 1% mana
		- ?, next attack reduce enemy physical and magical attack for 2 turns
		- ?, next attack reduce enemy physical and magical defense for 2 turns

	? Hex, 10mp, deals 1 damage pure per turn forever (too OP because of shields)
	- Poison, 70mp, deals 100 magic damage per turn for 10 turns
	? Create antidote, creates an antidote to poison (There's no poison on enemies yet)
	- Heal, 100mp, gives 5000hp to a player
	- Heal2, 300mp, gives 5000hp both players
	- Res, 200mp, gives re life to the living
	- Antibuff, 100mp, remove the next buff (could be op)
	- Restore voice, 1mp, removes silence
	- Give mana, 512mp, gives 512mp to player, halfes each cast
	- Moon strike, 150mp, jumps for one turn, arrives next turn to do 7000 damage (5 hits)
	- Lightning, 50mp, does 1000 damage to all enemies
	- Ice, 100mp, stuns enemy and does 1000 damage
	- Items:
		- Small mana potion x1, restores 500mp
		- Big mana potion x1, restores 1000mp
		- Voice restorer x3, restores voice, does 500 damage
		- Drop/Pick up time gem 1
		- Drop/Pick up time gem 2
	- Wish: (Granted at the end of the round)
		- Wish for health (gives both players 2000hp)
		- Wish for safety (gives both players x2 defence phys/magic, for 3 turn)
		- Wish for power (gives both player gain x2 attack for 1 turn)
	- Sleep, gains 1000hp
	? Dream
		- Dream of wealth
		- Dream of speed, dodge rate
	? Judge
	? Environment

--- Enemies:

- Small shieldster, 4000hp, starts with shield
	- Creates single shield if doesn't have one, if has one creates one for random ally
	- If has 5 shields, does shield blast, dealing 5000 damage and removing all shields

- Super shieldster, 10000hp, starts with 2 shields
	- If has no shields, creates double shields on self
	- If has shields, shield bump for 500 damage per shield

- Shield summoner, 5000hp, starts with a shield
	- Causes a shield to be stolen if available when attacked
	- Always attacks for 3000 damage in one hit

- Fake shieldster, 5000hp, starts with two shields randomly either real or fake
	- Creates 2 random sheilds on allies, one of them fake

- Mana bruiser, 10000hp
	- Every turn takes 50 mana from both players
	- If a player has less than 50 mana, take 100 mana away from the players with lower mana

? Thief
	- Counters with steal

- Standard A, 5000hp
	- Does 1000 damage to a random enemy

- Standard B, 10000hp
	- Does 500 damage to a random enemy

- Standard C, 20000hp
	- Does 2000 damage to a random enemy

- Swift 1000hp, has 90% dodge chance
	- Does 500 damage to a random enemy

- Accelerator, 20000hp
	- Attacks both players for 1 damage, doubles each turn

- Gunner, 10000hp
	- If has no charge, charge an attack
	- If has charge, attack with charge 2000 damage in 8 hits

- Multihitter, 3000hp
	- 50% chance of casting a 300 damage attack (3 hits)
	- 50% chance of casting a spell that makes all attacks do 100 more damage (lasts 1 turn)

- Teacher Teacher, 30000hp
	- 50% chance of attacking for 900 damage (3 hits)
	- 50% chance of spawning teacher

- Teacher, 10000hp
	- 50% chance of attacking for 300 damage
	- 50% chance of spawning student

- Student 3333hp
	- Does 100 damage in 2 hits

- Stunner, 3000hp
	- 50% chance of casting a 100 damage attack
	- 50% chance of casting a spell that stuns for 1 turns

? Shocker 3000hp
	- Does 100 damage
	- Upon death, gives the attacker a buff the causes them to take 300 damage every time they hit an opponent for 2 turns

- Time rewinder 50000hp, starts with 3 charages
	- If has 3 charges, spend them to give your team another turn.
	- Otherwise, get a charge

? Controller, 15000hp, starts with amulet
	- If has amulet, 50% chance of casting mind control on player 2, causing them to waste their turns until the amulet is broken
	- 50% chance of doing 3000 damage
	- If attacked, amulet is broken

- Berserker, 10000hp
	- 33% chance of casting berserk on a random enemy
	- 33% chance blocking to take 50% damage
	- 33% chance attacking for 200 damage

- Spike, 3000hp, has a buff reflects damage x10
	- Does nothing

- Silencer, 10000hp (Could be used with Controller?)
	- Gives a random player a buff that will cause them to become silenced if they spend mana next turn

? Flash bang, 1000hp
	- Attacks for 100 damage, 50% chance of buff that gives 10% miss chance for 1 turn.
	- Upon death, blinds every for 1 turn.

- Life taker, 20000hp
	- 50% chance of reducing an enemies max hp by 1%
	- 50% chance of doing nothing

- Reaper 10000hp
	- does 9999 damage in one hit

--- Waves:
- Dull spike:
	- Silencer
	- Spike
	- Silencer

- Life lights:
	- Silencer
	- Life taker
	- Silencer

- Tragic mid game:
	- Stunner
	- Silencer
	- Life taker
	- Silencer
	- Stunner

- Spike trap:
	- Spike
	- Berserker
	- Spike
	- Berserker
	- Spike

- Simple shields:
	- Small shieldster
	- Small shieldster
	- Small shieldster

- Simple shields 2:
	- Small shieldster
	- Small shieldster
	- Small shieldster
	- Small shieldster
	- Small shieldster

- Shields:
	- Small shieldster
	- Super shieldster
	- Small shieldster

- Shields 2:
	- Super shieldster
	- Small shieldster
	- Super shieldster
	- Small shieldster
	- Super shieldster

- Summoner:
	- Super shieldster
	- Small shieldster
	- Shield summoner
	- Small shieldster
	- Super shieldster

- Grand defense:
	- Super shieldster
	- Super shieldster
	- Shield summoner
	- Super shieldster
	- Super shieldster

- Input randomness:
	- Small shieldster
	- Small shieldster
	- Super shieldster
	- Small shieldster
	- Small shieldster

- Output randomness:
	- Fake shieldster
	- Super shieldster
	- Fake shieldster

- Rng mana loss:
	- Fake shieldster
	- Mana bruiser
	- Mana bruiser
	- Fake shieldster

- Simple acceleration:
	- Accelerator
	- Accelerator

- 5 core acceleration:
	- Accelerator
	- Accelerator
	- Accelerator
	- Accelerator
	- Accelerator

- Odds of extra acceleration:
	- Simple shieldster
	- Accelerator
	- Accelerator
	- Accelerator

- Odds of extra acceleration 2:
	- Simple shieldster
	- Accelerator
	- Accelerator
	- Accelerator
	- Simple shieldster

- Before school:
	- Student
	- Student
	- Student
	- Student
	- Student

- First lesson:
	- Teacher
	- Teacher

- Stratified education:
	- Teacher Teacher
	- Teacher
	- Student

- Release the teachers
	- Multihitter
	- Multihitter
	- Teacher Teacher
	- Multihitter
	- Multihitter

- Learning many hits:
	- Teacher
	- Multihitter
	- Multihitter
	- Teacher

- What matters more:
	- Mana bruiser
	- Teacher

- Multiplier acceleration:
	- Multihitter
	- Accelerator
	- Accelerator
	- Multihitter

- Many bullets
	- Multihitter
	- Gunner
	- Multihitter
	- Gunner
	- Multihitter

- Careful blinding:
	- Gunner
	- Flash bang
	- Gunner
	- Flash bang
	- Gunner

- Max stun:
	- Stunner
	- Stunner
	- Stunner
	- Stunner
	- Stunner

- Infinite stun:
	- Stunner
	- Stunner
	- Time rewinder
	- Stunner
	- Stunner

- Fast learning
	- Time rewinder
	- Teacher Teacher

- Many turns:
	- Small shieldster
	- Time rewinder
	- Time rewinder
	- Teacher

- Infinite school
	- Time rewinder
	- Time rewinder
	- Teacher Teacher
	- Time rewinder
	- Time rewinder

- Shocking
	- Shocker
	- Shocker
	- Shocker
	- Shocker
	- Shocker

- Tut1
	- Standard A
	- Standard A

- Tut2
	- Standard B
	- Standard B

- Kill the weak ones first
	- Standard A
	- Standard B
	- Standard A

- Now the big one is more important
	- Standard A
	- Standard C
	- Standard A

- Useless shieldster
	- Small shieldster

- Easy mix
	- Standard A
	- Standard B
	- Small shieldster
	- Standard A
	- Standard B

- Standard mix
	- Standard B
	- Standard A
	- Standard C
	- Standard A
	- Standard B

- Swift vs standard:
	- Standard B
	- Swift
	- Standard A
	- Swift
	- Standard B

- Swift rng:
	- Swift
	- Swift
	- Swift
	- Swift
	- Swift

- Crippled
	- Life taker
	- Life taker
	- Life taker
	- Life taker
	- Life taker

- No AOE crippled
	- Life taker
	- Life taker
	- Spike
	- Life taker
	- Life taker

- Quick death
	- Reaper

- Instant death
	- Reaper
	- Reaper

--- Levels
- 1. Intro level:
	- Tut1
	- Tut2
	- Kill the weak ones first
	- Now the big one is more important
	- Useless shieldster

- 2. Simple mixes level:
	- Easy mix
	- Swift vs standard
	- Standard mix
	- Swift rng

- 3. Shield level:
	- Simple sheilds
	- Shields
	- Simple sheilds 2
	- Shields 2
	- Summoner

- 4. Shield scramble level:
	- Input randomness
	- Output randomness
	- Input randomness
	- Output randomness
	- Rnd mana loss

- 5. Learning accelerators level:
	- Simple acceleration
	- 5 core acceleration
	- Odds of extra acceleration
	- Odds of extra acceleration 2

- 6. Learning learning level:
	- Before school
	- First lesson
	- Learning many hits
	- Stratified education
	- Release the teachers

- 7. Stunning time level:
	- Max stun
	- Infinite stun
	- Fast learning
	- Many turns
	- Infinite school

- 8. Pain level:
	- Life lights
	- Spike trap
	- Tragic mid game
	- Dull spike
	- Grand defense

- 9. Losing life early on level:
	- Crippled
	- No AOE crippled
	- What matters more
	- Stratified education (The math changes because you have so little hp)

--- Misc:
	- A spell that instantly kills
	- Sickness can be cured by healing, then small DoT heals have a purpose
	- Curing a stun is the same as giving your turn away
	- An enemy that charges every other attack
	- An item that cures debuffs at the cost of hp 
	- Fake shield, if you only wanted to pop shields
	- Moon jump
	- A spell that reduces your max hp slowly over time
	- An enemy that passes turns
	- An enemy that destroys your save point 
	- You get a "double turn" when you kill the last enemy
	- A spell that causes spells to cost twice as much, (maybe self? bkb-like?)
	- A spell that gives you a secret item
	- An enemy that kills you for the whole match
	- An enemy that spawns another when hit

--- EFB notes:
	- An enemy that interchanges between physical attacks and defense debuffs
#endif

struct CameraPosition {
#define CAMERA_POSITION_NAME_MAX_LEN 32
	char name[CAMERA_POSITION_NAME_MAX_LEN];
	Vec3 position;
	Vec3 rotation;
};

struct Globals {
};

struct Game {
	Framebuffer *gameFramebuffer;
	Texture *gameTexture;

	Globals globals;

#define CAMERA_POSITIONS_MAX 8
	CameraPosition cameraPositions[CAMERA_POSITIONS_MAX];

	Matrix4 cameraMatrix;

	Model *commonFbx;
	Model *fbxModel;
	Model *testModel;

	bool debugMode;
	bool debugFreeCamera;
	float debugFreeCameraSpeed;
	Vec3 debugCameraPosition;
	Vec3 debugCameraRotation;
};

Game *game = NULL;

void runGame();
void updateGame();
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

	bool good = initNetworking();
	if (!good) logf("Failed to init networking\n");

	platformUpdateLoop(updateGame);
}

void updateGame() {
	if (!game) {
		game = (Game *)zalloc(sizeof(Game));

		// RegMem(Globals, );
		// loadStruct("Globals", "assets/info/globals.txt", &game->globals);

		RegMem(CameraPosition, name);
		RegMem(CameraPosition, position);
		RegMem(CameraPosition, rotation);
		loadStructArray("CameraPosition", "assets/info/cameraPoses.txt", game->cameraPositions, CAMERA_POSITIONS_MAX, sizeof(CameraPosition));

		game->debugCameraPosition = game->cameraPositions[0].position;
		game->debugCameraRotation = game->cameraPositions[0].rotation;
		game->debugFreeCameraSpeed = 0.5;
	}

	Globals *globals = &game->globals;
	float elapsed = platform->elapsed;
	float secondPhase = timePhase(platform->time);

	if (keyJustPressed(KEY_BACKTICK)) game->debugMode = !game->debugMode;
	if (game->debugMode) {
		if (keyJustPressed(' ')) game->debugFreeCamera = !game->debugFreeCamera;

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

		if (ImGui::TreeNode("Meshes")) {
			showMeshGui();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Models")) {
			showModelsGui();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Camera positions")) {
			if (ImGui::Button("Save")) saveStructArray("CameraPosition", "assets/info/cameraPoses.txt", game->cameraPositions, CAMERA_POSITIONS_MAX, sizeof(CameraPosition));
			ImGui::SameLine();
			if (ImGui::Button("Load")) loadStructArray("CameraPosition", "assets/info/cameraPoses.txt", game->cameraPositions, CAMERA_POSITIONS_MAX, sizeof(CameraPosition));

			for (int i = 0; i < CAMERA_POSITIONS_MAX; i++) {
				ImGui::Separator();
				ImGui::PushID(i);
				CameraPosition *cam = &game->cameraPositions[i];
				ImGui::InputText("Name", cam->name, CAMERA_POSITION_NAME_MAX_LEN);
				ImGui::DragFloat3("Position", &cam->position.x);
				ImGui::DragFloat3("Rotation", &cam->rotation.x);
				if (ImGui::Button("Capture")) {
					cam->position = game->debugCameraPosition;
					cam->rotation = game->debugCameraRotation;
				}
				ImGui::SameLine();
				if (ImGui::Button("Restore")) {
					game->debugCameraPosition = cam->position;
					game->debugCameraRotation = cam->rotation;
				}
				ImGui::PopID();
			}

			ImGui::TreePop();
		}
		ImGui::End();
	}

	{ /// Camera
		if (game->debugMode && game->debugFreeCamera) {
			platform->useRelativeMouse = true;
				Vec3 moveVec = v3();
				if (keyPressed('W')) moveVec.z--;
				if (keyPressed('S')) moveVec.z++;
				if (keyPressed('A')) moveVec.x--;
				if (keyPressed('D')) moveVec.x++;
				if (keyPressed(KEY_SHIFT)) moveVec.y++;
				if (keyPressed(KEY_CTRL)) moveVec.y--;

				moveVec *= game->debugFreeCameraSpeed;
				moveVec = game->cameraMatrix.multiplyAffine(moveVec);
				game->debugCameraPosition += moveVec;

				float sensitivity = 1;
				game->debugCameraRotation.z += -platform->relativeMouse.x * sensitivity*0.001;
				game->debugCameraRotation.x += -platform->relativeMouse.y * sensitivity*0.001;
				if (game->debugCameraRotation.x < 0) game->debugCameraRotation.x = 0;
				if (game->debugCameraRotation.x > M_PI) game->debugCameraRotation.x = M_PI;

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
			platform->useRelativeMouse = false;
		}

		WorldProps *world = defaultWorld;

		game->cameraMatrix = mat4();
		game->cameraMatrix = game->cameraMatrix.translate(game->debugCameraPosition);
		game->cameraMatrix = game->cameraMatrix.rotateEuler(game->debugCameraRotation);

		world->viewMatrix = game->cameraMatrix.invert();

		Matrix4 *projection = &world->projectionMatrix;
		projection->setIdentity();
		*projection = projection->perspective(60, platform->windowWidth/(float)platform->windowHeight, 0.1, FAR_PLANE);
	}

	clearRenderer();

	if (!game->gameFramebuffer) {
		game->gameFramebuffer = createFramebuffer();
		game->gameTexture = createTexture(platform->windowWidth, platform->windowHeight);

		setFramebuffer(game->gameFramebuffer);
		setColorAttachment(game->gameTexture, 0);
		addDepthAttachment(game->gameTexture->width, game->gameTexture->height);
		setFramebuffer(NULL);
	}

	setFramebuffer(game->gameFramebuffer);
	// pushTargetTexture(game->gameTexture);
	// attachDepthBuffer(game->gameTexture->width, game->gameTexture->height);
	clearRenderer(0xFF808080);
	// clearRenderer();

	{
		Model *model = getModel("assets/models/Cubes/Cubes.model");
		ModelProps props = newModelProps();
		drawModel(model, props);
	}

	// {
	// 	Matrix4 matrix = mat4();
	// 	matrix.SCALE(0.05);
	// 	Model *model = getModel(game->commonFbx, "Axis");
	// 	// drawModel(model, matrix);
	// }

	drawSphere(v3(15, 0, 0), 1, 0xFFFF0000);
	drawSphere(v3(0, 15, 0), 1, 0xFF00FF00);
	drawSphere(v3(0, 0, 15), 1, 0xFF0000FF);
	// AABB bounds;
	// bounds.min = v3(-1, -1, -1);
	// bounds.max = v3(1, 1, 1);
	// drawBoundsOutline(bounds, 0.01, 0xFFFFFF00);


	// defaultWorld->sunPosition.x = lerp(-1, 1, timePhase(platform->time*0.1)) * 10;
	// defaultWorld->sunPosition.y = 5;
	// defaultWorld->sunPosition.z = lerp(-1, 1, timePhase(platform->time*0.05)) * 20;
	defaultWorld->sunPosition = v3(lerp(-10, 10, timePhase(platform->time*0.2)), -24, 28);
	// drawSphere(defaultWorld->sunPosition, 1, 0xFFFFFF00);

	process3dDrawQueue();
	drawOnScreenLog();

	setFramebuffer(NULL);
	// popTargetTexture();

	Matrix3 mat = mat3();
	mat.SCALE(game->gameTexture->width, game->gameTexture->height);
	drawPost3dTexture(game->gameTexture, mat);
	// RenderProps props = newRenderProps();
	// drawTexture(game->gameTexture, props);
}
