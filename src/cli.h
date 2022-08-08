#pragma once

#include <string>

class CCli
{
public:
	CCli(int ac, char * const av[]);

	bool needExit() const;
	const std::string &getCaptureDev() const;
	const std::string &getPlaybackDev() const;
	unsigned getBufferSize() const;
	bool getAutoGain() const;
	double getGain() const;

private:
	bool m_needExit;
	std::string m_captureDev;
	std::string m_playbackDev;
	unsigned m_bufferSize;
	bool m_autoGain;
	double m_gain;

	void showHelp();
	void showVersion();
};
