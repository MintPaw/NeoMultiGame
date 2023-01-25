bool saveLoadVersionCheck(int version, int startVersion, int endVersion, bool save);
bool versionSaveLoadU32(DataStream *stream, bool save, int version, u32 *ptr, int startVersion, int endVersion);
bool versionSaveLoadInt(DataStream *stream, bool save, int version, int *ptr, int startVersion, int endVersion);
bool versionSaveLoadFloat(DataStream *stream, bool save, int version, float *ptr, int startVersion, int endVersion);
bool versionSaveLoadVec2(DataStream *stream, bool save, int version, Vec2 *ptr, int startVersion, int endVersion);
/// FUNCTIONS ^

bool saveLoadVersionCheck(int version, int startVersion, int endVersion, bool save) {
	// save doesn't matter
	if (startVersion > version) {
		logf("Member variable starts in the future? (%d-%d, %d) [%s]\n", startVersion, endVersion, version, save?"save":"load");
		return false;
	}
	if (endVersion < version) {
		logf("Member variable is gone now. (%d-%d, %d) [%s]\n", startVersion, endVersion, version, save?"save":"load");
		return false;
	}

	return true;
}

bool versionSaveLoadU32(DataStream *stream, bool save, int version, u32 *ptr, int startVersion, int endVersion) {
	if (!saveLoadVersionCheck(version, startVersion, endVersion, save)) return false;
	if (save) writeU32(stream, *ptr);
	else *ptr = readU32(stream);
	return true;
}

bool versionSaveLoadInt(DataStream *stream, bool save, int version, int *ptr, int startVersion, int endVersion) {
	if (!saveLoadVersionCheck(version, startVersion, endVersion, save)) return false;
	if (save) writeU32(stream, *ptr);
	else *ptr = readU32(stream);
	return true;
}

bool versionSaveLoadFloat(DataStream *stream, bool save, int version, float *ptr, int startVersion, int endVersion) {
	if (!saveLoadVersionCheck(version, startVersion, endVersion, save)) return false;
	if (save) writeFloat(stream, *ptr);
	else *ptr = readFloat(stream);
	return true;
}

bool versionSaveLoadVec2(DataStream *stream, bool save, int version, Vec2 *ptr, int startVersion, int endVersion) {
	if (!saveLoadVersionCheck(version, startVersion, endVersion, save)) return false;
	if (save) writeVec2(stream, *ptr);
	else *ptr = readVec2(stream);
	return true;
}
