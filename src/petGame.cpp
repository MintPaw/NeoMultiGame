struct Globals {
};

enum Activity {
	ACTIVITY_RESTING=0,
	ACTIVITY_WORKING=1,
	ACTIVITY_OBEDIENCE_TRAINING=2,
	ACTIVITY_ENDURANCE_TRAINING=3,
};
char *activityStrings[] = {
	"Resting",
	"Working",
	"Obedience training",
	"Endurance training",
};

enum Location {
	LOCATION_HOME=0,
	LOCATION_PET_SHOP=1,
	LOCATION_SITE1=2,
	LOCATION_SITE2=3,
	LOCATION_SITE3=4,
	LOCATION_DUMMY1=5,
	LOCATION_DUMMY2=6,
	LOCATION_DUMMY3=7,
	LOCATION_DUMMY4=8,
	LOCATION_DUMMY5=9,
	LOCATION_DUMMY6=10,
	LOCATION_DUMMY7=11,
	LOCATION_DUMMY8=12,
	LOCATION_DUMMY9=13,
	LOCATION_DUMMY10=14,
	LOCATION_DUMMY11=15,
	LOCATION_DUMMY12=16,
	LOCATION_DUMMY13=17,
	LOCATION_DUMMY14=18,
	LOCATION_DUMMY15=19,
	LOCATION_DUMMY16=20,

	LOCATION_ON_PLAYER=21,
	LOCATION_ITEM_SHOP=22,
};
char *locationStrings[] = {
	"Home",
	"Pet shop",
	"Site1",
	"Site2",
	"Site3",
	"LOCATION_DUMMY1",
	"LOCATION_DUMMY2",
	"LOCATION_DUMMY3",
	"LOCATION_DUMMY4",
	"LOCATION_DUMMY5",
	"LOCATION_DUMMY6",
	"LOCATION_DUMMY7",
	"LOCATION_DUMMY8",
	"LOCATION_DUMMY9",
	"LOCATION_DUMMY10",
	"LOCATION_DUMMY11",
	"LOCATION_DUMMY12",
	"LOCATION_DUMMY13",
	"LOCATION_DUMMY14",
	"LOCATION_DUMMY15",
	"LOCATION_DUMMY16",
	"On player",
	"Item shop",
};

struct Pet {
#define PET_NAME_MAX_LEN 32
	char name[PET_NAME_MAX_LEN];
	int color;
	int id;
	int hp;
	int xp;

	Activity activity;
	Location location;

	float charisma;
	float attraction;
	float endurance;
	float obedience;

	float price;
	float upkeep;
};

struct Worksite {
#define WORKSITE_NAME_MAX_LEN 32
	char name[WORKSITE_NAME_MAX_LEN];
	int capacity;
	int petCount;

	bool owned;
	float price;
	int clientsPerDay;

	bool hasShade;
	int frontAttractionsLevel;

	int obedienceTrainingRoomLevel;
	int enduranceTrainingRoomLevel;

	int petsInObedienceTrainingCount;
	int petsInEnduranceTrainingCount;

	// Unserialized
	int clientsLeft;
};

struct Player {
	int day;
	float money;
	float missionPrice;
};

enum ItemType {
	ITEM_BUILDING_SUPPLIES,
	ITEM_OBEDIENCE_TRAINING_PERMIT,
	ITEM_ENDURANCE_TRAINING_PERMIT,
};
char *itemTypeStrings[] = {
	"Building supplies",
	"Obedience training permit",
	"Endurance training permit",
};

struct Item {
	int id;
	ItemType type;

#define ITEM_NAME_MAX_LEN 32
	char name[ITEM_NAME_MAX_LEN]; // Why do items have names anyways?

	int amount;
	float price;
	Location location;
};

struct Game {
	Texture *gameTexture;

	Globals globals;

	Player player;

#define PETS_MAX 128
	Pet pets[PETS_MAX];
	int petsNum;

#define SITES_MAX 128
	Worksite sites[SITES_MAX];
	int sitesNum;

#define ITEMS_MAX 128
	Item items[ITEMS_MAX];
	int itemsNum;

	int selectedHomePet;
	int selectedShopPet;
	int selectedShopItem;

	DataStream *lastSaveStream;

	char **savePaths;
	int savePathsNum;

	bool debugMode;
	bool debugDidAction;
};

Game *game = NULL;

void runGame();
void updateGame();
bool tryMovePet(Pet *pet, Location location, Activity activity);
bool hasItem(ItemType type);
DataStream *saveGame();
void loadGame(DataStream *stream);
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

	initPlatform(1280, 720, "A pet game?");
	platform->sleepWait = true;
	initAudio();
	initRenderer(1280, 720);
	initMesh();
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

		// RegMem(Globals, );
		// loadStruct("Globals", "assets/info/globals.txt", &game->globals);

		RegMem(Pet, name);
		RegMem(Pet, color);
		RegMem(Pet, id);
		RegMem(Pet, hp);
		RegMem(Pet, xp);
		RegMem(Pet, activity);
		RegMem(Pet, location);
		RegMem(Pet, charisma);
		RegMem(Pet, attraction);
		RegMem(Pet, endurance);
		RegMem(Pet, obedience);
		RegMem(Pet, price);
		RegMem(Pet, upkeep);

		RegMem(Worksite, name);
		RegMem(Worksite, capacity);
		RegMem(Worksite, petCount);
		RegMem(Worksite, owned);
		RegMem(Worksite, price);
		RegMem(Worksite, clientsPerDay);
		RegMem(Worksite, hasShade);
		RegMem(Worksite, frontAttractionsLevel);
		RegMem(Worksite, obedienceTrainingRoomLevel);
		RegMem(Worksite, enduranceTrainingRoomLevel);
		RegMem(Worksite, petsInObedienceTrainingCount);
		RegMem(Worksite, petsInEnduranceTrainingCount);

		RegMem(Player, day);
		RegMem(Player, money);
		RegMem(Player, missionPrice);

		RegMem(Item, id);
		RegMem(Item, type);
		RegMem(Item, name);
		RegMem(Item, location);
		RegMem(Item, amount);
		RegMem(Item, price);
		RegMem(Item, location);

		loadStructArray("Pet", "assets/info/defaultPets.txt", game->pets, PETS_MAX, sizeof(Pet)); //@copyPastedLoadPets
		for (int i = 0; i < PETS_MAX; i++) {
			Pet *pet = &game->pets[i];
			pet->hp = 100;
			if (pet->name[0]) game->petsNum = i+1;
		}

		loadStructArray("Worksite", "assets/info/defaultSites.txt", game->sites, SITES_MAX, sizeof(Worksite)); //@copyPastedLoadSites
		for (int i = 0; i < SITES_MAX; i++) {
			Worksite *site = &game->sites[i];
			if (site->name[0]) game->sitesNum = i+1;
		}

		loadStructArray("Item", "assets/info/defaultItems.txt", game->items, ITEMS_MAX, sizeof(Item)); //@copyPastedLoadItems
		for (int i = 0; i < ITEMS_MAX; i++) {
			Item *item = &game->items[i];
			if (item->name[0]) game->itemsNum = i+1;
		}

		game->player.money = 10;
		game->sites[0].owned = true;
		game->player.missionPrice = 500;

#if 1
		float BPB_RootEntCnt = 0;
		float BPB_BytsPerSec = 512;
		u32 rootDirSectors = ceilf(((BPB_RootEntCnt * 32.0) + (BPB_BytsPerSec - 1.0)) / BPB_BytsPerSec);
		logf("Root: %d\n", rootDirSectors);
		exit(1);
#endif
	}

	Globals *globals = &game->globals;
	float elapsed = platform->elapsed;
	float secondPhase = timePhase(platform->time);
	Player *player = &game->player;

	if (!game->gameTexture) game->gameTexture = createTexture(platform->windowWidth, platform->windowHeight);
	pushTargetTexture(game->gameTexture);
	clearRenderer();

	if (keyJustPressed(KEY_BACKTICK)) game->debugMode = !game->debugMode;
	if (game->debugMode) {
		ImGui::Begin("Debug", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		if (ImGui::Button("Give $100")) player->money += 100;

		if (ImGui::TreeNode("ImGui demo")) {
			ImGui::ShowDemoWindow();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Plot demo")) {
			ImPlot::ShowDemoWindow();
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Generate a pet")) {
			static int sPointsToGive = 4;
			ImGui::InputInt("Points to give", &sPointsToGive);
			if (ImGui::Button("Generate")) {

				int pointsLeft = sPointsToGive;
				Pet dummyPet = {};
				Pet *pet = &dummyPet;

				pet->charisma = 1;
				pet->attraction = 1;
				pet->endurance = 1;
				pet->obedience = 1;
				pointsLeft -= 4;

				for (int i = 0; i < pointsLeft; i++) {
					int choice = rndInt(0, 3);
					if (choice == 0) {
						pet->charisma++;
					} else if (choice == 1) {
						pet->attraction++;
					} else if (choice == 2) {
						pet->endurance++;
					} else if (choice == 3) {
						pet->obedience++;
					}
				}

				logf("Generated pet:\n");
				logf("Charisma: %.2f\n", pet->charisma);
				logf("Attraction: %.2f\n", pet->attraction);
				logf("Endurance: %.2f\n", pet->endurance);
				logf("Obedience: %.2f\n", pet->obedience);
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Pet editor")) {
			if (game->debugDidAction) {
				ImGui::Text("You did an action");
			} else {
				if (ImGui::Button("Save default pets")) saveStructArray("Pet", "assets/info/defaultPets.txt", game->pets, game->petsNum, sizeof(Pet));
				ImGui::SameLine();
				if (ImGui::Button("Load default pets")) {
					loadStructArray("Pet", "assets/info/defaultPets.txt", game->pets, PETS_MAX, sizeof(Pet)); //@copyPastedLoadPets
					for (int i = 0; i < PETS_MAX; i++) {
						Pet *pet = &game->pets[i];
						pet->hp = 100;
						if (pet->name[0]) game->petsNum = i+1;
					}
				}
			}

			if (ImGui::BeginTable("petTable", 11, ImGuiTableFlags_SizingStretchProp)) {
				ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 100);
				ImGui::TableSetupColumn("Color", ImGuiTableColumnFlags_WidthFixed, 100);
				ImGui::TableSetupColumn("Id", ImGuiTableColumnFlags_WidthFixed, 100);
				ImGui::TableSetupColumn("Activity", ImGuiTableColumnFlags_WidthFixed, 100);
				ImGui::TableSetupColumn("Location", ImGuiTableColumnFlags_WidthFixed, 100);
				ImGui::TableSetupColumn("Charisma", ImGuiTableColumnFlags_WidthFixed, 100);
				ImGui::TableSetupColumn("Attraction", ImGuiTableColumnFlags_WidthFixed, 100);
				ImGui::TableSetupColumn("Endurance", ImGuiTableColumnFlags_WidthFixed, 100);
				ImGui::TableSetupColumn("Obedience", ImGuiTableColumnFlags_WidthFixed, 100);
				ImGui::TableSetupColumn("Price", ImGuiTableColumnFlags_WidthFixed, 100);
				ImGui::TableSetupColumn("Upkeep", ImGuiTableColumnFlags_WidthFixed, 100);
				ImGui::TableHeadersRow();

				for (int i = 0; i < game->petsNum; i++) {
					Pet *pet = &game->pets[i];
					ImGui::PushID(i);
					// logf("Before color: %08x, after color: %08x\n", pet->color, argbToRgba(pet->color));
					guiPushStyleColor(ImGuiCol_FrameBg, lerpColor(pet->color, 0xFF000000, 0.50));

					ImGui::TableNextColumn();
					ImGui::InputText("###Name", pet->name, PET_NAME_MAX_LEN);
					ImGui::TableNextColumn();
					guiInputRgb("###Color", &pet->color);
					ImGui::TableNextColumn();
					ImGui::InputInt("###Id", &pet->id, 0);
					ImGui::TableNextColumn();
					ImGui::Combo("###Activity", (int *)&pet->activity, activityStrings, ArrayLength(activityStrings));
					ImGui::TableNextColumn();
					ImGui::Combo("###Location", (int *)&pet->location, locationStrings, ArrayLength(locationStrings));
					ImGui::TableNextColumn();
					ImGui::InputFloat("###Charisma", &pet->charisma);
					ImGui::TableNextColumn();
					ImGui::InputFloat("###Attraction", &pet->attraction);
					ImGui::TableNextColumn();
					ImGui::InputFloat("###Endurance", &pet->endurance);
					ImGui::TableNextColumn();
					ImGui::InputFloat("###Obedience", &pet->obedience);
					ImGui::TableNextColumn();
					ImGui::InputFloat("###Price", &pet->price);
					ImGui::TableNextColumn();
					ImGui::InputFloat("###Upkeep", &pet->upkeep);

					guiPopStyleColor();
					ImGui::PopID();
				}
				ImGui::EndTable();
			}

			if (ImGui::Button("Add pet")) {
				if (game->petsNum < PETS_MAX) game->petsNum++;
			}

			ImGui::SameLine();
			if (ImGui::Button("Remove pet")) {
				if (game->petsNum > 0) game->petsNum--;
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Worksite editor")) {
			if (game->debugDidAction) {
				ImGui::Text("You did an action");
			} else {
				if (ImGui::Button("Save Worksites")) saveStructArray("Worksite", "assets/info/defaultSites.txt", game->sites, game->sitesNum, sizeof(Worksite));
				ImGui::SameLine();
				if (ImGui::Button("Load Worksites")) {
					loadStructArray("Worksite", "assets/info/defaultSites.txt", game->sites, SITES_MAX, sizeof(Worksite)); //@copyPastedLoadSites
					for (int i = 0; i < SITES_MAX; i++) {
						Worksite *site = &game->sites[i];
						if (site->name[0]) game->sitesNum = i+1;
					}
				}
			}

			for (int i = 0; i < game->sitesNum; i++) {
				ImGui::PushID(i);
				Worksite *site = &game->sites[i];

				ImGui::InputText("Name", site->name, WORKSITE_NAME_MAX_LEN);
				ImGui::InputInt("Capacity", &site->capacity);
				ImGui::InputFloat("Price", &site->price);
				ImGui::InputInt("Clients per day", &site->clientsPerDay, 0);
				ImGui::Separator();

				ImGui::PopID();
			}

			if (ImGui::Button("Add worksite")) {
				if (game->sitesNum < SITES_MAX) game->sitesNum++;
			}

			ImGui::SameLine();
			if (ImGui::Button("Remove worksite")) {
				if (game->sitesNum > 0) game->sitesNum--;
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Item editor")) {
			if (game->debugDidAction) {
				ImGui::Text("You did an action");
			} else {
				if (ImGui::Button("Save Items")) saveStructArray("Item", "assets/info/defaultItems.txt", game->items, game->itemsNum, sizeof(Item));
				ImGui::SameLine();
				if (ImGui::Button("Load Items")) {
					loadStructArray("Item", "assets/info/defaultItems.txt", game->items, ITEMS_MAX, sizeof(Item)); //@copyPastedLoadItems
					for (int i = 0; i < ITEMS_MAX; i++) {
						Item *item = &game->items[i];
						if (item->name[0]) game->itemsNum = i+1;
					}
				}
			}

			for (int i = 0; i < game->itemsNum; i++) {
				ImGui::PushID(i);
				Item *item = &game->items[i];

				ImGui::InputInt("Id", &item->id);
				ImGui::Combo("Type", (int *)&item->type, itemTypeStrings, ArrayLength(itemTypeStrings));
				ImGui::InputText("Name", item->name, ITEM_NAME_MAX_LEN);
				ImGui::InputInt("Amount", &item->amount);
				ImGui::InputFloat("Price", &item->price);
				ImGui::Combo("Location", (int *)&item->location, locationStrings, ArrayLength(locationStrings));
				ImGui::Separator();

				ImGui::PopID();
			}

			if (ImGui::Button("Add item")) {
				if (game->itemsNum < ITEMS_MAX) game->itemsNum++;
			}

			ImGui::SameLine();
			if (ImGui::Button("Remove item")) {
				if (game->itemsNum > 0) game->itemsNum--;
			}
			ImGui::TreePop();
		}

		ImGui::End();
	}

	{
		ImGui::SetNextWindowPos(ImVec2(0.2*platform->windowWidth, 0.5*platform->windowHeight), ImGuiCond_Once, ImVec2(0.5, 0.5));
		ImGui::Begin("Home", NULL, ImGuiWindowFlags_AlwaysAutoResize);

		if (ImGui::TreeNode("Save/Load")) {
			static char *sSaveAsName = (char *)zalloc(PATH_MAX_LEN);
			ImGui::InputText("Save as", sSaveAsName, PATH_MAX_LEN);
			ImGui::SameLine();
			if (ImGui::Button("Save")) {
				if (game->lastSaveStream) {
					writeDataStream(frameSprintf("assets/saves/%s", sSaveAsName), game->lastSaveStream);
				} else {
					logf("Can't save on day 0\n");
				}
			}

			ImGui::Separator();

			// char **getDirectoryList(const char *dirPath, int *numFiles, bool includingUnderscored=false, bool shallow=false, bool includeFolders=false);
			if (game->savePathsNum == 0 || platform->frameCount % (60*5) == 0) {
				if (game->savePaths) {
					for (int i = 0; i < game->savePathsNum; i++) {
						free(game->savePaths[i]);
					}
					free(game->savePaths);
				}

				game->savePaths = getDirectoryList("assets/saves", &game->savePathsNum);
			}

			for (int i = 0; i < game->savePathsNum; i++) {
				char *path = game->savePaths[i];
				if (ImGui::Button(path)) {
					DataStream *stream = loadDataStream(path);
					loadGame(stream);
					destroyDataStream(stream);
				}

				ImGui::SameLine();
				if (ImGui::Button("Delete")) {
					deleteFile(path);
				}
			}

			ImGui::TreePop();
		}

		ImGui::Text("Day: %d\n", player->day);
		ImGui::Text("Money: %.2f\n", player->money);
		ImGui::Separator();

		Pet *homePets[PETS_MAX];
		int homePetsNum = 0;
		for (int i = 0; i < game->petsNum; i++) {
			Pet *pet = &game->pets[i];
			if (
				pet->location == LOCATION_HOME ||
				pet->location == LOCATION_SITE1 ||
				pet->location == LOCATION_SITE2 ||
				pet->location == LOCATION_SITE3
			) {
				homePets[homePetsNum++] = pet;
			}
		}

		for (int i = 0; i < homePetsNum; i++) {
			Pet *pet = homePets[i];

			guiPushStyleColor(ImGuiCol_Header, lerpColor(pet->color, 0xFF000000, 0.5));
			guiPushStyleColor(ImGuiCol_HeaderHovered, pet->color);

			char *label = frameSprintf("%s %dhp (%s)", pet->name, pet->hp, locationStrings[pet->location]);
			if (ImGui::Selectable(label, game->selectedHomePet == i)) {
				game->selectedHomePet = i;
			}

			guiPopStyleColor(2);
		}

		ImGui::Separator();

		Pet *pet = NULL;
		if (homePetsNum > 0) {
			if (game->selectedHomePet > homePetsNum-1) game->selectedHomePet = homePetsNum-1;
			pet = homePets[game->selectedHomePet];
		}

		if (pet) {
			ImGui::Text("Name: %s", pet->name);
			ImGui::Text("Hp: %d", pet->hp);
			ImGui::Text("Xp: %d", pet->xp);
			if (game->debugMode && ImGui::Button("Get 100xp")) pet->xp += 100;

			ImGui::Text("Charisma: %.2f", pet->charisma);
			if (pet->charisma < 10) {
				ImGui::SameLine(120);
				ImGui::Button("+###levelUpCharisma");
				int xpCost = pet->charisma;
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Costs %dxp", xpCost);
				if (ImGui::IsItemClicked()) {
					if (pet->xp >= xpCost) {
						pet->xp -= xpCost;
						pet->charisma += 0.1;
						if (pet->charisma > 10) pet->charisma = 10;
					} else {
						logf("Not enough xp\n");
					}
				}
			}

			ImGui::Text("Attraction: %.2f", pet->attraction);
			if (pet->attraction < 10) {
				ImGui::SameLine(120);
				ImGui::Button("+###levelUpAttraction");
				int xpCost = pet->attraction;
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Costs %dxp", xpCost);
				if (ImGui::IsItemClicked()) {
					if (pet->xp >= xpCost) {
						pet->xp -= xpCost;
						pet->attraction += 0.1;
						if (pet->attraction > 10) pet->attraction = 10;
					} else {
						logf("Not enough xp\n");
					}
				}
			}

			ImGui::Text("Endurance: %.2f", pet->endurance);
			if (pet->endurance < 10) {
				ImGui::SameLine(120);
				ImGui::Button("+###levelUpEndurance");
				int xpCost = pet->endurance;
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Costs %dxp", xpCost);
				if (ImGui::IsItemClicked()) {
					if (pet->xp >= xpCost) {
						pet->xp -= xpCost;
						pet->endurance += 0.1;
						if (pet->endurance > 10) pet->endurance = 10;
					} else {
						logf("Not enough xp\n");
					}
				}
			}

			ImGui::Text("Obedience: %.2f", pet->obedience);
			if (pet->obedience < 10) {
				ImGui::SameLine(120);
				ImGui::Button("+###levelUpObedience");
				int xpCost = pet->obedience;
				if (ImGui::IsItemHovered()) ImGui::SetTooltip("Costs %dxp", xpCost);
				if (ImGui::IsItemClicked()) {
					if (pet->xp >= xpCost) {
						pet->xp -= xpCost;
						pet->obedience += 0.1;
						if (pet->obedience > 10) pet->obedience = 10;
					} else {
						logf("Not enough xp\n");
					}
				}
			}

			ImGui::Separator();
			ImGui::Text("Location: %s", locationStrings[pet->location]);
			ImGui::Text("Activity: %s", activityStrings[pet->activity]);

			if (ImGui::Button("Send to home to rest")) {
				tryMovePet(pet, LOCATION_HOME, ACTIVITY_RESTING);
			}

			for (int i = 0; i < game->sitesNum; i++) {
				Worksite *site = &game->sites[i];
				if (!site->owned) continue;

				Location location = (Location)(LOCATION_SITE1+i);

				ImGui::PushID(i);

				if (ImGui::Button("Work")) tryMovePet(pet, location, ACTIVITY_WORKING);

				if (site->obedienceTrainingRoomLevel > 0) {
					ImGui::SameLine();
					if (ImGui::Button("Train obedience")) tryMovePet(pet, location, ACTIVITY_OBEDIENCE_TRAINING);
				}

				if (site->enduranceTrainingRoomLevel > 0) {
					ImGui::SameLine();
					if (ImGui::Button("Train endurance")) tryMovePet(pet, location, ACTIVITY_ENDURANCE_TRAINING);
				}
				ImGui::PopID();
			}
		}

		ImGui::Separator();
		if (ImGui::Button("Next day")) {
			game->debugDidAction = true;
			for (int i = 0; i < game->sitesNum; i++) {
				Worksite *site = &game->sites[i];
				site->clientsLeft = site->clientsPerDay;
				if (site->frontAttractionsLevel == 1) site->clientsLeft++;
			}

			for (int i = 0; i < game->petsNum; i++) {
				Pet *pet = &game->pets[i];

				if (pet->location == LOCATION_PET_SHOP) continue;

				if (pet->activity == ACTIVITY_WORKING) {
					Worksite *site = NULL;
					if (pet->location == LOCATION_SITE1) {
						site = &game->sites[0];
					} else if (pet->location == LOCATION_SITE2) {
						site = &game->sites[1];
					} else if (pet->location == LOCATION_SITE3) {
						site = &game->sites[2];
					} else {
						logf("%s is working at invalid place %s\n", pet->name, locationStrings[pet->location]);
					}

					float workChance = pow(pet->obedience/10.0, 0.2);
					bool worked = false;
					if (rndPerc(workChance)) {
						worked = true;
					}

					if (worked) {
						int hpToLose = clampMap(pet->endurance, 0, 10, 35, 5);
						int clientsToSee = clampMap(pet->attraction, 0, 10, 1, 6);
						float moneyToGainPerClient = pet->charisma;

						if (clientsToSee > site->clientsLeft) clientsToSee = site->clientsLeft;
						site->clientsLeft -= clientsToSee;

						if (site->hasShade) hpToLose *= 0.5;
						if (hpToLose < 1) hpToLose = 1;

						pet->hp -= hpToLose;
						pet->xp += clientsToSee;
						player->money += clientsToSee * moneyToGainPerClient;
						player->money -= pet->upkeep;

						logf("%s saw %d clients for %.2f each (%.2f)\n", pet->name, clientsToSee, moneyToGainPerClient, clientsToSee*moneyToGainPerClient);

						if (pet->hp < 0) {
							pet->hp *= 2;
							tryMovePet(pet, LOCATION_HOME, ACTIVITY_RESTING);
							logf("%s got sick\n", pet->name);
						}
					} else {
						logf("%s refused to work today\n", pet->name);
						player->money -= pet->upkeep;
					}
				} else if (pet->activity == ACTIVITY_RESTING) {
					player->money -= pet->upkeep;
					pet->hp += 20;
					if (pet->hp > 100) pet->hp = 100;
				} else if (pet->activity == ACTIVITY_OBEDIENCE_TRAINING) {
					player->money -= 10;
					pet->obedience += 0.5;
					if (pet->obedience > 10) pet->obedience = 10; //@todo Make this cap flexible
				} else if (pet->activity == ACTIVITY_ENDURANCE_TRAINING) {
					player->money -= 5;
					pet->endurance += 0.5;
					if (pet->endurance > 10) pet->endurance = 10; //@todo Make this cap flexible
				}
			}

			for (int i = 0; i < game->sitesNum; i++) {
				Worksite *site = &game->sites[i];
				if (!site->owned) continue;
				if (site->clientsLeft > 0) logf("%s still had %d clients left\n", site->name, site->clientsLeft);
			}

			player->day++;

			if (game->lastSaveStream) destroyDataStream(game->lastSaveStream);
			game->lastSaveStream = saveGame();
			writeDataStream("assets/saves/autosave", game->lastSaveStream);
		}

		ImGui::Separator();
		for (int i = 0; i < game->sitesNum; i++) {
			Worksite *site = &game->sites[i];
			if (site->owned) continue;
			ImGui::Button(frameSprintf("Buy %s ($%.2f, capacity: %d)", site->name, site->price, site->capacity));
			if (ImGui::IsItemClicked()) {
				game->debugDidAction = true;
				if (player->money >= site->price) {
					player->money -= site->price;
					site->owned = true;
				} else {
					logf("Not enough money!\n");
				}
			}
		}

		ImGui::Separator();
		if (player->missionPrice) {
			int dayToComplete = ceilToNearest(player->day, 30);
			if (dayToComplete == 0) dayToComplete = 30;
			ImGui::Text("Mission, pay $%.2f by day %d", player->missionPrice, dayToComplete);
			if (ImGui::Button("Pay")) {
				if (player->money >= player->missionPrice) {
					player->money -= player->missionPrice;
					//@todo Give mission pet
				} else {
					logf("Not enough money");
				}
			}
		} else {
			ImGui::Text("There is no mission right now");
		}
		ImGui::End();
	}

	{
		ImGui::SetNextWindowPos(ImVec2(0.8*platform->windowWidth, 0.6*platform->windowHeight), ImGuiCond_Once, ImVec2(0.5, 0.5));
		ImGui::Begin("Pet shop", NULL, ImGuiWindowFlags_AlwaysAutoResize);

		Pet *shopPets[PETS_MAX];
		int shopPetsNum = 0;
		for (int i = 0; i < game->petsNum; i++) {
			Pet *pet = &game->pets[i];
			if (pet->location == LOCATION_PET_SHOP) {
				shopPets[shopPetsNum++] = pet;
			}
		}

		for (int i = 0; i < shopPetsNum; i++) {
			Pet *pet = shopPets[i];

			guiPushStyleColor(ImGuiCol_Header, lerpColor(pet->color, 0xFF000000, 0.5));
			guiPushStyleColor(ImGuiCol_HeaderHovered, pet->color);

			char *label = frameSprintf("%s $%.2f", pet->name, pet->price);
			if (ImGui::Selectable(label, game->selectedShopPet == i)) {
				game->selectedShopPet = i;
			}

			guiPopStyleColor(2);
		}

		ImGui::Separator();

		Pet *pet = NULL;
		if (shopPetsNum > 0) {
			if (game->selectedShopPet > shopPetsNum-1) game->selectedShopPet = shopPetsNum-1;
			pet = shopPets[game->selectedShopPet];
		}

		if (pet) {
			ImGui::Text("Name: %s", pet->name);
			ImGui::Text("Price: %.2f", pet->price);
			ImGui::Separator();
			ImGui::Text("Charisma: %.2f", pet->charisma);
			ImGui::Text("Attraction: %.2f", pet->attraction);
			ImGui::Text("Endurance: %.2f", pet->endurance);
			ImGui::Text("Obedience: %.2f", pet->obedience);

			if (ImGui::Button("Buy")) {
				game->debugDidAction = true;
				if (player->money >= pet->price) {
					player->money -= pet->price;
					tryMovePet(pet, LOCATION_HOME, ACTIVITY_RESTING);
				} else {
					logf("Not enough money!\n");
				}
			}
		}
		ImGui::End();
	}

	{
		ImGui::SetNextWindowPos(ImVec2(0.8*platform->windowWidth, 0.1*platform->windowHeight), ImGuiCond_Once, ImVec2(0.5, 0.5));
		ImGui::Begin("Item shop", NULL, ImGuiWindowFlags_AlwaysAutoResize);

		Item *shopItems[ITEMS_MAX];
		int shopItemsNum = 0;

		for (int i = 0; i < game->itemsNum; i++) {
			Item *item = &game->items[i];

			if (item->location == LOCATION_ITEM_SHOP) {
				shopItems[shopItemsNum++] = item;
			}
		}

		for (int i = 0; i < shopItemsNum; i++) {
			Item *item = shopItems[i];

			char *label = frameSprintf("%s $%.2f", item->name, item->price);
			if (ImGui::Selectable(label, game->selectedShopItem == i)) {
				game->selectedShopItem = i;
			}
		}

		ImGui::Separator();

		Item *item = NULL;
		if (shopItemsNum > 0) {
			if (game->selectedShopItem > shopItemsNum-1) game->selectedShopItem = shopItemsNum-1;
			item = shopItems[game->selectedShopItem];
		}

		if (item) {
			ImGui::Text("Allows you to build addons on Worksites");

			if (ImGui::Button("Buy")) {
				game->debugDidAction = true;
				if (player->money >= item->price) {
					player->money -= item->price;
					item->location = LOCATION_ON_PLAYER;
				} else {
					logf("Not enough money!\n");
				}
			}
		}
		ImGui::End();
	}

	{
		Vec2 siteWindowPos = v2(0.4, 0.15);
		Vec2 screenSize = v2(platform->windowWidth, platform->windowHeight);

		for (int i = 0; i < game->sitesNum; i++) {
			Worksite *site = &game->sites[i];
			if (!site->owned) continue;

			Vec2 pos = siteWindowPos * screenSize;
			ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y), ImGuiCond_Once, ImVec2(0.5, 0.5));
			ImGui::Begin(frameSprintf("Worksite: %s", site->name), NULL, ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::Text("Capacity: %d/%d", site->petCount, site->capacity);

			if (hasItem(ITEM_BUILDING_SUPPLIES)) {
				if (!site->hasShade) {
					float price = site->price * 0.33;
					ImGui::Button(frameSprintf("Add shaded enclosures ($%.2f)", price));
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Birds working here take 50%% less damage.");
					if (ImGui::IsItemClicked()) {
						if (player->money >= price) {
							player->money -= price;
							site->hasShade = true;
						} else {
							logf("Not enough money!\n");
						}
					}
				}

				if (site->frontAttractionsLevel == 0) {
					float price = site->price * 1.25;
					ImGui::Button(frameSprintf("Add front attraction ($%.2f)", price));
					if (ImGui::IsItemHovered()) ImGui::SetTooltip("Will attract 1 more clients per day.");
					if (ImGui::IsItemClicked()) {
						if (player->money >= price) {
							player->money -= price;
							site->frontAttractionsLevel = 1;
						} else {
							logf("Not enough money!\n");
						}
					}
				}
			}

			if (hasItem(ITEM_OBEDIENCE_TRAINING_PERMIT)) {
				if (site->obedienceTrainingRoomLevel == 0) {
					float price = site->price * 0.25;
					ImGui::Button(frameSprintf("Add obedience training room ($%.2f)", price));
					if (ImGui::IsItemClicked()) {
						if (player->money >= price) {
							player->money -= price;
							site->obedienceTrainingRoomLevel = 1;
						} else {
							logf("Not enough money!\n");
						}
					}
				}
			}

			if (hasItem(ITEM_ENDURANCE_TRAINING_PERMIT)) {
				if (site->enduranceTrainingRoomLevel == 0) {
					float price = site->price * 0.25;
					ImGui::Button(frameSprintf("Add endurance training room ($%.2f)", price));
					if (ImGui::IsItemClicked()) {
						if (player->money >= price) {
							player->money -= price;
							site->enduranceTrainingRoomLevel = 1;
						} else {
							logf("Not enough money!\n");
						}
					}
				}
			}

			ImGui::End();

			siteWindowPos.y += 0.15;
		}
	}

	popTargetTexture();

	clearRenderer();
	{
		Matrix3 matrix = mat3();
		drawPost3dTexture(game->gameTexture, matrix);
	}

	drawOnScreenLog();
}

bool canMovePet(Pet *pet, Location location, Activity activity);
bool canMovePet(Pet *pet, Location location, Activity activity) {
	Worksite *site = NULL;

	if (location == LOCATION_SITE1) {
		site = &game->sites[0];
	} else if (location == LOCATION_SITE2) {
		site = &game->sites[1];
	} else if (location == LOCATION_SITE3) {
		site = &game->sites[2];
	}

	if (site) {
		if (activity == ACTIVITY_OBEDIENCE_TRAINING) return site->petsInObedienceTrainingCount < site->obedienceTrainingRoomLevel;
		if (activity == ACTIVITY_ENDURANCE_TRAINING) return site->petsInEnduranceTrainingCount < site->enduranceTrainingRoomLevel;
		if (activity == ACTIVITY_WORKING) return site->petCount < site->capacity;
	}

	return true;
}

void removePet(Pet *pet);
void removePet(Pet *pet) {
	Worksite *site = NULL;

	if (pet->location == LOCATION_SITE1) {
		site = &game->sites[0];
	} else if (pet->location == LOCATION_SITE2) {
		site = &game->sites[1];
	} else if (pet->location == LOCATION_SITE3) {
		site = &game->sites[2];
	}

	if (site) {
		if (pet->activity == ACTIVITY_OBEDIENCE_TRAINING) site->petsInObedienceTrainingCount--;
		if (pet->activity == ACTIVITY_ENDURANCE_TRAINING) site->petsInEnduranceTrainingCount--;
		if (pet->activity == ACTIVITY_WORKING) site->petCount--;
	}

	pet->location = LOCATION_HOME;
	pet->activity = ACTIVITY_RESTING;
}

void addPet(Pet *pet, Location location, Activity activity);
void addPet(Pet *pet, Location location, Activity activity) {
	Worksite *site = NULL;

	if (location == LOCATION_SITE1) {
		site = &game->sites[0];
	} else if (location == LOCATION_SITE2) {
		site = &game->sites[1];
	} else if (location == LOCATION_SITE3) {
		site = &game->sites[2];
	}

	if (site) {
		if (activity == ACTIVITY_OBEDIENCE_TRAINING) site->petsInObedienceTrainingCount++;
		if (activity == ACTIVITY_ENDURANCE_TRAINING) site->petsInEnduranceTrainingCount++;
		if (activity == ACTIVITY_WORKING) site->petCount++;
	}

	pet->location = location;
	pet->activity = activity;
}

bool tryMovePet(Pet *pet, Location location, Activity activity) {
	if (!canMovePet(pet, location, activity)) {
		logf("No room!\n");
		return false;
	}

	removePet(pet);
	addPet(pet, location, activity);
	return true;
}

bool hasItem(ItemType type) {
	for (int i = 0; i < game->itemsNum; i++) {
		Item *item = &game->items[i];
		if (item->amount == 0) continue;
		if (item->location == LOCATION_ITEM_SHOP) continue;
		if (item->type == type) return true;
	}

	return false;
}

DataStream *saveGame() {
	DataStream *stream = newDataStream();

	int saveVersion = 0;
	writeU32(stream, saveVersion);

	char *playerString = saveStructString("Player", &game->player);
	writeString(stream, playerString);
	free(playerString);

	char *petsString = saveStructArrayString("Pet", game->pets, game->petsNum, sizeof(Pet));
	writeString(stream, petsString);
	free(petsString);

	char *worksitesString = saveStructArrayString("Worksite", game->sites, game->sitesNum, sizeof(Worksite));
	writeString(stream, worksitesString);
	free(worksitesString);

	char *itemsString = saveStructArrayString("Item", game->items, game->itemsNum, sizeof(Item));
	writeString(stream, itemsString);
	free(itemsString);

	return stream;
}

void loadGame(DataStream *stream) {
	int saveVersion = readU32(stream);

	char *playerString = readString(stream);
	loadStructString("Player", playerString, &game->player);
	free(playerString);

	char *petsString = readString(stream);
	loadStructArrayString("Pet", petsString, game->pets, PETS_MAX, sizeof(Pet));
	free(petsString);

	char *worksitesString = readString(stream);
	loadStructArrayString("Worksite", worksitesString, game->sites, SITES_MAX, sizeof(Worksite));
	free(worksitesString);

	char *itemsString = readString(stream);
	loadStructArrayString("Item", itemsString, game->items, ITEMS_MAX, sizeof(Item));
	free(itemsString);
}
