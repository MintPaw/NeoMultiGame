enum NguiStyleType {
	NGUI_STYLE_WINDOW_POSITION,
	NGUI_STYLE_WINDOW_PIVOT,
	NGUI_STYLE_WINDOW_SIZE,
	NGUI_STYLE_WINDOW_PADDING,
	NGUI_STYLE_ELEMENT_PADDING,
	NGUI_STYLE_ELEMENTS_IN_ROW,
	NGUI_STYLE_ELEMENT_SIZE,
	NGUI_STYLE_BUTTON_LABEL_GRAVITY,
	NGUI_STYLE_BUTTON_HOVER_OFFSET,
	NGUI_STYLE_WINDOW_BG_COLOR,
	NGUI_STYLE_FG_COLOR,
	NGUI_STYLE_HOVER_TINT,
	NGUI_STYLE_ACTIVE_TINT,
	NGUI_STYLE_ACTIVE_FLASH_BRIGHTNESS,
	NGUI_STYLE_TEXT_COLOR,
	NGUI_STYLE_INDENT,
	NGUI_STYLE_ICON_PATH_PTR,
	NGUI_STYLE_ICON_ROTATION,
	NGUI_STYLE_ICON_SCALE,
	NGUI_STYLE_ICON_TRANSLATION,
	NGUI_STYLE_ICON_ALPHA,
	NGUI_STYLE_ICON_GRAVITY,
	NGUI_STYLE_HIGHLIGHT_TINT,
	NGUI_STYLE_HIGHLIGHT_CRUSH,
	NGUI_STYLE_HOVER_SOUND_PATH_PTR,
	NGUI_STYLE_ACTIVE_SOUND_PATH_PTR,
	NGUI_STYLE_TYPES_MAX,
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

	Vec2 position;
	Vec2 graphicsOffset;
	int fgColor;

	bool active;
	bool justActive;

	float creationTime;
	float hoveringTime;
	float timeSinceLastClicked;

	Rect drawRect;
};

struct Ngui {
	Font *defaultFont;
	Vec2 mouse;
	float time;

	NguiElement *elements;
	int elementsNum;
	int elementsMax;

	int nextNguiElementId;
	int currentOrderIndex;

	float uiScale;

	NguiStyleTypeInfo styleTypeInfos[NGUI_STYLE_TYPES_MAX];

	NguiStyleStack globalStyleStack;
	NguiStyleStack *currentStyleStack;

	NguiElement *currentWindow;
};
Ngui *ngui = NULL;

void nguiInit();

void nguiPushStyleOfType(NguiStyleStack *styleStack, NguiStyleType type, NguiDataType dataType, void *ptr);
void nguiPushStyleInt(NguiStyleType type, int value) {
	nguiPushStyleOfType(ngui->currentStyleStack, type, NGUI_DATA_TYPE_INT, &value);
}
void nguiPushStyleColorInt(NguiStyleType type, int value) {
	nguiPushStyleOfType(ngui->currentStyleStack, type, NGUI_DATA_TYPE_COLOR_INT, &value);
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
void nguiPushStyleIconXform(Xform2 xform) {
	nguiPushStyleFloat(NGUI_STYLE_ICON_ROTATION, xform.rotation);
	nguiPushStyleVec2(NGUI_STYLE_ICON_SCALE, xform.scale);
	nguiPushStyleVec2(NGUI_STYLE_ICON_TRANSLATION, xform.translation);
}

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
void copyStyleVar(NguiStyleStack *dest, NguiStyleStack *src, NguiStyleType type);

void nguiDraw(float elapsed);

NguiElement *getNguiElement(char *name);
int getNguiId(int parentId, char *name);

void nguiSetNextWindowSize(Vec2 size);
void nguiStartWindow(char *name, int flags = 0);
void nguiEndWindow();
bool nguiButton(char *name, char *subText="");

int getSizeForDataType(NguiDataType dataType);
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

	info = &ngui->styleTypeInfos[NGUI_STYLE_ICON_PATH_PTR];
	info->enumName = "NGUI_STYLE_ICON_PATH_PTR";
	info->name = "Icon path pointer";
	info->dataType = NGUI_DATA_TYPE_STRING_PTR;

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

	nguiPushStyleVec2(NGUI_STYLE_WINDOW_POSITION, v2(0, 0));
	nguiPushStyleVec2(NGUI_STYLE_WINDOW_PIVOT, v2(0, 0));
	nguiPushStyleVec2(NGUI_STYLE_WINDOW_SIZE, v2(500, 500));
	nguiPushStyleVec2(NGUI_STYLE_WINDOW_PADDING, v2(2, 2));
	nguiPushStyleVec2(NGUI_STYLE_ELEMENT_PADDING, v2(5, 5));
	nguiPushStyleInt(NGUI_STYLE_ELEMENTS_IN_ROW, 1);
	nguiPushStyleVec2(NGUI_STYLE_ELEMENT_SIZE, v2(250, 80));
	nguiPushStyleVec2(NGUI_STYLE_BUTTON_LABEL_GRAVITY, v2(0, 0));
	nguiPushStyleVec2(NGUI_STYLE_BUTTON_HOVER_OFFSET, v2(20, 0));
	nguiPushStyleColorInt(NGUI_STYLE_WINDOW_BG_COLOR, 0xA0202020);
	nguiPushStyleColorInt(NGUI_STYLE_FG_COLOR, 0xFF353535);
	nguiPushStyleColorInt(NGUI_STYLE_HOVER_TINT, 0x40FFFFFF);
	nguiPushStyleColorInt(NGUI_STYLE_ACTIVE_TINT, 0xA0FFFFFF);
	nguiPushStyleFloat(NGUI_STYLE_ACTIVE_FLASH_BRIGHTNESS, 0);
	nguiPushStyleColorInt(NGUI_STYLE_TEXT_COLOR, 0xFFECECEC);
	nguiPushStyleFloat(NGUI_STYLE_INDENT, 0);
	nguiPushStyleStringPtr(NGUI_STYLE_ICON_PATH_PTR, "");
	nguiPushStyleFloat(NGUI_STYLE_ICON_ROTATION, 0);
	nguiPushStyleVec2(NGUI_STYLE_ICON_SCALE, v2(1, 1));
	nguiPushStyleVec2(NGUI_STYLE_ICON_TRANSLATION, v2(0, 0));
	nguiPushStyleFloat(NGUI_STYLE_ICON_ALPHA, 0.25);
	nguiPushStyleVec2(NGUI_STYLE_ICON_GRAVITY, v2(1, 0.5));
	nguiPushStyleColorInt(NGUI_STYLE_HIGHLIGHT_TINT, 0x5C000000);
	nguiPushStyleFloat(NGUI_STYLE_HIGHLIGHT_CRUSH, 3.5);
	nguiPushStyleStringPtr(NGUI_STYLE_HOVER_SOUND_PATH_PTR, "assets/common/audio/tickEffect.ogg");
	nguiPushStyleStringPtr(NGUI_STYLE_ACTIVE_SOUND_PATH_PTR, "assets/common/audio/clickEffect.ogg");
}

void nguiPushStyleOfType(NguiStyleStack *styleStack, NguiStyleType type, NguiDataType dataType, void *ptr) {
	NguiStyleTypeInfo styleTypeInfo = ngui->styleTypeInfos[type];
	if (styleTypeInfo.dataType != dataType) {
		logf("Type mismatch on push ngui style type %d (got %d, expected %d)\n", type, dataType, styleTypeInfo.dataType);
		return;
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
	for (int i = 0; i < ngui->elementsNum; i++) {
		NguiElement *element = &ngui->elements[i];
		if (element->alive <= 0) {
			if (element->styleStack.vars) free(element->styleStack.vars);
			arraySpliceIndex(ngui->elements, ngui->elementsNum, sizeof(NguiElement), i);
			i--;
			ngui->elementsNum--;
			continue;
		}
	}

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
		if (elementsLeftNum > 20) {
			int k = 5;
		}
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
				if (child->alive == 1) child->position = lerp(child->position, cursor, 0.05);
				Vec2 buttonSize = nguiGetStyleVec2(NGUI_STYLE_ELEMENT_SIZE);
				Rect childRect = makeRect(child->position, buttonSize) * ngui->uiScale;
				childRect.x += nguiGetStyleFloat(NGUI_STYLE_INDENT) * ngui->uiScale;

				child->drawRect = childRect;

				childrenSize.x = MaxNum(childrenSize.x, childRect.x + childRect.width);
				childrenSize.y = MaxNum(childrenSize.y, childRect.y + childRect.height);

				Vec2 elementPadding = nguiGetStyleVec2(NGUI_STYLE_ELEMENT_PADDING) * ngui->uiScale;

				elementsInRow++;
				if (!skipCursorBump) {
					prevCursor = cursor;
					if (elementsInRow < nguiGetStyleInt(NGUI_STYLE_ELEMENTS_IN_ROW)) {
						cursor.x += buttonSize.x + elementPadding.x;
					} else {
						cursor.x = 0;
						cursor.y += buttonSize.y + elementPadding.y;
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
			Rect windowRect = makeRect(windowPosition, windowSize);

			drawRect(windowRect, nguiGetStyleColorInt(NGUI_STYLE_WINDOW_BG_COLOR));

			for (int i = 0; i < childrenNum; i++) { // Window position
				NguiElement *child = children[i];
				child->drawRect.x += windowRect.x + windowPadding.x;
				child->drawRect.y += windowRect.y + windowPadding.y;
			}

			for (int i = 0; i < childrenNum; i++) { // Update
				NguiElement *child = children[i];
				ngui->currentStyleStack = &child->styleStack;

				char *label = child->name;
				float alpha = 1;
				alpha *= child->alive;
				pushAlpha(alpha);

				Rect childRect = child->drawRect;

				if (child->type == NGUI_ELEMENT_BUTTON) {
					Vec2 graphicsOffset = v2();
					int fgColor = nguiGetStyleColorInt(NGUI_STYLE_FG_COLOR);

					if (contains(childRect, ngui->mouse)) {
						if (child->hoveringTime == 0) playSound(getSound(nguiGetStyleStringPtr(NGUI_STYLE_HOVER_SOUND_PATH_PTR)));

						int hoverTint = nguiGetStyleColorInt(NGUI_STYLE_HOVER_TINT);
						fgColor = lerpColor(fgColor, hoverTint | 0xFF000000, getAofArgb(hoverTint) / 255.0);
						if (platform->mouseJustDown) {
							playSound(getSound(nguiGetStyleStringPtr(NGUI_STYLE_ACTIVE_SOUND_PATH_PTR)));
							int activeTint = nguiGetStyleColorInt(NGUI_STYLE_ACTIVE_TINT);
							child->fgColor = lerpColor(child->fgColor, activeTint | 0xFF000000, getAofArgb(activeTint) / 255.0);

							child->timeSinceLastClicked = 0.001;
							child->justActive = true;
						}

						graphicsOffset = nguiGetStyleVec2(NGUI_STYLE_BUTTON_HOVER_OFFSET) * ngui->uiScale;
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
						if (perc > 0.75) fgColor = lerpColor(fgColor, activeTint, flashBrightness);
					}

					if (child->fgColor == 0) child->fgColor = fgColor;
					child->fgColor = lerpColor(child->fgColor, fgColor, 0.1);

					child->graphicsOffset = lerp(child->graphicsOffset, graphicsOffset, 0.05);

					Rect graphicsRect = childRect;
					graphicsRect.x += child->graphicsOffset.x;
					graphicsRect.y += child->graphicsOffset.y;

					{
						drawRect(graphicsRect, child->fgColor);

						float highlightCrush = nguiGetStyleFloat(NGUI_STYLE_HIGHLIGHT_CRUSH);

						RenderProps props = newRenderProps();
						props.matrix.TRANSLATE(graphicsRect.x, graphicsRect.y);
						props.matrix.SCALE(graphicsRect.width, graphicsRect.height);
						props.uvMatrix.SCALE(highlightCrush);
						props.uv0 = v2(0, 1);
						props.uv1 = v2(1, 0);
						props.srcWidth = props.srcHeight = 1;

						int highlightTint = nguiGetStyleColorInt(NGUI_STYLE_HIGHLIGHT_TINT);
						int highlightColor = lerpColor(child->fgColor, 0xFF000000 | highlightTint, getAofArgb(highlightTint)/255.0);
						props.tint = highlightColor;
						drawTexture(renderer->linearGrad256, props);
					}

					char *iconPath = nguiGetStyleStringPtr(NGUI_STYLE_ICON_PATH_PTR);
					if (iconPath[0]) {
						Vec2 iconGravity = nguiGetStyleVec2(NGUI_STYLE_ICON_GRAVITY);
						Rect iconRect = getInnerRectOfSize(graphicsRect, v2(graphicsRect.height, graphicsRect.height), iconGravity);
						Texture *iconTexture = getTexture(iconPath);
						if (iconTexture) {
							setScissor(iconRect);
							Matrix3 matrix = mat3();
							matrix.TRANSLATE(iconRect.x, iconRect.y);
							matrix.SCALE(iconRect.width, iconRect.height);
							matrix.TRANSLATE(0.5, 0.5);
							matrix.ROTATE(nguiGetStyleFloat(NGUI_STYLE_ICON_ROTATION));
							matrix.SCALE(nguiGetStyleVec2(NGUI_STYLE_ICON_SCALE));
							matrix.TRANSLATE(nguiGetStyleVec2(NGUI_STYLE_ICON_TRANSLATION));
							matrix.TRANSLATE(-0.5, -0.5);
							float alpha = nguiGetStyleFloat(NGUI_STYLE_ICON_ALPHA);
							drawSimpleTexture(iconTexture, matrix, v2(0, 0), v2(1, 1), alpha);
							clearScissor();
						} else {
							drawRect(iconRect, 0xFFFF0000);
						}
					}

					{
						Vec2 labelGravity = nguiGetStyleVec2(NGUI_STYLE_BUTTON_LABEL_GRAVITY);
						int textColor = nguiGetStyleColorInt(NGUI_STYLE_TEXT_COLOR);
						Rect textRect = getInnerRectOfSize(graphicsRect, getSize(graphicsRect)*v2(1, 0.85), v2(0, 0));
						DrawTextProps props = newDrawTextProps(ngui->defaultFont, textColor);
						drawTextInRect(label, props, textRect, labelGravity);
					}

					if (child->subText[0]) {
						int subTextColor = nguiGetStyleColorInt(NGUI_STYLE_TEXT_COLOR);
						subTextColor = lerpColor(subTextColor, 0x00FFFFFF&subTextColor, 0.25);

						Rect subTextRect = getInnerRectOfSize(graphicsRect, getSize(graphicsRect)*v2(0.8, 0.3), v2(1, 1));
						DrawTextProps props = newDrawTextProps(ngui->defaultFont, subTextColor);
						drawTextInRect(child->subText, props, subTextRect, v2(1, 1));
					}
				}

				popAlpha();
			}

			ngui->currentStyleStack = &window->styleStack; // @windowStyleStack

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

	ngui->time += elapsed;
}

NguiElement *getNguiElement(char *name) {
	NguiElement *element = NULL;

	for (int i = 0; i < ngui->elementsNum; i++) {
		NguiElement *possibleElement = &ngui->elements[i];
		if (streq(possibleElement->name, name)) {
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
	element->parentId = ngui->currentWindow ? ngui->currentWindow->id : 0;

	element->styleStack.varsNum = 0;
	for (int i = 0; i < NGUI_STYLE_TYPES_MAX; i++) {
		copyStyleVar(&element->styleStack, &ngui->globalStyleStack, (NguiStyleType)i);
	}

	return element;
}

int getElementIndex(NguiElement *element) {
	for (int i = 0; i < ngui->elementsNum; i++) {
		NguiElement *possibleElement = &ngui->elements[i];
		if (possibleElement == element) return i;
	}

	return 0;
}

void nguiStartWindow(char *name, int flags) {
	NguiElement *element = getNguiElement(name);
	element->type = NGUI_ELEMENT_WINDOW;
	ngui->currentWindow = element;
	ngui->currentOrderIndex = 0;
}

void nguiEndWindow() {
	ngui->currentWindow = NULL;
}

bool nguiButton(char *name, char *subText) {
	NguiElement *element = getNguiElement(name);
	element->type = NGUI_ELEMENT_BUTTON;
	element->subText = subText;
	return element->justActive;
}

int getSizeForDataType(NguiDataType dataType) {
	int size = 0;
	if (dataType == NGUI_DATA_TYPE_INT) size = sizeof(int);
	if (dataType == NGUI_DATA_TYPE_COLOR_INT) size = sizeof(int);
	if (dataType == NGUI_DATA_TYPE_FLOAT) size = sizeof(float);
	if (dataType == NGUI_DATA_TYPE_VEC2) size = sizeof(Vec2);
	if (dataType == NGUI_DATA_TYPE_STRING_PTR) size = sizeof(char *);
	if (!size) Panic(frameSprintf("Invalid size for ngui data type %d?", dataType));
	return size;
}
