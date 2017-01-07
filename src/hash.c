#include <errno.h>

#include <hash-md5.h>

#include <hash.h>

GQuark hash_calc_error_quark()
{
        return g_quark_from_static_string("HASH_CALC_ERR");
}

GThread *hash_start_calc(struct hash_data *data, enum HASH_TYPE type, GError **err)
{
        GError *g_thread_err;

        if (type >= HASH_COUNT) {
                errno = EINVAL;
                g_set_error(err, hash_calc_error_quark(), errno,
                            "Invalid hash type: %s", g_strerror(errno));
                return NULL;
        }

        data->type = type;
        
        data->job = g_thread_try_new("hash_job", _hash_job, data, &g_thread_err);

        if (!data->job) {
                g_propagate_error(err, g_thread_err);
                return NULL;
        } else {
                return data->job;
        }
}

gpointer _hash_job(gpointer h_data)
{
        gchar *hash_str;
        GError *err;
        struct hash_data *data;

        data = h_data;

        gtk_spinner_start(GTK_SPINNER(data->spinner));

        switch (data->type) {
        case HASH_MD5:
                hash_str = hash_calc_md5(data->path, &err);

                if (!hash_str) {
                        g_warning(err->message);
                        return NULL;
                }

                break;
        default:
                g_warning("No valid hash type!");
                return NULL;
        }

        gtk_label_set_text(GTK_LABEL(data->hash), hash_str);

        gtk_spinner_stop(GTK_SPINNER(data->spinner));
        gtk_widget_hide(data->spinner);

        return data;
}
