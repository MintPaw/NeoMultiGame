struct Globals {
	Vec2 characterCardSize;

#define CUSTOM_STYLE_STACKS_MAX 64
	NguiNamedStyleStack customStyleStacks[CUSTOM_STYLE_STACKS_MAX];
	int customStyleStacksNum;
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
	BUFF_SHIELD,
	BUFF_FAKE_SHIELD,
	BUFF_SPUN_UP,
	BUFF_STUNNED,
	BUFF_TIME_TUNE_CHARGE,
	BUFF_VOLUME_WARNING,
	BUFF_SILENCE,
	BUFF_DAMAGE_SPRING,
	BUFF_FOCUS,
	BUFF_VISUALIZE,
	BUFF_STEALTHED,
	BUFF_MARKED,
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
	float time; // Just used for Fake Shield names right now
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
	SPELL_SUMMONER_ATTACK,

	SPELL_STORM_ATTACK,
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

	SPELL_SLEEP,
	SPELL_WAKE_UP,
	SPELL_FOCUS,
	SPELL_DREAM,
	SPELL_VISUALIZE,
	SPELL_STEALTH,
	SPELL_RE_STEALTH,
	SPELL_STEALTH_WEAPON_SWITCH,
	SPELL_STEALTH_REFILL,
	SPELL_STEALTH_MARK,
	SPELL_STEALTH_SNIPE,

	SPELL_SMALL_ETHER,
	SPELL_LARGE_ETHER,

	SPELL_CREATE_SHIELD,
	SPELL_SHIELD_BLAST,
	SPELL_CREATE_2_SHIELDS,
	SPELL_SHIELD_BUMP,

	SPELL_CREATE_FAKE_SHIELD,
	SPELL_TAKE_MANA,
	SPELL_TAKE_MANA_BIG,

	SPELL_ACCELERATED_SLASH,

	SPELL_STUDENT_ATTACK,
	SPELL_TEACHER_ATTACK,
	SPELL_TEACHER_TEACHER_ATTACK,
	SPELL_CREATE_STUDENT,
	SPELL_CREATE_TEACHER,

	SPELL_MULTIHITTER_HIT,
	SPELL_MULTIHITTER_SPIN_UP,
	SPELL_STUNNER_ATTACK,
	SPELL_STUN,
	SPELL_GAIN_TIME_TUNE_CHARGE,
	SPELL_TIME_TUNE,
	SPELL_VOLUME_WARNING,
	SPELL_TAKE_LIFE,

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

	bool enabledWhileAwake;
	bool enabledWhileAsleep;
	bool enabledWhileStealthed;
};
struct Spell {
	SpellType type;
	int id;
	SpellTypeInfo *info;
	int srcId;
	int destId;
};

enum UnitType {
	UNIT_NONE,
	UNIT_PLAYER1,
	UNIT_PLAYER2,
	UNIT_STANDARD_A,
	UNIT_STANDARD_B,
	UNIT_STANDARD_C,
	UNIT_SWIFT,
	UNIT_SMALL_SHIELDSTER,
	UNIT_SUPER_SHIELDSTER,
	UNIT_SHIELD_SUMMONER,
	UNIT_FAKE_SHIELDSTER,
	UNIT_MANA_BRUISER,
	UNIT_ACCELERATOR,
	UNIT_STUDENT,
	UNIT_TEACHER,
	UNIT_TEACHER_TEACHER,
	UNIT_MULTIHITTER,
	UNIT_STUNNER,
	UNIT_TIME_TUNER,
	UNIT_SILENCER,
	UNIT_LIFE_TAKER,
	UNIT_SPIKE,
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

	bool hasTurn;
	int extraTurns;

#define SPELLS_AVAILABLE_MAX 128
	SpellType spellsAvailable[SPELLS_AVAILABLE_MAX];
	int spellsAvailableAmounts[SPELLS_AVAILABLE_MAX];
	int spellsAvailableNum;

#define BUFFS_MAX 64
	Buff buffs[BUFFS_MAX];
	int buffsNum;

	bool glassBroken;
	int giveManaCastCount;
	bool asleep;

	int accelerationCount;

	float maxHpReductionPerc;

	Rect cardRect;
	bool couldBeTargettedThisFrame;
};

enum EffectType {
	EFFECT_DAMAGE,
	EFFECT_GAIN_HP,
	EFFECT_LOSE_MP,
	EFFECT_GAIN_MP,
	EFFECT_DODGE,
};
struct Effect {
	EffectType type;
	Vec2 position;

	int unitId;
	int intValue;

	float time;
};

enum GameState {
	GAME_STATE_MENU,
	GAME_STATE_PLAY,
};

struct Game {
	Globals globals;
	Vec2 realSize;
	Vec2 size;
	Vec2 sizeScale;
	Font *defaultFont;
	Font *hudFont;
	Vec2 mouse;

	GameState state;
	GameState prevState;
	float stateTime;

	UnitTypeInfo unitTypeInfos[UNIT_TYPES_MAX];
#define UNITS_MAX 128
	Unit units[UNITS_MAX];
	int unitsNum;
	int nextUnitId;

	int chosenAllyUnit;

	SpellTypeInfo spellTypeInfos[SPELL_TYPES_MAX];
#define SPELL_QUEUE_MAX 128
	Spell spellQueue[SPELL_QUEUE_MAX];
	int spellQueueNum;
	int nextSpellId;

	bool spellImmNeverMisses;

	BuffTypeInfo buffTypeInfos[BUFF_TYPES_MAX];
	int nextBuffId;

	int level;
	int wave;
	int startingWave;

	int targetedUnit;
	SpellType currentSpellType;
	int currentSpellAvailableIndex;
	float prevSpellTime;
	float spellTime;

	float baseSpellTime;

#define EFFECTS_MAX 128
	Effect effects[EFFECTS_MAX];
	int effectsNum;

	char *tooltip;
	float winLossTime;

	float prevAllyAdvantage;
	float prevAllyAdvantageGain;

	bool inEditor;
};

Game *game = NULL;

void runGame();
void updateGame();
void drawPass(Pass *pass);

Unit *getCurrentUnit();
Unit *getUnit(int id);
Unit *getUnitByType(UnitType type);
Spell *castSpell(Unit *src, Unit *dest, SpellType type);
bool isHidden(Unit *unit);

void dealDamage(Unit *src, Unit *dest, int damageAmount, bool isMagic=false);
void gainHp(Unit *unit, int amount);
void gainMp(Unit *unit, int amount);
void loseMp(Unit *unit, int amount);
void refillFlasks(Unit *unit);

Buff *getBuff(Unit *unit, BuffType type);
int countBuffs(Unit *unit, BuffType type);
Buff *giveBuff(Unit *unit, BuffType type, int turns);
void removeBuff(Unit *unit, Buff *buff);
void removeAllBuffsOfType(Unit *unit, BuffType type);

Effect *createEffect(EffectType type, Unit *unit);

Unit *createUnit(UnitType type);
void nextWave();

void saveLoadGlobals(bool save);

void nguiShowImGuiStylesEditor(NguiNamedStyleStack *styleStacks, int *styleStacksNum, int styleStacksMax);
void versionSaveLoadNamedStyleStacks(DataStream *stream, bool save, int version, NguiNamedStyleStack *styleStacks, int *styleStacksNum, int styleStacksMax, int minVersion, int maxVersion);
void pushGameStyleStack(char *name);
void popGameStyleStack(char *name);
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
	nguiInit();

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

			info = &game->unitTypeInfos[UNIT_SMALL_SHIELDSTER];
			strcpy(info->name, "Small Shieldster");
			info->maxHp = 4000;

			info = &game->unitTypeInfos[UNIT_SUPER_SHIELDSTER];
			strcpy(info->name, "Super Shieldster");
			info->maxHp = 10000;

			info = &game->unitTypeInfos[UNIT_SHIELD_SUMMONER];
			strcpy(info->name, "Shield Summoner");
			info->maxHp = 5000;

			info = &game->unitTypeInfos[UNIT_FAKE_SHIELDSTER];
			strcpy(info->name, "Fake Shieldster");
			info->maxHp = 5000;

			info = &game->unitTypeInfos[UNIT_MANA_BRUISER];
			strcpy(info->name, "Mana Bruiser");
			info->maxHp = 10000;

			info = &game->unitTypeInfos[UNIT_ACCELERATOR];
			strcpy(info->name, "Accelerator");
			info->maxHp = 20000;

			info = &game->unitTypeInfos[UNIT_STUDENT];
			strcpy(info->name, "Student");
			info->maxHp = 3333;

			info = &game->unitTypeInfos[UNIT_TEACHER];
			strcpy(info->name, "Teacher");
			info->maxHp = 10000;

			info = &game->unitTypeInfos[UNIT_TEACHER_TEACHER];
			strcpy(info->name, "Teacher Teacher");
			info->maxHp = 30000;

			info = &game->unitTypeInfos[UNIT_MULTIHITTER];
			strcpy(info->name, "Multihitter");
			info->maxHp = 3000;

			info = &game->unitTypeInfos[UNIT_STUNNER];
			strcpy(info->name, "Stunner");
			info->maxHp = 3000;

			info = &game->unitTypeInfos[UNIT_TIME_TUNER];
			strcpy(info->name, "Time Tuner");
			info->maxHp = 50000;

			info = &game->unitTypeInfos[UNIT_SILENCER];
			strcpy(info->name, "Silencer");
			info->maxHp = 10000;

			info = &game->unitTypeInfos[UNIT_LIFE_TAKER];
			strcpy(info->name, "Life Taker");
			info->maxHp = 20000;

			info = &game->unitTypeInfos[UNIT_SPIKE];
			strcpy(info->name, "Spike");
			info->maxHp = 3000;
		}

		{
			SpellTypeInfo *info = NULL;

			for (int i = 0; i < SPELL_TYPES_MAX; i++) {
				SpellTypeInfo *info = &game->spellTypeInfos[i];
				info->enabledWhileAwake = true;
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

			info = &game->spellTypeInfos[SPELL_SUMMONER_ATTACK];
			strcpy(info->name, "Summoner Attack");
			info->damage = 3000;

			info = &game->spellTypeInfos[SPELL_QUICK_ATTACK];
			strcpy(info->name, "Quick Attack");
			info->damage = 3000;
			info->mp = 20;

			info = &game->spellTypeInfos[SPELL_STORM_ATTACK];
			strcpy(info->name, "Storm Attack");
			info->damage = 1500;
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
			info->damage = 2000;

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
			info->mp = 100;
			info->damage = 2000;

			info = &game->spellTypeInfos[SPELL_ICE];
			strcpy(info->name, "Ice");
			info->targetType = TARGET_SINGLE;
			info->mp = 100;
			info->damage = 1000;

			info = &game->spellTypeInfos[SPELL_SLEEP];
			strcpy(info->name, "Sleep");
			info->targetType = TARGET_NONE;

			info = &game->spellTypeInfos[SPELL_WAKE_UP];
			strcpy(info->name, "Wake up");
			info->targetType = TARGET_NONE;
			info->enabledWhileAwake = false;
			info->enabledWhileAsleep = true;

			info = &game->spellTypeInfos[SPELL_FOCUS];
			strcpy(info->name, "Focus");
			info->targetType = TARGET_NONE;
			info->enabledWhileAwake = false;
			info->enabledWhileAsleep = true;

			info = &game->spellTypeInfos[SPELL_DREAM];
			strcpy(info->name, "Dream");
			info->targetType = TARGET_NONE;
			info->enabledWhileAwake = false;
			info->enabledWhileAsleep = true;

			info = &game->spellTypeInfos[SPELL_VISUALIZE];
			strcpy(info->name, "Visualize");
			info->targetType = TARGET_NONE;
			info->enabledWhileAwake = false;
			info->enabledWhileAsleep = true;

			info = &game->spellTypeInfos[SPELL_STEALTH];
			strcpy(info->name, "Stealth");
			info->targetType = TARGET_NONE;
			info->enabledWhileAwake = false;
			info->enabledWhileAsleep = true;

			info = &game->spellTypeInfos[SPELL_RE_STEALTH];
			strcpy(info->name, "Re-Stealth");
			info->targetType = TARGET_NONE;
			info->enabledWhileAwake = false;
			info->enabledWhileStealthed = true;

			info = &game->spellTypeInfos[SPELL_STEALTH_WEAPON_SWITCH];
			strcpy(info->name, "Quick Weapon Switch");
			info->targetType = TARGET_NONE;
			info->enabledWhileAwake = false;
			info->enabledWhileStealthed = true;

			info = &game->spellTypeInfos[SPELL_STEALTH_REFILL];
			strcpy(info->name, "Refill");
			info->targetType = TARGET_NONE;
			info->enabledWhileAwake = false;
			info->enabledWhileStealthed = true;

			info = &game->spellTypeInfos[SPELL_STEALTH_MARK];
			strcpy(info->name, "Mark");
			info->targetType = TARGET_NONE;
			info->enabledWhileAwake = false;
			info->enabledWhileStealthed = true;

			info = &game->spellTypeInfos[SPELL_STEALTH_SNIPE];
			strcpy(info->name, "Snipe");
			info->targetType = TARGET_SINGLE;
			info->enabledWhileAwake = false;
			info->enabledWhileStealthed = true;
			info->damage = 20000;

			info = &game->spellTypeInfos[SPELL_SMALL_ETHER];
			strcpy(info->name, "Small Ether");
			info->targetType = TARGET_SINGLE;
			info->canTargetAllies = true;
			info->damage = 500;

			info = &game->spellTypeInfos[SPELL_LARGE_ETHER];
			strcpy(info->name, "Large Ether");
			info->targetType = TARGET_SINGLE;
			info->canTargetAllies = true;
			info->damage = 1000;

			info = &game->spellTypeInfos[SPELL_CREATE_SHIELD];
			strcpy(info->name, "Create Shield");
			info->targetType = TARGET_SINGLE;
			info->canTargetAllies = true;

			info = &game->spellTypeInfos[SPELL_SHIELD_BLAST];
			strcpy(info->name, "Shield Blast");
			info->targetType = TARGET_SINGLE;
			info->damage = 5000;

			info = &game->spellTypeInfos[SPELL_CREATE_2_SHIELDS];
			strcpy(info->name, "Create 2 Shields");
			info->targetType = TARGET_SINGLE;

			info = &game->spellTypeInfos[SPELL_SHIELD_BUMP];
			strcpy(info->name, "Shield Bump");
			info->targetType = TARGET_SINGLE;
			info->damage = 500;

			info = &game->spellTypeInfos[SPELL_CREATE_FAKE_SHIELD];
			strcpy(info->name, "Create Fake Shield");
			info->targetType = TARGET_SINGLE;

			info = &game->spellTypeInfos[SPELL_TAKE_MANA];
			strcpy(info->name, "Take Mana");
			info->targetType = TARGET_SINGLE;
			info->damage = 50;

			info = &game->spellTypeInfos[SPELL_TAKE_MANA_BIG];
			strcpy(info->name, "Take Mana Big");
			info->targetType = TARGET_SINGLE;
			info->damage = 100;

			info = &game->spellTypeInfos[SPELL_ACCELERATED_SLASH];
			strcpy(info->name, "Accelerated Slash");
			info->targetType = TARGET_NONE;
			info->damage = 1;

			info = &game->spellTypeInfos[SPELL_STUDENT_ATTACK];
			strcpy(info->name, "Student Attack");
			info->targetType = TARGET_SINGLE;
			info->damage = 100;

			info = &game->spellTypeInfos[SPELL_TEACHER_ATTACK];
			strcpy(info->name, "Teacher Attack");
			info->targetType = TARGET_SINGLE;
			info->damage = 300;

			info = &game->spellTypeInfos[SPELL_TEACHER_TEACHER_ATTACK];
			strcpy(info->name, "Teacher Teacher Attack");
			info->targetType = TARGET_SINGLE;
			info->damage = 900;

			info = &game->spellTypeInfos[SPELL_CREATE_STUDENT];
			strcpy(info->name, "Create Student");
			info->targetType = TARGET_NONE;

			info = &game->spellTypeInfos[SPELL_CREATE_TEACHER];
			strcpy(info->name, "Create Teacher");
			info->targetType = TARGET_NONE;

			info = &game->spellTypeInfos[SPELL_MULTIHITTER_HIT];
			strcpy(info->name, "Hit");
			info->targetType = TARGET_SINGLE;
			info->damage = 300;

			info = &game->spellTypeInfos[SPELL_MULTIHITTER_SPIN_UP];
			strcpy(info->name, "Spin Up");
			info->targetType = TARGET_NONE;

			info = &game->spellTypeInfos[SPELL_STUNNER_ATTACK];
			strcpy(info->name, "Stunner Attack");
			info->targetType = TARGET_SINGLE;
			info->damage = 100;

			info = &game->spellTypeInfos[SPELL_STUN];
			strcpy(info->name, "Stun");
			info->targetType = TARGET_SINGLE;

			info = &game->spellTypeInfos[SPELL_GAIN_TIME_TUNE_CHARGE];
			strcpy(info->name, "Gain Time Tune Charge");
			info->targetType = TARGET_NONE;

			info = &game->spellTypeInfos[SPELL_TIME_TUNE];
			strcpy(info->name, "Time Tune");
			info->targetType = TARGET_NONE;

			info = &game->spellTypeInfos[SPELL_VOLUME_WARNING];
			strcpy(info->name, "Volume Warning");
			info->targetType = TARGET_SINGLE;

			info = &game->spellTypeInfos[SPELL_TAKE_LIFE];
			strcpy(info->name, "Take Life");
			info->targetType = TARGET_SINGLE;

			info = &game->spellTypeInfos[SPELL_WAIT];
			info->targetType = TARGET_NONE;
			strcpy(info->name, "Wait");
			info->enabledWhileAsleep = true;

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

			info = &game->buffTypeInfos[BUFF_SHIELD];
			strcpy(info->name, "Shield");

			info = &game->buffTypeInfos[BUFF_FAKE_SHIELD];
			strcpy(info->name, "Fake Shield");

			info = &game->buffTypeInfos[BUFF_SPUN_UP];
			strcpy(info->name, "Spun Up");

			info = &game->buffTypeInfos[BUFF_STUNNED];
			strcpy(info->name, "Stunned");

			info = &game->buffTypeInfos[BUFF_TIME_TUNE_CHARGE];
			strcpy(info->name, "Time Tune Charge");

			info = &game->buffTypeInfos[BUFF_VOLUME_WARNING];
			strcpy(info->name, "Volume Warning");

			info = &game->buffTypeInfos[BUFF_SILENCE];
			strcpy(info->name, "Silence");

			info = &game->buffTypeInfos[BUFF_DAMAGE_SPRING];
			strcpy(info->name, "Damage Spring");
		}

		game->baseSpellTime = 0.25;

		{
			Unit *unit = NULL;

			unit = createUnit(UNIT_PLAYER1);
			unit->ally = true;
			unit->weapon = WEAPON_DEFAULT;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_HERO_ATTACK;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_STORM_ATTACK;
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
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_SLEEP;
			// unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_WAKE_UP;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_FOCUS;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_DREAM;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_VISUALIZE;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_STEALTH;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_RE_STEALTH;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_STEALTH_WEAPON_SWITCH;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_STEALTH_REFILL;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_STEALTH_MARK;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_STEALTH_SNIPE;

			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_SMALL_ETHER;
			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_LARGE_ETHER;

			unit->spellsAvailable[unit->spellsAvailableNum++] = SPELL_WAIT;
		}

		saveLoadGlobals(false);

		maximizeWindow();
	}

	Globals *globals = &game->globals;
	float elapsed = platform->elapsed;

	if (!equal(game->realSize, v2(platform->windowSize))) {
		game->size = v2(1920, 1080);
		game->realSize = v2(platform->windowSize);
		game->sizeScale = game->realSize / game->size;

		if (game->defaultFont) destroyFont(game->defaultFont);
		game->defaultFont = NULL;
		if (game->hudFont) destroyFont(game->hudFont);
		game->hudFont = NULL;
	}
	if (!game->defaultFont) game->defaultFont = createFont("assets/common/arial.ttf", (int)(42 * game->sizeScale.y));
	if (!game->hudFont) game->hudFont = createFont("assets/common/arial.ttf", (int)(14 * game->sizeScale.y));

	game->mouse = platform->mouse / game->sizeScale;

	ngui->mouse = platform->mouse;
	ngui->uiScale = game->sizeScale;
	ngui->screenSize = game->realSize;

	if (keyJustPressed(KEY_BACKTICK)) game->inEditor = !game->inEditor;
	if (game->inEditor) {
		ImGui::Begin("Editor", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		if (ImGui::Button("Save Globals")) saveLoadGlobals(true);
		ImGui::SameLine();
		if (ImGui::Button("Load Globals")) saveLoadGlobals(false);
		ImGui::DragFloat2("characterCardSize", &globals->characterCardSize.x);
		ImGui::Separator();
		ImGui::InputFloat("baseSpellTime", &game->baseSpellTime);

		if (ImGui::TreeNode("Ngui Styles")) {
			nguiShowImGuiStylesEditor(globals->customStyleStacks, &globals->customStyleStacksNum, CUSTOM_STYLE_STACKS_MAX);
			ImGui::TreePop();
		}

		ImGui::Separator();
		ImGui::InputInt("Starting wave", &game->startingWave);
		ImGui::Text("allyAdvantage: %f", getAllyAdvantage());
		ImGui::Text("allyAdvantageGain: %f", game->prevAllyAdvantageGain);
		ImGui::End();
	}

	Pass *mainPass = createPass();
	mainPass->yIsUp = false;
	Pass *overlayPass = createPass();
	overlayPass->yIsUp = false;

	pushPass(mainPass);

	if (game->prevState != game->state) {
		game->prevState = game->state;
		game->stateTime = 0;
	}

	if (game->state == GAME_STATE_MENU) {
		nguiStartWindow("Main Menu Window", game->realSize*0.5, v2(0.5, 0.5));
		for (int i = 1; i < 9+1; i++) {
			if (nguiButton(frameSprintf("Level %d", i))) {
				game->level = i;
				game->wave = game->startingWave;
				game->state = GAME_STATE_PLAY;
			}
		}
		nguiEndWindow();
	} else if (game->state == GAME_STATE_PLAY) {
		if (game->stateTime == 0) nextWave();

		bool choosingAlly = false;
		{ /// Update turn
			Unit *currentUnit = getCurrentUnit();

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

				if (getBuff(currentUnit, BUFF_FROZEN)) {
					logf("Frozen\n");
					castSpell(currentUnit, NULL, SPELL_END_TURN);
				}

				if (getBuff(currentUnit, BUFF_STUNNED)) {
					logf("Stunned\n");
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

				if (game->inEditor) {
					ImGui::SetNextWindowPos(ImVec2(platform->windowWidth*0.5, platform->windowHeight*0.98), ImGuiCond_Always, ImVec2(0.5, 1.0));
					ImGui::SetNextWindowSize(ImVec2(platform->windowWidth*0.5, platform->windowHeight*0.2), ImGuiCond_Always);
					ImGui::Begin("Spells", NULL, 0);
				}

				if (allyCount == 0) {
					if (game->inEditor) {
						ImGui::Text("You lose");
						if (ImGui::Button("Try again")) logf("You can't\n");
					}
					game->winLossTime += elapsed;
				} else if (enemyCount == 0) {
					if (game->inEditor) {
						ImGui::Text("You win");
						if (ImGui::Button("Continue")) nextWave();
					}
					if (game->winLossTime >= 3) nextWave();
					game->winLossTime += elapsed;
				} else if (currentUnit) {
					game->winLossTime = 0;

					if (currentUnit->ally) {
						if (!game->chosenAllyUnit) {
							Unit *p1 = getUnitByType(UNIT_PLAYER1);
							Unit *p2 = getUnitByType(UNIT_PLAYER2);
							Unit *choices[2] = {};
							int choicesNum = 0;
							if (p1->hasTurn) choices[choicesNum++] = p1;
							if (p2->hasTurn) choices[choicesNum++] = p2;

							if (choicesNum == 1) game->chosenAllyUnit = choices[0]->id;
							if (choicesNum == 2) choosingAlly = true;

							if (choosingAlly) {
								for (int i = 0; i < choicesNum; i++) {
									Unit *unit = choices[i];
									if (unit->hp <= 0) continue;
									if (!unit->hasTurn) continue;
									if (game->inEditor) {
										if (ImGui::Button(unit->info->name)) game->chosenAllyUnit = unit->id;
									}
								}
							}
						} else {
							if (game->currentSpellType == SPELL_NONE) {
								int spellCount = 0;
								pushGameStyleStack("Spell Window Style");
								nguiStartWindow("SpellWindow", game->realSize, v2(1, 1));
								for (int i = 0; i < currentUnit->spellsAvailableNum; i++) {
									SpellType type = currentUnit->spellsAvailable[i];
									SpellTypeInfo *info = &game->spellTypeInfos[type];
									if (!info->enabledWhileAwake && !currentUnit->asleep) continue;
									if (!info->enabledWhileAsleep && currentUnit->asleep) continue;
									if (!info->enabledWhileStealthed && getBuff(currentUnit, BUFF_STEALTHED)) continue;
									int spellAvailableIndex = i;

									char *label = info->name;
									if (info->mp > 0) label = frameSprintf("%s (%dmp)", label, info->mp);
									if (currentUnit->spellsAvailableAmounts[spellAvailableIndex] > -1) label = frameSprintf("%s x%d", label, currentUnit->spellsAvailableAmounts[spellAvailableIndex]);

									bool clickedSpell = false;
									char *tooltipString = frameSprintf("Base damage: %d", info->damage);
									if (game->inEditor) {
										ImGui::PushID(i);
										if (spellCount % 5 != 0) ImGui::SameLine();
										if (type == SPELL_WAIT) ImGui::NewLine();
										if (ImGui::Button(label)) clickedSpell = true;
										if (ImGui::IsItemHovered()) {
											if (info->damage) ImGui::SetTooltip(tooltipString);
										}
										ImGui::PopID();
									}

									if (nguiButton(label)) clickedSpell = true;
									if (ngui->lastElement->hoveringTime) game->tooltip = tooltipString;

									if (clickedSpell) {
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

										if (info->mp && getBuff(currentUnit, BUFF_SILENCE)) {
											canCast = false;
											logf("Silenced\n");
										}

										if (canCast) {
											game->currentSpellType = type;
											game->currentSpellAvailableIndex = i;
										}
									}

									spellCount++;
								}

								if (game->inEditor) {
									if (ImGui::Button("Cancel")) game->chosenAllyUnit = 0;
								}

								nguiEndWindow();
								popGameStyleStack("Spell Window Style");
							} else {
								SpellTypeInfo *currentSpellInfo = &game->spellTypeInfos[game->currentSpellType];
								if (currentSpellInfo->targetType == TARGET_SINGLE) {
									if (game->inEditor) {
										ImGui::Text("Target:");
									}
									for (int i = 0; i < game->unitsNum; i++) {
										Unit *unit = &game->units[i];
										if (unit->ally && !currentSpellInfo->canTargetAllies) continue;
										if (unit->hp <= 0 && !currentSpellInfo->canTargetDead) continue;
										if (isHidden(unit)) continue;

										if (game->inEditor) {
											if (ImGui::Button(frameSprintf("%d: %s", i, unit->screenName))) game->targetedUnit = unit->id;
										}
										unit->couldBeTargettedThisFrame = true;

										if (unit->id == game->targetedUnit) {
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
												game->targetedUnit = 0;
											}
										}
									}

									if (platform->rightMouseJustDown || keyJustPressed(KEY_ESC)) game->currentSpellType = SPELL_NONE;

									if (game->inEditor) {
										if (ImGui::Button("Cancel")) game->currentSpellType = SPELL_NONE;
									}
								} else if (currentSpellInfo->targetType == TARGET_NONE) {
									currentUnit->spellsAvailableAmounts[game->currentSpellAvailableIndex]--;
									castSpell(currentUnit, NULL, game->currentSpellType);
									castSpell(currentUnit, NULL, SPELL_END_TURN);
									game->currentSpellType = SPELL_NONE;
								}
							}
						}
					} else {
						aiTakeTurn(currentUnit);
					}
				}

				if (game->inEditor) {
					ImGui::End();
				}
			}
		} ///

		{ /// Display
			if (game->inEditor) {
				ImGui::SetNextWindowPos(ImVec2(platform->windowWidth*0.5, platform->windowHeight*0.5), ImGuiCond_Always, ImVec2(0.5, 0.5));
				ImGui::Begin("Game", NULL, ImGuiWindowFlags_AlwaysAutoResize);

				auto guiShowUnit = [](Unit *unit) {
					int color = 0xFFFFFFFF;
					if (unit == getCurrentUnit()) color = lerpColor(color, 0xFF00FF00, 0.5);
					if (game->spellQueueNum != 0 && game->spellQueue[0].destId == unit->id) color = lerpColor(color, 0xFFFF0000, 0.5);

					int maxHp = unit->info->maxHp * (1 - unit->maxHpReductionPerc);

					ImVec4 imCol = guiGetImVec4Color(color);
					ImGui::TextColored(imCol, "%s", unit->screenName);
					ImGui::TextColored(imCol, "Hp: %d/%d", unit->hp, maxHp);
					if (unit->ally) ImGui::TextColored(imCol, "Mp: %d/%d", unit->mp, unit->info->maxMp);
					if (unit->weapon) ImGui::TextColored(imCol, "Weapon: %s", weaponStrings[unit->weapon]);
					if (unit->buffsNum > 0) {
						for (int i = 0; i < unit->buffsNum; i++) {
							Buff *buff = &unit->buffs[i];
							char *buffName = buff->info->name;
							if (buff->type == BUFF_FAKE_SHIELD && buff->time > 15) buffName = game->buffTypeInfos[BUFF_SHIELD].name;
							if (buff->type == BUFF_COMBO) {
								ImGui::Text("[%s (%d):%d]", buffName, buff->intUserData, buff->turns);
							} else {
								ImGui::Text("[%s:%d]", buffName, buff->turns);
							}
						}
					}
					for (int i = 0; i < game->effectsNum; i++) {
						Effect *effect = &game->effects[i];
						if (effect->unitId != unit->id) continue;

						if (effect->type == EFFECT_DAMAGE) {
							ImGui::TextColored(guiGetImVec4Color(0xFFFF3030), "-%d", effect->intValue);
						} else if (effect->type == EFFECT_GAIN_HP) {
							ImGui::TextColored(guiGetImVec4Color(0xFF30FF30), "+%d", effect->intValue);
						} else if (effect->type == EFFECT_LOSE_MP) {
							ImGui::TextColored(guiGetImVec4Color(0xFF3030FF), "-%d", effect->intValue);
						} else if (effect->type == EFFECT_GAIN_MP) {
							ImGui::TextColored(guiGetImVec4Color(0xFF303080), "+%d", effect->intValue);
						} else if (effect->type == EFFECT_DODGE) {
							ImGui::TextColored(guiGetImVec4Color(0xFF808080), "Dodged");
						}
					}
					ImGui::Separator();
				};

				Vec2 childSize = v2(500, 800);

				ImGui::BeginChild("AlliesChild", ImVec2(childSize.x, childSize.y), false);
				for (int i = 0; i < game->unitsNum; i++) {
					Unit *unit = &game->units[i];
					if (!unit->ally) continue;
					if (isHidden(unit)) continue;
					guiShowUnit(unit);
				}
				ImGui::EndChild();

				ImGui::SameLine();

				ImGui::BeginChild("EnemiesChild", ImVec2(childSize.x, childSize.y), false);
				for (int i = 0; i < game->unitsNum; i++) {
					Unit *unit = &game->units[i];
					if (unit->ally) continue;
					if (isHidden(unit)) continue;
					guiShowUnit(unit);
				}
				ImGui::EndChild();

				ImGui::End();
			}
		} ///

		{ /// Display 2d
			int totalAllies = 0;
			int totalEnemies = 0;
			for (int i = 0; i < game->unitsNum; i++) {
				Unit *unit = &game->units[i];
				if (unit->ally) totalAllies++;
				else totalEnemies++;
			}

			float cardPadding = 5;
			float allyWidth = totalAllies * (globals->characterCardSize.x + cardPadding);
			float enemyWidth = totalEnemies * (globals->characterCardSize.x + cardPadding);

			Vec2 allyCursor = v2();
			allyCursor.x = game->size.x/2 - allyWidth/2;
			allyCursor.y = game->size.y - globals->characterCardSize.y;

			Unit *currentUnit = getCurrentUnit();
			if (currentUnit->ally && game->chosenAllyUnit && game->currentSpellType == SPELL_NONE && game->spellQueueNum == 0) {
				allyCursor.x = cardPadding;
			}

			Vec2 enemyCursor = v2();
			enemyCursor.x = game->size.x/2 - enemyWidth/2;
			enemyCursor.y = 0;

			for (int i = 0; i < game->unitsNum; i++) {
				Unit *unit = &game->units[i];
				if (isHidden(unit)) continue;

				Vec2 *cursor = NULL;
				Vec2 forward = v2();
				if (unit->ally) {
					cursor = &allyCursor;
					forward = v2(0, -1);
				} else {
					cursor = &enemyCursor;
					forward = v2(0, 1);
				}

				int cardBgColor = 0xFF202020;
				Rect cardRect = makeRect(*cursor, globals->characterCardSize);

				bool couldBeSelected = false;
				if (unit->ally) couldBeSelected = true;
				if (unit->hp <= 0) couldBeSelected = false;
				if (!unit->hasTurn) couldBeSelected = false;
				if (unit->couldBeTargettedThisFrame) couldBeSelected = false;

				if (couldBeSelected) {
					if (choosingAlly) cardBgColor = lerpColor(cardBgColor, 0xFFFFFFFF, timePhase(platform->time * 3) * 0.1);
				}

				if (unit->couldBeTargettedThisFrame) {
					cardBgColor = lerpColor(cardBgColor, 0xFFFF0000, 0.1);
				}

				if (unit == currentUnit && !couldBeSelected) {
					cardBgColor = lerpColor(cardBgColor, 0xFFFFFF00, timePhase(platform->time * 1) * 0.1);
					cardRect = offset(cardRect, forward * 20);
				}

				bool hoveringCard = contains(cardRect, game->mouse);
				if (hoveringCard) {
					if (platform->mouseJustDown) {
						if (couldBeSelected) game->chosenAllyUnit = unit->id;
						if (unit->couldBeTargettedThisFrame) game->targetedUnit = unit->id;
					}
				}

				unit->cardRect.x = lerp(unit->cardRect.x, cardRect.x, 0.2);
				unit->cardRect.y = lerp(unit->cardRect.y, cardRect.y, 0.2);
				unit->cardRect.width = cardRect.width;
				unit->cardRect.height = cardRect.height;
				cardRect = unit->cardRect;

				drawRect(cardRect, cardBgColor);

				Rect topRect = cardRect;
				{
					topRect.height *= 0.6;
					// drawRect(topRect, 0xFF00FF00);

					Rect profilePicRect = makeCenteredSquare(getCenter(topRect), topRect.height);
					if (unit->buffsNum > 0) profilePicRect.x = topRect.x;
					drawRect(profilePicRect, 0xFF300000);
					// drawCircle(getCenter(profilePicRect), profilePicRect.width/2, 0xFFFF0000);

					Rect buffRect = makeRect();
					buffRect.x = profilePicRect.x + profilePicRect.width;
					buffRect.y = profilePicRect.y;
					buffRect.width = (topRect.width - profilePicRect.width)/2;
					buffRect.height = buffRect.width;
					for (int i = 0; i < unit->buffsNum; i++) {
						Buff *buff = &unit->buffs[i];
						if (i%2 == 0 && i != 0) {
							buffRect.x = profilePicRect.x + profilePicRect.width;
							buffRect.y += buffRect.height;
						}

						if (contains(buffRect, game->mouse)) {
							game->tooltip = frameSprintf("%s", buff->info->name);
							if (buff->turns > 0) game->tooltip = frameSprintf("%s\n%d turns left", game->tooltip, buff->turns);
						}

						drawRect(inflatePerc(buffRect, -0.1), 0xFF008000);
						drawTextInRect(buff->info->name, newDrawTextProps(game->defaultFont, 0xFF000000), buffRect, v2(0.5, 0.5));

						buffRect.x += buffRect.width;
					}

					Rect nameRect = topRect;
					nameRect.height *= 0.5;
					nameRect.y += nameRect.height;
					drawTextInRect(unit->screenName, newDrawTextProps(game->defaultFont, 0x80FFFFFF), nameRect, v2(0.5, 1));
				}

				Rect bottomRect = cardRect;
				{
					bottomRect.y = topRect.y + topRect.height;
					bottomRect.height = cardRect.height - topRect.height;

					auto drawBarRect = [](Rect barRegion, char *text, float perc, int barColor, int textColor) {
						Rect hpBarRect = inflatePerc(barRegion, v2(-0.3, -0.5));
						Rect hpRect = hpBarRect;
						hpRect.width *= perc;
						drawRect(hpRect, barColor);
						drawRectOutline(hpBarRect, 2, 0xFF000000);

						Rect textRect = barRegion;
						textRect.height *= 0.5;
						textRect.y += textRect.height;
						drawTextInRect(text, newDrawTextProps(game->defaultFont, textColor), textRect, v2(1, 0.5));
					};

					Rect hpRegion = bottomRect;
					hpRegion.height *= 0.5;
					drawBarRect(hpRegion, frameSprintf("%dhp", unit->hp), unit->hp / (float)unit->info->maxHp, 0xFF008000, 0x90FFFFFF);

					Rect mpRegion = hpRegion;
					mpRegion.y += mpRegion.height;
					drawBarRect(mpRegion, frameSprintf("%dmp", unit->mp), unit->mp / (float)unit->info->maxMp, 0xFF000080, 0x90FFFFFF);
				}

				unit->couldBeTargettedThisFrame = false;
				cursor->x += globals->characterCardSize.x + cardPadding;
			}

			{ /// Effects 2d
				for (int i = 0; i < game->effectsNum; i++) {
					Effect *effect = &game->effects[i];

					char *text = NULL;
					int color = 0xFFFFFFFF;
					bool bounceText = false;
					bool slideText = false;;
					if (effect->type == EFFECT_DAMAGE) {
						text = frameSprintf("-%d", effect->intValue);
						color = 0xFFFF3030;
						bounceText = true;
					} else if (effect->type == EFFECT_GAIN_HP) {
						text = frameSprintf("+%d", effect->intValue);
						color = 0xFF30FF30;
						slideText = true;
					} else if (effect->type == EFFECT_LOSE_MP) {
						text = frameSprintf("-%d", effect->intValue);
						color = 0xFF3030FF;
						slideText = true;
					} else if (effect->type == EFFECT_GAIN_MP) {
						text = frameSprintf("+%d", effect->intValue);
						color = 0xFF303080;
						slideText = true;
					} else if (effect->type == EFFECT_DODGE) {
						text = frameSprintf("Dodged");
						color = 0xFF808080;
						slideText = true;
					}

					if (text) {
						Unit *unit = getUnit(effect->unitId);

						Vec2 position = v2();
						Vec2 size = getTextSize(game->defaultFont, text);
						position = effect->position - size/2;

						if (bounceText) position.y += clampMap(effect->time, 0, 0.5, 0, 20, BOUNCE_OUT);
						if (slideText) position.y += clampMap(effect->time, 0, 0.5, 0, 20, QUAD_OUT);

						if (unit) {
							position += getCenter(unit->cardRect); 
							Vec2 forward = unit->ally ? v2(0, -1) : v2(0, 1);
							position += forward * globals->characterCardSize*0.5;
						}

						drawRect(makeRect(position, size), 0x20000000);
						drawText(text, newDrawTextProps(game->defaultFont, color, position));
					}
				}
			} ///
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
					spell->type == SPELL_LARGE_ATTACK ||
					spell->type == SPELL_SUMMONER_ATTACK
				) {
					if (game->spellTime == 0) dealDamage(src, dest, spell->info->damage);
				} else if (spell->type == SPELL_STORM_ATTACK) {
					if (spellTimeJustPassed(baseSpellTime*0.25)) dealDamage(src, dest, spell->info->damage/3);
					if (spellTimeJustPassed(baseSpellTime*0.50)) dealDamage(src, dest, spell->info->damage/3);
					if (spellTimeJustPassed(baseSpellTime*0.75)) dealDamage(src, dest, spell->info->damage/3);
				} else if (spell->type == SPELL_QUICK_ATTACK) {
					bool oldSpellImmNeverMisses = game->spellImmNeverMisses;
					game->spellImmNeverMisses = true;
					if (spellTimeJustPassed(baseSpellTime*0.25)) dealDamage(src, dest, spell->info->damage/2);
					if (spellTimeJustPassed(baseSpellTime*0.75)) dealDamage(src, dest, spell->info->damage/2);
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
						loseMp(p2, spell->info->mp);
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
						for (int i = 0; i < countBuffs(src, BUFF_ADD_ATTACK_REDUCTION); i++) giveBuff(dest, BUFF_ATTACK_REDUCTION, 2);
						for (int i = 0; i < countBuffs(src, BUFF_ADD_DEFENSE_REDUCTION); i++) giveBuff(dest, BUFF_DEFENSE_REDUCTION, 2);
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
				} else if (spell->type == SPELL_SLEEP) {
					if (spellTimeJustPassed(baseSpellTime*0.25)) gainHp(src, 1000);
					if (spellTimeJustPassed(baseSpellTime*0.75)) src->asleep = true;
				} else if (spell->type == SPELL_WAKE_UP) {
					if (spellTimeJustPassed(baseSpellTime*0.5)) src->asleep = false;
				} else if (spell->type == SPELL_FOCUS) {
					if (spellTimeJustPassed(baseSpellTime*0.25)) giveBuff(src, BUFF_FOCUS, 3);
					if (spellTimeJustPassed(baseSpellTime*0.75)) src->asleep = false;
				} else if (spell->type == SPELL_DREAM) {
					if (spellTimeJustPassed(baseSpellTime*0.5)) gainHp(src, src->info->maxHp);
				} else if (spell->type == SPELL_VISUALIZE) {
					if (spellTimeJustPassed(baseSpellTime*0.25)) giveBuff(src, BUFF_VISUALIZE, 3);
					if (spellTimeJustPassed(baseSpellTime*0.75)) src->asleep = false;
				} else if (spell->type == SPELL_STEALTH) {
					if (spellTimeJustPassed(baseSpellTime*0.5)) giveBuff(src, BUFF_STEALTHED, 2);
				} else if (spell->type == SPELL_RE_STEALTH) {
					if (spellTimeJustPassed(baseSpellTime*0.5)) {
						removeAllBuffsOfType(src, BUFF_STEALTHED);
						giveBuff(src, BUFF_STEALTHED, 2);
					}
				} else if (spell->type == SPELL_STEALTH_WEAPON_SWITCH) {
				} else if (spell->type == SPELL_STEALTH_REFILL) {
					if (spellTimeJustPassed(baseSpellTime*0.25)) refillFlasks(src);
					if (spellTimeJustPassed(baseSpellTime*0.75)) removeAllBuffsOfType(src, BUFF_STEALTHED);
				} else if (spell->type == SPELL_STEALTH_MARK) {
					if (spellTimeJustPassed(baseSpellTime*0.25)) {
						for (int i = 0; i < game->unitsNum; i++) {
							for (int i = 0; i < game->unitsNum; i++) {
								Unit *unit = &game->units[i];
								if (unit->ally == src->ally) continue;
								if (unit->hp <= 0) continue;
								if (isHidden(unit)) continue;

								if (getBuff(unit, BUFF_BLEED)) giveBuff(unit, BUFF_MARKED, 4);
							}
						}
					}
					if (spellTimeJustPassed(baseSpellTime*0.75)) removeAllBuffsOfType(src, BUFF_STEALTHED);
				} else if (spell->type == SPELL_STEALTH_SNIPE) {
					if (spellTimeJustPassed(baseSpellTime*0.25)) dealDamage(src, dest, spell->info->damage);
					if (spellTimeJustPassed(baseSpellTime*0.75)) removeAllBuffsOfType(src, BUFF_STEALTHED);
				} else if (spell->type == SPELL_SMALL_ETHER) {
					if (spellTimeJustPassed(baseSpellTime*0.5)) gainMp(dest, spell->info->damage);
				} else if (spell->type == SPELL_LARGE_ETHER) {
					if (spellTimeJustPassed(baseSpellTime*0.5)) gainMp(dest, spell->info->damage);
				} else if (spell->type == SPELL_CREATE_SHIELD) {
					if (spellTimeJustPassed(baseSpellTime*0.5)) giveBuff(dest, BUFF_SHIELD, -1);
				} else if (spell->type == SPELL_SHIELD_BLAST) {
					if (spellTimeJustPassed(baseSpellTime*0.5)) {
						dealDamage(src, dest, spell->info->damage);
						for (int i = 0; i < 5; i++) {
							Buff *buff = getBuff(src, BUFF_SHIELD);
							if (buff) removeBuff(src, buff);
						}
					}
				} else if (spell->type == SPELL_CREATE_2_SHIELDS) {
					if (spellTimeJustPassed(baseSpellTime*0.5)) {
						giveBuff(dest, BUFF_SHIELD, -1);
						giveBuff(dest, BUFF_SHIELD, -1);
					}
				} else if (spell->type == SPELL_SHIELD_BUMP) {
					if (spellTimeJustPassed(baseSpellTime*0.5)) {
						int shields = countBuffs(src, BUFF_SHIELD);
						dealDamage(src, dest, spell->info->damage * shields);
					}
				} else if (spell->type == SPELL_CREATE_FAKE_SHIELD) {
					if (spellTimeJustPassed(baseSpellTime*0.5)) giveBuff(dest, BUFF_FAKE_SHIELD, -1);
				} else if (spell->type == SPELL_TAKE_MANA) {
					if (spellTimeJustPassed(baseSpellTime*0.5)) loseMp(dest, spell->info->damage);
				} else if (spell->type == SPELL_TAKE_MANA_BIG) {
					if (spellTimeJustPassed(baseSpellTime*0.5)) loseMp(dest, spell->info->damage);
				} else if (spell->type == SPELL_ACCELERATED_SLASH) {
					if (spellTimeJustPassed(baseSpellTime*0.5)) {
						for (int i = 0; i < game->unitsNum; i++) {
							Unit *unit = &game->units[i];
							if (unit->ally == src->ally) continue;
							if (unit->hp <= 0) continue;
							if (isHidden(unit)) continue;
							dealDamage(src, unit, pow(2, src->accelerationCount));
						}

						src->accelerationCount++;
					}
				} else if (spell->type == SPELL_STUDENT_ATTACK) {
					if (spellTimeJustPassed(baseSpellTime*0.3)) dealDamage(src, dest, spell->info->damage/2);
					if (spellTimeJustPassed(baseSpellTime*0.6)) dealDamage(src, dest, spell->info->damage/2);
				} else if (spell->type == SPELL_TEACHER_ATTACK) {
					if (spellTimeJustPassed(baseSpellTime*0.5)) dealDamage(src, dest, spell->info->damage);
				} else if (spell->type == SPELL_TEACHER_TEACHER_ATTACK) {
					if (spellTimeJustPassed(baseSpellTime*0.25)) dealDamage(src, dest, spell->info->damage/3);
					if (spellTimeJustPassed(baseSpellTime*0.5)) dealDamage(src, dest, spell->info->damage/3);
					if (spellTimeJustPassed(baseSpellTime*0.75)) dealDamage(src, dest, spell->info->damage/3);
				} else if (spell->type == SPELL_CREATE_STUDENT) {
					if (spellTimeJustPassed(baseSpellTime*0.5)) createUnit(UNIT_STUDENT);
				} else if (spell->type == SPELL_CREATE_TEACHER) {
					if (spellTimeJustPassed(baseSpellTime*0.5)) createUnit(UNIT_TEACHER);
				} else if (spell->type == SPELL_MULTIHITTER_HIT) {
					if (spellTimeJustPassed(baseSpellTime*0.25)) dealDamage(src, dest, spell->info->damage/3);
					if (spellTimeJustPassed(baseSpellTime*0.5)) dealDamage(src, dest, spell->info->damage/3);
					if (spellTimeJustPassed(baseSpellTime*0.75)) dealDamage(src, dest, spell->info->damage/3);
				} else if (spell->type == SPELL_MULTIHITTER_SPIN_UP) {
					if (spellTimeJustPassed(baseSpellTime*0.5)) giveBuff(dest, BUFF_SPUN_UP, 3);
				} else if (spell->type == SPELL_STUNNER_ATTACK) {
					if (spellTimeJustPassed(baseSpellTime*0.5)) dealDamage(src, dest, spell->info->damage);
				} else if (spell->type == SPELL_STUN) {
					if (spellTimeJustPassed(baseSpellTime*0.5)) giveBuff(dest, BUFF_STUNNED, 1);
				} else if (spell->type == SPELL_GAIN_TIME_TUNE_CHARGE) {
					if (spellTimeJustPassed(baseSpellTime*0.5)) giveBuff(dest, BUFF_TIME_TUNE_CHARGE, -1);
				} else if (spell->type == SPELL_TIME_TUNE) {
					if (spellTimeJustPassed(baseSpellTime*0.5)) {
						removeAllBuffsOfType(src, BUFF_TIME_TUNE_CHARGE);
						for (int i = 0; i < game->unitsNum; i++) {
							Unit *unit = &game->units[i];
							if (unit->hp <= 0) continue;
							if (unit->ally != src->ally) continue;
							unit->extraTurns++;
						}
					}
				} else if (spell->type == SPELL_VOLUME_WARNING) {
					if (spellTimeJustPassed(baseSpellTime*0.5)) giveBuff(dest, BUFF_VOLUME_WARNING, 1);
				} else if (spell->type == SPELL_TAKE_LIFE) {
					if (spellTimeJustPassed(baseSpellTime*0.5)) {
						dest->maxHpReductionPerc += 0.01;
						int maxHp = dest->info->maxHp * (1 - dest->maxHpReductionPerc);
						if (dest->hp > maxHp) dest->hp = maxHp;
					}
				} else if (spell->type == SPELL_WAIT) {
					if (game->spellTime == 0) logf("Waiting...\n");
				} else if (spell->type == SPELL_END_TURN) {
					if (game->spellTime > baseSpellTime) {
						for (int i = 0; i < src->buffsNum; i++) {
							Buff *buff = &src->buffs[i];
							buff->turns--;

							bool oldSpellImmNeverMisses = game->spellImmNeverMisses;
							game->spellImmNeverMisses = true;

							if (buff->type == BUFF_BLEED) dealDamage(NULL, src, 500, true);
							if (buff->type == BUFF_POISON) dealDamage(NULL, src, 100, true);

							game->spellImmNeverMisses = oldSpellImmNeverMisses;

							if (buff->turns == 0) {
								arraySpliceIndex(src->buffs, src->buffsNum, sizeof(Buff), i);
								src->buffsNum--;
								i--;
								continue;
							}
						}

						logf("%s's turn is over\n", src->info->name);

						src->hasTurn = false;
						game->chosenAllyUnit = 0;
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

		{ /// Remove dead units
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
		} ///

		{ /// Unit frame loop
			for (int i = 0; i < game->unitsNum; i++) {
				Unit *unit = &game->units[i];
				for (int i = 0; i < unit->buffsNum; i++) {
					Buff *buff = &unit->buffs[i];
					buff->time += elapsed;
				}
			}
		} ///

		{ /// Update effects
			for (int i = 0; i < game->effectsNum; i++) {
				Effect *effect = &game->effects[i];

				bool complete = false;
				float maxTime = 3;

				if (effect->type == EFFECT_DAMAGE) {
				} else if (effect->type == EFFECT_LOSE_MP) {
				} else if (effect->type == EFFECT_DODGE) {
				}

				effect->time += elapsed;
				if (effect->time > maxTime) complete = true;

				if (complete) {
					arraySpliceIndex(game->effects, game->effectsNum, sizeof(Effect), i);
					game->effectsNum--;
					i--;
					continue;
				}
			}
		} ///
	}

	game->stateTime += elapsed;

	{ /// Tooltip
		pushPass(overlayPass);
		if (game->tooltip) {
			DrawTextProps props = newDrawTextProps(game->defaultFont, 0xFFFFFFFF);
			Vec2 size = getTextSize(game->tooltip, props);
			props.position.x = game->mouse.x;
			props.position.y = game->mouse.y - size.y;
			drawRect(makeRect(props.position, size), 0xC0000000);
			drawText(game->tooltip, props);
			game->tooltip = NULL;
		}
		popPass();
	} ///

	clearRenderer();

	popPass();

	drawPass(mainPass);
	destroyPass(mainPass);

	nguiDraw(elapsed);

	drawPass(overlayPass);
	destroyPass(overlayPass);

	guiDraw();
	drawOnScreenLog();
}

void drawPass(Pass *pass) {
	{ /// Scale pass
		for (int i = 0; i < pass->cmdsNum; i++) {
			PassCmd *cmd = &pass->cmds[i];
			if (cmd->type == PASS_CMD_QUAD) {
				for (int i = 0; i < 4; i++) {
					Vec3 *vert = &cmd->verts[i];
					vert->x = map(vert->x, 0, game->size.x, 0, game->realSize.x);
					vert->y = map(vert->y, 0, game->size.y, 0, game->realSize.y);
				}
			}
		}
	} ///

	{ /// Draw pass
		for (int i = 0; i < pass->cmdsNum; i++) {
			PassCmd *cmd = &pass->cmds[i];
			if (cmd->type == PASS_CMD_QUAD) {
				Vec2 verts[4] = {};
				verts[0] = v2(cmd->verts[0]);
				verts[1] = v2(cmd->verts[1]);
				verts[2] = v2(cmd->verts[2]);
				verts[3] = v2(cmd->verts[3]);

				// Matrix3 flipMatrix = {
				// 	1,  0,  0,
				// 	0, -1,  0,
				// 	0,  1,  1
				// };
				// for (int i = 0; i < 4; i++) cmd->uvs[i] = flipMatrix * cmd->uvs[i];
				drawTexturedQuad(cmd->textureId, verts, cmd->uvs, cmd->colors);
			} else {
				logf("Bad pass cmd type\n");
			}
		}
	} ///
}

Unit *getCurrentUnit() {
	if (game->chosenAllyUnit != UNIT_NONE) return getUnit(game->chosenAllyUnit);

	bool hasExtraTurns = false;
	for (int i = 0; i < game->unitsNum; i++) {
		Unit *unit = &game->units[i];
		if (unit->hp <= 0) continue;
		if (unit->extraTurns > 0) hasExtraTurns = true;
		if (unit->hasTurn) return unit;
	}

	if (hasExtraTurns) {
		for (int i = 0; i < game->unitsNum; i++) {
			Unit *unit = &game->units[i];
			if (unit->hp <= 0) continue;
			if (unit->extraTurns > 0) {
				unit->extraTurns--;
				unit->hasTurn = true;
			}
		}
	} else {
		// Round ends here
		game->prevAllyAdvantageGain = (getAllyAdvantage() - game->prevAllyAdvantage) / game->prevAllyAdvantage;
		game->prevAllyAdvantage = getAllyAdvantage();
		for (int i = 0; i < game->unitsNum; i++) {
			Unit *unit = &game->units[i];
			if (unit->hp <= 0) continue;
			unit->hasTurn = true;
		}
	}

	for (int i = 0; i < game->unitsNum; i++) {
		Unit *unit = &game->units[i];
		if (unit->hp <= 0) continue;
		if (unit->hasTurn) return unit;
	}

	logf("No current unit!\n");
	return NULL;
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

	loseMp(src, spell->info->mp);

	return spell;
}

bool isHidden(Unit *unit) {
	if (unit == NULL) return false;
	if (getBuff(unit, BUFF_AT_MOON)) return true;
	if (getBuff(unit, BUFF_STEALTHED)) return true;
	return false;
}

void dealDamage(Unit *src, Unit *dest, int damageAmount, bool isMagic) {
	if (!dest) Panic("dealDamage with no dest");

	float damageMulti = 1;
	float damageAddition = 0;
	float hitChance = 1;

	if (src) {
		for (int i = 0; i < src->buffsNum; i++) {
			Buff *buff = &src->buffs[i];
			if (!isMagic && buff->type == BUFF_QUAKED) damageMulti *= 0.5;
			if (!isMagic && buff->type == BUFF_PHYS_UP) damageMulti *= 2;
			if (buff->type == BUFF_ATTACK_REDUCTION) damageMulti *= 0.5;
			if (!isMagic && buff->type == BUFF_SPUN_UP) damageAddition += 100;
		}

		if (src->weapon == WEAPON_BIG_DAMAGE) damageMulti *= 2;

		if (src->ally && isMagic) {
			Unit *p1 = getUnitByType(UNIT_PLAYER1);
			if (p1->hp > 0 && p1->weapon == WEAPON_MAGIC_RESIST) damageMulti *= 0.75;
		}

		if (src->weapon == WEAPON_LOW_HP) {
			int powerLevel = 1;
			if (src->hp < src->info->maxHp*0.50) powerLevel = 2;
			if (src->hp < src->info->maxHp*0.25) powerLevel = 3;
			damageMulti *= powerLevel;
		}

		if (src->weapon == WEAPON_POISON && !isMagic) {
			loseMp(src, 10);
			giveBuff(dest, BUFF_POISON, 10);
		}
	}

	for (int i = 0; i < dest->buffsNum; i++) {
		Buff *buff = &dest->buffs[i];
		if (!isMagic && buff->type == BUFF_QUAKED) damageMulti *= 2;
		if (!isMagic && buff->type == BUFF_MARKED) damageMulti *= 2;
		if (buff->type == BUFF_DEFENSE_REDUCTION) damageMulti *= 2;
		if (buff->type == BUFF_VISUALIZE) hitChance *= 1 - 0.5; 
	}

	if (dest->weapon == WEAPON_DODGE) hitChance *= 1 - 0.1;

	if (game->spellImmNeverMisses) hitChance = 1;
	if (src && getBuff(src, BUFF_FOCUS)) hitChance = 1;
	if (!rndPerc(hitChance)) {
		damageMulti = 0;
		createEffect(EFFECT_DODGE, dest);
	}

	damageAmount += damageAddition;
	damageAmount *= damageMulti;

	if (damageAmount) {
		Buff *shield = getBuff(dest, BUFF_SHIELD);
		if (shield) {
			removeBuff(dest, shield);
			damageAmount = 0;
		}

		removeAllBuffsOfType(dest, BUFF_FAKE_SHIELD);
	}

	if (damageAmount) {
		if (dest->type == UNIT_SHIELD_SUMMONER) {
			for (int i = 0; i < game->unitsNum; i++) {
				Unit *unit = &game->units[i];
				if (unit->ally != dest->ally) continue;
				if (isHidden(unit)) continue;

				Buff *shield = getBuff(unit, BUFF_SHIELD);
				if (shield) {
					removeBuff(unit, shield);
					damageAmount = 0;
					break;
				}
			}
		}
	}

	if (src) {
		logf("%s dealt %d damage to %s\n", src->info->name, damageAmount, dest->info->name);
	} else {
		logf("%d damage dealt to %s\n", damageAmount, dest->info->name);
	}
	dest->hp -= damageAmount;
	Effect *effect = createEffect(EFFECT_DAMAGE, dest);
	effect->intValue = damageAmount;

	if (damageAmount) dest->asleep = false;

	if (src && src->weapon == WEAPON_VAMPIRE) gainHp(src, damageAmount * 0.02);

	if (getBuff(dest, BUFF_DAMAGE_SPRING)) {
		if (src) dealDamage(dest, src, damageAmount*4);
	}

	if (dest->hp <= 0 && getBuff(dest, BUFF_RELIFE)) {
		logf("Relifed\n");
		dest->hp = dest->info->maxHp;
		removeAllBuffsOfType(dest, BUFF_RELIFE);
	}

	if (dest->hp <= 0) {
		dest->buffsNum = 0;
		dest->hasTurn = false;
	}
}

void gainHp(Unit *unit, int amount) {
	if (unit->weapon == WEAPON_BIG_DAMAGE) amount = 0;
	unit->hp += amount;

	Effect *effect = createEffect(EFFECT_GAIN_HP, unit);
	effect->intValue = amount;

	int maxHp = unit->info->maxHp * (1 - unit->maxHpReductionPerc);
	if (unit->hp > maxHp) unit->hp = maxHp;
}

void gainMp(Unit *unit, int amount) {
	unit->mp += amount;
	if (unit->mp > unit->info->maxMp) unit->mp = unit->info->maxMp;

	Effect *effect = createEffect(EFFECT_GAIN_MP, unit);
	effect->intValue = amount;
}

void loseMp(Unit *unit, int amount) {
	if (amount == 0) return;
	Buff *volumeWarning = getBuff(unit, BUFF_VOLUME_WARNING);
	if (volumeWarning) {
		giveBuff(unit, BUFF_SILENCE, 3);
		removeBuff(unit, volumeWarning);
	}

	unit->mp -= amount;
	if (unit->mp < 0) unit->mp = 0;

	Effect *effect = createEffect(EFFECT_LOSE_MP, unit);
	effect->intValue = amount;
}

void refillFlasks(Unit *unit) {
	if (unit->type == UNIT_PLAYER2) {
		unit->spellsAvailableAmounts[unit->spellsAvailableNum-1] = 1;
		unit->spellsAvailableAmounts[unit->spellsAvailableNum-1] = 1;
	}
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

Effect *createEffect(EffectType type, Unit *unit) {
	if (game->effectsNum > EFFECTS_MAX-1) {
		logf("Too many effects!\n");
		game->effectsNum--;
	}

	Effect *effect = &game->effects[game->effectsNum++];
	memset(effect, 0, sizeof(Effect));
	effect->type = type;

	if (unit) {
		effect->unitId = unit->id;
		effect->position = rndVec2(-50, 50);
	}

	return effect;
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
	for (int i = 0; i < SPELLS_AVAILABLE_MAX; i++) unit->spellsAvailableAmounts[i] = -1;

	if (unit->type == UNIT_SMALL_SHIELDSTER) {
		giveBuff(unit, BUFF_SHIELD, -1);
	} else if (unit->type == UNIT_SUPER_SHIELDSTER) {
		giveBuff(unit, BUFF_SHIELD, -1);
		giveBuff(unit, BUFF_SHIELD, -1);
	} else if (unit->type == UNIT_SHIELD_SUMMONER) {
		giveBuff(unit, BUFF_SHIELD, -1);
	} else if (unit->type == UNIT_FAKE_SHIELDSTER) {
		for (int i = 0; i < 2; i++) {
			if (rndPerc(0.5)) {
				giveBuff(unit, BUFF_SHIELD, -1);
			} else {
				giveBuff(unit, BUFF_FAKE_SHIELD, -1);
			}
		}
	} else if (unit->type == UNIT_TIME_TUNER) {
		giveBuff(unit, BUFF_TIME_TUNE_CHARGE, -1);
		giveBuff(unit, BUFF_TIME_TUNE_CHARGE, -1);
		giveBuff(unit, BUFF_TIME_TUNE_CHARGE, -1);
	} else if (unit->type == UNIT_SPIKE) {
		giveBuff(unit, BUFF_DAMAGE_SPRING, -1);
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
		unit = createUnit(UNIT_SMALL_SHIELDSTER);
		NextWaveDef();
		logf("End of waves\n");
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
		logf("End of waves\n");
		EndWaveDef();
	} else if (game->level == 3) {
		StartWaveDef();
		unit = createUnit(UNIT_SMALL_SHIELDSTER);
		unit = createUnit(UNIT_SMALL_SHIELDSTER);
		unit = createUnit(UNIT_SMALL_SHIELDSTER);
		NextWaveDef();
		unit = createUnit(UNIT_SMALL_SHIELDSTER);
		unit = createUnit(UNIT_SUPER_SHIELDSTER);
		unit = createUnit(UNIT_SMALL_SHIELDSTER);
		NextWaveDef();
		unit = createUnit(UNIT_SMALL_SHIELDSTER);
		unit = createUnit(UNIT_SMALL_SHIELDSTER);
		unit = createUnit(UNIT_SMALL_SHIELDSTER);
		unit = createUnit(UNIT_SMALL_SHIELDSTER);
		unit = createUnit(UNIT_SMALL_SHIELDSTER);
		NextWaveDef();
		unit = createUnit(UNIT_SUPER_SHIELDSTER);
		unit = createUnit(UNIT_SMALL_SHIELDSTER);
		unit = createUnit(UNIT_SUPER_SHIELDSTER);
		unit = createUnit(UNIT_SMALL_SHIELDSTER);
		unit = createUnit(UNIT_SUPER_SHIELDSTER);
		NextWaveDef();
		unit = createUnit(UNIT_SUPER_SHIELDSTER);
		unit = createUnit(UNIT_SMALL_SHIELDSTER);
		unit = createUnit(UNIT_SHIELD_SUMMONER);
		unit = createUnit(UNIT_SMALL_SHIELDSTER);
		unit = createUnit(UNIT_SUPER_SHIELDSTER);
		NextWaveDef();
		logf("End of waves\n");
		EndWaveDef();
	} else if (game->level == 4) {
		auto inputRandomness = []() {
			Unit *unit = NULL;
			unit = createUnit(UNIT_SMALL_SHIELDSTER);
			unit = createUnit(UNIT_SMALL_SHIELDSTER);
			unit = createUnit(UNIT_SUPER_SHIELDSTER);
			unit = createUnit(UNIT_SMALL_SHIELDSTER);
			unit = createUnit(UNIT_SMALL_SHIELDSTER);
		};
		auto outputRandomness = []() {
			Unit *unit = NULL;
			unit = createUnit(UNIT_FAKE_SHIELDSTER);
			unit = createUnit(UNIT_SUPER_SHIELDSTER);
			unit = createUnit(UNIT_FAKE_SHIELDSTER);
		};
		auto rngManaLoss = []() {
			Unit *unit = NULL;
			unit = createUnit(UNIT_FAKE_SHIELDSTER);
			unit = createUnit(UNIT_MANA_BRUISER);
			unit = createUnit(UNIT_MANA_BRUISER);
			unit = createUnit(UNIT_FAKE_SHIELDSTER);
		};

		StartWaveDef();
		inputRandomness();
		NextWaveDef();
		outputRandomness();
		NextWaveDef();
		inputRandomness();
		NextWaveDef();
		outputRandomness();
		NextWaveDef();
		rngManaLoss();
		NextWaveDef();
		logf("End of waves\n");
		EndWaveDef();
	} else if (game->level == 5) {
		StartWaveDef();
		unit = createUnit(UNIT_ACCELERATOR);
		unit = createUnit(UNIT_ACCELERATOR);
		NextWaveDef();
		unit = createUnit(UNIT_ACCELERATOR);
		unit = createUnit(UNIT_ACCELERATOR);
		unit = createUnit(UNIT_ACCELERATOR);
		unit = createUnit(UNIT_ACCELERATOR);
		unit = createUnit(UNIT_ACCELERATOR);
		NextWaveDef();
		unit = createUnit(UNIT_SMALL_SHIELDSTER);
		unit = createUnit(UNIT_ACCELERATOR);
		unit = createUnit(UNIT_ACCELERATOR);
		unit = createUnit(UNIT_ACCELERATOR);
		NextWaveDef();
		unit = createUnit(UNIT_SMALL_SHIELDSTER);
		unit = createUnit(UNIT_ACCELERATOR);
		unit = createUnit(UNIT_ACCELERATOR);
		unit = createUnit(UNIT_ACCELERATOR);
		unit = createUnit(UNIT_SMALL_SHIELDSTER);
		NextWaveDef();
		logf("End of waves\n");
		EndWaveDef();
	} else if (game->level == 6) {
		StartWaveDef();
		unit = createUnit(UNIT_STUDENT);
		unit = createUnit(UNIT_STUDENT);
		unit = createUnit(UNIT_STUDENT);
		unit = createUnit(UNIT_STUDENT);
		unit = createUnit(UNIT_STUDENT);
		NextWaveDef();
		unit = createUnit(UNIT_TEACHER);
		unit = createUnit(UNIT_TEACHER);
		NextWaveDef();
		unit = createUnit(UNIT_TEACHER);
		unit = createUnit(UNIT_MULTIHITTER);
		unit = createUnit(UNIT_MULTIHITTER);
		unit = createUnit(UNIT_TEACHER);
		NextWaveDef();
		unit = createUnit(UNIT_TEACHER_TEACHER);
		unit = createUnit(UNIT_TEACHER);
		unit = createUnit(UNIT_STUDENT);
		NextWaveDef();
		unit = createUnit(UNIT_MULTIHITTER);
		unit = createUnit(UNIT_MULTIHITTER);
		unit = createUnit(UNIT_TEACHER_TEACHER);
		unit = createUnit(UNIT_MULTIHITTER);
		unit = createUnit(UNIT_MULTIHITTER);
		NextWaveDef();
		logf("End of waves\n");
		EndWaveDef();
	} else if (game->level == 7) {
		StartWaveDef();
		unit = createUnit(UNIT_STUNNER);
		unit = createUnit(UNIT_STUNNER);
		unit = createUnit(UNIT_STUNNER);
		unit = createUnit(UNIT_STUNNER);
		unit = createUnit(UNIT_STUNNER);
		NextWaveDef();
		unit = createUnit(UNIT_STUNNER);
		unit = createUnit(UNIT_STUNNER);
		unit = createUnit(UNIT_TIME_TUNER);
		unit = createUnit(UNIT_STUNNER);
		unit = createUnit(UNIT_STUNNER);
		NextWaveDef();
		unit = createUnit(UNIT_TEACHER_TEACHER);
		unit = createUnit(UNIT_TIME_TUNER);
		NextWaveDef();
		unit = createUnit(UNIT_SMALL_SHIELDSTER);
		unit = createUnit(UNIT_TIME_TUNER);
		unit = createUnit(UNIT_TIME_TUNER);
		unit = createUnit(UNIT_TEACHER);
		NextWaveDef();
		unit = createUnit(UNIT_TIME_TUNER);
		unit = createUnit(UNIT_TIME_TUNER);
		unit = createUnit(UNIT_TEACHER_TEACHER);
		unit = createUnit(UNIT_TIME_TUNER);
		unit = createUnit(UNIT_TIME_TUNER);
		NextWaveDef();
		logf("End of waves\n");
		EndWaveDef();
	} else if (game->level == 8) {
		StartWaveDef();
		unit = createUnit(UNIT_SILENCER);
		unit = createUnit(UNIT_LIFE_TAKER);
		unit = createUnit(UNIT_SILENCER);
		NextWaveDef();
		unit = createUnit(UNIT_SPIKE);
		unit = createUnit(UNIT_LIFE_TAKER);
		unit = createUnit(UNIT_SPIKE);
		NextWaveDef();
		unit = createUnit(UNIT_STUNNER);
		unit = createUnit(UNIT_SILENCER);
		unit = createUnit(UNIT_LIFE_TAKER);
		unit = createUnit(UNIT_SILENCER);
		unit = createUnit(UNIT_STUNNER);
		NextWaveDef();
		unit = createUnit(UNIT_SILENCER);
		unit = createUnit(UNIT_SPIKE);
		unit = createUnit(UNIT_SILENCER);
		NextWaveDef();
		unit = createUnit(UNIT_SUPER_SHIELDSTER);
		unit = createUnit(UNIT_SUPER_SHIELDSTER);
		unit = createUnit(UNIT_SHIELD_SUMMONER);
		unit = createUnit(UNIT_SUPER_SHIELDSTER);
		unit = createUnit(UNIT_SUPER_SHIELDSTER);
		NextWaveDef();
		logf("End of waves\n");
		EndWaveDef();
	} else if (game->level == 9) {
		StartWaveDef();
		unit = createUnit(UNIT_LIFE_TAKER);
		unit = createUnit(UNIT_LIFE_TAKER);
		unit = createUnit(UNIT_LIFE_TAKER);
		unit = createUnit(UNIT_LIFE_TAKER);
		unit = createUnit(UNIT_LIFE_TAKER);
		NextWaveDef();
		unit = createUnit(UNIT_LIFE_TAKER);
		unit = createUnit(UNIT_LIFE_TAKER);
		unit = createUnit(UNIT_SPIKE);
		unit = createUnit(UNIT_LIFE_TAKER);
		unit = createUnit(UNIT_LIFE_TAKER);
		NextWaveDef();
		unit = createUnit(UNIT_MANA_BRUISER);
		unit = createUnit(UNIT_TEACHER_TEACHER);
		NextWaveDef();
		unit = createUnit(UNIT_TEACHER_TEACHER);
		unit = createUnit(UNIT_TEACHER);
		unit = createUnit(UNIT_STUDENT);
		NextWaveDef();
		logf("End of waves\n");
		EndWaveDef();
	}

	for (int i = 0; i < game->unitsNum; i++) {
		Unit *unit = &game->units[i];
		unit->hasTurn = true;
		unit->extraTurns = 0;
	}

	game->wave++;
	game->prevAllyAdvantage = getAllyAdvantage();
}

void saveLoadGlobals(bool save) {
	Globals *globals = &game->globals;
	char *path = "assets/info/globals.bin";

	int version = 2;

	DataStream *stream;
	if (save) {
		stream = newDataStream();
	} else {
		stream = loadDataStream(path);
		if (!stream) return;
	}

	versionSaveLoadInt(stream, save, version, &version, 0, 999);
	versionSaveLoadVec2(stream, save, version, &globals->characterCardSize, 1, 999);

	versionSaveLoadNamedStyleStacks(stream, save, version, globals->customStyleStacks, &globals->customStyleStacksNum, CUSTOM_STYLE_STACKS_MAX, 2, 999);

	if (save) writeDataStream(path, stream);
	destroyDataStream(stream);

	if (save) logf("Done.\n");
}

void nguiShowImGuiStylesEditor(NguiNamedStyleStack *styleStacks, int *styleStacksNum, int styleStacksMax) {
	for (int i = 0; i < *styleStacksNum; i++) {
		ImGui::PushID(i);

		bool shouldSpliceStack = false;
		guiPushStyleColor(ImGuiCol_Button, 0xFF900000);
		if (ImGui::Button("X")) shouldSpliceStack = true;
		guiPopStyleColor();

		ImGui::SameLine();
		if (ImGui::ArrowButton("moveUp", ImGuiDir_Up)) {
			if (i > 0) {
				arraySwap(styleStacks, *styleStacksNum, sizeof(NguiNamedStyleStack), i, i-1);
				ImGui::PopID();
				continue;
			}
		}
		ImGui::SameLine();
		if (ImGui::ArrowButton("moveDown", ImGuiDir_Down)) {
			if (i < *styleStacksNum-1) {
				arraySwap(styleStacks, *styleStacksNum, sizeof(NguiNamedStyleStack), i, i+1);
				ImGui::PopID();
				continue;
			}
		}

		ImGui::SameLine();
		NguiNamedStyleStack *namedStyle = &styleStacks[i];
		if (ImGui::TreeNode(frameSprintf("%d: %s###styleNode%d", i, namedStyle->name, i))) {
			ImGui::InputText("Name", namedStyle->name, NGUI_NAMED_STYLE_STACK_NAME_MAX_LEN);
			nguiShowImGuiStyleEditor(&namedStyle->style);
			ImGui::TreePop();
		}

		ImGui::PopID();

		if (shouldSpliceStack) {
			arraySpliceIndex(styleStacks, *styleStacksNum, sizeof(NguiNamedStyleStack), i);
			*styleStacksNum = *styleStacksNum - 1;
			i--;
		}
	}

	if (ImGui::Button("Create custom style")) {
		if (*styleStacksNum > styleStacksMax-1) {
			logf("Too many custom style stacks!\n");
		} else {
			NguiNamedStyleStack *namedStyle = &styleStacks[*styleStacksNum];
			*styleStacksNum = *styleStacksNum + 1;
			memset(namedStyle, 0, sizeof(NguiNamedStyleStack));
		}
	}
}

void versionSaveLoadNamedStyleStacks(DataStream *stream, bool save, int version, NguiNamedStyleStack *styleStacks, int *styleStacksNum, int styleStacksMax, int minVersion, int maxVersion) {
	versionSaveLoadInt(stream, save, version, styleStacksNum, minVersion, maxVersion);
	for (int i = 0; i < *styleStacksNum; i++) {
		NguiNamedStyleStack *namedStyle = &styleStacks[i];
		if (save) { 
			writeString(stream, namedStyle->name);
			writeNguiStyleStack(stream, namedStyle->style);
		} else {
			readStringInto(stream, namedStyle->name, NGUI_NAMED_STYLE_STACK_NAME_MAX_LEN);
			namedStyle->style = readNguiStyleStack(stream);
		}
	}
}

void pushGameStyleStack(char *name) {
	Globals *globals = &game->globals;
	for (int i = 0; i < globals->customStyleStacksNum; i++) {
		NguiNamedStyleStack *namedStyle = &globals->customStyleStacks[i];
		if (streq(namedStyle->name, name)) {
			nguiPushStyleStack(&namedStyle->style);
			return;
		}
	}

	logf("Couldn't find style stack %s\n", name);
}

void popGameStyleStack(char *name) {
	Globals *globals = &game->globals;
	for (int i = 0; i < globals->customStyleStacksNum; i++) {
		NguiNamedStyleStack *namedStyle = &globals->customStyleStacks[i];
		if (streq(namedStyle->name, name)) {
			nguiPopStyleStack(&namedStyle->style);
			return;
		}
	}

	logf("Couldn't find style stack %s\n", name);
}
