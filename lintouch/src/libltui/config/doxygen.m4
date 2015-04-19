dnl $Id: swac-qt-mt.m4,v 1.2 2004/05/18 08:54:55 mman Exp $

dnl
dnl Try to find doxygen binary. If found, set DOXYGEN variable.
dnl
dnl LNT_PROG_DOXYGEN
dnl
AC_DEFUN([LNT_PROG_DOXYGEN],
[
DOXYGEN=""
AC_CHECK_PROGS(DOXYGEN,doxygen)

if test -z "$DOXYGEN" ; then
    AC_MSG_WARN([])
    AC_MSG_WARN([Doxygen not found])
    AC_MSG_WARN([Please install Doxygen from http://www.doxygen.org])
    AC_MSG_WARN([])
    AC_MSG_WARN([The API Documentation will not be generated])
    AC_MSG_WARN([])
fi

AC_SUBST(DOXYGEN)
])

dnl vim: set et ts=8 sw=8 tw=0:
dnl $Id: swac-qt-mt.m4,v 1.2 2004/05/18 08:54:55 mman Exp $
