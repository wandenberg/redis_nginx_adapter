#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])

m4_define([redis_nginx_adapter_major_version], [1])
m4_define([redis_nginx_adapter_minor_version], [0])
m4_define([redis_nginx_adapter_micro_version], [0])
m4_define([redis_nginx_adapter_version],
          [redis_nginx_adapter_major_version.redis_nginx_adapter_minor_version.redis_nginx_adapter_micro_version])

AC_INIT([redis_nginx_adapter], [redis_nginx_adapter_version], [https://github.com/wandenberg/redis_nginx_adapter/issues])
AC_CONFIG_SRCDIR([redis_nginx_adapter.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
AC_ENABLE_SHARED
AC_DISABLE_STATIC
LT_INIT
AC_PROG_LIBTOOL(libtool)

# Checks for programs.
AC_PROG_CC

AC_ARG_WITH([nginx-dir], [
  AC_HELP_STRING([--with-nginx-dir=DIR], [NGINX project directory])
], [nginx_dir=$withval], [nginx_dir=''])

if test "$nginx_dir" != '' ; then
    eval "eval NGINX_DIR=$nginx_dir"
    AC_SUBST(NGINX_DIR)
fi

NGINX_CPPFLAGS="-I $nginx_dir/src \
 -I $nginx_dir/src/core \
 -I $nginx_dir/src/http \
 -I $nginx_dir/src/os/unix \
 -I $nginx_dir/src/event \
 -I $nginx_dir/src/http/modules \
 -I $nginx_dir/objs"
AC_SUBST(NGINX_CPPFLAGS)

CPPFLAGS="$CPPFLAGS $NGINX_CPPFLAGS"

AC_ARG_WITH([hiredis-dir], [
  AC_HELP_STRING([--with-hiredis-dir=DIR], [Hiredis install directory])
], [hiredis_dir=$withval], [hiredis_dir=''])

if test "$hiredis_dir" != '' ; then
    eval "eval HIREDIS_DIR=$hiredis_dir"
    AC_SUBST(HIREDIS_DIR)
fi

HIREDIS_CPPFLAGS="-I $hiredis_dir/include "
AC_SUBST(HIREDIS_CPPFLAGS)

CPPFLAGS="$CPPFLAGS $HIREDIS_CPPFLAGS"

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS(hiredis/hiredis.h hiredis/async.h, , [
  AC_MSG_ERROR([Couldn't find hiredis/hiredis.h... try downloading the source from https://github.com/redis/hiredis/releases])
])

AC_CHECK_HEADERS(ngx_core.h ngx_http.h, , [
  AC_MSG_ERROR([Couldn't find ngx_http.h... use the --with-nginx-dir to set the path with Nginx sources])
])

AC_CHECK_HEADERS(signal.h, , [
  AC_MSG_ERROR([Couldn't find signal.h])
])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
