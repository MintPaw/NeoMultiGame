RenderProps newRenderProps();
RenderProps newRenderProps(RenderTexture *texture, Rect rect);
RenderProps newRenderProps(Texture *texture, Rect rect);
void drawLine(Vec2 start, Vec2 end, int thickness, int color);
void drawLine(Line2 line, int thickness, int color) { drawLine(line.start, line.end, thickness, color); }
void drawRectOutline(Rect rect, float outlineSize, int color);
void drawRect(float x, float y, float width, float height, int color, int flags=0) { drawRect(makeRect(x, y, width, height), color, flags); }
void drawCircle(Circle circle, int color) { drawCircle(circle.position, circle.radius, color); }
Vec2 getSize(RenderTexture *renderTexture);
Vec2 getSize(Texture *texture);
Rect makeRect(RenderTexture *renderTexture);
Rect makeRect(Texture *texture);

void initRendererUtils();
Texture *createFrameTexture(int width, int height, void *data=NULL);
RenderTexture *createFrameRenderTexture(int width, int height, void *data=NULL);
void freeFrameTextures();

void drawLine(Vec2 start, Vec2 end, int thickness, int color) {
	Matrix3 matrix = mat3();
	matrix.TRANSLATE(start.x, start.y);
	matrix.ROTATE(toDeg(radsBetween(start, end)));
	matrix.TRANSLATE(0, -thickness/2);

	float width = start.distance(end);
	float height = thickness;
	matrix.SCALE(width, height);

	pushCamera2d(matrix);
	drawRect(0, 0, 1, 1, color);
	popCamera2d();
}

void drawRectOutline(Rect rect, float outlineSize, int color) {
	drawRect(makeRect(rect.x + outlineSize, rect.y, rect.width - outlineSize, outlineSize), color); // top
	drawRect(makeRect(rect.x, rect.y, outlineSize, rect.height - outlineSize), color); // left
	drawRect(makeRect(rect.x, rect.y + rect.height - outlineSize, rect.width, outlineSize), color); // bot
	drawRect(makeRect(rect.x + rect.width - outlineSize, rect.y, outlineSize, rect.height - outlineSize), color); // right

	// drawRect(makeRect(rect.x, rect.y, rect.width - outlineSize*2, outlineSize), color); // top
	// drawRect(makeRect(rect.x, rect.y + outlineSize*2, outlineSize, rect.height - outlineSize*4), color); // left
	// drawRect(makeRect(rect.x, rect.y + rect.height - outlineSize, rect.width, outlineSize), color); // bot
	// drawRect(makeRect(rect.x + rect.width - outlineSize, rect.y, outlineSize, rect.height - outlineSize*2), color); // right
}

RenderProps createRenderProps() { return newRenderProps(); } // One day changed everything to this

RenderProps newRenderProps() {
	RenderProps props = {};
	props.alpha = 1;
	props.tint = 0xFFFFFFFF;
	props.matrix = mat3();
	props.uvMatrix = mat3();
	props.uv1 = v2(1, 1);
	return props;
}

RenderProps newRenderProps(RenderTexture *texture, Rect rect) {
	if (!texture) return newRenderProps();

	RenderProps props = newRenderProps();
	props.matrix.TRANSLATE(rect.x, rect.y);
	props.matrix.SCALE(rect.width/(float)texture->width, rect.height/(float)texture->height);
	return props;
}

RenderProps newRenderProps(Texture *texture, Rect rect) {
	if (!texture) return newRenderProps();

	RenderProps props = newRenderProps();
	props.matrix.TRANSLATE(rect.x, rect.y);
	props.matrix.SCALE(rect.width/(float)texture->width, rect.height/(float)texture->height);
	return props;
}

Vec2 getSize(RenderTexture *renderTexture) { 
	if (!renderTexture) Panic("getSize of NULL renderTexture");
	return v2(renderTexture->width, renderTexture->height);
}

Vec2 getSize(Texture *texture) {
	if (!texture) Panic("getSize of NULL texture");
	return v2(texture->width, texture->height);
}

Rect makeRect(Texture *texture) {
	if (!texture) Panic("makeRect of NULL texture");
	return makeRect(0, 0, texture->width, texture->height);
}

Rect makeRect(RenderTexture *renderTexture) {
	if (!renderTexture) Panic("makeRect of NULL renderTexture");
	return makeRect(0, 0, renderTexture->width, renderTexture->height);
}

void drawCapsule(Capsule2 cap, int color);
void drawCapsule(Capsule2 cap, int color) {
	drawCircle(cap.start, cap.radius, color);
	drawCircle(cap.end, cap.radius, color);

	Matrix3 cam = mat3();
	cam.TRANSLATE(cap.start);
	cam.ROTATE(toDeg(radsBetween(cap.start, cap.end)));
	pushCamera2d(cam);
	Rect rect = makeRect(0, 0, cap.start.distance(cap.end), cap.radius*2);
	rect.y -= cap.radius;
	drawRect(rect, color);
	popCamera2d();
}

struct RendererUtils {
#define FRAME_TEXTURES_MAX 512
	Texture *frameTextures[FRAME_TEXTURES_MAX];
	int frameTexturesNum;

#define FRAME_RENDER_TEXTURES_MAX 512
	RenderTexture *frameRenderTextures[FRAME_RENDER_TEXTURES_MAX];
	int frameRenderTexturesNum;
};
RendererUtils *rendererUtils = NULL;

void initRendererUtils() {
	rendererUtils = (RendererUtils *) zalloc(sizeof(RendererUtils));

	char *gradData = (char *)frameMalloc(1 * 256 * 4);
	for (int y = 0; y < 256; y++) {
		Vec4 color = v4(y/255.0, 1, 1, 1);
		int colorInt = argbToHex(color);
		memcpy(&gradData[y*4], &colorInt, sizeof(int));
	}

	renderer->linearGrad256 = createTexture(1, 256, gradData);
	setTextureClamped(renderer->linearGrad256, true);
}

Texture *createFrameTexture(int width, int height, void *data) {
	Texture *texture = createTexture(width, height, data);
	if (rendererUtils->frameTexturesNum > FRAME_TEXTURES_MAX-1) {
		logf("Too many frame textures, texture will leak\n");
		return texture;
	}

	rendererUtils->frameTextures[rendererUtils->frameTexturesNum++] = texture;
	return texture;
}

RenderTexture *createFrameRenderTexture(int width, int height, void *data) {
	RenderTexture *texture = createRenderTexture(width, height, data);
	if (rendererUtils->frameRenderTexturesNum > FRAME_RENDER_TEXTURES_MAX-1) {
		logf("Too many frame render textures, render texture will leak\n");
		return texture;
	}

	rendererUtils->frameRenderTextures[rendererUtils->frameRenderTexturesNum++] = texture;
	return texture;
}

void freeFrameTextures() {
	for (int i = 0; i < rendererUtils->frameTexturesNum; i++) destroyTexture(rendererUtils->frameTextures[i]);
	rendererUtils->frameTexturesNum = 0;

	for (int i = 0; i < rendererUtils->frameRenderTexturesNum; i++) destroyTexture(rendererUtils->frameRenderTextures[i]);
	rendererUtils->frameRenderTexturesNum = 0;
}
