#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <errno.h>

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


struct md5_row {
        GtkWidget *label;
        GtkWidget *hash;
};

static gchar *md5_calc_hash(FmPath *path, GError **err)
{
        MD5_CTX ctx;
        char *path_str;
        gchar *hash;

        if (!path) {
                errno = EINVAL;
                g_set_error(err, MD5_HASH_ERR, errno,
                            "Invalid path: %s", g_strerror(errno));
                goto err_ret;
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
err_ret:
        return NULL;
}

static gpointer md5_init(GtkBuilder *ui, gpointer uidata, FmFileInfoList *files)
{
        GError *err;
        gchar *hash;
        
        FmPath *path;
        FmFileInfo *file;

        guint n_row, n_col;

        GtkWidget *table;
        
        file = fm_file_info_list_peek_head(files);

        if ((path = fm_file_info_get_path(file)) == NULL) {
                errno = ENOENT;
                return NULL;
        }
        
        struct md5_row *row = malloc(sizeof(struct md5_row));
        memset(row, 0x00, sizeof(struct md5_row));

        row->label = gtk_label_new(NULL);
        row->hash = gtk_label_new(NULL);

        g_object_ref_sink(row->label);
        g_object_ref_sink(row->hash);

        gtk_label_set_markup(GTK_LABEL(row->label), "<b>MD5sum:</b>");
        gtk_misc_set_alignment(GTK_MISC(row->label), 0.0f, 0.5f);

        gtk_label_set_line_wrap(GTK_LABEL(row->hash), TRUE);

        table = GTK_WIDGET(gtk_builder_get_object(ui, "general_table"));
        gtk_table_get_size(GTK_TABLE(table), &n_row, &n_col);
        gtk_table_attach_defaults(GTK_TABLE(table), row->label, 0, 1, n_row, n_row+1);
        gtk_table_attach_defaults(GTK_TABLE(table), row->hash, 2, 3, n_row, n_row+1);


        gtk_widget_show(row->label);
        gtk_widget_show(row->hash);
        
        /* Try to calculate HASH here */
        err = NULL;

        hash = md5_calc_hash(path, &err);


        if (!hash || strlen(hash) == 0) {
                g_warning(err->message);
                goto no_output;
        }

        g_warning("Hash: %s", hash);

        gtk_label_set_text(GTK_LABEL(row->hash), hash);

        return row;

no_output:

        gtk_widget_destroy(row->label);
        gtk_widget_destroy(row->hash);
        g_object_unref(row->label);
        g_object_unref(row->hash);

        free(row);
        
        return NULL;
}

static void md5_finish(gpointer pdata, gboolean cancelled)
{       
        struct md5_row *data = pdata;
        if (data) {
                gtk_widget_destroy(data->label);
                gtk_widget_destroy(data->hash);
                //g_object_unref(data->label);
                g_object_unref(data->hash);

                free(data);
        }
}

FmFilePropertiesExtensionInit fm_module_init_gtk_file_prop = {
        .init = &md5_init,
        .finish = &md5_finish
};


FM_DEFINE_MODULE(gtk_file_prop, *)
