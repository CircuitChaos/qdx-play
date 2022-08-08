#include <vector>
#include <alsa/asoundlib.h>
#include "alsautil.h"
#include "throw.h"

std::vector<std::string> detectDevices()
{
	std::vector<std::string> rs;
	char **hints;
	int err(snd_device_name_hint(-1, "pcm", (void ***) &hints));
	xassert(err == 0, "snd_device_name_hint(): %d", err);

	for (char **hint(hints); *hint; ++hint)
	{
		char *name(snd_device_name_get_hint(*hint, "NAME"));
		rs.push_back(name);
		if (name && strcmp("null", name))
			free(name);
	}

	snd_device_name_free_hint((void **) hints);
	return rs;
}

std::string alsautil::detectCaptureDev()
{
	std::vector<std::string> devs(detectDevices());
	for (auto i: devs)
		if (i.rfind("hw:CARD=Transceiver,", 0) == 0)
			return i;
	return "";
}

std::string alsautil::detectPlaybackDev()
{
	std::vector<std::string> devs(detectDevices());
	for (auto i: devs)
		if (i == "default")
			return i;
	return "";
}
