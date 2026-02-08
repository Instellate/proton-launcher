#!/usr/bin/env bash

# https://techbase.kde.org/Development/Tutorials/Localization/i18n_Build_Systems/Outside_KDE_repositories
# the command extractrc is provided by the package kde-dev-scripts

# https://api.kde.org/frameworks/ki18n/html/prg_guide.html

set -euo pipefail

BASEDIR=`pwd`	# root of translatable sources
PROJECT="proton-launcher"	# project name
BUGADDR="https://github.com/Instellate/proton-launcher/issues"	# MSGID-Bugs
WDIR=`pwd`		# working dir

echo "Preparing rc files"
cd ${BASEDIR}
# we use simple sorting to make sure the lines do not jump around too much from system to system
find src qml -name '*.rc' -o -name '*.ui' | sort > ${WDIR}/rcfiles.list
xargs --arg-file=${WDIR}/rcfiles.list extractrc > ${WDIR}/rc.cpp
echo "Done preparing rc files"

echo "Preparing source files: C++, C, QML..."
# see above on sorting
find src qml -name '*.cpp' -o -name '*.hpp' -o -name '*.h' -o -name '*.c' -o -name '*.qml' \
  | sort > ${WDIR}/infiles.list

echo "rc.cpp" >> ${WDIR}/infiles.list

# extracting messages on xgettext
cd ${WDIR}
xgettext --from-code=UTF-8 -C -kde -ci18n -ki18n:1 -ki18nc:1c,2 -ki18np:1,2 -ki18ncp:1c,2,3 -ktr2i18n:1 \
	-kI18N_NOOP:1 -kI18N_NOOP2:1c,2 -kaliasLocale -kki18n:1 -kki18nc:1c,2 -kki18np:1,2 -kki18ncp:1c,2,3 \
  -kN_:1 \
	--msgid-bugs-address="${BUGADDR}" \
	--files-from=infiles.list -D ${BASEDIR} -D ${WDIR} -o ${BASEDIR}/po/${PROJECT}.pot || { echo "error while calling xgettext. aborting."; exit 1; }

echo "Done extracting messages"

echo "Merging translations"

cd ${BASEDIR}/po
catalogs=`find . -name '*.po'`
for catalog in $catalogs; do
  echo $catalog
  msgmerge -o $catalog.new $catalog ${PROJECT}.pot
  mv $catalog.new $catalog
done

cd ${WDIR}

echo "Done merging translations"

echo "Cleaning up"

#cp -v infiles.list /tmp

rm rcfiles.list
rm infiles.list
rm rc.cpp

echo "Done"

