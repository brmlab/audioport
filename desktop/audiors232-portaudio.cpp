#include <limits.h>
#include "audiors232-portaudio.h"

#include <stdio.h>

#define SAMPRATE 48000
#define FRAMES 8192
#define BITLEN 8

AudioRS232::AudioRS232()
{
	cb = new CyclicBuffer(16384);
}

AudioRS232::~AudioRS232()
{
	Pa_StopStream(stream);
	Pa_CloseStream(stream);
	Pa_Terminate();
	delete cb;
}

bool AudioRS232::start()
{
	if (paNoError != Pa_Initialize())
		return false;
	if (paNoError != Pa_OpenDefaultStream(&stream, 2, 2, paInt16, SAMPRATE, FRAMES, paCallback, this))
		return false;
	if (paNoError != Pa_StartStream(stream))
		return false;
	return true;
}

int paCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData)
{
	AudioRS232 *audio = (AudioRS232 *)userData;
	short *buf = (short *)outputBuffer;
	for (unsigned long i = 0; i < framesPerBuffer / BITLEN / 16; ++i) {
		//convertSend will write 16 * BITLEN frames
		convertSend(buf, audio->cb->get(), SHRT_MAX, BITLEN);
		buf += 16 * BITLEN * 2;
	}
	return paContinue;
}

void AudioRS232::send(const char *str)
{
	cb->put(str);
}
