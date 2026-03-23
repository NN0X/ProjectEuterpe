#ifndef INIT_H
#define INIT_H

#include <functional>

#include <miniaudio.h>

ma_device* init(std::function<void(ma_device*, void*, const void*, ma_uint32)> callback);
void close(ma_device* device);

#endif
