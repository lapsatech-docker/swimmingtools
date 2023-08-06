#ifndef PHP_SWT_H
/* Prevent double inclusion */
#define PHP_SWT_H

/* Define Extension Properties */
#define PHP_SWT_EXTNAME    "swt"
#define PHP_SWT_EXTVER    "1.0"

/* Import configure options
   when building outside of
   the PHP source tree */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* Include PHP Standard Header */
#include "php.h"

/* Define the entry point symbol
 * Zend will use when loading this module
 */
extern zend_module_entry swt_module_entry;
#define phpext_swt_ptr &swt_module_entry

#define PHP_SWT_DESCRIPTOR_RES_NAME "SwimFile Descriptor"
static int le_swt_descriptor;

#endif /* PHP_SWT_H */
