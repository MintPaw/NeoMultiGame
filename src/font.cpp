#define FONT_WIDTH_LIMIT 2048
#define FONT_HEIGHT_LIMIT 2048

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
};

struct TextProps {
	Font *font;
	char *string;
	Vec2 position;
	int color; // This probably doesn't need to exist
	float maxWidth;

	int index;
	Vec2 cursor;
	Vec2 scale;
};

struct FontSystem {
	bool disabled;
	Font *defaultFont;
	Font *logFont;
	bool yIsUp;
};

struct DrawTextProps {
	bool skipDraw;
	Font *font;
	Vec2 position;
	int color;
	float maxWidth;
	Vec2 scale;
};

FontSystem *fontSys = NULL;

Font *createFont(const char *ttfPath, int fontSize);

TextProps startText(const char *string, Vec2 position=v2());
bool nextTextChar(TextProps *textProps, Matrix3 *outMatrix, Matrix3 *outUvMatrix, bool *outDisabled);

Vec2 getTextSize(char *string, DrawTextProps props);
Vec2 getTextSize(Font *font, const char *string, float maxWidth=9999);
Vec2 drawText(Font *font, const char *text, Vec2 position, int color=0xFF000000, float maxWidth=9999, bool skipDraw=false, Vec2 scale=v2(1, 1));
void drawTextInRect(char *text, DrawTextProps props, Rect toFit, Vec2 gravity=v2(0.5, 0.5), bool justify=false);
void passTextInRect(char *text, DrawTextProps props, Rect toFit, Vec2 gravity=v2(0.5, 0.5));

DrawTextProps newDrawTextProps();
DrawTextProps newDrawTextProps(Font *font, int color, Vec2 position=v2());
Vec2 drawText(const char *text, DrawTextProps props);
void passText(char *text, DrawTextProps drawTextProps);

void initFonts();
void drawOnScreenLog();
void destroyFont(Font *font);
/// FUNCTIONS ^

void initFonts() {
	if (platform->isCommandLineOnly) return;

	fontSys = (FontSystem *)zalloc(sizeof(FontSystem));

	if (fileExists("assets/common/arial.ttf")) {
		fontSys->defaultFont = createFont("assets/common/arial.ttf", 20);
		fontSys->logFont = createFont("assets/common/arial.ttf", 30);
	} else {
		logf("Need a default font\n");
		Panic("Can't continue");
	}
}

TextProps startText(const char *string, Vec2 position) {
	TextProps props = {};
	props.color = 0xFF000000;
	props.maxWidth = 9999;
	props.string = (char *)string;
	props.position = position;
	props.scale = v2(1, 1);
	return props;
}

bool nextTextChar(TextProps *textProps, Matrix3 *outMatrix, Matrix3 *outUvMatrix, bool *outDisabled) {
	*outUvMatrix = mat3();
	*outMatrix = mat3();
	*outDisabled = false;

	if (textProps->index == strlen(textProps->string)) return false;

	if (textProps->index == 0) {
		if (!textProps->font && fontSys->defaultFont) textProps->font = fontSys->defaultFont;
		textProps->cursor = textProps->position;
	}

	const char *breakableChars = " -";
	bool canLineBreakHere = false;
	if (strchr(breakableChars, textProps->string[textProps->index])) canLineBreakHere = true;

	if (canLineBreakHere) {
		const char *nextBreakableChars = " -\n";
		const char *nextBreakPtr = strpbrk(&textProps->string[textProps->index+1], nextBreakableChars);
		int nextBreakIndex;
		if (nextBreakPtr) nextBreakIndex = nextBreakPtr - textProps->string;
		else nextBreakIndex = strlen(textProps->string);

		int prevWordChar = -1;
		float wordWidth = 0;
		// printf("Going from %d to %d\n", i, nextBreakIndex);
		for (int i = textProps->index; i < nextBreakIndex; i++) {
			char curWordChar = textProps->string[i];

			stbtt_packedchar *charData = &textProps->font->charData[curWordChar];
			int charWidth = charData->xadvance; //@todo Kerning support for wordwrap would go here, if we used kerning at all...

			wordWidth += charWidth;
			prevWordChar = curWordChar;
		}

		if (textProps->cursor.x - textProps->position.x + wordWidth > textProps->maxWidth) {
			textProps->cursor.x = textProps->position.x;
			if (fontSys->yIsUp) {
				textProps->cursor.y -= textProps->font->lineSpacing * textProps->scale.y;
			} else {
				textProps->cursor.y += textProps->font->lineSpacing * textProps->scale.y;
			}
		}
	}

	char curChar = textProps->string[textProps->index];

	if (curChar == '\n') {
		textProps->cursor.x = textProps->position.x;
		if (fontSys->yIsUp) {
			textProps->cursor.y -= textProps->font->lineSpacing * textProps->scale.y;
		} else {
			textProps->cursor.y += textProps->font->lineSpacing * textProps->scale.y;
		}
		*outDisabled = true;
	} else if (curChar == ' ' && textProps->cursor.x == textProps->position.x) { // Para holdover?
		*outDisabled = true;
	} else {
		stbtt_packedchar *charData = &textProps->font->charData[curChar];

		Vec2 charDataOff = v2();
		charDataOff.x = charData->xoff;
		if (fontSys->yIsUp) { 
			float lineHeight = textProps->font->ascent - textProps->font->descent;
			charDataOff.y += lineHeight - (charData->y1 - charData->y0);
			charDataOff.y -= charData->yoff;
			charDataOff.y -= textProps->font->ascent;
		} else {
			charDataOff.y = charData->yoff + textProps->font->ascent;
		}
		outMatrix->TRANSLATE(textProps->cursor + charDataOff * textProps->scale);

		float srcX = charData->x0;
		float srcY = charData->y0;
		float srcWidth = charData->x1 - charData->x0;
		float srcHeight = charData->y1 - charData->y0;
		float textureWidth = textProps->font->texture->width;
		float textureHeight = textProps->font->texture->height;

		if (fontSys->yIsUp) {
			outUvMatrix->SCALE(1, -1);
			outUvMatrix->TRANSLATE(srcX/textureWidth, srcY/textureHeight);
			outUvMatrix->SCALE(srcWidth/textureWidth, srcHeight/textureHeight);
			outUvMatrix->TRANSLATE(0, 1);
			outUvMatrix->SCALE(1, -1);
		} else {
			outUvMatrix->TRANSLATE(srcX/textureWidth, srcY/textureHeight);
			outUvMatrix->SCALE(srcWidth/textureWidth, srcHeight/textureHeight);
		}
		outMatrix->SCALE(srcWidth, srcHeight);

		if (curChar == ' ') *outDisabled = true;

		textProps->cursor.x += charData->xadvance * textProps->scale.x;
	}

	outMatrix->SCALE(textProps->scale);

	textProps->index++;
	return true;
}

Font *createFont(const char *ttfPath, int fontSize) {
	if (!fileExists(ttfPath)) {
		logf("Can't find font at %s\n", ttfPath);
		return fontSys->defaultFont;
	}

	Font *font = (Font *)zalloc(sizeof(Font));
	font->path = stringClone(ttfPath);
	font->fontSize = fontSize;

	int ttfDataSize;
	void *ttfData = readFile(ttfPath, &ttfDataSize);

	unsigned char *temp1bppPixels = (unsigned char *)malloc(FONT_WIDTH_LIMIT * FONT_HEIGHT_LIMIT);
	stbtt_pack_context packContext;

	int good = 0;
	good = stbtt_PackBegin(&packContext, temp1bppPixels, FONT_WIDTH_LIMIT, FONT_HEIGHT_LIMIT, 0, 1, NULL);
	if (!good) Panic("Faied to PackBegin");

	// stbtt_PackSetOversampling(&packContext, 2, 2);

	int totalChars = 127;
	font->charData = (stbtt_packedchar *)malloc(totalChars * sizeof(stbtt_packedchar));
	good = stbtt_PackFontRange(&packContext, (unsigned char *)ttfData, 0, fontSize, 0, totalChars, font->charData);
	if (!good) Panic("Failed to PackFontRange");

	stbtt_PackEnd(&packContext);

	stbtt_fontinfo info;
	good = stbtt_InitFont(&info, (unsigned char *)ttfData, 0);
	if (!good) Panic("Failed to init font");

	int ascent, descent, lineGap;
	stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
	float scale = stbtt_ScaleForPixelHeight(&info, fontSize);
	font->ascent = ascent * scale;
	font->descent = descent * scale;
	font->lineGap = lineGap * scale;
	font->lineSpacing = font->ascent - font->descent + font->lineGap;

#if 0 // Output 1bpp image
	{
		char outputPath[256];
		strcpy(outputPath, platform->filePathPrefix);
		strcat(outputPath, "assets/output1bbpBitmapFont.png");
		good = stbi_write_png(outputPath, FONT_WIDTH_LIMIT, FONT_HEIGHT_LIMIT, 1, temp1bppPixels, FONT_WIDTH_LIMIT);
		if (!good) {
			printf("Failed to write image\n");
			Assert(0);
		}
	}
#endif

	unsigned char *temp4bppPixels = (unsigned char *)malloc(FONT_WIDTH_LIMIT * FONT_HEIGHT_LIMIT * 4);
	memset(temp4bppPixels, 0, FONT_WIDTH_LIMIT * FONT_HEIGHT_LIMIT * 4);
	for (int y = 0; y < FONT_HEIGHT_LIMIT; y++) {
		for (int x = 0; x < FONT_WIDTH_LIMIT; x++) {
			unsigned char byte = temp1bppPixels[y * FONT_WIDTH_LIMIT + x];
			temp4bppPixels[(y * FONT_WIDTH_LIMIT + x) * 4 + 0] = 255;
			temp4bppPixels[(y * FONT_WIDTH_LIMIT + x) * 4 + 1] = 255;
			temp4bppPixels[(y * FONT_WIDTH_LIMIT + x) * 4 + 2] = 255;
			temp4bppPixels[(y * FONT_WIDTH_LIMIT + x) * 4 + 3] = byte;
		}
	}

#if 0 // Output 4bpp image
	{
		char outputPath[256];
		strcpy(outputPath, platform->filePathPrefix);
		strcat(outputPath, "assets/output4bbpBitmapFont.png");
		good = stbi_write_png(outputPath, FONT_WIDTH_LIMIT, FONT_HEIGHT_LIMIT, 4, temp4bppPixels, FONT_WIDTH_LIMIT * 4);
		if (!good) {
			printf("Failed to write image\n");
			Assert(0);
		}
	}
#endif

	/// Flip for opengl
	unsigned char tempRow[FONT_WIDTH_LIMIT * 4];
	for (int y = 0; y < FONT_HEIGHT_LIMIT/2; y++) {
		int curTopRow = y;
		int curBottomRow = FONT_HEIGHT_LIMIT - y - 1;
		unsigned char *topRowStart = temp4bppPixels + curTopRow * FONT_WIDTH_LIMIT * 4;
		unsigned char *bottomRowStart = temp4bppPixels + curBottomRow * FONT_WIDTH_LIMIT * 4;

		memcpy(tempRow, topRowStart, FONT_WIDTH_LIMIT * 4);
		memcpy(topRowStart, bottomRowStart, FONT_WIDTH_LIMIT * 4);
		memcpy(bottomRowStart, tempRow, FONT_WIDTH_LIMIT * 4);
	}

	font->texture = createTexture(FONT_WIDTH_LIMIT, FONT_HEIGHT_LIMIT, temp4bppPixels);

	free(temp1bppPixels);
	free(temp4bppPixels);
	return font;
}

Vec2 getTextSize(char *string, DrawTextProps props) {
	props.skipDraw = true;
	return drawText(string, props);
}

Vec2 getTextSize(Font *font, const char *string, float maxWidth) {
	return drawText(font, string, v2(), 0xFFFFFFFF, maxWidth, true);
}

Vec2 drawText(Font *font, const char *text, Vec2 position, int color, float maxWidth, bool skipDraw, Vec2 scale) {
	DrawTextProps props = newDrawTextProps(font, color, position);
	props.maxWidth = maxWidth;
	props.scale = scale;
	props.skipDraw = skipDraw;
	props.scale = scale;
	return drawText(text, props);
}

DrawTextProps newDrawTextProps() {
	DrawTextProps props = {};
	props.font = fontSys->defaultFont;
	props.color = 0xFF000000;
	props.maxWidth = 9999;
	props.scale = v2(1, 1);
	return props;
}

DrawTextProps newDrawTextProps(Font *font, int color, Vec2 position) {
	DrawTextProps props = newDrawTextProps();
	props.font = font;
	props.color = color;
	props.position = position;
	return props;
}

Vec2 drawText(const char *text, DrawTextProps drawTextProps) {
	if (fontSys->disabled) return v2(1, 1);
	if (!text) return v2(1, 1);
	if (!drawTextProps.font) {
		logf("Tried to draw text '%s' with no font\n", text);
		return v2(1, 1);
	}

	TextProps textProps = startText(text, drawTextProps.position);
	textProps.font = drawTextProps.font;
	textProps.maxWidth = drawTextProps.maxWidth;
	textProps.scale = drawTextProps.scale;
	textProps.color = drawTextProps.color;

	Matrix3 charMatrix;
	Matrix3 charUvMatrix;
	bool charDisabled;
	Vec2 textSize = v2();
	while (nextTextChar(&textProps, &charMatrix, &charUvMatrix, &charDisabled)) {
		if (textSize.x < textProps.cursor.x) textSize.x = textProps.cursor.x;
		if (drawTextProps.skipDraw || charDisabled) continue;

		RenderProps props = newRenderProps();
		props.tint = textProps.color;
		props.srcWidth = 1;
		props.srcHeight = 1;
		props.matrix = charMatrix;
		props.uvMatrix = charUvMatrix;
		drawTexture(textProps.font->texture, props);
	}

	textSize.y = textProps.cursor.y + textProps.font->lineSpacing * textProps.scale.y;
	return textSize - drawTextProps.position;
}

void passText(char *text, DrawTextProps drawTextProps) {
	if (fontSys->disabled) return;
	if (!text) return;
	if (!drawTextProps.font) {
		logf("Tried to draw text '%s' with no font\n", text);
		return;
	}
	Pass *pass = getCurrentPass();

	bool oldYIsUp = fontSys->yIsUp;
	fontSys->yIsUp = pass->yIsUp;

	Vec2 textSize = v2();

	TextProps textProps = startText(text, drawTextProps.position);
	textProps.font = drawTextProps.font;
	textProps.maxWidth = drawTextProps.maxWidth;
	textProps.scale = drawTextProps.scale;
	textProps.color = drawTextProps.color;

	Matrix3 charMatrix;
	Matrix3 charUvMatrix;
	bool charDisabled;
	while (nextTextChar(&textProps, &charMatrix, &charUvMatrix, &charDisabled)) {
		if (textSize.x < textProps.cursor.x) textSize.x = textProps.cursor.x;
		if (drawTextProps.skipDraw || charDisabled) continue;

		Vec2 uv0 = charUvMatrix * v2(0, 0);
		Vec2 uv1 = charUvMatrix * v2(1, 1);
		passTexture(textProps.font->texture, charMatrix, textProps.color, uv0, uv1);
	}

	textSize.y = textProps.cursor.y + textProps.font->lineSpacing * drawTextProps.scale.y;
	textSize -= drawTextProps.position;

	fontSys->yIsUp = oldYIsUp;
}

void drawTextInRect(char *text, DrawTextProps props, Rect toFit, Vec2 gravity, bool justify) {
	Vec2 size = getTextSize(props.font, text);
	Rect textRect = getInnerRectOfAspect(toFit, size, gravity);
	Vec2 textScale = getSize(textRect) / size;

	props.scale = textScale;
	props.position = getPosition(textRect);

	if (!justify) {
		drawText(text, props);
		return;
	}

	int linesNum = 0;
	char **lines = frameSplitString(text, "\n", &linesNum);
	Vec2 *lineSize = (Vec2 *)frameMalloc(sizeof(Vec2) * linesNum);

	float widestLine = 0;
	for (int i = 0; i < linesNum; i++) {
		char *line = lines[i];
		lineSize[i] = getTextSize(line, props);
		if (widestLine < lineSize[i].x) widestLine = lineSize[i].x;
	}

	for (int i = 0; i < linesNum; i++) {
		char *line = lines[i];

		DrawTextProps lineProps = props;
		lineProps.position.x += (widestLine - lineSize[i].x)/2;
		drawText(line, lineProps);

		props.position.y += lineSize[i].y;
	}
}

void passTextInRect(char *text, DrawTextProps props, Rect toFit, Vec2 gravity) {
	Vec2 size = getTextSize(props.font, text);

	Rect textRect = getInnerRectOfAspect(toFit, size, gravity);

	props.scale.x = textRect.width / size.x;
	props.scale.y = textRect.height / size.y;
	props.position = getPosition(textRect);
	passText(text, props);
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
		float timeSinceLastLog = logSys->time - logBuffer->logTime;

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

		Vec2 size = getTextSize(fontSys->logFont, logBuffer->buffer, platform->windowWidth);
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

			drawText(fontSys->logFont, logBuffer->buffer, position, color, platform->windowWidth);
		}

		if (shouldDestroy) logBuffer->buffer[0] = 0;
	}
}

void destroyFont(Font *font) {
	if (font == fontSys->defaultFont) return;
	destroyTexture(font->texture);
	free(font->charData);
	free(font);
}
