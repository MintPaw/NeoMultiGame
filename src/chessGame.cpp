#define USING_CHEN 0

#if USING_CHEN
#include "Chenglite.c"
#else
#include "MisterQueen/bb.h"
#include "MisterQueen/bk.h"
#include "MisterQueen/board.h"
#include "MisterQueen/move.h"
#include "MisterQueen/search.h"
#include "MisterQueen/tinycthread.h"
#include "MisterQueen/util.h"
#endif

#if 0
Move count levels



Black wants to castle: r3k1nr/ppp2ppp/2n5/2bpp2b/7q/3P1P1N/PPP1P1BP/RNBQ1R1K w kq - 0 1
#endif

struct Globals {
};

enum PieceType { PIECE_PAWN, PIECE_ROOK, PIECE_KNIGHT, PIECE_BISHOP, PIECE_QUEEN, PIECE_KING };
char pieceTypeChars[] = { 'p', 'r', 'n', 'b', 'q', 'k' };
enum Team { TEAM_BLACK, TEAM_WHITE, TEAM_NONE };
char *teamStrings[] = { "Black", "White", "None" };

struct Piece {
	PieceType type;
	Team team;
	Vec2i position;
};

struct Match {
#define FEN_MAX_LEN 128
	char currentFen[FEN_MAX_LEN];
#define PIECES_MAX 64
	Piece pieces[PIECES_MAX];
	int piecesNum;
	bool canShortCastle[2];
	bool canLongCastle[2];

	Team turnTeam;
};

enum EffectType {
	EFFECT_SLIDE,
	EFFECT_IN_CHECK,
};
struct Effect {
	EffectType type;
	float time;

	Team pieceTeam;
	PieceType pieceType;
	Vec2i srcCell;
	Vec2i destCell;

	Piece *pieceToHide;
};

struct Game {
	Texture *gameTexture;

	Globals globals;
	float time;
	Vec2 size;

	Vec2 boardOffset;
	Vec2 cellSize;

	Piece *heldPiece;

	Match match;

#define EFFECTS_MAX 128
	Effect effects[EFFECTS_MAX];
	int effectsNum;

#define FEN_HISTORY_MAX 128
	char fenHistory[FEN_HISTORY_MAX][FEN_MAX_LEN];
	int fenHistoryNum;

	bool freePlacement;
	bool disableAI;

	bool debugMode;
	char newFen[FEN_MAX_LEN];
};

Game *game = NULL;

void runGame();
void updateGame();
void setupBoard(Match *match, char *str);
bool canMovePiece(Match *match, Piece *piece, Vec2i newPosition);
Piece *getPieceOnCell(Match *match, Vec2i position);
int getPieceIndex(Match *match, Piece *piece);
void getBestMove(Match *match, Vec2i *srcCell, Vec2i *destCell);
bool isCellUnderAttack(Match *match, Vec2i cell, Team teamBeingAttacked);
bool isPlayerInCheck(Match *match, Team team);
bool isPlayerInCheckMate(Match *match, Team team);
void movePiece(Match *match, Piece *piece, Vec2i destCell, bool generateEffect);
Piece *getPieceOfType(Match *match, Team team, PieceType type);
Rect getCellRect(Vec2i position);
/// FUNCTIONS ^

void runGame() {
#if defined(_WIN32)
#if !defined(FALLOW_INTERNAL) // This needs to be a macro
	snprintf(projectAssetDir, PATH_MAX_LEN, "%s", exeDir);
#else
	// if (directoryExists("C:/Dropbox")) strcpy(projectAssetDir, "C:/Dropbox/???");
#endif
#endif

	initFileOperations();

	initPlatform(1280, 720, "A game?");
	platform->sleepWait = true;
	initAudio();
	initRenderer(1280, 720);
	initFonts();
	initTextureSystem();

	platformUpdateLoop(updateGame);
}

void updateGame() {
	if (!game) {
		game = (Game *)zalloc(sizeof(Game));

		// RegMem(Globals, );
		// loadStruct("Globals", "assets/info/globals.txt", &game->globals);
		maximizeWindow();

		game->cellSize = v2(80, 80);
	}

	game->size = v2(platform->windowWidth, platform->windowHeight);

	Globals *globals = &game->globals;
	float elapsed = platform->elapsed;
	float secondPhase = timePhase(platform->time);
	Vec2 mouse = platform->mouse;
	Match *match = &game->match;

	{ /// Debug
		ImGui::Begin("Debug", NULL, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Checkbox("Free placement", &game->freePlacement);
		ImGui::Checkbox("Disable ai", &game->disableAI);
		ImGui::Text("%s's turn", teamStrings[match->turnTeam]);

		ImGui::PushItemWidth(400);
		ImGui::InputText("Current game", match->currentFen, FEN_MAX_LEN);

		ImGui::InputText("New game", game->newFen, FEN_MAX_LEN);
		ImGui::SameLine();
		if (ImGui::Button("Start")) {
			setupBoard(match, game->newFen);
		}

		ImGui::PopItemWidth();

		if (ImGui::Button("Rook fort")) {
			setupBoard(match, "2rrkr2/2r2r2/8/8/8/8/PPPPPPPP/RNBQKBNR w KQ - 0 1");
		}

		if (ImGui::Button("Step back once") || keyJustPressed(KEY_BACKSPACE)) {
			if (game->fenHistoryNum > 0) {
				setupBoard(match, game->fenHistory[game->fenHistoryNum-1]);
			} else if (fileExists("assets/info/lastBoard.txt")) {
				char *str = (char *)readFile("assets/info/lastBoard.txt");
				setupBoard(match, str);
				free(str);
			} else {
				logf("Can't go back anymore...\n");
			}
		}
		ImGui::Text("Fen histroy:");
		for (int i = 0; i < game->fenHistoryNum; i++) {
			ImGui::PushID(i);
			ImGui::Text(game->fenHistory[i]);
			ImGui::SameLine();
			if (ImGui::Button("Go back")) {
				setupBoard(match, game->fenHistory[i]);
				game->fenHistoryNum = i;
			}
			ImGui::PopID();
		}

		ImGui::End();
	}

	if (keyJustPressed(KEY_BACKTICK)) game->debugMode = !game->debugMode;
	if (game->debugMode) {
	}

	clearRenderer(0xFFFFFFFF);

	if (game->time == 0) {
		setupBoard(match, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
		strcpy(game->newFen, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

#if USING_CHEN
		promotedPieceChar[wN] = 'n';
		promotedPieceChar[wB] = 'b';
		promotedPieceChar[wR] = 'r';
		promotedPieceChar[wQ] = 'q';
		promotedPieceChar[bN] = 'n';
		promotedPieceChar[bB] = 'b';
		promotedPieceChar[bR] = 'r';
		promotedPieceChar[bQ] = 'q';

		CHEN_pieceChars[32] = wP;
		CHEN_pieceChars[30] = wN;
		CHEN_pieceChars[18] = wB;
		CHEN_pieceChars[34] = wR;
		CHEN_pieceChars[33] = wQ;
		CHEN_pieceChars[27] = wK;
		CHEN_pieceChars[64] = bP;
		CHEN_pieceChars[62] = bN;
		CHEN_pieceChars[50] = bB;
		CHEN_pieceChars[66] = bR;
		CHEN_pieceChars[65] = bQ;
		CHEN_pieceChars[59] = bK;
#else
    bb_init();
    prng_seed(time(NULL));
#endif
	}

	float boardSize = game->cellSize.x * 8;
	game->boardOffset = game->size/2 - v2(boardSize, boardSize)/2;

	Rect boardScreenRect = makeRect(game->boardOffset, v2(boardSize, boardSize));
	Vec2 boardMouse = boardScreenRect.inflate(-2).getClosestPoint(mouse) - game->boardOffset;
	Vec2i hoveredCell;
	hoveredCell.x = boardMouse.x / game->cellSize.x;
	hoveredCell.y = boardMouse.y / game->cellSize.y;

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			int cellIndex = y * 8 + x;
			Vec2i vec = v2i(x, y);
			Rect rect = getCellRect(vec);

			bool flipColors = false;
			if (cellIndex % 2 != 0) flipColors = true;
			if (y % 2 != 0) flipColors = !flipColors;
			int color = 0xFFF0D9B5;
			if (flipColors) color = 0xFFB58863;

			drawRect(rect, color);

			if (equal(vec, hoveredCell)) {
				drawRectOutline(rect.inflate(-5), 2, 0xFFFFFFFF);
			}
		}
	}

	auto getPieceTexture = [](Team team, PieceType type)->Texture * {
		if (team == TEAM_BLACK && type == PIECE_ROOK) return getTexture("assets/images/br.png");
		if (team == TEAM_BLACK && type == PIECE_KNIGHT) return getTexture("assets/images/bn.png");
		if (team == TEAM_BLACK && type == PIECE_BISHOP) return getTexture("assets/images/bb.png");
		if (team == TEAM_BLACK && type == PIECE_QUEEN) return getTexture("assets/images/bq.png");
		if (team == TEAM_BLACK && type == PIECE_KING) return getTexture("assets/images/bk.png");
		if (team == TEAM_BLACK && type == PIECE_PAWN) return getTexture("assets/images/bp.png");
		if (team == TEAM_WHITE && type == PIECE_ROOK) return getTexture("assets/images/wr.png");
		if (team == TEAM_WHITE && type == PIECE_KNIGHT) return getTexture("assets/images/wn.png");
		if (team == TEAM_WHITE && type == PIECE_BISHOP) return getTexture("assets/images/wb.png");
		if (team == TEAM_WHITE && type == PIECE_QUEEN) return getTexture("assets/images/wq.png");
		if (team == TEAM_WHITE && type == PIECE_KING) return getTexture("assets/images/wk.png");
		if (team == TEAM_WHITE && type == PIECE_PAWN) return getTexture("assets/images/wp.png");
		return NULL;
	};

	for (int i = 0; i < match->piecesNum; i++) {
		Piece *piece = &match->pieces[i];
		Rect rect = getCellRect(piece->position);
		Vec2 screenPosition = getCenter(rect);

		if (contains(rect, mouse)) {
			if (platform->mouseJustDown && !game->heldPiece) {
				game->heldPiece = piece;
			}
		}

		if (piece == game->heldPiece) screenPosition = mouse;

		bool shouldDraw = true;
		for (int i = 0; i < game->effectsNum; i++) {
			Effect *effect = &game->effects[i];
			if (effect->pieceToHide == piece) shouldDraw = false;
		}

		if (shouldDraw) {
			Texture *texture = getPieceTexture(piece->team, piece->type);
			RenderProps props = newRenderProps();
			props.matrix.TRANSLATE(screenPosition - getSize(texture)/2);
			drawTexture(texture, props);
		}
	}

	if (game->heldPiece) {
		for (int y = 0; y < 8; y++) {
			for (int x = 0; x < 8; x++) {
				Vec2i cell = v2i(x, y);

				int color;
				if (canMovePiece(match, game->heldPiece, cell)) {
					color = 0x4000FF00;
				} else {
					color = 0x40FF0000;
				}

				Rect rect = getCellRect(cell);
				drawRect(rect, color);
			}
		}
	}

	bool playMade = false;
	if (!platform->mouseDown) {
		if (game->heldPiece) {
			Piece *piece = game->heldPiece;

			bool canMove = canMovePiece(match, piece, hoveredCell);
			if (piece->team != match->turnTeam) canMove = false;
			if (game->freePlacement) canMove = true;

			if (canMove) {
				Match oldMatch = *match;
				movePiece(match, piece, hoveredCell, false);

				if (isPlayerInCheck(match, match->turnTeam)) {
					logf("Can't make move, you'd be in check!\n");
					canMove = false;
				}

				*match = oldMatch;
			}

			if (canMove) {
				strcpy(game->fenHistory[game->fenHistoryNum], match->currentFen);
				game->fenHistoryNum++;
				if (platform->isDebugVersion) writeFile("assets/info/lastBoard.txt", match->currentFen, strlen(match->currentFen)+1);
				// logf("FEN before this move: %s\n", match->currentFen);
				movePiece(match, piece, hoveredCell, false);
				playMade = true;
				match->turnTeam = (match->turnTeam == TEAM_BLACK) ? TEAM_WHITE : TEAM_BLACK;
			}

			game->heldPiece = NULL;
		}
	}

	{
		char *str = match->currentFen;
		int strNum = 0;

		for (int y = 0; y < 8; y++) {
			int blanks = 0;
			for (int x = 0; x < 8; x++) {
				Vec2i cell = v2i(x, y);
				Piece *piece = getPieceOnCell(match, cell);
				if (!piece) {
					blanks++;
					continue;
				}

				if (blanks) {
					str[strNum++] = '0'+blanks;
					blanks = 0;
				}
				char ch = pieceTypeChars[piece->type];
				if (piece->team == TEAM_WHITE) ch -= 'z'-'Z';
				str[strNum++] = ch;
			}

			if (blanks) str[strNum++] = '0'+blanks;
			if (y != 7) str[strNum++] = '/';
		}

		str[strNum++] = ' ';
		if (match->turnTeam == TEAM_BLACK) str[strNum++] = 'b';
		else str[strNum++] = 'w';

		str[strNum++] = ' ';
		if (match->canLongCastle[0] == false && match->canLongCastle[1] == false && match->canShortCastle[0] == false && match->canShortCastle[1] == false) {
			str[strNum++] = '-';
		} else {
			if (match->canShortCastle[TEAM_WHITE]) str[strNum++] = 'K';
			if (match->canLongCastle[TEAM_WHITE]) str[strNum++] = 'Q';
			if (match->canShortCastle[TEAM_BLACK]) str[strNum++] = 'k';
			if (match->canLongCastle[TEAM_BLACK]) str[strNum++] = 'q';
		}

		str[strNum++] = ' ';
		str[strNum++] = '-'; // En passant

		str[strNum++] = ' ';
		str[strNum++] = '0';
		str[strNum++] = ' ';
		str[strNum++] = '1';

		str[strNum] = 0;
	}

	if (!game->disableAI && match->turnTeam == TEAM_BLACK) {
		Vec2i srcCell;
		Vec2i destCell;
		getBestMove(match, &srcCell, &destCell);

		bool canMove = true;
		if (equal(srcCell, destCell)) {
			logf("AI tried to move to the same space!\n");
			canMove = false;
		}

		if (canMove) {
			Piece *piece = getPieceOnCell(match, srcCell);
			movePiece(match, piece, destCell, true);
			playMade = true;
			//@incomplete Piece promotion
		}
		match->turnTeam = TEAM_WHITE;
	}

	if (isPlayerInCheckMate(match, TEAM_WHITE)) {
		if (platform->frameCount % 120 == 0) logf("Game over, black wins\n");
	}

	if (isPlayerInCheckMate(match, TEAM_BLACK)) {
		if (platform->frameCount % 120 == 0) logf("Game over, black wins\n");
	}

	for (int i = 0; i < game->effectsNum; i++) {
		Effect *effect = &game->effects[i];
		float maxTime = 1;

		if (effect->type == EFFECT_SLIDE) {
			maxTime = 0.1;
			float perc = effect->time / maxTime;

			Texture *texture = getPieceTexture(effect->pieceTeam, effect->pieceType);
			Vec2 srcPos = getCenter(getCellRect(effect->srcCell));
			Vec2 destPos = getCenter(getCellRect(effect->destCell));
			Vec2 pos = lerp(srcPos, destPos, perc);

			RenderProps props = newRenderProps();
			props.matrix.TRANSLATE(pos - getSize(texture)/2);
			drawTexture(texture, props);
		}

		if (effect->type == EFFECT_IN_CHECK) {
			maxTime = 1;
			Rect cell = getCellRect(effect->srcCell);
			drawRectOutline(cell, 8, lerpColor(0x00FF0000, 0xFFFF0000, timePhase(effect->time*4)));
		}

		effect->time += elapsed;
		if (effect->time > maxTime) {
			arraySpliceIndex(game->effects, game->effectsNum, sizeof(Effect), i);
			game->effectsNum--;
			i--;
			continue;
		}
	}

	float fps = 1.0/(float)platform->frameTimeAvg * 1000.0;
	char *str = frameSprintf("Fps: %.1f (%.1fms)", fps, platform->frameTimeAvg);
	drawText(fontSys->defaultFont, str, v2(), 0xFF808080);

	game->time += elapsed;

	drawOnScreenLog();
}

void setupBoard(Match *match, char *str) {
	match->piecesNum = 0;
	match->canShortCastle[0] = match->canShortCastle[1] = match->canLongCastle[0] = match->canLongCastle[1] = false;

	int strIndex = 0;

	Vec2i cell = v2i();
	char ch;
	for (;;) {
		ch = str[strIndex++];
		if (ch >= '1' && ch <= '8') {
			cell.x += ch - '0';
		} else if (ch == '/') {
			cell.x = 0;
			cell.y++;
		} else {
			for (int i = 0; i < ArrayLength(pieceTypeChars); i++) {
				Team team = TEAM_NONE;
				if (ch == pieceTypeChars[i]) team = TEAM_BLACK;
				if (ch == pieceTypeChars[i] - ('z'-'Z')) team = TEAM_WHITE;
				if (team != TEAM_NONE) {
					Piece *piece = &match->pieces[match->piecesNum++];
					piece->type = (PieceType)i;
					piece->team = team;
					piece->position = cell;
					cell.x++;
				}
			}
		}

		if (ch == ' ') break;
	}

	ch = str[strIndex++];
	if (ch == 'w') match->turnTeam = TEAM_WHITE;
	else if (ch == 'b') match->turnTeam = TEAM_WHITE;
	else logf("Bad FEN (line %d)", __LINE__);

	ch = str[strIndex++];
	if (ch != ' ') logf("Bad FEN (line %d)", __LINE__);

	for (;;) {
		ch = str[strIndex++];
		if (ch == '-') {
			strIndex++; // Eat space
			break;
		}
		if (ch == ' ') break;
		if (ch == 'K') match->canShortCastle[TEAM_WHITE] = true;
		if (ch == 'Q') match->canLongCastle[TEAM_WHITE] = true;
		if (ch == 'k') match->canShortCastle[TEAM_BLACK] = true;
		if (ch == 'q') match->canLongCastle[TEAM_BLACK] = true;
	}

	ch = str[strIndex++];
	if (ch != '-') logf("En passant not supported yet!\n");
};


#define CheckCellAndBreakIfContainsPieceAlsoSetSpaceAsAllowedIfEnemy(cell) { \
	Piece *otherPiece = getPieceOnCell(match, (cell)); \
	if (otherPiece) { \
		if (otherPiece->team != piece->team) allowedSpaces[allowedSpacesNum++] = (cell); \
		break; \
	} \
	allowedSpaces[allowedSpacesNum++] = (cell); \
}

bool canMovePiece(Match *match, Piece *piece, Vec2i newPosition) {
	Vec2i *allowedSpaces = (Vec2i *)frameMalloc(sizeof(Vec2i) * 64);
	int allowedSpacesNum = 0;

	if (piece->type == PIECE_PAWN) {
		//@incomplete En passant
		if (piece->team == TEAM_BLACK) {
			if (!getPieceOnCell(match, piece->position + v2i(0, 1))) allowedSpaces[allowedSpacesNum++] = piece->position + v2i(0, 1);
			if (piece->position.y == 1 && !getPieceOnCell(match, piece->position + v2i(0, 1))) allowedSpaces[allowedSpacesNum++] = piece->position + v2i(0, 2);

			Vec2i d0 = piece->position + v2i(-1, 1);
			Vec2i d1 = piece->position + v2i(1, 1);
			if (getPieceOnCell(match, d0) && getPieceOnCell(match, d0)->team != piece->team) allowedSpaces[allowedSpacesNum++] = d0;
			if (getPieceOnCell(match, d1) && getPieceOnCell(match, d1)->team != piece->team) allowedSpaces[allowedSpacesNum++] = d1;
		}

		if (piece->team == TEAM_WHITE) {
			if (!getPieceOnCell(match, piece->position + v2i(0, -1))) allowedSpaces[allowedSpacesNum++] = piece->position + v2i(0, -1);
			if (piece->position.y == 6 && !getPieceOnCell(match, piece->position + v2i(0, -1))) allowedSpaces[allowedSpacesNum++] = piece->position + v2i(0, -2);

			Vec2i d0 = piece->position + v2i(-1, -1);
			Vec2i d1 = piece->position + v2i(1, -1);
			if (getPieceOnCell(match, d0) && getPieceOnCell(match, d0)->team != piece->team) allowedSpaces[allowedSpacesNum++] = d0;
			if (getPieceOnCell(match, d1) && getPieceOnCell(match, d1)->team != piece->team) allowedSpaces[allowedSpacesNum++] = d1;
		}
	}

	if (piece->type == PIECE_KNIGHT) {
		allowedSpaces[allowedSpacesNum++] = piece->position + v2i(1, 2);
		allowedSpaces[allowedSpacesNum++] = piece->position + v2i(-1, 2);
		allowedSpaces[allowedSpacesNum++] = piece->position + v2i(1, -2);
		allowedSpaces[allowedSpacesNum++] = piece->position + v2i(-1, -2);
		allowedSpaces[allowedSpacesNum++] = piece->position + v2i(2, 1);
		allowedSpaces[allowedSpacesNum++] = piece->position + v2i(-2, 1);
		allowedSpaces[allowedSpacesNum++] = piece->position + v2i(2, -1);
		allowedSpaces[allowedSpacesNum++] = piece->position + v2i(-2, -1);
	}

	bool addRookMoves = false;
	bool addBishopRules = false;
	if (piece->type == PIECE_ROOK) addRookMoves = true;
	if (piece->type == PIECE_BISHOP) addBishopRules = true;
	if (piece->type == PIECE_QUEEN) addRookMoves = addBishopRules = true;

	if (addRookMoves) {
		for (int x = piece->position.x-1; x >= 0; x--) CheckCellAndBreakIfContainsPieceAlsoSetSpaceAsAllowedIfEnemy(v2i(x, piece->position.y));
		for (int x = piece->position.x+1; x < 8; x++) CheckCellAndBreakIfContainsPieceAlsoSetSpaceAsAllowedIfEnemy(v2i(x, piece->position.y));
		for (int y = piece->position.y-1; y >= 0; y--) CheckCellAndBreakIfContainsPieceAlsoSetSpaceAsAllowedIfEnemy(v2i(piece->position.x, y));
		for (int y = piece->position.y+1; y < 8; y++) CheckCellAndBreakIfContainsPieceAlsoSetSpaceAsAllowedIfEnemy(v2i(piece->position.x, y));
	}

	if (addBishopRules) {
		for (int i = 1; i < 8; i++) CheckCellAndBreakIfContainsPieceAlsoSetSpaceAsAllowedIfEnemy(piece->position + v2i(i, i));
		for (int i = 1; i < 8; i++) CheckCellAndBreakIfContainsPieceAlsoSetSpaceAsAllowedIfEnemy(piece->position + v2i(-i, i));
		for (int i = 1; i < 8; i++) CheckCellAndBreakIfContainsPieceAlsoSetSpaceAsAllowedIfEnemy(piece->position + v2i(i, -i));
		for (int i = 1; i < 8; i++) CheckCellAndBreakIfContainsPieceAlsoSetSpaceAsAllowedIfEnemy(piece->position + v2i(-i, -i));
	}

	if (piece->type == PIECE_KING) {
		allowedSpaces[allowedSpacesNum++] = piece->position + v2i(1, 0);
		allowedSpaces[allowedSpacesNum++] = piece->position + v2i(0, 1);
		allowedSpaces[allowedSpacesNum++] = piece->position + v2i(-1, 0);
		allowedSpaces[allowedSpacesNum++] = piece->position + v2i(0, -1);
		allowedSpaces[allowedSpacesNum++] = piece->position + v2i(1, 1);
		allowedSpaces[allowedSpacesNum++] = piece->position + v2i(-1, 1);
		allowedSpaces[allowedSpacesNum++] = piece->position + v2i(1, -1);
		allowedSpaces[allowedSpacesNum++] = piece->position + v2i(-1, -1);

		//@incomplete Check squares inbetween for check
		//@incomplete Make sure king isn't in check
		if (match->canShortCastle[piece->team]) {
			Piece *rook = getPieceOnCell(match, piece->position + v2i(3, 0));
			if (rook && rook->type == PIECE_ROOK && rook->team == piece->team) {
				Vec2i test0 = piece->position + v2i(1, 0);
				Vec2i test1 = piece->position + v2i(2, 0);
				if (!getPieceOnCell(match, test0) && !getPieceOnCell(match, test1)) {
					allowedSpaces[allowedSpacesNum++] = test1;
				}
			}
		}
		if (match->canLongCastle[piece->team]) {
			Piece *rook = getPieceOnCell(match, piece->position + v2i(-4, 0));
			if (rook && rook->type == PIECE_ROOK && rook->team == piece->team) {
				Vec2i test0 = piece->position + v2i(-1, 0);
				Vec2i test1 = piece->position + v2i(-2, 0);
				Vec2i test2 = piece->position + v2i(-3, 0);
				if (!getPieceOnCell(match, test0) && !getPieceOnCell(match, test1) && !getPieceOnCell(match, test2)) {
					allowedSpaces[allowedSpacesNum++] = test1;
				}
			}
		}
	}

	for (int i = 0; i < allowedSpacesNum; i++) {
		Vec2i cell = allowedSpaces[i];
		bool shouldRemove = false;
		if (cell.x < 0 || cell.x > 8) shouldRemove = true;
		if (cell.y < 0 || cell.y > 8) shouldRemove = true;

		Piece *otherPiece = getPieceOnCell(match, cell);
		if (otherPiece && otherPiece->team == piece->team) shouldRemove = true;

		if (shouldRemove) {
			arraySpliceIndex(allowedSpaces, allowedSpacesNum, sizeof(Vec2i), i);
			allowedSpacesNum--;
			i--;
			continue;
		}
	}

	bool allowed = false;
	for (int i = 0; i < allowedSpacesNum; i++) {
		Vec2i cell = allowedSpaces[i];
		if (equal(cell, newPosition)) {
			allowed = true;
			break;
		}
	}

	if (!allowed) return false;

	return true;
}

Piece *getPieceOnCell(Match *match, Vec2i position) {
	if (position.x < 0 || position.x > 7) return NULL;
	if (position.y < 0 || position.y > 7) return NULL;

	for (int i = 0; i < match->piecesNum; i++) {
		Piece *piece = &match->pieces[i];
		if (equal(piece->position, position)) return piece;
	}

	return NULL;
}

int getPieceIndex(Match *match, Piece *piece) {
	for (int i = 0; i < match->piecesNum; i++) {
		if (piece == &match->pieces[i]) return i;
	}

	logf("Couldn't find piece index\n");
	return -1;
}

void getBestMove(Match *match, Vec2i *srcCell, Vec2i *destCell) {
#if USING_CHEN
	CHESSBOARD board[1];
	SEARCH search[1];

	CHEN_InitSearch(search);

	CHEN_ParseFen(board, match->currentFen);
	CHEN_PrintBoard(board);
	CHEN_SearchPosition(board, search, 6);

	u32 src = GetMoveSource(search->bestMove);
	u32 dest = GetMoveTarget(search->bestMove);
	*srcCell = v2i(GetFile(src), 7-GetRank(src));
	*destCell = v2i(GetFile(dest), 7-GetRank(dest));
#else
	Board board = {};
	board_reset(&board);
	board_load_fen(&board, match->currentFen);
	board_print(&board);

	Search search = {};
	search.use_book = 0;
	search.duration = 1;
	do_search(&search, &board);

	Move *move = &search.move;
	srcCell->x = move->src % 8;
	srcCell->y = 7 - (move->src / 8);
	destCell->x = move->dst % 8;
	destCell->y = 7 - (move->dst / 8);

	// char moveString[128] = {};
	// move_to_string(move, moveString);
	// logf("MoveString is: %s\n", moveString);
	// logf("Really its %d -> %d (%x %x)\n", move->src, move->dst, move->src, move->dst);
	// logf("I think it's %d,%d -> %d,%d\n", srcCell->x, srcCell->y, destCell->x, destCell->y);
#endif
}

bool isPlayerInCheck(Match *match, Team team) {
	Piece *king = getPieceOfType(match, team, PIECE_KING);
	if (!king) {
		logf("No king, wtf?\n");
		return false;
	}

	if (isCellUnderAttack(match, king->position, team)) return true;

	return false;
}

bool isPlayerInCheckMate(Match *match, Team team) {
	if (!isPlayerInCheck(match, team)) return false;

	Piece *king = getPieceOfType(match, team, PIECE_KING);
	if (!king) {
		logf("No king, wtf?\n");
		return false;
	}

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			Vec2i cell = v2i(x, y);
			if (canMovePiece(match, king, cell)) {
				Match oldMatch = *match;
				movePiece(match, king, cell, false);
				if (!isCellUnderAttack(match, cell, team)) {
					*match = oldMatch;
					return false;
				} else {
					*match = oldMatch;
				}
			}
		}
	}

	return true;
}

bool isCellUnderAttack(Match *match, Vec2i cell, Team teamBeingAttacked) {
	for (int i = 0; i < match->piecesNum; i++) {
		Piece *piece = &match->pieces[i];
		if (piece->team != teamBeingAttacked) {
			if (canMovePiece(match, piece, cell)) {
				return true;
			}
		}
	}

	return false;
}

void movePiece(Match *match, Piece *piece, Vec2i destCell, bool generateEffect) {
	float xDist = destCell.x - piece->position.x;
	if (piece->type == PIECE_KING) {
		match->canShortCastle[piece->team] = false;
		match->canLongCastle[piece->team] = false;

		if (abs(xDist) == 2) {
			if (xDist == 2) {
				Piece *rook = getPieceOnCell(match, piece->position + v2i(3, 0));
				rook->position.x -= 2;
			} else {
				Piece *rook = getPieceOnCell(match, piece->position + v2i(-4, 0));
				rook->position.x += 3;
			}
		}
	}

	if (piece->type == PIECE_ROOK) {
		if (equal(piece->position, v2i(0, 0))) match->canLongCastle[TEAM_BLACK] = false;
		if (equal(piece->position, v2i(7, 0))) match->canShortCastle[TEAM_BLACK] = false;
		if (equal(piece->position, v2i(0, 7))) match->canLongCastle[TEAM_WHITE] = false;
		if (equal(piece->position, v2i(7, 7))) match->canShortCastle[TEAM_WHITE] = false;
	}

	Vec2i srcCell = piece->position;
	piece->position = destCell;

	int killedPieceIndex = -1;
	for (int i = 0; i < match->piecesNum; i++) {
		Piece *otherPiece = &match->pieces[i];
		if (equal(otherPiece->position, destCell)) {
			if (piece != otherPiece) {
				arraySpliceIndex(match->pieces, match->piecesNum, sizeof(Piece), i);
				match->piecesNum--;
				break;
			}
		}
	}

	if (generateEffect) {
		Effect *effect = &game->effects[game->effectsNum++]; //@robustness
		memset(effect, 0, sizeof(Effect));
		effect->type = EFFECT_SLIDE;
		effect->pieceToHide = piece;
		effect->srcCell = srcCell;
		effect->destCell = destCell;
		effect->pieceTeam = piece->team;
		effect->pieceType = piece->type;

		Team otherTeam = match->turnTeam == TEAM_WHITE ? TEAM_BLACK : TEAM_WHITE;
		if (isPlayerInCheck(match, otherTeam)) {
			Piece *king = getPieceOfType(match, otherTeam, PIECE_KING);
			if (king) {
				Effect *effect = &game->effects[game->effectsNum++]; //@robustness
				memset(effect, 0, sizeof(Effect));
				effect->type = EFFECT_IN_CHECK;
				effect->srcCell = king->position;
			}
		}
	}
}

Piece *getPieceOfType(Match *match, Team team, PieceType type) {
	for (int i = 0; i < match->piecesNum; i++) {
		Piece *piece = &match->pieces[i];
		if (piece->team == team && piece->type == type) return piece;
	}

	return NULL;
}

Rect getCellRect(Vec2i position) {
	Vec2 pos = (v2(position.x, position.y) * game->cellSize) + game->boardOffset;
	Rect rect = makeRect(pos, game->cellSize);
	return rect;
}
