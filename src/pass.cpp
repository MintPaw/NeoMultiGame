enum PassCmdType {
	PASS_CMD_CLEAR,
};

struct PassCmd {
	PassCmdType type;
	Vec2 quadVerts[4];
	Vec2 quadUvs[4];
	int quadColors[4];
	int passCmdColor;
};

struct Pass {
	int passOrder;
	PassCmd *cmds;
	int cmdsNum;
	int cmdsMax;
};

struct PassSystem {
#define PASSES_MAX 32
	Pass passes[PASSES_MAX];
	int passesNum;

#define PASS_STACK_MAX 32
	Pass *passStack[PASS_STACK_MAX];
	int passStackNum;
};

PassSystem *passSys = NULL;

void initPassSystem();
Pass *passCreate();
void passPush(Pass *pass);
void passPop();
PassCmd *passTexture(Texture *texture, Matrix3 matrix=mat3(), float alpha=1, int tint=0xFFFFFFFF);

Pass *getCurrentPass();
PassCmd *createPassCmd(Pass *pass);
/// FUNCTIONS ^

void initPassSystem() {
	passSys = (PassSystem *)zalloc(sizeof(PassSystem));
	logf("PassSystem is %.1fmb\n", (float)sizeof(PassSystem) / (float)sizeof(Megabytes(1)));
}

Pass *passCreate() {
	if (!passSys) initPassSystem();

	if (passSys->passesNum > PASSES_MAX-1) {
		logf("Too many passes!\n");
		passSys->passesNum--;
	}

	Pass *pass = &passSys->passes[passSys->passesNum++];
	memset(pass, 0, sizeof(Pass));
	return pass;
}

void passPush(Pass *pass) {
	if (passSys->passStackNum > PASS_STACK_MAX-1) {
		logf("Pass stack overflow!\n");
		passSys->passStackNum--;
	}

	passSys->passStack[passSys->passStackNum++] = pass;
}

void passPop() {
	if (passSys->passStackNum <= 0) {
		logf("Pass stack underflow!\n");
		return;
	}

	passSys->passStackNum--;
}

PassCmd *passTexture(Texture *texture, Matrix3 matrix, float alpha, int tint) {
	// alpha *= renderer->alphaStack[renderer->alphaStackNum-1];
	// if (renderer->disabled) return;
	if (alpha == 0) return NULL;
	PassCmd *cmd = createPassCmd(getCurrentPass());
	if (!cmd) return NULL;

	cmd->quadVerts[0] = v2(0, 0);
	cmd->quadVerts[1] = v2(0, 1);
	cmd->quadVerts[2] = v2(1, 1);
	cmd->quadVerts[3] = v2(1, 0);

	// matrix = renderer->baseMatrix2d * matrix;

	for (int i = 0; i < 4; i++) {
		cmd->quadVerts[i] = matrix * cmd->quadVerts[i];
	}

	cmd->quadUvs[0] = v2(0, 0);
	cmd->quadUvs[0] = v2(0, 1);
	cmd->quadUvs[0] = v2(1, 1);
	cmd->quadUvs[0] = v2(1, 0);

	Matrix3 flipMatrix = {
		1,  0,  0,
		0, -1,  0,
		0,  1,  1
	};
	for (int i = 0; i < 4; i++) {
		cmd->quadUvs[i] = flipMatrix * cmd->quadUvs[i];
	}

	setAofArgb(tint, getAofArgb(tint) * alpha);
	cmd->quadColors[0] = tint;
	cmd->quadColors[1] = tint;
	cmd->quadColors[2] = tint;
	cmd->quadColors[3] = tint;

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
