#include <errno.h>

#include <hash-data.h>
#include <hash-md5.h>

#include <hash.h>

GQuark hash_calc_error_quark()
{
        return g_quark_from_static_string("HASH_CALC_ERR");
}

GThread *hash_start_calc(struct hash_data *data, enum HASH_TYPE type, GError **err)
{
        GError *g_thread_err;
        gpointer (*thread_func)(gpointer);

        switch (type) {

        case HASH_MD5:
                thread_func = hash_job_calc_md5;
                break;
        default:
                errno = EINVAL;
                g_set_error(err, hash_calc_error_quark(), errno,
                            "Invalid hash type: %s", g_strerror(errno));
                return NULL;
        }
        
        data->job = g_thread_try_new("hash_job", thread_func, data, &g_thread_err);

        if (!data->job) {
                g_propagate_error(err, g_thread_err);
                return NULL;
        } else {
                return data->job;
        }
}

