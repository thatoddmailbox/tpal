#ifndef _TPAL_USER_H
#define _TPAL_USER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

char * tpal_user_get_app_data_path(const char * vendor, const char * program);

#ifdef __cplusplus
}
#endif

#endif