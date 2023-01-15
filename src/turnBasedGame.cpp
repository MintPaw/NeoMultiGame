struct Globals {
};

enum Weapon {
	WEAPON_NONE,
	WEAPON_DEFAULT,
	WEAPON_VAMPIRE,
	WEAPON_BIG_DAMAGE,
	WEAPON_MAGIC_RESIST,
	WEAPON_LOW_HP,
	WEAPON_DODGE,
	WEAPON_POISON,
	WEAPON_GLASS,
};
char *weaponStrings[] = {
	"None",
	"Default",
	"Vampire",
	"Big damage",
	"Magic resist",
	"Low hp",
	"Dodge",
	"Poison",
	"Glass",
};

enum BuffType {
	BUFF_NONE,
	BUFF_COMBO,
	BUFF_QUAKED,
	BUFF_PHYS_UP,
	BUFF_BODY_BLOCKING,
	BUFF_BLEED,
	BUFF_POISON,
	BUFF_ADD_POISON,
	BUFF_ADD_MANA_SIPHON,
	BUFF_ADD_ATTACK_REDUCTION,
	BUFF_ADD_DEFENSE_REDUCTION,
	BUFF_ATTACK_REDUCTION,
	BUFF_DEFENSE_REDUCTION,
	BUFF_RELIFE,
	BUFF_ANTI_BUFF,
	BUFF_AT_MOON,
	BUFF_FROZEN,
	BUFF_TYPES_MAX,
};
struct BuffTypeInfo {
#define BUFF_NAME_MAX_LEN 64
	char name[BUFF_NAME_MAX_LEN];
};
struct Buff {
	int id;
	BuffType type;
	BuffTypeInfo *info;

	int turns;

	int intUserData;
};

enum TargetType {
	TARGET_SINGLE,
	TARGET_NONE,
};

enum SpellType {
	SPELL_NONE,

	SPELL_HERO_ATTACK,
	SPELL_SMALL_ATTACK,
	SPELL_MEDIUM_ATTACK,
	SPELL_LARGE_ATTACK,

	SPELL_QUICK_ATTACK,
	SPELL_WIDE_STRIKE,
	SPELL_COMBO_ATTACK,
	SPELL_EARTHQUAKE,
	SPELL_PHYS_UP,
	SPELL_BODY_BLOCK,

	SPELL_DRAW_DEFAULT,
	SPELL_DRAW_VAMPIRE,
	SPELL_DRAW_BIG_DAMAGE,
	SPELL_DRAW_MAGIC_RESIST,
	SPELL_DRAW_LOW_HP,
	SPELL_DRAW_DODGE,
	SPELL_DRAW_POISON,
	SPELL_DRAW_GLASS,

	SPELL_SWIPE,
	SPELL_ADD_POISON,
	SPELL_ADD_MANA_SIPHON,
	SPELL_ADD_ATTACK_REDUCTION,
	SPELL_ADD_DEFENSE_REDUCTION,

	SPELL_HEAL,
	SPELL_HEAL2,
	SPELL_RESURRECT,
	SPELL_GIVE_MANA,
	SPELL_ANTI_BUFF,
	SPELL_MOON_STRIKE,
	SPELL_MOON_STRIKE_RAID,
	SPELL_LIGHTNING,
	SPELL_ICE,

	SPELL_SMALL_ETHER,
	SPELL_LARGE_ETHER,

	SPELL_WAIT,

	SPELL_END_TURN,

	SPELL_TYPES_MAX,
};
struct SpellTypeInfo {
#define SPELL_TYPE_NAME_MAX_LEN 64
	char name[SPELL_TYPE_NAME_MAX_LEN];
	TargetType targetType;
	int damage;
	int mp;

	bool canTargetAllies;
	bool canTargetDead;
};
struct Spell {
	SpellType type;
	int id;
	SpellTypeInfo *info;
	int srcId;
	int destId;
};

enum UnitType {
	UNIT_PLAYER1,
	UNIT_PLAYER2,
	UNIT_STANDARD_A,
	UNIT_STANDARD_B,
	UNIT_STANDARD_C,
	UNIT_SWIFT,
	UNIT_TYPES_MAX,
};
struct UnitTypeInfo {
#define UNIT_TYPE_NAME_MAX_LEN 64
	char name[UNIT_TYPE_NAME_MAX_LEN];

	int maxHp;
	int maxMp;
	float dodgeChance;
};
struct Unit {
	UnitType type;
	int id;
	UnitTypeInfo *info;

	Weapon weapon;

#define SCREEN_NAME_MAX_LEN 128
	char screenName[SCREEN_NAME_MAX_LEN];

	bool ally;
	int hp;
	int mp;

#define SPELLS_AVAILABLE_MAX 128
	SpellType spellsAvailable[SPELLS_AVAILABLE_MAX];
	int spellsAvailableAmounts[SPELLS_AVAILABLE_MAX];
	int spellsAvailableNum;

#define BUFFS_MAX 64
	Buff buffs[BUFFS_MAX];
	int buffsNum;

	bool glassBroken;
	int giveManaCastCount;
};

struct Game {
	Globals globals;

	UnitTypeInfo unitTypeInfos[UNIT_TYPES_MAX];
#define UNITS_MAX 128
	Unit units[UNITS_MAX];
	int unitsNum;
	int nextUnitId;

	SpellTypeInfo spellTypeInfos[SPELL_TYPES_MAX];
#define SPELL_QUEUE_MAX 128
	Spell spellQueue[SPELL_QUEUE_MAX];
	int spellQueueNum;
	int nextSpellId;

	bool spellImmNeverMisses;

	BuffTypeInfo buffTypeInfos[BUFF_TYPES_MAX];
	int nextBuffId;

	int turnQueue[UNITS_MAX];
	int turnQueueNum;

	int level;
	int wave;

	SpellType currentSpellType;
	int currentSpellAvailableIndex;
	float prevSpellTime;
	float spellTime;

	float baseSpellTime;

	bool inEditor;
};

Game *game = NULL;

void runGame();
void updateGame();

Unit *getUnit(int id);
Unit *getUnitByType(UnitType type);
Spell *castSpell(Unit *src, Unit *dest, SpellType type);
void dealDamage(Unit *src, Unit *dest, int amount, bool isMagic=false);
bool isHidden(Unit *unit);
void gainHp(Unit *unit, int amount);
void gainMp(Unit *unit, int amount);
Buff *getBuff(Unit *unit, BuffType type);
int countBuffs(Unit *unit, BuffType type);
Buff *giveBuff(Unit *unit, BuffType type, int turns);
void removeBuff(Unit *unit, Buff *buff);
void removeAllBuffsOfType(Unit *unit, BuffType type);
Unit *createUnit(UnitType type);
void nextWave();
/// FUNCTIONS ^

#include "turnBasedGameAi.cpp"

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

	initPlatform(1920, 1080, "A game?");
	platform->sleepWait = true;
	initAudio();
	initRenderer(1920, 1080);
	initMesh();
	initModel();
	initSkeleton();
	initFonts();
	initTextureSystem();

	platformUpdateLoop(updateGame);
}

void updateGame() {
	if (!game) {
		game = (Game *)zalloc(sizeof(Game));

		{
			UnitTypeInfo *info = NULL;

			for (int i = 0; i < UNIT_TYPES_MAX; i++) {
				UnitTypeInfo *info = &game->unitTypeInfos[i];
				info->maxHp = 10000;
				info->maxMp = 1000;
			}

			info = &game->unitTypeInfos[UNIT_PLAYER1];
			strcpy(info->name, "Player1");

			info = &game->unitTypeInfos[UNIT_PLAYER2];
			strcpy(info->name, "Player2");

			info = &game->unitTypeInfos[UNIT_STANDARD_A];
			strcpy(info->name, "Standard A");
			info->maxHp = 10000;

			info = &game->unitTypeInfos[UNIT_STANDARD_B];
			strcpy(info->name, "Standard B");
			info->maxHp = 20000;

			info = &game->unitTypeInfos[UNIT_STANDARD_C];
			strcpy(info->name, "Standard C");
			info->maxHp = 40000;

			info = &game->unitTypeInfos[UNIT_SWIFT];
			strcpy(info->name, "Swift");
			info->maxHp = 3500;
			info->dodgeChance = 0.9;
		}

		{
			SpellTypeInfo *info = NULL;

			for (int i = 0; i < SPELL_TYPES_MAX; i++) {
				SpellTypeInfo *info = &game->spellTypeInfos[i];
			}

			info = &game->spellTypeInfos[SPELL_NONE];
			strcpy(info->name, "None");

			info = &game->spellTypeInfos[SPELL_HERO_ATTACK];
			strcpy(info->name, "Hero Attack");
			info->damage = 5000;

			info = &game->spellTypeInfos[SPELL_SMALL_ATTACK];
			strcpy(info->name, "Small Attack");
			info->damage = 500;

			info = &game->spellTypeInfos[SPELL_MEDIUM_ATTACK];
			strcpy(info->name, "Medium Attack");
			info->damage = 1000;

			info = &game->spellTypeInfos[SPELL_LARGE_ATTACK];
			strcpy(info->name, "Large Attack");
			info->damage = 2000;

			info = &game->spellTypeInfos[SPELL_QUICK_ATTACK];
			strcpy(info->name, "Quick Attack");
			info->damage = 3000;
			info->mp = 20;

			info = &game->spellTypeInfos[SPELL_WIDE_STRIKE];
			strcpy(info->name, "Wide Strike");
			info->damage = 1500;
			info->mp = 20;

			info = &game->spellTypeInfos[SPELL_COMBO_ATTACK];
			strcpy(info->name, "Combo Attack");
			info->damage = 2500;
			info->mp = 20;

			info = &game->spellTypeInfos[SPELL_EARTHQUAKE];
			strcpy(info->name, "Earthquake");
			info->targetType = TARGET_NONE;
			info->damage = 500;
			info->mp = 100;

			info = &game->spellTypeInfos[SPELL_PHYS_UP];
			strcpy(info->name, "Phys Up");
			info->targetType = TARGET_NONE;
			info->damage = 0;
			info->mp = 100;

			info = &game->spellTypeInfos[SPELL_BODY_BLOCK];
			strcpy(info->name, "Body Block");
			info->targetType = TARGET_NONE;

			info = &game->spellTypeInfos[SPELL_DRAW_DEFAULT];
			strcpy(info->name, "Draw Default");
			info->targetType = TARGET_NONE;

			info = &game->spellTypeInfos[SPELL_DRAW_VAMPIRE];
			strcpy(info->name, "Draw Vampire");
			info->targetType = TARGET_NONE;

			info = &game->spellTypeInfos[SPELL_DRAW_BIG_DAMAGE];
			strcpy(info->name, "Draw Big Damage");
			info->targetType = TARGET_NONE;

			info = &game->spellTypeInfos[SPELL_DRAW_MAGIC_RESIST];
			strcpy(info->name, "Draw Magic Resist");
			info->targetType = TARGET_NONE;
			info->mp = 100;

			info = &game->spellTypeInfos[SPELL_DRAW_LOW_HP];
			strcpy(info->name, "Draw Low Hp");
			info->targetType = TARGET_NONE;

			info = &game->spellTypeInfos[SPELL_DRAW_DODGE];
			strcpy(info->name, "Draw Dodge");
			info->damage = 4000;

			info = &game->spellTypeInfos[SPELL_DRAW_POISON];
			strcpy(info->name, "Draw Poison");
			info->targetType = TARGET_NONE;

			info = &game->spellTypeInfos[SPELL_DRAW_GLASS];
			strcpy(info->name, "Draw Glass");
			info->damage = 10000;

			info = &game->spellTypeInfos[SPELL_SWIPE];
			strcpy(info->name, "Swipe");
			info->damage = 1500;

			info = &game->spellTypeInfos[SPELL_ADD_POISON];
			strcpy(info->name, "Add Poison");
			info->targetType = TARGET_NONE;

			info = &game->spellTypeInfos[SPELL_ADD_MANA_SIPHON];
			strcpy(info->name, "Add Mana Siphon");
			info->targetType = TARGET_NONE;

			info = &game->spellTypeInfos[SPELL_ADD_ATTACK_REDUCTION];
			strcpy(info->name, "Add Attack Reduction");
			info->targetType = TARGET_NONE;

			info = &game->spellTypeInfos[SPELL_ADD_DEFENSE_REDUCTION];
			strcpy(info->name, "Add Defense Reduction");
			info->targetType = TARGET_NONE;

			info = &game->spellTypeInfos[SPELL_HEAL];
			strcpy(info->name, "Heal");
			info->targetType = TARGET_SINGLE;
			info->canTargetAllies = true;
			info->mp = 100;
			info->damage = 5000;

			info = &game->spellTypeInfos[SPELL_HEAL2];
			strcpy(info->name, "Heal 2");
			info->targetType = TARGET_NONE;
			info->mp = 300;
			info->damage = 5000;

			info = &game->spellTypeInfos[SPELL_RESURRECT];
			strcpy(info->name, "Resurrect");
			info->targetType = TARGET_SINGLE;
			info->canTargetAllies = true;
			info->canTargetDead = true;
			info->mp = 200;

			info = &game->spellTypeInfos[SPELL_GIVE_MANA];
			strcpy(info->name, "Give Mana");
			info->targetType = TARGET_SINGLE;
			info->canTargetAllies = true;
			info->mp = 0;

			info = &game->spellTypeInfos[SPELL_ANTI_BUFF];
			strcpy(info->name, "Anti Buff");
			info->targetType = TARGET_SINGLE;
			info->canTargetAllies = true;
			info->mp = 100;

			info = &game->spellTypeInfos[SPELL_MOON_STRIKE];
			strcpy(info->name, "Moon Strike");
			info->targetType = TARGET_SINGLE;
			info->mp = 150;
			info->damage = 7000;

			info = &game->spellTypeInfos[SPELL_MOON_STRIKE_RAID];
			strcpy(info->name, "Moon Strike Raid");
			info->targetType = TARGET_SINGLE;
			info->damage = game->spellTypeInfos[SPELL_MOON_STRIKE].damage;

			info = &game->spellTypeInfos[SPELL_LIGHTNING];
			strcpy(info->name, "Lightning");
			info->targetType = TARGET_NONE;
			info->mp = 50;
			info->damage = 1000;

			info = &game->spellTypeInfos[SPELL_ICE];
			strcpy(info->name, "Ice");
			info->targetType = TARGET_SINGLE;
			info->mp = 100;
			info->damage = 1000;

			info = &game->spellTypeInfos[SPELL_SMALL_ETHER];
			strcpy(info->name, "Small Eather");
			info->targetType = TARGET_SINGLE;
			info->canTargetAllies = true;
			info->damage = 500;

			info = &game->spellTypeInfos[SPELL_LARGE_ETHER];
			strcpy(info->name, "Large Eather");
			info->targetType = TARGET_SINGLE;
			info->canTargetAllies = true;
			info->damage = 1000;

			info = &game->spellTypeInfos[SPELL_WAIT];
			info->targetType = TARGET_NONE;
			strcpy(info->name, "Wait");

			info = &game->spellTypeInfos[SPELL_END_TURN];
			strcpy(info->name, "End Turn");
		}

		{
			BuffTypeInfo *info = NULL;

			for (int i = 0; i < BUFF_TYPES_MAX; i++) {
				BuffTypeInfo *info = &game->buffTypeInfos[i];
			}

			info = &game->buffTypeInfos[BUFF_NONE];
			strcpy(info->name, "None");

			info = &game->buffTypeInfos[BUFF_COMBO];
			strcpy(info->name, "Combo");

			info = &game->buffTypeInfos[BUFF_QUAKED];
			strcpy(info->name, "Quaked");

			info = &game->buffTypeInfos[BUFF_PHYS_UP];
			strcpy(info->name, "Phys Up");

			info = &game->buffTypeInfos[BUFF_BODY_BLOCKING];
			strcpy(info->name, "Body Blocking");

			info = &game->buffTypeInfos[BUFF_BLEED];
			strcpy(info->name, "Bleed");

			info = &game->buffTypeInfos[BUFF_POISON];
			strcpy(info->name, "Poison");

			info = &game->buffTypeInfos[BUFF_ADD_POISON];
			strcpy(info->name, "Added Poison");

			info = &game->buffTypeInfos[BUFF_ADD_MANA_SIPHON];
			strcpy(info->name, "Added Mana Siphon");

			info = &game->buffTypeInfos[BUFF_ADD_ATTACK_REDUCTION];
			strcpy(info->name, "Added Attack Reduction");

			info = &game->buffTypeInfos[BUFF_ADD_DEFENSE_REDUCTION];
			strcpy(info->name, "Added Defense Reduction");

			info = &game->buffTypeInfos[BUFF_ATTACK_REDUCTION];
			strcpy(info->name, "Attack Reduction");

			info = &game->buffTypeInfos[BUFF_DEFENSE_REDUCTION];
			strcpy(info->name, "Defense Reduction");

			info = &game->buffTypeInfos[BUFF_RELIFE];
			strcpy(info->name, "Relife");

			info = &game->buffTypeInfos[BUFF_ANTI_BUFF];
			strcpy(info->name, "Anti Buff");

			info = &game->buffTypeInfos[BUFF_AT_MOON];
			strcpy(info->name, "At Moon");

			info = &game->buffTypeInfos[BUFF_FROZEN];
			strcpy(info->name, "Frozen");
		}

		game->baseSpellTime = 1;
		game->level = 2;
		game->wave = 0;

		{
			Unit *unit = NULL;

			unit = createUnit(UNIT_PLAYER1);
			unit->ally = true;
			unit->weapon = WEAPON_DEFAULT;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_HERO_ATTACK;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_QUICK_ATTACK;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_WIDE_STRIKE;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_COMBO_ATTACK;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_EARTHQUAKE;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_PHYS_UP;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_BODY_BLOCK;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_DRAW_DEFAULT;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_DRAW_VAMPIRE;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_DRAW_BIG_DAMAGE;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_DRAW_MAGIC_RESIST;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_DRAW_LOW_HP;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_DRAW_DODGE;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_DRAW_POISON;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_DRAW_GLASS;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_WAIT;

			unit = createUnit(UNIT_PLAYER2);
			unit->ally = true;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_SWIPE;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_ADD_POISON;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_ADD_MANA_SIPHON;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_ADD_ATTACK_REDUCTION;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_ADD_DEFENSE_REDUCTION;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_HEAL;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_HEAL2;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_RESURRECT;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_GIVE_MANA;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_ANTI_BUFF;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_MOON_STRIKE;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_LIGHTNING;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_ICE;

			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_SMALL_ETHER;
			unit->spellsAvailableAmounts[unit->spellsAvailableNum-1] = 1;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_LARGE_ETHER;
			unit->spellsAvailableAmounts[unit->spellsAvailableNum-1] = 1;

			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_WAIT;

			nextWave();
		}

		maximizeWindow();
	}

	Globals *globals = &game->globals;
	float elapsed = platform->elapsed;

	if (keyJustPressed(KEY_BACKTICK)) game->inEditor = !game->inEditor;
	if (game->inEditor) {
		ImGui::Begin("Editor", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::InputFloat("baseSpellTime", &game->baseSpellTime);
		ImGui::End();
	}

	{ /// Display
		ImGui::SetNextWindowPos(ImVec2(platform->windowWidth*0.5, platform->windowHeight*0.5), ImGuiCond_Always, ImVec2(0.5, 0.5));
		ImGui::Begin("Turns", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("Turns: ");
		for (int i = 0; i < game->turnQueueNum; i++) {
			Unit *unit = getUnit(game->turnQueue[i]);
			ImGui::SameLine();
			ImGui::Text("(%s)", unit->screenName);
		}
		ImGui::End();

		ImGui::SetNextWindowPos(ImVec2(platform->windowWidth*0.5, platform->windowHeight*0.80), ImGuiCond_Always, ImVec2(0.5, 1));
		ImGui::Begin("Game", NULL, ImGuiWindowFlags_AlwaysAutoResize);

		auto guiShowUnit = [](Unit *unit) {
			int color = 0xFFFFFFFF;
			if (game->turnQueueNum != 0 && game->turnQueue[0] == unit->id) color = lerpColor(color, 0xFF00FF00, 0.5);
			if (game->spellQueueNum != 0 && game->spellQueue[0].destId == unit->id) color = lerpColor(color, 0xFFFF0000, 0.5);

			ImVec4 imCol = guiGetImVec4Color(color);
			ImGui::TextColored(imCol, "%s", unit->screenName);
			ImGui::TextColored(imCol, "Hp: %d/%d", unit->hp, unit->info->maxHp);
			if (unit->ally) ImGui::TextColored(imCol, "Mp: %d/%d", unit->mp, unit->info->maxMp);
			if (unit->weapon) ImGui::TextColored(imCol, "Weapon: %s", weaponStrings[unit->weapon]);
			if (unit->buffsNum > 0) {
				for (int i = 0; i < unit->buffsNum; i++) {
					Buff *buff = &unit->buffs[i];
					if (buff->type == BUFF_COMBO) {
						ImGui::Text("[%s (%d):%d]", buff->info->name, buff->intUserData, buff->turns);
					} else {
						ImGui::Text("[%s:%d]", buff->info->name, buff->turns);
					}
				}
			}
			ImGui::Separator();
		};

		Vec2 childSize = v2(400, 400);

		ImGui::BeginChild("AlliesChild", ImVec2(childSize.x, childSize.y));
		for (int i = 0; i < game->unitsNum; i++) {
			Unit *unit = &game->units[i];
			if (!unit->ally) continue;
			if (isHidden(unit)) continue;
			guiShowUnit(unit);
		}
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("EnemiesChild", ImVec2(childSize.x, childSize.y));
		for (int i = 0; i < game->unitsNum; i++) {
			Unit *unit = &game->units[i];
			if (unit->ally) continue;
			if (isHidden(unit)) continue;
			guiShowUnit(unit);
		}
		ImGui::EndChild();

		ImGui::End();
	} ///

	{ /// Update turn
		if (game->turnQueueNum == 0) {
			for (int i = 0; i < game->unitsNum; i++) {
				Unit *unit = &game->units[i];
				if (unit->hp <= 0) continue;
				game->turnQueue[game->turnQueueNum++] = unit->id;
			}
		}

		Unit *currentUnit = getUnit(game->turnQueue[0]);
		if (!currentUnit && platform->frameCount%60 == 0) logf("No current unit\n");

		if (game->spellQueueNum == 0) {
			Buff *atMoon = getBuff(currentUnit, BUFF_AT_MOON);
			if (atMoon) {
				Unit *target = getUnit(atMoon->intUserData);
				if (!target) {
					for (int i = 0; i < game->unitsNum; i++) {
						Unit *possibleTarget = &game->units[i];
						if (possibleTarget->ally == currentUnit->ally) continue;
						if (possibleTarget->hp <= 0) continue;
						if (isHidden(possibleTarget)) continue;
						target = possibleTarget;
						break;
					}
				}
				if (target) {
					castSpell(currentUnit, target, SPELL_MOON_STRIKE_RAID);
				} else {
					logf("Graceful Touchdown\n");
				}
				removeBuff(currentUnit, atMoon);
			}

			Buff *frozen = getBuff(currentUnit, BUFF_FROZEN);
			if (frozen) {
				logf("Frozen\n");
				castSpell(currentUnit, NULL, SPELL_END_TURN);
			}
		}

		if (game->spellQueueNum == 0) {
			int allyCount = 0;
			int enemyCount = 0;
			for (int i = 0; i < game->unitsNum; i++) {
				Unit *unit = &game->units[i];
				if (unit->hp <= 0) continue;
				if (unit->ally) allyCount++;
				if (!unit->ally) enemyCount++;
			}

			if (allyCount == 0) {
				ImGui::SetNextWindowPos(ImVec2(platform->windowWidth*0.5, platform->windowHeight*0.95), ImGuiCond_Always, ImVec2(0.5, 1));
				ImGui::Begin("Game Over", NULL, ImGuiWindowFlags_AlwaysAutoResize);
				ImGui::Text("You lose");
				if (ImGui::Button("Try again"));
				ImGui::End();
			} else if (enemyCount == 0) {
				ImGui::SetNextWindowPos(ImVec2(platform->windowWidth*0.5, platform->windowHeight*0.95), ImGuiCond_Always, ImVec2(0.5, 1));
				ImGui::Begin("Game Over", NULL, ImGuiWindowFlags_AlwaysAutoResize);
				ImGui::Text("You win");
				if (ImGui::Button("Continue")) {
					nextWave();
				}
				ImGui::End();
			} else if (currentUnit) {
				if (currentUnit->ally) {
					ImGui::SetNextWindowPos(ImVec2(platform->windowWidth*0.5, platform->windowHeight*0.95), ImGuiCond_Always, ImVec2(0.5, 1));
					ImGui::Begin("Spells", NULL, ImGuiWindowFlags_AlwaysAutoResize);
					if (game->currentSpellType == SPELL_NONE) {
						int spellCount = 0;
						for (int i = 0; i < currentUnit->spellsAvailableNum; i++) {
							SpellType type = currentUnit->spellsAvailable[i];
							SpellTypeInfo *info = &game->spellTypeInfos[type];
							int spellAvailableIndex = i;

							ImGui::PushID(i);
							char *label = info->name;
							if (info->mp > 0) label = frameSprintf("%s (%dmp)", label, info->mp);
							if (currentUnit->spellsAvailableAmounts[spellAvailableIndex] > -1) label = frameSprintf("%s x%d", label, currentUnit->spellsAvailableAmounts[spellAvailableIndex]);
							if (spellCount % 5 != 0) ImGui::SameLine();
							if (ImGui::Button(label)) {
								bool canCast = true;

								if (currentUnit->mp < info->mp) {
									canCast = false;
									logf("Not enough mp\n");
								}

								if (currentUnit->spellsAvailableAmounts[spellAvailableIndex] == 0) {
									canCast = false;
									logf("No more\n");
								}

								if (
									(type == SPELL_DRAW_DEFAULT && currentUnit->weapon == WEAPON_DEFAULT) ||
									(type == SPELL_DRAW_VAMPIRE && currentUnit->weapon == WEAPON_VAMPIRE) ||
									(type == SPELL_DRAW_MAGIC_RESIST && currentUnit->weapon == WEAPON_MAGIC_RESIST) ||
									(type == SPELL_DRAW_LOW_HP && currentUnit->weapon == WEAPON_LOW_HP) ||
									(type == SPELL_DRAW_DODGE && currentUnit->weapon == WEAPON_DODGE) ||
									(type == SPELL_DRAW_POISON && currentUnit->weapon == WEAPON_POISON) ||
									(type == SPELL_DRAW_GLASS && currentUnit->weapon == WEAPON_GLASS)
								) {
									canCast = false;
									logf("You already have that weapon\n");
								}

								if (type == SPELL_DRAW_GLASS && currentUnit->glassBroken) {
									canCast = false;
									logf("Broken.\n");
								}

								if (type == SPELL_DRAW_MAGIC_RESIST) {
									Unit *p2 = getUnitByType(UNIT_PLAYER2);
									if (p2->hp <= 0) {
										canCast = false;
										logf("%s is dead\n", p2->info->name);
									}
									if (p2->mp < info->mp) {
										canCast = false;
										logf("%s doesn't have enough mp\n", p2->info->name);
									}
								}

								if (canCast) {
									game->currentSpellType = type;
									game->currentSpellAvailableIndex = i;
								}
							}

							if (ImGui::IsItemHovered()) {
								if (info->damage) ImGui::SetTooltip("Base damage: %d", info->damage);
							}

							spellCount++;
							ImGui::PopID();
						}
					} else {
						SpellTypeInfo *currentSpellInfo = &game->spellTypeInfos[game->currentSpellType];
						if (currentSpellInfo->targetType == TARGET_SINGLE) {
							ImGui::Text("Target:");
							for (int i = 0; i < game->unitsNum; i++) {
								Unit *unit = &game->units[i];
								if (unit->ally && !currentSpellInfo->canTargetAllies) continue;
								if (unit->hp <= 0 && !currentSpellInfo->canTargetDead) continue;
								if (isHidden(unit)) continue;
								if (ImGui::Button(frameSprintf("%d: %s", i, unit->screenName))) {
									bool canCast = true;
									if (game->currentSpellType == SPELL_RESURRECT && getBuff(unit, BUFF_RELIFE)) {
										canCast = false;
										logf("It won't work\n");
									}

									if (canCast) {
										currentUnit->spellsAvailableAmounts[game->currentSpellAvailableIndex]--;
										castSpell(currentUnit, unit, game->currentSpellType);
										castSpell(currentUnit, NULL, SPELL_END_TURN);
										game->currentSpellType = SPELL_NONE;
									}
								}
							}
							if (ImGui::Button("Cancel")) game->currentSpellType = SPELL_NONE;
						} else if (currentSpellInfo->targetType == TARGET_NONE) {
							currentUnit->spellsAvailableAmounts[game->currentSpellAvailableIndex]--;
							castSpell(currentUnit, NULL, game->currentSpellType);
							castSpell(currentUnit, NULL, SPELL_END_TURN);
							game->currentSpellType = SPELL_NONE;
						}
					}
					ImGui::End();
				} else {
					aiTakeTurn(currentUnit);
				}
			}
		}
	} ///

	{ /// Update spells
		if (game->spellQueueNum > 0) {
			Spell *spell = &game->spellQueue[0];
			bool complete = false;

			auto spellTimeJustPassed = [](float amount)->bool {
				if (amount == 0) return game->spellTime == 0;
				if (game->spellTime >= amount && game->prevSpellTime < amount) return true;
				return false;
			};

			Unit *src = getUnit(spell->srcId);
			Unit *dest = getUnit(spell->destId);

			float baseSpellTime = game->baseSpellTime;

			if (
				spell->type == SPELL_HERO_ATTACK ||
				spell->type == SPELL_SMALL_ATTACK ||
				spell->type == SPELL_MEDIUM_ATTACK ||
				spell->type == SPELL_LARGE_ATTACK
			) {
				if (game->spellTime == 0) dealDamage(src, dest, spell->info->damage);
			} else if (spell->type == SPELL_QUICK_ATTACK) {
				bool oldSpellImmNeverMisses = game->spellImmNeverMisses;
				game->spellImmNeverMisses = true;
				if (spellTimeJustPassed(baseSpellTime*0.25)) dealDamage(src, dest, spell->info->damage/3);
				if (spellTimeJustPassed(baseSpellTime*0.50)) dealDamage(src, dest, spell->info->damage/3);
				if (spellTimeJustPassed(baseSpellTime*0.75)) dealDamage(src, dest, spell->info->damage/3);
				game->spellImmNeverMisses = oldSpellImmNeverMisses;
			} else if (spell->type == SPELL_WIDE_STRIKE) {
				if (spellTimeJustPassed(baseSpellTime*0.5)) {
					int destIndex = -1;
					for (int i = 0; i < game->unitsNum; i++) {
						if (&game->units[i] == dest) destIndex = i;
					}
					Unit *underDest = NULL;
					if (destIndex > 0) {
						underDest = &game->units[destIndex-1];
						if (underDest->ally != dest->ally) underDest = NULL;
						if (isHidden(underDest)) underDest = NULL;
					}
					Unit *overDest = NULL;
					if (destIndex < game->unitsNum-1) {
						overDest = &game->units[destIndex+1];
						if (overDest->ally != dest->ally) overDest = NULL;
						if (isHidden(overDest)) overDest = NULL;
					}
					dealDamage(src, dest, spell->info->damage);
					if (underDest) dealDamage(src, underDest, spell->info->damage);
					if (overDest) dealDamage(src, overDest, spell->info->damage);
				}
			} else if (spell->type == SPELL_COMBO_ATTACK) {
				if (spellTimeJustPassed(baseSpellTime*0.5)) {
					Buff *comboBuff = getBuff(dest, BUFF_COMBO);
					if (!comboBuff) comboBuff = giveBuff(dest, BUFF_COMBO, 1);
					if (comboBuff) {
						comboBuff->intUserData++;
						comboBuff->turns++;
						dealDamage(src, dest, spell->info->damage * comboBuff->intUserData);
					} else {
						dealDamage(src, dest, spell->info->damage);
					}
				}
			} else if (spell->type == SPELL_EARTHQUAKE) {
				if (spellTimeJustPassed(baseSpellTime*0.5)) {
					for (int i = 0; i < game->unitsNum; i++) {
						Unit *unit = &game->units[i];
						if (unit->hp <= 0) continue;
						if (isHidden(unit)) continue;
						if (unit->ally == src->ally) continue;
						dealDamage(src, unit, spell->info->damage);
						giveBuff(unit, BUFF_QUAKED, 2);
					}
				}
			} else if (spell->type == SPELL_PHYS_UP) {
				if (spellTimeJustPassed(baseSpellTime*0.5)) {
					for (int i = 0; i < game->unitsNum; i++) {
						Unit *unit = &game->units[i];
						if (unit->hp <= 0) continue;
						if (isHidden(unit)) continue;
						if (unit->ally == src->ally) {
							giveBuff(unit, BUFF_PHYS_UP, 2);
						}
					}
				}
			} else if (spell->type == SPELL_BODY_BLOCK) {
				if (spellTimeJustPassed(baseSpellTime*0.5)) giveBuff(src, BUFF_BODY_BLOCKING, 2);
			} else if (spell->type == SPELL_DRAW_DEFAULT) {
				if (spellTimeJustPassed(baseSpellTime*0.5)) src->weapon = WEAPON_DEFAULT;
			} else if (spell->type == SPELL_DRAW_VAMPIRE) {
				if (spellTimeJustPassed(baseSpellTime*0.5)) {
					src->weapon = WEAPON_VAMPIRE;
					for (int i = 0; i < game->unitsNum; i++) {
						Unit *unit = &game->units[i];
						if (unit->hp <= 0) continue;
						if (isHidden(unit)) continue;
						giveBuff(unit, BUFF_BLEED, 3);
					}
				}
			} else if (spell->type == SPELL_DRAW_BIG_DAMAGE) {
				if (spellTimeJustPassed(baseSpellTime*0.5)) src->weapon = WEAPON_BIG_DAMAGE;
			} else if (spell->type == SPELL_DRAW_MAGIC_RESIST) {
				if (spellTimeJustPassed(baseSpellTime*0.5)) {
					src->weapon = WEAPON_MAGIC_RESIST;
					Unit *p2 = getUnitByType(UNIT_PLAYER2);
					p2->mp -= spell->info->mp;
				}
			} else if (spell->type == SPELL_DRAW_LOW_HP) {
				if (spellTimeJustPassed(baseSpellTime*0.5)) {
					src->weapon = WEAPON_LOW_HP;
					for (int i = 0; i < game->unitsNum; i++) {
						Unit *unit = &game->units[i];
						if (unit->hp <= 0) continue;
						if (isHidden(unit)) continue;
						if (unit->ally) gainHp(unit, 100);
					}
				}
			} else if (spell->type == SPELL_DRAW_DODGE) {
				if (spellTimeJustPassed(baseSpellTime*0.3)) {
					src->weapon = WEAPON_DODGE;
				} else if (spellTimeJustPassed(baseSpellTime*0.6)) {
					dealDamage(src, dest, spell->info->damage/2);
				} else if (spellTimeJustPassed(baseSpellTime*0.9)) {
					dealDamage(src, dest, spell->info->damage/2);
				}
			} else if (spell->type == SPELL_DRAW_POISON) {
				if (spellTimeJustPassed(baseSpellTime*0.5)) {
					src->weapon = WEAPON_POISON;
					giveBuff(src, BUFF_POISON, 3);
				}
			} else if (spell->type == SPELL_DRAW_GLASS) {
				if (spellTimeJustPassed(baseSpellTime*0.5)) {
					dealDamage(src, dest, spell->info->damage);
					src->glassBroken = true;
				}
			} else if (spell->type == SPELL_SWIPE) {
				if (spellTimeJustPassed(baseSpellTime*0.3)) {
					dealDamage(src, dest, spell->info->damage/2);
				}
				if (spellTimeJustPassed(baseSpellTime*0.6)) {
					int damage = spell->info->damage/2;
					for (int i = 0; i < countBuffs(src, BUFF_ADD_POISON); i++) giveBuff(dest, BUFF_POISON, 10);
					for (int i = 0; i < countBuffs(src, BUFF_ADD_MANA_SIPHON); i++) gainMp(src, damage * 0.01);
					for (int i = 0; i < countBuffs(src, BUFF_ADD_ATTACK_REDUCTION); i++) giveBuff(dest, BUFF_ATTACK_REDUCTION, 3);
					for (int i = 0; i < countBuffs(src, BUFF_ADD_DEFENSE_REDUCTION); i++) giveBuff(dest, BUFF_DEFENSE_REDUCTION, 3);
					dealDamage(src, dest, damage);
				}

				if (spellTimeJustPassed(baseSpellTime*0.9)) {
					removeAllBuffsOfType(src, BUFF_ADD_POISON);
					removeAllBuffsOfType(src, BUFF_ADD_MANA_SIPHON);
					removeAllBuffsOfType(src, BUFF_ADD_ATTACK_REDUCTION);
					removeAllBuffsOfType(src, BUFF_ADD_DEFENSE_REDUCTION);
				}
			} else if (spell->type == SPELL_ADD_POISON) {
				if (spellTimeJustPassed(baseSpellTime*0.5)) giveBuff(src, BUFF_ADD_POISON, -1);
			} else if (spell->type == SPELL_ADD_MANA_SIPHON) {
				if (spellTimeJustPassed(baseSpellTime*0.5)) giveBuff(src, BUFF_ADD_MANA_SIPHON, -1);
			} else if (spell->type == SPELL_ADD_ATTACK_REDUCTION) {
				if (spellTimeJustPassed(baseSpellTime*0.5)) giveBuff(src, BUFF_ADD_ATTACK_REDUCTION, -1);
			} else if (spell->type == SPELL_ADD_DEFENSE_REDUCTION) {
				if (spellTimeJustPassed(baseSpellTime*0.5)) giveBuff(src, BUFF_ADD_DEFENSE_REDUCTION, -1);
			} else if (spell->type == SPELL_HEAL) {
				if (spellTimeJustPassed(baseSpellTime*0.5)) gainHp(dest, spell->info->damage);
			} else if (spell->type == SPELL_HEAL2) {
				if (spellTimeJustPassed(baseSpellTime*0.5)) {
					for (int i = 0; i < game->unitsNum; i++) {
						Unit *unit = &game->units[i];
						if (unit->hp <= 0) continue;
						if (isHidden(unit)) continue;
						if (unit->ally) gainHp(unit, spell->info->damage);
					}
				}
			} else if (spell->type == SPELL_RESURRECT) {
				if (spellTimeJustPassed(baseSpellTime*0.5)) {
					if (dest->hp <= 0) {
						dest->hp = dest->info->maxHp;
					} else {
						giveBuff(dest, BUFF_RELIFE, -1);
					}
				}
			} else if (spell->type == SPELL_GIVE_MANA) {
				if (spellTimeJustPassed(baseSpellTime*0.5)) {
					int mp = 512;
					for (int i = 0; i < src->giveManaCastCount; i++) mp /= 2;
					gainMp(dest, mp);
					src->giveManaCastCount++;
				}
			} else if (spell->type == SPELL_ANTI_BUFF) {
				if (spellTimeJustPassed(baseSpellTime*0.5)) giveBuff(dest, BUFF_ANTI_BUFF, 2);
			} else if (spell->type == SPELL_MOON_STRIKE) {
				if (spellTimeJustPassed(baseSpellTime*0.5)) {
					Buff *buff = giveBuff(src, BUFF_AT_MOON, -1);
					if (buff) buff->intUserData = dest->id;
				}
			} else if (spell->type == SPELL_MOON_STRIKE_RAID) {
				if (spellTimeJustPassed(baseSpellTime*0.4)) dealDamage(src, dest, spell->info->damage/5);
				if (spellTimeJustPassed(baseSpellTime*0.5)) dealDamage(src, dest, spell->info->damage/5);
				if (spellTimeJustPassed(baseSpellTime*0.6)) dealDamage(src, dest, spell->info->damage/5);
				if (spellTimeJustPassed(baseSpellTime*0.7)) dealDamage(src, dest, spell->info->damage/5);
				if (spellTimeJustPassed(baseSpellTime*0.8)) dealDamage(src, dest, spell->info->damage/5);
			} else if (spell->type == SPELL_LIGHTNING) {
				if (spellTimeJustPassed(baseSpellTime*0.5)) {
					for (int i = 0; i < game->unitsNum; i++) {
						Unit *unit = &game->units[i];
						if (unit->ally) continue;
						if (unit->hp <= 0) continue;
						if (isHidden(unit)) continue;
						dealDamage(src, unit, spell->info->damage);
					}
				}
			} else if (spell->type == SPELL_ICE) {
				if (spellTimeJustPassed(baseSpellTime*0.5)) {
					giveBuff(dest, BUFF_FROZEN, 1);
					dealDamage(src, dest, spell->info->damage);
				}
			} else if (spell->type == SPELL_SMALL_ETHER) {
				if (spellTimeJustPassed(baseSpellTime*0.5)) gainMp(dest, spell->info->damage);
			} else if (spell->type == SPELL_LARGE_ATTACK) {
				if (spellTimeJustPassed(baseSpellTime*0.5)) gainMp(dest, spell->info->damage);
			} else if (spell->type == SPELL_WAIT) {
				if (game->spellTime == 0) logf("Waiting...\n");
			} else if (spell->type == SPELL_END_TURN) {
				if (game->spellTime > baseSpellTime) {
					Unit *unit = getUnit(game->turnQueue[0]);

					for (int i = 0; i < unit->buffsNum; i++) {
						Buff *buff = &unit->buffs[i];
						buff->turns--;

						bool oldSpellImmNeverMisses = game->spellImmNeverMisses;
						game->spellImmNeverMisses = true;

						if (buff->type == BUFF_BLEED) dealDamage(NULL, unit, 500, true);
						if (buff->type == BUFF_POISON) dealDamage(NULL, unit, 100, true);

						game->spellImmNeverMisses = oldSpellImmNeverMisses;

						if (buff->turns == 0) {
							arraySpliceIndex(unit->buffs, unit->buffsNum, sizeof(Buff), i);
							unit->buffsNum--;
							i--;
							continue;
						}
					}

					logf("%s's turn is over\n", unit->info->name);

					arraySpliceIndex(game->turnQueue, game->turnQueueNum, sizeof(int), 0);
					game->turnQueueNum--;
					complete = true;
				}
			} else {
				if (game->spellTime == 0) logf("%s (spellType %d) has no update loop\n", spell->info->name, spell->type);
				if (game->spellTime > 3) complete = true;
			}

			if (game->spellTime > baseSpellTime) complete = true;

			game->prevSpellTime = game->spellTime;
			game->spellTime += elapsed;

			if (complete) {
				arraySpliceIndex(game->spellQueue, game->spellQueueNum, sizeof(Spell), 0);
				game->spellQueueNum--;
				game->spellTime = 0;
				game->prevSpellTime = 0;
			}
		}
	} ///

	/// Remove dead units
	if (game->spellQueueNum == 0) {
		for (int i = 0; i < game->unitsNum; i++) {
			Unit *unit = &game->units[i];
			if (unit->ally) continue;
			if (unit->hp <= 0) {
				arraySpliceIndex(game->units, game->unitsNum, sizeof(Unit), i);
				game->unitsNum--;
				i--;
				continue;
			}
		}
	}

	clearRenderer();

	drawOnScreenLog();
}

Unit *getUnit(int id) {
	if (id == 0) return NULL;

	for (int i = 0; i < game->unitsNum; i++) {
		Unit *unit = &game->units[i];
		if (unit->id == id) return unit;
	}
	return NULL;
}

Unit *getUnitByType(UnitType type) {
	for (int i = 0; i < game->unitsNum; i++) {
		Unit *unit = &game->units[i];
		if (unit->type == type) return unit;
	}
	return NULL;
}

Spell *castSpell(Unit *src, Unit *dest, SpellType type) {
	if (game->spellQueueNum > SPELL_QUEUE_MAX-1) {
		logf("Spell queue is full!\n");
		game->spellQueueNum--;
	}

	if (dest && isHidden(dest)) {
		for (int i = 0; i < game->unitsNum; i++) {
			Unit *possibleDest = &game->units[i];
			if (possibleDest->ally != dest->ally) continue;
			if (possibleDest->hp <= 0) continue;
			if (isHidden(possibleDest)) continue;
			dest = possibleDest;
			break;
		}
	}

	if (src && dest && !src->ally && dest->type == UNIT_PLAYER2) {
		Unit *p1 = getUnitByType(UNIT_PLAYER1);
		if (getBuff(p1, BUFF_BODY_BLOCKING)) {
			logf("Redirected!\n");
			dest = p1;
		}
	}

	Spell *spell = &game->spellQueue[game->spellQueueNum++];
	memset(spell, 0, sizeof(Spell));
	spell->type = type;
	spell->id = ++game->nextSpellId;
	spell->info = &game->spellTypeInfos[spell->type];
	if (src) spell->srcId = src->id;
	if (dest) spell->destId = dest->id;

	src->mp -= spell->info->mp;

	return spell;
}

void dealDamage(Unit *src, Unit *dest, int amount, bool isMagic) {
	float damageMulti = 1;

	if (src) {
		for (int i = 0; i < src->buffsNum; i++) {
			Buff *buff = &src->buffs[i];
			if (!isMagic && buff->type == BUFF_QUAKED) damageMulti *= 0.5;
			if (!isMagic && buff->type == BUFF_PHYS_UP) damageMulti *= 2;
			if (buff->type == BUFF_ATTACK_REDUCTION) damageMulti *= 0.5;
		}

		if (src->weapon == WEAPON_BIG_DAMAGE) damageMulti *= 3;

		if (src->ally && isMagic) {
			Unit *p1 = getUnitByType(UNIT_PLAYER2);
			if (p1->hp > 0 && p1->weapon == WEAPON_MAGIC_RESIST) damageMulti *= 0.75;
		}

		if (src->weapon == WEAPON_LOW_HP) {
			int powerLevel = 1;
			if (src->hp < src->info->maxHp*0.50) powerLevel = 2;
			if (src->hp < src->info->maxHp*0.25) powerLevel = 3;
			damageMulti *= powerLevel;
		}

		if (src->weapon == WEAPON_POISON && !isMagic) {
			src->mp -= 10;
			giveBuff(dest, BUFF_POISON, 10);
		}
	}

	for (int i = 0; i < dest->buffsNum; i++) {
		Buff *buff = &dest->buffs[i];
		if (!isMagic && buff->type == BUFF_QUAKED) damageMulti *= 2;
		if (buff->type == BUFF_DEFENSE_REDUCTION) damageMulti *= 2;
	}

	float dodgeChance = dest->info->dodgeChance;

	if (dest->weapon == WEAPON_DODGE) dodgeChance += 0.1;

	if (game->spellImmNeverMisses) dodgeChance = 0;

	if (rndFloat(0, 1) < dodgeChance) {
		damageMulti = 0;
		logf("Dodged!\n");
	}

	amount *= damageMulti;

	if (src) {
		logf("%s dealt %d damage to %s\n", src->info->name, amount, dest->info->name);
	} else {
		logf("%d damage dealt to %s\n", amount, dest->info->name);
	}
	dest->hp -= amount;

	if (src && src->weapon == WEAPON_VAMPIRE) gainHp(src, amount * 0.01);

	if (dest->hp <= 0 && getBuff(dest, BUFF_RELIFE)) {
		logf("Relifed\n");
		dest->hp = dest->info->maxHp;
		removeAllBuffsOfType(dest, BUFF_RELIFE);
	}

	if (dest->hp <= 0) {
		for (int i = 1; i < game->turnQueueNum; i++) {
			if (game->turnQueue[i] == dest->id) {
				arraySpliceIndex(game->turnQueue, game->turnQueueNum, sizeof(int), i);
				game->turnQueueNum--;
				i--;
				continue;
			}
		}
	}
}

bool isHidden(Unit *unit) {
	if (unit == NULL) return false;
	if (getBuff(unit, BUFF_AT_MOON)) return true;
	return false;
}

void gainHp(Unit *unit, int amount) {
	if (unit->weapon == WEAPON_BIG_DAMAGE) amount = 0;
	unit->hp += amount;
	if (unit->hp > unit->info->maxHp) unit->hp = unit->info->maxHp;
}

void gainMp(Unit *unit, int amount) {
	logf("Gained %d\n", amount);
	unit->mp += amount;
	if (unit->mp > unit->info->maxMp) unit->mp = unit->info->maxMp;
}

Buff *getBuff(Unit *unit, BuffType type) {
	for (int i = 0; i < unit->buffsNum; i++) {
		Buff *buff = &unit->buffs[i];
		if (buff->type == type) return buff;
	}
	return NULL;
}

int countBuffs(Unit *unit, BuffType type) {
	int count = 0;
	for (int i = 0; i < unit->buffsNum; i++) {
		Buff *buff = &unit->buffs[i];
		if (buff->type == type) count++;
	}
	return count;
}

Buff *giveBuff(Unit *unit, BuffType type, int turns) {
	if (unit->buffsNum > BUFFS_MAX-1) {
		logf("Too many buffs!\n");
		unit->buffsNum--;
	}

	Buff *antiBuff = getBuff(unit, BUFF_ANTI_BUFF);
	if (antiBuff) {
		logf("Anti buff consumed %s\n", game->buffTypeInfos[type].name);
		removeBuff(unit, antiBuff);
		return NULL;
	}

	Buff *buff = &unit->buffs[unit->buffsNum++];
	memset(buff, 0, sizeof(Buff));
	buff->type = type;
	buff->turns = turns;
	buff->info = &game->buffTypeInfos[buff->type];
	return buff;
}

void removeBuff(Unit *unit, Buff *buff) {
	int buffIndex = -1;
	for (int i = 0; i < unit->buffsNum; i++) {
		Buff *otherBuff = &unit->buffs[i];
		if (otherBuff == buff) buffIndex = i;
	}

	if (buffIndex == -1) {
		logf("Couldn't find buff to remove???\n");
		return;
	}
	arraySpliceIndex(unit->buffs, unit->buffsNum, sizeof(Buff), buffIndex);
	unit->buffsNum--;
}

void removeAllBuffsOfType(Unit *unit, BuffType type) {
	for (int i = 0; i < unit->buffsNum; i++) {
		Buff *buff = &unit->buffs[i];
		if (buff->type == type) {
			arraySpliceIndex(unit->buffs, unit->buffsNum, sizeof(Buff), i);
			unit->buffsNum--;
			i--;
			continue;
		}
	}
}

Unit *createUnit(UnitType type) {
	if (game->unitsNum > UNITS_MAX-1) {
		logf("No more units!\n");
		return NULL;
	}

	Unit *unit = &game->units[game->unitsNum++];
	memset(unit, 0, sizeof(Unit));
	unit->type = type;
	unit->id = ++game->nextUnitId;
	unit->info = &game->unitTypeInfos[unit->type];
	unit->hp = unit->info->maxHp;
	unit->mp = unit->info->maxMp;

	for (int i = 0; i < SPELLS_AVAILABLE_MAX; i++) {
		unit->spellsAvailableAmounts[i] = -1;
	}

	{
		int existingCount = 0;
		for (int i = 0; i < game->unitsNum; i++) {
			Unit *unit = &game->units[i];
			if (unit->type == type) existingCount++;
		}
		if (existingCount == 1) {
			strcpy(unit->screenName, unit->info->name);
		} else {
			strcpy(unit->screenName, frameSprintf("%s (%d)", unit->info->name, existingCount-1));
		}
	}

	return unit;
}

void nextWave() {
	game->turnQueueNum = 0;

	Unit *unit = NULL;

	int waveCheck = 0;

#define StartWaveDef() if (game->wave == waveCheck) {
#define EndWaveDef() }; waveCheck++; 
#define NextWaveDef() EndWaveDef(); StartWaveDef();

	if (game->level == 1) {
		StartWaveDef();
		unit = createUnit(UNIT_STANDARD_A);
		unit = createUnit(UNIT_STANDARD_B);
		NextWaveDef();
		unit = createUnit(UNIT_STANDARD_B);
		unit = createUnit(UNIT_STANDARD_B);
		NextWaveDef();
		unit = createUnit(UNIT_STANDARD_A);
		unit = createUnit(UNIT_STANDARD_B);
		unit = createUnit(UNIT_STANDARD_A);
		NextWaveDef();
		unit = createUnit(UNIT_STANDARD_A);
		unit = createUnit(UNIT_STANDARD_C);
		unit = createUnit(UNIT_STANDARD_A);
		NextWaveDef();
		logf("You win\n");
		EndWaveDef();
	} else if (game->level == 2) {
		StartWaveDef();
		unit = createUnit(UNIT_STANDARD_A);
		unit = createUnit(UNIT_STANDARD_C);
		unit = createUnit(UNIT_STANDARD_A);
		NextWaveDef();
		unit = createUnit(UNIT_STANDARD_B);
		unit = createUnit(UNIT_SWIFT);
		unit = createUnit(UNIT_STANDARD_A);
		unit = createUnit(UNIT_SWIFT);
		unit = createUnit(UNIT_STANDARD_B);
		NextWaveDef();
		unit = createUnit(UNIT_STANDARD_B);
		unit = createUnit(UNIT_STANDARD_A);
		unit = createUnit(UNIT_STANDARD_C);
		unit = createUnit(UNIT_STANDARD_A);
		unit = createUnit(UNIT_STANDARD_B);
		NextWaveDef();
		unit = createUnit(UNIT_SWIFT);
		unit = createUnit(UNIT_SWIFT);
		unit = createUnit(UNIT_SWIFT);
		unit = createUnit(UNIT_SWIFT);
		unit = createUnit(UNIT_SWIFT);
		NextWaveDef();
		logf("You win\n");
		EndWaveDef();
	}

	game->wave++;
}
