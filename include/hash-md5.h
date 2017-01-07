/*!
 * \file hash-md5.h
 */
#ifndef _HASH_MD5_H_
#define _HASH_MD5_H_

#include <glib.h>
#include <libfm/fm.h>

GQuark hash_calc_md5_error_quark(void);


/**
 * @brief Calculates the hash of a file.
 *
 * The function \p md5_calc_hash calculated the md5 hash value of
 * the file in path \p path. The function will return \p NULL on
 * error, idicating the type of error in \p err. The returned string
 * has to be freed manually.
 *
 * @param path Absolute path to the file whose hash shall be
 *             calculated.
 * @param err  \p GError indicating th type of the error, if the
 *             function should return \p NULL.
 * @return     The string representation of the md5 hash or \p NULL,
 *             if an error occured.
 */
gchar *hash_calc_md5(FmPath *path, GError **err); 


#endif /* _HASH_MD5_H_ */
