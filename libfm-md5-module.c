#include <stdlib.h>
#include <stdio.h>

#include <libfm/fm.h>
#include <libfm/fm-gtk.h>
#include <libfm/fm-file-properties.h>



static gpointer md5_init(GtkBuilder *ui, gpointer uidata, FmFileInfoList *files)
{
        guint n_row, n_col;

        GtkWidget *md5_lb;
        GtkWidget *table;

        md5_lb = GTK_LABEL(gtk_label_new(NULL));
        gtk_label_set_markup(GTK_LABEL(md5_lb), "<b>MD5sum:</b>");
        gtk_misc_set_alignment(GTK_MISC(md5_lb), 0.0f, 0.5f);

        g_object_ref_sink(md5_lb);

        table = GTK_TABLE(gtk_builder_get_object(ui, "general_table"));
        gtk_table_get_size(table, &n_row, &n_col);

        gtk_table_attach_defaults(table, md5_lb, 0, 2, n_row, n_row+1);

        gtk_widget_show(md5_lb);

        return md5_lb;
}

static void md5_finish(gpointer data, gboolean cancelled)
{
        gtk_widget_destroy(data);
        g_object_unref(data);
}

FmFilePropertiesExtensionInit fm_module_init_gtk_file_prop = {
        .init = &md5_init,
        .finish = &md5_finish
};


FM_DEFINE_MODULE(gtk_file_prop, *)
