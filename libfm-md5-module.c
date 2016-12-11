#include <stdlib.h>
#include <stdio.h>

#include <libfm/fm.h>
#include <libfm/fm-gtk.h>
#include <libfm/fm-file-properties.h>



static gpointer md5_init(GtkBuilder *ui, gpointer uidata, FmFileInfoList *files)
{

        fprintf(stderr, "Init...\n");

        GtkWidget *md5_lb;

        md5_lb = gtk_label_new("MD5");

        gtk_container_add(GTK_CONTAINER(ui), md5_lb);

        return md5_lb;
}

static void md5_finish(gpointer data, gboolean cancelled)
{
        g_free(data);
}

FmFilePropertiesExtensionInit fm_module_init_gtk_file_prop = {
        .init = &md5_init,
        .finish = &md5_finish
};


FM_DEFINE_MODULE(gtk_file_prop, all/allfiles)
