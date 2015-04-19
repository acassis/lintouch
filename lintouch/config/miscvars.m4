dnl $Id: miscvars.m4 347 2005-02-23 14:56:10Z modesto $

dnl
dnl set TODAY
dnl
AC_DEFUN([LNT_VAR_TODAY],
[
TODAY=`date +%Y-%m-%d`
AC_SUBST(TODAY)
])

dnl
dnl set PREFIX and PREFIX_NATIVE
dnl
AC_DEFUN([LNT_VAR_PREFIX],
[
dnl prefix converted to native os format
mypref=$prefix
test "x$mypref" = xNONE && mypref=$ac_default_prefix
if test "$build" == "i686-pc-mingw32" ; then
        PREFIX="$mypref"
        PREFIX_NATIVE="$(winpath $mypref)"
else
        PREFIX="$mypref"
        PREFIX_NATIVE="$mypref"
fi
AC_SUBST(PREFIX)
AC_SUBST(PREFIX_NATIVE)
])

dnl
dnl LNT_VAR_CONDITIONAL(TG, ARG, COND)
dnl LNT_VAR_CONDITIONAL(OPT_MAKE_TARGET, "refreshdeps", test $host=="i686-pc-mingw32" )
dnl set VAR to ARG in case COND is true
dnl
AC_DEFUN([LNT_VAR_CONDITIONAL],
[
if $3; then
        $1=$2
else
        $1=""
fi
AC_SUBST($1)
])


dnl vim: set et ts=8 sw=8 tw=0:
dnl $Id: miscvars.m4 347 2005-02-23 14:56:10Z modesto $
