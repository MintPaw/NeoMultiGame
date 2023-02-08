bool swfLogShapeInfo = false;

struct SwfDataStream {
	u8 *data;
	int size;
	int byteIndex;
	u8 currentByte;
	int bitIndex;

	void *toFree;
};

void skipBytes(SwfDataStream *stream, u32 count) {
	if (count <= 0) return;

	stream->bitIndex = 0;
	stream->byteIndex += count;
	if (stream->size - stream->byteIndex < 0) return;
}

void byteAlign(SwfDataStream *stream) {
	if (stream->bitIndex > 0) {
		stream->bitIndex = 0;
		// stream->byteIndex++;
	}
}

void word32Align(SwfDataStream *stream) {
	byteAlign(stream);
	while (stream->byteIndex % 4 != 0) {
		stream->byteIndex++;
	}
}

u8 read(SwfDataStream *stream) {
	stream->bitIndex = 0;
	byteAlign(stream);
	u8 ret = stream->data[stream->byteIndex++];
	return ret;
}

u8 readNoBitReset(SwfDataStream *stream) {
	u8 ret = stream->data[stream->byteIndex++];
	return ret;
}

u16 readU16(SwfDataStream *stream) { return ((u16)read(stream) + ((u16)read(stream) << 8)); }
u32 readU32(SwfDataStream *stream) { return ((u32)read(stream) + ((u32)read(stream) << 8) + ((u32)read(stream) << 16) + ((u32)read(stream) << 24)); }

u32 readEncodedU32(SwfDataStream *stream) {
	u32 ret = read(stream);
	if (!(ret & 0x00000080)) return ret;
	ret = (ret & 0x0000007F) | (read(stream)) << 7;
	if(!(ret & 0x00004000)) return ret;
	ret = (ret & 0x00003FFF) | (read(stream)) << 14;
	if (!(ret & 0x00200000)) return ret;
	ret = (ret & 0x001FFFFF) | (read(stream)) << 21;
	if (!(ret & 0x10000000)) return ret;
	ret = (ret & 0xFFFFFFFF) | (read(stream)) << 28;

	return ret;
}

float readFixed(SwfDataStream *stream) {
	int afterPoint = readU16(stream);
	int beforePoint = readU16(stream);
	float ret = ((float) ((beforePoint << 16) + afterPoint)) / 65536;
	return ret;
}

float readFixed8(SwfDataStream *stream) {
	int afterPoint = read(stream);
	int beforePoint = read(stream);

	if (beforePoint >= 0x80) {
		beforePoint = -(((~beforePoint) & 0xff) + 1);
	}

	float ret = beforePoint + (((float) afterPoint) / 256.0);
	return ret;
}

float readFloat(SwfDataStream *stream) {
	u32 ret = readU32(stream);
	return *((float*) &ret);
}

u64 readUB(SwfDataStream *stream, u32 nBits) { // https://github.com/Yukariin/libswf/blob/master/src/DataStream.cpp#L109 12/27/20
	Assert(nBits <= 56);
	if (nBits == 0) return 0;

	u64 ret = 0;
	if (stream->bitIndex == 0) {
		stream->currentByte = readNoBitReset(stream);
	}

	for (int bit = 0; bit < nBits; bit++) {
		int nb = (stream->currentByte >> (7 - stream->bitIndex)) & 1;
		ret += (nb << (nBits - 1 - bit));
		stream->bitIndex++;
		if (stream->bitIndex == 8) {
			stream->bitIndex = 0;
			if (bit != nBits - 1) stream->currentByte = readNoBitReset(stream);
		}
	}

	return ret;
}

s64 readSB(SwfDataStream *stream, u32 nBits) {
	u64 ret = readUB(stream, nBits);
	if (ret >> (nBits - 1)) {
		ret |= (0xFFFFFFFFFFFFFFFF >> nBits) << nBits;
	}

	return (s64)ret;
}

float readFB(SwfDataStream *stream, u32 nBits) {
	float ret = readSB(stream, nBits);
	ret = ret / 0x10000;
	return ret;
}

char *readString(SwfDataStream *stream) {
	int startingByteIndex = stream->byteIndex;

	int count = 0;
	for (;;) {
		u8 r = read(stream);
		count++;
		if (r == 0) break;
	}

	stream->byteIndex = startingByteIndex;
	char *str = (char *)malloc(count);
	for (int i = 0; i < count; i++) {
		str[i] = read(stream);
	}

	return str;
}

void skipString(SwfDataStream *stream) {
	for (;;) {
		u8 r = read(stream);
		if (r == 0) break;
	}
}

struct SwfRect {
	Vec2 min;
	Vec2 max;
};

SwfRect readRect(SwfDataStream *stream) {
	int dimmBits = readUB(stream, 5);
	int Xmin = readSB(stream, dimmBits);
	int Xmax = readSB(stream, dimmBits);
	int Ymin = readSB(stream, dimmBits);
	int Ymax = readSB(stream, dimmBits);
	SwfRect ret;
	ret.min.x = (float)Xmin/20.0;
	ret.min.y = (float)Ymin/20.0;
	ret.max.x = (float)Xmax/20.0;
	ret.max.y = (float)Ymax/20.0;
	// Rect ret;
	// ret.x = (float)Xmin/20.0;
	// ret.y = (float)Ymin/20.0;
	// ret.width = (float)Xmax/20.0 - ret.x;
	// ret.height = (float)Ymax/20.0 - ret.y;

	byteAlign(stream);
	return ret;
}

Rect toRect(SwfRect swfRect) {
	Rect ret;
	ret.x = swfRect.min.x;
	ret.y = swfRect.min.y;
	ret.width = swfRect.max.x - swfRect.min.x;
	ret.height = swfRect.max.y - swfRect.min.y;
	return ret;
}

int readRgb(SwfDataStream *stream) {
	u8 a = 255;
	u8 r = read(stream);
	u8 g = read(stream);
	u8 b = read(stream);
	return argbToHex(a, r, g, b);
}

int readArgb(SwfDataStream *stream) {
	u8 a = read(stream);
	u8 r = read(stream);
	u8 g = read(stream);
	u8 b = read(stream);
	return argbToHex(a, r, g, b);
}

int readRgba(SwfDataStream *stream) {
	u8 r = read(stream);
	u8 g = read(stream);
	u8 b = read(stream);
	u8 a = read(stream);
	return argbToHex(a, r, g, b);
}

Matrix3 readMatrix(SwfDataStream *stream) {
	float scaleX = 1;
	float scaleY = 1;
	bool hasScale = readUB(stream, 1);
	if (hasScale) {
		int scaleBits = readUB(stream, 5);
		scaleX = readFB(stream, scaleBits);
		scaleY = readFB(stream, scaleBits);
	}

	float rotateSkew0 = 0;
	float rotateSkew1 = 0;
	bool hasRotate = readUB(stream, 1);
	if (hasRotate) {
		int rotateBits = readUB(stream, 5);
		rotateSkew0 = readFB(stream, rotateBits);
		rotateSkew1 = readFB(stream, rotateBits);
	}

	int translateBits = readUB(stream, 5);
	float translateX = (float)readSB(stream, translateBits) / 20.0;
	float translateY = (float)readSB(stream, translateBits) / 20.0;

	byteAlign(stream);

	float sx = scaleX;
	float sy = scaleY;
	float tx = translateX;
	float ty = translateY;
	float s0 = rotateSkew0;
	float s1 = rotateSkew1;

	Matrix3 ret = {
		sx, s0, 0,
		s1, sy, 0,
		tx, ty, 1
	};
	return ret;
}

SkMatrix toSkMatrix(Matrix3 mat) {
	float sx = mat.data[0];
	float sy = mat.data[4];
	float s0 = mat.data[1];
	float s1 = mat.data[3];
	float tx = mat.data[6];
	float ty = mat.data[7];
	SkMatrix ret;
	ret.setAll(sx, s1, tx, s0, sy, ty, 0, 0, 1);
	return ret;
}

struct GradeCord {
	u8 ratio;
	int color;
};
struct Gradient {
	bool isFocal;
	u8 spreadMode;
	u8 interpolationMode;
	GradeCord *coords;
	int coordsNum;

	float focalPoint;
};
Gradient readGradient(SwfDataStream *stream, int shapeNum, bool isFocal) {
	Gradient ret = {};
	ret.spreadMode = readUB(stream, 2);
	ret.interpolationMode = readUB(stream, 2);
	int numGradients = readUB(stream, 4);

	ret.coords = (GradeCord *)zalloc(sizeof(GradeCord) * numGradients);
	for (int i = 0; i < numGradients; i++) {
		GradeCord *coord = &ret.coords[ret.coordsNum++];
		coord->ratio = read(stream);
		if (shapeNum >= 3) {
			coord->color = readRgba(stream);
		} else {
			coord->color = readRgb(stream);
		}
	}

	if (isFocal) ret.focalPoint = readFixed8(stream);

	return ret;
}


enum SwfBitmapFormat {
	SWF_BITMAP_FORMAT_8=3,
	SWF_BITMAP_FORMAT_15=4,
	SWF_BITMAP_FORMAT_24=5,
};
struct SwfBitmap {
	int characterId;
	int width;
	int height;
	u8 *pixels;

	void *bitmapRuntimePointer;
};
enum FillStyleType {
	FILL_STYLE_SOLID=0x00,
	FILL_STYLE_LINEAR_GRADIENT=0x10,
	FILL_STYLE_RADIAL_GRADIENT=0x12,
	FILL_STYLE_FOCAL_RADIAL_GRADIENT=0x13,
	FILL_STYLE_REPEATING_BITMAP=0x40,
	FILL_STYLE_CLIPPED_BITMAP=0x41,
	FILL_STYLE_NON_SMOOTHED_REPEATING_BITMAP=0x42,
	FILL_STYLE_NON_SMOOTHED_CLIPPED_BITMAP=0x43,
};
struct FillStyle {
	FillStyleType fillStyleType;
	int color;
	Matrix3 matrix;
	Gradient gradient;
	u16 bitmapId;
	SwfBitmap *bitmap;
};
FillStyle readFillStyle(SwfDataStream *stream, int shapeNum) {
	FillStyle ret;
	ret.fillStyleType = (FillStyleType)read(stream);
	if (ret.fillStyleType == FILL_STYLE_SOLID) {
		if (shapeNum >= 3) ret.color = readRgba(stream);
		else ret.color = readRgb(stream);
	}

	if (
		ret.fillStyleType == FILL_STYLE_LINEAR_GRADIENT ||
		ret.fillStyleType == FILL_STYLE_RADIAL_GRADIENT ||
		ret.fillStyleType == FILL_STYLE_FOCAL_RADIAL_GRADIENT
	) {
		bool isFocal = ret.fillStyleType == FILL_STYLE_FOCAL_RADIAL_GRADIENT;
		ret.matrix = readMatrix(stream);
		ret.gradient = readGradient(stream, shapeNum, isFocal);
	} else if (
		ret.fillStyleType == FILL_STYLE_REPEATING_BITMAP ||
		ret.fillStyleType == FILL_STYLE_CLIPPED_BITMAP ||
		ret.fillStyleType == FILL_STYLE_NON_SMOOTHED_REPEATING_BITMAP ||
		ret.fillStyleType == FILL_STYLE_NON_SMOOTHED_CLIPPED_BITMAP
	) {
		ret.bitmapId = readU16(stream);
		ret.matrix = readMatrix(stream);
	}

	return ret;
}

enum CapStyle {
	CAP_STYLE_ROUND=0,
	CAP_STYLE_NONE=1,
	CAP_STYLE_SQUARE=2,
};
enum JoinStyle {
	JOIN_STYLE_ROUND=0,
	JOIN_STYLE_BEVEL=1,
	JOIN_STYLE_MITER=2,
};
struct LineStyle {
	float width;
	int color;
	float miterLimitFactor;
	u8 startCapStyle;
	u8 joinStyle;
};
LineStyle readLineStyle(SwfDataStream *stream, int shapeNum) {
	LineStyle ret = {};
	ret.width = (float)readU16(stream) / 20.0;

	bool hasFillFlag = false;
	if (shapeNum == 1 || shapeNum == 2) {
		ret.color = readRgb(stream);
	} else if (shapeNum == 3) {
		ret.color = readRgba(stream);
	} else {
		ret.startCapStyle = readUB(stream, 2);
		ret.joinStyle = readUB(stream, 2);
		hasFillFlag = readUB(stream, 1);
		readUB(stream, 1); // noHScaleFlag 
		readUB(stream, 1); // noVScaleFlag
		readUB(stream, 1); // pixelHintingFlag 
		readUB(stream, 5); // reserved
		readUB(stream, 1); // noClose
		readUB(stream, 2); // endCapStyle 

		if (ret.joinStyle == JOIN_STYLE_MITER) {
			ret.miterLimitFactor = readFixed8(stream);
		}

		if (hasFillFlag) {
			// fillStyle = readFillStyle(stream, shapeNum); //@incomplete I don't use this fill style?
		} else {
			ret.color = readRgba(stream);
		}
	}

	return ret;
}

// struct SwfColorTransform {
// 	s16 redMultTerm;
// 	s16 greenMultTerm;
// 	s16 blueMultTerm;
// 	s16 redAddTerm;
// 	s16 greenAddTerm;
// 	s16 blueAddTerm;
// };
// SwfColorTransform readColorTransform(SwfDataStream *stream) {
// 	SwfColorTransform ret = {};

// 	bool hasAddTerms = readUB(stream, 1);
// 	bool hasMultTerms = readUB(stream, 1);
// 	int bits = readUB(stream, 4);

// 	if (hasMultTerms) {
// 		ret.redMultTerm = readSB(stream, bits);
// 		ret.greenMultTerm = readSB(stream, bits);
// 		ret.blueMultTerm = readSB(stream, bits);
// 	}
// 	if (hasAddTerms) {
// 		ret.redAddTerm = readSB(stream, bits);
// 		ret.greenAddTerm = readSB(stream, bits);
// 		ret.blueAddTerm = readSB(stream, bits);
// 	}

// 	byteAlign(stream);
// 	return ret;
// }

struct ColorTransform {
	float redMultTerm;
	float greenMultTerm;
	float blueMultTerm;
	float alphaMultTerm;

	s16 redAddTerm;
	s16 greenAddTerm;
	s16 blueAddTerm;
	s16 alphaAddTerm;
};
ColorTransform readColorTransformWithAlpha(SwfDataStream *stream) {
	ColorTransform ret = {};

	bool hasAddTerms = readUB(stream, 1);
	bool hasMultTerms = readUB(stream, 1);
	int bits = readUB(stream, 4);

	if (hasMultTerms) {
		ret.redMultTerm = readSB(stream, bits) / 256.0;
		ret.greenMultTerm = readSB(stream, bits) / 256.0;
		ret.blueMultTerm = readSB(stream, bits) / 256.0;
		ret.alphaMultTerm = readSB(stream, bits) / 256.0;
	} else {
		ret.redMultTerm = 1;
		ret.greenMultTerm = 1;
		ret.blueMultTerm = 1;
		ret.alphaMultTerm = 1;
	}

	if (hasAddTerms) {
		ret.redAddTerm = readSB(stream, bits);
		ret.greenAddTerm = readSB(stream, bits);
		ret.blueAddTerm = readSB(stream, bits);
		ret.alphaAddTerm = readSB(stream, bits);
	}

	byteAlign(stream);
	return ret;
}
ColorTransform makeColorTransform() {
	ColorTransform trans = {};
	trans.redMultTerm = 1;
	trans.greenMultTerm = 1;
	trans.blueMultTerm = 1;
	trans.alphaMultTerm = 1;
	return trans;
}
int applyColorTransform(int color, ColorTransform transform) {
#if 0
	return color;
#else
	int a = (color >> 24) & 0xff;
	int r = (color >> 16) & 0xff;
	int g = (color >> 8) & 0xff;
	int b = color & 0xff;
	r = mathClamp((r * transform.redMultTerm) + transform.redAddTerm, 0, 255);
	g = mathClamp((g * transform.greenMultTerm) + transform.greenAddTerm, 0, 255);
	b = mathClamp((b * transform.blueMultTerm) + transform.blueAddTerm, 0, 255);
	a = mathClamp((a * transform.alphaMultTerm) + transform.alphaAddTerm, 0, 255);

	// ColorTransform freshTransform = makeColorTransform();
	// if (memcmp(&transform, &freshTransform, sizeof(ColorTransform)) != 0) {
	// 	int k =213213;
	// }

	return ((a & 0xFF) << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
#endif
}

ColorTransform applyColorTransform(ColorTransform orig, ColorTransform next) {
	ColorTransform ret = {};
#if 0
	ret = next;
#else
	// ret = next;
	ret.redMultTerm = orig.redMultTerm * next.redMultTerm;
	ret.greenMultTerm = orig.greenMultTerm * next.greenMultTerm;
	ret.blueMultTerm = orig.blueMultTerm * next.blueMultTerm;
	ret.alphaMultTerm = orig.alphaMultTerm * next.alphaMultTerm;

	ret.redAddTerm = orig.redAddTerm + next.redAddTerm;
	ret.greenAddTerm = orig.greenAddTerm + next.greenAddTerm;
	ret.blueAddTerm = orig.blueAddTerm + next.blueAddTerm;
	ret.alphaAddTerm = orig.alphaAddTerm + next.alphaAddTerm;
#endif
	return ret;
}

enum SwfTagType {
	SWF_TAG_END=0,
	SWF_TAG_FILE_ATTRIBUTES=69,
	SWF_TAG_SET_BACKGROUND_COLOR=9,
	SWF_TAG_DEFINE_SCENE_AND_FRAME_LABEL_DATA=86,
	SWF_TAG_DEFINE_FONT=10,
	SWF_TAG_DEFINE_FONT3=75,
	SWF_TAG_DEFINE_FONT_INFO=13,
	SWF_TAG_DEFINE_FONT_INFO2=62,
	SWF_TAG_DEFINE_TEXT=11,
	SWF_TAG_DEFINE_FONT_ALIGN_ZONES=73,
	SWF_TAG_DEFINE_FONT_NAME=88,
	SWF_TAG_DEFINE_SHAPE=2,
	SWF_TAG_DEFINE_SHAPE2=22,
	SWF_TAG_DEFINE_SHAPE3=32,
	SWF_TAG_DEFINE_SHAPE4=83,
	SWF_TAG_DEFINE_SPRITE=39,
	SWF_TAG_PLACE_OBJECT=4,
	SWF_TAG_PLACE_OBJECT2=26,
	SWF_TAG_PLACE_OBJECT3=70,
	SWF_TAG_SHOW_FRAME=1,
	SWF_TAG_REMOVE_OBJECT=5,
	SWF_TAG_REMOVE_OBJECT2=28,
	SWF_TAG_DO_ABC2=82,
	SWF_TAG_SYMBOL_CLASS=76,
	SWF_TAG_ENABLE_DEBUGGER2=64,
	SWF_TAG_FRAME_LABEL=43,
	SWF_TAG_DEFINE_EDIT_TEXT=37,
	SWF_TAG_CSM_TEXT_SETTINGS=74,
	SWF_TAG_DEFINE_MORPH_SHAPE=46,
	SWF_TAG_DEFINE_MORPH_SHAPE2=84,
	SWF_TAG_DEFINE_BITS_LOSSLESS=20,
	SWF_TAG_DEFINE_BITS_LOSSLESS2=36,
	SWF_TAG_DEFINE_BITS_JPEG2=21,
	SWF_TAG_DEFINE_BITS_JPEG3=35,
	SWF_TAG_DEFINE_BITS=6,
	SWF_TAG_JPEG_TABELS=8,
	SWF_TAG_PROTECT=24,
	SWF_TAG_DEFINE_SOUND=14,
	SWF_TAG_START_SOUND=15,
	SWF_TAG_SOUND_STREAM_HEAD=18,
	SWF_TAG_SOUND_STREAM_HEAD2=45,
	SWF_TAG_SOUND_STREAM_BLOCK=19,
	SWF_TAG_DO_ACTION=12,
	SWF_TAG_DEFINE_BUTTON2=34,
	SWF_TAG_IMPORT_ASSETS=57,
	SWF_TAG_EXPORT_ASSETS=56,
	SWF_TAG_IMPORT_ASSETS2=71,
};
struct RecordHeader {
	SwfTagType type;
	int length;
};
RecordHeader readRecordHeader(SwfDataStream *stream) {
	int tagIdAndLength = readU16(stream);
	RecordHeader ret;
	ret.type = (SwfTagType)(tagIdAndLength >> 6);
	ret.length = (int)tagIdAndLength & 0x3F;

	if (ret.length == 0x3F) ret.length = readU32(stream);
	if (ret.length > stream->size - stream->byteIndex) {
		logf("Tried to read record header that was too big\n");
		ret.length = stream->size - stream->byteIndex;
	}

	return ret;
}

struct SwfTagPointer {
	RecordHeader header;
	void *tag;
};

enum ShapeRecordType {
	SHAPE_RECORD_STYLE_CHANGE,
	SHAPE_RECORD_STRAIGHT_EDGE,
	SHAPE_RECORD_CURVED_EDGE,
	SHAPE_RECORD_END,
};
struct StyleChangeRecord {
	Vec2 control;
	bool stateNewStyles;
	bool stateLineStyle;
	bool stateFillStyle1;
	bool stateFillStyle0;
	bool stateMoveTo;
	float moveDeltaX;
	float moveDeltaY;
	int fillStyle0;
	int fillStyle1;
	int lineStyle;
	FillStyle *fillStyles;
	int fillStylesNum;

	LineStyle *lineStyles;
	int lineStylesNum;
};
struct EdgeRecord {
	Vec2 start;
	int lineStyleIndex;
	int fillStyle0Index;
	int fillStyle1Index;
	Vec2 control;
	Vec2 anchor;
};
struct ShapeRecord {
	ShapeRecordType type;
	union {
		StyleChangeRecord styleChangeRecord;
		EdgeRecord edgeRecord;
	};
};

enum DrawEdgeType {
	DRAW_EDGE_STRAIGHT_EDGE,
	DRAW_EDGE_CURVED_EDGE,
};
struct DrawEdgeRecord {
	DrawEdgeType type;
	u16 origOrderIndex;
	int lineStyleIndex;
	int fillStyle0Index;
	int fillStyle1Index;
	int fillStyleIndex;
	Vec2 start;
	Vec2 control;
	Vec2 anchor;
};
struct SwfSubShape {
	int fillStyleIndex;
	int lineStyleIndex;

	void *runtimeCachedPath;
};
struct SwfShape {
	int version;
	u16 shapeId;
	SwfRect shapeBounds;
	SwfRect edgeBounds;
	bool usesFillWindingRule;
	bool usesNonScalingStrokes;
	bool usesScalingStrokes;

	FillStyle *fillStyles;
	int fillStylesNum;

	LineStyle *lineStyles;
	int lineStylesNum;

	DrawEdgeRecord *drawEdges;
	int drawEdgesMax;
	int drawEdgesNum;

	SwfSubShape *subShapes;
	int subShapesNum;
};

struct ColorMatrixFilter {
	float matrix[20];
};
struct ConvolutionFilter {
	u8 matrixX;
	u8 matrixY;
	float divisor;
	float bias;
	float *matrix;
	int defaultColor;
	bool clamp;
	bool preserveAlpha;
};
struct BlurFilter {
	float blurX;
	float blurY;
	int passes;
};
struct DropShadowFilter {
	float dropShadowColor;
	float blurX;
	float blurY;
	float angle;
	float distance;
	float strength;
	bool innerShadow;
	bool knockout;
	bool compositeSource;
	int passes;
};

enum SwfFilterType {
	SWF_FILTER_DROP_SHADOW=0,
	SWF_FILTER_BLUR,
	SWF_FILTER_GLOW,
	SWF_FILTER_BEVEL,
	SWF_FILTER_GRADIENT_GLOW,
	SWF_FILTER_CONVOLUTION,
	SWF_FILTER_COLOR_MATRIX,
	SWF_FILTER_GRADIENT_BEVEL,
};
struct SwfFilter {
	SwfFilterType type;
	union {
		ColorMatrixFilter colorMatrixFilter;
		ConvolutionFilter convolutionFilter;
		BlurFilter blurFilter;
		DropShadowFilter dropShadowFilter;
	};
};
enum SwfBlendMode {
	SWF_BLEND_NORMAL=1, // Or 0!!
	SWF_BLEND_LAYER=2,
	SWF_BLEND_MULTIPLY=3,
	SWF_BLEND_SCREEN=4,
	SWF_BLEND_LIGHTEN=5,
	SWF_BLEND_DARKEN=6,
	SWF_BLEND_DIFFERENCE=7,
	SWF_BLEND_ADD=8,
	SWF_BLEND_SUBTRACT=9,
	SWF_BLEND_INVERT=10,
	SWF_BLEND_ALPHA=11,
	SWF_BLEND_ERASE=12,
	SWF_BLEND_OVERLAY=13,
	SWF_BLEND_HARDLIGHT=14,
	SWF_BLEND_NONE=64,
};
struct PlaceObject {
	int version;
	bool pfHasClipActions;
	bool pfHasClipDepth;
	bool pfHasName;
	bool pfHasRatio;
	bool pfHasColorTransform;
	bool pfHasMatrix;
	bool pfHasCharacter;
	bool pfMove;
	bool pfHasOpaqueBackground;
	bool pfHasVisible;
	bool pfHasImage;
	bool pfHasClassName;
	bool pfHasCacheAsBitmap;
	bool pfHasBlendMode;
	bool pfHasFilterList;
	u16 depth;
	char *className;
	u16 characterId;
	Matrix3 matrix;
	ColorTransform colorTransform;
	u16 ratio;
	char *name;
	u16 clipDepth;

	SwfFilter *filters;
	int filtersNum;

	SwfBlendMode blendMode;
	u8 bitmapCache;
	bool visible;
	int backgroundColor;
};
struct RemoveObject {
	u16 depth;
};
struct FrameLabel {
	char *name;
};
struct ControlTag {
	SwfTagType type;
	union {
		PlaceObject placeObject;
		RemoveObject removeObject;
		FrameLabel frameLabel;
	};
};
struct SwfDrawable;
struct SwfFrame {
	SwfDrawable *depths;
	int depthsNum;
	char **labels;
	int labelsNum;
};
enum SwfTextAlign {
	TEXT_ALIGN_LEFT = 0,
	TEXT_ALIGN_RIGHT = 1,
	TEXT_ALIGN_CENTER = 2,
	TEXT_ALIGN_JUSTIFY = 3,
};
struct SwfSprite {
	char *name;
	u16 spriteId;
	u16 frameCount;
	ControlTag *controlTags;
	u64 controlTagsNum;

	SwfFrame *frames;
	int framesNum;

	int highestDepth;

	int labelsInOrderMax;
	char **labelsInOrder;
	int labelsInOrderNum;

	Rect bounds;
	bool isTextField;
	SwfTextAlign textAlign;
	int textColor;

	char *initialText;
	Font *font;
};

struct DefineFontName {
	u16 fontId;
	char *name;
	Font *font;
};

struct DefineEditText {
	u16 characterId;
	SwfRect bounds;
	bool wordWrap;
	bool multiline;
	bool readOnly;
	bool autoSize;
	bool noSelect;
	bool border;
	bool wasStatic;
	u16 fontId;
	char *fontClass;
	u16 fontHeight;
	int textColor;
	u16 maxLength;
	SwfTextAlign align;
	char *variableName;
	char *initialText;
};

enum SwfDrawableType {
	SWF_DRAWABLE_NONE,
	SWF_DRAWABLE_SHAPE,
	SWF_DRAWABLE_SPRITE,
};

struct SwfDrawable {
	char *name; // 8
	ColorTransform *colorTransform; // 8
	SwfFilter *filters; // 8
	Matrix2x3 matrix; // 24
	u16 clipDepth; // 2
	u16 depth; // 2
	u8 filtersNum; // 1
	/*SwfBlendMode*/u8 spriteBlendMode; // 1

	/*SwfDrawableType*/u8 type; // 1
	union {
		SwfSprite *sprite;
		SwfShape *shape;
		SwfBitmap *bitmap;
	};
};

struct SwfHeader {
	u8 sig[3];
	u8 version;
	u32 fileLength;
	SwfRect frameSize;
	float frameRate;
	u16 frameCount;
};

struct Swf {
	SwfHeader header;
	SwfTagPointer *tags;
	int tagsNum;
	int tagsMax;

	char path[PATH_MAX_LEN];

#define LOADED_SWFS_MAX 64
	Swf *loadedSwfs[LOADED_SWFS_MAX];
	int loadedSwfsNum;

	SwfShape **allShapes;
	int allShapesNum;

	SwfSprite **allSprites;
	int allSpritesNum;

#define SWF_FONT_NAMES_MAX 32
	DefineFontName defineFontNames[SWF_FONT_NAMES_MAX];
	int defineFontNamesNum;

	Allocator characterMapAllocator; // I'm not sure why these are part of this struct
	HashMap *characterMap;

	MemoryArena *drawablesArena;
};

Swf *loadSwf(char *path);
SwfDrawable makeDrawableById(Swf *swf, PlaceObject *placeObject);
int processSubPath(DrawEdgeRecord *dest, int destNum, DrawEdgeRecord *src, int srcNum);
SwfSprite *getAliasedSprite(SwfSprite *sourceSprite, Swf *swf);

bool hasLabel(SwfSprite *sprite, char *label);
char *getLabelWithPrefix(SwfSprite *sprite, char *prefix);
void printDrawEdges(DrawEdgeRecord *edges, int edgesNum);
int getSpriteFrameForLabel(SwfSprite *sprite, char *label, int afterFrame=0);
int getFrameForLabel(SwfSprite *sprite, char *label, int afterFrame=0);
Rect getFrameBounds(SwfSprite *sprite, int frameIndex);
void destroySwf(Swf *swf);
/// FUNCTIONS ^

Swf *loadSwf(char *path) {
	NanoTime startTime = getNanoTime();

	int swfSize = 0;
	// void *swfData = readFile("assets/swf/Shared.swf", &swfSize);
	void *swfData = readFile(path, &swfSize);

	SwfDataStream stream = {};
	stream.data = (u8 *)swfData;
	stream.toFree = stream.data;
	stream.size = swfSize;

	Swf *swf = (Swf *)zalloc(sizeof(Swf));
	strcpy(swf->path, path);
	swf->drawablesArena = createMemoryArena();

	swf->header.sig[0] = read(&stream);
	swf->header.sig[1] = read(&stream);
	swf->header.sig[2] = read(&stream);

	swf->header.version = read(&stream);
	swf->header.fileLength = readU32(&stream);

	if (swf->header.sig[0] == 'C') {
		u64 uncompressedDataSize = swf->header.fileLength - 8;
		u8 *uncompressedData = (u8 *)malloc(uncompressedDataSize);

		u64 compressedDataSize = swfSize - 8;
		u8 *compressedData = stream.data + 8;

		int err = uncompress(uncompressedData, (unsigned long *)&uncompressedDataSize, compressedData, compressedDataSize);
		if (err != Z_OK) {
			logf("zlib error: %d\n", err);
			Panic("RIP");
		}

		free(stream.data);
		stream.toFree = uncompressedData;
		stream.data = uncompressedData - 8;
		stream.size = uncompressedDataSize + 8;
	}

	swf->header.frameSize = readRect(&stream);
	swf->header.frameRate = readFixed8(&stream);
	swf->header.frameCount = readU16(&stream);

	// logf("Sig: %c%c%c\n", swf->header.sig[0], swf->header.sig[1], swf->header.sig[2]);
	// logf("Version: %d\n", swf->header.version);
	// logf("FileLength: %d\n", swf->header.fileLength);
	// swf->header.frameSize.print("FrameSize");
	// logf("FrameRate: %f\n", swf->header.frameRate);
	// logf("FrameCount: %d\n", swf->header.frameCount);

	swf->tagsMax = 128;
	swf->tags = (SwfTagPointer *)zalloc(sizeof(SwfTagPointer) * swf->tagsMax);
	swf->tagsNum = 0;

	for (;;) {
		bool skipTag = false;
		RecordHeader recordHeader = readRecordHeader(&stream);

		SwfTagPointer *tagPointer = &swf->tags[swf->tagsNum++];
		tagPointer->header = recordHeader;
		if (recordHeader.type == SWF_TAG_FILE_ATTRIBUTES) {
			swf->tagsNum--;

			readUB(&stream, 1); // reserved
			bool useDirectBlit = readUB(&stream, 1);
			bool useGpu = readUB(&stream, 1);
			bool hasMetadata = readUB(&stream, 1);
			bool actionScript3 = readUB(&stream, 1);
			readUB(&stream, 2); // reserved
			bool useNetwork = readUB(&stream, 1);
			readUB(&stream, 24); // reserved
		} else if (recordHeader.type == SWF_TAG_SET_BACKGROUND_COLOR) {
			swf->tagsNum--;
			int rgb = readRgb(&stream);
		} else if (recordHeader.type == SWF_TAG_DEFINE_SCENE_AND_FRAME_LABEL_DATA) {
			swf->tagsNum--;
			int sceneCount = readEncodedU32(&stream);
			// tag->sceneFrameOffsets = (int *)malloc(sizeof(int) * tag->sceneCount);
			// tag->sceneFrameNames = (char **)malloc(sizeof(char *) * tag->sceneCount);

			for (int i = 0; i < sceneCount; i++) {
				int sceneFrameOffset = readEncodedU32(&stream);
				skipString(&stream); // sceneFrameName
			}

			int frameLabelCount = readEncodedU32(&stream);
			// tag->frameLabelNumbers = (int *)malloc(sizeof(int) * tag->frameLabelCount);
			// tag->frameLabelLabels = (char **)malloc(sizeof(char *) * tag->frameLabelCount);

			for (int i = 0; i < frameLabelCount; i++) {
				int frameLabelNumber = readEncodedU32(&stream);
				skipString(&stream); // frameLabelLabels
			}
		} else if (
			recordHeader.type == SWF_TAG_DEFINE_SHAPE ||
			recordHeader.type == SWF_TAG_DEFINE_SHAPE2 ||
			recordHeader.type == SWF_TAG_DEFINE_SHAPE3 ||
			recordHeader.type == SWF_TAG_DEFINE_SHAPE4
		) {
			SwfShape *tag = (SwfShape *)zalloc(sizeof(SwfShape));
			tagPointer->tag = tag;
			tagPointer->header.type = SWF_TAG_DEFINE_SHAPE;

			int shapeNum = 1;
			if (recordHeader.type == SWF_TAG_DEFINE_SHAPE) shapeNum = 1;
			if (recordHeader.type == SWF_TAG_DEFINE_SHAPE2) shapeNum = 2;
			if (recordHeader.type == SWF_TAG_DEFINE_SHAPE3) shapeNum = 3;
			if (recordHeader.type == SWF_TAG_DEFINE_SHAPE4) shapeNum = 4;
			tag->version = shapeNum;

			tag->shapeId = readU16(&stream);
			tag->shapeBounds = readRect(&stream);
			if (shapeNum == 4) {
				tag->edgeBounds = readRect(&stream);
				readUB(&stream, 5); // reserved
				tag->usesFillWindingRule = readUB(&stream, 1);
				tag->usesNonScalingStrokes = readUB(&stream, 1);
				tag->usesScalingStrokes = readUB(&stream, 1);
			}

			int fillStyleCount = read(&stream);
			tag->fillStyles = (FillStyle *)zalloc(sizeof(FillStyle) * fillStyleCount);
			for (int i = 0; i < fillStyleCount; i++) {
				tag->fillStyles[tag->fillStylesNum++] = readFillStyle(&stream, shapeNum);
			}

			int lineStyleCount = read(&stream);
			tag->lineStyles = (LineStyle *)zalloc(sizeof(LineStyle) * lineStyleCount);
			for (int i = 0; i < lineStyleCount; i++) {
				tag->lineStyles[tag->lineStylesNum++] = readLineStyle(&stream, shapeNum);
			}

			if (swfLogShapeInfo) logf("Starting shape %d fillStyles %d lineStyles\n", tag->fillStylesNum, tag->lineStylesNum);

			int fillBits = readUB(&stream, 4);
			int lineBits = readUB(&stream, 4);

			int lineStyleIndex = 0;
			int fillStyle0Index = 0;
			int fillStyle1Index = 0;

			int fillStyleIndexOffset = 0;
			int lineStyleIndexOffset = 0;

			Vec2 cursor = v2();

			int shapeRecordsMax = 8;
			ShapeRecord *shapeRecords = (ShapeRecord *)zalloc(sizeof(ShapeRecord) * shapeRecordsMax);
			int shapeRecordsNum = 0;
			for (;;) {
				if (shapeRecordsNum > shapeRecordsMax-1) {
					shapeRecords = (ShapeRecord *)resizeArray(shapeRecords, sizeof(ShapeRecord), shapeRecordsMax, shapeRecordsMax * 1.5);
					shapeRecordsMax *= 1.5;
				}

				ShapeRecord *record = &shapeRecords[shapeRecordsNum++];
				int typeFlag = readUB(&stream, 1);
				if (typeFlag == 0) { 
					bool stateNewStyles = readUB(&stream, 1);
					bool stateLineStyle = readUB(&stream, 1);
					bool stateFillStyle1 = readUB(&stream, 1);
					bool stateFillStyle0 = readUB(&stream, 1);
					bool stateMoveTo = readUB(&stream, 1);
					if (!stateNewStyles && !stateLineStyle && !stateFillStyle1 && !stateFillStyle0 && !stateMoveTo) {
						record->type = SHAPE_RECORD_END;
						break;
					} else {
						record->type = SHAPE_RECORD_STYLE_CHANGE;
						record->styleChangeRecord.stateNewStyles = stateNewStyles;
						record->styleChangeRecord.stateLineStyle = stateLineStyle;
						record->styleChangeRecord.stateFillStyle1 = stateFillStyle1;
						record->styleChangeRecord.stateFillStyle0 = stateFillStyle0;
						record->styleChangeRecord.stateMoveTo = stateMoveTo;
						if (stateMoveTo) {
							int moveBits = readUB(&stream, 5);
							record->styleChangeRecord.moveDeltaX = (float)readSB(&stream, moveBits) / 20.0;
							record->styleChangeRecord.moveDeltaY = (float)readSB(&stream, moveBits) / 20.0;
							cursor.x = record->styleChangeRecord.moveDeltaX;
							cursor.y = record->styleChangeRecord.moveDeltaY;
							record->styleChangeRecord.control = cursor;
							if (swfLogShapeInfo) logf("Move to %f %f\n", cursor.x, cursor.y);
						}
						if (stateFillStyle0) {
							record->styleChangeRecord.fillStyle0 = readUB(&stream, fillBits);
							fillStyle0Index = record->styleChangeRecord.fillStyle0;
							if (swfLogShapeInfo) logf("Set fillStyle0 to index %d (And going to add %d)\n", fillStyle0Index, fillStyleIndexOffset);
							if (fillStyle0Index != 0) fillStyle0Index += fillStyleIndexOffset;
						}
						if (stateFillStyle1) {
							record->styleChangeRecord.fillStyle1 = readUB(&stream, fillBits);
							fillStyle1Index = record->styleChangeRecord.fillStyle1;
							if (swfLogShapeInfo) logf("Set fillStyle1 to index %d (And going to add %d)\n", fillStyle1Index, fillStyleIndexOffset);
							if (fillStyle1Index != 0) fillStyle1Index += fillStyleIndexOffset;
						}
						if (stateLineStyle) {
							record->styleChangeRecord.lineStyle = readUB(&stream, lineBits);
							lineStyleIndex = record->styleChangeRecord.lineStyle;
							if (swfLogShapeInfo) logf("Set lineStyle to index %d (And going to add %d)\n", lineStyleIndex, lineStyleIndexOffset);
							if (lineStyleIndex != 0) lineStyleIndex += lineStyleIndexOffset;
						}
						if (stateNewStyles) {
							bool morphShape = false;
							if (morphShape) {
								logf("MorphShape???\n");
								Panic("Can't continue");
							} else {
								fillStyleIndexOffset = tag->fillStylesNum;
								lineStyleIndexOffset = tag->lineStylesNum;

								int fillStyleCount = read(&stream);
								if (swfLogShapeInfo) logf("Adding %d fillStyles (%d existing)\n", fillStyleCount, tag->fillStylesNum);
								if (tag->fillStylesNum+fillStyleCount > 0) {
									tag->fillStyles = (FillStyle *)resizeArray(tag->fillStyles, sizeof(FillStyle), tag->fillStylesNum, tag->fillStylesNum+fillStyleCount);
								}
								for (int i = 0; i < fillStyleCount; i++) tag->fillStyles[tag->fillStylesNum++] = readFillStyle(&stream, shapeNum);

								int lineStyleCount = read(&stream);
								if (swfLogShapeInfo) logf("Adding %d lineStyles (%d existing)\n", lineStyleCount, tag->lineStylesNum);
								if (tag->lineStylesNum+lineStyleCount > 0) {
									tag->lineStyles = (LineStyle *)resizeArray(tag->lineStyles, sizeof(LineStyle), tag->lineStylesNum, tag->lineStylesNum+lineStyleCount);
								}
								for (int i = 0; i < lineStyleCount; i++) tag->lineStyles[tag->lineStylesNum++] = readLineStyle(&stream, shapeNum);
							}

							fillBits = readUB(&stream, 4);
							lineBits = readUB(&stream, 4);
						}
					}
				} else {
					record->edgeRecord.lineStyleIndex = lineStyleIndex;
					record->edgeRecord.fillStyle0Index = fillStyle0Index;
					record->edgeRecord.fillStyle1Index = fillStyle1Index;

					int straightFlag = readUB(&stream, 1);
					if (straightFlag == 1) {
						if (swfLogShapeInfo) logf("Drawing straight line\n");
						record->type = SHAPE_RECORD_STRAIGHT_EDGE;
						int numBits = readUB(&stream, 4);
						int generalLineFlag = readUB(&stream, 1);
						bool vertLineFlag = 0;
						if (!generalLineFlag) vertLineFlag = readUB(&stream, 1);
						record->edgeRecord.start = cursor;
						Vec2 delta = v2();
						if (generalLineFlag || !vertLineFlag) delta.x = (float)readSB(&stream, numBits + 2) / 20.0;
						if (generalLineFlag || vertLineFlag) delta.y = (float)readSB(&stream, numBits + 2) / 20.0;
						cursor += delta;
						record->edgeRecord.control = cursor;
					} else {
						if (swfLogShapeInfo) logf("Drawing curved line\n");
						record->type = SHAPE_RECORD_CURVED_EDGE;
						int numBits = readUB(&stream, 4);
						record->edgeRecord.start = cursor;
						Vec2 delta;
						Vec2 anchorDelta;
						delta.x = (float)readSB(&stream, numBits + 2) / 20.0;
						delta.y = (float)readSB(&stream, numBits + 2) / 20.0;
						anchorDelta.x = (float)readSB(&stream, numBits + 2) / 20.0;
						anchorDelta.y = (float)readSB(&stream, numBits + 2) / 20.0;
						record->edgeRecord.control = cursor + delta;
						record->edgeRecord.anchor = cursor + delta + anchorDelta;
						cursor = record->edgeRecord.anchor;
					}
				}
			}

			{
				int subPathMax = shapeRecordsNum; // This is just a guess
				DrawEdgeRecord *subPath = (DrawEdgeRecord *)malloc(sizeof(DrawEdgeRecord) * subPathMax);
				int subPathNum = 0;

				int lineStyleIndex = 0;
				int fillStyle0Index = 0;
				int fillStyle1Index = 0;

				int drawEdgesMax = shapeRecordsNum; // This is just a guess
				DrawEdgeRecord *drawEdges = (DrawEdgeRecord *)zalloc(sizeof(DrawEdgeRecord) * drawEdgesMax);
				int drawEdgesNum = 0;

				for (int i = 0; i < shapeRecordsNum; i++) {
					ShapeRecord *record = &shapeRecords[i];
					if (record->type == SHAPE_RECORD_STRAIGHT_EDGE || record->type == SHAPE_RECORD_CURVED_EDGE) {
						if (
							record->edgeRecord.lineStyleIndex != lineStyleIndex ||
							record->edgeRecord.fillStyle0Index != fillStyle0Index ||
							record->edgeRecord.fillStyle1Index != fillStyle1Index
						) {
							while (drawEdgesNum + subPathNum*3 > drawEdgesMax-1) { //@copyPastedReserveMaxEdges
								drawEdges = (DrawEdgeRecord *)resizeArray(drawEdges, sizeof(DrawEdgeRecord), drawEdgesMax, drawEdgesMax * 1.5);
								drawEdgesMax *= 1.5;
							}
							drawEdgesNum = processSubPath(drawEdges, drawEdgesNum, subPath, subPathNum);
							subPathNum = 0;
							memset(subPath, 0, sizeof(DrawEdgeRecord) * subPathMax);
							lineStyleIndex = record->edgeRecord.lineStyleIndex;
							fillStyle0Index = record->edgeRecord.fillStyle0Index;
							fillStyle1Index = record->edgeRecord.fillStyle1Index;
						}

						if (subPathNum > subPathMax-1) {
							subPath = (DrawEdgeRecord *)resizeArray(subPath, sizeof(DrawEdgeRecord), subPathMax, subPathMax * 1.5);
							subPathMax *= 1.5;
						}

						DrawEdgeRecord *edge = &subPath[subPathNum++];
						edge->origOrderIndex = i;
						if (record->type == SHAPE_RECORD_STRAIGHT_EDGE) {
							edge->type = DRAW_EDGE_STRAIGHT_EDGE;
							edge->start = record->edgeRecord.start;
							edge->control = record->edgeRecord.control;
						} else {
							edge->type = DRAW_EDGE_CURVED_EDGE;
							edge->start = record->edgeRecord.start;
							edge->control = record->edgeRecord.control;
							edge->anchor = record->edgeRecord.anchor;
						}
						edge->lineStyleIndex = lineStyleIndex;
						edge->fillStyle0Index = fillStyle0Index;
						edge->fillStyle1Index = fillStyle1Index;
					}
				}

				while (drawEdgesNum + subPathNum*3 > drawEdgesMax-1) { //@copyPastedReserveMaxEdges
					drawEdges = (DrawEdgeRecord *)resizeArray(drawEdges, sizeof(DrawEdgeRecord), drawEdgesMax, drawEdgesMax * 1.5);
					drawEdgesMax *= 1.5;
				}
				drawEdgesNum = processSubPath(drawEdges, drawEdgesNum, subPath, subPathNum);

				tag->drawEdgesMax = drawEdgesNum;
				tag->drawEdges = (DrawEdgeRecord *)zalloc(sizeof(DrawEdgeRecord) * tag->drawEdgesMax);

				DrawEdgeRecord **edgesLeft = (DrawEdgeRecord **)frameMalloc(sizeof(DrawEdgeRecord *) * drawEdgesNum);
				int edgesLeftNum = 0;
				for (int i = 0; i < drawEdgesNum; i++) edgesLeft[edgesLeftNum++] = &drawEdges[i];

				DrawEdgeRecord **edgesOut = (DrawEdgeRecord **)frameMalloc(sizeof(DrawEdgeRecord *) * drawEdgesNum);
				int edgesOutNum = 0;

				{ /// Connect everything together
					int *chain = (int *)frameMalloc(sizeof(int) * edgesLeftNum);
					int chainNum = 0;
					for (;;) {
						if (edgesLeftNum <= 0) break;

						int earliestIndex = 0;
						DrawEdgeRecord *pathStart = edgesLeft[earliestIndex];

						chainNum = 0;

						for (;;) {
							bool foundEarlier = false;
							chain[chainNum++] = pathStart->origOrderIndex;
							for (int i = 0; i < edgesLeftNum; i++) {
								DrawEdgeRecord *possibleEarlier = edgesLeft[i];
								if (possibleEarlier->fillStyleIndex != pathStart->fillStyleIndex) continue;
								if (possibleEarlier->lineStyleIndex != pathStart->lineStyleIndex) continue;

								Vec2 end;
								if (possibleEarlier->type == DRAW_EDGE_STRAIGHT_EDGE) end = possibleEarlier->control;
								else if (possibleEarlier->type == DRAW_EDGE_CURVED_EDGE) end = possibleEarlier->anchor;
								if (!equal(pathStart->start, end)) continue;

								bool inChain = false;
								for (int i = 0; i < chainNum; i++) {
									if (possibleEarlier->origOrderIndex == chain[i]) {
										inChain = true;
										break;
									}
								}
								if (inChain) continue;

								foundEarlier = true;
								earliestIndex = i;
								pathStart = possibleEarlier;
								break;
							}

							if (!foundEarlier) break;
						}

						arraySpliceIndex(edgesLeft, edgesLeftNum, sizeof(DrawEdgeRecord *), earliestIndex);
						edgesLeftNum--;
						int trueStartIndex = pathStart->origOrderIndex;

						for (;;) {
							edgesOut[edgesOutNum++] = pathStart;
							bool foundNext = false;
							for (int i = 0; i < edgesLeftNum; i++) {
								DrawEdgeRecord *possibleNext = edgesLeft[i];

								if (possibleNext->fillStyleIndex != pathStart->fillStyleIndex) continue;
								if (possibleNext->lineStyleIndex != pathStart->lineStyleIndex) continue;
								if (possibleNext->origOrderIndex == trueStartIndex) continue;

								Vec2 end;
								if (pathStart->type == DRAW_EDGE_STRAIGHT_EDGE) end = pathStart->control;
								else if (pathStart->type == DRAW_EDGE_CURVED_EDGE) end = pathStart->anchor;
								if (!equal(possibleNext->start, end)) continue;

								foundNext = true;
								pathStart = possibleNext;
								arraySpliceIndex(edgesLeft, edgesLeftNum, sizeof(DrawEdgeRecord *), i);
								edgesLeftNum--;
								break;
							}
							if (!foundNext) break;
						}
					}
				}

				{ /// Put fills first
					for (;;) {
						if (edgesOutNum == 0) break;
						int lowestFillStyleIndex = 0;
						int lowestLineStyleIndex = 0;

						for (int i = 0; i < edgesOutNum; i++) {
							DrawEdgeRecord *edge = edgesOut[i];
							if (edge->fillStyleIndex != 0) {
								if (lowestFillStyleIndex == 0 || lowestFillStyleIndex > edge->fillStyleIndex) lowestFillStyleIndex = edge->fillStyleIndex;
							}
							if (edge->lineStyleIndex != 0) {
								if (lowestLineStyleIndex == 0 || lowestLineStyleIndex > edge->lineStyleIndex) lowestLineStyleIndex = edge->lineStyleIndex;
							}
						}

						for (int i = 0; i < edgesOutNum; i++) {
							DrawEdgeRecord *edge = edgesOut[i];
							if (lowestFillStyleIndex) {
								if (lowestFillStyleIndex == edge->fillStyleIndex) {
									tag->drawEdges[tag->drawEdgesNum++] = *edge;
									arraySpliceIndex(edgesOut, edgesOutNum, sizeof(DrawEdgeRecord *), i);
									edgesOutNum--;
									i--;
									continue;
								}
							} else {
								if (lowestLineStyleIndex && lowestLineStyleIndex == edge->lineStyleIndex) {
									tag->drawEdges[tag->drawEdgesNum++] = *edge;
									arraySpliceIndex(edgesOut, edgesOutNum, sizeof(DrawEdgeRecord *), i);
									edgesOutNum--;
									i--;
									continue;
								}
							}
						}

					}
				}

				free(subPath);
				free(drawEdges);
			}
			free(shapeRecords);

			{ /// Build sub shapes
				int lineStyleIndex = 0;
				int fillStyleIndex = 0;
				for (int i = 0; i < tag->drawEdgesNum; i++) {
					DrawEdgeRecord *edge = &tag->drawEdges[i];
					if (edge->fillStyleIndex != fillStyleIndex || edge->lineStyleIndex != lineStyleIndex) {
						tag->subShapesNum++;
						lineStyleIndex = edge->lineStyleIndex;
						fillStyleIndex = edge->fillStyleIndex;
					}
				}

				tag->subShapes = (SwfSubShape *)zalloc(sizeof(SwfSubShape) * tag->subShapesNum);
				int subShapeIndex = -1;
				SwfSubShape *subShape = NULL;
				lineStyleIndex = 0;
				fillStyleIndex = 0;

				Vec2 cursor = v2();
				SkPath *currentPath;
				for (int i = 0; i < tag->drawEdgesNum; i++) {
					DrawEdgeRecord *edge = &tag->drawEdges[i];
					if (edge->fillStyleIndex != fillStyleIndex || edge->lineStyleIndex != lineStyleIndex) {
						subShapeIndex++;
						cursor = v2();
						if (subShapeIndex > tag->subShapesNum-1) logf("Sub shape overflow\n");
						subShape = &tag->subShapes[subShapeIndex];
						lineStyleIndex = edge->lineStyleIndex;
						fillStyleIndex = edge->fillStyleIndex;

						int currentDrawEdgeIndex = i;
						for (int i = currentDrawEdgeIndex; i < tag->drawEdgesNum; i++) {
							DrawEdgeRecord *otherEdge = &tag->drawEdges[i];
							if (otherEdge->fillStyleIndex != fillStyleIndex || otherEdge->lineStyleIndex != lineStyleIndex) break;
						}

						subShape->lineStyleIndex = lineStyleIndex;
						subShape->fillStyleIndex = fillStyleIndex;
						subShape->runtimeCachedPath = new SkPath();
						currentPath = (SkPath *)subShape->runtimeCachedPath;
					}

					if (!cursor.equal(edge->start)) {
						cursor = edge->start;
						currentPath->moveTo(edge->start.x, edge->start.y);
					}

					if (edge->type == DRAW_EDGE_STRAIGHT_EDGE) {
						cursor = edge->control;
						currentPath->lineTo(edge->control.x, edge->control.y);
					} else if (edge->type == DRAW_EDGE_CURVED_EDGE) {
						cursor = edge->anchor;
						currentPath->quadTo(edge->control.x, edge->control.y, edge->anchor.x, edge->anchor.y);
					}
				}

				if (subShapeIndex != tag->subShapesNum-1) logf("Sub shape underflow (%d/%d)\n", subShapeIndex, tag->subShapesNum-1);
			}
		} else if (recordHeader.type == SWF_TAG_DEFINE_SPRITE) {
			SwfSprite *sprite = (SwfSprite *)zalloc(sizeof(SwfSprite));
			tagPointer->tag = sprite;

			sprite->spriteId = readU16(&stream);
			sprite->frameCount = readU16(&stream);

			u64 currentIndex = stream.byteIndex;
			u64 controlTagsCount = 0;
			for (;;) {
				controlTagsCount++;
				RecordHeader nextHeader = readRecordHeader(&stream);
				if (nextHeader.type == 0) {
					stream.byteIndex = currentIndex;
					break;
				}
				skipBytes(&stream, nextHeader.length);
			}

			sprite->controlTags = (ControlTag *)zalloc(sizeof(ControlTag) * controlTagsCount);

			for (;;) {
				RecordHeader controlTagHeader = readRecordHeader(&stream);
				if (controlTagHeader.type == SWF_TAG_PLACE_OBJECT2) {
					ControlTag *controlTag = &sprite->controlTags[sprite->controlTagsNum++];
					controlTag->type = SWF_TAG_PLACE_OBJECT;

					PlaceObject *placeObject = &controlTag->placeObject;
					placeObject->version = 2;
					placeObject->pfHasClipActions = readUB(&stream, 1);
					placeObject->pfHasClipDepth = readUB(&stream, 1);
					placeObject->pfHasName = readUB(&stream, 1);
					placeObject->pfHasRatio = readUB(&stream, 1);
					placeObject->pfHasColorTransform = readUB(&stream, 1);
					placeObject->pfHasMatrix = readUB(&stream, 1);
					placeObject->pfHasCharacter = readUB(&stream, 1);
					placeObject->pfMove = readUB(&stream, 1);
					placeObject->depth = readU16(&stream);
					if (placeObject->pfHasCharacter) placeObject->characterId = readU16(&stream);
					if (placeObject->pfHasMatrix) placeObject->matrix = readMatrix(&stream);
					if (placeObject->pfHasColorTransform) placeObject->colorTransform = readColorTransformWithAlpha(&stream);
					if (placeObject->pfHasRatio) placeObject->ratio = readU16(&stream);
					if (placeObject->pfHasName) placeObject->name = readString(&stream);
					if (placeObject->pfHasClipDepth) {
						placeObject->clipDepth = readU16(&stream);
						if (sprite->highestDepth < placeObject->clipDepth) sprite->highestDepth = placeObject->clipDepth;
					}
					if (placeObject->pfHasClipActions) {
						logf("Has clip actions!\n");
						Panic("RIP");
					}

					if (sprite->highestDepth < placeObject->depth) sprite->highestDepth = placeObject->depth;
				} else if (controlTagHeader.type == SWF_TAG_PLACE_OBJECT3) {
					ControlTag *controlTag = &sprite->controlTags[sprite->controlTagsNum++];
					controlTag->type = SWF_TAG_PLACE_OBJECT;

					PlaceObject *placeObject = &controlTag->placeObject;
					placeObject->version = 3;
					placeObject->pfHasClipActions = readUB(&stream, 1);
					placeObject->pfHasClipDepth = readUB(&stream, 1);
					placeObject->pfHasName = readUB(&stream, 1);
					placeObject->pfHasRatio = readUB(&stream, 1);
					placeObject->pfHasColorTransform = readUB(&stream, 1);
					placeObject->pfHasMatrix = readUB(&stream, 1);
					placeObject->pfHasCharacter = readUB(&stream, 1);
					placeObject->pfMove = readUB(&stream, 1);
					readUB(&stream, 1); // Reserved
					placeObject->pfHasOpaqueBackground = readUB(&stream, 1);
					placeObject->pfHasVisible = readUB(&stream, 1);
					placeObject->pfHasImage = readUB(&stream, 1);
					placeObject->pfHasClassName = readUB(&stream, 1);
					placeObject->pfHasCacheAsBitmap = readUB(&stream, 1);
					placeObject->pfHasBlendMode = readUB(&stream, 1);
					placeObject->pfHasFilterList = readUB(&stream, 1);
					placeObject->depth = readU16(&stream);
					if (placeObject->pfHasClassName) placeObject->className = readString(&stream);
					if (placeObject->pfHasCharacter) placeObject->characterId = readU16(&stream);
					if (placeObject->pfHasMatrix) placeObject->matrix = readMatrix(&stream);
					if (placeObject->pfHasColorTransform) placeObject->colorTransform = readColorTransformWithAlpha(&stream);
					if (placeObject->pfHasRatio) placeObject->ratio = readU16(&stream);
					if (placeObject->pfHasName) placeObject->name = readString(&stream);
					if (placeObject->pfHasClipDepth) {
						placeObject->clipDepth = readU16(&stream);
						if (sprite->highestDepth < placeObject->clipDepth) sprite->highestDepth = placeObject->clipDepth;
					}
					if (placeObject->pfHasFilterList) {
						int filterCount = read(&stream);
						placeObject->filtersNum = filterCount;
						placeObject->filters = (SwfFilter *)zalloc(sizeof(SwfFilter) * filterCount);
						for (int i = 0; i < filterCount; i++) {
							SwfFilter *filter = &placeObject->filters[i];
							filter->type = (SwfFilterType)read(&stream);
							if (filter->type == SWF_FILTER_DROP_SHADOW) {
								int dropShadowColor = readRgba(&stream);
								float blurX = readFixed(&stream);
								float blurY = readFixed(&stream);
								float angle = readFixed(&stream);
								float distance = readFixed(&stream);
								float strength = readFixed8(&stream);
								bool innerShadow = readUB(&stream, 1);
								bool knockout = readUB(&stream, 1);
								bool compositeSource = readUB(&stream, 1);
								int passes = readUB(&stream, 5);
							} else if (filter->type == SWF_FILTER_BLUR) {
								filter->blurFilter.blurX = readFixed(&stream);
								filter->blurFilter.blurY = readFixed(&stream);
								filter->blurFilter.passes = readUB(&stream, 5);
								readUB(&stream, 3); // reserved
							} else if (filter->type == SWF_FILTER_GLOW) {
								int glowColor = readRgba(&stream);
								float blurX = readFixed(&stream);
								float blurY = readFixed(&stream);
								float strength = readFixed8(&stream);
								bool innerGlow = readUB(&stream, 1);
								bool knockout = readUB(&stream, 1);
								bool compositeSource = readUB(&stream, 1);
								int passes = readUB(&stream, 5);
							} else if (filter->type == SWF_FILTER_BEVEL) {
								int shadowColor = readRgba(&stream);
								int highlightColor = readRgba(&stream);
								float blurX = readFixed(&stream);
								float blurY = readFixed(&stream);
								float angle = readFixed(&stream);
								float distance = readFixed(&stream);
								float strength = readFixed8(&stream);
								bool innerShadow = readUB(&stream, 1);
								bool knockout = readUB(&stream, 1);
								bool compositeSource = readUB(&stream, 1);
								bool onTop = readUB(&stream, 1);
								int passes = readUB(&stream, 4);
							} else if (filter->type == SWF_FILTER_GRADIENT_GLOW) {
								int numColors = read(&stream);
								for (int i = 0; i < numColors; i++) int color = readRgba(&stream);
								for (int i = 0; i < numColors; i++) int gradientRatio = read(&stream);
								float blurX = readFixed(&stream);
								float blurY = readFixed(&stream);
								float angle = readFixed(&stream);
								float distance = readFixed(&stream);
								float strength = readFixed8(&stream);
								bool innerShadow = readUB(&stream, 1);
								bool knockout = readUB(&stream, 1);
								bool compositeSource = readUB(&stream, 1);
								bool onTop = readUB(&stream, 1);
								int passes = readUB(&stream, 4);
							} else if (filter->type == SWF_FILTER_CONVOLUTION) {
								int matrixX = read(&stream);
								int matrixY = read(&stream);
								float divisor = readFloat(&stream);
								float bias = readFloat(&stream);
								for (int y = 0; y < matrixY; y++) {
									for (int x = 0; x < matrixX; x++) {
										float matrixValue = readFloat(&stream);
									}
								}
								int defaultColor = readRgba(&stream);
								bool clamp = readUB(&stream, 1);
								bool preserveAlpha = readUB(&stream, 1);
							} else if (filter->type == SWF_FILTER_COLOR_MATRIX) {
								for (int i = 0; i < 20; i++) {
									filter->colorMatrixFilter.matrix[i] = readFloat(&stream);
								}
							} else if (filter->type == SWF_FILTER_GRADIENT_BEVEL) {
								int numColors = read(&stream);
								for (int i = 0; i < numColors; i++) int color = readRgba(&stream);
								for (int i = 0; i < numColors; i++) int gradientRatio = read(&stream);
								float blurX = readFixed(&stream);
								float blurY = readFixed(&stream);
								float angle = readFixed(&stream);
								float distance = readFixed(&stream);
								float strength = readFixed8(&stream);
								bool innerShadow = readUB(&stream, 1);
								bool knockout = readUB(&stream, 1);
								bool compositeSource = readUB(&stream, 1);
								bool onTop = readUB(&stream, 1);
								int passes = readUB(&stream, 4);
							}
						}
					}
					if (placeObject->pfHasBlendMode) {
						placeObject->blendMode = (SwfBlendMode)read(&stream);
					} else {
						placeObject->blendMode = SWF_BLEND_NONE;
					}

					if (placeObject->pfHasCacheAsBitmap) placeObject->bitmapCache = read(&stream);
					if (placeObject->pfHasVisible) placeObject->visible = read(&stream);
					if (placeObject->pfHasOpaqueBackground) placeObject->backgroundColor = readRgba(&stream);
					if (placeObject->pfHasClipActions) {
						logf("Has clip actions!\n");
					}

					if (sprite->highestDepth < placeObject->depth) sprite->highestDepth = placeObject->depth;
				} else if (controlTagHeader.type == SWF_TAG_END) {
					ControlTag *controlTag = &sprite->controlTags[sprite->controlTagsNum++];
					controlTag->type = controlTagHeader.type;
					break;
				} else if (controlTagHeader.type == SWF_TAG_SHOW_FRAME) {
					ControlTag *controlTag = &sprite->controlTags[sprite->controlTagsNum++];
					controlTag->type = controlTagHeader.type;
				} else if (controlTagHeader.type == SWF_TAG_REMOVE_OBJECT2) {
					ControlTag *controlTag = &sprite->controlTags[sprite->controlTagsNum++];
					controlTag->type = SWF_TAG_REMOVE_OBJECT;
					controlTag->removeObject.depth = readU16(&stream);
				} else if (controlTagHeader.type == SWF_TAG_FRAME_LABEL) {
					ControlTag *controlTag = &sprite->controlTags[sprite->controlTagsNum++];
					controlTag->type = controlTagHeader.type;
					controlTag->frameLabel.name = readString(&stream);
				} else if (
					controlTagHeader.type == SWF_TAG_SOUND_STREAM_BLOCK ||
					controlTagHeader.type == SWF_TAG_SOUND_STREAM_HEAD ||
					controlTagHeader.type == SWF_TAG_SOUND_STREAM_HEAD2
				) {
					skipBytes(&stream, controlTagHeader.length); // No sound
				} else if (controlTagHeader.type == SWF_TAG_DO_ACTION) {
					skipBytes(&stream, controlTagHeader.length); // Nothing...
				} else {
					logf("Not handling control tag %d\n", controlTagHeader.type);
					Panic("RIP");
				}
				// logf("Parsed control flag %d\n", controlTagHeader.type);
				byteAlign(&stream); // I dunno if I actually need this
			}
		} else if (recordHeader.type == SWF_TAG_DEFINE_EDIT_TEXT) {
			DefineEditText *tag = (DefineEditText *)zalloc(sizeof(DefineEditText));
			tagPointer->tag = tag;

			tag->characterId = readU16(&stream);
			tag->bounds = readRect(&stream);
			bool hasText = readUB(&stream, 1);
			tag->wordWrap = readUB(&stream, 1);
			tag->multiline = readUB(&stream, 1);
			bool password = readUB(&stream, 1);
			tag->readOnly = readUB(&stream, 1);
			bool hasTextColor = readUB(&stream, 1);
			bool hasMaxLength = readUB(&stream, 1);
			bool hasFont = readUB(&stream, 1);
			bool hasFontClass = readUB(&stream, 1);
			tag->autoSize = readUB(&stream, 1);
			bool hasLayout = readUB(&stream, 1);
			tag->noSelect = readUB(&stream, 1);
			tag->border = readUB(&stream, 1);
			tag->wasStatic = readUB(&stream, 1);
			bool html = readUB(&stream, 1);
			bool useOutlines = readUB(&stream, 1);
			if (hasFont) tag->fontId = readU16(&stream);
			if (hasFontClass) tag->fontClass = readString(&stream);
			if (hasFont || hasFontClass) tag->fontHeight = readU16(&stream);
			if (hasTextColor) tag->textColor = readRgba(&stream);
			if (hasMaxLength) tag->maxLength = readU16(&stream);
			if (hasLayout) {
				tag->align = (SwfTextAlign)read(&stream);
				u16 leftMargin = readU16(&stream);
				u16 rightMargin = readU16(&stream);
				u16 indent = readU16(&stream);
				u16 leading = readU16(&stream);
			}
			tag->variableName = readString(&stream);
			if (hasText) {
				char *htmlText = readString(&stream);
				int htmlTextLen = strlen(htmlText);

				tag->initialText = (char *)zalloc(htmlTextLen);
				int charIndex = 0;

				int inTag = false;
				for (int i = 0; i < htmlTextLen; i++) {
					char ch = htmlText[i];
					if (ch == '<') {
						inTag = true;
						continue;
					} else if (ch == '>') {
						inTag = false;
						continue;
					}

					if (inTag) continue;

					tag->initialText[charIndex++] = ch;
				}

				free(htmlText);
			}
		} else if (recordHeader.type == SWF_TAG_SYMBOL_CLASS) {
			int numSymbols = readU16(&stream);
			for (int i = 0; i < numSymbols; i++) {
				int charId = readU16(&stream);
				char *name = readString(&stream);
				for (int i = 0; i < swf->tagsNum; i++) {
					SwfTagPointer *otherPtr = &swf->tags[i];
					if (otherPtr->header.type == SWF_TAG_DEFINE_SPRITE) {
						SwfSprite *sprite = (SwfSprite *)otherPtr->tag;
						if (sprite->spriteId == charId) {
							sprite->name = name;
							break;
						}
					}
				}
			}
		} else if (
			recordHeader.type == SWF_TAG_DEFINE_FONT ||
			recordHeader.type == SWF_TAG_DEFINE_FONT3 ||
			recordHeader.type == SWF_TAG_DEFINE_FONT_INFO ||
			recordHeader.type == SWF_TAG_DEFINE_FONT_INFO2 ||
			recordHeader.type == SWF_TAG_DEFINE_TEXT
		) {
			skipTag = true;
		} else if (recordHeader.type == SWF_TAG_DEFINE_FONT_ALIGN_ZONES) {
			skipTag = true;
		} else if (recordHeader.type == SWF_TAG_DEFINE_FONT_NAME) {
			if (swf->defineFontNamesNum < SWF_FONT_NAMES_MAX-1) {
				DefineFontName *defineFontName = &swf->defineFontNames[swf->defineFontNamesNum++];
				defineFontName->fontId = readU16(&stream);
				defineFontName->name = readString(&stream);
				skipString(&stream); // FontCopyright
				// defineFontName->font = createFont(frameSprintf("assets/fonts/%s.ttf", defineFontName->name), 20);
			} else {
				logf("Too many swf fonts\n");
				skipTag = true;
			}
		} else if (
			recordHeader.type == SWF_TAG_DEFINE_BITS_LOSSLESS ||
			recordHeader.type == SWF_TAG_DEFINE_BITS_LOSSLESS2
		) {
			SwfBitmap *bitmap = (SwfBitmap *)zalloc(sizeof(SwfBitmap));
			tagPointer->tag = bitmap;
			tagPointer->header.type = SWF_TAG_DEFINE_BITS_LOSSLESS;

			int version = 0;
			if (recordHeader.type == SWF_TAG_DEFINE_BITS_LOSSLESS) version = 1;
			if (recordHeader.type == SWF_TAG_DEFINE_BITS_LOSSLESS2) version = 2;
			bitmap->characterId = readU16(&stream);
			int bitmapFormat = read(&stream);
			bitmap->width = readU16(&stream);
			bitmap->height = readU16(&stream);
			// logf("Format%d: %d\n", version, bitmapFormat);
			int bytesLeft = recordHeader.length - 7;
			int bitmapColorTableSize = 0;
			if (bitmapFormat == SWF_BITMAP_FORMAT_8) {
				bitmapColorTableSize = read(&stream)+1;
				bytesLeft--;
			}

			bitmap->pixels = (u8 *)malloc(bitmap->width * bitmap->height * 4);
			int pixelsNum = 0;

			u64 uncompressedDataSize = Megabytes(5);
			u8 *uncompressedData = (u8 *)malloc(uncompressedDataSize);
			int err = uncompress(uncompressedData, (unsigned long *)&uncompressedDataSize, &stream.data[stream.byteIndex], bytesLeft);
			if (err != Z_OK) {
				logf("zlib error: %d\n", err);
				Panic("RIP");
			}

			SwfDataStream subStream = {};
			subStream.data = uncompressedData;
			subStream.size = uncompressedDataSize;

			if (bitmapFormat == SWF_BITMAP_FORMAT_8) {
				int *table = (int *)frameMalloc(sizeof(int) * bitmapColorTableSize + 1);
				for (int i = 0; i < bitmapColorTableSize; i++) {
					if (version == 1) table[i] = readRgb(&subStream);
					else table[i] = readRgba(&subStream);
				}

				for (int y = 0; y < bitmap->height; y++) {
					for (int x = 0; x < bitmap->width; x++) {
						int index = read(&subStream);
						int color = table[index];
						int a, r, g, b;
						hexToArgb(color, &a, &r, &g, &b);
						bitmap->pixels[pixelsNum++] = r;
						bitmap->pixels[pixelsNum++] = g;
						bitmap->pixels[pixelsNum++] = b;
						bitmap->pixels[pixelsNum++] = a;
					}
					word32Align(&subStream);
				}
			} else if (bitmapFormat == SWF_BITMAP_FORMAT_15 || bitmapFormat == SWF_BITMAP_FORMAT_24) {
				for (int y = 0; y < bitmap->height; y++) {
					for (int x = 0; x < bitmap->width; x++) {
						int a = 255;
						int r = 0;
						int g = 0;
						int b = 0;
						if (bitmapFormat == SWF_BITMAP_FORMAT_15) {
							if (version == 2) {
								logf("Invalid bitmap format combo\n");
							}
							readUB(&subStream, 1);
							r = readUB(&subStream, 5);
							g = readUB(&subStream, 5);
							b = readUB(&subStream, 5);
						} else if (bitmapFormat == SWF_BITMAP_FORMAT_24) {
							if (version == 1) {
								read(&subStream);
								r = read(&subStream);
								g = read(&subStream);
								b = read(&subStream);
							} else {
								int color = readArgb(&subStream);
								hexToArgb(color, &a, &r, &g, &b);
							}
						}
						bitmap->pixels[pixelsNum++] = r;
						bitmap->pixels[pixelsNum++] = g;
						bitmap->pixels[pixelsNum++] = b;
						bitmap->pixels[pixelsNum++] = a;
					}
					word32Align(&subStream);
				}
			}

			if (subStream.byteIndex != subStream.size) {
				logf("Bitmap version %d format %d was probably parsed wrong %d/%d\n", version, bitmapFormat, subStream.byteIndex, subStream.size);
			}

			free(uncompressedData);

			skipBytes(&stream, bytesLeft);
		} else if (recordHeader.type == SWF_TAG_IMPORT_ASSETS) {
			logf("Import Assets was depricated in SWF8!\n");
			skipTag = true;
		} else if (recordHeader.type == SWF_TAG_EXPORT_ASSETS) {
			logf("Export Assets never happens in my SWFs?\n");
			skipTag = true;
		} else if (recordHeader.type == SWF_TAG_IMPORT_ASSETS2) {
			char *url = readString(&stream);
			read(&stream);
			read(&stream);
			int count = readU16(&stream);
			if (count > 0) Panic("Can't load swf with import assets specified to characters\n");

			char *dir = frameStringClone(path);
			char *lastSlash = strrchr(dir, '/');
			if (lastSlash) *lastSlash = 0;

			if (swf->loadedSwfsNum > LOADED_SWFS_MAX-1) Panic("Too many loaded swfs!");
			swf->loadedSwfs[swf->loadedSwfsNum++] = loadSwf(frameSprintf("%s/%s", dir, url));
		} else if (recordHeader.type == SWF_TAG_DEFINE_BITS_JPEG2) {
			logf("Saw JpegBits2\n");
			skipTag = true; // #soon
		} else if (recordHeader.type == SWF_TAG_DEFINE_BITS_JPEG3) {
			logf("Saw JpegBits3\n");
			skipTag = true; // #soon
		} else if (recordHeader.type == SWF_TAG_DEFINE_BITS) {
			logf("Saw DefineBits\n");
			skipTag = true; // #soon
		} else if (recordHeader.type == SWF_TAG_JPEG_TABELS) {
			logf("Saw JpegTabels\n");
			skipTag = true; // #soon
		} else if (recordHeader.type == SWF_TAG_CSM_TEXT_SETTINGS) {
			skipTag = true; // We don't need super advanced text handling... yet
		} else if (recordHeader.type == SWF_TAG_DEFINE_MORPH_SHAPE) {
			logf("Skipping morphshape\n");
			skipTag = true; // I dunno yet.
		} else if (recordHeader.type == SWF_TAG_DEFINE_MORPH_SHAPE2) {
			logf("Skipping morphshape2\n");
			skipTag = true; // I dunno yet.
		} else if (
			recordHeader.type == SWF_TAG_PLACE_OBJECT ||
			recordHeader.type == SWF_TAG_PLACE_OBJECT2 ||
			recordHeader.type == SWF_TAG_PLACE_OBJECT3 ||
			recordHeader.type == SWF_TAG_SHOW_FRAME ||
			recordHeader.type == SWF_TAG_FRAME_LABEL ||
			recordHeader.type == SWF_TAG_REMOVE_OBJECT2
		) {
			skipTag = true; // We don't handle stuff placed on the main stage
		} else if (
			recordHeader.type == SWF_TAG_DO_ABC2 ||
			recordHeader.type == SWF_TAG_ENABLE_DEBUGGER2 ||
			recordHeader.type == SWF_TAG_PROTECT ||
			recordHeader.type == SWF_TAG_DO_ACTION ||
			recordHeader.type == SWF_TAG_DEFINE_BUTTON2
		) {
			skipTag = true; // Fuck action script
		} else if (
			recordHeader.type == SWF_TAG_SOUND_STREAM_BLOCK ||
			recordHeader.type == SWF_TAG_SOUND_STREAM_HEAD ||
			recordHeader.type == SWF_TAG_SOUND_STREAM_HEAD2 ||
			recordHeader.type == SWF_TAG_DEFINE_SOUND ||
			recordHeader.type == SWF_TAG_START_SOUND
		) {
			skipTag = true; // no sounds
		} else if (recordHeader.type == SWF_TAG_END) {
			break;
		} else {
			logf("Failed to parse code: %d, len: %d\n", recordHeader.type, recordHeader.length);
			break;
		}

		if (skipTag) {
			skipBytes(&stream, recordHeader.length);
			swf->tagsNum--;
		}

		if (swf->tagsNum > swf->tagsMax-1) {
			swf->tags = (SwfTagPointer *)resizeArray(swf->tags, sizeof(SwfTagPointer), swf->tagsMax, swf->tagsMax * 1.5);
			swf->tagsMax *= 1.5;
		}
	}

	{ /// Collect character ids
		swf->characterMapAllocator.type = ALLOCATOR_DEFAULT;
		HashMap *characterMap = createHashMap(sizeof(u16), sizeof(SwfTagPointer *), 8192, &swf->characterMapAllocator);
		swf->characterMap = characterMap;

		for (int i = 0; i < swf->tagsNum; i++) {
			SwfTagPointer *tagPointer = &swf->tags[i];
			if (tagPointer->header.type == SWF_TAG_DEFINE_SHAPE) {
				SwfShape *shape = (SwfShape *)tagPointer->tag;
				hashMapSet(characterMap, &shape->shapeId, (int)shape->shapeId, &tagPointer);
			}
			if (tagPointer->header.type == SWF_TAG_DEFINE_SPRITE) {
				SwfSprite *sprite = (SwfSprite *)tagPointer->tag;
				hashMapSet(characterMap, &sprite->spriteId, (int)sprite->spriteId, &tagPointer);
			}
			if (tagPointer->header.type == SWF_TAG_DEFINE_EDIT_TEXT) {
				DefineEditText *text = (DefineEditText *)tagPointer->tag;
				hashMapSet(characterMap, &text->characterId, (int)text->characterId, &tagPointer);
			}
			if (tagPointer->header.type == SWF_TAG_DEFINE_BITS_LOSSLESS) {
				SwfBitmap *bitmap = (SwfBitmap *)tagPointer->tag;
				hashMapSet(characterMap, &bitmap->characterId, (int)bitmap->characterId, &tagPointer);
			}
		}
	}

	{ /// Collect shapes
		int shapeCount = 0;
		for (int i = 0; i < swf->tagsNum; i++) {
			SwfTagPointer *tagPointer = &swf->tags[i];
			SwfTagType tagType = tagPointer->header.type;
			if (tagType == SWF_TAG_DEFINE_SHAPE) {
				shapeCount++;
			}
		}

		swf->allShapes = (SwfShape **)malloc(sizeof(SwfShape *) * shapeCount);

		for (int i = 0; i < swf->tagsNum; i++) {
			SwfTagPointer *tagPointer = &swf->tags[i];
			SwfTagType tagType = tagPointer->header.type;
			if (tagType == SWF_TAG_DEFINE_SHAPE) {
				SwfShape *shape = (SwfShape *)tagPointer->tag;
				if (shape->drawEdges) free(shape->drawEdges);
				for (int i = 0; i < shape->fillStylesNum; i++) {
					FillStyle *fill = &shape->fillStyles[i];
					if (fill->fillStyleType == FILL_STYLE_CLIPPED_BITMAP || fill->fillStyleType == FILL_STYLE_REPEATING_BITMAP) {
						SwfTagPointer *tagPointer;
						if (hashMapGet(swf->characterMap, &fill->bitmapId, (int)fill->bitmapId, &tagPointer)) {
							SwfBitmap *bitmap = (SwfBitmap *)tagPointer->tag;
							fill->bitmap = bitmap;
						}
					}
				}
				swf->allShapes[swf->allShapesNum++] = shape;
			}
		}
	}
	// logf("%d shapes * %d bytes = %.1fmb\n", swf->allShapesNum, sizeof(SwfShape), (swf->allShapesNum*sizeof(SwfShape))/(float)(Megabytes(1)));

	int swfHighestDepth = 0;
	for (int i = 0; i < swf->tagsNum; i++) {
		SwfTagPointer *tagPointer = &swf->tags[i];
		SwfTagType tagType = tagPointer->header.type;
		if (tagType != SWF_TAG_DEFINE_SPRITE) continue;
		SwfSprite *sprite = (SwfSprite *)tagPointer->tag;
		if (swfHighestDepth < sprite->highestDepth+1) swfHighestDepth = sprite->highestDepth+1;
	}

	int tempDepthsMax = swfHighestDepth+1;
	SwfDrawable *tempDepths = (SwfDrawable *)malloc(sizeof(SwfDrawable) * tempDepthsMax);

	{ /// Collect sprites
		int spriteCount = 0;
		for (int i = 0; i < swf->tagsNum; i++) {
			SwfTagPointer *tagPointer = &swf->tags[i];
			SwfTagType tagType = tagPointer->header.type;
			if (tagType == SWF_TAG_DEFINE_SPRITE) spriteCount++;
		}

		swf->allSprites = (SwfSprite **)malloc(sizeof(SwfSprite *) * spriteCount);

		for (int i = 0; i < swf->tagsNum; i++) {
			SwfTagPointer *tagPointer = &swf->tags[i];
			SwfTagType tagType = tagPointer->header.type;
			if (tagType != SWF_TAG_DEFINE_SPRITE) continue;

			SwfSprite *sprite = (SwfSprite *)tagPointer->tag;
			swf->allSprites[swf->allSpritesNum++] = sprite;

			memset(tempDepths, 0, sizeof(SwfDrawable) * tempDepthsMax);

			{ /// Setup frames
				int frameCount = 0;
				int totalFrameLabelCount = 0;
				for (int i = 0; i < sprite->controlTagsNum; i++) {
					ControlTag *tag = &sprite->controlTags[i];
					if (tag->type == SWF_TAG_SHOW_FRAME) frameCount++;
					if (tag->type == SWF_TAG_FRAME_LABEL && tag->frameLabel.name[0] != '/') totalFrameLabelCount++;
				}
				sprite->frames = (SwfFrame *)zalloc(sizeof(SwfFrame) * frameCount);
				sprite->framesNum = frameCount;
				sprite->labelsInOrder = (char **)zalloc(sizeof(char *) * totalFrameLabelCount);

				int currentFrameIndex = 0;
				bool firstTagOfFrame = true;
				SwfFrame *currentFrame = &sprite->frames[currentFrameIndex];
				for (int i = 0; i < sprite->controlTagsNum; i++) {
					ControlTag *tag = &sprite->controlTags[i];
					if (tag->type == SWF_TAG_END) break;

					if (firstTagOfFrame) {
						firstTagOfFrame = false;
						int frameLabelCount = 0;
						for (int j = i; j < sprite->controlTagsNum; j++) {
							ControlTag *otherTag = &sprite->controlTags[j];
							if (otherTag->type == SWF_TAG_FRAME_LABEL && otherTag->frameLabel.name[0] != '/') frameLabelCount++;
							if (otherTag->type == SWF_TAG_SHOW_FRAME) break;
						}
						currentFrame->labels = (char **)malloc(sizeof(char *) * frameLabelCount);
					}

					if (tag->type == SWF_TAG_SHOW_FRAME) {
						int realDepths = 0;
						for (int i = 0; i < tempDepthsMax; i++) {
							if (tempDepths[i].type != SWF_DRAWABLE_NONE) realDepths++;
						}
						currentFrame->depths = (SwfDrawable *)allocateMemory(swf->drawablesArena, sizeof(SwfDrawable) * realDepths);
						for (int i = 0; i < tempDepthsMax; i++) {
							if (tempDepths[i].type != SWF_DRAWABLE_NONE) currentFrame->depths[currentFrame->depthsNum++] = tempDepths[i];
						}
						currentFrameIndex++;
						currentFrame = &sprite->frames[currentFrameIndex];
						firstTagOfFrame = true;
					} else if (tag->type == SWF_TAG_PLACE_OBJECT) {
						PlaceObject *placeObject = &tag->placeObject;
						if (placeObject->pfHasCharacter || placeObject->pfHasClassName) {
							SwfDrawable drawable = makeDrawableById(swf, placeObject);
							drawable.depth = placeObject->depth;
							tempDepths[placeObject->depth] = drawable;
						}

						SwfDrawable *drawable = &tempDepths[placeObject->depth];

						if (placeObject->pfHasMatrix) {
							drawable->matrix = toMatrix2x3(placeObject->matrix);
						} else if (placeObject->pfHasCharacter || placeObject->pfHasClassName) {
							drawable->matrix = toMatrix2x3(mat3());
						}

						if (placeObject->pfHasClipDepth) drawable->clipDepth = placeObject->clipDepth;
						if (placeObject->name) {
							drawable->name = stringClone(placeObject->name);
							free(placeObject->name);
						}

						ColorTransform zeroTransform = {};
						if (memcmp(&placeObject->colorTransform, &zeroTransform, sizeof(ColorTransform)) != 0) {
							drawable->colorTransform = (ColorTransform *)malloc(sizeof(ColorTransform));
							*drawable->colorTransform = placeObject->colorTransform;
						}

						drawable->spriteBlendMode = placeObject->blendMode;
						drawable->filters = placeObject->filters;
						drawable->filtersNum = placeObject->filtersNum;
					} else if (tag->type == SWF_TAG_REMOVE_OBJECT) {
						SwfDrawable *drawable = &tempDepths[tag->removeObject.depth];
						memset(drawable, 0, sizeof(SwfDrawable));
						drawable->type = SWF_DRAWABLE_NONE;
					} else if (tag->type == SWF_TAG_FRAME_LABEL) {
						if (tag->frameLabel.name[0] != '/') {
							char *label = stringClone(tag->frameLabel.name);
							free(tag->frameLabel.name);
							currentFrame->labels[currentFrame->labelsNum++] = label;
							sprite->labelsInOrder[sprite->labelsInOrderNum++] = label;
						}
					} else {
						logf("Bad frame tag\n");
					}
				}
				free(sprite->controlTags);
			}
		}

		/// Bounds and aliasing
		for (int i = 0; i < swf->allSpritesNum; i++) {
			SwfSprite *sprite = swf->allSprites[i];
			for (int i = 0; i < sprite->framesNum; i++) {
				SwfFrame *frame = &sprite->frames[i];
				int frameIndex = i;
				for (int i = 0; i < frame->depthsNum; i++) {
					SwfDrawable *drawable = &frame->depths[i];

					Rect newBounds = {};
					if (drawable->type == SWF_DRAWABLE_SHAPE) {
						newBounds = toRect(drawable->shape->shapeBounds);
					} else if (drawable->type == SWF_DRAWABLE_SPRITE) {
						newBounds = drawable->sprite->bounds;
					}

					if (!isZero(newBounds)) {
						newBounds = toMatrix3(drawable->matrix) * newBounds;
						sprite->bounds = insert(sprite->bounds, newBounds);
					}
				}
			}

			if (sprite->name) {
				SwfSprite *newSprite = getAliasedSprite(sprite, swf);
				if (sprite != newSprite) swf->allSprites[i] = newSprite;
			}
		}
	} ///
	free(tempDepths);

	// logf("%d sprites * %d bytes = %.1fmb\n", swf->allSpritesNum, sizeof(SwfShape), (swf->allSpritesNum*sizeof(SwfSprite))/(float)(Megabytes(1)));

	destroyHashMap(swf->characterMap);
	// logf("Parsing %s complete %d tags\n", path, swf->tagsNum);
	free(stream.toFree);

	float ms = getMsPassed(startTime);
	if (ms > 1000) logf("Took %.1fsec to load %s\n", ms/1000, path);
	return swf;
}

int processSubPath(DrawEdgeRecord *dest, int destNum, DrawEdgeRecord *src, int srcNum) {
	if (srcNum == 0) return destNum;

	DrawEdgeRecord *firstEdge = &src[0];

	if (firstEdge->fillStyle1Index != 0) {
		for (int i = 0; i < srcNum; i++) {
			DrawEdgeRecord newEdge = src[i];
			newEdge.fillStyleIndex = firstEdge->fillStyle1Index;
			newEdge.lineStyleIndex = 0;
			dest[destNum++] = newEdge;
		}
	}

	if (firstEdge->fillStyle0Index != 0) {
		for (int i = srcNum-1; i >= 0; i--) {
			DrawEdgeRecord newEdge = src[i];
			if (newEdge.type == DRAW_EDGE_STRAIGHT_EDGE) { 
				Vec2 temp = newEdge.control;
				newEdge.control = newEdge.start;
				newEdge.start = temp;
			} else {
				Vec2 temp = newEdge.anchor;
				newEdge.anchor = newEdge.start;
				newEdge.start = temp;
			}
			newEdge.fillStyleIndex = firstEdge->fillStyle0Index;
			newEdge.lineStyleIndex = 0;
			dest[destNum++] = newEdge;
		}
	}

	if (firstEdge->lineStyleIndex != 0) {
		for (int i = 0; i < srcNum; i++) {
			DrawEdgeRecord newEdge = src[i];
			newEdge.fillStyleIndex = 0;
			newEdge.lineStyleIndex = firstEdge->lineStyleIndex;
			dest[destNum++] = newEdge;
		}
	}

	return destNum;
}

SwfDrawable makeDrawableById(Swf *swf, PlaceObject *placeObject) {
	SwfDrawable drawable = {};

	SwfTagType tagType;
	void *characterPtr = NULL;

	if (placeObject->pfHasClassName) {
		for (int i = 0; i < swf->loadedSwfsNum; i++) {
			Swf *loadedSwf = swf->loadedSwfs[i];
			for (int i = 0; i < loadedSwf->allSpritesNum; i++) {
				SwfSprite *sprite = loadedSwf->allSprites[i];
				if (streq(placeObject->className, sprite->name)) {
					tagType = SWF_TAG_DEFINE_SPRITE;
					characterPtr = sprite;
					break;
				}
			}
			if (characterPtr) break;
		}
	} else {
		SwfTagPointer *tagPointer = NULL;
		u16 characterId = placeObject->characterId;
		if (hashMapGet(swf->characterMap, &characterId, (int)characterId, &tagPointer)) {
			tagType = tagPointer->header.type;
			characterPtr = tagPointer->tag;
		}
	}

	if (characterPtr) {
		if (tagType == SWF_TAG_DEFINE_SHAPE) {
			drawable.type = SWF_DRAWABLE_SHAPE;
			drawable.shape = (SwfShape *)characterPtr;
			return drawable;
		} else if (tagType == SWF_TAG_DEFINE_SPRITE) {
			drawable.type = SWF_DRAWABLE_SPRITE;
			drawable.sprite = (SwfSprite *)characterPtr;
			if (drawable.sprite->name) drawable.sprite = getAliasedSprite(drawable.sprite, swf);
			// if (drawable.sprite->name && stringStartsWith(drawable.sprite->name, "Invis_")) {
			// 	drawable.type = SWF_DRAWABLE_NONE;
			// 	drawable.sprite = NULL;
			// }
			return drawable;
		} else if (tagType == SWF_TAG_DEFINE_EDIT_TEXT) {
			drawable.type = SWF_DRAWABLE_SPRITE;
			SwfSprite *sprite = (SwfSprite *)zalloc(sizeof(SwfSprite));
			DefineEditText *editText = (DefineEditText *)characterPtr;
			sprite->isTextField = true;
			sprite->textAlign = editText->align;
			sprite->textColor = editText->textColor;
			sprite->bounds = toRect(editText->bounds);
			sprite->initialText = editText->initialText;

			for (int i = 0; i < swf->defineFontNamesNum; i++) {
				DefineFontName *fontName = &swf->defineFontNames[i];
				if (editText->fontId == fontName->fontId) sprite->font = fontName->font;
			}
			// if (!sprite->font) logf("No font for text field!?\n");

			drawable.sprite = sprite;
			return drawable;
		}
	}

	drawable.type = SWF_DRAWABLE_NONE;
	return drawable;
}

SwfSprite *getSpriteByName(Swf *swf, char *spriteName) {
	for (int i = 0; i < swf->allSpritesNum; i++) {
		SwfSprite *sprite = swf->allSprites[i];
		if (streq(sprite->name, spriteName)) {
			return sprite;
		}
	}

	for (int i = 0; i < swf->loadedSwfsNum; i++) {
		Swf *loadedSwf = swf->loadedSwfs[i];
		SwfSprite *sprite = getSpriteByName(loadedSwf, spriteName);
		if (sprite) return sprite;
	}

	return NULL;
}

SwfSprite *getAliasedSprite(SwfSprite *sourceSprite, Swf *swf) {
	for (int i = swf->loadedSwfsNum-1; i >= 0; i--) {
		Swf *nextSwf = swf->loadedSwfs[i];
		SwfSprite *newSprite = getAliasedSprite(sourceSprite, nextSwf);
		if (newSprite) return newSprite;
	}

	for (int i = 0; i < swf->allSpritesNum; i++) {
		SwfSprite *sprite = swf->allSprites[i];
		if (streq(sprite->name, sourceSprite->name)) {
			return sprite;
		}
	}

	return NULL;
}

bool hasLabel(SwfSprite *sprite, char *label) {
	for (int i = 0; i < sprite->labelsInOrderNum; i++) {
		if (streq(sprite->labelsInOrder[i], label)) return true;
	}

	return false;
}

char *getLabelWithPrefix(SwfSprite *sprite, char *prefix) {
	char **possible = (char **)frameMalloc(sprite->labelsInOrderNum * sizeof(char *));
	int possibleNum = 0;

	for (int i = 0; i < sprite->labelsInOrderNum; i++) {
		if (stringStartsWith(sprite->labelsInOrder[i], prefix)) possible[possibleNum++] = sprite->labelsInOrder[i];
	}

	if (possibleNum == 0) return NULL;
	return possible[rndInt(0, possibleNum-1)];
}

void printDrawEdges(DrawEdgeRecord *edges, int edgesNum) {
	for (int i = 0; i < edgesNum; i++) {
		DrawEdgeRecord *edge = &edges[i];
		if (edge->type == DRAW_EDGE_STRAIGHT_EDGE) {
			logf(
				"%d(%d): Line: %.2f %.2f -> %.2f %.2f (%d %d)\t\t[%d %d]\n",
				i,
				edge->origOrderIndex,
				edge->start.x,
				edge->start.y,
				edge->control.x,
				edge->control.y,
				edge->lineStyleIndex,
				edge->fillStyleIndex,
				edge->fillStyle0Index,
				edge->fillStyle1Index
			);
		}
		if (edge->type == DRAW_EDGE_CURVED_EDGE) {
			logf(
				"%d(%d): Curve: %.2f %.2f -> %.2f %.2f (%d %d)\t\t[%d %d]\n",
				i,
				edge->origOrderIndex,
				edge->start.x,
				edge->start.y,
				edge->anchor.x,
				edge->anchor.y,
				edge->lineStyleIndex,
				edge->fillStyleIndex,
				edge->fillStyle0Index,
				edge->fillStyle1Index
			);
		}
	}
}

void destroySwf(Swf *swf) {
	for (int i = 0; i < swf->tagsNum; i++) {
		SwfTagPointer *tagPointer = &swf->tags[i];
		if (tagPointer->header.type == SWF_TAG_DEFINE_SHAPE) {
			SwfShape *shape = (SwfShape *)tagPointer->tag;
			free(shape->fillStyles);
			free(shape->lineStyles);
		} else if (tagPointer->header.type == SWF_TAG_DEFINE_SPRITE) {
			SwfSprite *sprite = (SwfSprite *)tagPointer->tag;
			for (int i = 0; i < sprite->framesNum; i++) {
				SwfFrame *frame = &sprite->frames[i];
				for (int i = 0; i < frame->labelsNum; i++) free(frame->labels[i]);
				if (frame->labels) free(frame->labels);
			}
			free(sprite->frames);
			if (sprite->labelsInOrder) free(sprite->labelsInOrder);
		} else if (tagPointer->header.type == SWF_TAG_DEFINE_BITS_LOSSLESS) {
			SwfBitmap *bitmap = (SwfBitmap *)tagPointer->tag;
			free(bitmap->pixels);
		} else if (tagPointer->header.type == SWF_TAG_DEFINE_EDIT_TEXT) {
			DefineEditText *editText = (DefineEditText *)tagPointer->tag;
			if (editText->fontClass) free(editText->fontClass);
			if (editText->variableName) free(editText->variableName); // I think this always exists
			if (editText->initialText) free(editText->initialText);
		}

		free(tagPointer->tag);
	}

	free(swf->allShapes);
	free(swf->allSprites);
	free(swf->tags);
	destroyMemoryArena(swf->drawablesArena);

	for (int i = 0; i < swf->loadedSwfsNum; i++) destroySwf(swf->loadedSwfs[i]);
	free(swf);
}

int getSpriteFrameForLabel(SwfSprite *sprite, char *label, int afterFrame) {
	for (int i = afterFrame; i < sprite->framesNum; i++) {
		int frameIndex = i;
		SwfFrame *frame = &sprite->frames[frameIndex]; 
		for (int i = 0; i < frame->labelsNum; i++) {
			if (streq(frame->labels[i], label)) {
				return frameIndex;
			}
		}
	}

	return -1;
}

int getFrameForLabel(SwfSprite *sprite, char *label, int afterFrame) { return getSpriteFrameForLabel(sprite, label, afterFrame); }

Rect getFrameBounds(SwfSprite *sprite, int frameIndex) {
	if (frameIndex < 0 || frameIndex > sprite->framesNum-1) return makeRect();
	SwfFrame *frame = &sprite->frames[frameIndex];

	Rect bounds = makeRect();
	for (int i = 0; i < frame->depthsNum; i++) {
		SwfDrawable *drawable = &frame->depths[i];

		Rect newBounds = {};
		if (drawable->type == SWF_DRAWABLE_SHAPE) {
			newBounds = toRect(drawable->shape->shapeBounds);
		} else if (drawable->type == SWF_DRAWABLE_SPRITE) {
			// newBounds = drawable->sprite->bounds;
			newBounds = getFrameBounds(drawable->sprite, 0);
		}

		if (!isZero(newBounds)) {
			newBounds = toMatrix3(drawable->matrix) * newBounds;
			bounds = insert(bounds, newBounds);
		}
	}

	return bounds;
}
