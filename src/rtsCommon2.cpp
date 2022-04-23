int turnDelay = 2;
int sendEveryXFrames = 1;
int minFramesPerTurn = 6;

float maxDisconnectTime = 30;

#define PLAYERS_PER_MATCH_MAX 3
#define TEAMS_PER_MATCH_MAX 2
#define LOBBY_HASH_MAX_LEN 32

enum LobbyMode {
	LOBBY_MODE_1V1,
	LOBBY_MODE_COOP,
	LOBBY_MODE_ANY,
};
const char *lobbyModeStrings[] = {
	"1v1",
	"Coop",
	"Any",
};

struct ServerConfig {
	LobbyMode lobbyMode;
	bool isValid;
	char mapName[PATH_MAX_LEN];
};

struct LobbyMember {
	bool dead;
	int clientId;
	bool isReady;
	int slot;

	int teamNumber;
	bool inGame;
};

struct Lobby {
	char hash[LOBBY_HASH_MAX_LEN];

#define MEMBERS_PER_LOBBY_MAX 64
	LobbyMember members[MEMBERS_PER_LOBBY_MAX];
	int leaderClientId;

	ServerConfig serverConfig;

	/// Server
	int id;
	float time;
	float inGameTime;
	bool serverInGame;

	/// Client
	bool clientIsConnected;
};

enum GameCmdType {
	GAME_CMD_NONE,

	GAME_CMD_CLICK_SELECT,
	GAME_CMD_SPLICE_SELECTION,
	GAME_CMD_BOX_SELECT,

	GAME_CMD_RIGHT_CLICK,
	GAME_CMD_RIGHT_CLICK_UNIT,
	GAME_CMD_CONSTRUCT_UNIT,
	GAME_CMD_PLACE_UNIT,
	GAME_CMD_ATTACK_MOVE,

	GAME_CMD_CREATE_CONTROL_GROUP,
	GAME_CMD_ADD_TO_CONTROL_GROUP,
	GAME_CMD_SELECT_CONTROL_GROUP,

	GAME_CMD_SPLICE_ACTION,

	GAME_CMD_EXPLODE,
	GAME_CMD_GENERIC_ORDER,
	GAME_CMD_GAIN_MINERALS,
	GAME_CMD_CHANGE_GAME_SPEED,

	GAME_CMD_RESIGN,
	GAME_CMD_TIMEOUT,
	GAME_CMD_ESC,
	GAME_CMD_END_TURN,
};
const char *gameCmdTypeStrings[] = {
	"None",

	"Click select",
	"Splice selection",
	"Box select",

	"Right click",
	"Right click unit",
	"Construct unit",
	"Place unit",
	"Attack move",

	"Create control group",
	"Add to control group",
	"Select control group",

	"Splice action",

	"Explode",
	"Generic order",
	"Gain minerals",
	"Change game speed",

	"Resign",
	"Timeout",
	"Esc",
	"End turn",
};

// #define GAME_CMDS_PER_PLAYER_PER_TURN 8

// struct Turn {
// 	int turnNumber;
// 	int playerNumber;
// 	GameCmd gameCmds[GAME_CMDS_PER_TURN];
// };

#pragma pack(push, 1)
struct GameCmd {
	int clientId;
	char playerNumber;
	char type;
	int turnToExecute;

	char unitType;
	char orderType;
	Vec2 position;
	Vec2 size;
	int targetId;
	float floatValue;

	bool queues;

	char worldHash;
};

enum NetMsgType {
	NET_MSG_CtS_TCP_CONNECT=0,
	NET_MSG_CtS_WEB_CONNECT=1,
	NET_MSG_CtS_UDP_CONNECT=2,

	NET_MSG_StC_CONNECTED=3,
	NET_MSG_CtS_QUEUE=4,
	NET_MSG_StC_MATCH_START=5,

	NET_MSG_CtS_REQUEST_CONFIG=6,
	NET_MSG_StC_TRANSMIT_CONFIG=7,
	NET_MSG_CtS_TRANSMIT_CONFIG=8,

	NET_MSG_CtS_CREATE_LOBBY,
	NET_MSG_StC_LOBBY_CREATED,
	NET_MSG_CtS_JOIN_LOBBY,
	NET_MSG_StC_LOBBY_JOINED,
	NET_MSG_CtS_I_AM_READY,
	NET_MSG_StC_SOMEONE_IS_READY,
	NET_MSG_CHANGE_LOBBY_CONFIG,
	// NET_MSG_CHANGE_LOBBY_LEADER,

	NET_MSG_CtS_DISCONNECT_FROM_MATCH,

	NET_MSG_CtS_TURN_DATA,
	NET_MSG_StC_TURN_DATA,

	NET_MSG_GARBAGE,
};
struct NetMsg {
	NetMsgType type;
	int clientId;
	int matchId;

	struct CtSConnect {
	};

	struct TransmitConfig {
		ServerConfig serverConfig;
	};

	struct LobbyCreated {
		char lobbyHash[LOBBY_HASH_MAX_LEN];
	};

	struct JoinLobby {
		char lobbyHash[LOBBY_HASH_MAX_LEN];
	};

	struct LobbyJoined {
		LobbyMember member;
		int slot;
		int leaderClientId;
	};

	struct IAmReady {
		char lobbyHash[LOBBY_HASH_MAX_LEN];
		LobbyMember member;
	};

	struct SomeoneIsReady {
		LobbyMember member;
	};

	struct ChangeLobbyConfig {
		char lobbyHash[LOBBY_HASH_MAX_LEN];
		ServerConfig newServerConfig;
	};

	struct MatchStart {
		int playerNumber;
		int framesPerTurn;
		char mapName[PATH_MAX_LEN];
		bool sameIp;
		int matchTcpListenerPort;
		int matchUdpListenerPort;
	};

	struct TurnData {
		int confirmedTurn;
		int turn;
#define GAME_CMDS_MAX 32
		GameCmd gameCmds[GAME_CMDS_MAX];
		int gameCmdsNum;
	};

	union {
		CtSConnect ctsConnect;
		MatchStart matchStart;
		TurnData turnData;
		TransmitConfig transmitConfig;
		LobbyCreated lobbyCreated;
		JoinLobby joinLobby;
		LobbyJoined lobbyJoined;
		IAmReady iAmReady;
		SomeoneIsReady someoneIsReady;
		ChangeLobbyConfig changeLobbyConfig;
	};
};
#pragma pack(pop)

struct MsgPacket {
	sockaddr_storage addr;
	Socket *tcpSocket;
	void *webSocket;
	NetMsg msg;
};

void inspectGameCmds(GameCmd *gameCmds, int gameCmdsNum) {
	for (int i = 0; i < gameCmdsNum; i++) {
		GameCmd *gameCmd = &gameCmds[i];
		char *label = frameSprintf(
			"%s (p%d, turnToExecute: %d [%d])###%d",
			gameCmdTypeStrings[gameCmd->type],
			gameCmd->playerNumber,
			gameCmd->turnToExecute,
			gameCmd->worldHash,
			i
		);

		if (ImGui::TreeNode(label)) {
			char *text = frameSprintf(
				"clientId: %d\n"
				"unitType: %d\n"
				"position: %f, %f\n"
				"size: %f %f\n"
				"targetId: %d\n"
				"floatValue: %f\n"
				"queues: %d\n",
				gameCmd->clientId, 
				// unitTypeString[gameCmd->unitType],
				gameCmd->unitType,
				gameCmd->position.x, gameCmd->position.y,
				gameCmd->size.x,
				gameCmd->size.y,
				gameCmd->targetId,
				gameCmd->floatValue,
				gameCmd->queues
					);

			ImGui::Text(text);
			ImGui::TreePop();
		}
	}
}

void regMemGameCmd() {
	RegMem(GameCmd, clientId);
	RegMem(GameCmd, playerNumber);
	RegMem(GameCmd, type);
	RegMem(GameCmd, turnToExecute);
	RegMem(GameCmd, unitType);
	RegMem(GameCmd, position);
	RegMem(GameCmd, size);
	RegMem(GameCmd, targetId);
	RegMem(GameCmd, floatValue);
	RegMem(GameCmd, queues);
	RegMem(GameCmd, worldHash);
}
