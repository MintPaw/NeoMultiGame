#include "stb_vorbis.c"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

#ifdef __EMSCRIPTEN__
# define SAMPLE_BUFFER_LIMIT (4096)
#else
# define SAMPLE_BUFFER_LIMIT (4096)
#endif

#define CHANNELS_MAX 512
#define SOUNDS_MAX 4096
#define SAMPLE_RATE 44100

struct Sound {
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

	int concurrentInstances;
	int maxConcurrentInstances;
};

struct Channel {
	int id;

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
	bool markedForDeletion;
};

struct Audio {
	bool disabled;
	bool htmlAllowedAudio;

	Channel channels[CHANNELS_MAX];
	int channelsNum;
	int nextChannelId;

	Sound sounds[SOUNDS_MAX];
	int soundsNum;

	float masterVolume;

	int memoryUsed;

	char soundStoreNames[SOUNDS_MAX][PATH_MAX_LEN];
	int soundStoreNamesNum;
	Sound *soundStore[SOUNDS_MAX];
	int soundStoreNum;

	int defaultMaxConcurrentInstances;

	ALCdevice *device;
	ALCcontext *context;

	ALuint buffers[2];
	ALuint source;

#define STORED_SAMPLES_MAX (SAMPLE_BUFFER_LIMIT*4)
	s16 storedSamples[STORED_SAMPLES_MAX];
	int storedSamplesPosition;
};

Audio *audio = NULL;

void initAudio();

void initSound(Sound *sound);
void updateAudio(float elapsed);
Channel *playSound(Sound *sound, bool looping=false);
void stopChannel(int channelId);
void stopChannel(Channel *channel);
Channel *getChannel(int id);
void seekChannelPerc(Channel *channel, float perc);

Sound *getSound(const char *path, bool onlyLoadRaw=false);

/// Private
void mixSound(s16 *destBuffer, int destSamplesNum);
void mixSoundInToGlobalBuffer(int samplesToAdd);
void checkAudioError(int lineNum);
void reconnectAudioDevice();
#define CheckAudioError() checkAudioError(__LINE__);

/// FUNCTIONS^

void initAudio() {
	// logf("Initing audio (%.2fmb)\n", sizeof(Audio) / 1024.0 / 1024.0);

	audio = (Audio *)zalloc(sizeof(Audio));
	audio->masterVolume = 1;

	if (platform->isCommandLineOnly) {
		audio->disabled = true;
		return;
	}

	reconnectAudioDevice();

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

void reconnectAudioDevice() {
	if (audio->device) {
		logf("Reconnecting...\n");
		CheckAudioError();
		// alcDestroyContext(audio->context);
		// alcCloseDevice(audio->device);
		CheckAudioError();

		audio->context = NULL;
		audio->device = NULL;
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

	if (audio->channelsNum > CHANNELS_MAX-1) {
		logf("There are no more sound channels!\n");
		return NULL;
	}

	if (sound->maxConcurrentInstances != 0 && sound->concurrentInstances > sound->maxConcurrentInstances-1) {
		return playSound(getSound("assets/common/audio/silence.ogg"), looping);
	}

	Channel *channel = &audio->channels[audio->channelsNum++];
	memset(channel, 0, sizeof(Channel));
	channel->id = ++audio->nextChannelId;
	channel->userVolume = 1;
	channel->userVolume2 = 1;
	channel->looping = looping;

	channel->lastVolume = -1;
	channel->sound = sound;

	sound->concurrentInstances++;
	return channel;
}

void stopChannel(int channelId) {
	stopChannel(getChannel(channelId));
}

void stopChannel(Channel *channel) {
	if (!channel) return;
	if (channel->sound) channel->sound->concurrentInstances--;
	channel->markedForDeletion = true;
}

void updateAudio(float elapsed) {
	if (!audio || audio->disabled) return;

#if defined(__EMSCRIPTEN__)
	if (platform->frameCount % 8 == 0) {
		EM_ASM({
			if (AL.currentCtx.audioCtx.state == "suspended") AL.currentCtx.audioCtx.resume();
		});
	}
#endif

#if 0 // Reconnect audio if disconnected (but not if default device changes!!!) (also doesn't work in html5)
	if (platform->frameCount % 30 == 0) {
		int connected = -1;
		alcGetIntegerv(audio->device, ALC_CONNECTED, 1, &connected);
		if (connected == 0) {
			reconnectAudioDevice();
		}
	}
#endif

	int samplesToAdd = elapsed * SAMPLE_RATE * 2;
	mixSoundInToGlobalBuffer(samplesToAdd);

	int toProcess;
	alGetSourcei(audio->source, AL_BUFFERS_PROCESSED, &toProcess);

	bool requeued = false;
	for(int i = 0; i < toProcess; i++) {
		requeued = true;

		int missingSamples = SAMPLE_BUFFER_LIMIT - audio->storedSamplesPosition;
		if (missingSamples > 0) {
			logf("Audio lag: %d\n", missingSamples);
			mixSoundInToGlobalBuffer(missingSamples);
		}

		ALuint buffer;
		alSourceUnqueueBuffers(audio->source, 1, &buffer);

		// if (audio->storedSamplesPosition < SAMPLE_BUFFER_LIMIT) logf("Not enough stored samples (%d)\n", SAMPLE_BUFFER_LIMIT-audio->storedSamplesPosition);
		// if (audio->storedSamplesPosition >= SAMPLE_BUFFER_LIMIT) logf("You're ahead by %d samples\n", audio->storedSamplesPosition-SAMPLE_BUFFER_LIMIT);
		alBufferData(buffer, AL_FORMAT_STEREO16, audio->storedSamples, SAMPLE_BUFFER_LIMIT * sizeof(s16), SAMPLE_RATE);
		CheckAudioError();

		alSourceQueueBuffers(audio->source, 1, &buffer);
		CheckAudioError();

		int samplesLeftInStore = audio->storedSamplesPosition - SAMPLE_BUFFER_LIMIT;
		if (samplesLeftInStore <= 0) {
			samplesLeftInStore = 0;
		} else {
			memmove(audio->storedSamples, audio->storedSamples + SAMPLE_BUFFER_LIMIT, sizeof(s16) * samplesLeftInStore);
			audio->storedSamplesPosition -= SAMPLE_BUFFER_LIMIT;
		}
	}

	if (requeued) {
		int isPlaying;
		alGetSourcei(audio->source, AL_SOURCE_STATE, &isPlaying);
		if (isPlaying == AL_STOPPED) alSourcePlay(audio->source);
	}

	CheckAudioError();

	for (int i = 0; i < audio->channelsNum; i++) {
		Channel *channel = &audio->channels[i];
		if (channel->markedForDeletion) {
			arraySpliceIndex(audio->channels, audio->channelsNum, sizeof(Channel), i);
			audio->channelsNum--;
			i--;
			continue;
		}
	}
}

void mixSound(s16 *destBuffer, int destSamplesNum) {
	if (destSamplesNum == 0) return;

	memset(destBuffer, 0, destSamplesNum * sizeof(s16));
	for (int i = 0; i < audio->channelsNum; i++) {
		Channel *channel = &audio->channels[i];
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
				samplesGot = stb_vorbis_get_samples_short_interleaved(sound->vorbis, 2, &sound->samples[sound->samplesStreamed], destSamplesNum * 2) * 2;
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

		if (vol != channel->lastVolume && channel->lastVolume != -1) {
			float minVol = channel->lastVolume;
			float maxVol = vol;
			startVol = minVol;
			volAdd = (maxVol-minVol) / destSamplesNum;
		}

		channel->lastVolume = vol;

		for (int i = 0; i < destSamplesNum; i+=2) {
			float curVol;
			curVol = startVol + volAdd * i;

			s16 rightSample = 0;
			s16 leftSample = 0;

			if (channel->delay > 0) {
				channel->delay -= 1 / (float)SAMPLE_RATE;
			} else {
				if (sound->channels == 2) {
					rightSample = sound->samples[channel->samplePosition];
					leftSample = sound->samples[channel->samplePosition+1];
				} else if (sound->channels == 1) {
					rightSample = sound->samples[channel->samplePosition];
					leftSample = sound->samples[channel->samplePosition];
				}

				channel->samplePosition += sound->channels;
			}

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

void mixSoundInToGlobalBuffer(int samplesToAdd) {
	if (samplesToAdd % 2 == 1) samplesToAdd--;
	int maxSamplesLeft = STORED_SAMPLES_MAX - audio->storedSamplesPosition;
	if (samplesToAdd > maxSamplesLeft) {
		samplesToAdd = maxSamplesLeft;
	}

	mixSound(audio->storedSamples + audio->storedSamplesPosition, samplesToAdd);
	audio->storedSamplesPosition += samplesToAdd;
}

Channel *getChannel(int id) {
	if (id == 0) return NULL;

	for (int i = 0; i < audio->channelsNum; i++) {
		Channel *channel = &audio->channels[i];
		if (channel->id == id) return channel;
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
		if (audio->soundsNum > SOUNDS_MAX-1) Panic("There are no more sound slots\n");

		Sound *sound = &audio->sounds[audio->soundsNum++];
		memset(sound, 0, sizeof(Sound));
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

void checkAudioError(int lineNum) {
	ALCenum error = alGetError();

	if (error != AL_NO_ERROR) {
		logf("Openal is in error state %d/%x (line: %d)\n", error, error, lineNum);
		// Assert(0);
	}
}
