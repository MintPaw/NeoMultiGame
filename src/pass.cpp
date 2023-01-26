enum PassCmdType {
	PASS_CMD_CLEAR,
	PASS_CMD_QUAD,
	PASS_CMD_TRI,
	PASS_CMD_MESH,
};

struct PassCmd {
	PassCmdType type;
	Vec3 verts[4];
	Vec2 uvs[4];
	int colors[4];
	Texture *texture;

	Mesh *mesh;
	Matrix4 meshMatrix;
	int meshTint;
};

struct Pass {
	Camera camera;

	PassCmd *cmds;
	int cmdsNum;
	int cmdsMax;

	bool yIsUp;

#define PASS_CAMERA_2D_STACK_MAX 128
	Matrix3 camera2dStack[CAMERA_2D_STACK_MAX];
	int camera2dStackNum;
	Matrix3 currentCamera2dMatrix;

#define PASS_ALPHA_STACK_MAX 128
	float alphaStack[ALPHA_STACK_MAX]; // This is not multiplicitive?
	int alphaStackNum;
};

struct PassSystem {
#define PASS_STACK_MAX 32
	Pass *passStack[PASS_STACK_MAX];
	int passStackNum;
};

PassSystem *passSys = NULL;

void initPassSystem();
Pass *createPass();
void destroyPass(Pass *pass);
void pushPass(Pass *pass);
void popPass();
void passPushCamera2d(Matrix3 matrix);
void passPopCamera2d(Matrix3 matrix);
void passRefreshGlobalMatrices();
void passPushAlpha(float value);
void passPopAlpha();
PassCmd *passTexture(Texture *texture, Matrix3 matrix=mat3(), int color=0xFFFFFFFF, Vec2 uv0=v2(0, 0), Vec2 uv1=v2(1, 1));
PassCmd *passTri(Tri tri, int color=0xFFFFFFFF);
PassCmd *passMesh(Mesh *mesh, Matrix4 matrix, int color=0xFFFFFFFF);

Pass *getCurrentPass();
PassCmd *createPassCmd(Pass *pass);
/// FUNCTIONS ^

void initPassSystem() {
	passSys = (PassSystem *)zalloc(sizeof(PassSystem));
	logf("PassSystem is %.1fmb\n", (float)sizeof(PassSystem) / (float)sizeof(Megabytes(1)));
}

Pass *createPass() {
	if (!passSys) initPassSystem();

	Pass *pass = (Pass *)zalloc(sizeof(Pass));
	pass->yIsUp = true;

	pushPass(pass);
	passPushCamera2d(mat3());
	passPushAlpha(1);
	popPass();

	return pass;
}

void destroyPass(Pass *pass) {
	if (pass->cmds) free(pass->cmds);
	free(pass);
}

void pushPass(Pass *pass) {
	if (passSys->passStackNum > PASS_STACK_MAX-1) {
		logf("Pass stack overflow!\n");
		passSys->passStackNum--;
	}

	renderer->inPass = true;
	passSys->passStack[passSys->passStackNum++] = pass;
}

void popPass() {
	if (passSys->passStackNum <= 0) {
		logf("Pass stack underflow!\n");
		return;
	}

	passSys->passStackNum--;

	if (passSys->passStackNum == 0) renderer->inPass = false;
}

void passPushCamera2d(Matrix3 matrix) {
	Pass *pass = getCurrentPass();
	if (pass->camera2dStackNum > PASS_CAMERA_2D_STACK_MAX-1) Panic("pass camera2d overflow");

	pass->camera2dStack[pass->camera2dStackNum++] = matrix;
	passRefreshGlobalMatrices();
}

void passPopCamera2d() {
	Pass *pass = getCurrentPass();
	if (pass->camera2dStackNum <= 1) Panic("pass camera2d underflow");

	pass->camera2dStackNum--;
	passRefreshGlobalMatrices();
}

void passRefreshGlobalMatrices() {
	Pass *pass = getCurrentPass();
	pass->currentCamera2dMatrix = mat3();
	for (int i = 0; i < pass->camera2dStackNum; i++) pass->currentCamera2dMatrix *= pass->camera2dStack[i];
}

void passPushAlpha(float value) {
	Pass *pass = getCurrentPass();
	if (pass->alphaStackNum > PASS_ALPHA_STACK_MAX-1) Panic("pass alpha overflow");
	pass->alphaStack[pass->alphaStackNum++] = value;
}

void passPopAlpha() {
	Pass *pass = getCurrentPass();
	if (pass->alphaStackNum <= 1) Panic("pass alpha underflow");
	pass->alphaStackNum--;
}

PassCmd *passTexture(Texture *texture, Matrix3 matrix, int color, Vec2 uv0, Vec2 uv1) {
	if (getAofArgb(color) == 0) return NULL;
	Pass *pass = getCurrentPass();
	PassCmd *cmd = createPassCmd(pass);
	cmd->type = PASS_CMD_QUAD;
	if (!cmd) return NULL;

	cmd->texture = texture;

	matrix = pass->currentCamera2dMatrix * matrix;
	cmd->verts[0] = matrix * v3(0, 0, 1);
	cmd->verts[1] = matrix * v3(1, 0, 1);
	cmd->verts[2] = matrix * v3(1, 1, 1);
	cmd->verts[3] = matrix * v3(0, 1, 1);

	cmd->uvs[0] = v2(uv0.x, uv0.y);
	cmd->uvs[1] = v2(uv1.x, uv0.y);
	cmd->uvs[2] = v2(uv1.x, uv1.y);
	cmd->uvs[3] = v2(uv0.x, uv1.y);

	// Matrix3 flipMatrix = {
	// 	1,  0,  0,
	// 	0, -1,  0,
	// 	0,  1,  1
	// };
	// for (int i = 0; i < 4; i++) {
	// 	cmd->uvs[i] = flipMatrix * cmd->uvs[i];
	// }

	color = setAofArgb(color, getAofArgb(color) * pass->alphaStack[pass->alphaStackNum-1]);
	cmd->colors[0] = color;
	cmd->colors[1] = color;
	cmd->colors[2] = color;
	cmd->colors[3] = color;

	return cmd;
}

PassCmd *passTri(Tri tri, int color) {
	Pass *pass = getCurrentPass();
	PassCmd *cmd = createPassCmd(pass);
	cmd->type = PASS_CMD_TRI;

	Matrix3 matrix = pass->currentCamera2dMatrix;
	cmd->verts[0] = matrix * tri.verts[0];
	cmd->verts[1] = matrix * tri.verts[1];
	cmd->verts[2] = matrix * tri.verts[2];

	color = setAofArgb(color, getAofArgb(color) * pass->alphaStack[pass->alphaStackNum-1]);
	cmd->colors[0] = color;
	cmd->colors[1] = color;
	cmd->colors[2] = color;
	return cmd;
}

PassCmd *passMesh(Mesh *mesh, Matrix4 matrix, int color) {
	if (getAofArgb(color) == 0) return NULL;
	PassCmd *cmd = createPassCmd(getCurrentPass());
	cmd->type = PASS_CMD_MESH;
	cmd->mesh = mesh;
	cmd->meshMatrix = matrix;
	cmd->meshTint = color;
	return cmd;
}

Pass *getCurrentPass() {
	if (passSys->passStackNum <= 0) return NULL;

	return passSys->passStack[passSys->passStackNum-1];
}

PassCmd *createPassCmd(Pass *pass) {
	if (!pass) return NULL;

	if (pass->cmdsNum > pass->cmdsMax-1) {
		if (pass->cmdsMax <= 1) pass->cmdsMax = 32;
		else pass->cmdsMax *= 1.5;
		pass->cmds = (PassCmd *)resizeArray(pass->cmds, sizeof(PassCmd), pass->cmdsNum, pass->cmdsMax);
	}

	PassCmd *cmd = &pass->cmds[pass->cmdsNum++];
	memset(cmd, 0, sizeof(PassCmd));
	return cmd;
}
