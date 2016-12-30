/*!
 * \file hash.h
 */
#ifndef _HASH_H_
#define _HASH_H_

#include <glib.h>

struct hash_data;

enum HASH_TYPE {
        HASH_MD5 = 0,
        HASH_COUNT
};

extern GThread *hash_start_calc(struct hash_data *data, enum HASH_TYPE type, GError **err);



#endif /* _HASH_H_ */
