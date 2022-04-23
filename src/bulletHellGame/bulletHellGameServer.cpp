#include "bulletHellGameCommon.cpp"

#define SAVES_LOG 0

enum ClientMode {
	CLIENT_NONE,
	CLIENT_IN_QUEUE,
	CLIENT_PLAYING,
};
char *clientModeStrings[] = {
	"None",
	"In queue",
	"Playing",
};

struct Client {
	Socket *socket;
	ClientMode mode;
	int id;

	int enemyId;
	int inputCount;
	int inputHash;

#define MSGS_MAX 64
	NetMsg outMsgs[MSGS_MAX];
	int outMsgsNum;

	bool hasInputs;
	int hasInputsForTurn;
	Input inputs[NET_MSG_INPUTS_MAX];

	float timeSinceLastSeen;
	float timeSinceLastPing;

	char *syncString;
	int syncStringNum;
	int syncStringMax;
};

struct Server {
	Socket *listener;

#define CLIENTS_MAX 256
	Client clients[CLIENTS_MAX];
	int clientsNum;
	int nextSessionToken;

	float timeTillNextListenerRetry;
};

Server *server = NULL;

void runGame();
void updateServer();
NetMsg *newOutMsg(Client *client, NetMsgType type);
NetMsg *newOutMsgFromClone(Client *client, NetMsg *origMsg);
Client *getClient(int id);
void serverCleanup();
void sigPipeHandler(int s) { }
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

	initPlatform(1280, 720, "Bullet hell server");
	platform->sleepWait = true;
	// initAudio();
	if (!platform->isCommandLineOnly) initRenderer(1280, 720);
	// initMesh();
	// initSkeleton();
	initFonts();
	initTextureSystem();

	bool good = initNetworking();
	if (!good) Panic("Failed to init networking");

	platform->atExitCallback = serverCleanup;
#if (__LINUX__)
	signal(SIGPIPE, sigPipeHandler);
#endif

	platformUpdateLoop(updateServer);
}

NanoTime last;
void updateServer() {
	if (!server) {
		server = (Server *)zalloc(sizeof(Server));
		logf("Server is %.2fmb btw\n", sizeof(Server)/(float)Megabytes(1));

		minimizeWindow();
	}

	float elapsed = platform->elapsed;
	if (elapsed > 0.02) logf("elapsed: %f\n", elapsed);

	float myElapsed = getMsPassed(last)/1000.0;
	last = getNanoTime();

	if (!server->listener) {
		logf("Starting listener\n");
		server->timeTillNextListenerRetry -= elapsed;

		if (server->timeTillNextListenerRetry < 0) {
			server->listener = createTcpSocket(sizeof(NetMsg));
			if (!server->listener) {
				logLastOSError();
				server->timeTillNextListenerRetry = 1;
			}

			if (!socketSetNonblock(server->listener)) {
				logLastOSError();
				socketClose(server->listener);
				server->listener = NULL;
				server->timeTillNextListenerRetry = 1;
			}

			int port = 43594;
			if (!socketBind(server->listener, createSockaddr(0, port))) {
				socketClose(server->listener);
				server->listener = NULL;
				server->timeTillNextListenerRetry = 1;

				logLastOSError();
				logf("Failed to bind tcp socket for server\n");
			}

			logf("Listener created!\n");
		}
	}

	if (server->listener) {
		Socket *newSocket = socketListen(server->listener);
		if (newSocket) {
			if (!socketSetNonblock(newSocket)) {
				logf("Failed to make tcp socket nonblocking for client\n");
				logLastOSError();
				Panic("Can't continue with nonblocking tcp socket for client\n");
			}
		} else {
			if (!lastSocketErrorWasWouldBlock()) {
				logLastOSError();
			}
		}

		if (newSocket) {
			logf("New connection %s:%d\n", getPeerIp(newSocket), getPeerPort(newSocket));
			if (server->clientsNum < CLIENTS_MAX) {
				Client *client = &server->clients[server->clientsNum++];
				memset(client, 0, sizeof(Client));
				client->socket = newSocket;
				client->id = ++server->nextSessionToken;
			} else {
				logf("Too many clients\n");
			}
		}
	}

	for (int i = 0; i < server->clientsNum; i++) {
		Client *client = &server->clients[i];
		for (;;) { /// Receive
			SocketlingStatus status = socketReceive(client->socket);
			if (status == SOCKETLING_ZERO_RECEIVE) {
				// logf("A client has disconnected\n");
				// exit(0);
			} else if (status == SOCKETLING_CONNECTION_RESET) {
				// logf("A client was destroyed\n");
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
				client->timeSinceLastSeen = 0;
				// logf("Good\n");
				// memcpy(&msg, client->socket->receiveBuffer, sizeof(NetMsg));
				NetMsg *msg = (NetMsg *)client->socket->receiveBuffer;

				if (msg->type != NET_MSG_CtS_CONNECT && msg->id != client->id) {
					logf("Client id mismatch %d should be %d\n", msg->id, client->id);
					continue;
				}

				if (msg->type == NET_MSG_CtS_CONNECT) {
					NetMsg *newMsg = newOutMsg(client, NET_MSG_StC_CONNECT);
					newMsg->id = client->id;
					logf("SERVER: token is: %d\n", client->id);

					newOutMsg(client, NET_MSG_StC_IN_QUEUE);
					client->mode = CLIENT_IN_QUEUE;
				} else if (
					msg->type == NET_MSG_CtC_INPUTS ||
					msg->type == NET_MSG_CtC_SPELL_CAST
				) {
					Client *enemyClient = getClient(client->enemyId);
					if (!enemyClient) {
						logf("No enemy client! <---------------- This is important!\n");
						continue;
					}
#if 0
					NetMsg *newMsg = newOutMsg(enemyClient, NET_MSG_CtC_INPUTS);
					for (int i = 0; i < NET_MSG_INPUTS_MAX; i++) {
						newMsg->unionInputs.inputs[i] = msg->unionInputs.inputs[i];
					}
#else
					newOutMsgFromClone(enemyClient, msg);
#endif

					if (msg->type == NET_MSG_CtC_INPUTS) {
						for (int i = 0; i < NET_MSG_INPUTS_MAX; i++) {
							Input *input = &msg->unionInputs.inputs[i];
							client->inputHash = incInputHash(client->inputHash, input, client->inputCount);

							char *str = frameSprintf(
								"%d(%d) | %d%d%d%d%d%d%d%d\n",
								client->inputCount,
								client->inputHash,
								input->up,
								input->down,
								input->left,
								input->right,
								input->focus,
								input->shoot,
								input->bomb,
								input->extra
							);
							client->inputCount++;
							if (!client->syncString) {
								client->syncStringMax = 1024;
								client->syncStringNum = 0;
								client->syncString = (char *)malloc(sizeof(char) * client->syncStringMax);
								client->syncString[0] = 0;
							}

							int len = strlen(str)+1;
							if (client->syncStringNum + len > client->syncStringMax-1) {
								client->syncString = (char *)resizeArray(client->syncString, sizeof(char), client->syncStringMax, client->syncStringMax * 1.5);
								client->syncStringMax *= 1.5;
							}

							strcat(client->syncString, str);
							client->syncStringNum += len;

							int lineCount = countChar(client->syncString, '\n');
							if (lineCount >= 600) {
#if SAVES_LOG
								appendFile(
									frameSprintf("assets/logs/inputs%d.txt", client->id),
									client->syncString,
									strlen(client->syncString)
								);
#endif
								free(client->syncString);
								client->syncString = NULL;
							}
						}

					}
				}
			} else {
				logf("Unknown socketling status on receive %d\n", status);
			}

			if (status != SOCKETLING_GOOD) break;
		}

		for (int i = 0; i < client->outMsgsNum; i++) { /// Send
			NetMsg *msg = &client->outMsgs[i];
			for (;;) {

				SocketlingStatus status = socketSend(client->socket, msg);
				if (status == SOCKETLING_WOULD_BLOCK || status == SOCKETLING_PENDING) {
					logf("Trying to send msg\n");
				} else if (status == SOCKETLING_ERROR) {
					logLastOSError();
				} else if (status == SOCKETLING_GOOD) {
					// logf("Sent %d to client %d\n", msg->type, client->id);
					break;
				} else if (status == SOCKETLING_CONNECTION_RESET) {
					// Nothing...
					client->outMsgsNum = 0;
					client->timeSinceLastSeen = 999;
					break;
				} else {
					logf("Unknown socketling status on send %d\n", status);
				}
			}
		}
		client->outMsgsNum = 0;
	}

	{
		Client *inQueue[2];
		int inQueueNum = 0;
		for (int i = 0; i < server->clientsNum; i++) {
			Client *client = &server->clients[i];
			if (client->mode == CLIENT_IN_QUEUE) {
				inQueue[inQueueNum++] = client;
				if (inQueueNum == 2) break;
			}
		}

		if (inQueueNum == 2) {
			int seed = rndInt(0, pow(2, 32)-1);

#if SAVES_LOG
			// if (directoryExists("assets/logs")) removeDirectory("assets/logs");
			// createDirectory("assets/logs");
#endif

			for (int i = 0; i < inQueueNum; i++) {
				Client *client = inQueue[i];
				Client *otherClient = i == 0 ? inQueue[1] : inQueue[0];

				client->enemyId = otherClient->id;
				client->mode = CLIENT_PLAYING;
				NetMsg *msg = newOutMsg(client, NET_MSG_StC_START_GAME);
				msg->unionStartGame.seed = seed;
				msg->unionStartGame.enemyClientId = otherClient->id;
				if (i == 0) logf("A game was started between %d and %d\n", client->id, otherClient->id);
			}
		}

		for (int i = 0; i < server->clientsNum; i++) {
			Client *client = &server->clients[i];
			client->timeSinceLastSeen += elapsed;
			client->timeSinceLastPing += elapsed;
			// logf("Client %d last seen: %f\n", client->id, client->timeSinceLastSeen);

			if (client->timeSinceLastPing > 1) {
				client->timeSinceLastPing = 0;
				// logf("Pinging client %d\n", client->id);
				newOutMsg(client, NET_MSG_PING);
			}

			if (client->timeSinceLastSeen > 60) {
				logf("Client %d was spliced for inactivity (%f)\n", client->id, client->timeSinceLastSeen);
				if (client->syncString) free(client->syncString);
				client->syncString = NULL;

				arraySpliceIndex(server->clients, server->clientsNum, sizeof(Client), i);
				server->clientsNum--;
				i--;
				continue;
			}
		}
	}

	if (!platform->isCommandLineOnly) {
		clearRenderer();

		ImGui::Begin("Server", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("Connected clients:");
		for (int i = 0; i < server->clientsNum; i++) {
			Client *client = &server->clients[i];
			ImGui::Text("Id: %d", client->id);
			ImGui::Text("Mode: %s", clientModeStrings[client->mode]);
			ImGui::Text("Time since last seen: %f", client->timeSinceLastSeen);
		}

		ImGui::End();

		drawOnScreenLog();
	}
}

NetMsg *newOutMsg(Client *client, NetMsgType type) {
	if (client->outMsgsNum > MSGS_MAX-1) {
		logf("Too many outMsgs on client\n");
		return NULL;
	}

	NetMsg *msg = &client->outMsgs[client->outMsgsNum++];
	memset(msg, 0, sizeof(NetMsg));
	msg->type = type;
	return msg;
}

NetMsg *newOutMsgFromClone(Client *client, NetMsg *origMsg) {
	if (client->outMsgsNum > MSGS_MAX-1) {
		logf("Too many outMsgs on client\n");
		return NULL;
	}

	NetMsg *newMsg = &client->outMsgs[client->outMsgsNum++];
	memcpy(newMsg, origMsg, sizeof(NetMsg));
	return newMsg;
}

Client *getClient(int id) {
	for (int i = 0; i < server->clientsNum; i++) {
		Client *client = &server->clients[i];
		if (client->id == id) return client;
	}

	return NULL;
}

void serverCleanup() {
	logf("RIP server\n");
}
