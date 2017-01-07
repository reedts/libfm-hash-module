/*!
 * \file hash.h
 */
#ifndef _HASH_H_
#define _HASH_H_

#include <glib.h>

#include <hash-data.h>

GQuark hash_calc_error_quark(void);

GThread *hash_start_calc(struct hash_data *data, enum HASH_TYPE type, GError **err);

gpointer _hash_job(gpointer data);

#endif /* _HASH_H_ */
