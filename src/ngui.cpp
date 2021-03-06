enum NguiStyleType {
	NGUI_STYLE_WINDOW_POSITION=0,
	NGUI_STYLE_WINDOW_PIVOT=1,
	NGUI_STYLE_WINDOW_SIZE=2,
	NGUI_STYLE_WINDOW_PADDING=3,
	NGUI_STYLE_WINDOW_LERP_SPEED=4,
	NGUI_STYLE_ELEMENT_DISABLED=5,
	NGUI_STYLE_ELEMENT_DISABLED_TINT=6,
	NGUI_STYLE_ELEMENT_PADDING=7,
	NGUI_STYLE_ELEMENTS_IN_ROW=8,
	NGUI_STYLE_ELEMENT_SIZE=9,
	NGUI_STYLE_BUTTON_LABEL_GRAVITY=10,
	NGUI_STYLE_BUTTON_HOVER_OFFSET=11,
	NGUI_STYLE_WINDOW_BG_COLOR=12,
	NGUI_STYLE_BG_COLOR=13,
	NGUI_STYLE_FG_COLOR=14,
	NGUI_STYLE_HOVER_TINT=15,
	NGUI_STYLE_ACTIVE_TINT=16,
	NGUI_STYLE_ACTIVE_FLASH_BRIGHTNESS=17,
	NGUI_STYLE_TEXT_COLOR=18,
	NGUI_STYLE_INDENT=19,
	NGUI_STYLE_ICON_PTR=20,
	NGUI_STYLE_ICON_ROTATION=21,
	NGUI_STYLE_ICON_SCALE=22,
	NGUI_STYLE_ICON_TRANSLATION=23,
	NGUI_STYLE_ICON_ALPHA=24,
	NGUI_STYLE_ICON_GRAVITY=25,
	NGUI_STYLE_HIGHLIGHT_TINT=26,
	NGUI_STYLE_HIGHLIGHT_CRUSH=27,
	NGUI_STYLE_HOVER_SOUND_PATH_PTR=28,
	NGUI_STYLE_ACTIVE_SOUND_PATH_PTR=29,
	NGUI_STYLE_BUTTON_HOVER_SCALE=30,
	NGUI_STYLE_TYPES_MAX=31,
};

enum NguiLayout {
	NGUI_LAYOUT_VERTICAL,
	NGUI_LAYOUT_GRID,
};

enum NguiDataType {
	NGUI_DATA_TYPE_INT,
	NGUI_DATA_TYPE_COLOR_INT,
	NGUI_DATA_TYPE_FLOAT,
	NGUI_DATA_TYPE_VEC2,
	NGUI_DATA_TYPE_STRING_PTR,
	NGUI_DATA_TYPE_PTR,
};

struct NguiStyleTypeInfo {
	char *enumName;
	char *name;
	NguiDataType dataType;
};

struct NguiStyleVar {
	NguiStyleType type;
#define NGUI_STYLE_VAR_DATA_SIZE (sizeof(Vec4))
	char data[NGUI_STYLE_VAR_DATA_SIZE];
};

struct NguiStyleStack {
	NguiStyleVar *vars;
	int varsNum;
	int varsMax;
};

enum NguiElementType {
	NGUI_ELEMENT_WINDOW,
	NGUI_ELEMENT_BUTTON,
	NGUI_ELEMENT_SLIDER,
};
struct NguiElement {
	NguiElementType type;

#define NGUI_ELEMENT_NAME_MAX_LEN 64
	char name[NGUI_ELEMENT_NAME_MAX_LEN];
	float alive;
	int id;
	int parentId;

	int orderIndex;
	NguiStyleStack styleStack;
	char *subText;
	void *valuePtr;
	float valueMin;
	float valueMax;

	Vec2 position;
	Vec2 size; // Only used for windows right now
	Xform2 graphicsXform;
	int bgColor;
	int fgColor;

	bool active;
	bool justActive;

	float creationTime;
	float hoveringTime;
	float timeSinceLastClicked;

	Rect childRect;
};

struct Ngui {
	Font *defaultFont;
	Vec2 mouse;
	float time;

	NguiElement *elements;
	int elementsNum;
	int elementsMax;

	Rect lastWindowRect;
	NguiElement *lastElement;

	int nextNguiElementId;
	int currentOrderIndex;

	int draggingId;

	float uiScale;

	NguiStyleTypeInfo styleTypeInfos[NGUI_STYLE_TYPES_MAX];

	NguiStyleStack globalStyleStack;
	NguiStyleStack *currentStyleStack;

	int currentParentId;
};
Ngui *ngui = NULL;

void nguiInit();

void nguiPushStyleOfType(NguiStyleStack *styleStack, NguiStyleType type, NguiDataType dataType, void *ptr);
void nguiPushStyleInt(NguiStyleType type, int value);
void nguiPushStyleColorInt(NguiStyleType type, int value);
void nguiPushStyleColorTintInt(NguiStyleType type, int value);
void nguiPushStyleFloat(NguiStyleType type, float value);
void nguiPushStyleVec2(NguiStyleType type, Vec2 value);
void nguiPushStyleStringPtr(NguiStyleType type, char *value);
void nguiPushStylePtr(NguiStyleType type, void *value);
void nguiPushStyleIconXform(Xform2 xform);
void nguiPushWindowPositionAndPivot(Vec2 position, Vec2 pivot);
void nguiPushStyleStack(NguiStyleStack *styleStack);

//@speed These could be a lot faster if the elements didn't have randomly ordered styleStacks like the global styleStacks.
//       Elements could have a different kind of style stack that's a fixed size index by the NguiStyleType
void nguiGetStyleOfType(NguiStyleStack *styleStack, NguiStyleType type, NguiDataType dataType, void *ptr);
int nguiGetStyleInt(NguiStyleType type) {
	int ret;
	nguiGetStyleOfType(ngui->currentStyleStack, type, NGUI_DATA_TYPE_INT, &ret);
	return ret;
}
int nguiGetStyleColorInt(NguiStyleType type) {
	int ret;
	nguiGetStyleOfType(ngui->currentStyleStack, type, NGUI_DATA_TYPE_COLOR_INT, &ret); 
	return ret;
}
float nguiGetStyleFloat(NguiStyleType type) {
	float ret;
	nguiGetStyleOfType(ngui->currentStyleStack, type, NGUI_DATA_TYPE_FLOAT, &ret);
	return ret;
}
Vec2 nguiGetStyleVec2(NguiStyleType type) {
	Vec2 ret;
	nguiGetStyleOfType(ngui->currentStyleStack, type, NGUI_DATA_TYPE_VEC2, &ret);
	return ret;
}
char *nguiGetStyleStringPtr(NguiStyleType type) {
	char *ret;
	nguiGetStyleOfType(ngui->currentStyleStack, type, NGUI_DATA_TYPE_STRING_PTR, &ret);
	return ret;
}
void *nguiGetStylePtr(NguiStyleType type) {
	void *ret;
	nguiGetStyleOfType(ngui->currentStyleStack, type, NGUI_DATA_TYPE_PTR, &ret);
	return ret;
}
Xform2 nguiGetStyleIconXform() {
	Xform2 ret;
	ret.rotation = nguiGetStyleFloat(NGUI_STYLE_ICON_ROTATION);
	ret.scale = nguiGetStyleVec2(NGUI_STYLE_ICON_SCALE);
	ret.translation = nguiGetStyleVec2(NGUI_STYLE_ICON_TRANSLATION);
	return ret;
}

void nguiPopAnyStyleVar(int amount=1);
void nguiPopStyleVar(NguiStyleType type);
void nguiPopStyleIconXform();
void nguiPopWindowPositionAndPivot();
void nguiPopStyleStack(NguiStyleStack *styleStack);

void copyStyleVar(NguiStyleStack *dest, NguiStyleStack *src, NguiStyleType type);

void nguiDraw(float elapsed);

NguiElement *getAndReviveNguiElement(char *name);
NguiElement *getNguiElementById(int id);
int getNguiId(int parentId, char *name);

void nguiSetNextWindowSize(Vec2 size);
void nguiStartWindow(char *name, int flags = 0);
void nguiEndWindow();
bool nguiButton(char *name, char *subText="");
bool nguiSlider(char *name, float *value, float min=0, float max=1);

bool nguiHoveringElement();

int getSizeForDataType(NguiDataType dataType);
void nguiShowImGuiStyleEditor(NguiStyleStack *styleStack);

void writeNguiStyleStack(DataStream *stream, NguiStyleStack *styleStack);
NguiStyleStack readNguiStyleStack(DataStream *stream);

/// FUNCTIONS ^

void nguiInit() {
	ngui = (Ngui *)zalloc(sizeof(Ngui));
	ngui->defaultFont = createFont("assets/common/arial.ttf", 80);
	ngui->uiScale = platform->windowScaling;

	ngui->elementsMax = 128;
	ngui->elements = (NguiElement *)zalloc(sizeof(NguiElement) * ngui->elementsMax);

	ngui->globalStyleStack.varsMax = 1;
	ngui->globalStyleStack.vars = (NguiStyleVar *)zalloc(sizeof(NguiStyleVar) * ngui->globalStyleStack.varsMax);

	ngui->currentStyleStack = &ngui->globalStyleStack;

	NguiStyleTypeInfo *info;
	info = &ngui->styleTypeInfos[NGUI_STYLE_WINDOW_POSITION];
	info->enumName = "NGUI_STYLE_WINDOW_POSITION";
	info->name = "Window position";
	info->dataType = NGUI_DATA_TYPE_VEC2;

	info = &ngui->styleTypeInfos[NGUI_STYLE_WINDOW_PIVOT];
	info->enumName = "NGUI_STYLE_WINDOW_PIVOT";
	info->name = "Window pivot";
	info->dataType = NGUI_DATA_TYPE_VEC2;

	info = &ngui->styleTypeInfos[NGUI_STYLE_WINDOW_SIZE];
	info->enumName = "NGUI_STYLE_WINDOW_SIZE";
	info->name = "Window size";
	info->dataType = NGUI_DATA_TYPE_VEC2;

	info = &ngui->styleTypeInfos[NGUI_STYLE_WINDOW_PADDING];
	info->enumName = "NGUI_STYLE_WINDOW_PADDING";
	info->name = "Window padding";
	info->dataType = NGUI_DATA_TYPE_VEC2;

	info = &ngui->styleTypeInfos[NGUI_STYLE_WINDOW_LERP_SPEED];
	info->enumName = "NGUI_STYLE_WINDOW_LERP_SPEED";
	info->name = "Window lerp speed";
	info->dataType = NGUI_DATA_TYPE_FLOAT;

	info = &ngui->styleTypeInfos[NGUI_STYLE_ELEMENT_DISABLED];
	info->enumName = "NGUI_STYLE_ELEMENT_DISABLED";
	info->name = "Element disabled";
	info->dataType = NGUI_DATA_TYPE_INT;

	info = &ngui->styleTypeInfos[NGUI_STYLE_ELEMENT_DISABLED_TINT];
	info->enumName = "NGUI_STYLE_ELEMENT_DISABLED_TINT";
	info->name = "Element disabled tint";
	info->dataType = NGUI_DATA_TYPE_COLOR_INT;

	info = &ngui->styleTypeInfos[NGUI_STYLE_ELEMENT_PADDING];
	info->enumName = "NGUI_STYLE_ELEMENT_PADDING";
	info->name = "Element padding";
	info->dataType = NGUI_DATA_TYPE_VEC2;

	info = &ngui->styleTypeInfos[NGUI_STYLE_ELEMENTS_IN_ROW];
	info->enumName = "NGUI_STYLE_ELEMENTS_IN_ROW";
	info->name = "Elements in row";
	info->dataType = NGUI_DATA_TYPE_INT;

	info = &ngui->styleTypeInfos[NGUI_STYLE_ELEMENT_SIZE];
	info->enumName = "NGUI_STYLE_ELEMENT_SIZE";
	info->name = "Button size";
	info->dataType = NGUI_DATA_TYPE_VEC2;

	info = &ngui->styleTypeInfos[NGUI_STYLE_BUTTON_LABEL_GRAVITY];
	info->enumName = "NGUI_STYLE_BUTTON_LABEL_GRAVITY";
	info->name = "Button label gravity";
	info->dataType = NGUI_DATA_TYPE_VEC2;

	info = &ngui->styleTypeInfos[NGUI_STYLE_BUTTON_HOVER_OFFSET];
	info->enumName = "NGUI_STYLE_BUTTON_HOVER_OFFSET";
	info->name = "Hover button offset";
	info->dataType = NGUI_DATA_TYPE_VEC2;

	info = &ngui->styleTypeInfos[NGUI_STYLE_WINDOW_BG_COLOR];
	info->enumName = "NGUI_STYLE_WINDOW_BG_COLOR";
	info->name = "Window bg color";
	info->dataType = NGUI_DATA_TYPE_COLOR_INT;

	info = &ngui->styleTypeInfos[NGUI_STYLE_BG_COLOR];
	info->enumName = "NGUI_STYLE_BG_COLOR";
	info->name = "Bg color";
	info->dataType = NGUI_DATA_TYPE_COLOR_INT;

	info = &ngui->styleTypeInfos[NGUI_STYLE_FG_COLOR];
	info->enumName = "NGUI_STYLE_FG_COLOR";
	info->name = "Fg color";
	info->dataType = NGUI_DATA_TYPE_COLOR_INT;

	info = &ngui->styleTypeInfos[NGUI_STYLE_HOVER_TINT];
	info->enumName = "NGUI_STYLE_HOVER_TINT";
	info->name = "Hover tint";
	info->dataType = NGUI_DATA_TYPE_COLOR_INT;

	info = &ngui->styleTypeInfos[NGUI_STYLE_ACTIVE_TINT];
	info->enumName = "NGUI_STYLE_ACTIVE_TINT";
	info->name = "Active tint";
	info->dataType = NGUI_DATA_TYPE_COLOR_INT;

	info = &ngui->styleTypeInfos[NGUI_STYLE_ACTIVE_FLASH_BRIGHTNESS];
	info->enumName = "NGUI_STYLE_ACTIVE_FLASH_BRIGHTNESS";
	info->name = "Active flash brightness";
	info->dataType = NGUI_DATA_TYPE_FLOAT;

	info = &ngui->styleTypeInfos[NGUI_STYLE_TEXT_COLOR];
	info->enumName = "NGUI_STYLE_TEXT_COLOR";
	info->name = "Text color";
	info->dataType = NGUI_DATA_TYPE_COLOR_INT;

	info = &ngui->styleTypeInfos[NGUI_STYLE_INDENT];
	info->enumName = "NGUI_STYLE_INDENT";
	info->name = "Indent";
	info->dataType = NGUI_DATA_TYPE_FLOAT;

	info = &ngui->styleTypeInfos[NGUI_STYLE_ICON_PTR];
	info->enumName = "NGUI_STYLE_ICON_PTR";
	info->name = "Icon pointer";
	info->dataType = NGUI_DATA_TYPE_PTR;

	info = &ngui->styleTypeInfos[NGUI_STYLE_ICON_ROTATION];
	info->enumName = "NGUI_STYLE_ICON_ROTATION";
	info->name = "Icon rotation";
	info->dataType = NGUI_DATA_TYPE_FLOAT;

	info = &ngui->styleTypeInfos[NGUI_STYLE_ICON_SCALE];
	info->enumName = "NGUI_STYLE_ICON_SCALE";
	info->name = "Icon scale";
	info->dataType = NGUI_DATA_TYPE_VEC2;

	info = &ngui->styleTypeInfos[NGUI_STYLE_ICON_TRANSLATION];
	info->enumName = "NGUI_STYLE_ICON_TRANSLATION";
	info->name = "Icon translation";
	info->dataType = NGUI_DATA_TYPE_VEC2;

	info = &ngui->styleTypeInfos[NGUI_STYLE_ICON_ALPHA];
	info->enumName = "NGUI_STYLE_ICON_ALPHA";
	info->name = "Icon alpha";
	info->dataType = NGUI_DATA_TYPE_FLOAT;

	info = &ngui->styleTypeInfos[NGUI_STYLE_ICON_GRAVITY];
	info->enumName = "NGUI_STYLE_ICON_GRAVITY";
	info->name = "Icon gravity";
	info->dataType = NGUI_DATA_TYPE_VEC2;

	info = &ngui->styleTypeInfos[NGUI_STYLE_HIGHLIGHT_TINT];
	info->enumName = "NGUI_STYLE_HIGHLIGHT_TINT";
	info->name = "Highlight tint";
	info->dataType = NGUI_DATA_TYPE_COLOR_INT;

	info = &ngui->styleTypeInfos[NGUI_STYLE_HIGHLIGHT_CRUSH];
	info->enumName = "NGUI_STYLE_HIGHLIGHT_CRUSH";
	info->name = "Highlight crush";
	info->dataType = NGUI_DATA_TYPE_FLOAT;

	info = &ngui->styleTypeInfos[NGUI_STYLE_HOVER_SOUND_PATH_PTR];
	info->enumName = "NGUI_STYLE_HOVER_SOUND_PATH_PTR";
	info->name = "Hover sound path";
	info->dataType = NGUI_DATA_TYPE_STRING_PTR;

	info = &ngui->styleTypeInfos[NGUI_STYLE_ACTIVE_SOUND_PATH_PTR];
	info->enumName = "NGUI_STYLE_ACTIVE_SOUND_PATH_PTR";
	info->name = "Active sound path";
	info->dataType = NGUI_DATA_TYPE_STRING_PTR;

	info = &ngui->styleTypeInfos[NGUI_STYLE_BUTTON_HOVER_SCALE];
	info->enumName = "NGUI_STYLE_BUTTON_HOVER_SCALE";
	info->name = "Button hover scale";
	info->dataType = NGUI_DATA_TYPE_VEC2;

	nguiPushStyleVec2(NGUI_STYLE_WINDOW_POSITION, v2(0, 0));
	nguiPushStyleVec2(NGUI_STYLE_WINDOW_PIVOT, v2(0, 0));
	nguiPushStyleVec2(NGUI_STYLE_WINDOW_SIZE, v2(500, 500));
	nguiPushStyleVec2(NGUI_STYLE_WINDOW_PADDING, v2(2, 2));
	nguiPushStyleFloat(NGUI_STYLE_WINDOW_LERP_SPEED, 0.2);
	nguiPushStyleInt(NGUI_STYLE_ELEMENT_DISABLED, 0);
	nguiPushStyleColorInt(NGUI_STYLE_ELEMENT_DISABLED_TINT, 0x80000000);
	nguiPushStyleVec2(NGUI_STYLE_ELEMENT_PADDING, v2(5, 5));
	nguiPushStyleInt(NGUI_STYLE_ELEMENTS_IN_ROW, 1);
	nguiPushStyleVec2(NGUI_STYLE_ELEMENT_SIZE, v2(250, 80));
	nguiPushStyleVec2(NGUI_STYLE_BUTTON_LABEL_GRAVITY, v2(0, 0));
	nguiPushStyleVec2(NGUI_STYLE_BUTTON_HOVER_OFFSET, v2(20, 0));
	nguiPushStyleColorInt(NGUI_STYLE_WINDOW_BG_COLOR, 0xA0202020);
	nguiPushStyleColorInt(NGUI_STYLE_BG_COLOR, 0xFF202020);
	nguiPushStyleColorInt(NGUI_STYLE_FG_COLOR, 0xFF404040);
	nguiPushStyleColorInt(NGUI_STYLE_HOVER_TINT, 0x40FFFFFF);
	nguiPushStyleColorInt(NGUI_STYLE_ACTIVE_TINT, 0xA0FFFFFF);
	nguiPushStyleFloat(NGUI_STYLE_ACTIVE_FLASH_BRIGHTNESS, 0);
	nguiPushStyleColorInt(NGUI_STYLE_TEXT_COLOR, 0xFFECECEC);
	nguiPushStyleFloat(NGUI_STYLE_INDENT, 0);
	nguiPushStylePtr(NGUI_STYLE_ICON_PTR, 0);
	nguiPushStyleFloat(NGUI_STYLE_ICON_ROTATION, 0);
	nguiPushStyleVec2(NGUI_STYLE_ICON_SCALE, v2(1, 1));
	nguiPushStyleVec2(NGUI_STYLE_ICON_TRANSLATION, v2(0, 0));
	nguiPushStyleFloat(NGUI_STYLE_ICON_ALPHA, 0.25);
	nguiPushStyleVec2(NGUI_STYLE_ICON_GRAVITY, v2(1, 0.5));
	nguiPushStyleColorInt(NGUI_STYLE_HIGHLIGHT_TINT, 0x5C000000);
	nguiPushStyleFloat(NGUI_STYLE_HIGHLIGHT_CRUSH, 3.5);
	nguiPushStyleStringPtr(NGUI_STYLE_HOVER_SOUND_PATH_PTR, "assets/common/audio/tickEffect.ogg");
	nguiPushStyleStringPtr(NGUI_STYLE_ACTIVE_SOUND_PATH_PTR, "assets/common/audio/clickEffect.ogg");
	nguiPushStyleVec2(NGUI_STYLE_BUTTON_HOVER_SCALE, v2(1.01, 1.01));

	Sound *sound;
	sound = getSound("assets/common/audio/tickEffect.ogg");
	sound->tweakVolume = 0.1;
	sound = getSound("assets/common/audio/clickEffect.ogg");
	sound->tweakVolume = 0.1;
}

void nguiPushStyleOfType(NguiStyleStack *styleStack, NguiStyleType type, NguiDataType dataType, void *ptr) {
	if (!ptr) Panic("Null pointer given to nguiPushStyleOfType");

	NguiStyleTypeInfo styleTypeInfo = ngui->styleTypeInfos[type];
	if (styleTypeInfo.dataType != dataType) {
		Panic(frameSprintf(
			"Type mismatch on push ngui style type %d (got %d, expected %d)\n",
			type,
			dataType,
			styleTypeInfo.dataType
		));
	}

	if (styleStack->varsMax == 0) {
		styleStack->varsMax = 1;
		styleStack->vars = (NguiStyleVar *)zalloc(sizeof(NguiStyleVar) * styleStack->varsMax);
	}
	if (styleStack->varsNum > styleStack->varsMax-1) {
		styleStack->vars = (NguiStyleVar *)resizeArray(
			styleStack->vars,
			sizeof(NguiStyleVar),
			styleStack->varsNum,
			styleStack->varsMax*2
		);
		styleStack->varsMax *= 2;
	}

	NguiStyleVar *var = &styleStack->vars[styleStack->varsNum++];
	memset(var, 0, sizeof(NguiStyleVar));
	var->type = type;

	int size = getSizeForDataType(dataType);
	memcpy(var->data, ptr, size);
}

void nguiPushStyleInt(NguiStyleType type, int value) {
	nguiPushStyleOfType(ngui->currentStyleStack, type, NGUI_DATA_TYPE_INT, &value);
}
void nguiPushStyleColorInt(NguiStyleType type, int value) {
	nguiPushStyleOfType(ngui->currentStyleStack, type, NGUI_DATA_TYPE_COLOR_INT, &value);
}
void nguiPushStyleColorTintInt(NguiStyleType type, int value) {
	int current;
	nguiGetStyleOfType(ngui->currentStyleStack, type, NGUI_DATA_TYPE_COLOR_INT, &current);

	current = tintColor(current, value);
	nguiPushStyleOfType(ngui->currentStyleStack, type, NGUI_DATA_TYPE_COLOR_INT, &current);
}
void nguiPushStyleFloat(NguiStyleType type, float value) {
	nguiPushStyleOfType(ngui->currentStyleStack, type, NGUI_DATA_TYPE_FLOAT, &value);
}
void nguiPushStyleVec2(NguiStyleType type, Vec2 value) {
	nguiPushStyleOfType(ngui->currentStyleStack, type, NGUI_DATA_TYPE_VEC2, &value);
}
void nguiPushStyleStringPtr(NguiStyleType type, char *value) {
	nguiPushStyleOfType(ngui->currentStyleStack, type, NGUI_DATA_TYPE_STRING_PTR, &value);
}
void nguiPushStylePtr(NguiStyleType type, void *value) {
	nguiPushStyleOfType(ngui->currentStyleStack, type, NGUI_DATA_TYPE_PTR, &value);
}
void nguiPushStyleIconXform(Xform2 xform) {
	nguiPushStyleFloat(NGUI_STYLE_ICON_ROTATION, xform.rotation);
	nguiPushStyleVec2(NGUI_STYLE_ICON_SCALE, xform.scale);
	nguiPushStyleVec2(NGUI_STYLE_ICON_TRANSLATION, xform.translation);
}
void nguiPushWindowPositionAndPivot(Vec2 position, Vec2 pivot) {
	nguiPushStyleVec2(NGUI_STYLE_WINDOW_POSITION, position);
	nguiPushStyleVec2(NGUI_STYLE_WINDOW_PIVOT, pivot);
}

void nguiPushStyleStack(NguiStyleStack *styleStack) {
	for (int i = 0; i < styleStack->varsNum; i++) {
		NguiStyleVar *var = &styleStack->vars[i];
		NguiStyleTypeInfo *styleTypeInfo = &ngui->styleTypeInfos[var->type];

		char data[NGUI_STYLE_VAR_DATA_SIZE];
		nguiGetStyleOfType(styleStack, var->type, styleTypeInfo->dataType, data);
		nguiPushStyleOfType(&ngui->globalStyleStack, var->type, styleTypeInfo->dataType, data);
	}
}

void nguiGetStyleOfType(NguiStyleStack *styleStack, NguiStyleType type, NguiDataType dataType, void *ptr) {
	NguiStyleTypeInfo styleTypeInfo = ngui->styleTypeInfos[type];
	if (styleTypeInfo.dataType != dataType) {
		Panic(frameSprintf(
			"Type mismatch on get ngui style type %d (got %d, expected %d)\n",
			type,
			dataType,
			styleTypeInfo.dataType
		));
	}

	NguiStyleVar *srcVar = NULL;
	for (int i = styleStack->varsNum-1; i >= 0; i--) {
		NguiStyleVar *var = &styleStack->vars[i];
		if (var->type == type) {
			srcVar = var;
			break;
		}
	}

	if (!srcVar) Panic(frameSprintf("Failed to get style value for type %d\n", type));

	int size = getSizeForDataType(dataType);
	memcpy(ptr, srcVar->data, size);
}

void nguiPopStyleVar(NguiStyleType type) {
	if (ngui->currentStyleStack->varsNum < 1) Panic(frameSprintf("Tried to pop %d, but there were no more vars", type));

	NguiStyleType topStyleType = ngui->currentStyleStack->vars[ngui->currentStyleStack->varsNum-1].type;
	if (topStyleType != type) {
		NguiStyleTypeInfo *topInfo = &ngui->styleTypeInfos[topStyleType];
		NguiStyleTypeInfo *currentInfo = &ngui->styleTypeInfos[type];
		Panic(frameSprintf("Tried to pop %s, but next was %s\n", currentInfo->enumName, topInfo->enumName));
	}

	nguiPopAnyStyleVar();
}

void nguiPopStyleIconXform() {
	nguiPopStyleVar(NGUI_STYLE_ICON_TRANSLATION);
	nguiPopStyleVar(NGUI_STYLE_ICON_SCALE);
	nguiPopStyleVar(NGUI_STYLE_ICON_ROTATION);
}

void nguiPopWindowPositionAndPivot() {
	nguiPopStyleVar(NGUI_STYLE_WINDOW_PIVOT);
	nguiPopStyleVar(NGUI_STYLE_WINDOW_POSITION);
}

void nguiPopStyleStack(NguiStyleStack *styleStack) {
	for (int i = styleStack->varsNum-1; i >= 0; i--) {
		NguiStyleVar *var = &styleStack->vars[i];
		nguiPopStyleVar(var->type);
	}
}

void nguiPopAnyStyleVar(int amount) {
	ngui->currentStyleStack->varsNum -= amount;
	if (ngui->currentStyleStack->varsNum < 0) {
		logf("Ngui style stack underflow!!!!\n");
		ngui->currentStyleStack->varsNum = 0;
	}
}

void copyStyleVar(NguiStyleStack *dest, NguiStyleStack *src, NguiStyleType type) {
	NguiStyleTypeInfo *styleTypeInfo = &ngui->styleTypeInfos[type];
	char data[NGUI_STYLE_VAR_DATA_SIZE];
	nguiGetStyleOfType(src, type, styleTypeInfo->dataType, data);
	nguiPushStyleOfType(dest, type, styleTypeInfo->dataType, data);
};


void nguiDraw(float elapsed) {
	for (int i = 0; i < ngui->elementsNum; i++) { /// Splice dead elements
		NguiElement *element = &ngui->elements[i];
		if (element->alive <= 0) {
			if (element->styleStack.vars) free(element->styleStack.vars);
			arraySpliceIndex(ngui->elements, ngui->elementsNum, sizeof(NguiElement), i);
			i--;
			ngui->elementsNum--;
			continue;
		}
	} ///

	ngui->currentStyleStack = &ngui->globalStyleStack;

	NguiElement **elementsLeft = (NguiElement **)frameMalloc(sizeof(NguiElement *) * ngui->elementsMax);
	int elementsLeftNum = 0;
	for (int i = 0; i < ngui->elementsNum; i++) {
		NguiElement *element = &ngui->elements[i];
		element->active = false;
		element->justActive = false;
		elementsLeft[elementsLeftNum++] = element;
	}

	int lastLeftNum = elementsLeftNum;
	for (;;) {
		if (elementsLeftNum == 0) break;
		for (int i = 0; i < elementsLeftNum; i++) {
			int windowIndex = i;
			NguiElement *window = elementsLeft[windowIndex];
			if (window->type != NGUI_ELEMENT_WINDOW) continue;

			ngui->currentStyleStack = &window->styleStack; // @windowStyleStack This is a really weird hack, since windows are never children
			NguiElement **children = (NguiElement **)frameMalloc(sizeof(NguiElement *) * ngui->elementsMax);
			int childrenNum = 0;

			for (int i = 0; i < elementsLeftNum; i++) {
				NguiElement *otherElement = elementsLeft[i];
				if (otherElement->parentId == window->id) {
					children[childrenNum++] = otherElement;

					arraySpliceIndex(elementsLeft, elementsLeftNum, sizeof(NguiElement *), i);
					i--;
					elementsLeftNum--;
					continue;
				}
			}

			auto qsortChildrenByOrderIndex = [](const void *a, const void *b)->int {
				NguiElement *childA = *(NguiElement **)a;
				NguiElement *childB = *(NguiElement **)b;
				int orderDiff = childA->orderIndex - childB->orderIndex;
				return orderDiff;
			};

			qsort(children, childrenNum, sizeof(NguiElement *), qsortChildrenByOrderIndex);

			Vec2 prevCursor = v2();
			Vec2 cursor = v2();
			Vec2 childrenSize = v2();
			NguiElement *prevChild = NULL;
			int elementsInRow = 0;
			for (int i = 0; i < childrenNum; i++) { // Layout
				NguiElement *child = children[i];
				ngui->currentStyleStack = &child->styleStack;

				child->active = false;

				bool skipCursorBump = false;
				if (child->alive != 1) skipCursorBump = true;
				if (child->creationTime == 0) {
					if (prevChild && prevChild->creationTime == 0) {
						child->position = prevCursor;
						skipCursorBump = true;
					} else {
						child->position = cursor;
					}
				}

				Vec2 elementSize = nguiGetStyleVec2(NGUI_STYLE_ELEMENT_SIZE);
				Vec2 position = cursor;

				if (child->alive == 1) child->position = lerp(child->position, position, 0.05);
				Rect childRect = makeRect(child->position, elementSize) * ngui->uiScale;
				childRect.x += nguiGetStyleFloat(NGUI_STYLE_INDENT) * ngui->uiScale;

				child->childRect = childRect;

				childrenSize.x = MaxNum(childrenSize.x, childRect.x + childRect.width);
				childrenSize.y = MaxNum(childrenSize.y, childRect.y + childRect.height);

				Vec2 elementPadding = nguiGetStyleVec2(NGUI_STYLE_ELEMENT_PADDING) * ngui->uiScale;

				elementsInRow++;
				if (!skipCursorBump) {
					prevCursor = cursor;
					if (elementsInRow < nguiGetStyleInt(NGUI_STYLE_ELEMENTS_IN_ROW)) {
						cursor.x += elementSize.x + elementPadding.x;
					} else {
						cursor.x = 0;
						cursor.y += elementSize.y + elementPadding.y;
						elementsInRow = 0;
					}

				}
				prevChild = child;
			}

			ngui->currentStyleStack = &window->styleStack; // @windowStyleStack

			Vec2 windowPadding = nguiGetStyleVec2(NGUI_STYLE_WINDOW_PADDING) * ngui->uiScale;
			// windowPosition is not multiplied by ngui->uiScale because it's given by the user as screen coords
			Vec2 windowPosition = nguiGetStyleVec2(NGUI_STYLE_WINDOW_POSITION);
			Vec2 windowSize = childrenSize;
			windowSize.x += windowPadding.x*2;
			windowSize.y += windowPadding.y*2;
			windowPosition -= windowSize * nguiGetStyleVec2(NGUI_STYLE_WINDOW_PIVOT);

			float windowLerpSpeed = nguiGetStyleFloat(NGUI_STYLE_WINDOW_LERP_SPEED);

			if (window->creationTime <= 0.02) { // I have to do this for 2 frames for some reason
				window->position = windowPosition;
				window->size = windowSize;
			}
			window->position = lerp(window->position, windowPosition, windowLerpSpeed);
			window->size = lerp(window->size, windowSize, windowLerpSpeed);

			Rect windowRect = makeRect(window->position, window->size);

			drawRect(windowRect, nguiGetStyleColorInt(NGUI_STYLE_WINDOW_BG_COLOR));

			for (int i = 0; i < childrenNum; i++) { // Window position
				NguiElement *child = children[i];
				child->childRect.x += windowRect.x + windowPadding.x;
				child->childRect.y += windowRect.y + windowPadding.y;
			}

			for (int i = 0; i < childrenNum; i++) { // Update
				NguiElement *child = children[i];
				ngui->currentStyleStack = &child->styleStack;

				char *label = child->name;
				char *tripleHash = strstr(label, "###");
				if (tripleHash) {
					char *newLabel = frameStringClone(label);
					*strstr(newLabel, "###") = 0;
					label = newLabel;
				}

				float alpha = 1;
				alpha *= child->alive;
				pushAlpha(alpha);

				Rect childRect = child->childRect;

				int bgColor = nguiGetStyleColorInt(NGUI_STYLE_BG_COLOR);
				int fgColor = nguiGetStyleColorInt(NGUI_STYLE_FG_COLOR);
				int hoverTint = nguiGetStyleColorInt(NGUI_STYLE_HOVER_TINT);
				int activeTint = nguiGetStyleColorInt(NGUI_STYLE_ACTIVE_TINT);
				Vec2 labelGravity = nguiGetStyleVec2(NGUI_STYLE_BUTTON_LABEL_GRAVITY);
				int labelTextColor = nguiGetStyleColorInt(NGUI_STYLE_TEXT_COLOR);
				bool disabled = nguiGetStyleInt(NGUI_STYLE_ELEMENT_DISABLED);
				int disabledTint = nguiGetStyleColorInt(NGUI_STYLE_ELEMENT_DISABLED_TINT);

				if (disabled) {
					bgColor = tintColor(bgColor, disabledTint);
					fgColor = tintColor(fgColor, disabledTint);
					labelTextColor = tintColor(labelTextColor, disabledTint);
				}

				if (child->bgColor == 0) child->bgColor = bgColor;
				child->bgColor = lerpColor(child->bgColor, bgColor, 0.1);

				if (child->fgColor == 0) child->fgColor = fgColor;
				child->fgColor = lerpColor(child->fgColor, fgColor, 0.1);

				auto drawElementBg = [](NguiElement *child, Rect rect)->void {
					{
						RenderProps props = newRenderProps();
						props.tint = child->bgColor;
						props.matrix.TRANSLATE(getPosition(rect));
						props.matrix.SCALE(getSize(rect));
						drawTexture(renderer->whiteTexture, props);
					}

					RenderProps props = newRenderProps();
					props.matrix.TRANSLATE(rect.x, rect.y);
					props.matrix.SCALE(rect.width, rect.height);
					props.uvMatrix.SCALE(nguiGetStyleFloat(NGUI_STYLE_HIGHLIGHT_CRUSH));
					props.uv0 = v2(0, 1);
					props.uv1 = v2(1, 0);
					props.srcWidth = props.srcHeight = 1;

					int highlightColor=tintColor(child->bgColor, nguiGetStyleColorInt(NGUI_STYLE_HIGHLIGHT_TINT));
					props.tint = highlightColor;
					drawTexture(renderer->linearGrad256, props);
				};

				if (child->type == NGUI_ELEMENT_BUTTON) {
					Xform2 graphicsXform = createXform2();

					if (contains(childRect, ngui->mouse) && !disabled) {
						if (child->hoveringTime == 0) {
							playSound(getSound(nguiGetStyleStringPtr(NGUI_STYLE_HOVER_SOUND_PATH_PTR)));
						}

						bgColor = tintColor(bgColor, hoverTint);
						if (platform->mouseJustDown) {
							playSound(getSound(nguiGetStyleStringPtr(NGUI_STYLE_ACTIVE_SOUND_PATH_PTR)));
							child->bgColor = tintColor(child->bgColor, activeTint);

							child->timeSinceLastClicked = 0.001;
							child->justActive = true;
						}

						graphicsXform.translation = nguiGetStyleVec2(NGUI_STYLE_BUTTON_HOVER_OFFSET) * ngui->uiScale;
						graphicsXform.scale = nguiGetStyleVec2(NGUI_STYLE_BUTTON_HOVER_SCALE);
						child->hoveringTime += elapsed;
					} else {
						child->hoveringTime = 0;
					}

					float modByForFlash = 0.3;
					if (child->timeSinceLastClicked > 0 && child->timeSinceLastClicked < modByForFlash*2) {
						float moddedValue = fmod(child->timeSinceLastClicked, modByForFlash);
						float perc = moddedValue / modByForFlash;
						int activeTint = nguiGetStyleColorInt(NGUI_STYLE_ACTIVE_TINT);
						float flashBrightness = nguiGetStyleFloat(NGUI_STYLE_ACTIVE_FLASH_BRIGHTNESS);
						if (perc > 0.75) bgColor = lerpColor(bgColor, activeTint, flashBrightness);
					}

					child->graphicsXform = lerp(child->graphicsXform, graphicsXform, 0.05);

					Rect graphicsRect = childRect;
					graphicsRect.x += child->graphicsXform.translation.x;
					graphicsRect.y += child->graphicsXform.translation.y;
					graphicsRect = inflatePerc(graphicsRect, child->graphicsXform.scale - 1);

					drawElementBg(child, graphicsRect);

					Texture *iconTexture = (Texture *)nguiGetStylePtr(NGUI_STYLE_ICON_PTR);
					if (iconTexture) {
						Vec2 iconGravity = nguiGetStyleVec2(NGUI_STYLE_ICON_GRAVITY);
						Rect iconRect = getInnerRectOfAspect(graphicsRect, getSize(iconTexture), iconGravity);
						setScissor(iconRect);

						Matrix3 matrix = mat3();
						matrix.TRANSLATE(iconRect.x, iconRect.y);
						matrix.TRANSLATE(getSize(iconRect)/2);
						matrix.SCALE(nguiGetStyleVec2(NGUI_STYLE_ICON_SCALE));
						matrix.ROTATE(nguiGetStyleFloat(NGUI_STYLE_ICON_ROTATION));
						matrix.TRANSLATE(nguiGetStyleVec2(NGUI_STYLE_ICON_TRANSLATION));
						matrix.TRANSLATE(-getSize(iconRect)/2);
						matrix.SCALE(iconRect.width, iconRect.height);

						float alpha = nguiGetStyleFloat(NGUI_STYLE_ICON_ALPHA);
						drawSimpleTexture(iconTexture, matrix, v2(0, 0), v2(1, 1), alpha);
						clearScissor();
					}

					{
						Rect textRect = getInnerRectOfSize(graphicsRect, getSize(graphicsRect)*v2(1, 0.85), v2(0, 0));
						DrawTextProps props = newDrawTextProps(ngui->defaultFont, labelTextColor);
						drawTextInRect(label, props, textRect, labelGravity);
					}

					if (child->subText[0]) {
						int subTextColor = nguiGetStyleColorInt(NGUI_STYLE_TEXT_COLOR);
						subTextColor = lerpColor(subTextColor, 0x00FFFFFF&subTextColor, 0.25);

						Rect subTextRect = getInnerRectOfSize(graphicsRect, getSize(graphicsRect)*v2(0.8, 0.3), v2(1, 1));
						DrawTextProps props = newDrawTextProps(ngui->defaultFont, subTextColor);
						drawTextInRect(child->subText, props, subTextRect, v2(1, 1));
					}
				} else if (child->type == NGUI_ELEMENT_SLIDER) {
					Rect graphicsRect = childRect;

					drawElementBg(child, graphicsRect);

					{
						Rect textRect = getInnerRectOfSize(graphicsRect, getSize(graphicsRect)*v2(1, 0.6), v2(0, 0));
						DrawTextProps props = newDrawTextProps(ngui->defaultFont, labelTextColor);
						drawTextInRect(label, props, textRect, labelGravity);
					}

					Rect barRect = getInnerRectOfSize(graphicsRect, getSize(graphicsRect)*v2(0.8, 0.2), v2(0.5, 0.8));
					drawRect(barRect, child->fgColor);

					float perc = norm(child->valueMin, child->valueMax, *(float *)child->valuePtr);
					Rect buttonRect = makeRect(0, 0, barRect.height*1.5, barRect.height*1.5);
					buttonRect.x = barRect.x + barRect.width*perc - buttonRect.width/2;
					buttonRect.y = barRect.y + barRect.height/2 - buttonRect.height/2;
					// drawRect(buttonRect, labelTextColor);

					Circle buttonCircle = makeCircle(getCenter(buttonRect), buttonRect.width/2);
					drawCircle(buttonCircle, labelTextColor);

					if (contains(buttonCircle, ngui->mouse) || contains(barRect, ngui->mouse)) {
						if (platform->mouseJustDown) ngui->draggingId = child->id;
					}

					if (ngui->draggingId == child->id) {
						float newPerc = Clamp01(norm(barRect.x, barRect.x + barRect.width, ngui->mouse.x));
						*(float *)child->valuePtr = lerp(child->valueMin, child->valueMax, newPerc);
					}
				}

				popAlpha();
			}

			ngui->currentStyleStack = &window->styleStack; // @windowStyleStack
			ngui->lastWindowRect = windowRect;

			arraySpliceIndex(elementsLeft, elementsLeftNum, sizeof(NguiElement *), windowIndex);
			i--;
			elementsLeftNum--;
			continue;
		}

		if (lastLeftNum == elementsLeftNum) {
			logf("Couldn't make progress updaing NguiElements?\n");
			break;
		}
	}
	ngui->currentStyleStack = &ngui->globalStyleStack;

	for (int i = 0; i < ngui->elementsNum; i++) {
		NguiElement *element = &ngui->elements[i];
		if (element->timeSinceLastClicked > 0) element->timeSinceLastClicked += elapsed;
		element->creationTime += elapsed;
		element->alive -= 0.05;
	}

	if (!platform->mouseDown) ngui->draggingId = 0;
	ngui->time += elapsed;
}

NguiElement *getAndReviveNguiElement(char *name) {
	NguiElement *element = NULL;

	for (int i = 0; i < ngui->elementsNum; i++) {
		NguiElement *possibleElement = &ngui->elements[i];
		if (streq(possibleElement->name, name) && possibleElement->parentId == ngui->currentParentId) {
			element = possibleElement;
			break;
		}
	}

	if (!element) {
		if (ngui->elementsNum > ngui->elementsMax-1) {
			logf("Too many NguiElements!!!\n");
			ngui->elementsNum--;

			// I need a bucket-array here to avoid shuffling memory and breaking all my pointers here:
			// ngui->elements = (NguiElement *)resizeArray(ngui->elements, sizeof(NguiElement), ngui->elementsNum, ngui->elementsMax*2);
			// ngui->elementsMax *= 2;
		}

		element = &ngui->elements[ngui->elementsNum++];
		memset(element, 0, sizeof(NguiElement));
		strncpy(element->name, name, NGUI_ELEMENT_NAME_MAX_LEN);
		element->id = ++ngui->nextNguiElementId;
		element->styleStack.varsMax = NGUI_STYLE_TYPES_MAX;
		element->styleStack.vars = (NguiStyleVar *)zalloc(sizeof(NguiStyleVar) * element->styleStack.varsMax);
	}

	element->alive = 1;
	element->orderIndex = ngui->currentOrderIndex++;
	element->parentId = ngui->currentParentId;

	element->styleStack.varsNum = 0;
	for (int i = 0; i < NGUI_STYLE_TYPES_MAX; i++) {
		copyStyleVar(&element->styleStack, &ngui->globalStyleStack, (NguiStyleType)i);
	}

	ngui->lastElement = element;
	return element;
}

NguiElement *getNguiElementById(int id) {
	for (int i = 0; i < ngui->elementsNum; i++) {
		NguiElement *element = &ngui->elements[i];
		if (element->id == id) return element;
	}

	return NULL;
}

int getElementIndex(NguiElement *element) {
	for (int i = 0; i < ngui->elementsNum; i++) {
		NguiElement *possibleElement = &ngui->elements[i];
		if (possibleElement == element) return i;
	}

	return 0;
}

void nguiStartWindow(char *name, int flags) {
	NguiElement *element = getAndReviveNguiElement(name);
	element->type = NGUI_ELEMENT_WINDOW;
	ngui->currentParentId = element->id;
	ngui->currentOrderIndex = 0;
}

void nguiEndWindow() {
	NguiElement *window = getNguiElementById(ngui->currentParentId);
	if (window) {
		ngui->lastWindowRect = makeRect(window->position, window->size);
	}

	ngui->currentParentId = 0;
}

bool nguiButton(char *name, char *subText) {
	NguiElement *element = getAndReviveNguiElement(name);
	element->type = NGUI_ELEMENT_BUTTON;
	element->subText = subText;
	return element->justActive;
}

bool nguiSlider(char *name, float *value, float min, float max) {
	NguiElement *element = getAndReviveNguiElement(name);
	element->type = NGUI_ELEMENT_SLIDER;
	element->valuePtr = value;
	element->valueMin = min;
	element->valueMax = max;
	return element->justActive;
}

bool nguiHoveringElement() {
	if (!ngui->lastElement) return false;
	if (ngui->lastElement->hoveringTime != 0) return true;
	return false;
}

int getSizeForDataType(NguiDataType dataType) {
	int size = 0;
	if (dataType == NGUI_DATA_TYPE_INT) size = sizeof(int);
	if (dataType == NGUI_DATA_TYPE_COLOR_INT) size = sizeof(int);
	if (dataType == NGUI_DATA_TYPE_FLOAT) size = sizeof(float);
	if (dataType == NGUI_DATA_TYPE_VEC2) size = sizeof(Vec2);
	if (dataType == NGUI_DATA_TYPE_STRING_PTR) size = sizeof(char *);
	if (dataType == NGUI_DATA_TYPE_PTR) size = sizeof(void *);
	if (!size) Panic(frameSprintf("Invalid size for ngui data type %d?", dataType));
	return size;
}

void nguiShowImGuiStyleEditor(NguiStyleStack *styleStack) {
	for (int i = 0; i < styleStack->varsNum; i++) {
		ImGui::PushID(i);
		NguiStyleVar *var = &styleStack->vars[i];
		NguiStyleTypeInfo *styleTypeInfo = &ngui->styleTypeInfos[var->type];

		if (ImGui::ArrowButton("moveUp", ImGuiDir_Up)) {
			if (i > 0) {
				arraySwap(styleStack->vars, styleStack->varsNum, sizeof(NguiStyleVar), i, i-1);
				ImGui::PopID();
				continue;
			}
		}
		ImGui::SameLine();

		if (ImGui::ArrowButton("moveDown", ImGuiDir_Down)) {
			if (i < styleStack->varsNum-1) {
				arraySwap(styleStack->vars, styleStack->varsNum, sizeof(NguiStyleVar), i, i+1);
				ImGui::PopID();
				continue;
			}
		}
		ImGui::SameLine();

		bool shouldSpliceVar = false;
		guiPushStyleColor(ImGuiCol_Button, 0xFF900000);
		if (ImGui::Button("X")) shouldSpliceVar = true;
		guiPopStyleColor();

		char **styleTypesList = (char **)frameMalloc(sizeof(char *) * NGUI_STYLE_TYPES_MAX);
		int styleTypesListNum = 0;
		for (int i = 0; i < NGUI_STYLE_TYPES_MAX; i++) {
			styleTypesList[styleTypesListNum++] = ngui->styleTypeInfos[i].enumName;
		}
		ImGui::SameLine();
		if (ImGui::Combo("###varType", (int *)&var->type, styleTypesList, NGUI_STYLE_TYPES_MAX, 20)) {
			styleTypeInfo = &ngui->styleTypeInfos[var->type];
			nguiGetStyleOfType(&ngui->globalStyleStack, var->type, styleTypeInfo->dataType, var->data); 
		}
		ImGui::SameLine();

		if (styleTypeInfo->dataType == NGUI_DATA_TYPE_INT) {
			ImGui::InputInt(frameSprintf("###%s", styleTypeInfo->name), (int *)var->data);
		} else if (styleTypeInfo->dataType == NGUI_DATA_TYPE_COLOR_INT) {
			guiInputArgb(frameSprintf("###%s", styleTypeInfo->name), (int *)var->data);
		} else if (styleTypeInfo->dataType == NGUI_DATA_TYPE_FLOAT) {
			ImGui::DragFloat(frameSprintf("###%s", styleTypeInfo->name), (float *)var->data, 0.01);
		} else if (styleTypeInfo->dataType == NGUI_DATA_TYPE_VEC2) {
			ImGui::DragFloat2(frameSprintf("###%s", styleTypeInfo->name), (float *)var->data, 0.01);
		} else if (styleTypeInfo->dataType == NGUI_DATA_TYPE_STRING_PTR) {
			ImGui::Text("%s: %s", styleTypeInfo->name, *(char **)var->data);
		}

		ImGui::PopID();

		if (shouldSpliceVar) {
			arraySpliceIndex(styleStack->vars, styleStack->varsNum, sizeof(NguiStyleVar), i);
			i--;
			styleStack->varsNum--;
			continue;
		}
	}

	if (ImGui::Button("Add style var")) {
		int value = 0;
		nguiPushStyleOfType(styleStack, NGUI_STYLE_WINDOW_BG_COLOR, NGUI_DATA_TYPE_COLOR_INT, &value);
	}
}

void writeNguiStyleStack(DataStream *stream, NguiStyleStack styleStack) {
	writeU32(stream, styleStack.varsNum);
	for (int i = 0; i < styleStack.varsNum; i++) {
		NguiStyleVar *var = &styleStack.vars[i];
		writeU32(stream, var->type);
		writeBytes(stream, var->data, sizeof(Vec4));
	}
}

NguiStyleStack readNguiStyleStack(DataStream *stream) {
	NguiStyleStack styleStack = {};
	styleStack.varsMax = styleStack.varsNum = readU32(stream);
	styleStack.vars = (NguiStyleVar *)zalloc(sizeof(NguiStyleVar) * styleStack.varsMax);
	for (int i = 0; i < styleStack.varsNum; i++) {
		NguiStyleVar *var = &styleStack.vars[i];
		var->type = (NguiStyleType)readU32(stream);
		readBytes(stream, var->data, sizeof(Vec4));
	}

	return styleStack;
}


