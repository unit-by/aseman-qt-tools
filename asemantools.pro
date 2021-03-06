TEMPLATE = lib
TARGET = AsemanToolsQml
QT += qml quick widgets
CONFIG += qt plugin

isEmpty(ASEMAN_BUILD_DEST) {
    DESTDIR = qml/AsemanTools/AsemanTools
} else {
    DESTDIR = $$ASEMAN_BUILD_DEST/qml/AsemanTools
}

#VERSION = 1.0.0
TARGET = $$qtLibraryTarget($$TARGET)
uri = AsemanTools

DEFINES += ASEMAN_QML_PLUGIN QT_MESSAGELOGCONTEXT

include(asemantools.pri)

SOURCES += \
    asemantoolsplugin.cpp

HEADERS += \
    asemantoolsplugin.h

QMLDIR_VALUE = $$cat(qmldir, blob) 'plugin AsemanToolsQml'
write_file($$OUT_PWD/$$DESTDIR/qmldir, QMLDIR_VALUE)

qmlFiles.source = qml/AsemanTools/
qmlFiles.target = $$DESTDIR/../
COPYFOLDERS += qmlFiles

!win32-msvc* {
    include (qmake/copyData.pri)
    copyData ()
}

isEmpty(PREFIX) {
    PREFIX = $$[QT_INSTALL_QML]
}

installPath = $$PREFIX/$$replace(uri, \\., /)
qmldir.files = $$OUT_PWD/$$DESTDIR/qmldir $$OUT_PWD/$$DESTDIR/plugins.qmltypes
qmldir.path = $$installPath
qmlFile.files = qml/AsemanTools/
qmlFile.path = $$PREFIX
target = $$TARGET
target.path = $$installPath
INSTALLS += qmlFile qmldir target
