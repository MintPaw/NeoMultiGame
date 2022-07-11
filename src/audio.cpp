#include "stb_vorbis.c"

#include <AL/al.h>
#include <AL/alc.h>

#ifdef __EMSCRIPTEN__
# define SAMPLE_BUFFER_LIMIT (2048*2)
#else
# define SAMPLE_BUFFER_LIMIT (2048)
#endif

#define CHANNELS_MAX 512
#define SOUNDS_MAX 4096
#define SAMPLE_RATE 44100

struct Sound {
	bool exists;
	char *path;

	float tweakVolume;
	float length; // In seconds

	u8 *oggData;
	int oggDataLen;

	s16 *samples;
	int samplesStreamed;
	int sampleRate;
	int samplesTotal;
	int channels;

	stb_vorbis_alloc vorbisTempMem;
	stb_vorbis *vorbis;

	ALuint al3dBuffer;

	int concurrentInstances;
	int maxConcurrentInstances;
};

struct Channel {
	int id;
	bool exists;

	Sound *sound;
	int samplePosition;
	float secondPosition;

	float delay;
	float userVolume;
	float userVolume2;
	float pan;
	bool looping;
	Vec2 position;

	/// Private
	float lastVolume;
};

struct SoundSource {
	int id;
	ALuint source;
	Sound *sound;
	bool ui;
};

struct Audio {
	bool disabled;
	bool htmlAllowedAudio;

	Channel channels[CHANNELS_MAX];
	int nextChannelId;
	Sound sounds[SOUNDS_MAX];
	int channelsCount;

	float masterVolume;
	bool doInstantVolumeChanges;

	int memoryUsed;

#define SOUND_SOURCES_MAX 128
	SoundSource *soundSources[SOUND_SOURCES_MAX];
	int soundSourcesNum;
	int nextSoundSourceId;

	char soundStoreNames[SOUNDS_MAX][PATH_MAX_LEN];
	int soundStoreNamesNum;
	Sound *soundStore[SOUNDS_MAX];
	int soundStoreNum;

	int defaultMaxConcurrentInstances;

	ALCdevice *device;
	ALCcontext *context;

	ALuint buffers[2];
	ALuint source;

	s16 *sampleBuffer;
};

Audio *audio = NULL;

void initAudio();

void initSound(Sound *sound);
void updateAudio();
Channel *playSound(Sound *sound, bool looping=false);
void stopChannel(int channelId);
void stopChannel(Channel *channel);
Channel *getChannel(int id);
void seekChannelPerc(Channel *channel, float perc);

Sound *getSound(const char *path, bool onlyLoadRaw=false);
SoundSource *playWorldSound(Vec3 position, char *path);
SoundSource *playUiSound(char *path);
void setPosition(SoundSource *soundSource, Vec3 position);
SoundSource *getSoundSource(int id);

// void updateAudio(); //@hack This is in main.cpp

/// Private
void mixSound(s16 *destBuffer, int destSamplesNum);
void checkAudioError(int lineNum);
#define CheckAudioError() checkAudioError(__LINE__);

void initAudio() {
	// logf("Initing audio (%.2fmb)\n", sizeof(Audio) / 1024.0 / 1024.0);

	audio = (Audio *)zalloc(sizeof(Audio));
	audio->masterVolume = 1;

	if (platform->isCommandLineOnly) {
		audio->disabled = true;
		return;
	}

	audio->device = alcOpenDevice(NULL);
	if (!audio->device) {
		audio->disabled = true;
		logf("OpenAL failed to init! Audio disabled\n");
		return;
	}
	// printf("ALC_DEFAULT_DEVICE_SPECIFIER: %s\n", alcGetString(audio->device, ALC_DEFAULT_DEVICE_SPECIFIER));
	// printf("ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER: %s\n", alcGetString(audio->device, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER));
	// printf("ALC_DEVICE_SPECIFIER: %s\n", alcGetString(audio->device, ALC_DEVICE_SPECIFIER));
	// printf("ALC_CAPTURE_DEVICE_SPECIFIER: %s\n", alcGetString(audio->device, ALC_CAPTURE_DEVICE_SPECIFIER));
	// printf("ALC_EXTENSIONS: %s\n", alcGetString(audio->device, ALC_EXTENSIONS));

	audio->context = alcCreateContext(audio->device, NULL);
	if (!audio->context) {
		audio->disabled = true;
		logf("OpenAL failed to create context! Audio disabled\n");
		return;
	}

	if (!alcMakeContextCurrent(audio->context)) {
		audio->disabled = true;
		logf("OpenAL failed to make context current! Audio disabled\n");
		return;
	}

	CheckAudioError();

	alGenSources(1, &audio->source);

	ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };

	alListener3f(AL_POSITION, 0, 0, 1.0f);
	alListener3f(AL_VELOCITY, 0, 0, 0);
	alListenerfv(AL_ORIENTATION, listenerOri);
	alSourcef(audio->source, AL_PITCH, 1);
	alSourcef(audio->source, AL_GAIN, 1);
	alSourcei(audio->source, AL_LOOPING, AL_FALSE);

	s16 *startingBuffer = (s16 *)frameMalloc(sizeof(s16) * SAMPLE_BUFFER_LIMIT);

	alGenBuffers(2, audio->buffers);
	alBufferData(audio->buffers[0], AL_FORMAT_STEREO16, startingBuffer, SAMPLE_BUFFER_LIMIT * sizeof(s16), SAMPLE_RATE);
	alBufferData(audio->buffers[1], AL_FORMAT_STEREO16, startingBuffer, SAMPLE_BUFFER_LIMIT * sizeof(s16), SAMPLE_RATE);
	CheckAudioError();

	alSourceQueueBuffers(audio->source, 2, audio->buffers);
	alSourcePlay(audio->source);

#if 0
	int isPlaying;
	alGetSourcei(audio->source, AL_SOURCE_STATE, &isPlaying);
	logf("Is playing: %d\n", isPlaying);
#endif

	CheckAudioError();

	Sound *sound;
	sound = getSound("assets/common/audio/silence.ogg");
	sound->maxConcurrentInstances = 0;
}

void initSound(Sound *sound) {
	sound->vorbisTempMem.alloc_buffer = (char *)malloc(500*1024);
	sound->vorbisTempMem.alloc_buffer_length_in_bytes = 500*1024;

	int err;
	sound->vorbis = stb_vorbis_open_memory(sound->oggData, sound->oggDataLen, &err, &sound->vorbisTempMem);

	if (err != 0) {
		logf("Stb vorbis failed to start stream with error %d\n", err);
		Assert(0);
	}

	stb_vorbis_info vorbisInfo = stb_vorbis_get_info(sound->vorbis);

	sound->length = stb_vorbis_stream_length_in_seconds(sound->vorbis);
	sound->sampleRate = vorbisInfo.sample_rate;
	sound->channels = vorbisInfo.channels;
	sound->samplesTotal = stb_vorbis_stream_length_in_samples(sound->vorbis) * sound->channels;
}

Channel *playSound(Sound *sound, bool looping) {
	if (!sound) {
		logf("Called playSound with NULL sound\n");
		return NULL;
	}

	Channel *channel = NULL;
	for (int i = 0; i < CHANNELS_MAX; i++) {
		if (!audio->channels[i].exists) {
			channel = &audio->channels[i];
			break;
		}
	}

	if (!channel) {
		logf("There are no more sound channels!\n");
		return NULL;
	}

	if (sound->maxConcurrentInstances != 0 && sound->concurrentInstances > sound->maxConcurrentInstances-1) {
		return playSound(getSound("assets/common/audio/silence.ogg"), looping);
	}

	memset(channel, 0, sizeof(Channel));

	channel->id = ++audio->nextChannelId;
	channel->userVolume = 1;
	channel->userVolume2 = 1;
	channel->exists = true;
	channel->looping = looping;

	channel->lastVolume = 0;
	channel->sound = sound;

	sound->concurrentInstances++;
	audio->channelsCount++;
	return channel;
}

void stopChannel(int channelId) {
	Channel *channel = getChannel(channelId);
	if (channel) stopChannel(channel);
}

void stopChannel(Channel *channel) {
	if (channel->sound) channel->sound->concurrentInstances--;
	audio->channelsCount--;
	channel->exists = false;
}

void updateAudio() {
	if (!audio || audio->disabled) return;

#if defined(__EMSCRIPTEN__)
	EM_ASM({
		if (AL.currentCtx.audioCtx.state == "suspended") AL.currentCtx.audioCtx.resume();
	});
#endif

	int toProcess;
	alGetSourcei(audio->source, AL_BUFFERS_PROCESSED, &toProcess);

	bool requeued = false;
	for(int i = 0; i < toProcess; i++) {
		requeued = true;

		ALuint buffer;
		alSourceUnqueueBuffers(audio->source, 1, &buffer);

		if (!audio->sampleBuffer) audio->sampleBuffer = (s16 *)malloc(sizeof(s16) * SAMPLE_BUFFER_LIMIT);
		mixSound(audio->sampleBuffer, SAMPLE_BUFFER_LIMIT);

		alBufferData(buffer, AL_FORMAT_STEREO16, audio->sampleBuffer, SAMPLE_BUFFER_LIMIT * sizeof(s16), SAMPLE_RATE);
		CheckAudioError();

		alSourceQueueBuffers(audio->source, 1, &buffer);
		CheckAudioError();
	}

	if (requeued) {
		int isPlaying;
		alGetSourcei(audio->source, AL_SOURCE_STATE, &isPlaying);
		if (isPlaying == AL_STOPPED) alSourcePlay(audio->source);
	}

	CheckAudioError();
}

void mixSound(s16 *destBuffer, int destSamplesNum) {
	memset(destBuffer, 0, destSamplesNum * sizeof(s16));

	for (int i = 0; i < CHANNELS_MAX; i++) {
		Channel *channel = &audio->channels[i];

		if (channel->delay > 0) {
			channel->delay -= 1/60.0;
			continue;
		}

		if (!channel->exists) continue;
		Sound *sound = channel->sound;

		if (sound->samplesStreamed < sound->samplesTotal) {
			if (!sound->samples) {
				sound->samples = (s16 *)malloc(sound->samplesTotal * sizeof(s16));
				audio->memoryUsed += sound->samplesTotal * sizeof(s16);
			}

			int samplesGot = 0;
			if (sound->channels == 1) {
				samplesGot = stb_vorbis_get_samples_short_interleaved(sound->vorbis, 1, &sound->samples[sound->samplesStreamed], destSamplesNum) * 1;
			} else if (sound->channels == 2) {
				samplesGot = stb_vorbis_get_samples_short_interleaved(sound->vorbis, 2, &sound->samples[sound->samplesStreamed], destSamplesNum) * 2;
			} else {
				Panic("Can't play sound with more than 2 channels");
			}

			sound->samplesStreamed += samplesGot;
			// logf("Streaming %d/%d samples\n", sound->samplesStreamed, sound->samplesTotal);

			if (samplesGot < destSamplesNum) {
				// logf("Finished streaming %d/%d samples\n", sound->samplesStreamed, sound->samplesTotal);
				stb_vorbis_close(sound->vorbis);
				sound->vorbis = NULL;
				free(sound->vorbisTempMem.alloc_buffer);

				free(sound->oggData);
			}
		}

		channel->userVolume = mathClamp(channel->userVolume, 0, 1);
		channel->userVolume2 = mathClamp(channel->userVolume2, 0, 1);

		float vol;
		{ // computeChannelVolume
			float tweak = 1;
			if (channel->sound) tweak = channel->sound->tweakVolume;
			vol = tweak * channel->userVolume * channel->userVolume2 * audio->masterVolume;
			vol = vol*vol;
		}

		float startVol = vol;
		float volAdd = 0;

		if (vol != channel->lastVolume) {
			float minVol = channel->lastVolume;
			float maxVol = vol;
			startVol = minVol;
			volAdd = (maxVol-minVol) / destSamplesNum;
		}

		channel->lastVolume = vol;

		for (int i = 0; i < destSamplesNum; i+=2) {
			float curVol;
			if (audio->doInstantVolumeChanges) {
				curVol = vol;
			} else {
				curVol = startVol + volAdd * i;
			}

			s16 rightSample = 0;
			s16 leftSample = 0;

			if (sound->channels == 2) {
				rightSample = sound->samples[channel->samplePosition];
				leftSample = sound->samples[channel->samplePosition+1];
			} else if (sound->channels == 1) {
				rightSample = sound->samples[channel->samplePosition];
				leftSample = sound->samples[channel->samplePosition];
			}

			channel->samplePosition += sound->channels;

			// s16 noise = rndFloat(-0.25, 0.25) * SHRT_MAX;
			// leftSample += noise;
			// rightSample += noise;

			float leftPan = 1;
			float rightPan = 1;
			if (channel->pan < 0) rightPan = 1 + channel->pan;
			if (channel->pan > 0) leftPan = 1 - channel->pan;

			leftSample *= curVol * leftPan;
			rightSample *= curVol * rightPan;

			destBuffer[i] = clampedS16Add(destBuffer[i], rightSample);
			destBuffer[i+1] = clampedS16Add(destBuffer[i+1], leftSample);

			if (channel->samplePosition >= sound->samplesTotal-1) {
				if (channel->looping) {
					channel->samplePosition = 0;
				} else {
					stopChannel(channel);
					break;
				}
			}
		}

		channel->secondPosition = ((float)channel->samplePosition / (float)sound->samplesTotal) * sound->length;
	}
}

Channel *getChannel(int id) {
	if (id == 0) return NULL;

	for (int i = 0; i < CHANNELS_MAX; i++) {
		Channel *channel = &audio->channels[i];
		if (channel->exists && channel->id == id) return channel;
	}

	return NULL;
}

void seekChannelPerc(Channel *channel, float perc) {
	channel->samplePosition = channel->sound->samplesTotal * perc;
}

Sound *getSound(const char *path, bool onlyLoadRaw) {
	/// Look in the store for a sound
	for (int i = 0; i < audio->soundStoreNamesNum; i++) {
		char *soundName = audio->soundStoreNames[i];
		if (streq(soundName, path)) {
			Sound *sound = audio->soundStore[i];
			if (!onlyLoadRaw && sound->length == 0) initSound(sound);
			return sound;
		}
	}

	/// Not in store, look for file
	if (fileExists(path)) {
		Sound *sound = NULL;
		for (int i = 0; i < SOUNDS_MAX; i++) {
			if (!audio->sounds[i].exists) {
				sound = &audio->sounds[i];
				break;
			}
		}

		if (!sound) {
			logf("There are no more sound slots\n");
			Assert(0);
		}

		memset(sound, 0, sizeof(Sound));
		sound->exists = true;
		sound->path = stringClone(path);
		sound->tweakVolume = 1;
		sound->maxConcurrentInstances = audio->defaultMaxConcurrentInstances;

		sound->oggData = (unsigned char *)readFile(sound->path, &sound->oggDataLen);

		if (!onlyLoadRaw) initSound(sound);

		if (audio->soundStoreNum < SOUNDS_MAX) {
			strcpy(audio->soundStoreNames[audio->soundStoreNamesNum++], path);
			audio->soundStore[audio->soundStoreNum++] = sound;
		} else {
			logf("Sound store full! Sound will leak\n");
		}

		return sound;
	}

	return NULL;
}

SoundSource *playUiSound(char *path) {
	SoundSource *source = playWorldSound(v3(), path);
	if (source) source->ui = true;
	return source;
}

SoundSource *playWorldSound(Vec3 position, char *path) {
	if (audio->soundSourcesNum > SOUND_SOURCES_MAX-1) return NULL;

	Sound *sound = getSound(path);
	if (!sound) {
		logf("No sound found at %s\n", path);
		return NULL;
	}

	int samplesNeeded = sound->samplesTotal - sound->samplesStreamed;
	if (samplesNeeded > 0) {
		int samplesGot = stb_vorbis_get_samples_short_interleaved(sound->vorbis, sound->channels, &sound->samples[sound->samplesStreamed], samplesNeeded);
		samplesGot *= sound->channels;
		sound->samplesStreamed += samplesGot;
	}

	if (!sound->al3dBuffer) {
		alGenBuffers(1, &sound->al3dBuffer);
		ALenum format = 0;
		if (sound->channels == 1) format = AL_FORMAT_MONO16;
		if (sound->channels == 2) format = AL_FORMAT_STEREO16;
		alBufferData(sound->al3dBuffer, format, sound->samples, sound->samplesTotal * sizeof(s16), sound->sampleRate);
	}

	SoundSource *source = (SoundSource *)zalloc(sizeof(SoundSource));
	source->id = ++audio->nextSoundSourceId;
	source->sound = sound;
	alGenSources(1, &source->source);
	// alSourcef(source->source, AL_PITCH, 1);
	// alSourcef(source->source, AL_GAIN, 1.0f);
	setPosition(source, position);
	// alSource3f(source->source, AL_POSITION, position.x, position.y, position.z);
	// alSource3f(source->source, AL_VELOCITY, 0, 0, 0);
	alSourcei(source->source, AL_LOOPING, AL_FALSE);
	alSourcei(source->source, AL_BUFFER, sound->al3dBuffer);
	// alSourcef(source->source, AL_ROLLOFF_FACTOR, 3);
	CheckAudioError();

	alSourcePlay(source->source);
	CheckAudioError();

	audio->soundSources[audio->soundSourcesNum++] = source;
	return source;
}

void setPosition(SoundSource *soundSource, Vec3 position) {
	alSource3f(soundSource->source, AL_POSITION, position.x, position.y, position.z);
}

SoundSource *getSoundSource(int id) {
	if (id == 0) return NULL;

	for (int i = 0; i < audio->soundSourcesNum; i++) {
		SoundSource *source = audio->soundSources[i];
		if (source->id == id) return source;
	}

	return NULL;
}

void stop(SoundSource *source);
void stop(SoundSource *source) {
	alSourceStop(source->source);
}

void setVolume(SoundSource *source, float volume);
void setVolume(SoundSource *source, float volume) {
	alSourcef(source->source, AL_GAIN, volume);
}

void update3dSound(Vec3 pos, Vec3 up, Vec3 front);
void update3dSound(Vec3 pos, Vec3 up, Vec3 front) {
	ALfloat listenerOri[] = { front.x, front.y, front.z, up.x, up.y, up.z };
	alListener3f(AL_POSITION, pos.x, pos.y, pos.z);
	alListener3f(AL_VELOCITY, 0, 0, 0);
	alListenerfv(AL_ORIENTATION, listenerOri);

	for (int i = 0; i < audio->soundSourcesNum; i++) {
		SoundSource *source = audio->soundSources[i];

		if (source->ui) {
			alSource3f(source->source, AL_POSITION, pos.x, pos.y, pos.z);
		}

		ALint state;
		alGetSourcei(source->source, AL_SOURCE_STATE, &state);
		if (state == AL_STOPPED) {
			alDeleteSources(1, &source->source);
			arraySpliceIndex(audio->soundSources, audio->soundSourcesNum, sizeof(SoundSource *), i);
			free(source);
			i--;
			audio->soundSourcesNum--;
			continue;
		}
	}

	CheckAudioError();
}

void setSoundLength(SoundSource *source, float newLength) {
	float multi = source->sound->length / newLength;
	alSourcef(source->source, AL_PITCH, multi);
}

void checkAudioError(int lineNum) {
	ALCenum error = alGetError();

	if (error != AL_NO_ERROR) {
		logf("Openal is in error state %d/%x (line: %d)\n", error, error, lineNum);
		// Assert(0);
	}
}
