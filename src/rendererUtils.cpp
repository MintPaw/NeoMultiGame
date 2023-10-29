RenderProps newRenderProps();
RenderProps newRenderProps(Texture *texture, Rect rect);
RenderProps createRenderProps() { return newRenderProps(); } // One day changed everything to this?
void drawRect(float x, float y, float width, float height, int color, int flags=0) { drawRect(makeRect(x, y, width, height), color, flags); }
void drawCircle(Vec2 position, float radius, int color) { drawCircle(makeCircle(position, radius), color); }
void drawLine(Vec2 start, Vec2 end, int thickness, int color);
void drawLine(Line2 line, int thickness, int color) { drawLine(line.start, line.end, thickness, color); }
void drawRectOutline(Rect rect, float outlineSize, int color);
Vec2 getSize(Texture *texture);
Rect makeRect(Texture *texture);

RenderProps newRenderProps() {
	RenderProps props = {};
	props.alpha = 1;
	props.tint = 0xFFFFFFFF;
	props.matrix = mat3();
	props.uvMatrix = mat3();
	props.uv1 = v2(1, 1);
  if (!renderer->useBadSrcSize) props.srcWidth = props.srcHeight = 1;
	return props;
}

RenderProps newRenderProps(Texture *texture, Rect rect) {
	if (!texture) return newRenderProps();

	RenderProps props = newRenderProps();
	props.matrix.TRANSLATE(rect.x, rect.y);
	props.matrix.SCALE(rect.width/(float)texture->width, rect.height/(float)texture->height);
	return props;
}

void drawLine(Vec2 start, Vec2 end, int thickness, int color) {
	Matrix3 matrix = mat3();
	matrix.TRANSLATE(start.x, start.y);
	matrix.ROTATE(-toDeg(radsBetween(start, end))); //@WeirdCoords Why is there a negative here?
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
}

Vec2 getSize(Texture *texture) {
	if (!texture) {
		logf("getSize of NULL texture\n");
		return v2();
	}
	return v2(texture->width, texture->height);
}

Rect makeRect(Texture *texture) {
	if (!texture) {
		return makeRect();
		logf("makeRect of NULL texture\n");
	}
	return makeRect(0, 0, texture->width, texture->height);
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
