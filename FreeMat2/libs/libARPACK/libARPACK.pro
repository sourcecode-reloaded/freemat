TEMPLATE = lib

TARGET = ARPACK

CONFIG += staticlib

HEADERS += SRC/debug.h \
SRC/stat.h \
SRC/version.h

SOURCES += SRC/cgetv0.f \
SRC/cnaitr.f \
SRC/cnapps.f \
SRC/cnaup2.f \
SRC/cnaupd.f \
SRC/cneigh.f \
SRC/cneupd.f \
SRC/cngets.f \
SRC/csortc.f \
SRC/cstatn.f \
SRC/dgetv0.f \
SRC/dlaqrb.f \
SRC/dnaitr.f \
SRC/dnapps.f \
SRC/dnaup2.f \
SRC/dnaupd.f \
SRC/dnaupe.f \
SRC/dnconv.f \
SRC/dneigh.f \
SRC/dneupd.f \
SRC/dngets.f \
SRC/dsaitr.f \
SRC/dsapps.f \
SRC/dsaup2.f \
SRC/dsaupd.f \
SRC/dsconv.f \
SRC/dseigt.f \
SRC/dsesrt.f \
SRC/dseupd.f \
SRC/dsgets.f \
SRC/dsortc.f \
SRC/dsortr.f \
SRC/dstatn.f \
SRC/dstats.f \
SRC/dstqrb.f \
SRC/sgetv0.f \
SRC/slaqrb.f \
SRC/snaitr.f \
SRC/snapps.f \
SRC/snaup2.f \
SRC/snaupd.f \
SRC/snaupe.f \
SRC/snconv.f \
SRC/sneigh.f \
SRC/sneupd.f \
SRC/sngets.f \
SRC/ssaitr.f \
SRC/ssapps.f \
SRC/ssaup2.f \
SRC/ssaupd.f \
SRC/ssconv.f \
SRC/sseigt.f \
SRC/ssesrt.f \
SRC/sseupd.f \
SRC/ssgets.f \
SRC/ssortc.f \
SRC/ssortr.f \
SRC/sstatn.f \
SRC/sstats.f \
SRC/sstqrb.f \
SRC/zgetv0.f \
SRC/znaitr.f \
SRC/znapps.f \
SRC/znaup2.f \
SRC/znaupd.f \
SRC/zneigh.f \
SRC/zneupd.f \
SRC/zngets.f \
SRC/zsortc.f \
SRC/zstatn.f \
UTIL/cmout.f \
UTIL/cvout.f \
UTIL/dmout.f \
UTIL/dvout.f \
UTIL/icnteq.f \
UTIL/icopy.f \
UTIL/iset.f \
UTIL/iswap.f \
UTIL/ivout.f \
UTIL/second.f \
UTIL/smout.f \
UTIL/svout.f \
UTIL/zmout.f \
UTIL/zvout.f 
