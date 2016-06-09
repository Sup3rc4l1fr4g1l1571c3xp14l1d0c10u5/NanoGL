#pragma once
#include <stdbool.h>

#define BUFFER_SIZE	(4096 * 32)

struct StreamChannel;

extern const struct __tagStreamChannelAPI {
	struct StreamChannel* (*Create)(void);
	void(*Dispose)(struct StreamChannel* self);
	bool(*Open)(struct StreamChannel* self, const char* filename);
	bool(*Stop)(struct StreamChannel* self);
	bool(*Play)(struct StreamChannel* self, bool loop);
	bool(*IsPlay)(struct StreamChannel* self);
	bool(*Pause)(struct StreamChannel* self);
	bool(*Progress)(struct StreamChannel* self);
} StreamChannel;



