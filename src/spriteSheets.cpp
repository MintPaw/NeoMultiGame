#define SPRITE_SHEET_WIDTH_MAX 2048
#define SPRITE_SHEET_HEIGHT_MAX 2048

struct SpriteSheetImage {
	int srcX;
	int srcY;
	int srcWidth;
	int srcHeight;

	int destOffX;
	int destOffY;
	int width;
	int height;
};

struct SpriteSheet {
	char path[PATH_MAX_LEN];
	Texture *texture;

	SpriteSheetImage *images;
	int imagesNum;
};

#define CACHED_SPRITE_SHEETS_MAX 128
SpriteSheet *_cachedSpriteSheets[CACHED_SPRITE_SHEETS_MAX];
int _cachedSpriteSheetsNum;

SpriteSheet *getSpriteSheet(char *dir);
SpriteSheet *createSpriteSheet(char *dir);
void destroySpriteSheet(SpriteSheet *sheet);
void clearSpriteSheetCache();
/// FUNCTIONS ^

SpriteSheet *getSpriteSheet(char *dir) {
  for (int i = 0; i < _cachedSpriteSheetsNum; i++) {
    SpriteSheet *cachedSpriteSheet = _cachedSpriteSheets[i];
    if (streq(cachedSpriteSheet->path, dir)) {
      return cachedSpriteSheet;
    }
  }

  return createSpriteSheet(dir);
}

SpriteSheet *createSpriteSheet(char *dir) {
	if (!directoryExists(dir)) return NULL;

	SpriteSheet *sheet = (SpriteSheet *)zalloc(sizeof(SpriteSheet));
  strcpy(sheet->path, dir);

	int pathsNum;
	char **paths = getFrameDirectoryList(dir, &pathsNum, false, true, true);

	bool goodDir = true;
	for (int i = 0; i < pathsNum; i++) {
		char *path = paths[i];
		if (!stringEndsWith(path, ".png")) goodDir = false;
	}
	if (!goodDir) return NULL;

	sheet->images = (SpriteSheetImage *)zalloc(sizeof(SpriteSheetImage) * pathsNum);

	stbrp_rect *rects = (stbrp_rect *)frameMalloc(pathsNum * sizeof(stbrp_rect));

	u8 **srcBitmaps = (u8 **)frameMalloc(sizeof(u8 *) * pathsNum);

	for (int i = 0; i < pathsNum; i++) {
		char *path = paths[i];

		stbrp_rect *stbRect = &rects[sheet->imagesNum];
		SpriteSheetImage *image = &sheet->images[sheet->imagesNum++];

		int pngDataSize;
		u8 *pngData = (u8 *)readFile(path, &pngDataSize);

		int width, height, channels;
		stbi_set_flip_vertically_on_load(false);
		srcBitmaps[i] = stbi_load_from_memory(pngData, pngDataSize, &width, &height, &channels, 4);
		stbi_set_flip_vertically_on_load(true);
		free(pngData);

		{ // Add trimming here!
			image->srcWidth = width;
			image->srcHeight = height;

			image->destOffX = 0;
			image->destOffY = 0;
			image->width = width;
			image->height = height;
		}

		stbRect->id = i;
		stbRect->w = image->srcWidth;
		stbRect->h = image->srcHeight;
	}

	stbrp_context context;
	int nodesMax = SPRITE_SHEET_WIDTH_MAX;
	stbrp_node *nodes = (stbrp_node *)frameMalloc(sizeof(stbrp_node) * nodesMax);
	stbrp_init_target(&context, SPRITE_SHEET_WIDTH_MAX-1, SPRITE_SHEET_HEIGHT_MAX-1, nodes, nodesMax);

	if (!stbrp_pack_rects(&context, rects, sheet->imagesNum)) {
		logf("Failed to pack rect!\n");
	}

	u8 *bitmapData = (u8 *)zalloc(SPRITE_SHEET_WIDTH_MAX * SPRITE_SHEET_HEIGHT_MAX * 4);
	memset(bitmapData, 0, SPRITE_SHEET_WIDTH_MAX * SPRITE_SHEET_HEIGHT_MAX * 4);

	int padding = 2;
	for (int i = 0; i < sheet->imagesNum; i++) {
		stbrp_rect *rpRect = &rects[i];

		SpriteSheetImage *image = &sheet->images[i];

		int startX = 0;
		int startY = 0;
		int endX = image->width;
		int endY = image->height;

		image->srcX = rpRect->x + startX + padding;
		image->srcY = rpRect->y + startY + padding;
		// image->srcY -= SPRITE_SHEET_HEIGHT_MAX - rpRect->h;

		for (int y = startY; y < endY; y++) {
			for (int x = startX; x < endX; x++) {
				u8 *readStart = &srcBitmaps[i][((y+image->destOffY)*image->srcWidth + (x+image->destOffX))*4];

				int writeY = rpRect->y + padding + y;
				int writeX = rpRect->x + padding + x;
				u8 *writeStart = &bitmapData[(writeY*SPRITE_SHEET_WIDTH_MAX + writeX)*4];

				int pixel = *(int *)readStart;

				*(int *)writeStart = pixel;
				// memcpy(writeStart, readStart, 4);
			}
		}

		free(srcBitmaps[i]);
	}

	sheet->texture = createTexture(SPRITE_SHEET_WIDTH_MAX, SPRITE_SHEET_HEIGHT_MAX);
	setTextureData(sheet->texture, bitmapData, SPRITE_SHEET_WIDTH_MAX, SPRITE_SHEET_HEIGHT_MAX, _F_TD_FLIP_Y);
	free(bitmapData);

  _cachedSpriteSheets[_cachedSpriteSheetsNum++] = sheet;

	return sheet;
}

void destroySpriteSheet(SpriteSheet *sheet) {
	destroyTexture(sheet->texture);
	free(sheet->images);
	free(sheet);
}

void clearSpriteSheetCache() {
  for (int i = 0; i < _cachedSpriteSheetsNum; i++) {
    SpriteSheet *cachedSpriteSheet = _cachedSpriteSheets[i];
    destroySpriteSheet(cachedSpriteSheet);
  }

  _cachedSpriteSheetsNum = 0;
}
