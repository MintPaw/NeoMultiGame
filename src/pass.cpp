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

	passSys->passStack[passSys->passStackNum++] = pass;
}

void popPass() {
	if (passSys->passStackNum <= 0) {
		logf("Pass stack underflow!\n");
		return;
	}

	passSys->passStackNum--;
}

PassCmd *passTexture(Texture *texture, Matrix3 matrix, int color, Vec2 uv0, Vec2 uv1) {
	if (getAofArgb(color) == 0) return NULL;
	PassCmd *cmd = createPassCmd(getCurrentPass());
	cmd->type = PASS_CMD_QUAD;
	if (!cmd) return NULL;

	cmd->texture = texture;

	cmd->verts[0] = v3(0, 0, 1);
	cmd->verts[1] = v3(1, 0, 1);
	cmd->verts[2] = v3(1, 1, 1);
	cmd->verts[3] = v3(0, 1, 1);

	for (int i = 0; i < 4; i++) {
		cmd->verts[i] = matrix * cmd->verts[i];
	}

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

	cmd->colors[0] = color;
	cmd->colors[1] = color;
	cmd->colors[2] = color;
	cmd->colors[3] = color;

	return cmd;
}

PassCmd *passTri(Tri tri, int color) {
	PassCmd *cmd = createPassCmd(getCurrentPass());
	cmd->type = PASS_CMD_TRI;

	cmd->verts[0] = tri.verts[0];
	cmd->verts[1] = tri.verts[1];
	cmd->verts[2] = tri.verts[2];

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
	if (passSys->passStackNum <= 0) {
		logf("No current pass!\n");
		return NULL;
	}

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
