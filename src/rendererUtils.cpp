RenderProps newRenderProps();
RenderProps newRenderProps(Texture *texture, Rect rect);
RenderProps newRenderProps(Xform2 xform);
RenderProps createRenderProps() { return newRenderProps(); } // One day changed everything to this?
void drawRect(float x, float y, float width, float height, int color) { drawRect(makeRect(x, y, width, height), color); }
void drawCircle(Vec2 position, float radius, int color) { drawCircle(makeCircle(position, radius), color); }
void drawLine(Vec2 start, Vec2 end, int thickness, int color);
void drawLine(Line2 line, int thickness, int color) { drawLine(line.start, line.end, thickness, color); }
void drawRectOutline(Rect rect, float outlineSize, int color);
Vec2 getSize(Texture *texture);
Rect getRect(Texture *texture);
Rect getInnerRectOfAspect(Rect toFit, Texture *texture, Vec2 gravity=v2(0.5, 0.5)) { return getInnerRectOfAspect(toFit, getSize(texture), gravity); }
void drawCapsule(Capsule2 cap, int color);
Texture *copyTexture(Texture *src);
Texture *trimAlpha(Texture *texture);

RenderProps newRenderProps() {
	RenderProps props = {};
	props.alpha = 1;
	props.tint = 0xFFFFFFFF;
	props.matrix = mat3();
	props.uvMatrix = mat3();
	props.uv1 = v2(1, 1);
  // if (!renderer->useBadSrcSize) props.srcWidth = props.srcHeight = 1;
	return props;
}

RenderProps newRenderProps(Texture *texture, Rect rect) {
	if (!texture) return newRenderProps();

	RenderProps props = newRenderProps();
	props.matrix.TRANSLATE(rect.x, rect.y);
	props.matrix.SCALE(rect.width/(float)texture->width, rect.height/(float)texture->height);
	props.matrix.SCALE(getSize(texture));
	return props;
}

RenderProps newRenderProps(Xform2 xform) {
	RenderProps props = newRenderProps();
	props.matrix = toMatrix(xform);
	return props;
}

void drawLine(Vec2 start, Vec2 end, int thickness, int color) {
	Matrix3 matrix = mat3();
	matrix.TRANSLATE(start.x, start.y);
	matrix.ROTATE(toDeg(radsBetween(start, end))); //@WeirdCoords Sometimes I need a negative here??
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

Rect getRect(Texture *texture) {
	if (!texture) {
		return makeRect();
		logf("getRect of NULL texture\n");
	}
	return makeRect(0, 0, texture->width, texture->height);
}

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

Texture *copyTexture(Texture *src) { //@speed This could use framebuffers
	u8 *bitmapData = getTextureData(src);
	Texture *dest = createTexture(src->width, src->height, bitmapData, _F_TD_SKIP_PREMULTIPLY);
	free(bitmapData);
	return dest;
}

u8 *pngByteData = NULL;
int pngByteDataNum = 0;

void writeTextureFunc(void *context, void *data, int size);
void writeTextureFunc(void *context, void *data, int size) {
	memcpy(pngByteData + pngByteDataNum, data, size);
	pngByteDataNum += size;
}

u8 *writeTextureToPngBytes(Texture *texture, int *outBytesNum);
u8 *writeTextureToPngBytes(Texture *texture, int *outBytesNum) {
	*outBytesNum = 0;

	u8 *bitmapData = getTextureData(texture);

	pngByteData = (u8 *)zalloc(Megabytes(10));
	stbi_flip_vertically_on_write(true);
	if (!stbi_write_png_to_func(writeTextureFunc, NULL, texture->width, texture->height, 4, bitmapData, texture->width*4)) {
		logf("Failed to write image to png bytes");
	}

	free(bitmapData);

	*outBytesNum = pngByteDataNum;
	return pngByteData;
}

Texture *trimAlpha(Texture *texture) { // Leaves 1px of alpha padding
	u8 *srcBitmapData = getTextureData(texture);
	int srcWidth = texture->width;
	int srcHeight = texture->height;

	int minY = -1;
	int maxY = 0;
	for (int y = 0; y < srcHeight; y++) {
		for (int x = 0; x < srcWidth; x++) {
			u8 alpha = srcBitmapData[(y*srcWidth+x)*4 + 3];
			if (alpha > 0) {
				if (minY == -1) minY = y-1;
				maxY = y+1;
			}
		}
	}

	int minX = -1;
	int maxX = 0;
	for (int x = 0; x < srcWidth; x++) {
		for (int y = 0; y < srcHeight; y++) {
			u8 alpha = srcBitmapData[(y*srcWidth+x)*4 + 3];
			if (alpha > 0) {
				if (minX == -1) minX = x-1;
				maxX = x+1;
			}
		}
	}

	if (minY < 0) minY = 0;
	if (maxY > srcHeight-1) maxY = srcHeight-1;

	if (minX < 0) minX = 0;
	if (maxX > srcWidth-1) maxX = srcWidth-1;

	int destWidth = maxX - minX + 1;
	int destHeight = maxY - minY + 1;

	u8 *destBitmapData = (u8 *)frameMalloc(destWidth * destHeight * 4);

	for (int y = 0; y < destHeight; y++) { //@speed This could be faster if going line-by-line
		for (int x = 0; x < destWidth; x++) {
			int destX = x;
			int destY = y;
			int destIndex = (destY * destWidth + destX) * 4;

			int srcX = x + minX;
			int srcY = y + minY;
			int srcIndex = (srcY * srcWidth + srcX) * 4;

			destBitmapData[destIndex + 0] = srcBitmapData[srcIndex + 0];
			destBitmapData[destIndex + 1] = srcBitmapData[srcIndex + 1];
			destBitmapData[destIndex + 2] = srcBitmapData[srcIndex + 2];
			destBitmapData[destIndex + 3] = srcBitmapData[srcIndex + 3];
		}
	}
	free(srcBitmapData);

	Texture *destTexture = createTexture(destWidth, destHeight, destBitmapData);
	return destTexture;
}
