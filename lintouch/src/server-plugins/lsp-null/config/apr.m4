dnl $Id:$

dnl
dnl Try to find APR Library. If found, set APR_CFLAGS, APR_LIBS,
dnl and APR_INCLUDES variable.
dnl
dnl LNT_LIB_APR
dnl
AC_DEFUN([LNT_LIB_APR],
[
APR_CONFIG=""
AC_CHECK_PROGS(APR_CONFIG,apr-config)

if test -z "$APR_CONFIG" ; then
    AC_MSG_WARN([])
    AC_MSG_WARN([Apache Portable Library not found])
    AC_MSG_WARN([Please install APR from http://apr.apache.org])
    AC_MSG_WARN([])
    AC_MSG_ERROR([Sorry, I can't continue without APR])
fi

APR_CFLAGS=`$APR_CONFIG --cflags`
APR_LIBS=`$APR_CONFIG --link-ld --libs`
APR_INCLUDES=`$APR_CONFIG --includes`

AC_SUBST(APR_CFLAGS)
AC_SUBST(APR_LIBS)
AC_SUBST(APR_INCLUDES)
])

dnl
dnl Try to find APU Library. If found, set APU_LIBS,
dnl and APU_INCLUDES variable.
dnl
dnl LNT_LIB_APU
dnl
AC_DEFUN([LNT_LIB_APU],
[
APU_CONFIG=""
AC_CHECK_PROGS(APU_CONFIG,apu-config)

if test -z "$APU_CONFIG" ; then
    AC_MSG_WARN([])
    AC_MSG_WARN([Apache Portable Library Utils not found])
    AC_MSG_WARN([Please install APU from http://apr.apache.org])
    AC_MSG_WARN([])
    AC_MSG_ERROR([Sorry, I can't continue without APU])
fi

APU_LIBS=`$APU_CONFIG --link-ld --libs`
APU_INCLUDES=`$APU_CONFIG --includes`

AC_SUBST(APU_LIBS)
AC_SUBST(APU_INCLUDES)
])

dnl vim: set et ts=8 sw=8 tw=0:
dnl $Id:$
