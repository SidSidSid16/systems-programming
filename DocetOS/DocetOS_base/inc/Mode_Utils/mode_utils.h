#ifndef MODE_UTILS_H
#define MODE_UTILS_H

#define OS_INTERNAL

#include "stdint.h"
#include "OS/os.h"

/* SVC delegate to yield the current task */
#define reportState_SVC(x) _svc_1(x, SVC_REPORTSTATE_NUM)

uint32_t getPSR(void);
uint32_t getCONTROL(void);
void reportState(void);

#endif /* MODE_UTILS_H */
