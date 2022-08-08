#include <alsa/asoundlib.h>
#include "alsa.h"
#include "alsahwparams.h"
#include "throw.h"

CAlsa::CAlsa(const std::string &devname, CAlsaDev::EType type): m_dev(devname, type)
{
	int e;
	CAlsaHwParams hw;
	snd_pcm_format_t format(type == CAlsaDev::TYPE_CAPTURE ? SND_PCM_FORMAT_S24_3LE : SND_PCM_FORMAT_S16);

	e = snd_pcm_hw_params_any(m_dev(), hw());
	xassert(e >= 0, "Unable to initialize HW params structure: %s", snd_strerror(e));

	e = snd_pcm_hw_params_set_access(m_dev(), hw(), SND_PCM_ACCESS_RW_INTERLEAVED);
	xassert(e >= 0, "Unable to set interleaved access: %s", snd_strerror(e));

	e = snd_pcm_hw_params_set_format(m_dev(), hw(), format);
	xassert(e >= 0, "Unable to set format: %s", snd_strerror(e));

	e = snd_pcm_hw_params_set_rate(m_dev(), hw(), 48000, 0);
	xassert(e >= 0, "Unable to set rate: %s", snd_strerror(e));

	e = snd_pcm_hw_params_set_channels(m_dev(), hw(), 2);
	xassert(e >= 0, "Unable to set channel count: %s", snd_strerror(e));

	e = snd_pcm_hw_params(m_dev(), hw());
	xassert(e >= 0, "Unable to set HW parameters: %s", snd_strerror(e));
}

CAlsa::~CAlsa()
{
}

bool CAlsa::readFrames(void *buf, unsigned numFrames)
{
	const snd_pcm_sframes_t rs(snd_pcm_readi(m_dev(), buf, numFrames));
	if (rs < 0)
	{
		printf("\nsnd_pcm_readi() error %ld, trying recovery\n", rs);
		const int err(snd_pcm_recover(m_dev(), rs, 0));
		xassert(err >= 0, "snd_pcm_recover() error %d", err);
		return false;
	}
	xassert(rs == (snd_pcm_sframes_t) numFrames, "snd_pcm_readi(): requested %u frames, but read %ld frames\n", numFrames, rs);
	return true;
}

bool CAlsa::writeFrames(const void *buf, unsigned numFrames)
{
	const snd_pcm_sframes_t rs(snd_pcm_writei(m_dev(), buf, numFrames));
	if (rs < 0)
	{
		printf("\nsnd_pcm_writei() error %ld, trying recovery\n", rs);
		const int err(snd_pcm_recover(m_dev(), rs, 0));
		xassert(err >= 0, "snd_pcm_recover() error %d", err);
		return false;
	}
	xassert(rs == (snd_pcm_sframes_t) numFrames, "snd_pcm_writei(): requested %u frames, but written %ld frames\n", numFrames, rs);
	return true;
}
