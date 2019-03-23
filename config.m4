dnl $Id$
dnl config.m4 for extension post_handler

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(post_handler, for post_handler support,
dnl Make sure that the comment is aligned:
dnl [  --with-post_handler             Include post_handler support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(post_handler, whether to enable post_handler support,
Make sure that the comment is aligned:
[  --enable-post_handler           Enable post_handler support])

if test "$PHP_POST_HANDLER" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-post_handler -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/post_handler.h"  # you most likely want to change this
  dnl if test -r $PHP_POST_HANDLER/$SEARCH_FOR; then # path given as parameter
  dnl   POST_HANDLER_DIR=$PHP_POST_HANDLER
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for post_handler files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       POST_HANDLER_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$POST_HANDLER_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the post_handler distribution])
  dnl fi

  dnl # --with-post_handler -> add include path
  dnl PHP_ADD_INCLUDE($POST_HANDLER_DIR/include)

  dnl # --with-post_handler -> check for lib and symbol presence
  dnl LIBNAME=post_handler # you may want to change this
  dnl LIBSYMBOL=post_handler # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $POST_HANDLER_DIR/$PHP_LIBDIR, POST_HANDLER_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_POST_HANDLERLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong post_handler lib version or lib not found])
  dnl ],[
  dnl   -L$POST_HANDLER_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(POST_HANDLER_SHARED_LIBADD)

  PHP_NEW_EXTENSION(post_handler, post_handler.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
