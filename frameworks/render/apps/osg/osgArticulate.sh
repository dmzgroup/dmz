#!/bin/sh

DEPTH="../../../../.."

if [ -f "$DEPTH/scripts/envsetup.sh" ] ; then
. ../../../../../scripts/envsetup.sh
elif [ -f "$HOME/cm/src/scripts/envsetup.sh" ] ; then
. $HOME/cm/src/scripts/envsetup.sh
fi

$DMZ_BIN_PATH/osgArticulate "$@"
