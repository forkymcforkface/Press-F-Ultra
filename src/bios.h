#ifndef PRESS_F_ULTRA_BIOS_H
#define PRESS_F_ULTRA_BIOS_H

#include <stddef.h>
#include <stdbool.h>

bool pfu_bios_path(char *path, size_t size);
bool pfu_bios_load(void *data, const char *root, const char *name);

#endif
