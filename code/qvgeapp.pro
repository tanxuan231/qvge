# This file is a part of
# QVGE - Qt Visual Graph Editor
#
# (c) 2016-2018 Ars L. Masiuk (ars.masiuk@gmail.com)
#
# It can be used freely, maintaining the information above.


TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += qvge
qvge.file = $$PWD/qvge/qvge.pro

SUBDIRS += src
src.file = $$PWD/src/src.pro
