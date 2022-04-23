#include "rtsCommon.cpp"

struct MatchLogger;
struct MatchHandler;

struct Match;

struct PlayerClient {
	volatile void *webSocket;
	Socket *tcpSocket;
	ThreadSafeQueue *incomingPackets;
	ThreadSafeQueue *outgoingPackets;

	int id;
	sockaddr_storage udpAddr;
	bool supportsUdp;
	Match *match;
	Lobby *lobby;
	float timeSinceLastSeen;
};

struct MatchPlayer {
	int playerClientId;
	ThreadSafeQueue *incomingPackets;
	ThreadSafeQueue *outgoingPackets;

	bool supportsUdp;
	volatile void *webSocket;
	Socket *tcpSocket;
	sockaddr_storage udpAddr;
	float timeSinceLastSeen;
	int playerNumber;
};

struct Match {
	int id;
	bool isDesynced;
	bool sameIp;
	ThreadSafeQueue *_webSocketPacketsIn;

#if defined(__linux__)
	mg_mgr mongooseManager;
	mg_connection *mongooseListener;
	mg_serve_http_opts mongooseSockOpts;
#endif

	Thread *tcpThread;
	Thread *webThread;
	Socket *udpListener;
	Socket *tcpListener;
	volatile int _webServerFreePort;

	Socket *_tcpSockets[PLAYERS_PER_MATCH_MAX];
	volatile int _tcpSocketNum;

	void *_webSockets[PLAYERS_PER_MATCH_MAX];
	int _webSocketsNum;

#define DEBUG_GAME_CMD_HISTORY_MAX 256
	GameCmd *gameCmdHistory;
	int gameCmdHistoryNum;
	int framesPerTurn;

	MatchPlayer players[PLAYERS_PER_MATCH_MAX];
	int playersNum;

#define GAME_CMDS_TO_HOLD_MAX 128
	GameCmd gameCmds[GAME_CMDS_TO_HOLD_MAX];
	int gameCmdsNum;

	int gotUpToTurn[PLAYERS_PER_MATCH_MAX];
	int confirmedUpToTurn[PLAYERS_PER_MATCH_MAX];

	int ticks;
	bool shouldEnd;

	NanoTime lastNanoTime;
};

enum ServerLogLevel {
	VERBOSE,
	INFO,
	WARNING,
	LERROR,
	PANIC,
};
const char serverLogLevelStrings[] = {
	'V',
	'I',
	'W',
	'E',
	'P',
};

struct Server {
	ThreadSafeQueue *_outgoingPackets;
	ThreadSafeQueue *_incomingPackets;

	Socket *_tcpSocket;
	Socket *_udpSocket;
	volatile int _port;

#if defined(__linux__)
	mg_mgr mongooseManager;
	mg_connection *mongooseListener;
	mg_serve_http_opts mongooseSockOpts;
#endif

	ThreadSafeQueue *_webSocketPacketsIn;
	ThreadSafeQueue *_webSocketPacketsOut;

#define PLAYERS_MAX 16
	PlayerClient playerClients[PLAYERS_MAX];
	int playerClientsNum;
	int nextPlayerClientId;

	int waitingPlayerClients[PLAYERS_MAX];
	int waitingPlayerClientsNum;

	int gameCmdHistoryMax;

#define MATCHES_MAX 8
	Match matches[MATCHES_MAX];
	int matchesNum;
	int nextMatchId;

	ServerConfig serverConfig;

#define LOBBIES_MAX 16
	Lobby lobbies[LOBBIES_MAX];
	int lobbiesNum;
	int nextLobbyId;
	int ticks;

#define SERVER_LOG_BUFFER_MAX_LEN Megabytes(16)
	char serverLogBuffer[SERVER_LOG_BUFFER_MAX_LEN];
	int serverLogBufferNum;
	int serverLogsSaved;

	volatile bool _shouldReSaveLog;
	NanoTime lastNanoTime;

	volatile u32 _serverLogfMutex;
};

#define serverLogf(...) realServerLogf(__FILE__, __LINE__, __VA_ARGS__)
void realServerLogf(char *file, int lineNum, ServerLogLevel level, const char *msg, ...);
void serverLogLastOsErrorf(ServerLogLevel level, const char *msg, ...);

void runGame();
void updateGame();
bool updateMatch(Match *match, float elapsed);
void updateUdpThread(void *threadStruct);
void updateTcpListenerThread(void *threadStruct);
void updateActualServer(void *threadStruct);

#if defined(__linux__)
void mongooseLoginHandler(mg_connection *connection, int event, void *eventData);
#endif

bool startMatch(int *clientIds, int clientIdsNum);
Lobby *getLobby(char *hash);

void updateMatchTcpListener(void *threadStruct);
// void updateMatchWebSocketListener(void *threadStruct);

PlayerClient *getPlayerClient(int id);
void saveLogFile();
void serverCleanup();
// FUNCTIONS ^

Server *server = NULL;

void runGame() {
	if (directoryExists("C:/Dropbox")) strcpy(projectAssetDir, "C:/Dropbox/rtsGame/rtsServerAssets");
	if (directoryExists("C:/Users/i am the sun king/Dropbox")) strcpy(projectAssetDir, "C:/Users/i am the sun king/Dropbox/rtsGame/rtsServerAssets");
	if (directoryExists("/media/sf_Dropbox")) strcpy(projectAssetDir, "/media/sf_Dropbox/rtsGame/rtsServerAssets");

#if defined(FALLOW_COMMAND_LINE_ONLY)
	strcpy(projectAssetDir, "/home/ubuntu/runPackage/rtsServerAssets");
#endif

	initFileOperations();

	initPlatform(1024, 512, "rtsServer");
	platform->sleepWait = true;
#if !defined(FALLOW_COMMAND_LINE_ONLY)
	initAudio();
	initRenderer(1024, 512);
#endif

	bool good = initNetworking();
	if (!good) Panic("Failed to init networking");

	platformUpdateLoop(updateGame);
}

void updateGame() {
	if (!server) {
		server = (Server *)zalloc(sizeof(Server));
		server->gameCmdHistoryMax = 10 * 60 * 60 / minFramesPerTurn * GAME_CMDS_MAX;

		{
			ServerConfig *config = &server->serverConfig;
			strcpy(config->mapName, "assets/maps/smallTest");
			// if (platform->isInternalVersion) strcpy(config->mapName, "assets/maps/CombatTest");
			config->isValid = true;
		}

		regMemGameCmd();

		server->_incomingPackets = createThreadSafeQueue(sizeof(MsgPacket), 256, false);
		server->_outgoingPackets = createThreadSafeQueue(sizeof(MsgPacket), 256, false);
		server->_webSocketPacketsIn = createThreadSafeQueue(sizeof(MsgPacket), 256, false);
		server->_webSocketPacketsOut = createThreadSafeQueue(sizeof(MsgPacket), 256, false);

		server->_port = 54029;

		WriteFence();
		ReadFence();

		createThread(updateUdpThread);
		// createThread(updateTcpListenerThread);

		signal(SIGINT, platformPreventCtrlC);
		if (platform->isCommandLineOnly) {
		} else {
			minimizeWindow();
		}
		platform->atExitCallback = serverCleanup;

#if defined(__linux__)
		mg_mgr_init(&server->mongooseManager, NULL);

		const char *mongoosePort = mallocSprintf("%d", server->_port+4);
		server->mongooseListener = mg_bind(&server->mongooseManager, mongoosePort, mongooseLoginHandler);
		mg_set_protocol_http_websocket(server->mongooseListener);

		server->mongooseSockOpts.document_root = ".";
		server->mongooseSockOpts.enable_directory_listing = "false";
#endif


		createThread(updateActualServer);

		Lobby *lobby = &server->lobbies[server->lobbiesNum++];
		memset(lobby, 0, sizeof(Lobby));
		lobby->id = ++server->nextLobbyId;
		snprintf(lobby->hash, LOBBY_HASH_MAX_LEN, "%d", lobby->id);
	}

	if (!platform->isCommandLineOnly) {
		clearRenderer();

		ImGui::SetNextWindowSize(ImVec2(renderer->width, renderer->height), ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::Begin("Server", NULL, ImGuiWindowFlags_AlwaysAutoResize);

		ImGui::BeginChild("scrolling", ImVec2(0, 256), false, ImGuiWindowFlags_HorizontalScrollbar);
		for (int i = 0; i < LOGF_BUFFERS_MAX; i++) {
			LogfBuffer *logBuf = &platform->logs[LOGF_BUFFERS_MAX-1 - i];
			if (!logBuf->buffer[0]) continue;
			ImGui::Text(logBuf->buffer);
		}
		ImGui::EndChild();

		if (ImGui::TreeNode("Matches")) {
			for (int i = 0; i < server->matchesNum; i++) {
				Match *match = &server->matches[i];
				if (ImGui::TreeNode(frameSprintf("Match id %d\n", match->id))) {
					for (int i = 0; i < match->playersNum; i++) {
						ImGui::Text("Got up to turn %d from player %d", match->gotUpToTurn[i], i);
						ImGui::Text("Confirmed up to turn %d from player %d", match->confirmedUpToTurn[i], i);
					}

					ImGui::Separator();

					if (ImGui::TreeNode("GameCmdHistory")) {
						inspectGameCmds(match->gameCmdHistory, match->gameCmdHistoryNum);
						ImGui::TreePop();
					}

					ImGui::Separator();
					ImGui::Text("Active:");
					inspectGameCmds(match->gameCmds, match->gameCmdsNum);
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Player clients")) {
			for (int i = 0; i < server->playerClientsNum; i++) {
				PlayerClient *client = &server->playerClients[i];
				if (ImGui::TreeNode(frameSprintf("Client id %d\n", client->id))) {
					bool isWaiting = false;
					for (int i = 0; i < server->waitingPlayerClientsNum; i++) {
						PlayerClient *waitingClient = getPlayerClient(server->waitingPlayerClients[i]);
						if (client == waitingClient) isWaiting = true;
					}

					if (isWaiting) ImGui::Text("Is waiting");
					if (client->match) ImGui::Text("In match");
					ImGui::Text("timeSinceLastSeen: %f\n", client->timeSinceLastSeen);
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}

		ImGui::End();

		guiDraw();

		Rect mouseRect = makeRect(0, 0, 4, 4);
		mouseRect.x = platform->mouse.x - mouseRect.width/2;
		mouseRect.y = platform->mouse.y - mouseRect.width/2;
		drawRect(mouseRect, 0xFFFF0000);
	}
}

#if defined(__linux__)
void mongooseLoginHandler(mg_connection *connection, int event, void *eventData) {
	if (event == MG_EV_WEBSOCKET_HANDSHAKE_DONE) {
		MsgPacket packet = {};
		packet.msg.type = NET_MSG_CtS_WEB_CONNECT;
		packet.webSocket = connection;

		bool good = threadSafeQueuePush(server->_incomingPackets, &packet);
		if (!good) serverLogf(LERROR, "Incoming queue is overloaded\n");
	} else if (event == MG_EV_WEBSOCKET_FRAME) {
		websocket_message *wm = (websocket_message *)eventData;
		if (wm->size != sizeof(NetMsg)) {
			serverLogf(WARNING, "Weird size %d instead of %d\n", wm->size, sizeof(NetMsg));
			return;
		}

		MsgPacket packet = {};
		memcpy(&packet.msg, wm->data, sizeof(NetMsg));
		packet.webSocket = connection;
		bool good = threadSafeQueuePush(server->_webSocketPacketsIn, &packet);
		if (good) {
			// serverLogf(INFO, "Server got %d\n", packet.msg.type);
		} else {
			MsgPacket tempPacket;
			threadSafeQueueShift(server->_webSocketPacketsIn, &tempPacket);
			good = threadSafeQueuePush(server->_webSocketPacketsIn, &packet);
			if (!good) serverLogf(LERROR, "Inifintely full _webSocketPacketsIn?\n");
		}

	} else if (event == MG_EV_HTTP_REQUEST) {
		serverLogf(INFO, "Got http request\n");
		mg_serve_http(connection, (http_message *)eventData, server->mongooseSockOpts);
	} else if (event == MG_EV_CLOSE) {
		serverLogf(INFO, "Got close\n");
		/* Disconnect. Tell everybody. */
		if (connection->flags & MG_F_IS_WEBSOCKET) {
			// broadcast(connection, mg_mk_str("-- left"));
		}
	}
}
#endif

void updateActualServer(void *threadStruct) {
	for (;;) {
		NanoTime now = getNanoTime();
		float elapsed = (float)getMsPassed(&server->lastNanoTime, &now)/1000.0;
		server->lastNanoTime = now;
		if (elapsed > 1 || elapsed < 0) elapsed = 0;

		for (;;) {
			MsgPacket packet;
			bool good = threadSafeQueueShift(server->_incomingPackets, &packet);
			if (!good) break;
			NetMsg *msg = &packet.msg;

			if (msg->type == NET_MSG_CtS_TCP_CONNECT || msg->type == NET_MSG_CtS_UDP_CONNECT || msg->type == NET_MSG_CtS_WEB_CONNECT) {
				// serverLogf(INFO, "Player logged\n");
				PlayerClient *newPlayerClient = NULL;
				{
					if (server->playerClientsNum > PLAYERS_MAX-1) {
						serverLogf(WARNING, "Too many playerClients %d/%d\n", server->playerClientsNum, PLAYERS_MAX);
						continue;
					}

					newPlayerClient = &server->playerClients[server->playerClientsNum++];
					memset(newPlayerClient, 0, sizeof(PlayerClient));
					newPlayerClient->id = ++server->nextPlayerClientId;
					newPlayerClient->incomingPackets = createThreadSafeQueue(sizeof(MsgPacket), 256, false);
					newPlayerClient->outgoingPackets = createThreadSafeQueue(sizeof(MsgPacket), 256, false);
				}

				if (msg->type == NET_MSG_CtS_WEB_CONNECT) {
					newPlayerClient->supportsUdp = false;
					newPlayerClient->webSocket = packet.webSocket;
				} else if (msg->type == NET_MSG_CtS_TCP_CONNECT) {
					newPlayerClient->supportsUdp = false;
					newPlayerClient->tcpSocket = packet.tcpSocket;
					serverLogf(INFO, "Got connect from %s:%d\n", getSocketIp(newPlayerClient->tcpSocket), getSocketPort(newPlayerClient->tcpSocket));
				} else {
					newPlayerClient->supportsUdp = true;
					newPlayerClient->udpAddr = packet.addr;
				}

				{ /// Garbage
					MsgPacket packet = {};
					packet.msg.type = NET_MSG_GARBAGE;
					bool good = threadSafeQueuePush(newPlayerClient->outgoingPackets, &packet);
					if (!good) serverLogf(LERROR, "client outgoingPackets is full [%d]\n", packet.msg.type);
				}
				{ /// Connected message
					MsgPacket packet = {};
					packet.msg.type = NET_MSG_StC_CONNECTED;
					packet.msg.clientId = newPlayerClient->id;
					serverLogf(INFO, "Giving new player id %d\n", newPlayerClient->id);
					bool good = threadSafeQueuePush(newPlayerClient->outgoingPackets, &packet);
					if (!good) serverLogf(LERROR, "client outgoingPackets is full [%d]\n", packet.msg.type);
				}

				// {
				// 	char ip[17] = {};
				// 	int port = 0;
				// 	getSockaddrIpAndPort(&newPlayerClient->udpAddr, ip, &port);
				// 	serverLogf(INFO, "New player connected, giving id: %d %s:%d\n", newPlayerClient->id, ip, port);
				// }
			} else {
				PlayerClient *client = getPlayerClient(msg->clientId);

				if (!client) {
					serverLogf(WARNING, "An unknown client is requesting a message %d\n", msg->type);
					continue;
				}

				bool good = threadSafeQueuePush(client->incomingPackets, &packet);
				if (!good) serverLogf(LERROR, "client outgoingPackets is full [%d]\n", packet.msg.type);
			}
		}

		if (server->_webSocketPacketsIn) {
			for (;;) {
				MsgPacket packet;
				bool good = threadSafeQueueShift(server->_webSocketPacketsIn, &packet);
				if (!good) break;
				bool processed = false;

				for (int i = 0; i < server->playerClientsNum; i++) {
					PlayerClient *client = &server->playerClients[i];
					if (client->webSocket == packet.webSocket) {
						processed = true;
						bool good = threadSafeQueuePush(client->incomingPackets, &packet);
						if (!good) serverLogf(LERROR, "client outgoingPackets is full webpush [%d]\n", packet.msg.type);
						break;
					}
				}

				if (!processed) serverLogf(INFO, "Unprocessed message\n");
			}
		}

#if defined(__linux__)
		for (;;) {
			if (mg_mgr_poll(&server->mongooseManager, 0) == 0) break;
		}
#endif


		for (int i = 0; i < server->playerClientsNum; i++) {
			PlayerClient *client = &server->playerClients[i];
			if (!client) {
				serverLogf(WARNING, "An unknown client is sending a message\n");
				continue;
			}

			for (;;) {
				MsgPacket packet;
				bool good = threadSafeQueueShift(client->incomingPackets, &packet);
				if (!good) break;
				// serverLogf(INFO, "Got login packet from %d\n", client->id);
				client->timeSinceLastSeen = 0;
				NetMsg *msg = &packet.msg;

				if (msg->type == NET_MSG_CtS_REQUEST_CONFIG) {
					serverLogf(INFO, "Client %d is requesting the config\n", client->id);

					MsgPacket packet = {};
					packet.msg.type = NET_MSG_StC_TRANSMIT_CONFIG;
					packet.msg.transmitConfig.serverConfig = server->serverConfig;
					bool good = threadSafeQueuePush(client->outgoingPackets, &packet);
					if (!good) serverLogf(LERROR, "client outgoingPackets is full [%d]\n", packet.msg.type);
				} else if (msg->type == NET_MSG_CtS_TRANSMIT_CONFIG) {
					server->serverConfig = msg->transmitConfig.serverConfig;
					serverLogf(INFO, "New server config loaded, map: %s\n", server->serverConfig.mapName);
				} else if (msg->type == NET_MSG_CtS_QUEUE) {
					serverLogf(INFO, "Client %d is queuing...\n", client->id);

					server->waitingPlayerClients[server->waitingPlayerClientsNum++] = client->id;
				} else if (msg->type == NET_MSG_CtS_CREATE_LOBBY) {
					if (server->lobbiesNum < LOBBIES_MAX-1) {
						Lobby *lobby = &server->lobbies[server->lobbiesNum++];
						memset(lobby, 0, sizeof(Lobby));
						lobby->id = ++server->nextLobbyId;

						strcpy(lobby->serverConfig.mapName, "assets/map/smallTest");
						lobby->serverConfig.isValid = true;

						snprintf(lobby->hash, LOBBY_HASH_MAX_LEN, "%d", lobby->id);

						MsgPacket packet = {};
						packet.msg.type = NET_MSG_StC_LOBBY_CREATED;
						strcpy(packet.msg.lobbyCreated.lobbyHash, lobby->hash);
						if (!threadSafeQueuePush(client->outgoingPackets, &packet)) serverLogf(LERROR, "client outgoingPackets is full [%d]\n", packet.msg.type);
					} else {
						serverLogf(WARNING, "//@incomplete Too many lobbies");
						continue;
					}
				} else if (msg->type == NET_MSG_CtS_JOIN_LOBBY) {
					Lobby *lobby = getLobby(msg->joinLobby.lobbyHash);
					if (!lobby) {
						serverLogf(WARNING, "//@incomplete Unknown lobby (%s)", msg->joinLobby.lobbyHash);
						continue;
					}

					int slot = -1;
					for (int i = 0; i < MEMBERS_PER_LOBBY_MAX; i++) {
						if (lobby->members[i].clientId == 0) {
							slot = i;
							break;
						}
					}

					if (slot == -1) {
						serverLogf(WARNING, "//@incomplete Lobby full");
						continue;
					}

					int livingMembers = 0;
					for (int i = 0; i < MEMBERS_PER_LOBBY_MAX; i++) {
						LobbyMember *member = &lobby->members[i];
						if (member->clientId != 0 && !member->dead) livingMembers++;
					}

					LobbyMember *newMember = &lobby->members[slot];
					newMember->slot = slot;
					newMember->clientId = client->id;
					newMember->teamNumber = livingMembers == 0 ? 0 : 1;
					client->lobby = lobby;

					for (int i = 0; i < MEMBERS_PER_LOBBY_MAX; i++) {
						LobbyMember *member = &lobby->members[i];
						PlayerClient *existingClient = getPlayerClient(member->clientId);
						if (!existingClient) continue;

						if (livingMembers == 0) {
							lobby->leaderClientId = client->id;
							serverLogf(INFO, "Lobby leader is %d\n", lobby->leaderClientId);
						}

						/// Tell each other client that there's a new client
						MsgPacket packet = {};
						packet.msg.type = NET_MSG_StC_LOBBY_JOINED;
						packet.msg.lobbyJoined.leaderClientId = lobby->leaderClientId;
						memcpy(&packet.msg.lobbyJoined.member, newMember, sizeof(LobbyMember));
						if (!threadSafeQueuePush(existingClient->outgoingPackets, &packet)) serverLogf(LERROR, "existingClient outgoingPackets is full [%d]\n", packet.msg.type);

						/// Tell the new client about the existing clients
						if (existingClient->id != client->id) {
							MsgPacket packet = {};
							packet.msg.type = NET_MSG_StC_LOBBY_JOINED;
							memcpy(&packet.msg.lobbyJoined.member, member, sizeof(LobbyMember));
							if (!threadSafeQueuePush(client->outgoingPackets, &packet)) serverLogf(LERROR, "client outgoingPackets is full [%d]\n", packet.msg.type);
						}
					}
				} else if (msg->type == NET_MSG_CtS_I_AM_READY) {
					Lobby *lobby = getLobby(msg->iAmReady.lobbyHash);

					if (!lobby) {
						serverLogf(INFO, "Unknown lobby\n");
						continue;
					}

					LobbyMember *updatedMember = NULL;
					for (int i = 0; i < MEMBERS_PER_LOBBY_MAX; i++) {
						LobbyMember *member = &lobby->members[i];
						if (member->clientId == client->id) {
							memcpy(member, &msg->iAmReady.member, sizeof(LobbyMember));
							updatedMember = member;
							break;
						}
					}
				} else if (msg->type == NET_MSG_CHANGE_LOBBY_CONFIG) {
					Lobby *lobby = getLobby(msg->changeLobbyConfig.lobbyHash);

					if (!lobby) {
						serverLogf(INFO, "Unknown lobby\n");
						continue;
					}

					memcpy(&lobby->serverConfig, &msg->changeLobbyConfig.newServerConfig, sizeof(ServerConfig));
				}
			}

			if (!client->supportsUdp && client->tcpSocket) {
				for (;;) {
					SocketlingStatus status = socketReceive(client->tcpSocket);
					if (status == SOCKETLING_ZERO_RECEIVE) {
						// Kicked?
						serverLogf(INFO, "Kick\n");
					} else if (status == SOCKETLING_CONNECTION_RESET) {
						// Kicked?
						serverLogf(INFO, "Reset\n");
					} else if (status == SOCKETLING_WOULD_BLOCK) {
						// Nothing...
					} else if (status == SOCKETLING_PENDING) {
						// Nothing...
					} else if (status == SOCKETLING_ERROR) {
						logLastOSError();
					} else if (status == SOCKETLING_GOOD) {
						serverLogf(INFO, "Got\n");
						MsgPacket packet = {};
						memcpy(&packet.msg, client->tcpSocket->receiveBuffer, sizeof(NetMsg));

						if (!threadSafeQueuePush(client->incomingPackets, &packet)) {
							serverLogf(LERROR, "Client incomingPackets is full (on repush) [%d]\n", packet.msg.type);
						}
					} else {
						serverLogf(LERROR, "Unknown socketling status %d\n", status);
					}

					if (status != SOCKETLING_GOOD) break;
				}
			}

			for (;;) {
				MsgPacket packet;
				bool good = threadSafeQueueShift(client->outgoingPackets, &packet);
				if (!good) break;

				if (client->supportsUdp) {
					// serverLogf(INFO, "Sending udp %d\n", packet.msg.type);
					packet.addr = client->udpAddr;
					bool good = threadSafeQueuePush(server->_outgoingPackets, &packet);
					if (!good) serverLogf(LERROR, "Server outgoing packets overloaded\n");
				} else if (client->tcpSocket) {
					SocketlingStatus status = socketSend(client->tcpSocket, &packet.msg);
					if (status == SOCKETLING_WOULD_BLOCK || status == SOCKETLING_PENDING) {
						if (!threadSafeQueuePush(client->outgoingPackets, &packet)) {
							serverLogf(LERROR, "Client outgoingPackets is full (on repush) [%d]\n", packet.msg.type);
						}
					} else if (status == SOCKETLING_ERROR) {
						logLastOSError();
					} else if (status == SOCKETLING_GOOD) {
						serverLogf(INFO, "Sent to %s:%d\n", getPeerIp(client->tcpSocket), getPeerPort(client->tcpSocket));
					} else {
						serverLogf(INFO, "Unknown socketling status %d\n", status);
					}
				} else if (client->webSocket) {
#if defined(__linux__)
					serverLogf(INFO, "Sending loging web %d\n", packet.msg.type);
					mg_send_websocket_frame((mg_connection *)client->webSocket, WEBSOCKET_OP_BINARY, &packet.msg, sizeof(NetMsg));
#endif
				}
			}
		}

		for (int i = 0; i < server->lobbiesNum; i++) {
			Lobby *lobby = &server->lobbies[i];

			int livingMembers = 0;
			int readyMembers = 0;
			int inGameMembers = 0;
			for (int i = 0; i < MEMBERS_PER_LOBBY_MAX; i++) {
				LobbyMember *member = &lobby->members[i];
				if (member->clientId == 0) continue;

				if (member->dead) continue;
				if (!member->dead) livingMembers++;

				if (member->isReady) readyMembers++;
				if (member->inGame) inGameMembers++;

				PlayerClient *client = getPlayerClient(member->clientId);
				if (!client) {
					member->dead = true;
					continue;

					// memset(member, 0, sizeof(LobbyMember));
					// i--;
					// continue;
				}

				if (server->ticks % 16 == 0) {
					if (server->ticks % 32 == 0 && client->id != lobby->leaderClientId) {
						MsgPacket packet = {};
						packet.msg.type = NET_MSG_CHANGE_LOBBY_CONFIG;
						memcpy(&packet.msg.changeLobbyConfig.newServerConfig, &lobby->serverConfig, sizeof(ServerConfig));
						if (!threadSafeQueuePush(client->outgoingPackets, &packet)) serverLogf(LERROR, "client outgoingPackets is full [%d]\n", packet.msg.type);
					} else {
						for (int i = 0; i < MEMBERS_PER_LOBBY_MAX; i++) {
							LobbyMember *otherMember = &lobby->members[i];
							if (otherMember->clientId == 0) continue;
							if (otherMember->clientId == member->clientId) continue;

							MsgPacket packet = {};
							packet.msg.type = NET_MSG_StC_SOMEONE_IS_READY;
							memcpy(&packet.msg.someoneIsReady.member, otherMember, sizeof(LobbyMember));
							if (!threadSafeQueuePush(client->outgoingPackets, &packet)) serverLogf(LERROR, "client outgoingPackets is full [%d]\n", packet.msg.type);
						}
					}
				}
			}

			if (readyMembers == livingMembers && livingMembers && !lobby->serverInGame) {
				int *clientIds = (int *)frameMalloc(sizeof(int) * MEMBERS_PER_LOBBY_MAX);
				int clientIdsNum = 0;
				for (int i = 0; i < MEMBERS_PER_LOBBY_MAX; i++) {
					LobbyMember *member = &lobby->members[i];
					if (member->clientId == 0) continue;
					clientIds[clientIdsNum++] = member->clientId;
				}

				if (startMatch(clientIds, clientIdsNum)) {
					lobby->serverInGame = true;
					lobby->inGameTime = 0;
				} else {
					serverLogf(LERROR, "Failed to start match\n");
				}
			}

			if (lobby->serverInGame) {
				lobby->inGameTime += elapsed;
				if (lobby->inGameTime > 30) {
					bool matchIsOver = true;
					for (int i = 0; i < MEMBERS_PER_LOBBY_MAX; i++) {
						LobbyMember *member = &lobby->members[i];
						if (member->clientId == 0) continue;
						if (member->inGame) {
							matchIsOver = false;
							break;
						}
					}

					if (matchIsOver) lobby->serverInGame = false;
				}
			}

			if (lobby->id != 1 && livingMembers <= 0 && lobby->time > maxDisconnectTime) {
				serverLogf(INFO, "Lobby %d was destroyed (%d/%d)\n", lobby->id, server->lobbiesNum, LOBBIES_MAX);
				arraySpliceIndex(server->lobbies, LOBBIES_MAX, sizeof(Lobby), i);
				server->lobbiesNum--;
				i--;
				continue;
			}

			lobby->time += elapsed;
		}

		for (int i = 0; i < server->matchesNum; i++) {
			bool matchDone = updateMatch(&server->matches[i], elapsed);
			if (matchDone) {
				serverLogf(INFO, "Match %d is being destroyed because it has no clients (%d/%d matches)\n", server->matches[i].id, server->matchesNum, MATCHES_MAX);
				arraySpliceIndex(server->matches, MATCHES_MAX, sizeof(Match), i);
				server->matchesNum--;
				i--;
				continue;
			}
		}

		if (server->waitingPlayerClientsNum >= 2) {
			//@todo Make this so it matches the first players to connect, not the last ones
			PlayerClient *client0 = getPlayerClient(server->waitingPlayerClients[server->waitingPlayerClientsNum-1]);
			server->waitingPlayerClientsNum--;
			PlayerClient *client1 = getPlayerClient(server->waitingPlayerClients[server->waitingPlayerClientsNum-1]);
			server->waitingPlayerClientsNum--;

			int clientIds[] = {client0->id, client1->id};
			int clientIdsNum = ArrayLength(clientIds);
			bool good = startMatch(clientIds, clientIdsNum);
			if (!good) serverLogf(LERROR, "Failed to start match\n");
		}

		{ /// Splice dead clients
			for (int i = 0; i < server->playerClientsNum; i++) {
				PlayerClient *client = &server->playerClients[i];
				client->timeSinceLastSeen += elapsed;
				// serverLogf(INFO, "Client %d last seen %f\n", client->id, client->timeSinceLastSeen);
				if (client->timeSinceLastSeen >= maxDisconnectTime) {
					for (int i = 0; i < server->waitingPlayerClientsNum; i++) {
						if (client->id == server->waitingPlayerClients[i]) {
							serverLogf(INFO, "Client %d removed from queue\n", client->id);
							arraySpliceIndex(server->waitingPlayerClients, PLAYERS_MAX, sizeof(int), i);
							server->waitingPlayerClientsNum--;
						}
					}

					serverLogf(INFO, "Client %d timed out (%.1fsec) (%d/%d)\n", client->id, client->timeSinceLastSeen, server->playerClientsNum, PLAYERS_MAX);
					destroyThreadSafeQueue(client->incomingPackets);
					destroyThreadSafeQueue(client->outgoingPackets);
					arraySpliceIndex(server->playerClients, PLAYERS_MAX, sizeof(PlayerClient), i);
					server->playerClientsNum--;
					i--;
					continue;
				}
			}
		}

		if (server->_shouldReSaveLog) {
			server->_shouldReSaveLog = false;
			writeFile("assets/log", server->serverLogBuffer, server->serverLogBufferNum);
		}

		if ((float)server->serverLogBufferNum / (float)SERVER_LOG_BUFFER_MAX_LEN >= 0.75) saveLogFile();

		server->ticks++;
		platformSleep(16);
	}
}

Lobby *getLobby(char *hash) {
	for (int i = 0; i < server->lobbiesNum; i++) {
		if (streq(server->lobbies[i].hash, hash)) {
			return &server->lobbies[i];
		}
	}
	return NULL;
}

bool startMatch(int *clientIds, int clientIdsNum) {
	if (server->matchesNum < MATCHES_MAX) {
		serverLogf(INFO, "A match is starting...\n");

		Match *match = &server->matches[server->matchesNum++];
		memset(match, 0, sizeof(Match));
		match->id = ++server->nextMatchId;
		match->gotUpToTurn[0] = -1;
		match->gotUpToTurn[1] = -1;
		match->framesPerTurn = 6;
		match->gameCmdHistory = (GameCmd *)zalloc(server->gameCmdHistoryMax * sizeof(GameCmd));

		match->sameIp = true;

		u32 lastIp = -1;
		for (int i = 0; i < clientIdsNum; i++) {
			PlayerClient *client = getPlayerClient(clientIds[i]);

			if (lastIp != -1) {
				if (lastIp != getIp(client->udpAddr)) match->sameIp = false;
			}

			lastIp = getIp(client->udpAddr);

			if (!client) {
				serverLogf(LERROR, "A client that was in queue in gone now\n");
				return false;
			}
			client->match = match;
			if (client->webSocket) match->framesPerTurn = 12;
			MatchPlayer *player = &match->players[match->playersNum++];
			memset(player, 0, sizeof(MatchPlayer));
			player->incomingPackets = createThreadSafeQueue(sizeof(MsgPacket), 256, false);
			player->outgoingPackets = createThreadSafeQueue(sizeof(MsgPacket), 256, false);
			player->playerClientId = client->id;
			player->supportsUdp = client->supportsUdp;
			player->playerNumber = i;
		}
		serverLogf(INFO, "A match between %d and %d\n", match->players[0].playerClientId, match->players[1].playerClientId);
		return true;
	} else {
		serverLogf(WARNING, "Players are waiting, but there are too many ongoing matches\n");
		return false;
	}
}

void updateMatchTcpListener(void *threadStruct) {
	Thread *thread = (Thread *)threadStruct;
	Match *match = (Match *)thread->param;

	for (;;) {
		while (match->_tcpSocketNum < 2) {
			Socket *tcpSocket = socketListen(match->tcpListener);

			if (!socketSetNonblock(tcpSocket)) {
				serverLogf(INFO, "Failed to make tcp socket nonblocking for player\n");
				logLastOSError();
				Panic("Can't continue with nonblocking tcp socket for player");
			}

			match->_tcpSockets[match->_tcpSocketNum++] = tcpSocket;
		}

		platformSleep(1);
	}
}

#if defined(__linux__)
void mongooseMatchHandler(mg_connection *connection, int event, void *eventData);

void mongooseMatchHandler(mg_connection *connection, int event, void *eventData) {
	Match *match = (Match *)connection->mgr->user_data;

	if (event == MG_EV_WEBSOCKET_HANDSHAKE_DONE) {
		match->_webSockets[match->_webSocketsNum++] = connection;
	} else if (event == MG_EV_WEBSOCKET_FRAME) {
		websocket_message *wm = (websocket_message *)eventData;
		if (wm->size != sizeof(NetMsg)) {
			serverLogf(WARNING, "Weird size %d instead of %d\n", wm->size, sizeof(NetMsg));
			return;
		}

 		MsgPacket packet = {};
 		memcpy(&packet.msg, wm->data, sizeof(NetMsg));
 		packet.webSocket = connection;
 		bool good = threadSafeQueuePush(match->_webSocketPacketsIn, &packet);
 		if (good) {
 			// serverLogf(INFO, "Server got %d\n", packet.msg.type);
 		} else {
 			MsgPacket tempPacket;
 			threadSafeQueueShift(match->_webSocketPacketsIn, &tempPacket);
 			good = threadSafeQueuePush(match->_webSocketPacketsIn, &packet);
 			if (!good) serverLogf(LERROR, "Inifintely match full _webSocketPacketsIn?\n");
 		}

	} else if (event == MG_EV_HTTP_REQUEST) {
		serverLogf(INFO, "Got http request\n");
		mg_serve_http(connection, (http_message *)eventData, server->mongooseSockOpts);
	} else if (event == MG_EV_CLOSE) {
		serverLogf(INFO, "Got match close\n");
		for (int i = 0; i < match->playersNum; i++) {
			MatchPlayer *player = &match->players[i];
			if (player->webSocket == connection) {
				serverLogf(INFO, "Removed player %d\n", player->playerClientId);
				arraySpliceIndex(match->players, PLAYERS_PER_MATCH_MAX, sizeof(MatchPlayer), i);
				i--;
				match->playersNum--;
				break;
			}
		}

		for (int i = 0; i < match->_webSocketsNum; i++) {
			void *current = match->_webSockets[i];
			if (current == connection) {
				arraySpliceIndex(match->_webSockets, PLAYERS_PER_MATCH_MAX, sizeof(void *), i);
				match->_webSocketsNum--;
				i--;
				continue;
			}
		}
		match->shouldEnd = true;
		// match->_webSockets[match->_webSocketsNum++] = connection;
		/* Disconnect. Tell everybody. */
		if (connection->flags & MG_F_IS_WEBSOCKET) {
			// broadcast(connection, mg_mk_str("-- left"));
		}
	}
}
#endif

bool updateMatch(Match *match, float elapsed) {
	if (match->ticks == 0) {
		int matchTcpListenerPort = -1;
		int matchUdpListenerPort = -1;
		{ /// Create udp listener
			match->udpListener = createUdpSocket(sizeof(NetMsg));
			if (!match->udpListener) {
				serverLogf(INFO, "Failed to create udp socket for match\n");
				logLastOSError();
			}

			bool good;
			good = socketSetNonblock(match->udpListener);
			if (!good) {
				serverLogf(INFO, "Failed to make udp socket nonblocking for match\n");
				logLastOSError();
				Panic("Can't continue with nonblocking socket for match");
			}

			good = socketBind(match->udpListener, createSockaddr(0, 0));
			if (!good) {
				serverLogf(INFO, "Failed to make bind udp socket\n");
				logLastOSError();
			}

			matchUdpListenerPort = getSocketPort(match->udpListener);
			serverLogf(INFO, "Match listener started on port: %d\n", matchUdpListenerPort);
		}

		{ /// Create tcp listener
			serverLogf(INFO, "Creating tcp listener\n");
			match->tcpListener = createTcpSocket(sizeof(NetMsg));

			if (!match->tcpListener) {
				serverLogf(INFO, "Failed to create tcp socket for match\n");
				logLastOSError();
			}

			bool good;
			good = socketBind(match->tcpListener, createSockaddr(0, 0));
			if (!good) {
				serverLogf(INFO, "Failed to make bind tcp socket\n");
				logLastOSError();
			}

			matchTcpListenerPort = getSocketPort(match->tcpListener);
			serverLogf(INFO, "Match listener started on port: %d\n", matchTcpListenerPort);
		}

		{ /// Figure out web server port
			Socket *tcpListener = createTcpSocket(sizeof(NetMsg));
			if (!tcpListener) {
				serverLogf(INFO, "Failed to create tcp socket to get port\n");
				logLastOSError();
			}

			if (!socketBind(tcpListener, createSockaddr(0, 0))) {
				serverLogf(INFO, "Failed to make bind tcp socket to get port\n");
				logLastOSError();
			}

			match->_webServerFreePort = getSocketPort(tcpListener);
			socketClose(tcpListener);
		}

		for (int i = 0; i < match->playersNum; i++) {
			MatchPlayer *player = &match->players[i];
			serverLogf(INFO, "player %d\n", i);

			MsgPacket packet = {};
			packet.msg.type = NET_MSG_StC_MATCH_START;
			packet.msg.matchId = match->id;
			packet.msg.matchStart.playerNumber = player->playerNumber;
			packet.msg.matchStart.sameIp = match->sameIp;
			packet.msg.matchStart.framesPerTurn = match->framesPerTurn;
			if (getPlayerClient(player->playerClientId)->webSocket) {
				packet.msg.matchStart.matchTcpListenerPort = match->_webServerFreePort;
			} else {
				packet.msg.matchStart.matchTcpListenerPort = matchTcpListenerPort;
			}
			packet.msg.matchStart.matchUdpListenerPort = matchUdpListenerPort;
			// serverLogf(INFO, "Server sending session port: %d %d\n", matchTcpListenerPort, packet.msg.matchStart.matchTcpListenerPort);
			strcpy(packet.msg.matchStart.mapName, server->serverConfig.mapName);
			bool good = threadSafeQueuePush(getPlayerClient(player->playerClientId)->outgoingPackets, &packet);
			if (!good) serverLogf(LERROR, "client outgoingPackets is full [%d]\n", packet.msg.type);
		}

		match->_webSocketPacketsIn = createThreadSafeQueue(sizeof(MsgPacket), 256, false);

#if defined(__linux__)
		mg_mgr_init(&match->mongooseManager, NULL);
		match->mongooseManager.user_data = match;

		char *mongoosePort = mallocSprintf("%d", match->_webServerFreePort);
		match->mongooseListener = mg_bind(&match->mongooseManager, mongoosePort, mongooseMatchHandler);
		mg_set_protocol_http_websocket(match->mongooseListener);

		match->mongooseSockOpts.document_root = ".";
		match->mongooseSockOpts.enable_directory_listing = "false";
#endif

		ReadFence();
		WriteFence();
		// match->tcpThread = createThread(updateMatchTcpListener, match);
		// match->webThread = createThread(updateMatchWebSocketListener, match);
	}

	bool shouldEndMatch = match->shouldEnd;

	{ // Websocket session
#if defined(__linux__)
		for (;;) {
			if (mg_mgr_poll(&match->mongooseManager, 0) == 0) break;
		}
#endif
	}

	{ /// Web socket session
		for (;;) {
			MsgPacket packet;
			bool good = threadSafeQueueShift(match->_webSocketPacketsIn, &packet);
			if (!good) break;
			NetMsg *msg = &packet.msg;

			MatchPlayer *owningPlayer = NULL;

			for (int i = 0; i < match->playersNum; i++) {
				MatchPlayer *player = &match->players[i];
				if (player->playerClientId == msg->clientId) {
					owningPlayer = player;
					player->webSocket = packet.webSocket;
					break;
				}
			}

			if (!owningPlayer) {
				serverLogf(LERROR, "Can't find MatchPlayer based off clientId: %d\n", msg->clientId);
			} else {
				if (!threadSafeQueuePush(owningPlayer->incomingPackets, &packet)) {
					serverLogf(LERROR, "Player incoming queue is overloaded for websockets\n");
				}
			}
		}

		for (int i = 0; i < match->playersNum; i++) {
			MatchPlayer *player = &match->players[i];
			if (!player->webSocket) continue;

#if defined(__linux__)
			for (;;) {
				MsgPacket packet;
				bool good = threadSafeQueueShift(player->outgoingPackets, &packet);
				if (!good) break;

				if (!player->webSocket) break;
				mg_send_websocket_frame((mg_connection *)player->webSocket, WEBSOCKET_OP_BINARY, &packet.msg, sizeof(NetMsg));
			}
#endif
		}
	}

	{ /// Tcp session
		for (int i = 0; i < match->_tcpSocketNum; i++) {
			Socket *tcpSocket = match->_tcpSockets[i];
			MatchPlayer *owningPlayer = NULL;

			for (int i = 0; i < match->playersNum; i++) {
				MatchPlayer *player = &match->players[i];
				if (player->tcpSocket == tcpSocket) {
					owningPlayer = player;
					break;
				}
			}

			for (;;) {
				SocketlingStatus status = socketReceive(tcpSocket);
				if (status == SOCKETLING_ZERO_RECEIVE) {
					shouldEndMatch = true;
				} else if (status == SOCKETLING_CONNECTION_RESET) {
					shouldEndMatch = true;
				} else if (status == SOCKETLING_WOULD_BLOCK) {
					// Nothing...
				} else if (status == SOCKETLING_PENDING) {
					// Nothing...
				} else if (status == SOCKETLING_ERROR) {
					logLastOSError();
					shouldEndMatch = true;
				} else if (status == SOCKETLING_GOOD) {
					// serverLogf(INFO, "Match received\n");
					MsgPacket packet = {};
					memcpy(&packet.msg, tcpSocket->receiveBuffer, sizeof(NetMsg));

					for (int i = 0; i < match->playersNum; i++) {
						MatchPlayer *player = &match->players[i];
						if (player->playerClientId == packet.msg.clientId) {
							owningPlayer = player;
							player->tcpSocket = tcpSocket;
							bool good = threadSafeQueuePush(player->incomingPackets, &packet);
							if (!good) serverLogf(LERROR, "Player incoming queue is overloaded (from tcp)\n");
							break;
						}
					}
				} else {
					serverLogf(LERROR, "Unknown socketling status %d\n", status);
				}

				if (status != SOCKETLING_GOOD) break;

			}

			if (owningPlayer) {
				for (;;) {
					MsgPacket packet;
					bool good = threadSafeQueueShift(owningPlayer->outgoingPackets, &packet);
					if (!good) break;
					NetMsg *msg = &packet.msg;

					SocketlingStatus status = socketSend(tcpSocket, msg);
					if (status == SOCKETLING_WOULD_BLOCK || status == SOCKETLING_PENDING) {
						if (!threadSafeQueuePush(owningPlayer->outgoingPackets, &packet)) {
							serverLogf(LERROR, "Player outgoingPackets is full (on repush) [%d]\n", packet.msg.type);
						}
					} else if (status == SOCKETLING_ERROR) {
						logLastOSError();
						shouldEndMatch = true;
					} else if (status == SOCKETLING_GOOD) {
						// Nothing...
					} else {
						serverLogf(INFO, "Unknown socketling status %d\n", status);
					}

				}
			}
		}

		for (int i = 0; i < PLAYERS_PER_MATCH_MAX; i++) {
			MatchPlayer *player = &match->players[i];
			if (!player->tcpSocket) continue;

			for (;;) { /// Tcp send
				MsgPacket packet;
				bool good = threadSafeQueueShift(player->outgoingPackets, &packet);
				if (!good) break;
				NetMsg *msg = &packet.msg;

				SocketlingStatus status = socketSend(player->tcpSocket, msg);
				if (status == SOCKETLING_WOULD_BLOCK || status == SOCKETLING_PENDING) {
					if (!threadSafeQueuePush(player->outgoingPackets, &packet)) {
						serverLogf(LERROR, "Player outgoingPackets is full (on repush) [%d]\n", packet.msg.type);
					}
				} else if (status == SOCKETLING_ERROR) {
					logLastOSError();
					shouldEndMatch = true;
				} else if (status == SOCKETLING_GOOD) {
					// Nothing...
				} else {
					serverLogf(INFO, "Unknown socketling status %d\n", status);
				}
			}
		}
	}

	{ /// Udp session
		for (;;) {
			SocketlingStatus status = socketReceive(match->udpListener);
			if (status == SOCKETLING_ZERO_RECEIVE) {
				shouldEndMatch = true;
			} else if (status == SOCKETLING_CONNECTION_RESET) {
				shouldEndMatch = true;
			} else if (status == SOCKETLING_WOULD_BLOCK) {
				// Nothing...
			} else if (status == SOCKETLING_PENDING) {
				// Nothing...
			} else if (status == SOCKETLING_ERROR) {
				logLastOSError();
			} else if (status == SOCKETLING_GOOD) {
				MsgPacket packet = {};
				memcpy(&packet.msg, match->udpListener->receiveBuffer, sizeof(NetMsg));
				packet.addr = match->udpListener->srcAddr; // This line may do something one day

				for (int i = 0; i < match->playersNum; i++) {
					MatchPlayer *player = &match->players[i];
					if (player->playerClientId == packet.msg.clientId) {
						player->udpAddr = packet.addr;
						bool good = threadSafeQueuePush(player->incomingPackets, &packet);
						if (!good) serverLogf(LERROR, "Player incoming queue is overloaded\n");
						break;
					}
				}
			} else {
				serverLogf(LERROR, "Unknown socketling status %d\n", status);
			}

			if (status != SOCKETLING_GOOD) break;
		}

		for (int i = 0; i < match->playersNum; i++) {
			MatchPlayer *player = &match->players[i];
			// serverLogf(INFO, "Player %d last seen %f\n", player->playerNumber, player->timeSinceLastSeen);
			if (!player->supportsUdp || getSockaddrPort(player->udpAddr) == 0) continue;
			for (;;) {
				MsgPacket packet;
				bool good = threadSafeQueueShift(player->outgoingPackets, &packet);
				if (!good) break;
				NetMsg *msg = &packet.msg;

				SocketlingStatus status = socketSend(match->udpListener, msg, player->udpAddr);
				if (status == SOCKETLING_WOULD_BLOCK || status == SOCKETLING_PENDING) {
					if (!threadSafeQueuePush(player->outgoingPackets, &packet)) {
						serverLogf(LERROR, "Player outgoingPackets is full (on repush) [%d]\n", packet.msg.type);
					}
				} else if (status == SOCKETLING_ERROR) {
					logLastOSError();
					shouldEndMatch = true;
				} else if (status == SOCKETLING_GOOD) {
					// Nothing...
				} else {
					serverLogf(INFO, "Unknown socketling status %d\n", status);
				}
			}
		}
	}

	int earliestConfirmedUpToTurn = INT_MAX;
	for (int i = 0; i < match->playersNum; i++) {
		if (match->confirmedUpToTurn[i] <= earliestConfirmedUpToTurn) {
			earliestConfirmedUpToTurn = match->confirmedUpToTurn[i];
		}
	}

	for (int i = 0; i < match->gameCmdsNum; i++) {
		GameCmd *gameCmd = &match->gameCmds[i];
		if (gameCmd->turnToExecute <= earliestConfirmedUpToTurn) {
			arraySplice(match->gameCmds, sizeof(GameCmd) * GAME_CMDS_TO_HOLD_MAX, sizeof(GameCmd), gameCmd);
			match->gameCmdsNum--;
			i--;
		}
	}

	for (int i = 0; i < match->playersNum; i++) {
		MatchPlayer *player = &match->players[i];
		player->timeSinceLastSeen += elapsed;
		if (player->timeSinceLastSeen >= maxDisconnectTime) {
			shouldEndMatch = true;
			break;
		}

		for (;;) {
			MsgPacket packet;
			bool good = threadSafeQueueShift(player->incomingPackets, &packet);
			if (!good) break;
			NetMsg *msg = &packet.msg;

			player->timeSinceLastSeen = 0;
			// { /// Sync timeSinceLastSeen
			// 	PlayerClient *client = getPlayerClient(player->playerClientId);
			// 	if (client) {
			// 		if (player->playerClientId == client->id) {
			// 			if (player->timeSinceLastSeen < client->timeSinceLastSeen) client->timeSinceLastSeen = player->timeSinceLastSeen;
			// 		}
			// 	}
			// }

			if (msg->type == NET_MSG_CtS_TURN_DATA) {
				for (int i = 0; i < msg->turnData.gameCmdsNum; i++) {
					if (match->gameCmdsNum > GAME_CMDS_TO_HOLD_MAX-1) serverLogf(PANIC, "Can't hold this many GameCmds");

					GameCmd *gameCmd = &msg->turnData.gameCmds[i];
					int diff = gameCmd->turnToExecute - match->gotUpToTurn[player->playerNumber];
					if (diff <= 0) continue;
					if (diff > 1) continue;
					// serverLogf(INFO, "Server got command for turn %d (diff %d p%d (%s))\n", gameCmd->turnToExecute, diff, gameCmd->playerNumber, gameCmdTypeStrings[gameCmd->type]);
					match->gameCmds[match->gameCmdsNum++] = *gameCmd;

					if (gameCmd->type == GAME_CMD_END_TURN) {
						// serverLogf(INFO, "Server got end turn for player %d on turn %d\n", gameCmd->playerNumber, gameCmd->turnToExecute);
						int turnToExecuteCompare = gameCmd->turnToExecute;
						char worldHash = gameCmd->worldHash;
						for (int i = match->gameCmdsNum-1; i >= 0; i--) {
							GameCmd *gameCmd = &match->gameCmds[i];
							if (gameCmd->type == GAME_CMD_END_TURN && gameCmd->turnToExecute == turnToExecuteCompare && gameCmd->playerNumber != player->playerNumber) {
								if (gameCmd->worldHash != worldHash) {
									if (!match->isDesynced) {
										time_t currentDateTime = time(0);
										tm *now = localtime(&currentDateTime);
										char *name = frameSprintf(
											"assets/replays/serverDesync %d%d%d %d%d%d.txt",
											now->tm_mon+1,
											now->tm_mday,
											now->tm_year+1900,
											now->tm_hour,
											now->tm_min,
											now->tm_sec
										);

										MatchPlayer *player0 = &match->players[0];
										MatchPlayer *player1 = &match->players[1];

										char ip0[17] = {};
										int port0 = 0;
										if (player0) getSockaddrIpAndPort(&player0->udpAddr, ip0, &port0);

										char ip1[17] = {};
										int port1 = 0;
										if (player1) getSockaddrIpAndPort(&player1->udpAddr, ip1, &port1);

										serverLogf(WARNING, "Match desynced, saved at '%s' between %s:%d and %s:%d\n", name, ip0, port0, ip1, port1);
										saveStructArray("GameCmd", name, match->gameCmdHistory, match->gameCmdHistoryNum, sizeof(GameCmd));
									}
									match->isDesynced = true;
								}
							}
						}
						match->gotUpToTurn[player->playerNumber] = gameCmd->turnToExecute;
					}

					memcpy(&match->gameCmdHistory[match->gameCmdHistoryNum], gameCmd, sizeof(GameCmd));
					match->gameCmdHistoryNum++;
				}

				match->confirmedUpToTurn[player->playerNumber] = msg->turnData.confirmedTurn;
			} else if (msg->type == NET_MSG_CtS_DISCONNECT_FROM_MATCH) {
				shouldEndMatch = true;
				serverLogf(INFO, "Player is disconnecting from their match\n");
			}
		}

		if (platform->frameCount % sendEveryXFrames == 0 || sendEveryXFrames <= 1) {
			MsgPacket packet = {};
			packet.msg.type = NET_MSG_StC_TURN_DATA;
			for (int i = 0; i < match->gameCmdsNum; i++) {
				NetMsg *msg = &packet.msg;
				GameCmd *gameCmd = &match->gameCmds[i];
				if (gameCmd->playerNumber == player->playerNumber) continue;
				// if (gameCmd->type == GAME_CMD_RESIGN) shouldEndMatch = true;
				if (msg->turnData.gameCmdsNum < GAME_CMDS_MAX) {
					msg->turnData.gameCmds[msg->turnData.gameCmdsNum++] = *gameCmd;
				} else {
					serverLogf(PANIC, "Trying to send too many GameCmds to the client");
				}
			}
			if (!threadSafeQueuePush(player->outgoingPackets, &packet)) {
				serverLogf(INFO, "Player outgoingPackets is too full [%d]\n", packet.msg.type);
			}
		}
	}

	match->ticks++;

	if (shouldEndMatch) {
#if defined(__linux__)
		mg_mgr_free(&match->mongooseManager);
#endif

		ReadFence();
		WriteFence();
		platformSleep(100);
		// destroyThread(match->tcpThread);
		// destroyThread(match->webThread);

		for (int i = 0; i < match->playersNum; i++) {
			MatchPlayer *player = &match->players[i];
			destroyThreadSafeQueue(player->incomingPackets);
			destroyThreadSafeQueue(player->outgoingPackets);
		}

		destroyThreadSafeQueue(match->_webSocketPacketsIn);
		if (match->udpListener) socketClose(match->udpListener);
		if (match->tcpListener) socketClose(match->tcpListener);

		for (int i = 0; i < match->_tcpSocketNum; i++) {
			socketClose(match->_tcpSockets[i]);
		}

		free(match->gameCmdHistory);

		return true;
	}

	return false;
}

PlayerClient *getPlayerClient(int id) {
	if (id == 0) return NULL;

	for (int i = 0; i < server->playerClientsNum; i++) {
		PlayerClient *client = &server->playerClients[i];
		if (client->id == id) return client;
	}

	return NULL;
}

void updateTcpListenerThread(void *threadStruct) {
	server->_tcpSocket = createTcpSocket(sizeof(NetMsg));
	if (!server->_tcpSocket) {
		logLastOSError();
		serverLogf(PANIC, "Failed to create tcp socket for server\n");
	}

	if (!socketSetNonblock(server->_tcpSocket)) {
		serverLogf(INFO, "Failed to make tcp socket nonblocking for server\n");
		logLastOSError();
		Panic("Can't continue with nonblocking tcp socket for server\n");
	}

	bool good;
	good = socketBind(server->_tcpSocket, createSockaddr(0, server->_port+2));
	if (!good) {
		logLastOSError();
		serverLogf(PANIC, "Failed to bind tcp socket for server\n");
	}

	for (;;) {
		Socket *newSocket = socketListen(server->_tcpSocket);

		if (newSocket == NULL && lastSocketErrorWasWouldBlock()) {
			continue;
		} else {
			serverLogf(INFO, "Failed to make listen\n");
			logLastOSError();
			Panic("Can't continue without listening\n");
		}

		if (!socketSetNonblock(newSocket)) {
			serverLogf(INFO, "Failed to make tcp socket nonblocking for client\n");
			logLastOSError();
			Panic("Can't continue with nonblocking tcp socket for client\n");
		}
		serverLogf(INFO, "New socket is %s:%d\n", getPeerIp(newSocket), getPeerPort(newSocket));

		MsgPacket packet = {};
		packet.msg.type = NET_MSG_CtS_TCP_CONNECT;
		packet.tcpSocket = newSocket;

		bool good = threadSafeQueuePush(server->_incomingPackets, &packet);
		if (!good) serverLogf(LERROR, "Incoming queue is overloaded\n");
	}
}

void updateUdpThread(void *threadStruct) {
	serverLogf(INFO, "Server starting, listening on port %d (NetMsg is %d bytes)\n", server->_port, sizeof(NetMsg));
	server->_udpSocket = createUdpSocket(sizeof(NetMsg));
	if (!server->_udpSocket) {
		serverLogf(INFO, "Failed to create udp socket\n");
		logLastOSError();
	}

	bool good;
	good = socketSetReuseaddr(server->_udpSocket);
	if (!good) {
		serverLogf(INFO, "Failed to make udp socket reuseaddr\n");
		logLastOSError();
	}

	good = socketSetNonblock(server->_udpSocket);
	if (!good) {
		serverLogf(INFO, "Failed to make udp socket nonblocking\n");
		logLastOSError();
		Panic("Can't continue with nonblocking socket");
	}

	good = socketBind(server->_udpSocket, createSockaddr(0, server->_port));
	if (!good) {
		serverLogf(INFO, "Failed to make bind udp socket\n");
		logLastOSError();
	}

	for (;;) {
		for (;;) {
			SocketlingStatus status = socketReceive(server->_udpSocket);
			if (status == SOCKETLING_ZERO_RECEIVE) {
				// Kicked?
			} else if (status == SOCKETLING_CONNECTION_RESET) {
				// Kicked?
			} else if (status == SOCKETLING_WOULD_BLOCK) {
				// Nothing...
			} else if (status == SOCKETLING_PENDING) {
				// Nothing...
			} else if (status == SOCKETLING_ERROR) {
				logLastOSError();
			} else if (status == SOCKETLING_GOOD) {
				MsgPacket packet = {};
				memcpy(&packet.msg, server->_udpSocket->receiveBuffer, sizeof(NetMsg));
				packet.addr = server->_udpSocket->srcAddr; // This line does something!

				if (!threadSafeQueuePush(server->_incomingPackets, &packet)) {
					serverLogf(LERROR, "Incoming queue is overloaded\n");
				}
			} else {
				serverLogf(LERROR, "Unknown socketling status %d\n", status);
			}

			if (status != SOCKETLING_GOOD) break;
		}

		for (;;) {
			MsgPacket packet;
			bool good = threadSafeQueueShift(server->_outgoingPackets, &packet);
			if (!good) break;
			NetMsg *msg = &packet.msg;

#if 0
			if (msg->type == NET_MSG_StC_TURN_DATA) {
				char ip[17];
				int port;
				getSockaddrIpAndPort(&packet.addr, ip, &port);
				serverLogf(INFO, "Sending turn data to: %s:%d\n", ip, port);
			}
#endif

			SocketlingStatus status = socketSend(server->_udpSocket, msg, packet.addr);
			if (status == SOCKETLING_WOULD_BLOCK || status == SOCKETLING_PENDING) {
				if (!threadSafeQueuePush(server->_outgoingPackets, &packet)) {
					serverLogf(LERROR, "_outgoingPackets is full [%d]\n", packet.msg.type);
				}
			} else if (status == SOCKETLING_ERROR) {
				logLastOSError();
			} else if (status == SOCKETLING_GOOD) {
				// Nothing...
			} else {
				serverLogf(INFO, "Unknown socketling status %d\n", status);
			}
		}

		// platformSleep(32);
		platformSleep(1);
	}
}

void serverLogLastOsErrorf(ServerLogLevel level, const char *msg, ...) {
	char *osErrorStr = getLastOSError();
	char *msgStr = NULL;
	{
		va_list args;
		va_start(args, msg);
		int size = stbsp_vsnprintf(NULL, 0, msg, args);
		va_end(args);

		msgStr = (char *)malloc(size+1);

		va_start(args, msg);
		stbsp_vsnprintf(msgStr, size+1, msg, args);
		va_end(args);
	}

	serverLogf(level, "%s (%s)\n", msgStr, osErrorStr);

	free(msgStr);
	if (osErrorStr) free(osErrorStr);
}

void realServerLogf(char *file, int lineNum, ServerLogLevel level, const char *msg, ...) {
	IncMutex(&server->_serverLogfMutex);

	time_t currentDateTime = time(0);
	tm *now = localtime(&currentDateTime);

	va_list args;
	va_start(args, msg);
	int size = stbsp_vsnprintf(NULL, 0, msg, args);
	va_end(args);

	char *msgStr = (char *)malloc(size+1);

	va_start(args, msg);
	stbsp_vsnprintf(msgStr, size+1, msg, args);
	va_end(args);

	char *logHeader = mallocSprintf(
		"[%d/%d/%d %02d:%02d:%02d (%s:%d)]",
		now->tm_mon+1,
		now->tm_mday,
		now->tm_year+1900,
		now->tm_hour,
		now->tm_min,
		now->tm_sec,
		file,
		lineNum
	);

	char *osErrorStr = NULL;
	if (level == WARNING || level == LERROR || level == PANIC) osErrorStr = getLastOSError();
	if (!osErrorStr) osErrorStr = (char *)zalloc(1);

	char *finalLogStr = mallocSprintf("%s %c: %s %s", logHeader, serverLogLevelStrings[level], osErrorStr, msgStr);
	free(osErrorStr);
	free(logHeader);
	free(msgStr);

	int finalLogStrNum = strlen(finalLogStr);
	if (server->serverLogBufferNum + finalLogStrNum > SERVER_LOG_BUFFER_MAX_LEN-1) saveLogFile();

	strcpy(&server->serverLogBuffer[server->serverLogBufferNum], finalLogStr);
	server->serverLogBufferNum += finalLogStrNum;

	server->_shouldReSaveLog = true;

	logf(finalLogStr);
	free(finalLogStr);

	if (level == PANIC) {
		saveLogFile();
		Panic("Server panic");
	}

	DecMutex(&server->_serverLogfMutex);
}

void saveLogFile() {
	time_t currentDateTime = time(0);
	tm *now = localtime(&currentDateTime);

	char *logFileName = frameSprintf(
		"%d%d%d %02d%02d%02d (%d) server log.txt",
		now->tm_mon+1,
		now->tm_mday,
		now->tm_year+1900,
		now->tm_hour,
		now->tm_min,
		now->tm_sec,
		server->serverLogsSaved
	);

	// logf("Would save %s\n", server->serverLogBuffer);
	writeFile(logFileName, server->serverLogBuffer, server->serverLogBufferNum);
	server->serverLogsSaved++;
	server->serverLogBufferNum = 0;
}

void serverCleanup() {
	saveLogFile();
	deinitNetworking();
}