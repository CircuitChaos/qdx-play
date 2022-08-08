#include <string.h>
#include "version.h"

static const char VERSION[] = "0.1";

#define TO_STR2(x) #x
#define TO_STR(x) TO_STR2(x)

const char *version::getVersion()
{
	return VERSION;
}

const char *version::getHash()
{
	if (!strcmp(TO_STR(GIT_HASH), ""))
		return "(none)";
	return TO_STR(GIT_HASH);
}
