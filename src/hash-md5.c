#include <bsd/md5.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include <hash-data.h>
#include <hash-md5.h>
#include <hash-module.h>


#define MD5_HASH_ERR hash_calc_md5_error_quark()

GQuark hash_calc_md5_error_quark()
{
        return g_quark_from_static_string("MD5_CALC_HASH_ERR");
}

gpointer hash_job_calc_md5(gpointer md5_data)
{
        gchar *hash_str;
        GError *err;
        struct hash_data *data;
        
        data = md5_data;

        hash_str = _hash_calc_md5(data->path, &err);

        if (!hash_str) {
                g_warning(err->message);
                return NULL;
        }

        gtk_label_set_text(GTK_LABEL(data->hash), hash_str);

        return data;
}


gchar *_hash_calc_md5(FmPath *path, GError **err)
{
        char *path_str;
        gchar *hash_str;
        MD5_CTX ctx;

        if (!path) {
                errno = EINVAL;
                g_set_error(err, MD5_HASH_ERR, errno,
                            "Invalid path: %s", g_strerror(errno));
                return NULL;
        }

        path_str = NULL;
        
        hash_str = g_malloc0(MD5_DIGEST_STRING_LENGTH); 
        
        if (!hash_str) {
                errno = ENOMEM;
                g_set_error(err, MD5_HASH_ERR, errno,
                            "No memory allocated: %s", g_strerror(errno));
                goto err_clean_up_ret;
        }
        
        path_str = fm_path_to_str(path);

        if (!path_str) {
                errno = EINVAL;
                g_set_error(err, MD5_HASH_ERR, errno,
                            "Invalid path: %s", g_strerror(errno));
                goto err_clean_up_ret;
        }

        MD5Init(&ctx);
        
        MD5File(path_str, hash_str);

        if (!hash_str || !strlen(hash_str)) {
                errno = EIO;
                g_set_error(err, MD5_HASH_ERR, errno,
                            "No hash: %s", g_strerror(errno));
                goto err_clean_up_ret;
        }
        
        g_free(path_str);
        return hash_str;

err_clean_up_ret:
        g_free(hash_str);
        g_free(path_str);
        return NULL;
}
