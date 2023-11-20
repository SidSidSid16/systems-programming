#ifndef MODE_UTILS_H
#define MODE_UTILS_H

#include "stdint.h"

uint32_t getPSR(void);
uint32_t getCONTROL(void);
void reportState(void);

#endif /* MODE_UTILS_H */
