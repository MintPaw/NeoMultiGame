#define MinNum(x, y) ((x) < (y) ? (x) : (y))
#define MaxNum(x, y) ((x) > (y) ? (x) : (y))
#define ArrayLength(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#define mathClamp(x, min, max) ((x) < (min) ? (min) : (x) > (max) ? (max) : (x))
#define MathClamp(x, min, max) ((x) < (min) ? (min) : (x) > (max) ? (max) : (x))
#define Clamp01(x) ((x) < (0) ? (0) : (x) > (1) ? (1) : (x))

#ifndef M_PI
# define M_PI 3.14159265359
#endif

#ifndef FORCE_INLINE
# define FORCE_INLINE
#endif

#define USE_BAD_COORDS

struct Rect;
struct Matrix3;
struct Matrix4;
struct Vec2;
struct Vec3;
struct Vec4;
struct Xform;
struct Tri;

enum Ease {
	LINEAR = 0,
	QUAD_IN, QUAD_OUT, QUAD_IN_OUT,
	CUBIC_IN, CUBIC_OUT, CUBIC_IN_OUT,
	QUART_IN, QUART_OUT, QUART_IN_OUT,
	QUINT_IN, QUINT_OUT, QUINT_IN_OUT,
	SINE_IN, SINE_OUT, SINE_IN_OUT,
	CIRC_IN, CIRC_OUT, CIRC_IN_OUT,
	EXP_IN, EXP_OUT, EXP_IN_OUT,
	ELASTIC_IN, ELASTIC_OUT, ELASTIC_IN_OUT,
	BACK_IN, BACK_OUT, BACK_IN_OUT,
	BOUNCE_IN, BOUNCE_OUT, BOUNCE_IN_OUT
};

const char *easeStrings[] = {
	"Linear",
	"Quad in", "Quad out", "Quad in/out",
	"Cubic in", "Cubic out", "Cubic in/out",
	"Quart in", "Quart out", "Quart in/out",
	"Quint in", "Quint out", "Quint in/out",
	"Sine in", "Sine out", "Sine in/out",
	"Circ in", "Circ out", "Circ in/out",
	"Exp in", "Exp out", "Exp in/out",
	"Elastic in", "Elastic out", "Elastic in/out",
	"Back in", "Back out", "Back in/out",
	"Bounce in", "Bounce out", "Bounce in/out",
};

float roundToNearest(float num, float nearest=1);
Vec2 roundToNearest(Vec2 num, Vec2 nearest);
float ceilToNearest(float num, float nearest=1);
bool lineIntersectsLine(Vec2 start1, Vec2 end1, Vec2 start2, Vec2 end2);
bool lineIntersectsCircle(Vec2 lineStart, Vec2 lineEnd, Vec2 circlePosition, float radius);

float distanceBetween(float x1, float y1, float x2, float y2);
float radsBetween(Vec2 p1, Vec2 p2);
float degsBetween(Vec2 p1, Vec2 p2);
Vec2 vectorBetween(Vec2 p1, Vec2 p2);

float normalizeRad(float rad);
float normalizeRad(float rad) {
	float result = rad;
	while (result < -M_PI) result += M_PI*2;
	while (result > M_PI) result -= M_PI*2;
	return result;
}

Vec4 eulerToQuaternion(Vec3 angle);
Vec3 quaternionToEuler(Vec4 quat);
Vec4 multiplyQuaternions(Vec4 q1, Vec4 q2);
Vec4 conjugateQuaternion(Vec4 quat);
Vec4 angleAxisToQuaternion(float rads, Vec3 axis);
void quaternionAngleAxis(Vec4 quat, float *angle, Vec3 *axis);
Vec4 quaternionMultiply(Vec4 q1, Vec4 q2);
Vec4 slerpQuaternions(Vec4 q1, Vec4 q2, float perc);
Vec4 nlerpQuaternions(Vec4 q1, Vec4 q2, float perc);

void findCofactor(float mat[4][4], float temp[4][4], int p, int q, int n);
float findDeterminant(float mat[4][4], int n);
void findAdjoint(float A[4][4], float adj[4][4]);
bool findInverse(float A[4][4], float inverse[4][4]);

float triangleSign(Vec2 p1, Vec2 p2, Vec2 p3);
bool pointInTriangle(Vec2 *tri, Vec2 point);
bool pointInTriangle(Vec2 pt, Vec2 v1, Vec2 v2, Vec2 v3);

float timePhase(float time, float freq=1);
float lerp(float min, float max, float perc);
Vec2 lerp(Vec2 min, Vec2 max, float perc);
Vec3 lerp(Vec3 min, Vec3 max, float perc);
Vec4 lerp(Vec4 min, Vec4 max, float perc);
Matrix3 lerp(Matrix3 min, Matrix3 max, float perc);
Matrix4 lerp(Matrix4 min, Matrix4 max, float perc);
Xform lerp(Xform min, Xform max, float perc);
float lerpRad(float min, float max, float perc);
float lerpDeg(float min, float max, float perc);
Vec2 lerpVec2(Vec2 min, Vec2 max, float perc); //@hack Change all these to use the overloaded lerp() instead
Vec3 lerpVec3(Vec3 min, Vec3 max, float perc);
int lerpColor(int colour1, int colour2, float perc);

float cubicBezier(float x2, float y2, float x3, float y3, float t);
float moveTowards(float value, float target, float speed);
Vec2 moveTowards(Vec2 value, Vec2 target, float speed);
Vec2 degToVec2(float deg);
Vec2 radToVec2(float rad);

float norm(float min, float max, float value);

int wrap(int min, int max, int value);
s16 FORCE_INLINE clampedS16Add(int left, int right);

float map(float value, float sourceMin, float sourceMax, float destMin, float destMax, Ease ease = LINEAR);
float clampMap(float value, float sourceMin, float sourceMax, float destMin, float destMax, Ease ease = LINEAR);
float clampMap(float value, Vec4 sourceDest, Ease ease=LINEAR);
float FORCE_INLINE dot(Vec2 a, Vec2 b);

int argbToHex(unsigned char a, unsigned char r, unsigned char g, unsigned char b);
int argbToHex(Vec4 argb);
int argbFloatToHex(float a, float r, float g, float b);
void hexToArgb(int argbHex, int *a, int *r, int *g, int *b);
void hexToArgbFloat(int argbHex, float *a, float *r, float *g, float *b);
Vec4 hexToArgbFloat(int argbHex);
int argbToRgba(int argb);
Vec4 argbToRgba(Vec4 argb);
Vec4 argbToRgbaFloat(int argb);
int getAofArgb(int src);
int setAofArgb(int src, int newA);
int setBofArgb(int src, int newB);
int tintColor(int color, int tint);
int alphaColor(int color, float alpha);
int getComplement(int color);
float FORCE_INLINE srgbToLinear(float value);
float FORCE_INLINE linearToSrgb(float value);

float toDeg(float rads);
float toDeg(Vec2 vec);
float toRad(float degs);
float toRad(Vec2 vec);

void minMaxToCenterSize(Vec3 min, Vec3 max, Vec3 *center, Vec3 *size);
void centerSizeToMinMax(Vec3 center, Vec3 size, Vec3 *min, Vec3 *max);

Matrix4 getBeamMatrix(Vec3 start, Vec3 end, float thickness=0.1);

float tweenEase(float p, Ease ease);

struct Vec2 {
	float x;
	float y;

	Vec2 normalize();
	float length();

	Vec2 add(Vec2 other);
	Vec2 subtract(Vec2 other);
	Vec2 multiply(Vec2 other);
	Vec2 divide(Vec2 other);

	Vec2 add(float other);
	Vec2 subtract(float other);
	Vec2 multiply(float other);
	Vec2 divide(float other);

	float toRads();
	float toDeg();
	float dot(Vec2 other);

	Vec2 round();
	Vec2 rotate(Vec2 center, float rads);

	float distance(Vec2 other);
	float distance(float x, float y);

	bool isZero();
	bool equal(Vec2 other);
	int getIntHash();
	int getFloatHashAsInt() { return this->x*this->y; };
	void print(const char *label=NULL);

	Vec2 operator/= (Vec2 b) { this->x /= b.x; this->y /= b.y; return *this; }
	Vec2 operator/= (float b) { this->x /= b; this->y /= b; return *this; }
	Vec2 operator*= (Vec2 b) { this->x *= b.x; this->y *= b.y; return *this; }
	Vec2 operator*= (float b) { this->x *= b; this->y *= b; return *this; }
	Vec2 operator+= (Vec2 b) { this->x += b.x; this->y += b.y; return *this; }
	Vec2 operator+= (float b) { this->x += b; this->y += b; return *this; }
	Vec2 operator-= (Vec2 b) { this->x -= b.x; this->y -= b.y; return *this; }
	Vec2 operator-= (float b) { this->x -= b; this->y -= b; return *this; }
	Vec2 operator-() { return {-this->x, -this->y}; }
};

Vec2 operator+ (Vec2 a, Vec2 b) { return { a.x + b.x, a.y + b.y }; }
Vec2 operator+ (Vec2 a, float b) { return { a.x + b, a.y + b }; }
Vec2 operator+ (float a, Vec2 b) { return { a + b.x, a + b.y }; }

Vec2 operator- (Vec2 a, Vec2 b) { return { a.x - b.x, a.y - b.y }; }
Vec2 operator- (Vec2 a, float b) { return { a.x - b, a.y - b }; }
Vec2 operator- (float a, Vec2 b) { return { a - b.x, a - b.y }; }

Vec2 operator* (Vec2 a, Vec2 b) { return { a.x * b.x, a.y * b.y }; }
Vec2 operator* (Vec2 a, float b) { return { a.x * b, a.y * b }; }
Vec2 operator* (float a, Vec2 b) { return { a * b.x, a * b.y }; }

Vec2 operator/ (Vec2 a, Vec2 b) { return { a.x / b.x, a.y / b.y }; }
Vec2 operator/ (Vec2 a, float b) { return { a.x / b, a.y / b }; }
Vec2 operator/ (float a, Vec2 b) { return { a / b.x, a / b.y }; }

Vec2 FORCE_INLINE v2(float x=0, float y=0);
Vec2 FORCE_INLINE v2(Vec3 xyz);

Vec2 v2(float x, float y) {
	Vec2 vec = {x, y};
	return vec;
}

bool FORCE_INLINE equal(Vec2 a, Vec2 b);
bool FORCE_INLINE equal(Vec2 a, Vec2 b) {
	float dx = b.x - a.x;
	float dy = b.y - a.y;
	return dx*dx + dy*dy < 0.0001;
}

float distance(Vec2 a, Vec2 b);
float distance(Vec2 a, Vec2 b) {
	return a.distance(b);
}

Vec2 normalize(Vec2 vec);
Vec2 normalize(Vec2 vec) {
	return vec.normalize();
}

float length(Vec2 vec);
float length(Vec2 vec) {
	return vec.length();
}

Vec2 ceil(Vec2 vec);
Vec2 ceil(Vec2 vec) {
	vec.x = ceilf(vec.x);
	vec.y = ceilf(vec.y);
	return vec;
}

template <typename T>
struct Vec2t {
	T x;
	T y;

	int getFloatHashAsInt() {
		return this->x*this->y;
	};

	bool equal(Vec2t other) {
		return this->x == other.x && this->y == other.y;
	};

	bool isZero() {
		return this->x == 0 && this->y == 0;
	};

	T FORCE_INLINE distance(Vec2t other) {
		float a = other.x - this->x;
		float b = other.y - this->y;

		float c = sqrt(a*a + b*b);
		return c;
	};

	T FORCE_INLINE distance(Vec2 other) {
		float a = other.x - (float)this->x;
		float b = other.y - (float)this->y;
		float c = sqrt(a*a + b*b);
		return c;
	};
};

template <typename T>
Vec2t<T> FORCE_INLINE v2t(T x=0, T y=0);

template <typename T>
Vec2t<T> v2t(T x, T y) {
	return {x, y};
}

struct Vec3 {
	union {
		struct {
			float x;
			float y;
			float z;
		};
		float m[3];
	};

	Vec3 add(Vec3 point);
	Vec3 subtract(Vec3 point);
	Vec3 multiply(Vec3 point);
	Vec3 multiply(float other);
	float dot(Vec3 other);
	Vec3 cross(Vec3 other);
	Vec3 normalize();
	float distance(Vec3 other);
	float length();
	bool isZero();

	bool equals(Vec3 other) {
		if (this->distance(other) < 0.0001) return true;
		return false;
	}

	void print(const char *label=NULL);

	Vec3 operator/= (Vec3 b) { this->x /= b.x; this->y /= b.y; this->z /= b.z; return *this; }
	Vec3 operator/= (float b) { this->x /= b; this->y /= b; this->z /= b; return *this; }
	Vec3 operator*= (Vec3 b) { this->x *= b.x; this->y *= b.y; this->z *= b.z; return *this; }
	Vec3 operator*= (float b) { this->x *= b; this->y *= b; this->z *= b; return *this; }
	Vec3 operator+= (Vec3 b) { this->x += b.x; this->y += b.y; this->z += b.z; return *this; }
	Vec3 operator+= (float b) { this->x += b; this->y += b; this->z += b; return *this; }
	Vec3 operator-= (Vec3 b) { this->x -= b.x; this->y -= b.y; this->z -= b.z; return *this; }
	Vec3 operator-= (float b) { this->x -= b; this->y -= b; this->z -= b; return *this; }
	Vec3 operator-() { return {-this->x, -this->y, -this->z}; }
};

Vec3 v3(float x=0, float y=0, float z=0);
Vec3 v3(float x, float y, float z) {
	Vec3 vec = {x, y, z};
	return vec;
}
Vec3 v3(Vec2 inPoint, float z=0);
Vec3 v3(Vec2 inPoint, float z) {
	Vec3 vec = {inPoint.x, inPoint.y, z};
	return vec;
}


Vec3 operator+ (Vec3 a, Vec3 b) { return { a.x + b.x, a.y + b.y, a.z + b.z }; }
Vec3 operator+ (Vec3 a, float b) { return { a.x + b, a.y + b, a.z + b }; }
Vec3 operator+ (float a, Vec3 b) { return { a + b.x, a + b.y, a + b.z }; }

Vec3 operator- (Vec3 a, Vec3 b) { return { a.x - b.x, a.y - b.y, a.z - b.z }; }
Vec3 operator- (Vec3 a, float b) { return { a.x - b, a.y - b, a.z - b }; }


Vec3 operator* (Vec3 a, Vec3 b) { return { a.x * b.x, a.y * b.y, a.z * b.z }; }
Vec3 operator* (Vec3 a, float b) { return { a.x * b, a.y * b, a.z * b }; }
Vec3 operator* (float a, Vec3 b) { return { a * b.x, a * b.y, a * b.z }; }

Vec3 operator/ (Vec3 a, Vec3 b) { return { a.x / b.x, a.y / b.y, a.z / b.z }; }
Vec3 operator/ (Vec3 a, float b) { return { a.x / b, a.y / b, a.z / b }; }
Vec3 operator/ (float a, Vec3 b) { return { a / b.x, a / b.y, a / b.z }; }

bool isZero(Vec3 vec);
bool isZero(Vec3 vec) {
	return vec.x == 0 && vec.y == 0 && vec.z == 0;
}

bool equal(Vec3 a, Vec3 b);
bool equal(Vec3 a, Vec3 b) {
	float dx = b.x - a.x;
	float dy = b.y - a.y;
	float dz = b.z - a.z;
	float distCubed = dx*dx + dy*dy + dz*dz;

	if (distCubed < 0.0001) return true;
	return false;
}


float distance(Vec3 a, Vec3 b);
float distance(Vec3 a, Vec3 b) {
	return a.distance(b);
}

Vec3 normalize(Vec3 vec);
Vec3 normalize(Vec3 vec) {
	return vec.normalize();
}

float length(Vec3 vec);
float length(Vec3 vec) {
	return vec.length();
}

Vec3 cross(Vec3 a, Vec3 b);
Vec3 cross(Vec3 a, Vec3 b) {
	return a.cross(b);
}

float dot(Vec3 a, Vec3 b);
float dot(Vec3 a, Vec3 b) {
	return a.dot(b);
}

struct Vec4 {
	float x;
	float y;
	float z;
	float w;

	float dot(Vec4 other);
	Vec4 normalize();
	Vec4 negate();
	void print(const char *label=NULL);

	float distance(Vec4 other) {
		float a = other.x - this->x;
		float b = other.y - this->y;
		float c = other.z - this->z;
		float d = other.w - this->w;

		float e = sqrt(a*a + b*b + c*c + d*d);
		return e;
	}


	bool equals(Vec4 other) {
		if (this->distance(other) < 0.001) return true;
		return false;
	}

	Vec4 operator*= (float b);
	Vec4 operator+= (float b);
	Vec4 operator+= (Vec4 b);
};
Vec4 operator+ (Vec4 a, Vec4 b) { return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; }
Vec4 operator+ (Vec4 a, float b) { return { a.x + b, a.y + b, a.z + b, a.w + b }; }
Vec4 operator+ (float a, Vec4 b) { return { a + b.x, a + b.y, a + b.z, a + b.w }; }
Vec4 Vec4::operator+= (float b) { *this = (*this) + b; return *this; }
Vec4 Vec4::operator+= (Vec4 b) { *this = (*this) + b; return *this; }

Vec4 operator- (Vec4 a, Vec4 b) { return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w }; }

Vec4 operator* (Vec4 a, Vec4 b) { return { a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w }; }
Vec4 operator* (Vec4 a, float b) { return { a.x * b, a.y * b, a.z * b, a.w * b }; }
Vec4 operator* (float a, Vec4 b) { return { a * b.x, a * b.y, a * b.z, a * b.w }; }
Vec4 Vec4::operator*= (float b) { *this = (*this) * b; return *this; }

Vec4 operator/ (Vec4 a, Vec4 b) { return { a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w }; }
Vec4 operator/ (Vec4 a, float b) { return { a.x / b, a.y / b, a.z / b, a.w / b }; }
Vec4 operator/ (float a, Vec4 b) { return { a / b.x, a / b.y, a / b.z, a / b.w }; }

Vec4 v4(float x=0, float y=0, float z=0, float w=0);
Vec4 v4(float x, float y, float z, float w) {
	Vec4 vec = {x, y, z, w};
	return vec;
}

Vec4 v4(Vec3 xyz, float w=0);
Vec4 v4(Vec3 xyz, float w) {
	Vec4 vec = {xyz.x, xyz.y, xyz.z, w};
	return vec;
}

struct Xform {
	Vec3 translation;
	Vec4 rotation;
	Vec3 scale;

	void print(const char *label=NULL) {
		logf("%s:\n", label);
		this->translation.print("translation");
		this->rotation.print("rotation");
		this->scale.print("scale");
	}

	bool equals(Xform other) {
		if (!this->translation.equals(other.translation)) return false;
		if (!this->rotation.equals(other.rotation)) return false;
		if (!this->scale.equals(other.scale)) return false;
		return true;
	}
};

struct Rect {
	float x;
	float y;
	float width;
	float height;

	bool equals(Rect other);
	bool contains(Vec2 point);
	bool contains(float px, float py);
	float distanceToPerimeter(float px, float py);
	Vec2 getClosestPoint(Vec2 from);
	Vec2 pos();
	Vec2 center();
	Vec2 size();
	Rect inflate(Vec2 size);
	Rect inflate(float size);
	void print(const char *label=NULL);
};
Rect operator* (Rect rect, Vec2 vec) {
	rect.x *= vec.x;
	rect.y *= vec.y;
	rect.width *= vec.x;
	rect.height *= vec.y;
	return rect;
}

Rect operator* (Rect rect, float value) {
	rect.x *= value;
	rect.y *= value;
	rect.width *= value;
	rect.height *= value;
	return rect;
}

struct Tri {
	Vec3 verts[3];
};

bool equal(Tri tri1, Tri tri2);
bool equal(Tri tri1, Tri tri2) {
	if (!tri1.verts[0].equals(tri2.verts[0])) return false;
	if (!tri1.verts[1].equals(tri2.verts[1])) return false;
	if (!tri1.verts[2].equals(tri2.verts[2])) return false;
	return true;
}

bool rayIntersectsTriangle(Vec3 orig, Vec3 dir, Tri tri, float *ret, Vec2 *uv);

Tri makeTriangle(Vec3 *verts);
Tri makeTriangle(Vec3 *verts) {
	Tri tri;
	tri.verts[0] = verts[0];
	tri.verts[1] = verts[1];
	tri.verts[2] = verts[2];
	return tri;
}

struct AABB {
	Vec3 min;
	Vec3 max;

	void print(const char *label=NULL) {
		logf("%s (%f, %f, %f) (%f, %f, %f)\n", label, this->min.x, this->min.y, this->min.z, this->max.x, this->max.y, this->max.z);
	}

	AABB operator+= (Vec3 vec) { 
		this->min += vec;
		this->max += vec;
		return *this;
	}

	AABB operator-= (Vec3 vec) { 
		this->min -= vec;
		this->max -= vec;
		return *this;
	}
};
AABB operator+ (AABB a, Vec3 b) {
	a.min += b;
	a.max += b;
	return a;
}
AABB operator* (AABB a, Vec3 b) {
	a.min *= b;
	a.max *= b;
	return a;
}

struct Capsule3 {
	Vec3 start;
	Vec3 end;
	float radius;
};

struct Matrix3 {
	float data[9];

	void zero();

	Matrix3 add(Matrix3 matrix) {
		Matrix3 ret;
		for (int i = 0; i < 9; i++) ret.data[i] = this->data[i] + matrix.data[i];
		return ret;
	}
	Vec3 multiply(Vec3 point);
	Matrix3 FORCE_INLINE multiply(float *array);
	Matrix3 FORCE_INLINE multiply(float value);
	float getRotationDeg();
	Matrix3 invert();
	Matrix3 transpose();
	Vec2 FORCE_INLINE multiply(Vec2 point);
	Matrix3 multiply(Matrix3 other);
	Rect multiply(Rect rect);
	bool equal(Matrix3 other);
	bool isZero();

	void print(const char *label=NULL);

	Matrix3 operator+= (Vec2 pos) { this->data[6] += pos.x; this->data[7] += pos.y; return *this; }

	void TRANSLATE(Vec2 pos) {
		if (pos.x == 0 && pos.y == 0) return;

		float array[9] = {
			1     , 0     , 0 ,
			0     , 1     , 0 ,
			pos.x , pos.y , 1 ,
		};

		*this = this->multiply(array);
	}
	void TRANSLATE(float x, float y) { this->TRANSLATE(v2(x, y)); }
	void TRANSLATE(float amount) { this->TRANSLATE(v2(amount, amount)); }

	void SCALE(Vec2 scale) {
		if (scale.x == 1 && scale.y == 1) return;

		float array[9] = {
			scale.x , 0       , 0 ,
			0       , scale.y , 0 ,
			0       , 0       , 1 ,
		};

		*this = this->multiply(array);
	}
	void SCALE(float x, float y) { this->SCALE(v2(x, y)); }
	void SCALE(float scale) { this->SCALE(v2(scale, scale)); }

	void SHEAR(Vec2 shear) {
		if (shear.x == 0 && shear.y == 0) return;

		float array[9] = {
			1,        shear.y , 0 ,
			shear.x , 1       , 0 ,
			0       , 0       , 1 ,
		};

		*this = this->multiply(array);
	}

	void ROTATE(float deg) {
		if (deg == 0) return;

		float s = sin(deg*M_PI/180.0);
		float c = cos(deg*M_PI/180.0);
#if defined(USE_BAD_COORDS)
		float array[9] = {
			c,  s, 0,
			-s, c, 0,
			0,  0, 1
		};
#else
		float array[9] = {
			c,  -s, 0,
			s,  c,  0,
			0,  0,  1
		};
#endif

		*this = this->multiply(array);
	}

	void ROTATE_X(float deg) {
		if (deg == 0) return;

		float s = sin(deg*M_PI/180.0);
		float c = cos(deg*M_PI/180.0);
		float array[9] = {
			1, 0,  0,
			0, c,  s,
			0, -s, c
		};

		*this = this->multiply(array);
	}

	void ROTATE_Y(float deg) {
		if (deg == 0) return;

		float s = sin(deg*M_PI/180.0);
		float c = cos(deg*M_PI/180.0);
		float array[9] = {
			c, 0, -s,
			0, 1, 0,
			s, 0, c
		};

		*this = this->multiply(array);
	}

	Matrix3 operator*= (Matrix3 b) { *this = this->multiply(b); return *this; }
};

Vec2 operator* (Matrix3 a, Vec2 b) { return a.multiply(b); }
Vec3 operator* (Matrix3 a, Vec3 b) { return a.multiply(b); }
Matrix3 operator* (Matrix3 a, Matrix3 b) { return a.multiply(b); }
Matrix3 operator* (Matrix3 a, float b) { return a.multiply(b); }

Vec2 FORCE_INLINE getPosition(Matrix3 matrix);
Vec2 getPosition(Matrix3 matrix) {
	Vec2 ret;
	ret.x = matrix.data[6];
	ret.y = matrix.data[7];
	return ret;
}

float FORCE_INLINE getRotationDeg(Matrix3 matrix);
float getRotationDeg(Matrix3 matrix) {
	float ret = matrix.getRotationDeg();
#if defined(USE_BAD_COORDS)
	ret *= -1;
#endif
	return ret;
}

Vec2 FORCE_INLINE getScale(Matrix3 matrix);
Vec2 getScale(Matrix3 matrix) {
#if defined(USE_BAD_COORDS)
	Vec2 vec0 = v2(matrix.data[0], -matrix.data[3]);
	Vec2 vec1 = v2(-matrix.data[1], matrix.data[4]);
#else
	Vec2 vec0 = v2(matrix.data[0], matrix.data[3]);
	Vec2 vec1 = v2(matrix.data[1], matrix.data[4]);
#endif

	Vec2 ret;
	ret.x = vec0.length();
	ret.y = vec1.length();
	return ret;
}


struct Matrix4 {
	float data[16];

	void setTo(float a, float b, float c, float d, float e, float f, float g, float h, float i, float j, float k, float l, float m, float n, float o, float p);
	void setTo(float *array);

	void setIdentity();
	Matrix4 perspective(float fovYDeg, float aspect, float nearPlane, float farPlane);

	Matrix4 multiply(float *array);
	Matrix4 multiply(Matrix4 matrix);
	Vec3 multiply(Vec3 point);
	Vec3 multiplyAffine(Vec3 point);
	Vec4 multiply(Vec4 point);
	AABB multiply(AABB bounds);
	Matrix4 multiply(float num);
	Matrix4 add(Matrix4 matrix);
	Matrix4 transpose();
	Matrix4 invert();

	Vec3 getPosition();
	Vec4 getQuaternion();
	Vec3 getScale();

	void TRANSLATE(Vec3 pos) { 
		float array[16] = {
			1     , 0     , 0     , 0 ,
			0     , 1     , 0     , 0 ,
			0     , 0     , 1     , 0 ,
			pos.x , pos.y , pos.z , 1 ,
		};

		*this = this->multiply(array);
	}
	void TRANSLATE(float x, float y, float z) { 
		this->TRANSLATE(v3(x, y, z));
	}

	void SCALE(Vec3 scale) {
		float array[16] = {
			scale.x , 0       , 0       , 0 ,
			0       , scale.y , 0       , 0 ,
			0       , 0       , scale.z , 0 ,
			0       , 0       , 0       , 1 ,
		};

		*this = this->multiply(array);
	}
	void SCALE(float x, float y, float z) { this->SCALE(v3(x, y, z)); }
	void SCALE(float scale) { this->SCALE(v3(scale, scale, scale)); }

	void ROTATE_QUAT(Vec4 quat) {
		Vec4 normalizedQuat = quat.normalize();
		float x = normalizedQuat.x;
		float y = normalizedQuat.y;
		float z = normalizedQuat.z;
		float w = normalizedQuat.w;

		float array[16] = {
			1.0f - 2.0f*y*y - 2.0f*z*z , 2.0f*x*y + 2.0f*z*w        , 2.0f*x*z - 2.0f*y*w        , 0.0f ,
			2.0f*x*y - 2.0f*z*w        , 1.0f - 2.0f*x*x - 2.0f*z*z , 2.0f*y*z + 2.0f*x*w        , 0.0f ,
			2.0f*x*z + 2.0f*y*w        , 2.0f*y*z - 2.0f*x*w        , 1.0f - 2.0f*x*x - 2.0f*y*y , 0.0f ,
			0.0f                       , 0.0f                       , 0.0f                       , 1.0f
		};

		*this = this->multiply(array);
	}
	void ROTATE_QUAT(float x, float y, float z, float w) { this->ROTATE_QUAT(v4(x, y, z, w)); }

	void ROTATE_EULER(Vec3 rotation) { 
		// *this = this->rotateEuler(rotation);
		Vec4 quat = eulerToQuaternion(rotation);
		this->ROTATE_QUAT(quat);
	}
	void ROTATE_EULER(float x, float y, float z) { this->ROTATE_EULER(v3(x, y, z)); }

	void print(const char *label=NULL);

	Matrix4 operator+= (Matrix4 b) { 
		for (int i = 0; i < 16; i++) {
			this->data[i] += b.data[i];
		}
		return *this;
	}
};

struct Line3 {
	Vec3 start;
	Vec3 end;
};

Matrix4 operator* (Matrix4 a, Matrix4 b) { return a.multiply(b); }
Vec3 operator* (Matrix4 a, Vec3 b) { return a.multiply(b); }
Vec4 operator* (Matrix4 a, Vec4 b) { return a.multiply(b); }
AABB operator* (Matrix4 a, AABB b) { return a.multiply(b); }
Matrix4 operator* (Matrix4 a, float b) { return a.multiply(b); }

Matrix4 getPerspectiveMatrix(float fovYDeg, float aspect, float nearPlane, float farPlane);

Matrix3 FORCE_INLINE mat3();
Matrix4 FORCE_INLINE mat4();
Rect makeRect(float x=0, float y=0, float width=0, float height=0);
Rect makeRect(Vec2 xy, Vec2 size);
Rect makeCenteredRect(Vec2 position, Vec2 size);
Rect makeCenteredSquare(Vec2 position, float size);
Rect getInnerRectOfAspect(Rect toFit, Vec2 aspect, Vec2 gravity=v2(0.5, 0.5));
Rect getInnerRectOfSize(Rect toFit, Vec2 size, Vec2 gravity);
Rect inflate(Rect rect, float size);
Rect inflate(Rect rect, Vec2 size);
Rect inflatePerc(Rect rect, float perc);
Rect inflatePerc(Rect rect, Vec2 perc);
Vec2 getSize(Rect rect);
Vec2 getPosition(Rect rect);
Xform newXform();
Matrix4 toMatrix(Xform xform);
Vec4 quaternionBetween(Vec3 start, Vec3 end, Vec3 axis={1, 0, 0});
Rect insert(Rect rect, Vec2 point);
Rect insert(Rect rect, Rect other);
bool isZero(Rect rect);


AABB makeAABB(Vec3 min=v3(), Vec3 max=v3());
AABB inflate(AABB bounds, float amount);
bool intersects(AABB bounds1, AABB bounds2);
bool intersects(AABB bounds, Tri tri);
bool overlaps(AABB aabb, Line3 line, Vec3 *hitOut);
float distance(AABB bounds1, AABB bounds2);
Vec3 getSize(AABB bounds);
Vec3 FORCE_INLINE getCenter(AABB bounds);
void getVerts(AABB bounds, Vec3 *verts);
AABB toAABB(Tri tri);
bool isZero(AABB bounds);
AABB expand(AABB bounds, AABB otherBounds);
AABB expand(AABB bounds, Vec3 point);
bool equal(AABB bounds1, AABB bounds2);

struct Line2 {
	Vec2 start;
	Vec2 end;
};

/// End header // ???

Vec2 v2(Vec3 xyz) {
	Vec2 vec = {xyz.x, xyz.y};
	return vec;
}


Vec3 v3(Vec4 xyzw) {
	Vec3 vec = {xyzw.x, xyzw.y, xyzw.z};
	return vec;
}

Matrix3 mat3() {
	Matrix3 mat = {
		1, 0, 0,
		0, 1, 0,
		0, 0, 1
	};
	return mat;
}

Matrix4 mat4() {
	Matrix4 mat = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	return mat;
}

Rect makeRect(float x, float y, float width, float height) {
	Rect rect = {x, y, width, height};
	return rect;
}

Rect makeRect(Vec2 position, Vec2 size) {
	Rect rect = {position.x, position.y, size.x, size.y};
	return rect;
}

Rect makeCenteredRect(Vec2 position, Vec2 size) {
	Rect rect = {};
	rect.x = position.x - size.x/2;
	rect.y = position.y - size.y/2;
	rect.width = size.x;
	rect.height = size.y;
	return rect;
}

Rect makeCenteredSquare(Vec2 position, float size) {
	return makeCenteredRect(position, v2(size, size));
}

Rect getInnerRectOfAspect(Rect toFit, Vec2 aspect, Vec2 gravity) {
	Rect ret = makeRect(v2(), aspect);
	float scaleX = toFit.width / ret.width;
	float scaleY = toFit.height / ret.height;
	if (scaleX < scaleY) {
		ret.width *= scaleX;
		ret.height *= scaleX;
	} else {
		ret.width *= scaleY;
		ret.height *= scaleY;
	}

	ret.x = toFit.x + toFit.width*gravity.x - ret.width*gravity.x;
	ret.y = toFit.y + toFit.height*gravity.y - ret.height*gravity.y;
	return ret;
}

Rect getInnerRectOfSize(Rect toFit, Vec2 size, Vec2 gravity) {
	Rect ret = makeRect(v2(), size);
	ret.x = toFit.x + toFit.width*gravity.x - ret.width*gravity.x;
	ret.y = toFit.y + toFit.height*gravity.y - ret.height*gravity.y;
	return ret;
}

Rect inflate(Rect rect, float size) {
	return inflate(rect, v2(size, size));
}
Rect inflate(Rect rect, Vec2 size) {
	rect.x -= size.x/2;
	rect.y -= size.y/2;
	rect.width += size.x;
	rect.height += size.y;
	return rect;
}
Rect inflatePerc(Rect rect, float perc) {
	return inflatePerc(rect, v2(perc, perc));
}
Rect inflatePerc(Rect rect, Vec2 perc) {
	Vec2 size = getSize(rect) * perc;
	rect.x -= size.x/2;
	rect.y -= size.y/2;
	rect.width += size.x;
	rect.height += size.y;
	return rect;
}

Vec2 getSize(Rect rect) {
	return v2(rect.width, rect.height);
}

Vec2 getPosition(Rect rect) {
	return v2(rect.x, rect.y);
}

Vec2 getCenter(Rect rect) {
	return rect.center();
}

float getDistance(Rect rect, Rect other);
float getDistance(Rect rect, Rect other) {
	Vec2 point = rect.getClosestPoint(getCenter(other));
	Vec2 otherPoint = other.getClosestPoint(point);
	float dist = otherPoint.distance(point);
	return dist;
}

void getVerts(AABB bounds, Vec3 *verts) {
	Vec3 size = getSize(bounds);

	verts[0] = bounds.min;
	verts[1] = bounds.min + v3(size.x, 0, 0);
	verts[2] = bounds.min + v3(0, size.y, 0);
	verts[3] = bounds.min + v3(0, 0, size.z);
	verts[4] = bounds.max;
	verts[5] = bounds.max - v3(size.x, 0, 0);
	verts[6] = bounds.max - v3(0, size.y, 0);
	verts[7] = bounds.max - v3(0, 0, size.z);
}

Xform newXform() {
	Xform xform;
	xform.translation = v3();
	xform.rotation = v4(0, 0, 0, 1);
	xform.scale = v3(1, 1, 1);
	return xform;
}

Matrix3 mat3FromQuat(Vec4 quat) {
	float x = quat.x, y = quat.y, z = quat.z, w = quat.w,
	tx = 2*x, ty = 2*y, tz = 2*z,
	txx = tx*x, tyy = ty*y, tzz = tz*z,
	txy = tx*y, txz = tx*z, tyz = ty*z,
	twx = w*tx, twy = w*ty, twz = w*tz;

	Matrix3 ret;
	ret.data[0] = 1 - (tyy + tzz);
	ret.data[1] = txy - twz;
	ret.data[2] = txz + twy;
	ret.data[3] = txy + twz;
	ret.data[4] = 1 - (txx + tzz);
	ret.data[5] = tyz - twx;
	ret.data[6] = txz - twy;
	ret.data[7] = tyz + twx;
	ret.data[8] = 1 - (txx + tyy);

	return ret.transpose();
}

Matrix4 toMatrix(Xform xform) {
	Matrix4 mat = mat4();
	mat.TRANSLATE(xform.translation);
	mat.SCALE(xform.scale);
	mat.ROTATE_QUAT(xform.rotation);
	return mat;
}

Xform multiplyXforms(Xform xform1, Xform xform2);
Xform multiplyXforms(Xform xform1, Xform xform2) {
	logf("multiplyXforms is a very bad function!\n");
	Xform result;
	result.translation = xform1.translation.add(xform2.translation);
	result.rotation = multiplyQuaternions(xform1.rotation, xform2.rotation);
	result.scale = xform1.scale.multiply(xform2.scale);
	return result;
}

Xform toXform(Matrix4 matrix);
Xform toXform(Matrix4 matrix) {
	Xform xform;
	xform.translation = matrix.getPosition();
	xform.rotation = matrix.getQuaternion();
	xform.scale = matrix.getScale();
	return xform;
}

float roundToNearest(float num, float nearest) {
	return roundf(num / nearest) * nearest;
}

Vec2 roundToNearest(Vec2 vec, Vec2 nearest) {
	vec.x = roundf(vec.x / nearest.x) * nearest.x;
	vec.y = roundf(vec.y / nearest.y) * nearest.y;
	return vec;
}

float ceilToNearest(float num, float nearest) {
	return ceilf(num / nearest) * nearest;
}

bool lineIntersectsLine(Vec2 start1, Vec2 end1, Vec2 start2, Vec2 end2) {
	logf("Totally untested\n");
	float ax = end1.x - start1.x;     // direction of line a
	float ay = end1.y - start1.y;     // ax and ay as above

	float bx = start2.x - end2.x;     // direction of line b, reversed
	float by = start2.y - end2.y;     // really -by and -by as above

	float dx = start2.x - start1.x;   // right-hand side
	float dy = start2.y - start1.y;

	float det = ax * by - ay * bx;

	if (det == 0) return false;

	float r = (dx * by - dy * bx) / det;
	float s = (ax * dy - ay * dx) / det;

	return !(r < 0 || r > 1 || s < 0 || s > 1);
}

bool lineIntersectsCircle(Vec2 lineStart, Vec2 lineEnd, Vec2 circlePosition, float radius) {
	Vec2 d = lineEnd.subtract(lineStart);
	Vec2 f = lineStart.subtract(circlePosition);
	float r = radius;

	float a = d.dot(d);
	float b = 2.0 * f.dot(d);
	float c = f.dot(f) - r*r;

	float discriminant = b*b - 4*a*c;
	if (discriminant < 0) {
		// no intersection
		return false;
	} else {
		// ray didn't totally miss sphere,
		// so there is a solution to
		// the equation.

		discriminant = sqrt(discriminant);

		// either solution may be on or off the ray so need to test both
		// t1 is always the smaller value, because BOTH discriminant and
		// a are nonnegative.
		float t1 = (-b - discriminant)/(2*a);
		float t2 = (-b + discriminant)/(2*a);

		// 3x HIT cases:
		//          -o->             --|-->  |            |  --|->
		// Impale(t1 hit,t2 hit), Poke(t1 hit,t2>1), ExitWound(t1<0, t2 hit), 

		// 3x MISS cases:
		//       ->  o                     o ->              | -> |
		// FallShort (t1>1,t2>1), Past (t1<0,t2<0), CompletelyInside(t1<0, t2>1)

		if( t1 >= 0 && t1 <= 1 ) {
			// t1 is the intersection, and it's closer than t2
			// (since t1 uses -b - discriminant)
			// Impale, Poke
			return true ;
		}

		// here t1 didn't intersect so we are either started
		// inside the sphere or completely past it
		if( t2 >= 0 && t2 <= 1 ) {
			// ExitWound
			return true;
		}

		// no intn: FallShort, Past, CompletelyInside
		return false;
	}
}

float distanceBetween(float x1, float y1, float x2, float y2) {
	float a = x2 - x1;
	float b = y2 - y1;

	float c = sqrt(a*a + b*b);
	return c;
}

float radsBetween(Vec2 p1, Vec2 p2) {
	return atan2(p2.y - p1.y, p2.x - p1.x);
}

float degsBetween(Vec2 p1, Vec2 p2) {
	return toDeg(radsBetween(p1, p2));
}

Vec2 vectorBetween(Vec2 p1, Vec2 p2) {
	float rads = radsBetween(p1, p2);

	Vec2 point = {(float)cos(rads), (float)sin(rads)};
	return point;
}

Vec4 eulerToQuaternion(Vec3 angle) {
	float yaw = angle.z * 0.5;
	float pitch = angle.y * 0.5;
	float roll = angle.x * 0.5;

	float cy = cos(yaw);
	float sy = sin(yaw);
	float cp = cos(pitch);
	float sp = sin(pitch);
	float cr = cos(roll);
	float sr = sin(roll);

	Vec4 quat;
	quat.w = cy*cp*cr + sy*sp*sr;
	quat.x = cy*cp*sr - sy*sp*cr;
	quat.y = sy*cp*sr + cy*sp*cr;
	quat.z = sy*cp*cr - cy*sp*sr;

	return quat;
}

Vec3 quaternionToEuler(Vec4 quat) {
	float yaw;
	float pitch;
	float roll;

	// roll (x-axis rotation)
	double sinr_cosp = 2.0 * (quat.w * quat.x + quat.y * quat.z);
	double cosr_cosp = 1.0 - 2.0 * (quat.x * quat.x + quat.y * quat.y);
	roll = atan2(sinr_cosp, cosr_cosp);

	// pitch (y-axis rotation)
	double sinp = 2.0 * (quat.w * quat.y - quat.z * quat.x);
	if (fabs(sinp) >= 1) {
		if (sinp < 0) pitch = -M_PI/2;
		else pitch = M_PI/2;
	} else {
		pitch = asin(sinp);
	}

	// yaw (z-axis rotation)
	double siny_cosp = 2.0 * (quat.w * quat.z + quat.x * quat.y);
	double cosy_cosp = 1.0 - 2.0 * (quat.y * quat.y + quat.z * quat.z);  
	yaw = atan2(siny_cosp, cosy_cosp);

	Vec3 ret = {roll, pitch, yaw};
	return ret;
}

Vec4 Vec4::normalize() {
	Vec4 ret = {0, 0, 0, 1};
	float norm = sqrt(this->x*this->x + this->y*this->y + this->z*this->z + this->w*this->w);
	if (norm != 0) {
		ret.x = this->x / norm;
		ret.y = this->y / norm;
		ret.z = this->z / norm;
		ret.w = this->w / norm;
	}

	return ret;
}

Vec4 Vec4::negate() {
	Vec4 ret = {-this->x, -this->y, -this->z, -this->w};
	return ret;
}

Vec4 multiplyQuaternions(Vec4 q1, Vec4 q2) {
#if 0
	Vec4 m = q1;
	Vec4 a = q2;

	Vec3 v = v3(m.x, m.y, m.z);
	Vec3 av = v3(a.x, a.y, a.z);

	Vec4 r;
	r.w = m.w * a.w - v.dot(av);

	Vec3 rv = v.cross(av);
	av = av * m.w;
	v = v * a.w;

	rv = rv + av + v;

	r.x = rv.x;
	r.y = rv.y;
	r.z = rv.z;
	return r;
#else
	Vec4 ret;
	ret.x =  q1.x * q2.w + q1.y * q2.z - q1.z * q2.y + q1.w * q2.x;
	ret.y = -q1.x * q2.z + q1.y * q2.w + q1.z * q2.x + q1.w * q2.y;
	ret.z =  q1.x * q2.y - q1.y * q2.x + q1.z * q2.w + q1.w * q2.z;
	ret.w = -q1.x * q2.x - q1.y * q2.y - q1.z * q2.z + q1.w * q2.w;
	if (ret.w < 0) ret = ret.negate();
	return ret;
#endif
}

Vec4 conjugateQuaternion(Vec4 quat) {
	Vec4 ret;
	ret.x = -quat.x;
	ret.y = -quat.y;
	ret.z = -quat.z;
	ret.w = quat.w;
	return ret;
}

Vec4 angleAxisToQuaternion(float rads, Vec3 axis) {
	Vec4 ret;
	ret.x = axis.x * sin(rads/2);
	ret.y = axis.y * sin(rads/2);
	ret.z = axis.z * sin(rads/2);
	ret.w = cos(rads/2);
	return ret;
}

void quaternionAngleAxis(Vec4 quat, float *angle, Vec3 *axis) {
	*angle = 2 * acos(quat.w);

	float sRoot = sqrt(1-quat.w*quat.w);
	if (sRoot > 0.001) {
		axis->x = quat.x / sRoot;
		axis->y = quat.y / sRoot;
		axis->z = quat.z / sRoot;
	} else {
		axis->x = 0;
		axis->y = 1;
		axis->z = 0;
	}
}

Vec4 quaternionMultiply(Vec4 q1, Vec4 q2) {
	Vec4 m = q1;
	Vec4 a = q2;

	Vec3 v = v3(m.x, m.y, m.z);
	Vec3 av = v3(a.x, a.y, a.z);

	Vec4 result;
	result.w = m.w * a.w - v.dot(av);

	Vec3 rv = v.cross(av);

	av = av.multiply(m.w);
	v = v.multiply(a.w);

	rv = rv.add(av).add(v);

	result.x = rv.x;
	result.y = rv.y;
	result.z = rv.z;

	return result;
}

Vec4 slerpQuaternions(Vec4 q1, Vec4 q2, float perc) {
#if 1
	float t = perc;
	// quaternion to return
	Vec4 qm = v4(0, 0, 0, 1);
	// Calculate angle between them.
	double cosHalfTheta = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;
	// if q1=q2 or q1=-q2 then theta = 0 and we can return q1
	if (abs(cosHalfTheta) >= 1.0){
		qm.w = q1.w;
		qm.x = q1.x;
		qm.y = q1.y;
		qm.z = q1.z;
		return qm;
	}
	// Calculate temporary values.
	double halfTheta = acos(cosHalfTheta);
	double sinHalfTheta = sqrt(1.0 - cosHalfTheta*cosHalfTheta);
	// if theta = 180 degrees then result is not fully defined
	// we could rotate around any axis normal to q1 or q2
	if (fabs(sinHalfTheta) < 0.001){ // fabs is floating point absolute
		qm.w = (q1.w * 0.5 + q2.w * 0.5);
		qm.x = (q1.x * 0.5 + q2.x * 0.5);
		qm.y = (q1.y * 0.5 + q2.y * 0.5);
		qm.z = (q1.z * 0.5 + q2.z * 0.5);
		return qm;
	}
	double ratioA = sin((1 - t) * halfTheta) / sinHalfTheta;
	double ratioB = sin(t * halfTheta) / sinHalfTheta; 
	//calculate Quaternion.
	qm.w = (q1.w * ratioA + q2.w * ratioB);
	qm.x = (q1.x * ratioA + q2.x * ratioB);
	qm.y = (q1.y * ratioA + q2.y * ratioB);
	qm.z = (q1.z * ratioA + q2.z * ratioB);
	return qm;
#else
	float dotProduct = q1.dot(q2);

	if (dotProduct < 0) {
		q2 = v4(-q2.x, -q2.y, -q2.z, -q2.w);
		dotProduct = -dotProduct;
	}

	if (dotProduct > 0.9995) {
		float percOf1 = 1 - perc;
		float percOf2 = perc;
		Vec4 result;
		result.x = q1.x * percOf1 + q2.x * percOf2;
		result.y = q1.y * percOf1 + q2.y * percOf2;
		result.z = q1.z * percOf1 + q2.z * percOf2;
		result.w = q1.w * percOf1 + q2.w * percOf2;
		result = result.normalize();
		return result;
	}

	perc = perc/2.0;

	float theta = (float) acos(dotProduct);
	if (theta < 0.0) theta = -theta;

	float st = (float)sin(theta);
	float sut = (float)sin(perc*theta);
	float sout = (float)sin((1-perc)*theta);
	float coeff1 = sout/st;
	float coeff2 = sut/st;

	Vec4 qr;
	qr.x = coeff1*q1.x + coeff2*q2.x;
	qr.y = coeff1*q1.y + coeff2*q2.y;
	qr.z = coeff1*q1.z + coeff2*q2.z;
	qr.w = coeff1*q1.w + coeff2*q2.w;

	qr = qr.normalize();
	return qr;
#endif
}

Vec4 nlerpQuaternions(Vec4 q1, Vec4 q2, float perc) {
	if (q2.dot(q1) < 0) q1 = q1.negate();

	Vec4 result = lerp(q1, q2, perc);
	result = result.normalize();
	return result;
}

Vec4 quaternionBetween(Vec3 start, Vec3 end, Vec3 axis) {
	Vec3 rotPoint1 = axis;
	Vec3 rotPoint2 = end.subtract(start).normalize();
	Vec4 quat = {0, 0, 0, 1};
	float dot = rotPoint1.dot(rotPoint2);
	if (fabs(dot - (-1.0)) < 0.000001f) {
		quat = angleAxisToQuaternion(toRad(180), axis);
	} else if (fabs(dot - (1.0f)) < 0.000001f) {
		quat = v4(0, 0, 0, 1);
	} else {
		float rotAngle = acos(dot);
		Vec3 rotAxis = rotPoint2.cross(rotPoint1);
		rotAxis = rotAxis.normalize();
		quat = angleAxisToQuaternion(-rotAngle, rotAxis);
		quat = quat.normalize();
	}

	return quat;
}

Rect insert(Rect rect, Vec2 point) {
	if (isZero(rect)) {
		rect.x = point.x;
		rect.y = point.y;
		return rect;
	}

	if (rect.x > point.x) {
		float dist = rect.x - point.x;
		rect.x -= dist;
		rect.width += dist;
	}

	if (rect.y > point.y) {
		float dist = rect.y - point.y;
		rect.y -= dist;
		rect.height += dist;
	}

	if (rect.x + rect.width < point.x) {
		float dist = point.x - (rect.x + rect.width);
		rect.width += dist;
	}

	if (rect.y + rect.height < point.y) {
		float dist = point.y - (rect.y + rect.height);
		rect.height += dist;
	}

	return rect;
}

Rect insert(Rect rect, Rect other) { //@speed This could probably be faster
	Vec2 verts[4] = {
		v2(other.x, other.y),
		v2(other.x+other.width, other.y),
		v2(other.x, other.y+other.height),
		v2(other.x+other.width, other.y+other.height),
	};

	for (int i = 0; i < 4; i++) {
		rect = insert(rect, verts[i]);
	}

	return rect;
}

bool isZero(Rect rect) {
	return rect.x < 0.0001 && rect.y < 0.0001 && rect.width < 0.0001 && rect.height < 0.0001 &&
		rect.x > -0.0001 && rect.y > -0.0001 && rect.width > -0.0001 && rect.height > -0.0001;
}

void toLines(Rect rect, Line2 *lines);
void toLines(Rect rect, Line2 *lines) {
	lines[0].start = getPosition(rect);
	lines[0].end = getPosition(rect) + v2(0, rect.height);

	lines[1].start = getPosition(rect) + v2(0, rect.height);
	lines[1].end = getPosition(rect) + v2(rect.width, rect.height);

	lines[2].start = getPosition(rect) + v2(rect.width, rect.height);
	lines[2].end = getPosition(rect) + v2(rect.width, 0);

	lines[3].start = getPosition(rect) + v2(rect.width, 0);
	lines[3].end = getPosition(rect);
}

Rect operator* (Matrix3 matrix, Rect rect) {
	Vec2 verts[4] = {
		v2(rect.x, rect.y),
		v2(rect.x+rect.width, rect.y),
		v2(rect.x, rect.y+rect.height),
		v2(rect.x+rect.width, rect.y+rect.height),
	};

	Rect ret = {};
	for (int i = 0; i < 4; i++) {
		verts[i] = matrix * verts[i];
		ret = insert(ret, verts[i]);
	}

	return ret;
}


void findCofactor(float mat[4][4], float temp[4][4], int p, int q, int n) {
	int i = 0, j = 0;

	// Looping for each element of the matrix
	for (int row = 0; row < n; row++) {
		for (int col = 0; col < n; col++) {
			//  Copying into temporary matrix only those element
			//  which are not in given row and column
			if (row != p && col != q) {
				temp[i][j++] = mat[row][col];

				// Row is filled, so increase row index and
				// reset col index
				if (j == n - 1) {
					j = 0;
					i++;
				}
			}
		}
	}
}

float findDeterminant(float mat[4][4], int n) {
	float D = 0; // Initialize result

	//  Base case : if matrix contains single element
	if (n == 1)
		return mat[0][0];

	float temp[4][4] = {}; // To store cofactors

	int sign = 1;  // To store sign multiplier

	// Iterate for each element of first row
	for (int f = 0; f < n; f++) {
		// Getting Cofactor of mat[0][f]
		findCofactor(mat, temp, 0, f, n);
		D += sign * mat[0][f] * findDeterminant(temp, n - 1);

		// terms are to be added with alternate sign
		sign = -sign;
	}

	return D;
}

void findAdjoint(float A[4][4], float adj[4][4]) {
	// temp is used to store cofactors of A[][]
	int sign = 1;
	float temp[4][4];

	for (int i=0; i<4; i++) {
		for (int j=0; j<4; j++) {
			// Get cofactor of A[i][j]
			findCofactor(A, temp, i, j, 4);

			// sign of adj[j][i] positive if sum of row
			// and column indexes is even.
			sign = ((i+j)%2==0)? 1: -1;

			// Interchanging rows and columns to get the
			// transpose of the cofactor matrix
			adj[j][i] = (sign)*(findDeterminant(temp, 4-1));
		}
	}
}

bool findInverse(float A[4][4], float inverse[4][4]) {
	// Find determinant of A[][]
	float det = findDeterminant(A, 4);
	if (det == 0) {
		// logf("Singular matrix, can't find its inverse\n");
		return false;
	}

	// Find adjoint
	float adj[4][4];
	findAdjoint(A, adj);

	// Find Inverse using formula "inverse(A) = adj(A)/det(A)"
	for (int i=0; i<4; i++)
		for (int j=0; j<4; j++)
			inverse[i][j] = adj[i][j]/float(det);

	return true;
}

float triangleSign(Vec2 p1, Vec2 p2, Vec2 p3) {
	return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool pointInTriangle(Vec2 *tri, Vec2 point) {
	return pointInTriangle(point, tri[0], tri[1], tri[2]);
}

bool pointInTriangle(Vec2 pt, Vec2 v1, Vec2 v2, Vec2 v3) {
	float d1 = triangleSign(pt, v1, v2);
	float d2 = triangleSign(pt, v2, v3);
	float d3 = triangleSign(pt, v3, v1);

	bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	return !(has_neg && has_pos);
}

float barryCentric(Vec3 p1, Vec3 p2, Vec3 p3, Vec2 pos) {
	float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
	float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
	float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
	float l3 = 1.0f - l1 - l2;
	return l1 * p1.y + l2 * p2.y + l3 * p3.y;
}

float lerpDegrees(float start, float end, float perc) {
	float difference = fabs(end - start);
	if (difference > 180) {
		if (end > start) start += 360;
		else end += 360;
	}

	float value = (start + ((end - start) * perc));

	if (value >= 0 && value <= 360) return value;

	return ((int)value % 360);
}

float timePhase(float time, float freq) {
	return (sin(time*freq*M_PI*2-M_PI*0.5)/2)+0.5;
}

float lerp(float min, float max, float perc) {
	return min + (max - min) * perc;
}

Vec2 lerp(Vec2 min, Vec2 max, float perc) {
	Vec2 point = {};
	point.x = lerp(min.x, max.x, perc);
	point.y = lerp(min.y, max.y, perc);
	return point;
}

Vec3 lerp(Vec3 min, Vec3 max, float perc) {
	Vec3 point = {};
	point.x = lerp(min.x, max.x, perc);
	point.y = lerp(min.y, max.y, perc);
	point.z = lerp(min.z, max.z, perc);
	return point;
}

Vec4 lerp(Vec4 min, Vec4 max, float perc) {
	Vec4 point = {};
	point.x = lerp(min.x, max.x, perc);
	point.y = lerp(min.y, max.y, perc);
	point.z = lerp(min.z, max.z, perc);
	point.w = lerp(min.w, max.w, perc);
	return point;
}

Matrix4 lerp(Matrix4 min, Matrix4 max, float perc) {
	min = min.multiply(1 - perc);
	max = max.multiply(perc);
	Matrix4 ret = min.add(max);
	return ret;
}

Matrix3 lerp(Matrix3 min, Matrix3 max, float perc) {
	min = min.multiply(1 - perc);
	max = max.multiply(perc);
	Matrix3 ret = min.add(max);
	return ret;
}

Xform lerp(Xform min, Xform max, float perc) {
	Xform result;

	result.translation = lerp(min.translation, max.translation, perc);

#if 0
	result.rotation = slerpQuaternions(min.rotation, max.rotation, perc); // This creates weird discontinues
#else
	result.rotation = nlerpQuaternions(min.rotation, max.rotation, perc);
#endif

	result.scale = lerp(min.scale, max.scale, perc);

	return result;
}

float lerpRad(float min, float max, float perc) {
	float CS = (1-perc)*cos(min) + perc*cos(max);
	float SN = (1-perc)*sin(min) + perc*sin(max);
	float ret = atan2(SN,CS);
	return ret;
}

float lerpDeg(float min, float max, float perc) {
#if 1
	min = toRad(min);
	max = toRad(max);
	float CS = (1-perc)*cos(min) + perc*cos(max);
	float SN = (1-perc)*sin(min) + perc*sin(max);
	float ret = atan2(SN,CS);
	return toDeg(ret);
#else // Should be the following, but untested
	return toDeg(lerpRad(toRad(min), toRad(max), perc));
#endif
}

Vec2 lerpVec2(Vec2 min, Vec2 max, float perc) {
	return lerp(min, max, perc);
}

Vec3 lerpVec3(Vec3 min, Vec3 max, float perc) {
	Vec3 point = {};
	point.x = lerp(min.x, max.x, perc);
	point.y = lerp(min.y, max.y, perc);
	point.z = lerp(min.z, max.z, perc);
	return point;
}

int lerpColor(int color1, int color2, float perc) {
	unsigned char a1 = (color1 >> 24) & 0xFF;
	unsigned char r1 = (color1 >> 16) & 0xFF;
	unsigned char g1 = (color1 >> 8) & 0xFF;
	unsigned char b1 = (color1     ) & 0xFF;

	unsigned char a2 = (color2 >> 24) & 0xFF;
	unsigned char r2 = (color2 >> 16) & 0xFF;
	unsigned char g2 = (color2 >> 8) & 0xFF;
	unsigned char b2 = (color2     ) & 0xFF;

	perc = Clamp01(perc);

	int ret = argbToHex(
		lerp(a1, a2, perc),
		lerp(r1, r2, perc),
		lerp(g1, g2, perc),
		lerp(b1, b2, perc)
	);

	if (ret == color1 && ret != color2 && perc > 0) ret = lerpColor(color1, color2, (perc*2)+0.001);

	return ret;
}

float cubicBezier_getBezierPoint(float perc, float n1, float n2);
float cubicBezier_getBezierPoint(float perc, float n1, float n2) {
	float diff = n2 - n1;
	return n1 + (diff * perc);
}    

float cubicBezier(Vec4 curve, float t) { return cubicBezier(curve.x, curve.y, curve.z, curve.w, t); }
float cubicBezier(float x2, float y2, float x3, float y3, float t) {
	t = Clamp01(t);

	float x1 = 0;
	float y1 = 0;
	float x4 = 1;
	float y4 = 1;

	// The Green Lines
	// float xa = cubicBezier_getBezierPoint(t, x1, x2);
	float ya = cubicBezier_getBezierPoint(t, y1, y2);
	// float xb = cubicBezier_getBezierPoint(t, x2, x3);
	float yb = cubicBezier_getBezierPoint(t, y2, y3);
	float xc = cubicBezier_getBezierPoint(t, x3, x4);
	float yc = cubicBezier_getBezierPoint(t, y3, y4);

	// The Blue Line
	// float xm = cubicBezier_getBezierPoint(t, xa, xb);
	float ym = cubicBezier_getBezierPoint(t, ya, yb);
	// float xn = cubicBezier_getBezierPoint(t, xb, xc);
	float yn = cubicBezier_getBezierPoint(t, yb, yc);

	// The Black Dot
	// float x = cubicBezier_getBezierPoint(t, xm, xn);
	float y = cubicBezier_getBezierPoint(t, ym, yn);

	return y;

	// Dunno what this does lol
	// float s = 1.0 - t;
	// float t2 = t*t;
	// float s2 = s*s;
	// float t3 = t2*t;
	// return (3.0 * B * s2 * t) + (3.0 * C * s * t2) + (t3);
}

float moveTowards(float value, float target, float speed) {
	if (fabs(value - target) < speed) value = target;
	else if (value < target) value += speed;
	else if (value > target) value -= speed;
	return value;
}

Vec2 moveTowards(Vec2 src, Vec2 dest, float speed) {
	if (distance(src, dest) < speed) {
		src = dest;
	} else {
		src += vectorBetween(src, dest) * speed;
	}
	return src;
}

Vec2 degToVec2(float deg) {
	Vec2 vec;
	vec.x = cos(toRad(deg));
	vec.y = sin(toRad(deg));
	return vec;
}

Vec2 radToVec2(float rad) {
	Vec2 vec;
	vec.x = cos(rad);
	vec.y = sin(rad);
	return vec;
}

float norm(float min, float max, float value) {
	return (value-min)/(max-min);
}

int wrap(int min, int max, int value) {
	if (value < min) value = max;
	else if (value > max) value = min;
	return value;
}

s16 clampedS16Add(int left, int right) {
	if (left + right > 32767) return 32767;
	if (left + right < -32768) return -32768;
	return left + right;
}

float map(float value, float sourceMin, float sourceMax, float destMin, float destMax, Ease ease) {
	float perc = norm(sourceMin, sourceMax, value);
	perc = tweenEase(perc, ease);
	return lerp(destMin, destMax, perc);
}

float clampMap(float value, float sourceMin, float sourceMax, float destMin, float destMax, Ease ease) {
	if (sourceMin == 0 && sourceMax == 0) return destMin; //@hack
	float perc = norm(sourceMin, sourceMax, value);
	perc = Clamp01(perc);
	perc = tweenEase(perc, ease);
	return lerp(destMin, destMax, perc);
}

float clampMap(float value, Vec4 sourceDest, Ease ease) {
	return clampMap(value, sourceDest.x, sourceDest.y, sourceDest.z, sourceDest.w, ease);
}


float dot(Vec2 a, Vec2 b) {
	return a.x*b.x + a.x*b.y;
}

//Vec3 getRGBFromHSV(float hue, float sat, float lum) {
//    Vec3 rbgOut = v3();

//    //Calculate Hue
//    rbgOut.x = findDist (hue, 0);
//    rbgOut.y = findDist (hue, 120);
//    rbgOut.z = findDist (hue, 240);
//    rbgOut = normalize(rbgOut) * 2;
//    rbgOut.x = min(rbgOut.x, 1);
//    rbgOut.y = min(rbgOut.y, 1);
//    rbgOut.z = min(rbgOut.z, 1);
//    logf("Hue: (%f, %f, %f)", rbgOut.x, rbgOut.y, rbgOut.z);

//    //Calculate Sat
//    float average = (rbgOut.x + rbgOut.y + rbgOut.z) / 3.0;
//    rbgOut.x = lerp(rbgOut.x, average, 1-sat);
//    rbgOut.y = lerp(rbgOut.y, average, 1-sat);
//    rbgOut.z = lerp(rbgOut.z, average, 1-sat);
//    logf("HueSat: (%f, %f, %f)", rbgOut.x, rbgOut.y, rbgOut.z);

//    //Calculate Lum
//    rbgOut.x *= lum;
//    rbgOut.y *= lum;
//    rbgOut.z *= lum;
//    logf("HueSatLum: (%f, %f, %f)", rbgOut.x, rbgOut.y, rbgOut.z);

//    return rbgOut;
//}
int pushColorAwayFromCenter(int color, float perc);
int pushColorAwayFromCenter(int color, float perc) {
	Vec4 c4 = hexToArgbFloat(color);
	float sat = c4.y + c4.z + c4.w;

	float dir = 0;
	if (sat > 3.0/2.0) {
		dir = -1;
	} else {
		dir = 1;
	}

	c4.y += dir * perc;
	c4.z += dir * perc;
	c4.w += dir * perc;

	color = argbToHex(c4);
	return color;
}

Vec3 hueShift(Vec3 color, float deg);
Vec3 hueShift(Vec3 color, float deg) {
	deg = -deg; //???
	float U = cos(deg*M_PI/180);
	float W = sin(deg*M_PI/180);

	Vec3 ret;
	ret.x = (.299+.701*U+.168*W)*color.x + (.587-.587*U+.330*W)*color.y + (.114-.114*U-.497*W)*color.z;
	ret.y = (.299-.299*U-.328*W)*color.x + (.587+.413*U+.035*W)*color.y + (.114-.114*U+.292*W)*color.z;
	ret.z = (.299-.3*U+1.25*W)*color.x + (.587-.588*U-1.05*W)*color.y + (.114+.886*U-.203*W)*color.z;
	return ret;
}

int hueShift(int color, float deg);
int hueShift(int color, float deg) {
	Vec4 c4 = hexToArgbFloat(color);
	Vec3 c3 = v3(c4.y, c4.z, c4.w);
	c3 = hueShift(c3, deg);
	c4.y = c3.x;
	c4.z = c3.y;
	c4.w = c3.z;
	color = argbToHex(c4);
	return color;
}

//findDist should return the distance from one rad angle to another
int argbToHex(unsigned char a, unsigned char r, unsigned char g, unsigned char b) {
	return ((a & 0xff) << 24) + ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

int argbToHex(Vec4 argb) {
	return argbToHex(argb.x*255.0, argb.y*255.0, argb.z*255.0, argb.w*255.0);
}

int argbFloatToHex(float a, float r, float g, float b) {
	return (((int)(a*255.0) & 0xff) << 24) + (((int)(r*255.0) & 0xff) << 16) + (((int)(g*255.0) & 0xff) << 8) + ((int)(b*255.0) & 0xff);
}

void hexToArgb(int argbHex, int *a, int *r, int *g, int *b) {
	*a = (argbHex >> 24) & 0xFF;
	*r = (argbHex >> 16) & 0xFF;
	*g = (argbHex >> 8) & 0xFF;
	*b = (argbHex     ) & 0xFF;
}

void hexToArgbFloat(int argbHex, float *a, float *r, float *g, float *b) {
	*a = ((argbHex >> 24) & 0xFF) / 255.0;
	*r = ((argbHex >> 16) & 0xFF) / 255.0;
	*g = ((argbHex >> 8) & 0xFF) / 255.0;
	*b = ((argbHex     ) & 0xFF) / 255.0;
}

Vec4 hexToArgbFloat(int argbHex) {
	int ia = (argbHex >> 24) & 0xFF;
	int ir = (argbHex >> 16) & 0xFF;
	int ig = (argbHex >> 8) & 0xFF;
	int ib = (argbHex     ) & 0xFF;

	Vec4 ret;
	ret.x = ia / 255.0;
	ret.y = ir / 255.0;
	ret.z = ig / 255.0;
	ret.w = ib / 255.0;
	return ret;
}

int argbToRgba(int argb) {
	int a, r, g, b;
	hexToArgb(argb, &a, &r, &g, &b);
	return argbToHex(r, g, b, a);
}

Vec4 argbToRgba(Vec4 argb) {
	return v4(argb.y, argb.z, argb.w, argb.x);
}

Vec4 argbToRgbaFloat(int argb) {
	int a, r, g, b;
	hexToArgb(argb, &a, &r, &g, &b);
	return v4(r/255.0, g/255.0, b/255.0, a/255.0);
}

int getAofArgb(int src) {
	int a, r, g, b;
	hexToArgb(src, &a, &r, &g, &b);
	return a;
}

int setAofArgb(int src, int newA) {
	int a, r, g, b;
	hexToArgb(src, &a, &r, &g, &b);
	a = newA;
	int ret = argbToHex(a, r, g, b);
	return ret;
}

int setBofArgb(int src, int newB) {
	int a, r, g, b;
	hexToArgb(src, &a, &r, &g, &b);
	b = newB;
	int ret = argbToHex(a, r, g, b);
	return ret;
}

int tintColor(int color, int tint) {
	int oA, oR, oG, oB;
	hexToArgb(color, &oA, &oR, &oG, &oB);
	int origAlpha = oA;

	int cA, cR, cG, cB;
	hexToArgb(tint, &cA, &cR, &cG, &cB);
	int ret = lerpColor(color, tint, (cA/255.0));
	setAofArgb(ret, origAlpha);
	return ret;
}

int alphaColor(int color, float alpha) {
	int a = getAofArgb(color);
	color = setAofArgb(color, a * alpha);
	return color;
}

int getComplement(int color) {
	int a, r, g, b;
	hexToArgb(color, &a, &r, &g, &b);
	r = 255 - r;
	g = 255 - g;
	b = 255 - b;
	color = argbToHex(a, r, g, b);
	return color;
}

// https://www.nayuki.io/res/srgb-transform-library/srgb-transform.c
#define FAST_SRGB

float srgbToLinear(float value) {
#ifdef FAST_SRGB
	return value * value;
#else
	if (value <= 0.0f) return 0.0f;
	else if (value >= 1.0f) return 1.0f;
	else if (value < 0.04045f) return value / 12.92f;
	else return powf((value + 0.055f) / 1.055f, 2.4f);
#endif
}

float linearToSrgb(float value) {
#ifdef FAST_SRGB
	return sqrt(value);
#else
	if (value <= 0.0f) return 0.0f;
	else if (value >= 1.0f) return 1.0f;
	else if (value < 0.0031308f) return value * 12.92f;
	else return powf(value, 1.0f / 2.4f) * 1.055f - 0.055f;
#endif
}

float toDeg(float rads) {
	return rads * 57.2958;
}

float toDeg(Vec2 vec) {
	return toDeg(toRad(vec));
}

float toRad(float degs) {
	return degs * 0.0174533;
}

float toRad(Vec2 vec) {
	return atan2(vec.y, vec.x);
}

void minMaxToCenterSize(Vec3 min, Vec3 max, Vec3 *center, Vec3 *size) {
	*center = (max + min) / 2;
	*size = max - min;
}

void centerSizeToMinMax(Vec3 center, Vec3 size, Vec3 *min, Vec3 *max) {
	*min = center - size/2;
	*max = center + size/2;
}

AABB makeAABB(Vec3 min, Vec3 max) {
	AABB bounds;
	bounds.min = min;
	bounds.max = max;
	return bounds;
}

AABB makeCenteredAABB(Vec3 center, Vec3 size);
AABB makeCenteredAABB(Vec3 center, Vec3 size) {
	AABB bounds;
	bounds.min = center - size/2;
	bounds.max = center + size/2;
	return bounds;
}

AABB inflate(AABB bounds, float amount) {
	bounds.min.x -= amount/2.0;
	bounds.min.y -= amount/2.0;
	bounds.min.z -= amount/2.0;
	bounds.max.x += amount/2.0;
	bounds.max.y += amount/2.0;
	bounds.max.z += amount/2.0;
	return bounds;
}

bool intersects(AABB bounds1, AABB bounds2) { 
	return (bounds1.min.x <= bounds2.max.x && bounds1.max.x >= bounds2.min.x) &&
		(bounds1.min.y <= bounds2.max.y && bounds1.max.y >= bounds2.min.y) &&
		(bounds1.min.z <= bounds2.max.z && bounds1.max.z >= bounds2.min.z);
}

Vec2 Vec2::normalize() {
	Vec2 ret = {};
	float norm = sqrt(this->x*this->x + this->y*this->y);
	if (norm != 0) {
		ret.x = this->x / norm;
		ret.y = this->y / norm;
	}

	return ret;
}

float Vec2::length() {
	return this->distance(v2(0, 0));
}

Vec2 Vec2::add(Vec2 other) {
	Vec2 ret;
	ret.x = this->x + other.x;
	ret.y = this->y + other.y;
	return ret;
}

Vec2 Vec2::subtract(Vec2 other) {
	Vec2 ret;
	ret.x = this->x - other.x;
	ret.y = this->y - other.y;
	return ret;
}

Vec2 Vec2::multiply(Vec2 other) {
	Vec2 ret;
	ret.x = this->x * other.x;
	ret.y = this->y * other.y;
	return ret;
}

Vec2 Vec2::divide(Vec2 other) {
	Vec2 ret;
	ret.x = this->x / other.x;
	ret.y = this->y / other.y;
	return ret;
}

float Vec2::dot(Vec2 other) {
	return this->x*other.x + this->y*other.y;
}

float Vec2::toRads() {
	return atan2(this->y, this->x);
}

float Vec2::toDeg() {
	return ::toDeg(this->toRads());
}

Vec2 Vec2::round() {
	Vec2 ret;
	ret.x = (int)this->x;
	ret.y = (int)this->y;
	return ret;
}

Vec2 Vec2::rotate(Vec2 center, float rads) {
	Vec2 ret;
	ret.x = cos(rads) * (this->x - center.x) - sin(rads) * (this->y - center.y) + center.x;
	ret.y = sin(rads) * (this->x - center.x) + cos(rads) * (this->y - center.y) + center.y;
	return ret;
}

Vec2 Vec2::add(float other) {
	Vec2 ret;
	ret.x = this->x + other;
	ret.y = this->y + other;
	return ret;
}

Vec2 Vec2::subtract(float other) {
	Vec2 ret;
	ret.x = this->x - other;
	ret.y = this->y - other;
	return ret;
}

Vec2 Vec2::multiply(float other) {
	Vec2 ret;
	ret.x = this->x * other;
	ret.y = this->y * other;
	return ret;
}

Vec2 Vec2::divide(float other) {
	Vec2 ret;
	ret.x = this->x / other;
	ret.y = this->y / other;
	return ret;
}

float Vec2::distance(Vec2 other) {
	return distanceBetween(this->x, this->y, other.x, other.y);
}

float Vec2::distance(float x, float y) {
	return distanceBetween(this->x, this->y, x, y);
}

bool Vec2::isZero() {
	return this->x == 0 && this->y == 0;
}

bool isZero(Vec2 point) {
	return point.x == 0 && point.y == 0;
}

bool Vec2::equal(Vec2 other) {
	if (this->distance(other) < 0.001) return true;
	return false;
}

int Vec2::getIntHash() {
	// return ((int)(this->x+256)<<16)|((int)(this->y+256)&0xFFFF);
	return this->x*this->y;

	// int x = this->x;
	// int y = this->y;
	// return ((x << 16) | (y & 0xFFFF));
}

void Vec2::print(const char *label) {
	logf("%s %f %f\n", label, this->x, this->y);
}

Vec3 Vec3::add(Vec3 point) {
	Vec3 ret;
	ret.x = this->x + point.x;
	ret.y = this->y + point.y;
	ret.z = this->z + point.z;
	return ret;
}

Vec3 Vec3::subtract(Vec3 point) {
	Vec3 ret;
	ret.x = this->x - point.x;
	ret.y = this->y - point.y;
	ret.z = this->z - point.z;
	return ret;
}

Vec3 Vec3::multiply(Vec3 point) {
	Vec3 ret;
	ret.x = this->x * point.x;
	ret.y = this->y * point.y;
	ret.z = this->z * point.z;
	return ret;
}

Vec3 Vec3::multiply(float other) {
	Vec3 ret;
	ret.x = this->x * other;
	ret.y = this->y * other;
	ret.z = this->z * other;
	return ret;
}

float Vec3::dot(Vec3 other) {
	float dot = 0;
	dot += this->x * other.x;
	dot += this->y * other.y;
	dot += this->z * other.z;
	return dot;
}

Vec3 Vec3::cross(Vec3 other) {
	Vec3 ret;
	ret.x = this->y * other.z - this->z * other.y;
	ret.y = this->z * other.x - this->x * other.z;
	ret.z = this->x * other.y - this->y * other.x;
	return ret;
}

Vec3 Vec3::normalize() {
	Vec3 ret = {};
	float norm = sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
	if (norm != 0) {
		ret.x = this->x / norm;
		ret.y = this->y / norm;
		ret.z = this->z / norm;
	}

	return ret;
}

float Vec3::distance(Vec3 other) {
	float a = other.x - this->x;
	float b = other.y - this->y;
	float c = other.z - this->z;

	float d = sqrt(a*a + b*b + c*c);
	return d;
}

float Vec3::length() {
	return this->distance(v3(0, 0, 0));
}

bool Vec3::isZero() {
	return this->x == 0 && this->y == 0 && this->z == 0;
}

void Vec3::print(const char *label) {
	logf("%s %f %f %f\n", label, this->x, this->y, this->z);
}

float Vec4::dot(Vec4 other) {
	float dot = 0;
	dot += this->x * other.x;
	dot += this->y * other.y;
	dot += this->z * other.z;
	dot += this->w * other.w;
	return dot;
}

void Vec4::print(const char *label) {
	logf("%s %f %f %f %f\n", label, this->x, this->y, this->z, this->w);
}

bool Rect::equals(Rect other) {
	return this->x == other.x && this->y == other.y && this->width == other.width && this->height == other.height;
}

bool Rect::contains(Vec2 point) { return point.x >= this->x && point.x <= this->x+this->width && point.y >= this->y && point.y <= this->y+this->height; }
bool Rect::contains(float px, float py) { return this->contains(v2(px, py)); }

bool contains(Rect rect, Vec2 point);
bool contains(Rect rect, Vec2 point) {
	return rect.contains(point);
}

bool contains(Rect rect, Rect other);
bool contains(Rect rect, Rect other) {
	return rect.x < other.x + other.width && rect.x + rect.width > other.x && rect.y < other.y + other.height && rect.y + rect.height > other.y;
}

bool overlaps(Rect rect, Rect other);
bool overlaps(Rect rect, Rect other) {
	return contains(rect, other);
}

float Rect::distanceToPerimeter(float px, float py) {
	Rect *rect = this;
	float l = rect->x;
	float t = rect->y;
	float r = rect->x + rect->width;
	float b = rect->y + rect->height;

	float x = mathClamp(px, l, r);
	float y = mathClamp(py, t, b);

	float dl = fabs(x-l);
	float dr = fabs(x-r);
	float dt = fabs(y-t);
	float db = fabs(y-b);

	float m;
	m = MinNum(dl, dr);
	m = MinNum(m, dt);
	m = MinNum(m, db);

	float minX;
	float minY;

	if (m == dt) {
		minX = x;
		minY = t;
	} else if (m == db) {
		minX = x;
		minY = b;
	} else if (m == dl) {
		minX = l;
		minY = y;
	} else {
		minX = r;
		minY = y;
	}

	return distanceBetween(minX, minY, px, py);
}

Vec2 Rect::getClosestPoint(Vec2 from) {
	if (from.x < this->x) from.x = this->x;
	else if (from.x > this->x + this->width) from.x = this->x + this->width;
	if (from.y < this->y) from.y = this->y;
	else if (from.y > this->y + this->height) from.y = this->y + this->height;
	return from;
}

Vec2 Rect::pos() {
	Vec2 ret;
	ret.x = this->x;
	ret.y = this->y;
	return ret;
}

Vec2 Rect::center() {
	Vec2 ret;
	ret.x = this->x + this->width/2;
	ret.y = this->y + this->height/2;
	return ret;
}

Vec2 Rect::size() {
	Vec2 ret;
	ret.x = this->width;
	ret.y = this->height;
	return ret;
}

Rect Rect::inflate(float size) {
	return this->inflate(v2(size, size));
}


Rect Rect::inflate(Vec2 size) {
	Rect ret = *this;
	ret.x -= size.x/2;
	ret.y -= size.y/2;
	ret.width += size.x;
	ret.height += size.y;

	return ret;
}

void Rect::print(const char *label) {
	logf("%s: {%f, %f, %f, %f}\n", label, this->x, this->y, this->width, this->height);
}

Matrix3 getProjectionMatrix(float width, float height);
Matrix3 getProjectionMatrix(float width, float height) {
#if 0
#if 1
	Matrix3 ret = {
		2.0/width,  0,          0,
		0,        -2.0/height,  0,
		-1,       1,          1
	};
#else
	Matrix3 ret = {
		2.0/width,  0,         0,
		0,        2.0/height,  0,
		-1,       -1,         -1
	};
#endif
#else
	Matrix3 ret = {
		(float)2.0/width,  0,                   0,
		0,                 (float)-2.0/height,  0,
		-1,                1,                   1
	};
#endif
	return ret;
}

float Matrix3::getRotationDeg() {
	float a = this->data[0];
	float b = this->data[3];
	float c = this->data[1];
	float d = this->data[4];

	float scaleX = sqrt((a * a) + (c * c));
	float scaleY = sqrt((b * b) + (d * d));

	float sign = atan(-c / a);
	float rad  = acos(a / scaleX);
	float deg  = toDeg(rad);

	float rotation;
	if (deg > 90 && sign > 0) {
		rotation = toRad(360.0 - deg);
	} else if (deg < 90 && sign < 0) {
		rotation = toRad(360.0 - deg);
	} else {
		rotation = rad;
	}

	return toDeg(rotation);
}

Matrix3 Matrix3::multiply(Matrix3 other) {
	Matrix3 ret;

	ret.data[0] = this->data[0] * other.data[0] + this->data[3] * other.data[1] + this->data[6] * other.data[2];
	ret.data[1] = this->data[1] * other.data[0] + this->data[4] * other.data[1] + this->data[7] * other.data[2];
	ret.data[2] = this->data[2] * other.data[0] + this->data[5] * other.data[1] + this->data[8] * other.data[2];
	ret.data[3] = this->data[0] * other.data[3] + this->data[3] * other.data[4] + this->data[6] * other.data[5];
	ret.data[4] = this->data[1] * other.data[3] + this->data[4] * other.data[4] + this->data[7] * other.data[5];
	ret.data[5] = this->data[2] * other.data[3] + this->data[5] * other.data[4] + this->data[8] * other.data[5];
	ret.data[6] = this->data[0] * other.data[6] + this->data[3] * other.data[7] + this->data[6] * other.data[8];
	ret.data[7] = this->data[1] * other.data[6] + this->data[4] * other.data[7] + this->data[7] * other.data[8];
	ret.data[8] = this->data[2] * other.data[6] + this->data[5] * other.data[7] + this->data[8] * other.data[8];

	return ret;
}

Matrix3 Matrix3::multiply(float *array) {
	Matrix3 ret;

	ret.data[0] = this->data[0] * array[0] + this->data[3] * array[1] + this->data[6] * array[2];
	ret.data[1] = this->data[1] * array[0] + this->data[4] * array[1] + this->data[7] * array[2];
	ret.data[2] = this->data[2] * array[0] + this->data[5] * array[1] + this->data[8] * array[2];
	ret.data[3] = this->data[0] * array[3] + this->data[3] * array[4] + this->data[6] * array[5];
	ret.data[4] = this->data[1] * array[3] + this->data[4] * array[4] + this->data[7] * array[5];
	ret.data[5] = this->data[2] * array[3] + this->data[5] * array[4] + this->data[8] * array[5];
	ret.data[6] = this->data[0] * array[6] + this->data[3] * array[7] + this->data[6] * array[8];
	ret.data[7] = this->data[1] * array[6] + this->data[4] * array[7] + this->data[7] * array[8];
	ret.data[8] = this->data[2] * array[6] + this->data[5] * array[7] + this->data[8] * array[8];

	return ret;
}

Vec2 Matrix3::multiply(Vec2 point) {
	Vec2 ret;
	float z = 1;
	ret.x = this->data[0]*point.x + this->data[3]*point.y + this->data[6];
	ret.y = this->data[1]*point.x + this->data[4]*point.y + this->data[7];
	z     = this->data[2]*point.x + this->data[5]*point.y + this->data[8];

	ret.x /= z;
	ret.y /= z;
	return ret;
}

Vec3 Matrix3::multiply(Vec3 point) {
	Vec3 ret;
	ret.x = this->data[0]*point.x + this->data[3]*point.y + this->data[6]*point.z;
	ret.y = this->data[1]*point.x + this->data[4]*point.y + this->data[7]*point.z;
	ret.z = this->data[2]*point.x + this->data[5]*point.y + this->data[8]*point.z;

	return ret;
}

Rect Matrix3::multiply(Rect rect) {
	Matrix3 *mat = this;

	Vec2 tl = v2(rect.x, rect.y);
	Vec2 tr = v2(rect.x + rect.width, rect.y);
	Vec2 bl = v2(rect.y, rect.y + rect.height);
	Vec2 br = v2(rect.x + rect.width, rect.y + rect.height);

	tl = mat->multiply(tl);
	tr = mat->multiply(tr);
	bl = mat->multiply(bl);
	br = mat->multiply(br);

	rect.x = tl.x;
	rect.y = tl.y;
	rect.width = tr.x - tl.x;
	rect.height = bl.y - tl.y;

	return rect;
}

Matrix3 Matrix3::multiply(float num) {
	Matrix3 ret;

	for (int i = 0; i < 9; i++) ret.data[i] = this->data[i] * num;

	return ret;
}

bool Matrix3::equal(Matrix3 other) {
	for (int i = 0; i < 9; i++) {
		if (this->data[i] != other.data[i]) return false;
	}

	return true;
}

bool Matrix3::isZero() {
	for (int i = 0; i < 9; i++) {
		if (this->data[i] != 0) return false;
	}

	return true;
}

void Matrix3::zero() {
	for (int i = 0; i < 9; i++) this->data[i] = 0;
}

Matrix3 Matrix3::invert() {
	// logf("I'm not sure if this works %s:%d\n", __FILE__, __LINE__); //@note This seems to work

	Matrix3 orig = *this;
	double det =
		orig.data[0] * (orig.data[4] * orig.data[8] - orig.data[5] * orig.data[7]) -
		orig.data[3] * (orig.data[1] * orig.data[8] - orig.data[7] * orig.data[2]) +
		orig.data[6] * (orig.data[1] * orig.data[5] - orig.data[4] * orig.data[2]);

	double invdet = 1 / det;

	Matrix3 inv;
	inv.data[0] = (orig.data[4] * orig.data[8] - orig.data[5] * orig.data[7]) * invdet;
	inv.data[3] = (orig.data[6] * orig.data[5] - orig.data[3] * orig.data[8]) * invdet;
	inv.data[6] = (orig.data[3] * orig.data[7] - orig.data[6] * orig.data[4]) * invdet;
	inv.data[1] = (orig.data[7] * orig.data[2] - orig.data[1] * orig.data[8]) * invdet;
	inv.data[4] = (orig.data[0] * orig.data[8] - orig.data[6] * orig.data[2]) * invdet;
	inv.data[7] = (orig.data[1] * orig.data[6] - orig.data[0] * orig.data[7]) * invdet;
	inv.data[2] = (orig.data[1] * orig.data[5] - orig.data[2] * orig.data[4]) * invdet;
	inv.data[5] = (orig.data[2] * orig.data[3] - orig.data[0] * orig.data[5]) * invdet;
	inv.data[8] = (orig.data[0] * orig.data[4] - orig.data[1] * orig.data[3]) * invdet;

	return inv;
}

Matrix3 Matrix3::transpose() {
	Matrix3 ret;

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			ret.data[j * 3 + i] = this->data[i * 3 + j];
		}
	}

	return ret;
}

void print(char *label, Matrix3 matrix);
void print(char *label, Matrix3 matrix) {
	logf(
		"%s\n"
		"%f\t%f\t%f\n"
		"%f\t%f\t%f\n"
		"%f\t%f\t%f\n",
		label,
		matrix.data[0], matrix.data[1], matrix.data[2],
		matrix.data[3], matrix.data[4], matrix.data[5],
		matrix.data[6], matrix.data[7], matrix.data[8]
	);
}

void Matrix3::print(const char *label) {
	logf(
		"%s\n"
		"%f\t%f\t%f\n"
		"%f\t%f\t%f\n"
		"%f\t%f\t%f\n",
		label,
		this->data[0], this->data[1], this->data[2],
		this->data[3], this->data[4], this->data[5],
		this->data[6], this->data[7], this->data[8]
	);
}

void Matrix4::setIdentity() {
	this->setTo(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);
}

void Matrix4::setTo(float *array) {
	this->data[0] = array[0];
	this->data[1] = array[1];
	this->data[2] = array[2];
	this->data[3] = array[3];
	this->data[4] = array[4];
	this->data[5] = array[5];
	this->data[6] = array[6];
	this->data[7] = array[7];
	this->data[8] = array[8];
	this->data[9] = array[9];
	this->data[10] = array[10];
	this->data[11] = array[11];
	this->data[12] = array[12];
	this->data[13] = array[13];
	this->data[14] = array[14];
	this->data[15] = array[15];
}

void Matrix4::setTo(float a, float b, float c, float d, float e, float f, float g, float h, float i, float j, float k, float l, float m, float n, float o, float p) {
	this->data[0] = a;
	this->data[1] = b;
	this->data[2] = c;
	this->data[3] = d;
	this->data[4] = e;
	this->data[5] = f;
	this->data[6] = g;
	this->data[7] = h;
	this->data[8] = i;
	this->data[9] = j;
	this->data[10] = k;
	this->data[11] = l;
	this->data[12] = m;
	this->data[13] = n;
	this->data[14] = o;
	this->data[15] = p;
}

Matrix4 Matrix4::multiply(float *array) {
	float temp[16] = {};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				temp[j + i * 4] += array[k + i * 4] * this->data[j + k * 4];
			}
		}
	}
	// ret.data[0] = this->data[0] * array[0] + this->data[3] * array[1] + this->data[6] * array[2];
	// ret.data[1] = this->data[1] * array[0] + this->data[4] * array[1] + this->data[7] * array[2];
	// ret.data[2] = this->data[2] * array[0] + this->data[5] * array[1] + this->data[8] * array[2];
	// ret.data[3] = this->data[0] * array[3] + this->data[3] * array[4] + this->data[6] * array[5];
	// ret.data[4] = this->data[1] * array[3] + this->data[4] * array[4] + this->data[7] * array[5];
	// ret.data[5] = this->data[2] * array[3] + this->data[5] * array[4] + this->data[8] * array[5];
	// ret.data[6] = this->data[0] * array[6] + this->data[3] * array[7] + this->data[6] * array[8];
	// ret.data[7] = this->data[1] * array[6] + this->data[4] * array[7] + this->data[7] * array[8];
	// ret.data[8] = this->data[2] * array[6] + this->data[5] * array[7] + this->data[8] * array[8];

	Matrix4 ret;
	ret.setTo(temp);
	return ret;
}

Matrix4 Matrix4::multiply(Matrix4 matrix) {
	return this->multiply(matrix.data);
}

Vec3 Matrix4::multiplyAffine(Vec3 point) {
	Vec3 ret;
	ret.x = this->data[0]*point.x + this->data[4]*point.y + this->data[8]*point.z;
	ret.y = this->data[1]*point.x + this->data[5]*point.y + this->data[9]*point.z;
	ret.z = this->data[2]*point.x + this->data[6]*point.y + this->data[10]*point.z;
	return ret;
}

Vec3 Matrix4::multiply(Vec3 point) {
	Vec3 ret;
	float w = 1;
	ret.x = this->data[0]*point.x + this->data[4]*point.y + this->data[8]*point.z + this->data[12];
	ret.y = this->data[1]*point.x + this->data[5]*point.y + this->data[9]*point.z + this->data[13];
	ret.z = this->data[2]*point.x + this->data[6]*point.y + this->data[10]*point.z + this->data[14];
	w     = this->data[3]*point.x + this->data[7]*point.y + this->data[11]*point.z + this->data[15];
	ret.x /= w;
	ret.y /= w;
	ret.z /= w;
	return ret;
}

Vec4 Matrix4::multiply(Vec4 point) {
	Vec4 ret;
	ret.x = this->data[0]*point.x + this->data[4]*point.y + this->data[8]*point.z + this->data[12]*point.w;
	ret.y = this->data[1]*point.x + this->data[5]*point.y + this->data[9]*point.z + this->data[13]*point.w;
	ret.z = this->data[2]*point.x + this->data[6]*point.y + this->data[10]*point.z + this->data[14]*point.w;
	ret.w = this->data[3]*point.x + this->data[7]*point.y + this->data[11]*point.z + this->data[15]*point.w;
	return ret;
}

Matrix4 Matrix4::multiply(float num) {
	Matrix4 ret;

	for (int i = 0; i < 16; i++) ret.data[i] = this->data[i] * num;

	return ret;
}

AABB Matrix4::multiply(AABB bounds) {
#if 1
	Vec3 points[8];
	getVerts(bounds, points);

	AABB ret;
	ret.min = v3(9999, 9999, 9999);
	ret.max = v3(-9999, -9999, -9999);
	for (int i = 0; i < 8; i++) {
		ret = expand(ret, this->multiply(points[i]));
	}

	return ret;
#else
	AABB ret;
	ret.min = *this * bounds.min;
	ret.max = *this * bounds.max;
	return ret
#endif

}

Matrix4 Matrix4::add(Matrix4 matrix) {
	Matrix4 ret;

	for (int i = 0; i < 16; i++) ret.data[i] = this->data[i] + matrix.data[i];

	return ret;
}

Matrix4 Matrix4::perspective(float fovYDeg, float aspect, float nearPlane, float farPlane) {
	Matrix4 persp = getPerspectiveMatrix(fovYDeg, aspect, nearPlane, farPlane);
	Matrix4 ret = *this * persp;
	return ret;
}

float horiFovToVert(float hori, float aspectRatio);
float horiFovToVert(float hori, float aspectRatio) {
	float vert = 2 * atan(tan(hori / 2.0) * aspectRatio);
	return vert;
}

Matrix4 getPerspectiveMatrix(float fovYDeg, float aspect, float nearPlane, float farPlane) {
#if 1
	float n = nearPlane;
	float f = farPlane;
	float scale = tan(fovYDeg * 0.5 * M_PI / 180) * n; 
	float r = aspect * scale;
	float l = -r; 
	float t = scale;
	float b = -t; 

	float array[16] = {
		2 * n / (r - l)   , 0                 , 0                         , 0  ,
		0                 , 2 * n / (t - b)   , 0                         , 0  ,
		(r + l) / (r - l) , (t + b) / (t - b) , -(f + n) / (f - n)        , -1 ,
		0                 , 0                 , -(2.0f * f * n) / (f - n) , 0
	};

	// float array[9] = {
	// 	2/width,  0,          0,
	// 	0,        -2/height,  0,
	// 	-1,       1,          1
	// };

	Matrix4 ret;
	ret.setTo(array);
	return ret;
#else
	float scale = 1.0 / tan(toRad(fovYDeg) * 0.5);
	float array[16] = {
		1.0f / (aspect * scale) , 0          , 0                                                       , 0  ,
		0                       , 1.0f/scale , 0                                                       , 0  ,
		0                       , 0          , -(farPlane+nearPlane) / (farPlane-nearPlane)            , -1 ,
		0                       , 0          , -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane) , 0
	};

	Matrix4 ret;
	ret.setTo(array);
	return ret;
#endif
}

Matrix4 lookAt(Vec3 from, Vec3 to);
Matrix4 lookAt(Vec3 from, Vec3 to) {
	Matrix4 mat = mat4();

#if 1
	Vec3 arb = v3(0, 0, 1);

	Vec3 forward = (from - to).normalize();
	Vec3 right = arb.cross(forward);
	Vec3 up = forward.cross(right);

	mat.data[0] = right.x;
	mat.data[1] = right.y;
	mat.data[2] = right.z;
	mat.data[4] = up.x;
	mat.data[5] = up.y;
	mat.data[6] = up.z;
	mat.data[8] = forward.x;
	mat.data[9] = forward.y;
	mat.data[10] = forward.z;

	mat.data[12] = from.x;
	mat.data[13] = from.y;
	mat.data[14] = from.z;
#else
	Vec3 zaxis = (to - from).normalize();
	Vec3 xaxis = zaxis.cross(arb).normalize();
	Vec3 yaxis = xaxis.cross(zaxis);

	zaxis *= -1;

	mat.data[0] = xaxis.x;
	mat.data[1] = yaxis.x;
	mat.data[2] = zaxis.x;
	mat.data[3] = 0;
	mat.data[4] = xaxis.y;
	mat.data[5] = yaxis.y;
	mat.data[6] = zaxis.y;
	mat.data[7] = 0;
	mat.data[8] = xaxis.z;
	mat.data[9] = yaxis.z;
	mat.data[10] = zaxis.z;
	mat.data[11] = 0;
	mat.data[12] = -xaxis.dot(from);
	mat.data[13] = -yaxis.dot(from);
	mat.data[14] = -zaxis.dot(from);
	mat.data[15] = 1;
#endif

	return mat;
}


Matrix4 Matrix4::transpose() {
	float array[16] = {};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			array[j * 4 + i] = this->data[i * 4 + j];
		}
	}

	Matrix4 ret;
	ret.setTo(array);
	return ret;
}

Matrix4 Matrix4::invert() {
	float mat[4][4];
	mat[0][0] = this->data[0];
	mat[0][1] = this->data[1];
	mat[0][2] = this->data[2];
	mat[0][3] = this->data[3];
	mat[1][0] = this->data[4];
	mat[1][1] = this->data[5];
	mat[1][2] = this->data[6];
	mat[1][3] = this->data[7];
	mat[2][0] = this->data[8];
	mat[2][1] = this->data[9];
	mat[2][2] = this->data[10];
	mat[2][3] = this->data[11];
	mat[3][0] = this->data[12];
	mat[3][1] = this->data[13];
	mat[3][2] = this->data[14];
	mat[3][3] = this->data[15];

	float inv[4][4];

	findInverse(mat, inv);

	Matrix4 ret;

	ret.data[0] = inv[0][0];
	ret.data[1] = inv[0][1];
	ret.data[2] = inv[0][2];
	ret.data[3] = inv[0][3];
	ret.data[4] = inv[1][0];
	ret.data[5] = inv[1][1];
	ret.data[6] = inv[1][2];
	ret.data[7] = inv[1][3];
	ret.data[8] = inv[2][0];
	ret.data[9] = inv[2][1];
	ret.data[10] = inv[2][2];
	ret.data[11] = inv[2][3];
	ret.data[12] = inv[3][0];
	ret.data[13] = inv[3][1];
	ret.data[14] = inv[3][2];
	ret.data[15] = inv[3][3];

	return ret;
}

Vec3 Matrix4::getPosition() {
	Matrix4 *mat = this;

	Vec3 ret;
	ret.x = mat->data[12];
	ret.y = mat->data[13];
	ret.z = mat->data[14];
	return ret;
}

Vec4 Matrix4::getQuaternion() {
	Matrix4 *mat = this;

	Matrix4 newMat = *this;

	Vec3 s = mat->getScale();
	if (s.x == 0 || s.y == 0 || s.z == 0) {
		logf("WARNING: Xform has scale of 0\n");
	}
	newMat.SCALE(1.0/s.x, 1.0/s.y, 1.0/s.z);

	mat = &newMat;

	// 0    1    2    3
	// 4    5    6    7
	// 8    9    10   11
	// 12   13   14   15

	// 00 10 20 30
	// 01 11 21 31
	// 02 12 22 32
	// 03 13 23 33

	Vec4 ret = {};
#if 0
	float m00 = mat->data[0];
	float m10 = mat->data[1];
	float m20 = mat->data[2];
	float m01 = mat->data[4];
	float m11 = mat->data[5];
	float m21 = mat->data[6];
	float m02 = mat->data[8];
	float m12 = mat->data[9];
	float m22 = mat->data[10];

	float tr = m00 + m11 + m22;

	if (tr > 0) { 
		float S = sqrt(tr+1.0) * 2; // S=4*qw 
		ret.w = 0.25 * S;
		ret.x = (m21 - m12) / S;
		ret.y = (m02 - m20) / S; 
		ret.z = (m10 - m01) / S; 
	} else if ((m00 > m11)&&(m00 > m22)) { 
		float S = sqrt(1.0 + m00 - m11 - m22) * 2; // S=4*qx 
		ret.w = (m21 - m12) / S;
		ret.x = 0.25 * S;
		ret.y = (m01 + m10) / S; 
		ret.z = (m02 + m20) / S; 
	} else if (m11 > m22) { 
		float S = sqrt(1.0 + m11 - m00 - m22) * 2; // S=4*qy
		ret.w = (m02 - m20) / S;
		ret.x = (m01 + m10) / S; 
		ret.y = 0.25 * S;
		ret.z = (m12 + m21) / S; 
	} else { 
		float S = sqrt(1.0 + m22 - m00 - m11) * 2; // S=4*qz
		ret.w = (m10 - m01) / S;
		ret.x = (m02 + m20) / S;
		ret.y = (m12 + m21) / S;
		ret.z = 0.25 * S;
	}
#else
	// 0    1    2    3
	// 4    5    6    7
	// 8    9    10   11
	// 12   13   14   15

	// 00 10 20 30
	// 01 11 21 31
	// 02 12 22 32
	// 03 13 23 33
	Vec4 q = {};
	float *a = mat->data;
  float trace = a[0] + a[5] + a[10]; // I removed + 1.0f; see discussion with Ethan
  if( trace > 0 ) {// I changed M_EPSILON to 0
    float s = 0.5f / sqrtf(trace+ 1.0f);
    q.w = 0.25f / s;
    q.x = ( a[6] - a[9] ) * s;
    q.y = ( a[8] - a[2] ) * s;
    q.z = ( a[1] - a[4] ) * s;
  } else {
    if ( a[0] > a[5] && a[0] > a[10] ) {
      float s = 2.0f * sqrtf( 1.0f + a[0] - a[5] - a[10]);
      q.w = (a[6] - a[9] ) / s;
      q.x = 0.25f * s;
      q.y = (a[4] + a[1] ) / s;
      q.z = (a[8] + a[2] ) / s;
    } else if (a[5] > a[10]) {
      float s = 2.0f * sqrtf( 1.0f + a[5] - a[0] - a[10]);
      q.w = (a[8] - a[2] ) / s;
      q.x = (a[4] + a[1] ) / s;
      q.y = 0.25f * s;
      q.z = (a[9] + a[6] ) / s;
    } else {
      float s = 2.0f * sqrtf( 1.0f + a[10] - a[0] - a[5] );
      q.w = (a[1] - a[4] ) / s;
      q.x = (a[8] + a[2] ) / s;
      q.y = (a[9] + a[6] ) / s;
      q.z = 0.25f * s;
    }
  }
	ret = q;
#endif
	// if (!ret.equals(ret.normalize())) logf("Norm error\n");
	if (ret.w < 0) ret = ret.negate();
	return ret;
}

Vec3 Matrix4::getScale() {
	Matrix4 *mat = this;

	Vec3 vec0 = v3(mat->data[0], mat->data[4], mat->data[8]);
	Vec3 vec1 = v3(mat->data[1], mat->data[5], mat->data[9]);
	Vec3 vec2 = v3(mat->data[2], mat->data[6], mat->data[10]);

	Vec3 ret;
	ret.x = vec0.length();
	ret.y = vec1.length();
	ret.z = vec2.length();
	return ret;
}

void Matrix4::print(const char *label) {
	logf(
		"%s\n"
		"%f\t%f\t%f\t%f\n"
		"%f\t%f\t%f\t%f\n"
		"%f\t%f\t%f\t%f\n"
		"%f\t%f\t%f\t%f\n",
		label,
		this->data[0], this->data[1], this->data[2], this->data[3],
		this->data[4], this->data[5], this->data[6], this->data[7],
		this->data[8], this->data[9], this->data[10], this->data[11],
		this->data[12], this->data[13], this->data[14], this->data[15]
	);
}

float tweenEase(float p, Ease ease) {
	float piOver2 = 3.14159/2;
	if (ease == LINEAR) {
		return p;
	} else if (ease == QUAD_IN) {
		return p * p;
	} else if (ease == QUAD_OUT) {
		return -(p * (p - 2));
	} else if (ease == QUAD_IN_OUT) {
		if (p < 0.5) return 2 * p * p;
		else return (-2 * p * p) + (4 * p) - 1;
	} else if (ease == CUBIC_IN) {
		return p * p * p;
	} else if (ease == CUBIC_OUT) {
		float f = (p - 1);
		return f * f * f + 1;
	} else if (ease == CUBIC_IN_OUT) {
		float f = ((2 * p) - 2);
		if (p < 0.5) return 4 * p * p * p;
		else return 0.5 * f * f * f + 1;
	} else if (ease == QUART_IN) {
		return p * p * p * p;
	} else if (ease == QUART_OUT) {
		float f = (p - 1);
		return f * f * f * (1 - p) + 1;
	} else if (ease == QUART_IN_OUT) {
		float f = (p - 1);
		if (p < 0.5) return 8 * p * p * p * p;
		else return -8 * f * f * f * f + 1;
	} else if (ease == QUINT_IN) {
		return p * p * p * p * p;
	} else if (ease == QUINT_OUT) {
		float f = (p - 1);
		return f * f * f * f * f + 1;
	} else if (ease == QUINT_IN_OUT) {
		float f = ((2 * p) - 2);
		if (p < 0.5) return 16 * p * p * p * p * p;
		else return  0.5 * f * f * f * f * f + 1;
	} else if (ease == SINE_IN) {
		return sin((p - 1) * piOver2) + 1;
	} else if (ease == SINE_OUT) {
		return sin(p * piOver2);
	} else if (ease == SINE_IN_OUT) {
		return 0.5 * (1 - cos(p * M_PI));
	} else if (ease == CIRC_IN) {
		return 1 - sqrt(1 - (p * p));
	} else if (ease == CIRC_OUT) {
		return sqrt((2 - p) * p);
	} else if (ease == CIRC_IN_OUT) {
		if (p < 0.5) return 0.5 * (1 - sqrt(1 - 4 * (p * p)));
		else return 0.5 * (sqrt(-((2 * p) - 3) * ((2 * p) - 1)) + 1);
	} else if (ease == EXP_IN) {
		return (p == 0.0) ? p : pow(2, 10 * (p - 1));
	} else if (ease == EXP_OUT) {
		return (p == 1.0) ? p : 1 - pow(2, -10 * p);
	} else if (ease == EXP_IN_OUT) {
		if (p == 0.0 || p == 1.0) return p;
		if (p < 0.5) return 0.5 * pow(2, (20 * p) - 10);
		else return -0.5 * pow(2, (-20 * p) + 10) + 1;
	} else if (ease == ELASTIC_IN) {
		return sin(13 * piOver2 * p) * pow(2, 10 * (p - 1));
	} else if (ease == ELASTIC_OUT) {
		return sin(-13 * piOver2 * (p + 1)) * pow(2, -10 * p) + 1;
	} else if (ease == ELASTIC_IN_OUT) {
		if (p < 0.5) return 0.5 * sin(13 * piOver2 * (2 * p)) * pow(2, 10 * ((2 * p) - 1));
		else return 0.5 * (sin(-13 * piOver2 * ((2 * p - 1) + 1)) * pow(2, -10 * (2 * p - 1)) + 2);
	} else if (ease == BACK_IN) {
		return p * p * p - p * sin(p * M_PI);
	} else if (ease == BACK_OUT) {
		float f = (1 - p);
		return 1 - (f * f * f - f * sin(f * M_PI));
	} else if (ease == BACK_IN_OUT) {
		if (p < 0.5) {
			float f = 2 * p;
			return 0.5 * (f * f * f - f * sin(f * M_PI));
		} else {
			float f = (1 - (2*p - 1));
			return 0.5 * (1 - (f * f * f - f * sin(f * M_PI))) + 0.5;
		}
	} else if (ease == BOUNCE_IN) {
		return 1 - tweenEase(1 - p, BOUNCE_OUT);
	} else if (ease == BOUNCE_OUT) {
		if (p < 4/11.0) return (121 * p * p)/16.0;
		else if (p < 8/11.0) return (363/40.0 * p * p) - (99/10.0 * p) + 17/5.0;
		else if (p < 9/10.0) return (4356/361.0 * p * p) - (35442/1805.0 * p) + 16061/1805.0;
		else return (54/5.0 * p * p) - (513/25.0 * p) + 268/25.0;
	} else if (ease == BOUNCE_IN_OUT) {
		if (p < 0.5) return 0.5 * tweenEase(p*2, BOUNCE_IN);
		else return 0.5 * tweenEase(p * 2 - 1, BOUNCE_OUT) + 0.5;
	}

	return 0;
}

double triangleDet2d(Vec2 *p1, Vec2 *p2, Vec2 *p3) {
	return p1->x * (p2->y - p3->y)
		+ p2->x * (p3->y - p1->y)
		+ p3->x * (p1->y - p2->y);
}

void fix2dTriWinding(Vec2 *p1, Vec2 *p2, Vec2 *p3) {
	double detTri = triangleDet2d(p1, p2, p3);
	if (detTri < 0.0) {
		double t = p3->x;
		p3->x = p2->x;
		p2->x = t;

		t = p3->y;
		p3->y = p2->y;
		p2->y = t;
	}
}

int rayCount = 0;
bool rayIntersectsTriangle(Vec3 orig, Vec3 dir, Tri tri, float *ret, Vec2 *uv) {
	rayCount++;
	// if (rayCount % 10000 == 0) logf("%d\n", rayCount);

	*ret = 0;
#if 1
	Vec3 v0v1 = tri.verts[1] - tri.verts[0];
	Vec3 v0v2 = tri.verts[2] - tri.verts[0]; 
	Vec3 pvec = dir.cross(v0v2); 
	float det = v0v1.dot(pvec); 
	if (fabs(det) < 0.0001) return false; 
	float invDet = 1 / det; 

	Vec3 tvec = orig - tri.verts[0]; 
	float u = tvec.dot(pvec) * invDet; 
	if (u < 0 || u > 1) return false; 

	Vec3 qvec = tvec.cross(v0v1); 
	float v = dir.dot(qvec) * invDet; 
	if (v < 0 || u + v > 1) return false; 

	float t = v0v2.dot(qvec) * invDet; 

	*ret = t;
	*uv = v2(u, v);
	return true; 
#endif

#if 0
	// compute plane's normal
    Vec3 v0v1 = v1 - v0; 
    Vec3 v0v2 = v2 - v0; 
    // no need to normalize
    Vec3 N = v0v1.cross(v0v2); // N 
    float area2 = N.length(); 
 
    // Step 1: finding P
 
    // check if ray and plane are parallel ?
    float NdotRayDirection = N.dot(dir); 
    if (fabs(NdotRayDirection) < 0.001) return false; // they are parallel so they don't intersect ! 
 
    // compute d parameter using equation 2
    float d = N.dot(v0); 
 
    // compute t (equation 3)
    float t = (N.dot(orig) + d) / NdotRayDirection; 
    // check if the triangle is in behind the ray
    if (t < 0) return false; // the triangle is behind 
 
    // compute the intersection point using equation 1
    Vec3 P = orig + t * dir; 
 
    // Step 2: inside-outside test
    Vec3 C; // vector perpendicular to triangle's plane 
 
    // edge 0
    Vec3 edge0 = v1 - v0; 
    Vec3 vp0 = P - v0; 
    C = edge0.cross(vp0); 
    if (N.dot(C) < 0) return false; // P is on the right side 
 
    // edge 1
    Vec3 edge1 = v2 - v1; 
    Vec3 vp1 = P - v1; 
    C = edge1.cross(vp1); 
    if (N.dot(C) < 0)  return false; // P is on the right side 
 
    // edge 2
    Vec3 edge2 = v0 - v2; 
    Vec3 vp2 = P - v2; 
    C = edge2.cross(vp2); 
    if (N.dot(C) < 0) return false; // P is on the right side; 
 
		*ret = t;
    return true; // this ray hits the triangle 
#endif

#if 0
	Vec3 e1 = v1 - v0;
	Vec3 e2 = v2 - v0;

	Vec3 h = dir.cross(e2);
	float a = e1.dot(h);

	if (a > -0.00001 && a < 0.00001) return false;

	float f = 1.0/a;
	Vec3 s = orig - v0;
	float u = f * (s.dot(h));

	if (u < 0.0 || u > 1.0) return false;

	Vec3 q = s.cross(e1);
	float v = f * dir.dot(q);

	if (v < 0.0 || u + v > 1.0) return false;

	// at this stage we can compute t to find out where
	// the intersection point is on the line
	float t = f * e2.dot(q);

	if (t > 0.00001) {// ray intersection
		*ret = t;
		return true;
	} else {
		// this means that there is a line intersection
		// but not a ray intersection
		return false;
	}
#endif
}

bool overlaps(AABB bounds, Line3 line, Vec3 *hitOut) {
	// returns true if line (L1, L2) intersects with the bounds (B1, B2)
	// returns intersection point in Hit

	Vec3 dir = (line.end - line.start).normalize();
	float t1 = (bounds.min.x - line.start.x) / dir.x;
	float t2 = (bounds.max.x - line.start.x) / dir.x;
	float t3 = (bounds.min.y - line.start.y) / dir.y;
	float t4 = (bounds.max.y - line.start.y) / dir.y;
	float t5 = (bounds.min.z - line.start.z) / dir.z;
	float t6 = (bounds.max.z - line.start.z) / dir.z;

	float tmin = MaxNum(MaxNum(MinNum(t1, t2), MinNum(t3, t4)), MinNum(t5, t6));
	float tmax = MinNum(MinNum(MaxNum(t1, t2), MaxNum(t3, t4)), MaxNum(t5, t6));

	// if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
	if (tmax < 0) return false;

	// if tmin > tmax, ray doesn't intersect AABB
	if (tmin > tmax) return false;

	float dist = line.start.distance(line.end);
	if (tmin < 0) {
		if (tmax > dist) return false;
		if (hitOut) *hitOut = line.start + dir * tmax;
		return true;
	}

	if (tmin > dist) return false;
	if (hitOut) *hitOut = line.start + dir * tmin;
	return true;
}

float distance(AABB bounds1, AABB bounds2) {
	Line3 line;
	line.start = getCenter(bounds1);
	line.end = normalize(getCenter(bounds2) - line.start) * 99999;
	Vec3 hit1, hit2;
	if (!overlaps(bounds1, line, &hit1)) return 99999;
	if (!overlaps(bounds2, line, &hit2)) return 99999;

	float dist = distance(hit1, hit2);
	if (distance(getCenter(bounds1), hit2) < distance(getCenter(bounds1), hit1)) dist *= -1;

	return dist;
}

void AABBToTriangles(AABB box, Tri *outTris);
void AABBToTriangles(AABB box, Tri *outTris) {
	Tri boxTris[12];

	Vec3 size = box.max - box.min;

	Tri *botA = &outTris[0];
	{
		botA->verts[0] = box.min;

		botA->verts[1] = box.min;
		botA->verts[1].x += size.x;

		botA->verts[2] = box.min;
		botA->verts[2].y += size.y;
	}

	Tri *botB = &outTris[1];
	{
		botB->verts[0] = box.min;
		botB->verts[0].x += size.x;

		botB->verts[1] = box.min;
		botB->verts[1].x += size.x;
		botB->verts[1].y += size.y;

		botB->verts[2] = box.min;
		botB->verts[2].y += size.y;
	}

	Tri *backA = &outTris[2];
	{
		backA->verts[0] = box.min;

		backA->verts[1] = box.min;
		backA->verts[1].x += size.x;

		backA->verts[2] = box.min;
		backA->verts[2].z += size.z;
	}

	Tri *backB = &outTris[3];
	{
		backB->verts[0] = box.min; 
		backB->verts[0].x += size.x;

		backB->verts[1] = box.min; 
		backB->verts[1].x += size.x;
		backB->verts[1].z += size.z;

		backB->verts[2] = box.min; 
		backB->verts[2].z += size.z;
	}

	Tri *rightA = &outTris[4];
	{
		rightA->verts[0] = box.min; 

		rightA->verts[1] = box.min; 
		rightA->verts[1].z += size.z;

		rightA->verts[2] = box.min; 
		rightA->verts[2].y += size.y;
	}

	Tri *rightB = &outTris[5];
	{
		rightB->verts[0] = box.min; 
		rightB->verts[0].z += size.z;

		rightB->verts[1] = box.min; 
		rightB->verts[1].z += size.z;
		rightB->verts[1].y += size.y;

		rightB->verts[2] = box.min; 
		rightB->verts[2].y += size.y;
	}

	Tri *topA = &outTris[6];
	*topA = *botA;
	topA->verts[0].z += size.z;
	topA->verts[1].z += size.z;
	topA->verts[2].z += size.z;

	Tri *topB = &outTris[7];
	*topB = *botB;
	topB->verts[0].z += size.z;
	topB->verts[1].z += size.z;
	topB->verts[2].z += size.z;

	Tri *frontA = &outTris[8];
	*frontA = *backA;
	frontA->verts[0].y += size.y;
	frontA->verts[1].y += size.y;
	frontA->verts[2].y += size.y;

	Tri *frontB = &outTris[9];
	*frontB = *backB;
	frontB->verts[0].y += size.y;
	frontB->verts[1].y += size.y;
	frontB->verts[2].y += size.y;

	Tri *leftA = &outTris[10];
	*leftA = *rightA;
	leftA->verts[0].x += size.x;
	leftA->verts[1].x += size.x;
	leftA->verts[2].x += size.x;

	Tri *leftB = &outTris[11];
	*leftB = *rightB;
	leftB->verts[0].x += size.x;
	leftB->verts[1].x += size.x;
	leftB->verts[2].x += size.x;
}

void AABBToLines(AABB box, Vec3 *points); // 12 lines = 24 Vec3's (interleaved)
void AABBToLines(AABB box, Vec3 *points) {
	Vec3 size = box.max - box.min;

	points[0] = box.min;
	points[1] = points[0];
	points[1].x += size.x;

	points[2] = box.min;
	points[3] = points[2];
	points[3].y += size.y;

	points[4] = box.min;
	points[4].x += size.x;
	points[4].y += size.y;
	points[5] = points[4];
	points[5].x -= size.x;

	points[6] = box.min;
	points[6].x += size.x;
	points[6].y += size.y;
	points[7] = points[6];
	points[7].y -= size.y;

	points[8] = points[0] + v3(0, 0, size.z);
	points[9] = points[1] + v3(0, 0, size.z);
	points[10] = points[2] + v3(0, 0, size.z);
	points[11] = points[3] + v3(0, 0, size.z);
	points[12] = points[4] + v3(0, 0, size.z);
	points[13] = points[5] + v3(0, 0, size.z);
	points[14] = points[6] + v3(0, 0, size.z);
	points[15] = points[7] + v3(0, 0, size.z);

	points[16] = box.min;
	points[17] = points[16];
	points[17].z += size.z;

	points[18] = points[16] + v3(size.x, 0, 0);
	points[19] = points[17] + v3(size.x, 0, 0);

	points[20] = points[16] + v3(0, size.y, 0);
	points[21] = points[17] + v3(0, size.y, 0);

	points[22] = points[16] + v3(size.x, size.y, 0);
	points[23] = points[17] + v3(size.x, size.y, 0);
}

void toPoints(AABB box, Vec3 *points); // 8 points
void toPoints(AABB box, Vec3 *points) {
	Vec3 size = box.max - box.min;
	points[0] = box.min + size*v3(0, 0, 0);
	points[1] = box.min + size*v3(1, 0, 0);
	points[2] = box.min + size*v3(0, 1, 0);
	points[3] = box.min + size*v3(1, 1, 0);
	points[4] = box.min + size*v3(0, 0, 1);
	points[5] = box.min + size*v3(1, 0, 1);
	points[6] = box.min + size*v3(0, 1, 1);
	points[7] = box.min + size*v3(1, 1, 1);
}

bool contains(AABB bounds, Vec3 point);
bool contains(AABB bounds, Vec3 point) {
	if (
		point.x > bounds.min.x && point.x < bounds.max.x &&
		point.y > bounds.min.y && point.y < bounds.max.y &&
		point.z > bounds.min.z && point.z < bounds.max.z
	) {
		return true;
	}

	return false;
}
// bool contains(AABB bounds, Tri tri); // I think intersects handles this now
// bool contains(AABB bounds, Tri tri) {
// 	if (contains(bounds, tri.verts[0])) return true;
// 	if (contains(bounds, tri.verts[1])) return true;
// 	if (contains(bounds, tri.verts[2])) return true;
// 	if (intersects(bounds, tri)) return true;
// 	return false;
// }
void AABBIntersectsTriangle_Project(Vec3 *verts, int vertsNum, Vec3 axis, float *min, float *max) {
	*min = 99999;
	*max = -99999;
	for (int i = 0; i < vertsNum; i++) {
		Vec3 vert = verts[i];
		float val = axis.dot(vert);
		if (val < *min) *min = val;
		if (val > *max) *max = val;
	}
}

bool intersects(AABB bounds, Tri tri) {
	float triangleMin, triangleMax;

	// Test the bounds normals (x-, y- and z-axes)
	Vec3 boxNormals[3] = {
		v3(1,0,0),
		v3(0,1,0),
		v3(0,0,1),
	};

	float *boxMinCoords = &bounds.min.x;
	float *boxMaxCoords = &bounds.max.x;
	for (int i = 0; i < 3; i++) {
		AABBIntersectsTriangle_Project(tri.verts, 3, boxNormals[i], &triangleMin, &triangleMax);
		if (triangleMax < boxMinCoords[i] || triangleMin > boxMaxCoords[i]) return false; // No intersection possible.
	}

	// Test the tri normal
	Vec3 normal = (tri.verts[0] - tri.verts[1]).cross(tri.verts[2] - tri.verts[1]);
	float boxMin, boxMax;
	float triangleOffset = normal.dot(tri.verts[0]);
	Vec3 boxVerts[8];
	getVerts(bounds, boxVerts);
	AABBIntersectsTriangle_Project(boxVerts, 8, normal, &boxMin, &boxMax);
	if (boxMax < triangleOffset || boxMin > triangleOffset) return false; // No intersection possible.

	// Test the nine edge cross-products
	Vec3 triangleEdges[3] = {
		tri.verts[0] - tri.verts[1],
		tri.verts[1] - tri.verts[2],
		tri.verts[2] - tri.verts[0],
	};

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			// The bounds normals are the same as it's edge tangents
			Vec3 axis = triangleEdges[i].cross(boxNormals[j]);
			AABBIntersectsTriangle_Project(boxVerts, 8, axis, &boxMin, &boxMax);
			AABBIntersectsTriangle_Project(tri.verts, 3, axis, &triangleMin, &triangleMax);
			if (boxMax < triangleMin || boxMin > triangleMax) return false; // No intersection possible
		}
	}

	// No separating axis found.
	return true;
}

Vec3 getSize(AABB bounds) {
	return bounds.max - bounds.min;
}

Vec3 getCenter(AABB bounds) {
#if 0
	Vec3 size = getSize(bounds);
	Vec3 center;
	center = bounds.min + size/2;
	return center;
#else
	return (bounds.min + bounds.max)/2.0;
#endif
}

AABB toAABB(Tri tri) {
	AABB bounds;
	bounds.min.x = 99999;
	bounds.min.y = 99999;
	bounds.min.z = 99999;
	bounds.max.x = -99999;
	bounds.max.y = -99999;
	bounds.max.z = -99999;
	for (int i = 0; i < 3; i++) {
		if (bounds.min.x > tri.verts[i].x) bounds.min.x = tri.verts[i].x;
		if (bounds.min.y > tri.verts[i].y) bounds.min.y = tri.verts[i].y;
		if (bounds.min.z > tri.verts[i].z) bounds.min.z = tri.verts[i].z;
		if (bounds.max.x < tri.verts[i].x) bounds.max.x = tri.verts[i].x;
		if (bounds.max.y < tri.verts[i].y) bounds.max.y = tri.verts[i].y;
		if (bounds.max.z < tri.verts[i].z) bounds.max.z = tri.verts[i].z;
	}

	float ep = 0.01;
	bounds.min.x -= ep;
	bounds.min.y -= ep;
	bounds.min.z -= ep;
	bounds.max.x += ep;
	bounds.max.y += ep;
	bounds.max.z += ep;
	return bounds;
}

AABB toAABB(Capsule3 cap);
AABB toAABB(Capsule3 cap) {
	AABB bounds;
	bounds.min.x = MinNum(cap.start.x - cap.radius, cap.end.x - cap.radius);
	bounds.min.y = MinNum(cap.start.y - cap.radius, cap.end.y - cap.radius);
	bounds.min.z = MinNum(cap.start.z - cap.radius, cap.end.z - cap.radius);
	bounds.max.x = MaxNum(cap.start.x + cap.radius, cap.end.x + cap.radius);
	bounds.max.y = MaxNum(cap.start.y + cap.radius, cap.end.y + cap.radius);
	bounds.max.z = MaxNum(cap.start.z + cap.radius, cap.end.z + cap.radius);
	return bounds;
}

Capsule3 toCapsule(AABB bounds);
Capsule3 toCapsule(AABB bounds) {
	Vec3 size = getSize(bounds);

	Capsule3 cap = {};
	cap.start = bounds.min;
	cap.start.x += size.x/2;
	cap.start.y += size.y/2;

	cap.end = cap.start;
	cap.end.z += size.z;

	cap.radius = (size.x + size.y)/2.0;
	return cap;
}

Vec3 getCenter(Capsule3 cap) {
	return (cap.start + cap.end)/2;
}

bool isZero(AABB bounds) {
	if (bounds.min.isZero() && bounds.max.isZero()) return true;
	return false;
}

AABB expand(AABB bounds, AABB otherBounds) {
	bounds = expand(bounds, otherBounds.min);
	bounds = expand(bounds, otherBounds.max);
	return bounds;
}
AABB expand(AABB bounds, Vec3 point) {
	if (bounds.min.x > point.x) bounds.min.x = point.x;
	if (bounds.min.y > point.y) bounds.min.y = point.y;
	if (bounds.min.z > point.z) bounds.min.z = point.z;
	if (bounds.max.x < point.x) bounds.max.x = point.x;
	if (bounds.max.y < point.y) bounds.max.y = point.y;
	if (bounds.max.z < point.z) bounds.max.z = point.z;
	return bounds;
}

bool equal(AABB bounds1, AABB bounds2) {
	return equal(bounds1.min, bounds2.min) && equal(bounds2.min, bounds2.min);
}

Vec3 calculateTriangleNormal(Tri tri) {
	Vec3 p0 = tri.verts[1] - tri.verts[0];
	Vec3 p1 = tri.verts[2] - tri.verts[0];
	Vec3 faceNormal = p0.cross(p1);
	return faceNormal;
}

Matrix4 getBeamMatrix(Vec3 start, Vec3 end, float thickness) {
	Vec3 centerPos = start.add(end).multiply(0.5);

	Matrix4 mat = mat4();
	mat.TRANSLATE(centerPos.x, centerPos.y, centerPos.z);
	mat.ROTATE_QUAT(quaternionBetween(start, end));
	mat.SCALE(start.distance(end) * 0.5, thickness, thickness);
	return mat;
}

float rayTestCapsule(Vec3 start, Vec3 dir, Vec3 capsuleStart, Vec3 capsuleEnd, float capsuleRadius);
float rayTestCapsule(Vec3 start, Vec3 dir, Vec3 capsuleStart, Vec3 capsuleEnd, float capsuleRadius) {
	Vec3 ro = start;
	Vec3 rd = dir;
	Vec3 pa = capsuleStart;
	Vec3 pb = capsuleEnd;
	float ra = capsuleRadius;

	Vec3 ba = pb - pa;
	Vec3 oa = ro - pa;
	float baba = ba.dot(ba);
	float bard = ba.dot(rd);
	float baoa = ba.dot(oa);
	float rdoa = rd.dot(oa);
	float oaoa = oa.dot(oa);
	float a = baba      - bard*bard;
	float b = baba*rdoa - baoa*bard;
	float c = baba*oaoa - baoa*baoa - ra*ra*baba;
	float h = b*b - a*c;
	if( h >= 0.0 )
	{
		float t = (-b-sqrt(h))/a;
		float y = baoa + t*bard;
		// body
		if(y > 0.0 && y < baba ) return t;
		// caps
		Vec3 oc = (y <= 0.0) ? oa : ro - pb;
		b = rd.dot(oc);
		c = oc.dot(oc) - ra*ra;
		h = b*b - c;
		if( h>0.0 ) return -b - sqrt(h);
	}
	return -1.0;
}

Line3 makeLine3(Vec3 start, Vec3 end) { return {start, end}; }

Vec3 projectPointOnInfiniteLine(Vec3 point, Line3 line);
Vec3 projectPointOnInfiniteLine(Vec3 point, Line3 line) {
	Vec3 a = line.start;
	Vec3 b = line.end;
	Vec3 p = point;

	Vec3 ap = p-a;
	Vec3 ab = b-a;
	Vec3 result = a + ap.dot(ab)/ab.dot(ab) * ab;
	return result;
}

// https://wickedengine.net/2020/04/26/capsule-collision-detection/
Vec3 projectPointOnLine(Vec3 a, Vec3 b, Vec3 p);
Vec3 projectPointOnLine(Vec3 a, Vec3 b, Vec3 p) {
  Vec3 ab = b - a;
  float t = (p - a).dot(ab) / ab.dot(ab);
  return a + MinNum(MaxNum(t, 0), 1) * ab; // saturate(t) can be written as: min((max(t, 0), 1)
}

Vec3 projectPointOnLine(Vec3 point, Line3 line);
Vec3 projectPointOnLine(Vec3 point, Line3 line) {
	Vec3 a = line.start;
	Vec3 b = line.end;
	Vec3 p = point;

  Vec3 ab = b - a;
  float t = (p - a).dot(ab) / ab.dot(ab);
  return a + MinNum(MaxNum(t, 0), 1) * ab; // saturate(t) can be written as: min((max(t, 0), 1)
}

bool intersects(Vec3 point, Line3 line);
bool intersects(Vec3 point, Line3 line) {
	Vec3 a = line.start;
	Vec3 b = line.end;
	Vec3 p = point;

	float ab = a.distance(b);
	float ap = a.distance(p);
	float bp = b.distance(p);

#if 1
	if (fabs(ab - (ap + bp)) < 0.001) return true;
	return false;
#else
	if (ab == ap + bp) return true;
	return false;
#endif
}

// https://wickedengine.net/2020/04/26/capsule-collision-detection/
struct IntersectionResult {
	float length;
	Vec3 penetrationNormal;
	float penetrationDepth;
	Vec3 calcCenter;
};
IntersectionResult sphereIntersectsTriangle(Vec3 center, float radius, Tri tri) {
	Vec3 p0 = tri.verts[0];
	Vec3 p1 = tri.verts[1];
	Vec3 p2 = tri.verts[2];
	Vec3 N = ((p1 - p0).cross(p2 - p0)).normalize(); // plane normal
	float dist = (center - p0).dot(N); // signed distance between sphere and plane
	// if(!mesh.is_double_sided() && dist > 0) continue; // can pass through back side of triangle (optional)
	if (dist < -radius || dist > radius) { // no intersection
		IntersectionResult result;
		result.length = -1;
		return result;
	}


	Vec3 point0 = center - N * dist; // projected sphere center on triangle plane

	// Now determine whether point0 is inside all triangle edges: 
	Vec3 c0 = (point0 - p0).cross(p1 - p0);
	Vec3 c1 = (point0 - p1).cross(p2 - p1);
	Vec3 c2 = (point0 - p2).cross(p0 - p2);
	bool inside = c0.dot(N) <= 0 && c1.dot(N) <= 0 && c2.dot(N) <= 0;

	float radiussq = radius * radius; // sphere radius squared

	// Edge 1:
	Vec3 point1 = projectPointOnLine(p0, p1, center);
	Vec3 vec1 = center - point1;
	float distsq1 = vec1.dot(vec1);
	bool intersects = distsq1 < radiussq;

	// Edge 2:
	Vec3 point2 = projectPointOnLine(p1, p2, center);
	Vec3 vec2 = center - point2;
	float distsq2 = vec2.dot(vec2);
	intersects |= distsq2 < radiussq;

	// Edge 3:
	Vec3 point3 = projectPointOnLine(p2, p0, center);
	Vec3 vec3 = center - point3;
	float distsq3 = vec3.dot(vec3);
	intersects |= distsq3 < radiussq;

	if (inside || intersects) {
		Vec3 best_point = point0;
		Vec3 intersection_vec;

		if(inside) {
			intersection_vec = center - point0;
		} else {
			Vec3 d = center - point1;
			float best_distsq = d.dot(d);
			best_point = point1;
			intersection_vec = d;

			d = center - point2;
			float distsq = d.dot(d);
			if(distsq < best_distsq) {
				distsq = best_distsq;
				best_point = point2;
				intersection_vec = d;
			}

			d = center - point3;
			distsq = d.dot(d);
			if(distsq < best_distsq) {
				distsq = best_distsq;
				best_point = point3; 
				intersection_vec = d;
			}
		}

		float len = intersection_vec.length();  // vector3 length calculation: sqrt(v.dot(v))
		Vec3 penetration_normal;
		if (len > 0.001) {
			penetration_normal = intersection_vec / len;  // normalize
		} else {
			penetration_normal = v3();
		}

		float penetration_depth = radius - len; // radius = sphere radius
		IntersectionResult result;
		result.length = len;
		result.penetrationNormal = penetration_normal;
		result.penetrationDepth = penetration_depth;
		result.calcCenter = center;
		return result;
	}

	IntersectionResult result;
	result.length = -1;
	return result;
}

IntersectionResult intersects(Capsule3 cap, Tri tri);
IntersectionResult intersects(Capsule3 cap, Tri tri) {
	// Compute capsule line endpoints A, B like before in capsule-capsule case:
	Vec3 tip = cap.start; // I swapped tip/A and base/b
	Vec3 base = cap.end;

	Vec3 p0 = tri.verts[0];
	Vec3 p1 = tri.verts[1];
	Vec3 p2 = tri.verts[2];
	Vec3 N = ((p1 - p0).cross(p2 - p0)).normalize(); // plane normal

	Vec3 CapsuleNormal = (tip - base).normalize();
	Vec3 LineEndOffset = CapsuleNormal * cap.radius; 
	Vec3 A = base;
	Vec3 B = tip;
	tip -= LineEndOffset;
	base += LineEndOffset;

	bool para = false;
	if (CapsuleNormal.dot(N) < 0.01) para = true;

	// Vec3 reference_point = {find closest point on triangle to line_plane_intersection};
	Vec3 reference_point;

	// Then for each triangle, ray-plane intersection:
	//  N is the triangle plane normal (it was computed in sphere – triangle intersection case)
	if (!para) {
		float t = N.dot((p0 - base) / fabs(N.dot(CapsuleNormal)));
		Vec3 line_plane_intersection = base + CapsuleNormal * t;

		// Determine whether line_plane_intersection is inside all triangle edges: 
		Vec3 c0 = (line_plane_intersection - p0).cross(p1 - p0);
		Vec3 c1 = (line_plane_intersection - p1).cross(p2 - p1);
		Vec3 c2 = (line_plane_intersection - p2).cross(p0 - p2);
		bool inside = c0.dot(N) <= 0 && c1.dot(N) <= 0 && c2.dot(N) <= 0;

		if (inside) {
			reference_point = line_plane_intersection;
		} else {
			// Edge 1:
			Vec3 point1 = projectPointOnLine(p0, p1, line_plane_intersection);
			Vec3 vec1 = line_plane_intersection - point1;
			float distsq = vec1.dot(vec1);
			float best_dist = distsq;
			reference_point = point1;

			// Edge 2:
			Vec3 point2 = projectPointOnLine(p1, p2, line_plane_intersection);
			Vec3 vec2 = line_plane_intersection - point2;
			distsq = vec2.dot(vec2);
			if(distsq < best_dist) {
				reference_point = point2;
				best_dist = distsq;
			}

			// Edge 3:
			Vec3 point3 = projectPointOnLine(p2, p0, line_plane_intersection);
			Vec3 vec3 = line_plane_intersection - point3;
			distsq = vec3.dot(vec3);
			if(distsq < best_dist) {
				reference_point = point3;
				best_dist = distsq;
			}
		}
	} else {
		reference_point = p0;
	}

	// The center of the best sphere candidate:
	Vec3 center = projectPointOnLine(A, B, reference_point);

	IntersectionResult result = sphereIntersectsTriangle(center, cap.radius, tri);
	return result;
}

void extractFrustumPlanes(Matrix4 projectionView, Vec4 *planes);
void extractFrustumPlanes(Matrix4 projectionView, Vec4 *planes) {
	float *m = &projectionView.data[0];
	// Left clipping plane
	planes[0].x = m[3] + m[0];
	planes[0].y = m[7] + m[4];
	planes[0].z = m[11] + m[8];
	planes[0].w = m[15] + m[12];
	// Right clipping plane
	planes[1].x = m[3] - m[0];
	planes[1].y = m[7] - m[4];
	planes[1].z = m[11] - m[8];
	planes[1].w = m[15] - m[12];
	// Top clipping plane
	planes[2].x = m[3] - m[1];
	planes[2].y = m[7] - m[5];
	planes[2].z = m[11] - m[9];
	planes[2].w = m[15] - m[13];
	// Bottom clipping plane
	planes[3].x = m[3] + m[1];
	planes[3].y = m[7] + m[5];
	planes[3].z = m[11] + m[9];
	planes[3].w = m[15] + m[13];
	// Near clipping plane
	planes[4].x = m[3] + m[2];
	planes[4].y = m[7] + m[6];
	planes[4].z = m[11] + m[10];
	planes[4].w = m[15] + m[14];
	// Far clipping plane
	planes[5].x = m[3] - m[2];
	planes[5].y = m[7] - m[6];
	planes[5].z = m[11] - m[10];
	planes[5].w = m[15] - m[14];
	// Normalize the plane equations, if requested
	// if (normalize) {
	// 	NormalizePlane(planes[0]);
	// 	NormalizePlane(planes[1]);
	// 	NormalizePlane(planes[2]);
	// 	NormalizePlane(planes[3]);
	// 	NormalizePlane(planes[4]);
	// 	NormalizePlane(planes[5]);
	// }
}

bool frustumContains(Matrix4 projectionView, Vec3 point);
bool frustumContains(Matrix4 projectionView, Vec3 point) {
	Vec4 planes[6];
	extractFrustumPlanes(projectionView, planes);

	for (int i = 0; i < 6; i++) {
		Vec4 plane = planes[i];

		float dist = plane.x*point.x + plane.y*point.y + plane.z*point.z + plane.w;
		// logf("dist: %f\n", dist);
		if (dist < 0) return false;
	}

	return true;
}

bool frustumContains(Matrix4 projectionView, AABB bounds);
bool frustumContains(Matrix4 projectionView, AABB bounds) {
	Vec3 size = getSize(bounds);
	Vec3 points[8];
	points[0] = bounds.min;

	points[1] = bounds.min;
	points[1].x += size.x;

	points[2] = bounds.min;
	points[2].y += size.y;

	points[3] = bounds.min;
	points[3].x += size.x;
	points[3].y += size.y;

	points[4] = points[0] + v3(0, 0, size.z);
	points[5] = points[1] + v3(0, 0, size.z);
	points[6] = points[2] + v3(0, 0, size.z);
	points[7] = points[3] + v3(0, 0, size.z);

	Vec4 planes[6];
	extractFrustumPlanes(projectionView, planes);


	for (int i = 0; i < 8; i++) {
		Vec3 point = points[i];

		bool inside = true;

		for (int i = 0; i < 6; i++) {
			Vec4 plane = planes[i];

			float dist = plane.x*point.x + plane.y*point.y + plane.z*point.z + plane.w;
			if (dist < 0) {
				inside = false;
				break;
			}
		}

		if (inside) return true;
	}

	return false;
}

/// Circle
struct Circle {
	Vec2 position;
	float radius;
};

Circle makeCircle(Vec2 position, float radius);
Circle makeCircle(Vec2 position, float radius) {
	return {position, radius};
}

bool contains(Circle circle, Vec2 point);
bool contains(Circle circle, Vec2 point) {
	return circle.position.distance(point) <= circle.radius;
}

bool contains(Rect rect, Circle circle);
bool contains(Rect rect, Circle circle) {
	float distX = fabs(circle.position.x - rect.x - rect.width/2);
	float distY = fabs(circle.position.y - rect.y - rect.height/2);

	if (distX > (rect.width/2 + circle.radius)) return false;
	if (distY > (rect.height/2 + circle.radius)) return false;

	if (distX <= (rect.width/2)) return true;
	if (distY <= (rect.height/2)) return true;

	float dx = distX - rect.width/2;
	float dy = distY - rect.height/2;
	if (dx*dx+dy*dy <= (circle.radius*circle.radius)) return true;

	if (contains(rect, circle.position)) return true;
	if (contains(circle, getCenter(rect))) return true;

	return false;
}

bool contains(Circle circle0, Circle circle1);
bool contains(Circle circle0, Circle circle1) {
	return circle0.position.distance(circle1.position) < circle0.radius+circle1.radius;
}

/// Line2

Line2 operator* (Matrix3 a, Line2 b) { return { a * b.start, a * b.end }; }

Line2 makeLine2(Vec2 start=v2(), Vec2 end=v2());
Line2 makeLine2(Vec2 start, Vec2 end) {
	Line2 line;
	line.start = start;
	line.end = end;
	return line;
}

bool isZero(Line2 line);
bool isZero(Line2 line) {
	if (!isZero(line.start)) return false;
	if (!isZero(line.end)) return false;
	return true;
}

bool overlaps(Line2 line1, Line2 line2);
bool overlaps(Line2 line1, Line2 line2) {
  float uA = ((line2.end.x-line2.start.x)*(line1.start.y-line2.start.y) - (line2.end.y-line2.start.y)*(line1.start.x-line2.start.x)) /
		((line2.end.y-line2.start.y)*(line1.end.x-line1.start.x) - (line2.end.x-line2.start.x)*(line1.end.y-line1.start.y));
  float uB = ((line1.end.x-line1.start.x)*(line1.start.y-line2.start.y) - (line1.end.y-line1.start.y)*(line1.start.x-line2.start.x)) /
		((line2.end.y-line2.start.y)*(line1.end.x-line1.start.x) - (line2.end.x-line2.start.x)*(line1.end.y-line1.start.y));

  if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) return true;
  return false;
}

bool overlaps(Rect rect, Line2 line);
bool overlaps(Rect rect, Line2 line) {
	Line2 lines[4];
	toLines(rect, lines);
	for (int i = 0; i < 4; i++) {
		if (overlaps(lines[i], line)) return true;
	}

	return false;
}

/// / Line2

struct Capsule2 {
	Vec2 start;
	Vec2 end;
	float radius;
};

Capsule2 makeCapsule2(Vec2 start, Vec2 end, float radius);
Capsule2 makeCapsule2(Vec2 start, Vec2 end, float radius) {
	Capsule2 cap;
	cap.start = start;
	cap.end = end;
	cap.radius = radius;
	return cap;
}

Capsule2 makeCapsule2(Line2 line, float radius);
Capsule2 makeCapsule2(Line2 line, float radius) {
	Capsule2 cap;
	cap.start = line.start;
	cap.end = line.end;
	cap.radius = radius;
	return cap;
}

float distFromLineToPoint(Vec2 start, Vec2 end, Vec2 point);
float distFromLineToPoint(Vec2 start, Vec2 end, Vec2 point) {
  // Return minimum distance between line segment vw and point p
  float l2 = (end-start).dot(end-start);  // i.e. |w-v|^2 -  avoid a sqrt
  if (l2 == 0.0) return point.distance(start);   // v == w case
  // Consider the line extending the segment, parameterized as v + t (w - v).
  // We find projection of point p onto the line. 
  // It falls where t = [(p-v) . (w-v)] / |w-v|^2
  // We clamp t from [0,1] to handle points outside the segment vw.
  float t = MaxNum(0, MinNum(1, (point - start).dot(end - start) / l2));
  Vec2 projection = start + t * (end - start);  // Projection falls on the segment
  return point.distance(projection);
}

bool contains(Capsule2 capsule, Vec2 point);
bool contains(Capsule2 capsule, Vec2 point) {
	float dist = distFromLineToPoint(capsule.start, capsule.end, point);
	if (dist <= capsule.radius) return true;
	return false;
}

void secsToHMS(int *outHours, int *outMins, int *outSecs, float secs);
void secsToHMS(int *outHours, int *outMins, int *outSecs, float secs) {
	*outHours = 0;
	*outMins = 0;
	*outSecs = 0;
	while (secs > 60*60) {
		*outHours = *outHours + 1;
		secs -= 60*60;
	}
	while (secs > 60) {
		*outMins = *outMins + 1;
		secs -= 60;
	}
	*outSecs = (int)secs;
};

char *secsToHMSFrameString(float timeLeft);
char *secsToHMSFrameString(float timeLeft) {
	int hours, mins, secs;
	secsToHMS(&hours, &mins, &secs, timeLeft);

	char *str = NULL;
	if (mins > 0) {
		str = frameSprintf("%dm %ds", mins, secs);
	} else {
		str = frameSprintf("%ds", secs);
	}

	return str;
}

/// Vec2i
struct Vec2i {
	int x, y;

	void print(const char *label) {
		logf("%s %d %d\n", label, this->x, this->y);
	}
};

Vec2i operator+ (Vec2i a, Vec2i b) { return { a.x + b.x, a.y + b.y }; }
Vec2i operator- (Vec2i a, Vec2i b) { return { a.x - b.x, a.y - b.y }; }

Vec2i v2i(int x=0, int y=0);
Vec2i v2i(int x, int y) {
	Vec2i ret = {x, y};
	return ret;
}

Vec2i v2i(Vec2 vec);
Vec2i v2i(Vec2 vec) {
	Vec2i ret = {(int)vec.x, (int)vec.y};
	return ret;
}

bool equal(Vec2i a, Vec2i b);
bool equal(Vec2i a, Vec2i b) {
	if (a.x == b.x && a.y == b.y) return true;
	return false;
}

Vec2 v2(Vec2i veci) {
	return v2(veci.x, veci.y);
}

/// /Vec2i

/// Vec4i
struct Vec4i {
	int x, y, z, w;

	void print(const char *label) {
		logf("%s %d %d\n", label, this->x, this->y);
	}
};

Vec4i operator+ (Vec4i a, Vec4i b) { return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; }
Vec4i operator- (Vec4i a, Vec4i b) { return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w }; }

Vec4i v4i(int x=0, int y=0, int z=0, int w=0);
Vec4i v4i(int x, int y, int z, int w) {
	Vec4i ret = {x, y, z, w};
	return ret;
}

Vec4i v4i(Vec4 vec);
Vec4i v4i(Vec4 vec) {
	Vec4i ret = {(int)vec.x, (int)vec.y, (int)vec.z, (int)vec.w};
	return ret;
}

bool equal(Vec4i a, Vec4i b);
bool equal(Vec4i a, Vec4i b) {
	if (a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w) return true;
	return false;
}

Vec4 v4(Vec4i veci) {
	return v4(veci.x, veci.y, veci.z, veci.w);
}

/// /Vec2i

/// Matrix 2x3

struct Matrix2x3 {
	float data[6];
	// a b
	// c d
	// x y
};

Matrix2x3 toMatrix2x3(Matrix3 matrix);
Matrix2x3 toMatrix2x3(Matrix3 matrix) {
	Matrix2x3 matrix2;
	matrix2.data[0] = matrix.data[0];
	matrix2.data[1] = matrix.data[1];
	matrix2.data[2] = matrix.data[3];
	matrix2.data[3] = matrix.data[4];
	matrix2.data[4] = matrix.data[6];
	matrix2.data[5] = matrix.data[7];
	return matrix2;
}

Matrix3 toMatrix3(Matrix2x3 matrix2);
Matrix3 toMatrix3(Matrix2x3 matrix2) {
	Matrix3 matrix = mat3();
	matrix.data[0] = matrix2.data[0];
	matrix.data[1] = matrix2.data[1];
	matrix.data[3] = matrix2.data[2];
	matrix.data[4] = matrix2.data[3];
	matrix.data[6] = matrix2.data[4];
	matrix.data[7] = matrix2.data[5];
	return matrix;
}

/// / Matrix 2x3

/// Tri2
struct Tri2 {
	Vec2 verts[3];
};

Tri2 operator*(Matrix3 matrix, Tri2 tri) {
	Tri2 ret;
	ret.verts[0] = matrix * tri.verts[0];
	ret.verts[1] = matrix * tri.verts[1];
	ret.verts[2] = matrix * tri.verts[2];
	return ret;
}

void toTris(Rect rect, Tri2 *tri0, Tri2 *tri1);
void toTris(Rect rect, Tri2 *tri0, Tri2 *tri1) {
	tri0->verts[0].x = rect.x;
	tri0->verts[0].y = rect.y;

	tri0->verts[1].x = rect.x + rect.width;
	tri0->verts[1].y = rect.y;

	tri0->verts[2].x = rect.x + rect.width;
	tri0->verts[2].y = rect.y + rect.height;

	tri1->verts[0].x = rect.x;
	tri1->verts[0].y = rect.y;

	tri1->verts[1].x = rect.x;
	tri1->verts[1].y = rect.y + rect.height;

	tri1->verts[2].x = rect.x + rect.width;
	tri1->verts[2].y = rect.y + rect.height;
};

bool overlaps(Tri2 tri, Vec2 point);
bool overlaps(Tri2 tri, Vec2 point) {
	auto sign = [](Vec2 p1, Vec2 p2, Vec2 p3)->float {
		return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
	};

	float d1 = sign(point, tri.verts[0], tri.verts[1]);
	float d2 = sign(point, tri.verts[1], tri.verts[2]);
	float d3 = sign(point, tri.verts[2], tri.verts[0]);

	bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	return !(has_neg && has_pos);
}

void toLines(Tri2 tri, Line2 *lines); // 3 lines
void toLines(Tri2 tri, Line2 *lines) {
	lines[0].start = tri.verts[0];
	lines[0].end = tri.verts[1];

	lines[1].start = tri.verts[1];
	lines[1].end = tri.verts[2];

	lines[2].start = tri.verts[2];
	lines[2].end = tri.verts[0];
}

bool overlaps(Tri2 tri0, Tri2 tri1);
bool overlaps(Tri2 tri0, Tri2 tri1) {
	if (overlaps(tri0, tri1.verts[0])) return true;
	if (overlaps(tri0, tri1.verts[1])) return true;
	if (overlaps(tri0, tri1.verts[2])) return true;
	if (overlaps(tri1, tri0.verts[0])) return true;
	if (overlaps(tri1, tri0.verts[1])) return true;
	if (overlaps(tri1, tri0.verts[2])) return true;

	Line2 lines0[3];
	toLines(tri0, lines0);

	Line2 lines1[3];
	toLines(tri1, lines1);

	for (int i = 0; i < 3; i++) {
		Line2 line0 = lines0[i];
		for (int i = 0; i < 3; i++) {
			Line2 line1 = lines1[i];
			if (overlaps(line0, line1)) return true;
		}
	}
	return false;
}
/// / Tri2

struct Xform2 {
	Vec2 translation;
	Vec2 scale;
	float rotation;
};

Xform2 createXform2();
Xform2 createXform2() {
	Xform2 xform = {};
	xform.scale = v2(1, 1);
	return xform;
}

Xform2 lerp(Xform2 a, Xform2 b, float perc);
Xform2 lerp(Xform2 a, Xform2 b, float perc) {
	a.translation = lerp(a.translation, b.translation, perc);
	a.scale = lerp(a.scale, b.scale, perc);
	a.rotation = lerp(a.rotation, b.rotation, perc);
	return a;
}

struct Cone {
	Vec3 position;
	Vec3 direction;
	float radius;
	float length;
};

struct Sphere {
	Vec3 position;
	float radius;
};

Sphere makeSphere(Vec3 position, float radius);
Sphere makeSphere(Vec3 position, float radius) {
	Sphere sphere = {};
	sphere.position = position;
	sphere.radius = radius;
	return sphere;
}

bool overlaps(AABB aabb, Sphere sphere) {
	// https://web.archive.org/web/19991129023147/http://www.gamasutra.com/features/19991018/Gomez_4.htm
	float d = 0;
	float *C = &sphere.position.x;
	float *Bmin = &aabb.min.x;
	float *Bmax = &aabb.max.x;

	for (int i = 0; i < 3; i++ ) {
		if (C[i] < Bmin[i]) {
			float s = C[i] - Bmin[i];
			d += s*s;
		} else if(C[i] > Bmax[i]) {
			float s = C[i] - Bmax[i];
			d += s*s;
		}
	}

	float r = sphere.radius;
	return d <= r*r;
} 

