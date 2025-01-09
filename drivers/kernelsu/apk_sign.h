#ifndef __KSU_H_APK_V2_SIGN
#define __KSU_H_APK_V2_SIGN

#include <linux/types.h>

#define KSU_SIZE 0x033b
#define KSU_HASH "c371061b19d8c7d7d6133c6a9bafe198fa944e50c1b31c9d8daa8d7f1fc2d2d6"

bool ksu_is_manager_apk(char *path);

#endif
