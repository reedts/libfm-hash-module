#ifndef _HASH_DATA_H_
#define _HASH_DATA_H_

#include <glib.h>
#include <gtk/gtk.h>
#include <libfm/fm-path.h>

struct hash_data {
        GtkWidget *label;
        GtkWidget *hash;
        FmPath    *path;
        GThread   *job;
};

#endif /* _HASH_DATA_H_ */
