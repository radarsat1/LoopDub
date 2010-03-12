#!/bin/sh

# Run all necessary autotools

if ! autoconf --version >/dev/null; then
    echo No autoconf found.
    exit 1
fi

if ! automake --version >/dev/null; then
    echo No automake found.
    exit 1
fi

if ! autoheader --version >/dev/null; then
    echo No autoheader found.
    exit 1
fi

if ! libtoolize --version >/dev/null; then
    echo No libtoolize found.
    exit 1
fi

if ! aclocal --version >/dev/null; then
    echo No aclocal found.
    exit 1
fi

############

if ! libtoolize --force; then
    echo Error running libtoolize.
    exit 1
fi

if ! aclocal; then
    echo Error running aclocal.
    exit 1
fi

if ! autoheader; then
    echo Error running autoheader.
    exit 1
fi

if ! automake --add-missing; then
    echo Error running automake.
    exit 1
fi

if ! autoconf; then
    echo Error running autoconf.
    exit 1
fi

CONF=--enable-maintainer-mode
echo Running configure with options $CONF $@
sh configure --enable-maintainer-mode $@
