enum PlatformKey {
	KEY_BACKTICK=96,
	KEY_LEFT=400, KEY_RIGHT, KEY_UP, KEY_DOWN,
	KEY_TAB, KEY_BACKSPACE,
	KEY_ESC, KEY_ENTER,
	KEY_SHIFT, KEY_RIGHT_SHIFT,
	KEY_CTRL, KEY_RIGHT_CTRL,
	KEY_ALT, KEY_RIGHT_ALT,
	KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
	MOUSE_LEFT, MOUSE_RIGHT,
	KEY_INSERT, KEY_DELETE, 
	KEY_HOME, KEY_END, 
	KEY_PAGE_UP, KEY_PAGE_DOWN, 
  KEYS_MAX,
};

enum PlatformEventType {
	PLATFORM_EVENT_KEY_DOWN,
	PLATFORM_EVENT_KEY_UP,
	PLATFORM_EVENT_MOUSE_MOVE,
	PLATFORM_EVENT_MOUSE_WHEEL,
};
struct PlatformEvent {
	PlatformEventType type;

  Vec2 position;
	int keyCode;
	int wheelValue;
};

struct BackendNanoTime;

void backendPlatformInit(int windowWidth, int windowHeight, char *windowTitle);
float backendPlatformGetWindowScaling();
void backendPlatformStartUpdateLoop(void (*platformFrontendUpdateCallback)());
void backendPlatformExit();

int backendPlatformGetWindowWidth();
int backendPlatformGetWindowHeight();

void backendPlatformMaximizeWindow();
void backendPlatformMinimizeWindow();
void backendPlatformRestoreWindow();
void backendPlatformResizeWindow(int width, int height);
void backendHideCursor();
void backendShowCursor();


void backendPlatformSleep(int ms);
BackendNanoTime backendPlatformGetNanoTime();
float backendPlatformGetMsPassed(BackendNanoTime startTime);

void backendPlatformSetClipboard(char *str);
void backendPlatformNavigateToUrl(char *url);

void backendPlatformShowErrorWindow(char *msg);
char *backendPlatformGetLastErrorMessage();
int backendPlatformGetMemoryUsage();

void backendPlatformImGuiInit();
void backendPlatformImGuiStartFrame(int windowWidth, int windowHeight);
void backendPlatformImGuiDraw();

#define PLATFORM_EVENTS_MAX 128
PlatformEvent _platformEvents[PLATFORM_EVENTS_MAX];
int _platformEventsNum;

int _keysThatImGuiCaresAbout[] = {
	KEY_LEFT, KEY_RIGHT, KEY_UP, KEY_DOWN, KEY_BACKSPACE, ' ', KEY_ENTER, KEY_ESC, KEY_CTRL, KEY_SHIFT, KEY_ALT, KEY_RIGHT_CTRL, KEY_RIGHT_SHIFT, KEY_RIGHT_ALT,
	KEY_TAB, KEY_INSERT, KEY_DELETE, KEY_HOME, KEY_END, KEY_PAGE_UP, KEY_PAGE_DOWN, 
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
	'\'', ',', '-', '.', '/', ';', '=', '[', '\\', ']', '`',
};
