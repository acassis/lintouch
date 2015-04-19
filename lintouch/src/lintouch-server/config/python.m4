dnl $Id: python.m4 347 2005-02-23 14:56:10Z modesto $

dnl
dnl Try to find python installation. If found, set PYTHON_INCLUDES and
dnl PYTHON_LIBS variable.
dnl
dnl LNT_LIB_PYTHON
dnl
AC_DEFUN([LNT_LIB_PYTHON],
[
AM_PATH_PYTHON(2.3)
if test "$build" == "i686-pc-mingw32" ; then
    PYTHON_INCLUDES="-I`PYTHONBASE=$(which python); echo ${PYTHONBASE%/*}/include`"
    PYTHON_LIBS="`PYTHONBASE=$(which python); echo ${PYTHONBASE%/*}/libs/python*.lib`"
    DISTUTILS_BUILD_FLAGS="--compiler=mingw32"
    DISTUTILS_INSTALL_DIR="`echo \\$\\(shell winpath \\$\\(DESTDIR\\)\\$\\{prefix\\} \\| sed \\'s:/:\\\\\\\\\\\\\\\\:g\\'\\)`"
    AC_SUBST(PYTHON)
    AC_SUBST(PYTHON_INCLUDES)
    AC_SUBST(PYTHON_LIBS)
else
    AM_CHECK_PYTHON_HEADERS
    AM_CHECK_PYTHON_LIB

    if test -z "$PYTHON_INCLUDES" ; then
        AC_MSG_WARN([])
        AC_MSG_WARN([Python headers not found])
        AC_MSG_WARN([Please install Python from http://www.python.org])
        AC_MSG_WARN([])
        AC_MSG_ERROR([Sorry, I can't continue without Python headers])
    fi

    if test -z "$PYTHON_LIBS" ; then
        AC_MSG_WARN([])
        AC_MSG_WARN([Python libraries not found])
        AC_MSG_WARN([Please install Python from http://www.python.org])
        AC_MSG_WARN([])
        AC_MSG_ERROR([Sorry, I can't continue without Python libraries])
    fi
    DISTUTILS_INSTALL_DIR="`echo \\$\\(DESTDIR\\)\\$\\{prefix\\}`"
fi
AC_SUBST(DISTUTILS_BUILD_FLAGS)
AC_SUBST(DISTUTILS_INSTALL_DIR)
])

dnl vim: set et ts=8 sw=8 tw=0:
dnl $Id: python.m4 347 2005-02-23 14:56:10Z modesto $
