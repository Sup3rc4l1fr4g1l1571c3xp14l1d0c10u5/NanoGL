#pragma once
#include <stdbool.h>

extern const struct __tagChannelManagerAPI {
	bool(*Initialize)(void);
	void(*Finalize)(void);
	bool(*CreateFromFile)(int channel, const char *filename);
	bool(*Close)(int channel);
	bool(*Play)(int channel, bool loop);
	bool(*IsPlay)(int channel);
	bool(*Stop)(int channel);
	void(*Progress)(void);
} ChannelManager;
