#include <unistd.h>
#include <cmath>
#include "cli.h"
#include "throw.h"
#include "version.h"

static const unsigned DEFAULT_BUFFER_SIZE = 4800;

CCli::CCli(int ac, char * const av[]):
	m_needExit(false),
	m_bufferSize(DEFAULT_BUFFER_SIZE),
	m_autoGain(true),
	m_gain(NAN)
{
	int opt;
	while ((opt = getopt(ac, av, "b:c:g:hp:v")) != -1)
	{
		switch (opt)
		{
		case '?':
			xthrow("Option -%c not recognized; use -h for help", optopt);
			break;

		case ':':
			xthrow("Option -%c requires argument; use -h for help", optopt);
			break;

		case 'b':
			m_bufferSize = atoi(optarg);
			break;

		case 'c':
			m_captureDev = optarg;
			break;

		case 'p':
			m_playbackDev = optarg;
			break;

		case 'g':
			if (std::string(optarg) == "auto")
			{
				m_autoGain = true;
				m_gain = NAN;
			}
			else
			{
				m_autoGain = false;
				m_gain = atof(optarg);
			}
			break;

		case 'h':
			showHelp();
			m_needExit = true;
			return;

		case 'v':
			showVersion();
			m_needExit = true;
			return;

		default:
			break;
		}
	}

	if (optind != ac)
		xthrow("Excessive arguments; use -h for help");

	if (m_bufferSize == 0)
		xthrow("Buffer size cannot be zero");

	if (!m_autoGain && m_gain <= 0.001)
		xthrow("Gain must be positive");
}

void CCli::showHelp()
{
	const char help[] = \
			"Syntax: qdx-play [-hv] [-b <buffer size>] [-p <playback device>] [-c <capture device>] [-g <gain>]\n"
			"\n"
			"If playback device is not provided, then default is used.\n"
			"\n"
			"If capture device is not provided, then it's autodetected.\n"
			"\n"
			"Buffer size is in samples. Default value is %u samples and should be OK.\n"
			"\n"
			"Gain can be any floating-point number greater than 0.\n"
			"\n"
			"Gain can be also set to \"auto\", which will make the program dynamically\n"
			"adjust the gain for the best experience. This is also the default.\n"
			"\n"
			"Note that gain is only used to amplify input from the QDX before sending\n"
			"it to the sound card. qdx-play doesn't touch the CAT interface of the radio.\n"
			"If you want to set gain of the QDX itself, use rigctl or another program.\n"
			"\n"
			"For newest version of this program, visit:\n"
			"https://github.com/CircuitChaos/qdx-play\n";
	showVersion();
	printf(help, DEFAULT_BUFFER_SIZE);
}

void CCli::showVersion()
{
	printf("qdx-play version %s, git hash %s\n", version::getVersion(), version::getHash());
}

bool CCli::needExit() const
{
	return m_needExit;
}

const std::string &CCli::getCaptureDev() const
{
	return m_captureDev;
}

const std::string &CCli::getPlaybackDev() const
{
	return m_playbackDev;
}

unsigned CCli::getBufferSize() const
{
	return m_bufferSize;
}

bool CCli::getAutoGain() const
{
	return m_autoGain;
}

double CCli::getGain() const
{
	return m_gain;
}
