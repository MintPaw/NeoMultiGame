struct Ngui {

};
Ngui *ngui = NULL;

void nguiInit();
/// FUNCTIONS ^


void nguiInit() {
	ngui = (Ngui *)zalloc(sizeof(Ngui));
}
