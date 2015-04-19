dnl $Id: apr.m4 533 2005-04-13 08:48:54Z mman $

dnl
dnl Try to find net-snmp Library. If found, set NETSNMP_CFLAGS,
dnl NETSNMP_LIBS variable
dnl
dnl LNT_LIB_NETSNMP
dnl
AC_DEFUN([LNT_LIB_NETSNMP],
[
NETSNMP_CONFIG=""
AC_CHECK_PROGS(NETSNMP_CONFIG,net-snmp-config)

if test -z "$NETSNMP_CONFIG" ; then
    AC_MSG_WARN([])
    AC_MSG_WARN([Net-SNMP Library not found])
    AC_MSG_WARN([Please install Net-SNMP from http://www.net-snmp.org])
    AC_MSG_WARN([])
    AC_MSG_ERROR([Sorry, I can't continue without Net-SNMP])
fi

NETSNMP_CFLAGS=`$NETSNMP_CONFIG --cflags`
NETSNMP_LIBS=`$NETSNMP_CONFIG --libs`

AC_SUBST(NETSNMP_CFLAGS)
AC_SUBST(NETSNMP_LIBS)
])

dnl vim: set et ts=8 sw=8 tw=0:
dnl $Id: apr.m4 533 2005-04-13 08:48:54Z mman $
