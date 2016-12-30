/*!
 * \file hash-module.h
 */

#ifndef _HASH_MODULE_H_
#define _HASH_MODULE_H_

#include <libfm/fm.h>
#include <libfm/fm-gtk.h>



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
extern gpointer hash_init(GtkBuilder *ui, gpointer uidata, FmFileInfoList *files);

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
extern void hash_finish(gpointer pdata, gboolean cancelled);


extern FmFilePropertiesExtensionInit fm_module_init_gtk_file_prop;


#endif /* _HASH_MODULE_H_ */
