#!/usr/bin/env bash
# =============================================================================
#
#   @file scripts/formatCode.sh
#
#   This is a helper script to format the code present in HAN-FUN source
#   repository using the library's configuration for uncrustify.
#
#   @version    1.4.1
#
#   @copyright  Copyright (c) 2014  ULE Alliance
#
#   For licensing information, please see the file 'LICENSE' in the root folder.
#
#   Initial development by Bithium S.A. [http://www.bithium.com]
#
# =============================================================================

FILES="/tmp/$(basename $0).$$.tmp"

PROJECT_DIR="$(git rev-parse --show-toplevel)"

pushd $PROJECT_DIR > format.log

# =============================================================================
# Configuration
# =============================================================================


UNCRUSTIFY_CONFIG="$PROJECT_DIR/scripts/uncrustify.cfg"

if [[ -e "$UNCRUSTIFY_CONFIG" && -f "$UNCRUSTIFY_CONFIG" ]]; then
	UNCRUSTIFY_CONFIG="-c $UNCRUSTIFY_CONFIG"
else
	echo "ERROR : Uncrustify configuration not found !"
  exit -1
fi


echo -n "Generating file list ... "
git ls-files --exclude-standard $@ | grep ".*\.\(c\(pp\)\?\|h\)$" > $FILES
echo "done !"

NFILES=$(wc -l $FILES | cut -f 1 -d \ )

echo -ne "Running uncrustify ... \r"

uncrustify $UNCRUSTIFY_CONFIG -F $FILES --no-backup 2>&1 | tee -a format.log | \
	awk -v nfiles=$NFILES -v cnt=1 '/Parsing/{ printf("Running uncrustify ... [%d/%d]\r", cnt++, nfiles ) }'

grep "Error" format.log
if [ $? -eq 0 ]; then
	echo "Running uncrustify ..... failed ! "
	echo "There was an error while formating the code. Please check the log (format.log)"
	exit
fi
echo "Running uncrustify ..... done !   "

echo -ne "Doing some more tweaks ... \r"

# Cleanup.
rm -rf $FILES

popd >> format.log
