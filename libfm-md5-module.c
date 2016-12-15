#include <glib.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bsd/md5.h>
#include <sys/types.h>

#include <libfm/fm.h>
#include <libfm/fm-gtk.h>
#include <libfm/fm-file-properties.h>

#define MD5_HASH_ERR md5_calc_hash_error_quark()

GQuark md5_calc_hash_error_quark(void)
{
        return g_quark_from_static_string("MD5_CALC_HASH_ERR");
}


struct md5_data {
        GtkWidget *label;
        GtkWidget *hash;
};

/*!
 * \brief Calculates the hash of a file.
 *
 * The function \p md5_calc_hash calculated the md5 hash value of
 * the file in path \p path. The function will return \p NULL on
 * error, idicating the type of error in \p err. The returned string
 * has to be freed manually.
 *
 * \param path Absolute path to the file whose hash shall be
 *             calculated.
 * \param err  \p GError indicating th type of the error, if the
 *             function should return \p NULL.
 * \return     The string representation of the md5 hash or \p NULL,
 *             if an error occured.
 */
static gchar *md5_calc_hash(FmPath *path, GError **err)
{
        char *path_str;
        gchar *hash;
        MD5_CTX ctx;

        if (!path) {
                errno = EINVAL;
                g_set_error(err, MD5_HASH_ERR, errno,
                            "Invalid path: %s", g_strerror(errno));
                return NULL;
        }

        path_str = NULL;
        
        hash = g_malloc0(MD5_DIGEST_STRING_LENGTH); 
        
        if (!hash) {
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
        
        MD5File(path_str, hash);

        if (!hash) {
                errno = EIO;
                g_set_error(err, MD5_HASH_ERR, errno,
                            "No hash: %s", g_strerror(errno));
                goto err_clean_up_ret;
        }
        
        g_free(path_str);
        return hash;

err_clean_up_ret:
        g_free(hash);
        g_free(path_str);
        return NULL;
}

static inline gboolean md5_check_file(FmFileInfoList *files)
{
        FmFileInfo *file = fm_file_info_list_peek_head(files);
        return ((fm_file_info_list_get_length(files) == 1) &&
                (!fm_file_info_is_dir(file)) &&
                (!fm_file_info_is_symlink(file))
               );   
}


static gpointer md5_init(GtkBuilder *ui, gpointer uidata, FmFileInfoList *files)
{
        GError *err;
        gchar *hash;
        
        FmPath *path;
        FmFileInfo *file;

        guint n_row, n_col;

        GtkWidget *table;

        if (!md5_check_file(files)) {
                errno = EINVAL;
                return NULL;
        }
        
        file = fm_file_info_list_peek_head(files);

        if ((path = fm_file_info_get_path(file)) == NULL) {
                errno = ENOENT;
                return NULL;
        }
        
        struct md5_data *data = g_malloc0(sizeof(struct md5_data));

        data->label = gtk_label_new(NULL);
        data->hash = gtk_label_new(NULL);

        g_object_ref_sink(data->label);
        g_object_ref_sink(data->hash);

        gtk_label_set_markup(GTK_LABEL(data->label), "<b>MD5sum:</b>");
        gtk_misc_set_alignment(GTK_MISC(data->label), 0.0f, 0.5f);

        gtk_label_set_line_wrap(GTK_LABEL(data->hash), TRUE);

        table = GTK_WIDGET(gtk_builder_get_object(ui, "general_table"));
        gtk_table_get_size(GTK_TABLE(table), &n_row, &n_col);
        gtk_table_attach_defaults(GTK_TABLE(table), data->label, 0, 1, n_row, n_row+1);
        gtk_table_attach_defaults(GTK_TABLE(table), data->hash, 2, 3, n_row, n_row+1);


        gtk_widget_show(data->label);
        gtk_widget_show(data->hash);
        
        /* Try to calculate HASH here */
        err = NULL;

        hash = md5_calc_hash(path, &err);


        if (!hash) {
                g_warning(err->message);
                goto no_output;
        }

        if (strlen(hash) == 0) {
                g_warning("Got empty hash!");
                return NULL;
        }

        g_warning("Hash: %s", hash);

        gtk_label_set_text(GTK_LABEL(data->hash), hash);

        return data;

no_output:

        gtk_widget_destroy(data->label);
        gtk_widget_destroy(data->hash);
        g_object_unref(data->label);
        g_object_unref(data->hash);

        g_free(data);
        
        return NULL;
}

static void md5_finish(gpointer pdata, gboolean cancelled)
{       
        struct md5_data *data = pdata;
        if (data) {
                gtk_widget_destroy(data->label);
                gtk_widget_destroy(data->hash);
                g_object_unref(data->label);
                g_object_unref(data->hash);

                g_free(data);
        }
}

FmFilePropertiesExtensionInit fm_module_init_gtk_file_prop = {
        .init = &md5_init,
        .finish = &md5_finish
};


FM_DEFINE_MODULE(gtk_file_prop, *)
