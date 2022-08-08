#pragma once

#include <string>
#include <alsa/asoundlib.h>

class CAlsaDev
{
public:
	enum EType
	{
		TYPE_PLAYBACK,
		TYPE_CAPTURE
	};

	CAlsaDev(const std::string &devname, EType type);
	~CAlsaDev();

	snd_pcm_t *operator()();

private:
	snd_pcm_t *m_hnd;
};
