#include <hash-data.h>
#include <hash-md5.h>

#include <hash.h>

GThread *hash_start_calc(struct hash_data *data, enum HASH_TYPE type, GError **err)
{
        GError *g_thread_err;
        g_warning("Starting job.");
        
        data->job = g_thread_try_new("hash_job", &hash_job_calc_md5, data, &g_thread_err);

        if (!data->job) {
                g_propagate_error(err, g_thread_err);
                return NULL;
        } else {
                return data->job;
        }
}

