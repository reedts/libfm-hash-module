#ifndef _HASH_DATA_H_
#define _HASH_DATA_H_

#include <glib.h>
#include <gtk/gtk.h>
#include <libfm/fm-path.h>

enum HASH_TYPE {
        HASH_MD5 = 0,
        HASH_COUNT
};

struct hash_data {
        GtkWidget       *label;
        GtkWidget       *hash;
        GtkWidget       *spinner;
        enum HASH_TYPE   type;
        FmPath          *path;
        GThread         *job;
};

#endif /* _HASH_DATA_H_ */
