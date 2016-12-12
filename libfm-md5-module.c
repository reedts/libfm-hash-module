#include <stdlib.h>
#include <stdio.h>
#include <glib.h>

#include <libfm/fm.h>
#include <libfm/fm-gtk.h>
#include <libfm/fm-file-properties.h>

struct md5_row {
        GtkWidget *label;
        GtkWidget *hash;
};



static gpointer md5_init(GtkBuilder *ui, gpointer uidata, FmFileInfoList *files)
{
        guint n_row, n_col;

        GtkWidget *md5_label;
        GtkWidget *md5_hash_label;
        GtkWidget *table;
        
        struct md5_row *row = g_malloc0(sizeof(struct md5_row));

        md5_label = GTK_LABEL(gtk_label_new(NULL));
        md5_hash_label = GTK_LABEL(gtk_label_new(NULL));

        g_object_ref_sink(md5_label);
        g_object_ref_sink(md5_hash_label);

        gtk_label_set_markup(GTK_LABEL(md5_label), "<b>MD5sum:</b>");
        gtk_misc_set_alignment(GTK_MISC(md5_label), 0.0f, 0.5f);

        table = GTK_TABLE(gtk_builder_get_object(ui, "general_table"));
        gtk_table_get_size(table, &n_row, &n_col);
        gtk_table_attach_defaults(table, md5_label, 0, 1, n_row, n_row+1);
        gtk_table_attach_defaults(table, md5_hash_label, 1, 2, n_row, n_row+1);

        gtk_widget_show(md5_label);
        gtk_widget_show(md5_hash_label);
        
        /* Try to calculate HASH here */

        row->label = md5_label;
        row->hash = md5_hash_label;

        return row;

no_output:

        gtk_widget_destroy(md5_label);
        gtk_widget_destroy(md5_hash_label);
        g_object_unref(md5_label);
        g_object_unref(md5_hash_label);

        gfree(row);
        
        return NULL;

}

static void md5_finish(gpointer pdata, gboolean cancelled)
{       
        struct md5_row *data = pdata;
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
