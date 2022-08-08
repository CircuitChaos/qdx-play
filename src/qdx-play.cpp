#include <vector>
#include <stdexcept>
#include <cstdlib>
#include <inttypes.h>
#include <cmath>
#include <cstdio>
#include "cli.h"
#include "alsautil.h"
#include "alsa.h"
#include "throw.h"

void fillWithSilence(std::vector<double> &buf)
{
	for (size_t i(0); i < buf.size(); ++i)
		buf[i] = 0.0;
}

// converts input from SND_PCM_FORMAT_S24_3LE to double (-1...0...1)
static void convertCaptureBuf(std::vector<double> &out, const std::vector<uint8_t> &in)
{
	xassert(out.size() * 6 == in.size(), "Buffer size mismatch");
	for (size_t i(0); i < out.size(); ++i)
	{
		// input is 24-bit signed little-endian, which means that:
		// - max is ff ff 7f (meaning 7f ff ff)
		// - min is 00 00 80 (meaning 80 00 00)
		//
		// left channel, then right channel. we ignore the right
		// channel, because it's the same as the left one.
		//
		// first, let's convert it to 32-bit unsigned
		const uint32_t uns((in[i * 6] << 8) | (in[i * 6 + 1] << 16) | (in[i * 6 + 2] << 24));

		// now copy to 32-bit signed
		int32_t sig;
		memcpy(&sig, &uns, sizeof(sig));

		// convert to double
		double dbl(sig);

		// convert from 32-bit signed to -1...0...1
		dbl /= INT32_MAX;
		out[i] = dbl;
	}
}

static bool processBuffers(const std::vector<double> &in, std::vector<double> &out, double &gain, bool autoGain)
{
	xassert(in.size() == out.size(), "Buffer size mismatch");

	double maxOrigLevel(0.0), maxGainedLevel(0.0);
	for (size_t i(0); i < in.size(); ++i)
	{
		const double sample(in[i]);
		const double absSample(fabs(sample));
		const double gainSample(sample * gain);
		const double absGainSample(fabs(gainSample));

		if (absSample > maxOrigLevel)
			maxOrigLevel = absSample;

		if (absGainSample > maxGainedLevel)
			maxGainedLevel = absGainSample;

		if (absGainSample > 0.9)
		{
			// clipping is about to occur
			if (autoGain)
			{
				// this is too rapid
				// gain = 0.85 / absSample;
				gain /= 1.1;
				// printf("Decreasing gain to %.2f to prevent clipping\n", gain);
				return false;
			}
			else
				printf("Warning: near-clipping, need to decrease gain\n");
		}

		out[i] = gainSample;
	}

	// printf("Input signal level: %.2f%%, gain %.2f, output signal level: %.2f%%\n",
	// 	maxOrigLevel * 100,
	// 	gain,
	// 	maxGainedLevel * 100);

	if (autoGain)
	{
		if (maxGainedLevel < .1)
			gain *= 9.5;
		else if (maxGainedLevel < .7)
			gain *= 1.1;
		// printf("Increasing gain to %.2f because output signal level is < 70%%\n", gain);
	}

	printf("Input level: %.3f%%, gain: %.3f, output level: %.3f%%      \r",
		maxOrigLevel * 100,
		gain,
		maxGainedLevel * 100);
	fflush(stdout);

	return true;
}

// converts output from double (-1...0...+1) to SND_PCM_FORMAT_S16
static void convertPlaybackBuf(std::vector<int16_t> &out, const std::vector<double> &in)
{
	xassert(in.size() * 2 == out.size(), "Buffer size mismatch");
	for (size_t i(0); i < in.size(); ++i)
	{
		// convert to 16-bit signed
		int32_t sample(in[i] * INT16_MAX);

		// output to both channels
		out[i * 2] = sample;
		out[i * 2 + 1] = sample;
	}
}

static int Main(int ac, char * const av[])
{
	CCli cli(ac, av);
	if (cli.needExit())
		return EXIT_SUCCESS;

	std::string captureDev(cli.getCaptureDev());
	if (captureDev.empty())
		captureDev = alsautil::detectCaptureDev();
	xassert(!captureDev.empty(), "Could not find QDX audio device; is QDX connected?");

	std::string playbackDev(cli.getPlaybackDev());
	if (playbackDev.empty())
		playbackDev = alsautil::detectPlaybackDev();
	xassert(!playbackDev.empty(), "Could not determine default playback device; specify it with -p");

	CAlsa capture(captureDev, CAlsaDev::TYPE_CAPTURE);
	CAlsa playback(playbackDev, CAlsaDev::TYPE_PLAYBACK);

	// there's no 3-byte type anyway
	std::vector<uint8_t> m_captureRawBuf;
	std::vector<int16_t> m_playbackRawBuf;
	std::vector<double> m_captureDoubleBuf;
	std::vector<double> m_playbackDoubleBuf;

	m_captureRawBuf.resize(3 * 2 * cli.getBufferSize());
	m_playbackRawBuf.resize(2 * cli.getBufferSize());
	m_captureDoubleBuf.resize(cli.getBufferSize());
	m_playbackDoubleBuf.resize(cli.getBufferSize());

	// fill the playback buffer with silence first, and play it
	// so we have some time before first buffer underrun occurs
	fillWithSilence(m_playbackDoubleBuf);
	convertPlaybackBuf(m_playbackRawBuf, m_playbackDoubleBuf);
	playback.writeFrames(&m_playbackRawBuf[0], cli.getBufferSize());
	playback.writeFrames(&m_playbackRawBuf[0], cli.getBufferSize());

	double currentGain(cli.getAutoGain() ? 1.0 : cli.getGain());
	for (;;)
	{
		if (!capture.readFrames(&m_captureRawBuf[0], cli.getBufferSize()))
			continue;

		convertCaptureBuf(m_captureDoubleBuf, m_captureRawBuf);
		while (!processBuffers(m_captureDoubleBuf, m_playbackDoubleBuf, currentGain, cli.getAutoGain()));
		convertPlaybackBuf(m_playbackRawBuf, m_playbackDoubleBuf);

		if (!playback.writeFrames(&m_playbackRawBuf[0], cli.getBufferSize()))
		{
			// buffer underrun, let's play some silence
			fillWithSilence(m_playbackDoubleBuf);
			convertPlaybackBuf(m_playbackRawBuf, m_playbackDoubleBuf);
			playback.writeFrames(&m_playbackRawBuf[0], cli.getBufferSize());
			playback.writeFrames(&m_playbackRawBuf[0], cli.getBufferSize());
		}
	}

	return EXIT_SUCCESS;
}

int main(int ac, char * const av[])
{
	try
	{
		return Main(ac, av);
	}
	catch (const std::runtime_error &e)
	{
		fprintf(stderr, "Fatal error: %s\n", e.what());
		return EXIT_FAILURE;
	}

	/* NOTREACHED */
	return EXIT_SUCCESS;
}
