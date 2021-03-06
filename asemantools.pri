QT += qml quick
CONFIG +=  c++11
contains(DEFINES,ASEMAN_TOOLS_FULL) {
    QT += multimedia sensors positioning widgets
    !android: linux: QT += dbus
}

android {
    manifest.source = android-build
    manifest.target = .
    COPYFOLDERS += manifest
    include(qmake/copyData.pri)
    copyData()

    QT += androidextras
    SOURCES += \
        $$PWD/asemanjavalayer.cpp \
        $$PWD/asemanandroidservice.cpp \
        $$PWD/private/asemanandroidstoremanagercore.cpp \
        $$PWD/private/asemanandroidcameracapturecore.cpp

    HEADERS += \
        $$PWD/asemanjavalayer.h \
        $$PWD/asemanandroidservice.h \
        $$PWD/private/asemanandroidstoremanagercore.h \
        $$PWD/private/asemanandroidcameracapturecore.h
} else {
    ios {

    } else {
        contains(BUILD_MODE,ubuntutouch) {
            DEFINES += Q_OS_UBUNTUTOUCH
        } else {
        contains(QT,widgets) {

            HEADERS += \
                $$PWD/qtsingleapplication/qtsinglecoreapplication.h \
                $$PWD/qtsingleapplication/qtsingleapplication.h

            SOURCES += \
                $$PWD/qtsingleapplication/qtsinglecoreapplication.cpp \
                $$PWD/qtsingleapplication/qtsingleapplication.cpp

            win32: SOURCES += $$PWD/qtsingleapplication/qtlockedfile_win.cpp
            unix:  SOURCES += $$PWD/qtsingleapplication/qtlockedfile_unix.cpp
        }
        }
    }
}

QML_IMPORT_PATH = \
    $$PWD/qml/

!contains(DEFINES,DISABLE_KEYCHAIN) {
    LIBS += -lqt5keychain
    SOURCES += $$PWD/asemankeychain.cpp
    HEADERS += $$PWD/asemankeychain.h
}
contains(QT,macextras) {
    SOURCES += $$PWD/private/asemanmactaskbarbuttonengine.cpp
    HEADERS += $$PWD/private/asemanmactaskbarbuttonengine.h
}
contains(QT,winextras) {
    SOURCES += $$PWD/private/asemanwintaskbarbuttonengine.cpp
    HEADERS += $$PWD/private/asemanwintaskbarbuttonengine.h
}
contains(QT,sensors) {
    DEFINES += ASEMAN_SENSORS
    SOURCES += $$PWD/asemansensors.cpp
    HEADERS += $$PWD/asemansensors.h
}
contains(QT,widgets) {
    DEFINES += NATIVE_ASEMAN_NOTIFICATION
    SOURCES +=  \
        $$PWD/asemannativenotification.cpp \
        $$PWD/asemannativenotificationitem.cpp \
        $$PWD/asemansystemtray.cpp
    HEADERS +=  \
        $$PWD/asemannativenotification.h \
        $$PWD/asemannativenotificationitem.h \
        $$PWD/asemansystemtray.h
}
contains(QT,multimedia) {
    DEFINES += ASEMAN_MULTIMEDIA
    SOURCES +=  \
        $$PWD/asemanaudiorecorder.cpp \
        $$PWD/asemanaudioencodersettings.cpp
    HEADERS +=  \
        $$PWD/asemanaudiorecorder.h \
        $$PWD/asemanaudioencodersettings.h
}
contains(QT,webkitwidgets) {
    DEFINES += ASEMAN_WEBKIT
}
contains(QT,webenginewidgets) {
    DEFINES += ASEMAN_WEBENGINE
}
contains(QT,positioning) {
    DEFINES += ASEMAN_POSITIONING
    SOURCES += \
        $$PWD/asemanlocationlistener.cpp \
        $$PWD/private/asemanabstractlocationlistenercore.cpp \
        $$PWD/private/asemanqtlocationlistenercore.cpp
    HEADERS += \
        $$PWD/asemanlocationlistener.h \
        $$PWD/private/asemanabstractlocationlistenercore.h \
        $$PWD/private/asemanqtlocationlistenercore.h
    android {
        SOURCES += \
            $$PWD/private/asemanandroidlocationlistenercore.cpp
        HEADERS += \
            $$PWD/private/asemanandroidlocationlistenercore.h
    }
}
linux|openbsd {
contains(QT,dbus) {
    DEFINES += LINUX_NATIVE_ASEMAN_NOTIFICATION
    SOURCES += $$PWD/asemanlinuxnativenotification.cpp \
        $$PWD/private/asemanunitytaskbarbuttonengine.cpp \
        $$PWD/asemankdewallet.cpp
    HEADERS += $$PWD/asemanlinuxnativenotification.h \
        $$PWD/private/asemanunitytaskbarbuttonengine.h \
        $$PWD/asemankdewallet.h
}
}
macx {
    !contains(DEFINES, DISABLE_CORE_SERVICES) {
        LIBS += -framework CoreServices
        INCLUDEPATH += /System/Library/Frameworks/CoreServices.framework/Headers/
        DEFINES += OSX_CORE_SERVICES_AVAILABLE
    }
    !contains(QMAKE_HOST.arch, x86_64) {
        LIBS +=  -framework CoreFoundation -framework Carbon -lobjc
    }

    DEFINES += MAC_NATIVE_ASEMAN_NOTIFICATION
    SOURCES += $$PWD/asemanmacnativenotification.cpp
    HEADERS += $$PWD/asemanmacnativenotification.h
}

SOURCES += \
    $$PWD/asemandevices.cpp \
    $$PWD/asemanqtlogger.cpp \
    $$PWD/asemantools.cpp \
    $$PWD/asemandesktoptools.cpp \
    $$PWD/asemanlistobject.cpp \
    $$PWD/asemanhashobject.cpp \
    $$PWD/asemanquickview.cpp \
    $$PWD/asemanapplication.cpp \
    $$PWD/asemancalendarconvertercore.cpp \
    $$PWD/asemancalendarconverter.cpp \
    $$PWD/asemanbackhandler.cpp \
    $$PWD/asemansysteminfo.cpp \
    $$PWD/asemanabstractcolorfulllistmodel.cpp \
    $$PWD/asemanimagecoloranalizor.cpp \
    $$PWD/asemancountriesmodel.cpp \
    $$PWD/asemanmimedata.cpp \
    $$PWD/asemanmimeapps.cpp \
    $$PWD/asemandragobject.cpp \
    $$PWD/asemandownloader.cpp \
    $$PWD/asemannotification.cpp \
    $$PWD/asemanautostartmanager.cpp \
    $$PWD/asemanquickitemimagegrabber.cpp \
    $$PWD/asemanquickobject.cpp \
    $$PWD/asemanfilesystemmodel.cpp \
    $$PWD/asemandebugobjectcounter.cpp \
    $$PWD/asemanfiledownloaderqueue.cpp \
    $$PWD/asemanfiledownloaderqueueitem.cpp \
    $$PWD/asemanwebpagegrabber.cpp \
    $$PWD/asemantitlebarcolorgrabber.cpp \
    $$PWD/asemantaskbarbutton.cpp \
    $$PWD/private/asemanabstracttaskbarbuttonengine.cpp \
    $$PWD/asemanmapdownloader.cpp \
    $$PWD/asemandragarea.cpp \
    $$PWD/asemanabstractlistmodel.cpp \
    $$PWD/asemanqttools.cpp \
    $$PWD/asemancalendarmodel.cpp \
    $$PWD/asemanlistrecord.cpp \
    $$PWD/asemanquickviewwrapper.cpp \
    $$PWD/asemanfonthandler.cpp \
    $$PWD/asemansimpleqtcryptor.cpp \
    $$PWD/asemanmixedlistmodel.cpp \
    $$PWD/asemanhostchecker.cpp \
    $$PWD/asemannetworkmanager.cpp \
    $$PWD/asemannetworkmanageritem.cpp \
    $$PWD/asemannetworksleepmanager.cpp \
    $$PWD/asemanstoremanager.cpp \
    $$PWD/private/asemanabstractstoremanagercore.cpp \
    $$PWD/private/asemannullstoremanagercore.cpp \
    $$PWD/private/asemanabstractcameracapturecore.cpp \
    $$PWD/private/asemannullcameracapturecore.cpp \
    $$PWD/asemancameracapture.cpp \
    $$PWD/asemanqmlsmartcomponent.cpp \
    $$PWD/asemanstoremanagermodel.cpp \
    $$PWD/asemanwindowdetails.cpp \
    $$PWD/asemansettings.cpp \
    $$PWD/asemantexttools.cpp \
    $$PWD/asemanapplicationitem.cpp \
    $$PWD/asemanencrypter.cpp \
    $$PWD/asemancontributorsmodel.cpp \
    $$PWD/qtsingleapplication/qtlockedfile.cpp \
    $$PWD/qtsingleapplication/qtlocalpeer.cpp \
    $$PWD/asemanqmlengine.cpp

HEADERS += \
    $$PWD/asemandevices.h \
    $$PWD/asemanqtlogger.h \
    $$PWD/asemantools.h \
    $$PWD/asemandesktoptools.h \
    $$PWD/asemanlistobject.h \
    $$PWD/asemanhashobject.h \
    $$PWD/asemanquickview.h \
    $$PWD/asemanapplication.h \
    $$PWD/aseman_macros.h \
    $$PWD/asemancalendarconvertercore.h \
    $$PWD/asemancalendarconverter.h \
    $$PWD/asemanbackhandler.h \
    $$PWD/asemansysteminfo.h \
    $$PWD/asemanabstractcolorfulllistmodel.h \
    $$PWD/asemanimagecoloranalizor.h \
    $$PWD/asemancountriesmodel.h \
    $$PWD/asemanmimedata.h \
    $$PWD/asemanmimeapps.h \
    $$PWD/asemandragobject.h \
    $$PWD/asemandownloader.h \
    $$PWD/asemannotification.h \
    $$PWD/asemanautostartmanager.h \
    $$PWD/asemanquickitemimagegrabber.h \
    $$PWD/asemanquickobject.h \
    $$PWD/asemanfilesystemmodel.h \
    $$PWD/asemandebugobjectcounter.h \
    $$PWD/asemanfiledownloaderqueue.h \
    $$PWD/asemanfiledownloaderqueueitem.h \
    $$PWD/asemanwebpagegrabber.h \
    $$PWD/asemantitlebarcolorgrabber.h \
    $$PWD/asemantaskbarbutton.h \
    $$PWD/private/asemanabstracttaskbarbuttonengine.h \
    $$PWD/asemanmapdownloader.h \
    $$PWD/asemandragarea.h \
    $$PWD/asemanabstractlistmodel.h \
    $$PWD/asemanqttools.h \
    $$PWD/asemancalendarmodel.h \
    $$PWD/asemanlistrecord.h \
    $$PWD/asemanquickviewwrapper.h \
    $$PWD/asemanfonthandler.h \
    $$PWD/asemansimpleqtcryptor.h \
    $$PWD/private/serpent_sbox.h \
    $$PWD/asemanmixedlistmodel.h \
    $$PWD/asemanhostchecker.h \
    $$PWD/asemannetworkmanager.h \
    $$PWD/asemannetworkmanageritem.h \
    $$PWD/asemannetworksleepmanager.h \
    $$PWD/asemanstoremanager.h \
    $$PWD/private/asemannullstoremanagercore.h \
    $$PWD/private/asemanabstractstoremanagercore.h \
    $$PWD/private/asemanabstractcameracapturecore.h \
    $$PWD/private/asemannullcameracapturecore.h \
    $$PWD/asemancameracapture.h \
    $$PWD/asemanqmlsmartcomponent.h \
    $$PWD/asemanstoremanagermodel.h \
    $$PWD/asemanglobals.h \
    $$PWD/asemanwindowdetails.h \
    $$PWD/asemansettings.h \
    $$PWD/asemantexttools.h \
    $$PWD/asemanapplicationitem.h \
    $$PWD/asemanencrypter.h \
    $$PWD/asemancontributorsmodel.h \
    $$PWD/qtsingleapplication/qtlockedfile.h \
    $$PWD/qtsingleapplication/qtlocalpeer.h \
    $$PWD/asemanqmlengine.h

OTHER_FILES += \
    $$PWD/android-build/src/land/aseman/android/AsemanActivity.java \
    $$PWD/android-build/src/land/aseman/android/AsemanApplication.java \
    $$PWD/android-build/src/land/aseman/android/AsemanJavaLayer.java \
    $$PWD/android-build/src/land/aseman/android/AsemanService.java \
    $$PWD/android-build/src/land/aseman/android/AsemanBootBroadcast.java \
    $$PWD/android-build/src/land/aseman/android/AsemanServiceDelegate.java \
    $$PWD/android-build/src/com/android/vending/billing/IInAppBillingService.aidl \
    $$PWD/android-build/src/land/aseman/android/store/util/Base64.java \
    $$PWD/android-build/src/land/aseman/android/store/util/Base64DecoderException.java \
    $$PWD/android-build/src/land/aseman/android/store/util/IabException.java \
    $$PWD/android-build/src/land/aseman/android/store/util/IabHelper.java \
    $$PWD/android-build/src/land/aseman/android/store/util/IabResult.java \
    $$PWD/android-build/src/land/aseman/android/store/util/Inventory.java \
    $$PWD/android-build/src/land/aseman/android/store/util/Purchase.java \
    $$PWD/android-build/src/land/aseman/android/store/util/Security.java \
    $$PWD/android-build/src/land/aseman/android/store/util/SkuDetails.java \
    $$PWD/android-build/src/land/aseman/android/store/StoreManager.java \
    $$PWD/android-build/src/land/aseman/android/extra/AsemanLocationListener.java


!contains(DEFINES,ASEMAN_QML_PLUGIN) {
    RESOURCES += \
        $$PWD/asemanresource.qrc
}

DISTFILES += \
    $$PWD/android-build/src/land/aseman/android/extra/AsemanCameraCapture.java
