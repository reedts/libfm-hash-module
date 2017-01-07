/**
 * @file libfm-md5-module.c
 * @brief Contains all necessary functions for md5 hashing in libfm.
 *
 * This file provides functions that can be used in a module for the
 * libfm library to show md5 hash sums for files in the file properties
 * dialog.
 */
#include <glib.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libfm/fm.h>
#include <libfm/fm-gtk.h>
#include <libfm/fm-file-properties.h>

#include <hash.h>
#include <hash-data.h>
#include <hash-module.h>

/**
 * @brief Checks whether files contains only one valid file.
 *
 * The function \p md5_check_file checks, whether the \p FmFileInfoList
 * \p *files contains only one file. This file must not be a directory
 * or a symlink. Calculating the md5 hash of both is not eligible.
 *
 * @param files \p FmFileInfoList containing all selected files
 *
 * @return \p TRUE , if \p files contains only one file that is not a dirctory
 *         nor a symlink. \p FALSE otherwise.
 */
static inline gboolean check_file(FmFileInfoList *files)
{
        FmFileInfo *file = fm_file_info_list_peek_head(files);
        return ((fm_file_info_list_get_length(files) == 1) &&
                (!fm_file_info_is_dir(file)) &&
                (!fm_file_info_is_symlink(file))
               );   
}


gpointer hash_init(GtkBuilder *ui, gpointer uidata, FmFileInfoList *files)
{
        GError *err;
        
        FmPath *path;
        FmFileInfo *file;

        guint n_row, n_col;

        GtkWidget *table;

        if (!check_file(files)) {
                errno = EINVAL;
                return NULL;
        }
        
        file = fm_file_info_list_peek_head(files);

        if ((path = fm_file_info_get_path(file)) == NULL) {
                errno = ENOENT;
                return NULL;
        }
        
        struct hash_data *data = g_malloc0(sizeof(struct hash_data));

        data->label = gtk_label_new(NULL);
        data->hash  = gtk_label_new(NULL);
        data->path  = path;

        g_object_ref_sink(data->label);
        g_object_ref_sink(data->hash);
        fm_path_ref(data->path);

        gtk_label_set_markup(GTK_LABEL(data->label), "<b>MD5sum:</b>");
        gtk_misc_set_alignment(GTK_MISC(data->label), 0.0f, 0.5f);

        gtk_label_set_line_wrap(GTK_LABEL(data->hash), TRUE);
        gtk_label_set_selectable(GTK_LABEL(data->hash), TRUE);

        table = GTK_WIDGET(gtk_builder_get_object(ui, "general_table"));
        gtk_table_get_size(GTK_TABLE(table), &n_row, &n_col);
        gtk_table_attach_defaults(GTK_TABLE(table), data->label, 0, 1, n_row, n_row+1);
        gtk_table_attach_defaults(GTK_TABLE(table), data->hash, 2, 3, n_row, n_row+1);


        gtk_widget_show(data->label);
        gtk_widget_show(data->hash);
        
        /* Try to calculate HASH here */
        err = NULL;

        if (!hash_start_calc(data, HASH_MD5, &err)) {
                g_warning(err->message);
                goto no_output;
        }

        return data;

no_output:
        
        gtk_widget_destroy(data->label);
        gtk_widget_destroy(data->hash);
        g_object_unref(data->label);
        g_object_unref(data->hash);
        fm_path_unref(data->path);

        g_free(data);
        
        return NULL;
}

void hash_finish(gpointer pdata, gboolean cancelled)
{       
        struct hash_data *data = pdata;
        if (data) {
                gtk_widget_destroy(data->label);
                gtk_widget_destroy(data->hash);
                g_object_unref(data->label);
                g_object_unref(data->hash);
                fm_path_unref(data->path);

                g_free(data);
        }
}


FmFilePropertiesExtensionInit fm_module_init_gtk_file_prop = {
        .init = &hash_init,
        .finish = &hash_finish
};

FM_DEFINE_MODULE(gtk_file_prop, *)
