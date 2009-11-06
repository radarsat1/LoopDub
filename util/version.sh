#!/bin/sh

# This script produces a version number for the current git branch,
# based on the number of commits since the last-tagged version, the
# current branch, and any uncommitted changes.

BRANCH="$(git branch | grep \* | sed s,/,_,g | awk '{print $2}')"

LASTTAG="$(git tag -l | perl -nle 'print $_  if m/\d+\.\d+(\.\d+)*/' | head -n 1)"
if [ "$LASTTAG"x = x ]; then
    LASTTAG="0"
    INCREMENT="-0"
else
    INCREMENT=-$(git rev-list "$LASTTAG"..HEAD | wc -l | awk '{print $1}')
    if [ "$INCREMENT"x = "-0" ]; then
        INCREMENT=""
    fi
fi

BRANCH=-"$BRANCH"
if [ "$BRANCH"x = -masterx ]; then
    BRANCH=""
fi

DIFF=""
if [ "$(git diff | head -n 5)"x != x ]; then

    # determine checksum program to use
    SUM=$(which sha1sum | awk '{print $1}')
    if [ "$SUM"x = x -o "$SUM"x = nox ]; then
		SUM=$(which md5sum | awk '{print $1}')
    fi
    if [ "$SUM"x = x -o "$SUM"x = nox ]; then
		SUM=$(which md5 | awk '{print $1}')
    fi

    # checksum differences between index and HEAD
    if [ "$SUM"x = x -o "$SUM" = nox ]; then
        DIFF="-dirty"
    else
        DIFF=-"$(git diff | $SUM | awk '{print substr($1,0,8)}')"
    fi

fi

echo $LASTTAG$INCREMENT$BRANCH$DIFF
