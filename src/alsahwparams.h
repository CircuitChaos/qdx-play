#pragma once

#include <alsa/asoundlib.h>

class CAlsaHwParams
{
public:
	CAlsaHwParams();
	~CAlsaHwParams();

	snd_pcm_hw_params_t *operator()();

private:
	snd_pcm_hw_params_t *m_params;
};
