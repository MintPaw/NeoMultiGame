#ifndef __VN_IMPL
enum VnCommandType {
	VN_NONE,
	VN_SET_IMAGE,
	VN_SHOW_IMAGE,
	VN_IMAGE_SAY,
	VN_SET_WAVES,
};

struct VnCommand {
	VnCommandType type;
	char path[PATH_MAX_LEN];//You don't define PATH_MAX_LEN here. Are you sure you can use it like this?? It's not really necessary anyways. =P
	int imageIndex;

#define VN_TEXT_MAX_LEN 512
	char text[VN_TEXT_MAX_LEN];
};

void playScript(int scriptId);
VnCommand *newVnCommand(VnCommandType type);
void setImage(int imageIndex, char *path);
void showImage(int imageIndex);
void imageSay(char *text);
void setWaves(char *text);

#else

void playScript(int scriptId) {
	if (scriptId == 1) {
		setImage(0, "assets/images/vn/testPortrait1.png");
		setImage(1, "assets/images/vn/testPortrait2.png");
		showImage(0);
		imageSay("Hello, other person");
		showImage(1);
		imageSay("Hello.");
		imageSay("How are you? This is a lot of text! This is a lot of text! This is a lot of text! This is a lot of text! This is a lot of text! This is a lot of text! This is a lot of text! This is a lot of text! This is a lot of text! This is a lot of text! This is a lot of text! This is a lot of text! This is a lot of text! This is a lot of text! This is a lot of text!");
		showImage(0);
		imageSay("I'm doing fine, thanks!");
	}
	if (scriptId == 2) {
		setImage(0, "assets/images/vn/testPortrait1.png");
		showImage(0);
		imageSay("It's seek time baby~!");
		char seekWave = (char)23;
		char enemyWaves[PATTERNS_MAX];
		for (int i = 0; i < PATTERNS_MAX; i++) {enemyWaves[i] = seekWave;}
		setWaves(enemyWaves);
	}
}

VnCommand *newVnCommand(VnCommandType type) {
	if (game->vnCommandsNum > VN_COMMANDS_MAX-1) {
		logf("Too many vn commands\n");
		return NULL;
	}

	VnCommand *command = &game->vnCommands[game->vnCommandsNum++];
	memset(command, 0, sizeof(VnCommand));
	command->type = type;
	return command;
}

void setImage(int imageIndex, char *path) {
	VnCommand *command = newVnCommand(VN_SET_IMAGE);
	command->imageIndex = imageIndex;
	strncpy(command->path, path, PATH_MAX_LEN);
}
void showImage(int imageIndex) {
	VnCommand *command = newVnCommand(VN_SHOW_IMAGE);
	command->imageIndex = imageIndex;
}
void imageSay(char *text) {
	VnCommand *command = newVnCommand(VN_IMAGE_SAY);
	strncpy(command->text, text, VN_TEXT_MAX_LEN);
}
void setWaves(char *text) {
	VnCommand *command = newVnCommand(VN_SET_WAVES);
	strncpy(command->text, text, VN_TEXT_MAX_LEN);
}
#endif
