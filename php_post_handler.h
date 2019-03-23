/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_POST_HANDLER_H
#define PHP_POST_HANDLER_H

extern zend_module_entry post_handler_module_entry;
#define phpext_post_handler_ptr &post_handler_module_entry

#define PHP_POST_HANDLER_VERSION "0.1.0"

#ifdef PHP_WIN32
#	define PHP_POST_HANDLER_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_POST_HANDLER_API __attribute__ ((visibility("default")))
#else
#	define PHP_POST_HANDLER_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif


ZEND_BEGIN_MODULE_GLOBALS(post_handler)
	zend_long  enable;
	zend_long  handler_type;
	char *handler;
ZEND_END_MODULE_GLOBALS(post_handler)

#define POST_HANDLER_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(post_handler, v)

#if defined(ZTS) && defined(COMPILE_DL_POST_HANDLER)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

static int post_handler_file_begin(zend_file_handle *file_handle, char *script_file, int *lineno);

#endif	/* PHP_POST_HANDLER_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
