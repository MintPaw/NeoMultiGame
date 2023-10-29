bool saveLoadVersionCheck(int version, int startVersion, int endVersion, bool save);
bool saveLoadBool(DataStream *stream, bool save, int version, bool *ptr, int startVersion, int endVersion);
bool saveLoadU32(DataStream *stream, bool save, int version, u32 *ptr, int startVersion, int endVersion);
bool saveLoadU64(DataStream *stream, bool save, int version, u32 *ptr, int startVersion, int endVersion);
bool saveLoadS64(DataStream *stream, bool save, int version, s64 *ptr, int startVersion, int endVersion);
bool saveLoadInt(DataStream *stream, bool save, int version, int *ptr, int startVersion, int endVersion);
bool saveLoadInt64To32(DataStream *stream, bool save, int version, int *ptr, int startVersion, int endVersion);
bool saveLoadFloat(DataStream *stream, bool save, int version, float *ptr, int startVersion, int endVersion);
bool saveLoadVec2(DataStream *stream, bool save, int version, Vec2 *ptr, int startVersion, int endVersion);
bool saveLoadVec3(DataStream *stream, bool save, int version, Vec3 *ptr, int startVersion, int endVersion);
bool saveLoadVec4(DataStream *stream, bool save, int version, Vec4 *ptr, int startVersion, int endVersion);
bool saveLoadRect(DataStream *stream, bool save, int version, Rect *ptr, int startVersion, int endVersion);
bool saveLoadStringInto(DataStream *stream, bool save, int version, char *ptr, int maxLen, int startVersion, int endVersion);
bool saveLoadString(DataStream *stream, bool save, int version, char **ptr, int startVersion, int endVersion);
bool saveLoadJaiString(DataStream *stream, bool save, int version, char **ptr, int startVersion, int endVersion);
bool saveLoadBytes(DataStream *stream, bool save, int version, void *ptr, int size, int startVersion, int endVersion);
/// FUNCTIONS ^

bool saveLoadVersionCheck(int version, int startVersion, int endVersion, bool save) {
	// save arg doesn't matter except for logging
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

bool saveLoadBool(DataStream *stream, bool save, int version, bool *ptr, int startVersion, int endVersion) {
	if (!saveLoadVersionCheck(version, startVersion, endVersion, save)) return false;
	if (save) writeU8(stream, *ptr);
	else *ptr = readU8(stream);
	return true;
}

bool saveLoadU32(DataStream *stream, bool save, int version, u32 *ptr, int startVersion, int endVersion) {
	if (!saveLoadVersionCheck(version, startVersion, endVersion, save)) return false;
	if (save) writeU32(stream, *ptr);
	else *ptr = readU32(stream);
	return true;
}

bool saveLoadU64(DataStream *stream, bool save, int version, u64 *ptr, int startVersion, int endVersion) {
	if (!saveLoadVersionCheck(version, startVersion, endVersion, save)) return false;
	if (save) writeU64(stream, *ptr);
	else *ptr = readU64(stream);
	return true;
}

bool saveLoadS64(DataStream *stream, bool save, int version, s64 *ptr, int startVersion, int endVersion) {
	return saveLoadU64(stream, save, version, (u64 *)ptr, startVersion, endVersion);
}

bool saveLoadInt(DataStream *stream, bool save, int version, int *ptr, int startVersion, int endVersion) {
	if (!saveLoadVersionCheck(version, startVersion, endVersion, save)) return false;
	if (save) writeU32(stream, *ptr);
	else *ptr = readU32(stream);
	return true;
}

bool saveLoadInt64To32(DataStream *stream, bool save, int version, int *ptr, int startVersion, int endVersion) {
	if (!saveLoadVersionCheck(version, startVersion, endVersion, save)) return false;
	if (save) {
    writeU32(stream, *ptr);
    writeU32(stream, 0);
  } else {
    *ptr = readU32(stream);
    readU32(stream);
  }
	return true;
}

bool saveLoadFloat(DataStream *stream, bool save, int version, float *ptr, int startVersion, int endVersion) {
	if (!saveLoadVersionCheck(version, startVersion, endVersion, save)) return false;
	if (save) writeFloat(stream, *ptr);
	else *ptr = readFloat(stream);
	return true;
}

bool saveLoadVec2(DataStream *stream, bool save, int version, Vec2 *ptr, int startVersion, int endVersion) {
	if (!saveLoadVersionCheck(version, startVersion, endVersion, save)) return false;
	if (save) writeVec2(stream, *ptr);
	else *ptr = readVec2(stream);
	return true;
}

bool saveLoadVec3(DataStream *stream, bool save, int version, Vec3 *ptr, int startVersion, int endVersion) {
	if (!saveLoadVersionCheck(version, startVersion, endVersion, save)) return false;
	if (save) writeVec3(stream, *ptr);
	else *ptr = readVec3(stream);
	return true;
}

bool saveLoadVec4(DataStream *stream, bool save, int version, Vec4 *ptr, int startVersion, int endVersion) {
	if (!saveLoadVersionCheck(version, startVersion, endVersion, save)) return false;
	if (save) writeVec4(stream, *ptr);
	else *ptr = readVec4(stream);
	return true;
}

bool saveLoadRect(DataStream *stream, bool save, int version, Rect *ptr, int startVersion, int endVersion) {
	if (!saveLoadVersionCheck(version, startVersion, endVersion, save)) return false;
	if (save) writeRect(stream, *ptr);
	else *ptr = readRect(stream);
	return true;
}

bool saveLoadStringInto(DataStream *stream, bool save, int version, char *ptr, int maxLen, int startVersion, int endVersion) {
	if (!saveLoadVersionCheck(version, startVersion, endVersion, save)) return false;
	if (save) writeString(stream, ptr);
	else readStringInto(stream, ptr, maxLen);
	return true;
}

bool saveLoadString(DataStream *stream, bool save, int version, char **ptr, int startVersion, int endVersion) {
	if (!saveLoadVersionCheck(version, startVersion, endVersion, save)) return false;
	if (save) writeString(stream, *ptr);
	else *ptr = readString(stream);
	return true;
}

bool saveLoadJaiString(DataStream *stream, bool save, int version, char **ptr, int startVersion, int endVersion) {
	if (!saveLoadVersionCheck(version, startVersion, endVersion, save)) return false;
	if (save) writeJaiString(stream, *ptr);
	else *ptr = readJaiString(stream);
	return true;
}

bool saveLoadBytes(DataStream *stream, bool save, int version, void *ptr, int size, int startVersion, int endVersion) {
	if (!saveLoadVersionCheck(version, startVersion, endVersion, save)) return false;
	if (save) writeBytes(stream, ptr, size);
	else readBytes(stream, ptr, size);
	return true;
}
