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
static inline gboolean md5_check_file(FmFileInfoList *files)
{
        FmFileInfo *file = fm_file_info_list_peek_head(files);
        return ((fm_file_info_list_get_length(files) == 1) &&
                (!fm_file_info_is_dir(file)) &&
                (!fm_file_info_is_symlink(file))
               );   
}


/**
 * @brief Initialises the md5 module.
 *
 * The function \p md5_init initialises the md5 module using the provided
 * parameters passed by libfm. It will initialise the needed data structures
 * and calculate the hash of the given file using \p md5_calc_hash but only,
 * if \p md5_check_file gives it ok. It will otherwise return \p NULL , if
 * no valid file is specified or the hash could not be calculated because of
 * other reasons.
 *
 * @param ui \p GtkBuilder providing functionality to interact with the GUI
 *           of the file properties window.
 * @param files \p FmFileInfoList containing all selected files.
 *
 * @return A \p gpointer to the \code struct md5_row \endcode containing
 *         pointers to \p GtkWidget s with label and hash.
 */
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


        if (!hash) {
                g_warning(err->message);
                goto no_output;
        }

        if (strlen(hash) == 0) {
                g_warning("Got empty hash!");
                return NULL;
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

/**
 * @brief Finalizes the md5 module.
 *
 * The function \p md5_finish finalizes the md5 module in such way that all
 * used resources are freed. All \p GtkWidgets used by this module will be 
 * unreferenced via \p g_object_unref . 
 *
 * @param pdata Pointer to a \code struct md5_row \endcode containing
 *              Pointers to widgets used by this module.
 */
static void md5_finish(gpointer pdata, gboolean cancelled)
{       
        struct md5_row *data = pdata;
        if (data) {
                gtk_widget_destroy(data->label);
                gtk_widget_destroy(data->hash);
                g_object_unref(data->label);
                g_object_unref(data->hash);

                free(data);
        }
}

FmFilePropertiesExtensionInit fm_module_init_gtk_file_prop = {
        .init = &md5_init,
        .finish = &md5_finish
};


FM_DEFINE_MODULE(gtk_file_prop, *)
