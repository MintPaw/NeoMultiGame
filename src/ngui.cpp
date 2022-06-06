enum NguiElementType {
	NGUI_ELEMENT_WINDOW,
	NGUI_ELEMENT_BUTTON,
};
struct NguiElement {
	NguiElementType type;

#define NGUI_ELEMENT_NAME_MAX_LEN 64
	char name[NGUI_ELEMENT_NAME_MAX_LEN];
	bool exists;

	Vec2 position;
	Vec2 size;
	bool active;
	bool justActive;

	int orderIndex;
	int id;
	int parentId;

	float creationTime;
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
	Vec2 currentWindowSize;
	NguiElement *currentWindow;
};
Ngui *ngui = NULL;

void nguiInit();
void nguiDraw(float elapsed);

NguiElement *getNguiElement(char *name);
int getNguiId(int parentId, char *name);

void nguiSetNextWindowSize(Vec2 size);
void nguiStartWindow(char *name, Vec2 size, int flags = 0);
void nguiEndWindow();
/// FUNCTIONS ^


void nguiInit() {
	ngui = (Ngui *)zalloc(sizeof(Ngui));
	ngui->defaultFont = createFont("assets/common/arial.ttf", 40);
	ngui->uiScale = 1;

	ngui->elementsMax = 128;
	ngui->elements = (NguiElement *)zalloc(sizeof(NguiElement) * ngui->elementsMax);
}

void nguiDraw(float elapsed) {
	for (int i = 0; i < ngui->elementsNum; i++) {
		NguiElement *element = &ngui->elements[i];
		if (!element->exists) {
			arraySpliceIndex(ngui->elements, ngui->elementsNum, sizeof(NguiElement), i);
			i--;
			ngui->elementsNum--;
			continue;
		}
	}

	NguiElement **elementsLeft = (NguiElement **)frameMalloc(sizeof(NguiElement *) * ngui->elementsMax);
	int elementsLeftNum = 0;
	for (int i = 0; i < ngui->elementsNum; i++) {
		NguiElement *element = &ngui->elements[i];
		element->active = false;
		element->justActive = false;
		element->exists = false;
		elementsLeft[elementsLeftNum++] = element;
	}

	int lastLeftNum = elementsLeftNum;
	for (;;) {
		if (elementsLeftNum == 0) break;
		for (int i = 0; i < elementsLeftNum; i++) {
			int windowIndex = i;
			NguiElement *window = elementsLeft[windowIndex];
			if (window->type != NGUI_ELEMENT_WINDOW) continue;

			Rect rect = makeRect(window->position, window->size) * ngui->uiScale;
			drawRect(rect, 0xA0202020);

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

			Vec2 prevCursor = window->position;
			Vec2 cursor = window->position;
			NguiElement *prevChild = NULL;
			for (int i = 0; i < childrenNum; i++) {
				NguiElement *child = children[i];

				child->active = false;
				char *label = child->name;

				bool skipCursorBump = false;
				if (ngui->time - child->creationTime < 0.001) {
					if (prevChild && ngui->time - prevChild->creationTime < 0.001) {
						child->position = prevCursor;
						skipCursorBump = true;
					} else {
						child->position = cursor;
					}
				}
				child->position = lerp(child->position, cursor, 0.05);
				Rect childRect = makeRect(child->position, child->size) * ngui->uiScale;

				if (child->type == NGUI_ELEMENT_BUTTON) {
					childRect = inflatePerc(childRect, -0.05);

					int buttonColor = 0xFF404040;
					if (contains(childRect, ngui->mouse)) {
						buttonColor = lerpColor(buttonColor, 0xFFFFFFFF, 0.25);
						if (platform->mouseJustDown) child->justActive = true;
					}
					drawRect(childRect, buttonColor);
					DrawTextProps props = newDrawTextProps(ngui->defaultFont, 0xFFA0A0A0);
					drawTextInRect(label, props, childRect, v2(0, 0.5));
				}

				if (!skipCursorBump) {
					prevCursor = cursor;
					cursor.y += child->size.y;
				}
				prevChild = child;
			}

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

	// for (int i = 0; i < ngui->elementsNum; i++) {
	// 	NguiElement *element = &ngui->elements[i];
	// }

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
		element->creationTime = ngui->time;
	}

	element->exists = true;
	element->orderIndex = ngui->currentOrderIndex++;
	if (ngui->currentWindow) element->parentId = ngui->currentWindow->id;
	return element;
}

int getElementIndex(NguiElement *element) {
	for (int i = 0; i < ngui->elementsNum; i++) {
		NguiElement *possibleElement = &ngui->elements[i];
		if (possibleElement == element) return i;
	}

	return 0;
}

void nguiStartWindow(char *name, Vec2 size, int flags) {
	NguiElement *element = getNguiElement(name);
	element->type = NGUI_ELEMENT_WINDOW;
	element->size = size;
	ngui->currentWindow = element;
	ngui->currentOrderIndex = 0;
}

void nguiEndWindow() {
	ngui->currentWindow = NULL;
}

bool nguiButton(char *name) {
	NguiElement *element = getNguiElement(name);
	element->type = NGUI_ELEMENT_BUTTON;
	element->size = v2(200, 80);
	return element->justActive;
}
