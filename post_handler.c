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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_post_handler.h"

#include "php_variables.h"
#include "php_globals.h"
#include "php_main.h"
#include "rfc1867.h"
#include "php_content_types.h"
#include "SAPI.h"

ZEND_DECLARE_MODULE_GLOBALS(post_handler)

static int le_post_handler;

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("post_handler.enable",      "0", PHP_INI_ALL, OnUpdateLong, enable, zend_post_handler_globals, post_handler_globals)
    STD_PHP_INI_ENTRY("post_handler.handler_type",      "1", PHP_INI_ALL, OnUpdateLong, handler_type, zend_post_handler_globals, post_handler_globals)
    STD_PHP_INI_ENTRY("post_handler.handler", "", PHP_INI_ALL, OnUpdateString, handler, zend_post_handler_globals, post_handler_globals)
PHP_INI_END()

/* }}} */

SAPI_API int sapi_unregister_post_entries(sapi_post_entry *post_entries)
{
	sapi_post_entry *p=post_entries;

	while (p->content_type) {
		sapi_unregister_post_entry(p);
		p++;
	}
	return SUCCESS;
}

SAPI_API SAPI_POST_READER_FUNC(post_handler_read_handler) /* {{{ */
{
	if (post_handler_globals.enable <= 0) {
		return;
	}

	zend_file_handle file_handle;
	char *script_file=NULL;
	int lineno = 0;
	volatile int exit_status = 0;

	zval_ptr_dtor(&PG(http_globals)[TRACK_VARS_SERVER]);
	array_init(&PG(http_globals)[TRACK_VARS_SERVER]);

	if (sapi_module.register_server_variables) {
		sapi_module.register_server_variables(&PG(http_globals)[TRACK_VARS_SERVER]);

		zval *values_entry;
		zend_string	*string_key;

		zend_hash_update(&EG(symbol_table), zend_string_init("_SERVER", sizeof("_SERVER")-1, 1), &PG(http_globals)[TRACK_VARS_SERVER]);
		Z_ADDREF(PG(http_globals)[TRACK_VARS_SERVER]);
	}

	if (post_handler_globals.handler_type == 1) {
		if (post_handler_globals.handler == NULL) {
			return;
		}

		if (post_handler_file_begin(&file_handle, post_handler_globals.handler, &lineno) != SUCCESS) {
			return;
		}

		php_execute_script(&file_handle);
		exit_status = EG(exit_status);
	} else {
		if (post_handler_globals.handler == NULL) {
			return;
		}
		if (zend_eval_string_ex(post_handler_globals.handler, NULL, "post_handler handler", 1) == FAILURE) {
		}
		exit_status = EG(exit_status);
	}
	if (exit_status > 0) {
			zend_bailout();
		}
}

SAPI_API SAPI_POST_HANDLER_FUNC(post_handler_post_handler) /* {{{ */
{
	rfc1867_post_handler(content_type_dup, arg);
}

static sapi_post_entry php_post_entries[] = {
	{ DEFAULT_POST_CONTENT_TYPE, sizeof(DEFAULT_POST_CONTENT_TYPE)-1, sapi_read_standard_form_data,	php_std_post_handler },
	{ MULTIPART_CONTENT_TYPE,    sizeof(MULTIPART_CONTENT_TYPE)-1,    NULL,                         rfc1867_post_handler },
	{ NULL, 0, NULL, NULL }
};

static sapi_post_entry post_handler_post_entries[] = {
	{ DEFAULT_POST_CONTENT_TYPE, sizeof(DEFAULT_POST_CONTENT_TYPE)-1, sapi_read_standard_form_data, php_std_post_handler },
	{ MULTIPART_CONTENT_TYPE,    sizeof(MULTIPART_CONTENT_TYPE)-1,    post_handler_read_handler, post_handler_post_handler },
	{ NULL, 0, NULL, NULL }
};


/* {{{ cli_seek_file_begin
 */
static int post_handler_file_begin(zend_file_handle *file_handle, char *script_file, int *lineno)
{
	int c;

	*lineno = 1;

	file_handle->type = ZEND_HANDLE_FP;
	file_handle->opened_path = NULL;
	file_handle->free_filename = 0;
	if (!(file_handle->handle.fp = VCWD_FOPEN(script_file, "rb"))) {
		php_error_docref(NULL, E_ERROR, "Could not open input file %s", script_file);
		return FAILURE;
	}
	file_handle->filename = script_file;

	/* #!php support */
	c = fgetc(file_handle->handle.fp);
	if (c == '#' && (c = fgetc(file_handle->handle.fp)) == '!') {
		while (c != '\n' && c != '\r' && c != EOF) {
			c = fgetc(file_handle->handle.fp);	/* skip to end of line */
		}
		/* handle situations where line is terminated by \r\n */
		if (c == '\r') {
			if (fgetc(file_handle->handle.fp) != '\n') {
				zend_long pos = zend_ftell(file_handle->handle.fp);
				zend_fseek(file_handle->handle.fp, pos - 1, SEEK_SET);
			}
		}
		*lineno = 2;
	} else {
		rewind(file_handle->handle.fp);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ php_post_handler_init_globals
 */
static void php_post_handler_init_globals(zend_post_handler_globals *post_handler_globals)
{
	post_handler_globals->enable = 0;
	post_handler_globals->handler_type = 1;
	post_handler_globals->handler = NULL;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(post_handler)
{
	REGISTER_INI_ENTRIES();

	if (post_handler_globals.enable <= 0) {
		return SUCCESS;
	}

	sapi_unregister_post_entries(php_post_entries);
	sapi_register_post_entries(post_handler_post_entries);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(post_handler)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(post_handler)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(post_handler)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(post_handler)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "post_handler support", "enabled");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ post_handler_functions[]
 *
 */
// const zend_function_entry post_handler_functions[] = {
// 	PHP_FE_END	/* Must be the last line in post_handler_functions[] */
// };
/* }}} */

/* {{{ post_handler_module_entry
 */
zend_module_entry post_handler_module_entry = {
	STANDARD_MODULE_HEADER,
	"post_handler",
	NULL,
	PHP_MINIT(post_handler),
	PHP_MSHUTDOWN(post_handler),
	PHP_RINIT(post_handler),
	PHP_RSHUTDOWN(post_handler),
	PHP_MINFO(post_handler),
	PHP_POST_HANDLER_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */


#ifdef COMPILE_DL_POST_HANDLER
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(post_handler)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
