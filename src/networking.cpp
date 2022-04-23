#if defined(_WIN32)
# pragma comment (lib, "ws2_32.lib")
# include "WS2tcpip.h"
// # include <curl/curl.h>
#endif

#if defined(__linux__)
# include "sys/socket.h"
# include <netdb.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <fcntl.h>
# define SOCKET int
# define INVALID_SOCKET (-1)
# define SOCKET_ERROR (-1)
#endif

#if defined(__EMSCRIPTEN__)
# include <netdb.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <fcntl.h>
# define SOCKET int
# define INVALID_SOCKET (-1)
# define SOCKET_ERROR (-1)
# include <emscripten/websocket.h>

EM_BOOL webSocketOpen(int eventType, const EmscriptenWebSocketOpenEvent *e, void *userData);
EM_BOOL webSocketClose(int eventType, const EmscriptenWebSocketCloseEvent *e, void *userData);
EM_BOOL webSocketError(int eventType, const EmscriptenWebSocketErrorEvent *e, void *userData);
EM_BOOL webSocketMessage(int eventType, const EmscriptenWebSocketMessageEvent *e, void *userData);
#endif

enum SocketlingStatus {
	SOCKETLING_NONE,
	SOCKETLING_GOOD,
	SOCKETLING_ZERO_RECEIVE,
	SOCKETLING_CONNECTION_RESET,
	SOCKETLING_WOULD_BLOCK,
	SOCKETLING_PENDING,
	SOCKETLING_ERROR,
};
struct Socket {
	SOCKET socket;
	int packetSize;

	bool isTcp;
	int webSocket;

	bool isPending;

	sockaddr_storage selfAddr;
	sockaddr_storage srcAddr;

	char clientHostName[NI_MAXHOST] = {};
	char clientService[NI_MAXSERV] = {};

	ThreadSafeQueue *webSocketReceiveQueue;
	void *receiveBuffer;
};

struct NetworkingSystem {
	int x;
#if defined(_WIN32)
	// CURL *curl;
#endif
};
NetworkingSystem *netSys = NULL;

bool initNetworking();
Socket *createConnectedSenderSocket(const char *ip, int port);

SocketlingStatus socketSend(Socket *socket, void *data);
SocketlingStatus socketSend(Socket *socket, void *data, sockaddr_storage addr);
SocketlingStatus socketReceive(Socket *socket);
bool socketClose(Socket *socket);
void deinitNetworking();

sockaddr_storage createSockaddr(const char *ip, int port);
void getSockaddrIpAndPort(sockaddr_storage *addr, char *ip, int *port);

Socket *createUdpSocket(int packetSize);
Socket *createTcpSocket(int packetSize);
bool socketBind(Socket *socket, sockaddr_storage addr);
bool socketSetReuseaddr(Socket *socket);
bool socketSetNonblock(Socket *socket);
bool lastSocketErrorWasWouldBlock();
bool lastSocketErrorWasConnReset();
bool lastSocketErrorWasBrokenPipe();

bool initNetworking() {
	// netSys = (NetworkingSystem *)zalloc(sizeof(NetworkingSystem));

#if defined(_WIN32)
	WSAData wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) return false;

	// netSys->curl = curl_easy_init();
	// if (!netSys->curl) {
	// 	logf("No curl!\n");
	// }
#endif

#if defined(__EMSCRIPTEN__)
	if (!emscripten_websocket_is_supported()) {
		logf("WebSockets are not supported, cannot continue!\n");
		return false;
	}
#endif

	return true;
}

Socket *createUdpSocket(int packetSize) {
	Socket *sock = (Socket *)zalloc(sizeof(Socket));
	sock->packetSize = packetSize;
	sock->receiveBuffer = malloc(sock->packetSize);

	sock->socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock->socket == 0 || sock->socket == INVALID_SOCKET) {
		free(sock);
		return NULL;
	}

	return sock;
}

Socket *createTcpSocket(int packetSize) {
	Socket *sock = (Socket *)zalloc(sizeof(Socket));
	sock->packetSize = packetSize;
	sock->receiveBuffer = malloc(sock->packetSize);

	sock->isTcp = true;

#if defined(__EMSCRIPTEN__)
	sock->isPending = true;
	sock->webSocketReceiveQueue = createThreadSafeQueue(sock->packetSize, 256, false);
#else
	sock->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock->socket == 0 || sock->socket == INVALID_SOCKET) {
		free(sock);
		return NULL;
	}
#endif

	return sock;
}

bool socketBind(Socket *socket, sockaddr_storage addr) {
	socket->selfAddr = addr;

#if defined(__EMSCRIPTEN__)
	return true;
#endif

	int result = bind(socket->socket, (sockaddr *)&socket->selfAddr, sizeof(socket->selfAddr));
	if (result == SOCKET_ERROR) return false;

	return true;
}

bool socketSetReuseaddr(Socket *socket) {
#if defined(__EMSCRIPTEN__)
	return true;
#endif

	int iOptVal = 1;
	int result = setsockopt(socket->socket, SOL_SOCKET, SO_REUSEADDR, (char *)&iOptVal, sizeof(iOptVal));
	if (result == SOCKET_ERROR) return false;

	return true;
}

bool socketSetNonblock(Socket *socket) {
#if defined(__EMSCRIPTEN__)
	return true;
#endif

#if defined(_WIN32)
	unsigned long ioctlValue = 1;
	int result = ioctlsocket(socket->socket, FIONBIO, &ioctlValue);
	if (result == SOCKET_ERROR) return false;
#else
	int flags = fcntl(socket->socket, F_GETFL, 0);
	if (flags == -1) return false;
	int result = fcntl(socket->socket, F_SETFL, flags | O_NONBLOCK);
	if (result == -1) return false;
#endif

	return true;
}

bool lastSocketErrorWasWouldBlock() {
#if defined(_WIN32)
	if (WSAGetLastError() == WSAEWOULDBLOCK) return true;
#else
	if (errno == EWOULDBLOCK) return true;
#endif

	return false;
}

bool lastSocketErrorWasConnReset() {
#if defined(_WIN32)
	if (WSAGetLastError() == WSAECONNRESET) return true;
#else
	if (errno == ECONNRESET) return true;
#endif

	return false;
}

bool lastSocketErrorWasBrokenPipe() {
	if (errno == EPIPE) return true;
	return false;
}

Socket *socketListen(Socket *listener) {
	int result = listen(listener->socket, 8);
	if (result == SOCKET_ERROR) return NULL;

	sockaddr_storage acceptAddr;
	socklen_t acceptAddrSize = sizeof(acceptAddr);
	int acceptResult = accept(listener->socket, (sockaddr *)&acceptAddr, &acceptAddrSize);
	if (acceptResult == INVALID_SOCKET) {
		return NULL;
	}

	logf("Here %d %d\n", result, acceptResult);
	Socket *clientSocket = (Socket *)zalloc(sizeof(Socket));
	clientSocket->socket = acceptResult;
	clientSocket->selfAddr = acceptAddr;
	clientSocket->packetSize = listener->packetSize;
	clientSocket->receiveBuffer = malloc(clientSocket->packetSize);

	clientSocket->isTcp = true;

	return clientSocket;
}

bool socketConnect(Socket *connector, sockaddr_storage addr);
bool socketConnect(Socket *connector, sockaddr_storage addr) {
#if defined(__EMSCRIPTEN__)
	EmscriptenWebSocketCreateAttributes createAttribs;
	emscripten_websocket_init_create_attributes(&createAttribs);

	char ip[17] = {};
	int port = 0;
	getSockaddrIpAndPort(&addr, ip, &port);

	createAttribs.url = mallocSprintf("ws://%s:%d", ip, port);
	createAttribs.createOnMainThread = false;
	logf("Web socket connecting to: %s\n", createAttribs.url);

	EMSCRIPTEN_WEBSOCKET_T webSocket = emscripten_websocket_new(&createAttribs);
	if (webSocket <= 0) {
		logf("WebSocket creation failed, error code %d!\n", (EMSCRIPTEN_RESULT)webSocket);
		return false;
	}

	connector->webSocket = webSocket;

	emscripten_websocket_set_onopen_callback(connector->webSocket, connector, webSocketOpen);
	emscripten_websocket_set_onclose_callback(connector->webSocket, connector, webSocketClose);
	emscripten_websocket_set_onerror_callback(connector->webSocket, connector, webSocketError);
	emscripten_websocket_set_onmessage_callback(connector->webSocket, connector, webSocketMessage);

	return true;
#endif

	int result = connect(connector->socket, (sockaddr *)&addr, sizeof(addr));
	if (result == SOCKET_ERROR) return false;

	return true;
}

SocketlingStatus socketReceive(Socket *socket) {
	if (socket->isPending) {
		logf("Sockets should probably not be pending\n");
		return SOCKETLING_PENDING;
	}
	memset(socket->receiveBuffer, 0, socket->packetSize);

#if defined(__EMSCRIPTEN__)
	if (!threadSafeQueueShift(socket->webSocketReceiveQueue, socket->receiveBuffer)) {
		return SOCKETLING_WOULD_BLOCK;
	}

	// logf("Pop(%x): %d\n", socket, socket->webSocketReceiveQueue->currentIndex);
	return SOCKETLING_GOOD;
#endif

	int result;
	if (socket->isTcp) {
		result = recv(socket->socket, (char *)socket->receiveBuffer, socket->packetSize, 0);
	} else {
		socklen_t clientAddrSize = sizeof(socket->srcAddr);
		result = recvfrom(socket->socket, (char *)socket->receiveBuffer, socket->packetSize, 0, (sockaddr *)&socket->srcAddr, &clientAddrSize);
	}
	if (result == 0) return SOCKETLING_ZERO_RECEIVE;
	if (result == -1) {
		if (lastSocketErrorWasWouldBlock()) return SOCKETLING_WOULD_BLOCK;
		if (lastSocketErrorWasConnReset()) return SOCKETLING_CONNECTION_RESET;
		return SOCKETLING_ERROR;
	}

	if (result < socket->packetSize) logf("Recved too little data\n"); //@incomplete This could just be considered blocking

	return SOCKETLING_GOOD;
}

SocketlingStatus socketSend(Socket *socket, void *data) {
	if (!socket->isTcp) Panic("Udp with no address");

	sockaddr_storage dummy = {};
	return socketSend(socket, data, dummy);
}

SocketlingStatus socketSend(Socket *socket, void *data, sockaddr_storage addr) {
	if (socket->isPending) {
		logf("Sockets should probably not be pending\n");
		return SOCKETLING_PENDING;
	}

#if defined(__EMSCRIPTEN__)
	EMSCRIPTEN_RESULT emResult = emscripten_websocket_send_binary(socket->webSocket, data, socket->packetSize);
	if (emResult < 0) {
		logf("emscripten_websocket_send_binary failed\n");
		return SOCKETLING_ERROR;
	}

	return SOCKETLING_GOOD;
#endif

#if _WIN32
	int flags = 0;
#else
	int flags = MSG_NOSIGNAL;
#endif

	int result;
	if (socket->isTcp) {
		result = send(socket->socket, (char *)data, socket->packetSize, flags);
	} else {
		result = sendto(socket->socket, (char *)data, socket->packetSize, flags, (sockaddr *)&addr, sizeof(addr));
	}
	if (result < socket->packetSize) {
		if (lastSocketErrorWasWouldBlock()) return SOCKETLING_WOULD_BLOCK;
		if (lastSocketErrorWasConnReset()) return SOCKETLING_CONNECTION_RESET;
		if (lastSocketErrorWasBrokenPipe()) return SOCKETLING_CONNECTION_RESET;
		return SOCKETLING_ERROR;
	}

	// {
	// 	int optlen = sizeof(int);
	// 	int optval;
	// 	getsockopt(socket->socket, SOL_SOCKET, SO_MAX_MSG_SIZE, (char *)&optval, &optlen);
	// 	logf("Size would be %d\n", optval);
	// }

	return SOCKETLING_GOOD;
}

sockaddr_storage createSockaddr(const char *ip, int port) {
	sockaddr_storage addr = {};
	sockaddr_in *inAddr = (sockaddr_in *)&addr;
	inAddr->sin_family = AF_INET;

	if (ip) {
		inet_pton(AF_INET, ip, &inAddr->sin_addr);
	} else {
		inAddr->sin_addr.s_addr = htonl(INADDR_ANY);
	}

	inAddr->sin_port = htons(port);
	return addr;
}

char *getSocketIp(Socket *socket);
char *getSocketIp(Socket *socket) {
	if (!socket) return "";
	sockaddr_storage addr = {};
	socklen_t addrSize = sizeof(addr);
	int result = getsockname(socket->socket, (sockaddr *)&addr, &addrSize);
	if (result == SOCKET_ERROR) return NULL;

	char *tempStr = inet_ntoa(((sockaddr_in *)&addr)->sin_addr);
	return tempStr;
}

int getSocketPort(Socket *socket);
int getSocketPort(Socket *socket) {
	if (!socket) return 0;
	sockaddr_storage addr = {};
	socklen_t addrSize = sizeof(addr);
	int result = getsockname(socket->socket, (sockaddr *)&addr, &addrSize);
	if (result == SOCKET_ERROR) return -1;

	int port = ntohs(((sockaddr_in *)&addr)->sin_port);
	return port;
}


char *getPeerIp(Socket *socket);
char *getPeerIp(Socket *socket) {
	sockaddr_storage addr = {};
	socklen_t addrSize = sizeof(addr);
	int result = getpeername(socket->socket, (sockaddr *)&addr, &addrSize);
	if (result == SOCKET_ERROR) return NULL;

	char *tempStr = inet_ntoa(((sockaddr_in *)&addr)->sin_addr);
	return tempStr;
}

int getPeerPort(Socket *socket);
int getPeerPort(Socket *socket) {
	sockaddr_storage addr = {};
	socklen_t addrSize = sizeof(addr);
	int result = getpeername(socket->socket, (sockaddr *)&addr, &addrSize);
	if (result == SOCKET_ERROR) return -1;

	int port = ntohs(((sockaddr_in *)&addr)->sin_port);
	return port;
}

int getSockaddrPort(sockaddr_storage addr);
int getSockaddrPort(sockaddr_storage addr) {
	int port = ntohs(((sockaddr_in *)&addr)->sin_port);
	return port;
}

void getSockaddrIpAndPort(sockaddr_storage *addr, char *ip, int *port) {
	// unsigned char ipChars[4];
	// ipChars[0] = addr->sin_addr.S_un.S_un_b.s_b1;
	// ipChars[1] = addr->sin_addr.S_un.S_un_b.s_b2;
	// ipChars[2] = addr->sin_addr.S_un.S_un_b.s_b3;
	// ipChars[3] = addr->sin_addr.S_un.S_un_b.s_b4;
	// char *tempStr = frameSprintf("%d.%d.%d.%d", ipChars[0], ipChars[1], ipChars[2], ipChars[3]);

	sockaddr_in *inAddr = (sockaddr_in *)addr;
	char *tempStr = inet_ntoa(inAddr->sin_addr);

	strcpy(ip, tempStr);

	*port = ntohs(inAddr->sin_port);
}

void printSocketInfo(Socket *socket);
void printSocketInfo(Socket *socket) {
	sockaddr_storage addr = {};
	socklen_t addrSize = sizeof(addr);
	int iResult = getsockname(socket->socket, (sockaddr *)&addr, &addrSize);
	if (iResult == SOCKET_ERROR) {
		logLastOSError();
	}

	char ip[17] = {};
	int port = 0;
	getSockaddrIpAndPort(&addr, ip, &port);
	logf("%s:%d\n", ip, port);
}

u32 getIp(sockaddr_storage addr) {
	sockaddr_in *inAddr = (sockaddr_in *)&addr;
	u32 ip = inAddr->sin_addr.s_addr;
	return ip;
}

#if defined(__EMSCRIPTEN__)
EM_BOOL webSocketOpen(int eventType, const EmscriptenWebSocketOpenEvent *e, void *userData);
EM_BOOL webSocketClose(int eventType, const EmscriptenWebSocketCloseEvent *e, void *userData);
EM_BOOL webSocketError(int eventType, const EmscriptenWebSocketErrorEvent *e, void *userData);
EM_BOOL webSocketMessage(int eventType, const EmscriptenWebSocketMessageEvent *e, void *userData);

EM_BOOL webSocketOpen(int eventType, const EmscriptenWebSocketOpenEvent *e, void *userData) {
	Socket *socket = (Socket *)userData;
	socket->isPending = false;
	return 0;
}

EM_BOOL webSocketClose(int eventType, const EmscriptenWebSocketCloseEvent *e, void *userData) {
	logf("close(eventType=%d, wasClean=%d, code=%d, reason=%s, userData=%d)\n", eventType, e->wasClean, e->code, e->reason, (int)userData);
	return 0;
}

EM_BOOL webSocketError(int eventType, const EmscriptenWebSocketErrorEvent *e, void *userData) {
	logf("error(eventType=%d, userData=%d)\n", eventType, (int)userData);
	return 0;
}

EM_BOOL webSocketMessage(int eventType, const EmscriptenWebSocketMessageEvent *e, void *userData) {
	Socket *socket = (Socket *)userData;

	// logf("message(eventType=%d, userData=%d, data=%p, numBytes=%d, isText=%d)\n", eventType, (int)userData, e->data, e->numBytes, e->isText);
	if (e->isText) {
		logf("text data: \"%s\"\n", e->data);
		return 0;
	}

	if (e->numBytes != socket->packetSize) {
		logf("Got a weird amount of data %d instead of %d\n", e->numBytes, socket->packetSize);
		return 0;
	}

	if (!threadSafeQueuePush(socket->webSocketReceiveQueue, e->data)) {
		logf("Session incoming queue is overloaded\n");
	}

	// logf("Push(%x): %d\n", socket, socket->webSocketReceiveQueue->currentIndex);
	return 0;
}
#endif

bool socketClose(Socket *socket) {
#if defined(_WIN32)
	if (closesocket(socket->socket) == SOCKET_ERROR) return false;
#elif defined(__EMSCRIPTEN__)
	emscripten_websocket_close(socket->webSocket, 4001, "C++ said so");
	emscripten_websocket_delete(socket->webSocket);
#else
	if (close(socket->socket) == -1) return false;
#endif

	free(socket);
	return true;
}

void deinitNetworking() {
#if defined(_WIN32)
	WSACleanup();
#endif
}
