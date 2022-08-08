#pragma once

#include <string>
#include "alsadev.h"

class CAlsa
{
public:
	CAlsa(const std::string &devname, CAlsaDev::EType type);
	~CAlsa();

	// sample format for capture is fixed to SND_PCM_FORMAT_S24_3LE,
	// so frame size is 6 (3 for left channel, 3 for right channel)

	// sample format for playback is fixed to SND_PCM_FORMAT_S16,
	// so frame size is 4 (2 for left channel, 2 for right channel)

	bool readFrames(void *buf, unsigned numFrames);
	bool writeFrames(const void *buf, unsigned numFrames);

private:
	CAlsaDev m_dev;
};
