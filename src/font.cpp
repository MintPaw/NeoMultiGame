#define FONT_WIDTH_LIMIT 2048
#define FONT_HEIGHT_LIMIT 2048
// #define FONT_WIDTH_LIMIT 4096
// #define FONT_HEIGHT_LIMIT 4096
#define MAX_UNICODE_CHAR 254
// #define MAX_UNICODE_CHAR 127

#define L_FONT "Font Log"
#define ARIAL_FONT "assets/common/arial.ttf"

#define Font __OTHER_FONT // Because X11 has a Font typedef // And so does raylib
struct Font {
	char *path;
	int fontSize;
	Texture *texture;
	stbtt_packedchar *charData;
	float ascent;
	float descent;
	float lineGap;
	float lineSpacing;

	void *skiaUserData;
	float renderScale;
	float lastAccessedTime;
};

struct FontSizeMapping {
	Font *bestFont;
	int srcSize;
};

struct TextProps {
	char *fontPath;
	Vec2 position;
	int color;
	float maxWidth;
	float scale;
	bool centered;
	float lineSpacingScale;
	float extrudeVertsPerc;

	void (*charCallback)(TextProps *);
	void *charCallbackUserData;
	Vec2 currentOffset;
	int currentColor;
	int currentIndex;
	Vec2 currentCursor;

	bool skipDraw;
	Font *internalFont;
};

struct FontSystem {
	bool disabled;

#define FONTS_MAX 128
	Font *fonts[FONTS_MAX];
	int fontsNum;

#define FONT_SIZE_MAPPINGS_MAX 1024
	FontSizeMapping fontSizeMappings[FONT_SIZE_MAPPINGS_MAX];
	int fontSizeMappingsNum;

	u8 *temp1bppPixels;
	u8 *temp4bppPixels;
};

FontSystem *fontSys = NULL;

void initFonts();
Font *createFontInternal(char *ttfPath, int fontSize);

Rect drawTextInRect(char *text, TextProps props, Rect toFit, Vec2 gravity=v2(0.5, 0.5));

TextProps createTextProps(char *fontPath, int color=0xFFFFFFFF, Vec2 position=v2());

Vec2 drawText(char *text, TextProps props);
Vec2 getTextSize(char *string, TextProps props);

int scanNextGlyph(char *text, int index, int *outCharsInThisGlyph);
int drawSingleTextLine(char *text, TextProps textProps);

Font *getFontInternal(char *ttfPath, int fontSize);
void drawOnScreenLog();
void destroyFont(Font *font);

Font *getFontFromMappingCache(char *ttfPath, int fontSize);
void destroyFontSizeMappings(Font *font);

void guiDrawFontDebug();
/// FUNCTIONS ^

void initFonts() {
#if defined(FALLOW_COMMAND_LINE_ONLY)
	return;
#endif

	fontSys = (FontSystem *)zalloc(sizeof(FontSystem));

	if (!fileExists(ARIAL_FONT)) Panic("Need arial font!\n");
}

Font *createFontInternal(char *ttfPath, int fontSize) {
	NanoTime startTime = getNanoTime();
	if (!fileExists(ttfPath)) {
		logf("Can't find font at %s\n", ttfPath);
		ttfPath = ARIAL_FONT;
	}

	Font *font = (Font *)zalloc(sizeof(Font));
	font->path = stringClone(ttfPath);
	font->fontSize = fontSize;
	font->renderScale = 1;

	int ttfDataSize;
	void *ttfData = readFile(ttfPath, &ttfDataSize);

	if (!fontSys->temp1bppPixels) fontSys->temp1bppPixels = (u8 *)malloc(FONT_WIDTH_LIMIT * FONT_HEIGHT_LIMIT);
	stbtt_pack_context packContext;
	if (!stbtt_PackBegin(&packContext, fontSys->temp1bppPixels, FONT_WIDTH_LIMIT, FONT_HEIGHT_LIMIT, 0, 16, NULL)) Panic("Faied to PackBegin");

	font->charData = (stbtt_packedchar *)malloc(MAX_UNICODE_CHAR * sizeof(stbtt_packedchar));

	if (!stbtt_PackFontRange(&packContext, (u8 *)ttfData, 0, fontSize, 0, MAX_UNICODE_CHAR, font->charData)) {
		stbtt_PackEnd(&packContext);
		free(font->charData);
		free(ttfData);
		free(font->path);
		free(font);
		return NULL;
	}

	stbtt_PackEnd(&packContext);

	{
		stbtt_fontinfo info;
		if (!stbtt_InitFont(&info, (u8 *)ttfData, 0)) Panic("Failed to init font");

		int ascent, descent, lineGap;
		stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
		float scale = stbtt_ScaleForPixelHeight(&info, fontSize);
		font->ascent = ascent * scale;
		font->descent = descent * scale;
		font->lineGap = lineGap * scale;
		font->lineSpacing = font->ascent - font->descent + font->lineGap;
		free(ttfData);
	}

	int textureWidth = 0;
	int textureHeight = 0;
	for (int i = 0; i < MAX_UNICODE_CHAR; i++) {
		stbtt_packedchar *charData = &font->charData[i];
		if (textureWidth < charData->x1) textureWidth = charData->x1;
		if (textureHeight < charData->y1) textureHeight = charData->y1;
	}

	if (!fontSys->temp4bppPixels) fontSys->temp4bppPixels = (u8 *)malloc(FONT_WIDTH_LIMIT * FONT_HEIGHT_LIMIT * 4);
	for (int y = 0; y < textureHeight; y++) {
		for (int x = 0; x < textureWidth; x++) {
			u8 byte = fontSys->temp1bppPixels[y * FONT_WIDTH_LIMIT + x];
			fontSys->temp4bppPixels[(y * textureWidth + x) * 4 + 0] = byte;
			fontSys->temp4bppPixels[(y * textureWidth + x) * 4 + 1] = byte;
			fontSys->temp4bppPixels[(y * textureWidth + x) * 4 + 2] = byte;
			fontSys->temp4bppPixels[(y * textureWidth + x) * 4 + 3] = byte;
		}
	}

	flipBitmapData(fontSys->temp4bppPixels, textureWidth, textureHeight);

	font->texture = createTexture(textureWidth, textureHeight, fontSys->temp4bppPixels, _F_TD_SKIP_PREMULTIPLY);

	logTo(L_FONT, "Took %fms to load font %s(%d)\n", getMsPassed(startTime), ttfPath, fontSize);
	return font;
}

TextProps createTextProps(char *fontPath, int color, Vec2 position) {
	TextProps props = {};
	props.fontPath = fontPath;
	props.color = color;
	props.position = position;
	props.maxWidth = 9999;
	props.scale = 1;
	props.lineSpacingScale = 1;
	return props;
}

Vec2 drawText(char *text, TextProps textProps) {
	if (textProps.internalFont) logf("Why does this textProps already have a font???\n");
	if (!textProps.fontPath) textProps.fontPath = ARIAL_FONT;

	{
		float wantedFontSize = 24 * textProps.scale;

		int closestAllowedSize = 0;
		{
			int allowedFontSizes[] = {8, 10, 12, 14, 16, 18, 24, 30, 36, 42, 50, 60, 80, 100, 120, 140, 160, 180, 200, 220, 240, 260, 280, 300};
			for (int i = 0; i < ArrayLength(allowedFontSizes); i++) {
				if (fabs(closestAllowedSize - allowedFontSizes[i]) > fabs(wantedFontSize - allowedFontSizes[i])) {
					closestAllowedSize = allowedFontSizes[i];
				}
			}
		}

		textProps.internalFont = getFontInternal(textProps.fontPath, closestAllowedSize);
		float gotFontSize = textProps.internalFont->fontSize * textProps.internalFont->renderScale;

		textProps.scale = wantedFontSize / gotFontSize;
		textProps.scale *= textProps.internalFont->renderScale;
	}

	int linesMax = strlen(text)+1;
	char **lines = (char **)frameMalloc(sizeof(char *) * linesMax);
	int linesNum = 0;

	float *lineWidths = (float *)frameMalloc(sizeof(float) * linesMax);

	int lineMaxLen = strlen(text)+1;
	char *line = (char *)frameMalloc(sizeof(char) * lineMaxLen);
	int lineCharIndex = 0;

	float lineWidth = 0;

	int textIndex = 0;
	int prevGlyph = 0;
	for (;;) {
		int charsInThisGlyph = 0;
		int glyph = scanNextGlyph(text, textIndex, &charsInThisGlyph);
		if (glyph == 0) break;

		bool breakLine = false;

		char *breakableChars = " -";
		bool canLineBreakHere = false;
		if (strchr(breakableChars, (char)glyph)) canLineBreakHere = true;

		if (canLineBreakHere) {
			char *nextBreakableChars = " -\n";

			char *nextBreakPtr = strpbrk(&text[textIndex+1], nextBreakableChars);
			int nextBreakIndex;
			if (nextBreakPtr) nextBreakIndex = nextBreakPtr - text;
			else nextBreakIndex = strlen(text);

			float wordWidth = 0;
			int wordTextIndex = textIndex;
			for (;;) {
				int charsInThisGlyph = 0;
				int wordGlyph = scanNextGlyph(text, wordTextIndex, &charsInThisGlyph);

				stbtt_packedchar *charData = &textProps.internalFont->charData[wordGlyph];
				float charWidth = charData->xadvance * textProps.scale;
				if (wordTextIndex == nextBreakIndex-1) charWidth = charData->xoff + (charData->x1 - charData->x0) * textProps.scale;

				wordWidth += charWidth;

				wordTextIndex += charsInThisGlyph;
				if (wordTextIndex == nextBreakIndex) break;
			}

			if (lineWidth + wordWidth > textProps.maxWidth) breakLine = true;
		}

		bool addChar = true;

		if (glyph == '\n') {
			breakLine = true;
			addChar = false;
		}

		if (breakLine) {
			if (prevGlyph) {
				stbtt_packedchar *charData = &textProps.internalFont->charData[prevGlyph];
				lineWidth -= charData->xadvance * textProps.scale;
				lineWidth += (charData->xoff + (charData->x1 - charData->x0)) * textProps.scale;
			}
			lineWidths[linesNum] = lineWidth;
			lines[linesNum] = frameStringClone(line);
			linesNum++;
			lineWidth = 0;
			lineCharIndex = 0;
			line[0] = 0;
		}

		if (addChar) {
			for (int i = 0; i < charsInThisGlyph; i++) {
				line[lineCharIndex++] = text[textIndex+i];
			}
			line[lineCharIndex] = 0;

			stbtt_packedchar *charData = &textProps.internalFont->charData[glyph];
			float charWidth = charData->xadvance * textProps.scale;
			lineWidth += charWidth;
		}

		prevGlyph = glyph;
		textIndex += charsInThisGlyph;
	}
	if (lineCharIndex > 0) {
		if (prevGlyph) {
			stbtt_packedchar *charData = &textProps.internalFont->charData[prevGlyph];
			lineWidth -= charData->xadvance * textProps.scale;
			lineWidth += (charData->xoff + (charData->x1 - charData->x0)) * textProps.scale;
		}
		lineWidths[linesNum] = lineWidth;
		lines[linesNum] = frameStringClone(line);
		linesNum++;
	}

	float widestWidth = 0;
	for (int i = 0; i < linesNum; i++) {
		float lineWidth = lineWidths[i];
		if (widestWidth < lineWidth) widestWidth = lineWidth;
	}

	textProps.maxWidth = 9999;
	Vec2 cursor = textProps.position;
	for (int i = 0; i < linesNum; i++) {
		char *line = lines[i];
		float lineWidth = lineWidths[i];

		TextProps lineProps = textProps;
		lineProps.position = cursor;
		if (textProps.centered) lineProps.position.x += (widestWidth - lineWidth)/2;
		textProps.currentIndex += drawSingleTextLine(line, lineProps);

		if (i == linesNum - 1) { // The last line is always full height
			cursor.y += textProps.internalFont->lineSpacing * textProps.scale;
		} else {
			cursor.y += textProps.internalFont->lineSpacing * textProps.lineSpacingScale * textProps.scale;
		}
	}

	return v2(widestWidth, cursor.y - textProps.position.y);
}

Vec2 getTextSize(char *string, TextProps props) {
	props.skipDraw = true;
	return drawText(string, props);
}

int scanNextGlyph(char *text, int index, int *outCharsInThisGlyph) {
	int startingIndex = index;

	int glyph = 0;
	char firstByte = text[index];
	if (!IS_BIT_SET(firstByte, 7)) {
		char byte1 = text[index++];
		glyph = byte1;
	} else if (IS_BIT_SET(firstByte, 7) && IS_BIT_SET(firstByte, 6) && !IS_BIT_SET(firstByte, 5)) {
		char byte1 = text[index++] & 0b00011111;
		char byte2 = text[index++] & 0b00111111;

		glyph |= byte1 << 6;
		glyph |= byte2 << 0;

	} else if (IS_BIT_SET(firstByte, 7) && IS_BIT_SET(firstByte, 6) && IS_BIT_SET(firstByte, 5) && !IS_BIT_SET(firstByte, 4)) {
		char byte1 = text[index++] & 0b00001111;
		char byte2 = text[index++] & 0b00111111;
		char byte3 = text[index++] & 0b00111111;

		glyph |= byte1 << 12;
		glyph |= byte2 << 6;
		glyph |= byte3 << 0;

	} else if (IS_BIT_SET(firstByte, 7) && IS_BIT_SET(firstByte, 6) && IS_BIT_SET(firstByte, 5) && IS_BIT_SET(firstByte, 4) && !IS_BIT_SET(firstByte, 3)) {
		char byte1 = text[index++] & 0b00000111;
		char byte2 = text[index++] & 0b00111111;
		char byte3 = text[index++] & 0b00111111;
		char byte4 = text[index++] & 0b00111111;

		glyph |= byte1 << 18;
		glyph |= byte2 << 12;
		glyph |= byte3 << 6;
		glyph |= byte4 << 0;
	}

	if (glyph > MAX_UNICODE_CHAR-1) glyph = '?';

	*outCharsInThisGlyph = index - startingIndex;
	return glyph;
}

int drawSingleTextLine(char *line, TextProps textProps) {
	if (fontSys->disabled) return 0;
	if (!line) return 0;

	textProps.currentCursor = textProps.position;

	int lineIndex = 0;

	if (line[0] == ' ') {
		lineIndex += 1;
		textProps.currentIndex += 1;
	}

	for (;;) {
		bool charDisabled = false;
		if (lineIndex == strlen(line)) break;

		textProps.currentOffset = v2();
		textProps.currentColor = textProps.color;
		if (textProps.charCallback) textProps.charCallback(&textProps);

		int charsInThisGlyph = 0;
		int glyph = scanNextGlyph(line, lineIndex, &charsInThisGlyph);
		lineIndex += charsInThisGlyph;
		textProps.currentIndex += charsInThisGlyph;

		if (glyph == ' ') charDisabled = true;

		stbtt_packedchar *charData = &textProps.internalFont->charData[glyph];

		float srcX = charData->x0;
		float srcY = charData->y0;
		float srcWidth = charData->x1 - charData->x0;
		float srcHeight = charData->y1 - charData->y0;
		float textureWidth = textProps.internalFont->texture->width;
		float textureHeight = textProps.internalFont->texture->height;

		Matrix3 charUvMatrix = mat3();
		charUvMatrix.TRANSLATE(srcX/textureWidth, srcY/textureHeight);
		charUvMatrix.SCALE(srcWidth/textureWidth, srcHeight/textureHeight);

		Vec2 charDataOff = v2();
		charDataOff.x = charData->xoff;
		charDataOff.y = charData->yoff + textProps.internalFont->ascent;

		Matrix3 charMatrix = mat3();
		charMatrix.TRANSLATE(textProps.currentOffset);
		charMatrix.TRANSLATE(textProps.currentCursor);
		charMatrix.SCALE(textProps.scale);
		charMatrix.TRANSLATE(charDataOff);
		charMatrix.SCALE(srcWidth, srcHeight);

		textProps.currentCursor.x += charData->xadvance * textProps.scale;

		if (textProps.skipDraw || charDisabled) continue;

		if (textProps.extrudeVertsPerc) {
			GpuVertex gpuVerts[4];

			gpuVerts[0].position = v3(charMatrix * v2(0, 0), 1);
			gpuVerts[1].position = v3(charMatrix * v2(0, 1), 1);
			gpuVerts[2].position = v3(charMatrix * v2(1, 1), 1);
			gpuVerts[3].position = v3(charMatrix * v2(1, 0), 1);

			gpuVerts[0].uv = charUvMatrix * v2(0, 0);
			gpuVerts[1].uv = charUvMatrix * v2(0, 1);
			gpuVerts[2].uv = charUvMatrix * v2(1, 1);
			gpuVerts[3].uv = charUvMatrix * v2(1, 0);

			{
				Vec2 centerPos = (v2(gpuVerts[0].position) + v2(gpuVerts[1].position) + v2(gpuVerts[2].position) + v2(gpuVerts[3].position))/4;
				float dist = distance(centerPos, v2(gpuVerts[0].position));
				for (int i = 0; i < 4; i++) {
					GpuVertex *vert = &gpuVerts[i];
					Vec2 dir = normalize(v2(vert->position) - centerPos);
					Vec2 offset = dir * dist * textProps.extrudeVertsPerc;
					vert->position.x += offset.x;
					vert->position.y += offset.y;
				}
			}

			{
				Vec2 centerPos = (gpuVerts[0].uv + gpuVerts[1].uv + gpuVerts[2].uv + gpuVerts[3].uv)/4;
				float dist = distance(centerPos, gpuVerts[0].uv);
				for (int i = 0; i < 4; i++) {
					GpuVertex *vert = &gpuVerts[i];
					Vec2 dir = normalize(vert->uv - centerPos);
					Vec2 offset = dir * dist * textProps.extrudeVertsPerc;
					vert->uv.x += offset.x;
					vert->uv.y += offset.y;
				}
			}

			gpuVerts[0].color = hexToArgbFloat(textProps.currentColor);
			gpuVerts[1].color = hexToArgbFloat(textProps.currentColor);
			gpuVerts[2].color = hexToArgbFloat(textProps.currentColor);
			gpuVerts[3].color = hexToArgbFloat(textProps.currentColor);

			Matrix3 flipMatrix = {
				1,  0,  0,
				0, -1,  0,
				0,  1,  1
			};

			Matrix3 baseMatrix = getCurrentVertexMatrix();
			for (int i = 0; i < 4; i++) {
				gpuVerts[i].position = baseMatrix * gpuVerts[i].position;
				gpuVerts[i].uv = flipMatrix * gpuVerts[i].uv;
			}

			GpuVertex gpuVerts6[6];
			gpuVerts6[0] = gpuVerts[0];
			gpuVerts6[1] = gpuVerts[1];
			gpuVerts6[2] = gpuVerts[2];
			gpuVerts6[3] = gpuVerts[0];
			gpuVerts6[4] = gpuVerts[2];
			gpuVerts6[5] = gpuVerts[3];

			setTexture(textProps.internalFont->texture);
			drawVerts(gpuVerts6, 6);
		} else {
			RenderProps props = newRenderProps();
			props.tint = textProps.currentColor;
			props.matrix = charMatrix;
			props.uvMatrix = charUvMatrix;
			drawTexture(textProps.internalFont->texture, props);
		}
	}

	return lineIndex;
}

Rect drawTextInRect(char *text, TextProps props, Rect toFit, Vec2 gravity) {
	Vec2 size = getTextSize(text, props);
	Rect textRect = getInnerRectOfAspect(toFit, size, gravity);
	Vec2 textScale = getSize(textRect) / size;

	props.scale = textScale.x;
	props.position = getPosition(textRect);

	drawText(text, props);

	return textRect;
}

Font *getFontInternal(char *ttfPath, int fontSize) {
	auto addFontSizeMapping = [](Font *font, int srcSize) {
		if (fontSys->fontSizeMappingsNum > FONT_SIZE_MAPPINGS_MAX-1) {
			logf("Too many font size mappings!\n");
			fontSys->fontSizeMappingsNum--;
		}

		FontSizeMapping *mapping = &fontSys->fontSizeMappings[fontSys->fontSizeMappingsNum++];
		memset(mapping, 0, sizeof(FontSizeMapping));
		mapping->srcSize = srcSize;
		mapping->bestFont = font;
	};

	int MIN_FONT_SIZE = 2;
	if (fontSize < MIN_FONT_SIZE) fontSize = MIN_FONT_SIZE;

	{
		Font *existingFont = getFontFromMappingCache(ttfPath, fontSize);
		if (existingFont) return existingFont;
	}

	Font *font = NULL;
	float renderScale = 1;
	int originalFontSize = fontSize;

	for (int i = 0; i < 10; i++) {
		font = createFontInternal(ttfPath, fontSize);
		if (font) break;

		fontSize /= 2;
		renderScale *= 2;
		if (fontSize <= MIN_FONT_SIZE) {
			logf("ERROR: Font too small (%d)\n", fontSize);
			fontSize = MIN_FONT_SIZE;
		}

		font = getFontFromMappingCache(ttfPath, fontSize);
		if (font) {
			addFontSizeMapping(font, originalFontSize);
			return font;
		}
	}

	if (!font) Panic(frameSprintf("Failed to get font %s %d (%d)\n", ttfPath, fontSize, originalFontSize));

	font->renderScale = renderScale;
	font->lastAccessedTime = platform->time;
	addFontSizeMapping(font, originalFontSize);

	if (fontSys->fontsNum > FONTS_MAX-1) {
		Font *lruFont = NULL;
		int lruFontIndex = -1;
		for (int i = 0; i < fontSys->fontsNum; i++) {
			Font *font = fontSys->fonts[i];
			if (!lruFont || lruFont->lastAccessedTime < font->lastAccessedTime) {
				lruFont = font;
				lruFontIndex = i;
			}
		}

		destroyFontSizeMappings(lruFont);
		destroyFont(lruFont);
		arraySpliceIndex(fontSys->fonts, fontSys->fontsNum, sizeof(Font *), lruFontIndex);
		fontSys->fontsNum--;
	}
	fontSys->fonts[fontSys->fontsNum++] = font;

	return font;
}

void drawOnScreenLog() {
	if (!logSys) initLoggingSystem();

	int logCount = 0;
	for (int i = 0; i < LOGF_BUFFERS_MAX; i++) {
		LogfBuffer *logBuffer = &logSys->logs[i];

#if !defined(FALLOW_INTERNAL) //@todo Make this per-project
		if (!logBuffer->isInfo) continue;
#endif

		if (!logBuffer->buffer) return;
		if (logBuffer->buffer[0] == 0) continue;
		if (logCount >= 20) continue;

		float timeSinceLastLog = platform->time - logBuffer->logTime;

		float timeToFade = 1;
		float fadeTime = 1;

		if (logBuffer->isInfo) timeToFade = 3;

		bool shouldDestroy = false;
		int color;
		float fadePerc = 1;
		if (timeSinceLastLog < timeToFade) {
			color = 0xFFEEEEEE;
		} else {
			fadePerc = 1 - ((timeSinceLastLog - timeToFade) / fadeTime);
			color = lerpColor(0x00EEEEEE, 0xFFEEEEEE, fadePerc);
			if (fadePerc <= 0) shouldDestroy = true;
		}

		float textScale = platform->windowHeight / 1080.0;

		TextProps textProps = createTextProps(ARIAL_FONT);
		textProps.color = color;
		textProps.maxWidth = platform->windowWidth;
		textProps.scale = textScale;

		Vec2 size = getTextSize(logBuffer->buffer, textProps);
		Vec2 position;
		position.x = platform->windowWidth/2 - size.x/2;
		position.y = platform->windowHeight * 0.3;
		position.y -= logCount++ * size.y;

		bool shouldDraw = true;
		if (position.y < -200) shouldDraw = false;

		if (shouldDraw) {
			if (!logBuffer->isInfo) {
				Rect rect = makeRect(position.x, position.y, size.x, size.y);
				int color = lerpColor(0x80CC9900, 0x80FFFF00, timePhase(platform->time * 3));
				color = lerpColor(setAofArgb(color, 0), color, fadePerc);
				drawRect(rect, color); 
			}

			textProps.position = position;
			drawText(logBuffer->buffer, textProps);
		}

		if (shouldDestroy) logBuffer->buffer[0] = 0;
	}
}

void destroyFont(Font *font) {
	logTo(L_FONT, "Destroying font %s(%d)\n", font->path, font->fontSize);
	destroyTexture(font->texture);
	free(font->charData);

	void deinitSkiaFont(Font *font); //@headerHack
	deinitSkiaFont(font);

	free(font);
}

Font *getFontFromMappingCache(char *ttfPath, int fontSize) {
	for (int i = 0; i < fontSys->fontSizeMappingsNum; i++) {
		FontSizeMapping *mapping = &fontSys->fontSizeMappings[i];
		if (fabs(fontSize - mapping->srcSize) < 1 && streq(mapping->bestFont->path, ttfPath)) {
			mapping->bestFont->lastAccessedTime = platform->time;
			return mapping->bestFont;
		}
	}

	return NULL;
}

void destroyFontSizeMappings(Font *font) {
	for (int i = 0; i < fontSys->fontSizeMappingsNum; i++) {
		FontSizeMapping *mapping = &fontSys->fontSizeMappings[i];
		if (mapping->bestFont == font) {
			arraySpliceIndex(fontSys->fontSizeMappings, fontSys->fontSizeMappingsNum, sizeof(FontSizeMapping), i);
			i--;
			fontSys->fontSizeMappingsNum--;
			continue;
		}
	}
}

void updateFontSystem() {
	for (int i = 0; i < fontSys->fontsNum; i++) {
		Font *font = fontSys->fonts[i];
		if (platform->time - font->lastAccessedTime > 15) {
			destroyFontSizeMappings(font);
			destroyFont(font);
			arraySpliceIndex(fontSys->fonts, fontSys->fontsNum, sizeof(Font *), i);
			fontSys->fontsNum--;
		}
	}
}

void guiDrawFontDebug() {
	if (ImGui::TreeNode(frameSprintf("Fonts loaded [%d/%d]###fontLoaded", fontSys->fontsNum, FONTS_MAX))) {
		for (int i = 0; i < fontSys->fontsNum; i++) {
			Font *font = fontSys->fonts[i];
			ImGui::Text("%s - %d x%g (%gpx)\n", font->path, font->fontSize, font->renderScale, font->fontSize * font->renderScale);
		}
		ImGui::TreePop();
	}

	if (ImGui::TreeNode(frameSprintf("Mappings created [%d/%d]###mappingsCreated", fontSys->fontSizeMappingsNum, FONT_SIZE_MAPPINGS_MAX))) {
		for (int i = 0; i < fontSys->fontSizeMappingsNum; i++) {
			FontSizeMapping *mapping = &fontSys->fontSizeMappings[i];
			Font *font = mapping->bestFont;
			ImGui::Text("%s[%d] - %d x%g (%gpx)\n", font->path, mapping->srcSize, font->fontSize, font->renderScale, font->fontSize * font->renderScale);
		}
		ImGui::TreePop();
	}
}
