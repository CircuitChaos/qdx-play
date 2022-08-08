#include "alsahwparams.h"
#include "throw.h"

CAlsaHwParams::CAlsaHwParams()
{
	const int rs(snd_pcm_hw_params_malloc(&m_params));
	xassert(rs >= 0, "Unable to allocate HW params: %s", snd_strerror(rs));
}

CAlsaHwParams::~CAlsaHwParams()
{
	snd_pcm_hw_params_free(m_params);
}

snd_pcm_hw_params_t *CAlsaHwParams::operator()()
{
	return m_params;
}
