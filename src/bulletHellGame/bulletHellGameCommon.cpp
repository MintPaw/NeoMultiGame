struct Input {
	bool up, down, left, right;
	bool focus, shoot, bomb, extra;
};

enum SpellType {
	SPELL_BUBBLE_SHIELD,
	SPELL_RETURN_FIRE,
	SPELL_BOSS_SKULL_EASY,
	SPELL_BOSS_SKULL_MED,
	SPELL_BOSS_SKULL_HARD,
	SPELL_BOSS_SKULL_LUNA,
	SPELL_BOSS_QUIET_EASY,
	SPELL_BOSS_QUIET_MED,
	SPELL_BOSS_QUIET_HARD,
	SPELL_BOSS_QUIET_LUNA,
	SPELL_BOSS_SPACE_EASY,
	SPELL_BOSS_SPACE_MED,
	SPELL_BOSS_SPACE_HARD,
	SPELL_BOSS_SPACE_LUNA,
	SPELL_BOSS_SPORE_EASY,
	SPELL_BOSS_SPORE_MED,
	SPELL_BOSS_SPORE_HARD,
	SPELL_BOSS_SPORE_LUNA,
	SPELL_BOSS_ZAPPY_EASY,
	SPELL_BOSS_ZAPPY_MED,
	SPELL_BOSS_ZAPPY_HARD,
	SPELL_BOSS_ZAPPY_LUNA,
	SPELL_BOSS_FLAME_EASY,
	SPELL_BOSS_FLAME_MED,
	SPELL_BOSS_FLAME_HARD,
	SPELL_BOSS_FLAME_LUNA,
	SPELL_UPGRADE_ATTACK,
	SPELL_GROW_ENEMY,
	SPELL_SHRINK_SELF,
	SPELL_SCREEN_WRAP,
	SPELL_SUMMON_ATTACK_SUN,
	SPELL_SUMMON_ATTACK_RAINBOW,
	SPELL_SUMMON_ATTACK_NARROWING,
	SPELL_SUMMON_ATTACK_MISSILE,
	SPELL_SUMMON_ATTACK_XAIMED,
	SPELL_SUMMON_ATTACK_SPOOKY,
	SPELL_SUMMON_ATTACK_SAND,
	SPELL_SUMMON_ATTACK_BLADE,
};
char *spellTypeStrings[] = {//This needs to be removed
	"error00", "error01", "error02", "error03", "error04", "error05", "error06", "error07", "error08", "error09",
	"error10", "error11", "error12", "error13", "error14", "error15", "error16", "error17", "error18", "error19",
	"error20", "error21", "error22", "error23", "error24", "error25", "error26", "error27", "error28", "error29",
	"error30", "error31", "error32", "error33", "error34", "error35", "error36", "error37", "error38", "error39",
	"error40", "error41", "error42", "error43", "error44", "error45", "error46", "error47", "error48", "error49",
	"error50", "error51", "error52", "error53", "error54", "error55", "error56", "error57", "error58", "error59",
	"error60", "error61", "error62", "error63",
};

struct ShopItem {
	SpellType effect;
#define SHOP_ITEM_NAME_MAX_LEN 64
	char name[SHOP_ITEM_NAME_MAX_LEN];
	int price;
};

struct SpellCast {
	SpellType type;
	int frame;
	int bossSpellLevel;//This needs to be removed
	int attackPatternLevel;//This needs to be removed
};

enum NetMsgType {
	NET_MSG_NONE,
	NET_MSG_PING,
	NET_MSG_PONG,

	NET_MSG_CtS_CONNECT,
	NET_MSG_StC_CONNECT,

	NET_MSG_StC_IN_QUEUE,
	NET_MSG_StC_START_GAME,

	NET_MSG_CtC_INPUTS,
	NET_MSG_CtC_SPELL_CAST,
};

struct NetMsgInputs {
#define NET_MSG_INPUTS_MAX 1
// #define NET_MSG_INPUTS_MAX 15
	int inputsTotal;
	Input inputs[NET_MSG_INPUTS_MAX];
};

struct NetMsgStartGame {
	int seed;
	int enemyClientId;
};

struct NetMsgSpellCast {
	SpellCast cast;
};

struct NetMsg {
	NetMsgType type;
	int id;

	union {
		NetMsgInputs unionInputs;
		NetMsgStartGame unionStartGame;
		NetMsgSpellCast unionSpellCast;
	};
};

int incInputHash(int hash, Input *input, int frame);
void fillUniversalShop(ShopItem *shopList, int *shopListNum);
void addSpellToUniversalShop(ShopItem *shopList, int *shopListNum, SpellType effect, char *name, int price);
/// FUNCTIONS ^

int incInputHash(int hash, Input *input, int frame) {
	hash += input->left * 1;
	hash += input->right * 1 << 1;
	hash += input->up * 1 << 2;
	hash += input->down * 1 << 3;
	hash += input->focus * 1 << 4;
	hash += input->shoot * 1 << 5;
	hash += input->bomb * 1 << 6;
	hash += input->extra * 1 << 7;
	// hash += frame * 1 << 8;
	return hash;
}

void fillUniversalShop(ShopItem *shopList, int *shopListNum) {
	addSpellToUniversalShop(shopList, shopListNum, SPELL_BUBBLE_SHIELD,           "Bubble shield",  250);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_RETURN_FIRE,             "Return fire",    100);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_BOSS_SKULL_EASY,         "Boss easy",      250);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_BOSS_SKULL_MED,          "Boss med",       250);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_BOSS_SKULL_HARD,         "Boss hard",      350);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_BOSS_SKULL_LUNA,         "Boss luna",      999);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_BOSS_QUIET_EASY,         "Boss easy",      150);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_BOSS_QUIET_MED,          "Boss med",       250);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_BOSS_QUIET_HARD,         "Boss hard",      350);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_BOSS_QUIET_LUNA,         "Boss luna",      999);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_BOSS_SPACE_EASY,         "Boss easy",      150);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_BOSS_SPACE_MED,          "Boss med",       250);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_BOSS_SPACE_HARD,         "Boss hard",      350);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_BOSS_SPACE_LUNA,         "Boss luna",      999);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_BOSS_SPORE_EASY,         "Boss easy",      150);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_BOSS_SPORE_MED,          "Boss med",       250);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_BOSS_SPORE_HARD,         "Boss hard",      350);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_BOSS_SPORE_LUNA,         "Boss luna",      999);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_BOSS_ZAPPY_EASY,         "Boss easy",      150);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_BOSS_ZAPPY_MED,          "Boss med",       250);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_BOSS_ZAPPY_HARD,         "Boss hard",      350);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_BOSS_ZAPPY_LUNA,         "Boss luna",      999);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_BOSS_FLAME_EASY,         "Boss easy",      150);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_BOSS_FLAME_MED,          "Boss med",       250);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_BOSS_FLAME_HARD,         "Boss hard",      350);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_BOSS_FLAME_LUNA,         "Boss luna",      999);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_UPGRADE_ATTACK,          "Upgrade attack", 200);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_GROW_ENEMY,              "Grow enemy",     300);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_SHRINK_SELF,             "Shrink self",    300);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_SCREEN_WRAP,             "Screen wrap",    250);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_SUMMON_ATTACK_SUN,       "Send pattern",   100);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_SUMMON_ATTACK_RAINBOW,   "Send pattern",   100);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_SUMMON_ATTACK_NARROWING, "Send pattern",   100);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_SUMMON_ATTACK_MISSILE,   "Send pattern",   100);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_SUMMON_ATTACK_XAIMED,    "Send pattern",   100);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_SUMMON_ATTACK_SPOOKY,    "Send pattern",   100);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_SUMMON_ATTACK_SAND,      "Send pattern",   100);
	addSpellToUniversalShop(shopList, shopListNum, SPELL_SUMMON_ATTACK_BLADE,     "Send pattern",   100);
}

void addSpellToUniversalShop(ShopItem *shopList, int *shopListNum, SpellType effect, char *name, int price) {
	shopList[effect].effect = effect;
	strcpy(shopList[effect].name, name);
	shopList[effect].price = price;

	*shopListNum = max(*shopListNum, effect+1);
}
