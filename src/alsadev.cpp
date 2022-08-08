#include <string.h>
#include "alsadev.h"
#include "throw.h"

CAlsaDev::CAlsaDev(const std::string &devname, EType type): m_hnd(NULL)
{
	printf("Using %s device: %s\n", type == TYPE_PLAYBACK ? "playback" : "capture", devname.c_str());
	const int rs(snd_pcm_open(&m_hnd, devname.c_str(), (type == TYPE_PLAYBACK) ? SND_PCM_STREAM_PLAYBACK : SND_PCM_STREAM_CAPTURE, 0));
	xassert(rs >= 0, "Unable to open PCM device: %s", snd_strerror(rs));
}

CAlsaDev::~CAlsaDev()
{
	snd_pcm_close(m_hnd);
}

snd_pcm_t *CAlsaDev::operator()()
{
	return m_hnd;
}
