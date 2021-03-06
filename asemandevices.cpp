/*
    Copyright (C) 2014 Aseman
    http://aseman.co

    This project is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This project is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define LINUX_DEFAULT_DPI 96
#define WINDOWS_DEFAULT_DPI 96
#define UTOUCH_DEFAULT_DPI 76

#include "asemandevices.h"
#include "asemanapplication.h"
#include "asemanmimedata.h"
#include "asemandesktoptools.h"

#ifdef Q_OS_ANDROID
#include "asemanjavalayer.h"
#endif

#include <QTimerEvent>
#include <QGuiApplication>
#include <QMimeType>
#include <QMimeDatabase>
#include <QUrl>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QClipboard>
#include <QtCore/qmath.h>
#include <QScreen>
#include <QDateTime>
#include <QDebug>
#include <QMimeData>
#include <QProcess>
#include <QGuiApplication>

#ifdef ASEMAN_MULTIMEDIA
#if (QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
#include <QBuffer>
#include <QCameraInfo>
#endif
#endif

#ifdef Q_OS_WIN
#include <QSysInfo>
#endif

class AsemanDevicesPrivate
{
public:
    int hide_keyboard_timer;
    bool keyboard_stt;

    QMimeDatabase mime_db;

#ifdef Q_OS_ANDROID
    AsemanJavaLayer *java_layer;
#endif

    static QHash<AsemanDevices::Flags, bool> flags;
};

QHash<AsemanDevices::Flags, bool> AsemanDevicesPrivate::flags = QHash<AsemanDevices::Flags, bool>();

AsemanDevices::AsemanDevices(QObject *parent) :
    QObject(parent)
{
    p = new AsemanDevicesPrivate;
    p->hide_keyboard_timer = 0;
    p->keyboard_stt = false;

#ifdef Q_OS_ANDROID
    p->java_layer = AsemanJavaLayer::instance();

    connect( p->java_layer, SIGNAL(incomingShare(QString,QString)), SLOT(incoming_share(QString,QString)), Qt::QueuedConnection );
    connect( p->java_layer, SIGNAL(incomingImage(QString))        , SLOT(incoming_image(QString))        , Qt::QueuedConnection );
    connect( p->java_layer, SIGNAL(selectImageResult(QString))    , SLOT(select_image_result(QString))   , Qt::QueuedConnection );
    connect( p->java_layer, SIGNAL(activityPaused())              , SLOT(activity_paused())              , Qt::QueuedConnection );
    connect( p->java_layer, SIGNAL(activityResumed())             , SLOT(activity_resumed())             , Qt::QueuedConnection );
#endif

    connect( QGuiApplication::inputMethod(), SIGNAL(visibleChanged()), SLOT(keyboard_changed()) );
    connect( static_cast<QGuiApplication*>(QCoreApplication::instance())->clipboard(), SIGNAL(dataChanged()), SIGNAL(clipboardChanged()) );

    QScreen *scr = screen();
    if( scr )
        connect( scr, SIGNAL(geometryChanged(QRect)), SIGNAL(geometryChanged()) );
}

bool AsemanDevices::isMobile()
{
    return isTouchDevice() && !isTablet();
}

bool AsemanDevices::isTablet()
{
#ifdef Q_OS_ANDROID
    return isTouchDevice() && AsemanJavaLayer::instance()->isTablet();
#else
    return isTouchDevice() && lcdPhysicalSize() >= 6;
#endif
}

bool AsemanDevices::isLargeTablet()
{
#ifdef Q_OS_ANDROID
    return isTablet() && AsemanJavaLayer::instance()->getSizeName() == 3;
#else
    return isTouchDevice() && lcdPhysicalSize() >= 9;
#endif
}

bool AsemanDevices::isTouchDevice()
{
    return isAndroid() || isIOS() || isWindowsPhone() || isUbuntuTouch();
}

bool AsemanDevices::isDesktop()
{
    return !isTouchDevice();
}

bool AsemanDevices::isMacX()
{
#ifdef Q_OS_MAC
    return true;
#else
    return false;
#endif
}

bool AsemanDevices::isWindows()
{
#ifdef Q_OS_WIN
    return true;
#else
    return false;
#endif
}

bool AsemanDevices::isLinux()
{
#if defined(Q_OS_LINUX) || defined(Q_OS_OPENBSD)
    return true;
#else
    return false;
#endif
}

bool AsemanDevices::isAndroid()
{
#ifdef Q_OS_ANDROID
    return true;
#else
    return false;
#endif
}

bool AsemanDevices::isIOS()
{
#ifdef Q_OS_IOS
    return true;
#else
    return false;
#endif
}

bool AsemanDevices::isUbuntuTouch()
{
#ifdef Q_OS_UBUNTUTOUCH
    return true;
#else
    return false;
#endif
}

bool AsemanDevices::isWindowsPhone()
{
#ifdef Q_OS_WINPHONE
    return true;
#else
    return false;
#endif
}

bool AsemanDevices::isWindows8()
{
#ifdef Q_OS_WIN
    return QSysInfo::windowsVersion() == QSysInfo::WV_WINDOWS8 ||
           QSysInfo::windowsVersion() == QSysInfo::WV_WINDOWS8_1;
#else
    return false;
#endif
}

QScreen *AsemanDevices::screen()
{
    const QList<QScreen*> & screens = QGuiApplication::screens();
    if( screens.isEmpty() )
        return 0;

    return screens.first();
}

QObject *AsemanDevices::screenObj() const
{
    return screen();
}

qreal AsemanDevices::lcdPhysicalSize()
{
    qreal w = lcdPhysicalWidth();
    qreal h = lcdPhysicalHeight();

    return qSqrt( h*h + w*w );
}

qreal AsemanDevices::lcdPhysicalWidth()
{
    if( QGuiApplication::screens().isEmpty() )
        return 0;

    return (qreal)screenSize().width()/lcdDpiX();
}

qreal AsemanDevices::lcdPhysicalHeight()
{
    if( QGuiApplication::screens().isEmpty() )
        return 0;

    return (qreal)screenSize().height()/lcdDpiY();
}

qreal AsemanDevices::lcdDpiX()
{
#ifdef Q_OS_ANDROID
    return AsemanJavaLayer::instance()->densityDpi();
#else
    if( QGuiApplication::screens().isEmpty() )
        return 0;

    QScreen *scr = QGuiApplication::screens().first();
    return scr->physicalDotsPerInchX();
#endif
}

qreal AsemanDevices::lcdDpiY()
{
#ifdef Q_OS_ANDROID
    return AsemanJavaLayer::instance()->densityDpi();
#else
    if( QGuiApplication::screens().isEmpty() )
        return 0;

    QScreen *scr = QGuiApplication::screens().first();
    return scr->physicalDotsPerInchY();
#endif
}

QSize AsemanDevices::screenSize()
{
#ifdef Q_OS_ANDROID
    return QSize(AsemanJavaLayer::instance()->screenSizeWidth(),
                 AsemanJavaLayer::instance()->screenSizeHeight());
#else
    if( QGuiApplication::screens().isEmpty() )
        return QSize();

    QScreen *scr = QGuiApplication::screens().first();
    return scr->size();
#endif
}

qreal AsemanDevices::keyboardHeight() const
{
#ifdef Q_OS_UBUNTUTOUCH
    return screenSize().height()*0.5;
#else
    const QSize & scr_size = screenSize();
    bool portrait = scr_size.width()<scr_size.height();
    if( portrait )
    {
        if( isMobile() )
            return screenSize().height()*0.6;
        else
            return screenSize().height()*0.4;
    }
    else
    {
        if( isMobile() )
            return screenSize().height()*0.7;
        else
            return screenSize().height()*0.5;
    }
#endif
}

QString AsemanDevices::deviceName()
{
    if(isDesktop())
#ifdef Q_OS_WIN
        return QSysInfo::prettyProductName() + " " + QSysInfo::currentCpuArchitecture();
#else
        return "desktop";
#endif
#ifdef Q_OS_ANDROID
    else
        return AsemanJavaLayer::instance()->deviceName();
#else
    else
        return "mobile";
#endif
}

QString AsemanDevices::deviceId()
{
#if defined(Q_OS_ANDROID)
    return AsemanJavaLayer::instance()->deviceId();
#elif defined(Q_OS_LINUX)
    static QString cg_hostId;
    if(!cg_hostId.isEmpty())
        return cg_hostId;

    QProcess prc;
    prc.start("hostid");
    prc.waitForStarted();
    prc.waitForReadyRead();
    prc.waitForFinished();

    cg_hostId = prc.readAll();
    cg_hostId = cg_hostId.trimmed();
    return cg_hostId;
#else
    return QString();
#endif
}

bool AsemanDevices::transparentStatusBar()
{
#ifdef Q_OS_ANDROID
    return AsemanJavaLayer::instance()->transparentStatusBar();
#else
    return false;
#endif
}

bool AsemanDevices::transparentNavigationBar()
{
#ifdef Q_OS_ANDROID
    return AsemanJavaLayer::instance()->transparentNavigationBar();
#else
    return false;
#endif
}

qreal AsemanDevices::standardTitleBarHeight() const
{
    static qreal res = 0;
    if(res)
        return res;

    if(isDesktop() || lcdPhysicalSize()<5)
        res = 50*density()*1.2;
    else
        res = 50*density();

    return res;
}

qreal AsemanDevices::statusBarHeight()
{
    return transparentStatusBar()? 24*density() : 0;
}

qreal AsemanDevices::navigationBarHeight()
{
    return transparentNavigationBar()? 44*density() : 0;
}

void AsemanDevices::setFlag(AsemanDevices::Flags flag, bool state)
{
    AsemanDevicesPrivate::flags[flag] = state;
}

bool AsemanDevices::flag(AsemanDevices::Flags flag)
{
    return AsemanDevicesPrivate::flags.value(flag);
}

int AsemanDevices::densityDpi()
{
#ifdef Q_OS_ANDROID
    return AsemanJavaLayer::instance()->densityDpi();
#else
    return lcdDpiX();
#endif
}

qreal AsemanDevices::density()
{
    const bool disabled = AsemanDevices::flag(DisableDensities);
    if(disabled)
        return 1;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    if(QGuiApplication::testAttribute(Qt::AA_EnableHighDpiScaling))
        return screen()->devicePixelRatio();
#endif

#ifdef Q_OS_ANDROID
    qreal ratio = isTablet()? 1.28 : 1;
//    if( isLargeTablet() )
//        ratio = 1.6;

    return AsemanJavaLayer::instance()->density()*ratio;
#else
#ifdef Q_OS_IOS
    qreal ratio = isTablet()? 1.28 : 1;
    return ratio*densityDpi()/180.0;
#else
#if defined(Q_OS_LINUX) || defined(Q_OS_OPENBSD)
#ifdef Q_OS_UBUNTUTOUCH
    return screen()->logicalDotsPerInch()/UTOUCH_DEFAULT_DPI;
#else
    return screen()->logicalDotsPerInch()/LINUX_DEFAULT_DPI;
#endif
#else
#ifdef Q_OS_WIN32
    return 0.95*screen()->logicalDotsPerInch()/WINDOWS_DEFAULT_DPI;
#else
    return 1;
#endif
#endif
#endif
#endif
}

qreal AsemanDevices::fontDensity()
{
#ifdef Q_OS_ANDROID
    const bool disabled = AsemanDevices::flag(DisableDensities);
    qreal ratio = isMobile()? (1.28)*1.25 : (1.28)*1.35;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    if(QGuiApplication::testAttribute(Qt::AA_EnableHighDpiScaling))
        return screen()->devicePixelRatio()*ratio/1.28;
    else
#endif
    if(disabled)
        return ratio/1.28;
    else
        return AsemanJavaLayer::instance()->density()*ratio;
#else
#ifdef Q_OS_IOS
    return 1.4;
#else
#if defined(Q_OS_LINUX) || defined(Q_OS_OPENBSD)
#ifdef Q_OS_UBUNTUTOUCH
    qreal ratio = 1.3;
    return ratio*density();
#else
    qreal ratio = 1.3;
    return ratio*density();
#endif
#else
#ifdef Q_OS_WIN32
    qreal ratio = 1.4;
    return ratio*density();
#else
    qreal ratio = 1.3;
    return ratio*density();
#endif
#endif
#endif
#endif
}

bool AsemanDevices::cameraIsAvailable() const
{
#ifdef ASEMAN_MULTIMEDIA
#if (QT_VERSION >= QT_VERSION_CHECK(5, 3, 0))
    return QCameraInfo::availableCameras().count();
#else
    return false;
#endif
#else
    return false;
#endif
}

QString AsemanDevices::localFilesPrePath()
{
#ifdef Q_OS_WIN
    return "file:///";
#else
    return "file://";
#endif
}

QString AsemanDevices::clipboard() const
{
    return QGuiApplication::clipboard()->text();
}

bool AsemanDevices::keyboard() const
{
    return p->keyboard_stt;
}

QList<QUrl> AsemanDevices::clipboardUrl() const
{
    return QGuiApplication::clipboard()->mimeData()->urls();
}

void AsemanDevices::setClipboardUrl(const QList<QUrl> &urls)
{
    QString data = "copy";

    foreach( const QUrl &url, urls )
        data += "\nfile://" + url.toLocalFile();

    QMimeData *mime = new QMimeData();
    mime->setUrls(urls);
    mime->setData( "x-special/gnome-copied-files", data.toUtf8() );

    QGuiApplication::clipboard()->setMimeData(mime);
}

QString AsemanDevices::cameraLocation()
{
    return AsemanApplication::cameraPath();
}

QString AsemanDevices::picturesLocation()
{
    QStringList probs;
    probs = QStandardPaths::standardLocations( QStandardPaths::PicturesLocation );

#ifdef Q_OS_ANDROID
    probs << "/sdcard/Pictures";
#else
    probs << QDir::homePath() + "/Pictures";
#endif

    foreach( const QString & prob, probs )
        if( QFile::exists(prob) )
            return prob;

    return probs.last();
}

QString AsemanDevices::musicsLocation()
{
    QStringList probs;
    probs = QStandardPaths::standardLocations( QStandardPaths::MusicLocation );

#ifdef Q_OS_ANDROID
    probs << "/sdcard/Music";
#else
    probs << QDir::homePath() + "/Music";
#endif

    foreach( const QString & prob, probs )
        if( QFile::exists(prob) )
            return prob;

    return probs.last();
}

QString AsemanDevices::documentsLocation()
{
    QStringList probs;
    probs = QStandardPaths::standardLocations( QStandardPaths::DocumentsLocation );

#ifdef Q_OS_ANDROID
    probs << "/sdcard/documents";
    probs << "/sdcard/Documents";
#else
    probs << QDir::homePath() + "/Documents";
#endif

    foreach( const QString & prob, probs )
        if( QFile::exists(prob) )
            return prob;

    return probs.last();
}

QString AsemanDevices::downloadsLocation()
{
    QStringList probs;
    probs = QStandardPaths::standardLocations( QStandardPaths::DownloadLocation );

#ifdef Q_OS_ANDROID
    probs << "/sdcard/downloads";
    probs << "/sdcard/Downloads";
#else
    probs << QDir::homePath() + "/Downloads";
#endif

    foreach( const QString & prob, probs )
        if( QFile::exists(prob) )
            return prob;

    return probs.last();
}

QString AsemanDevices::resourcePath()
{
#ifdef Q_OS_ANDROID
    return "assets:";
#else
#ifndef Q_OS_MAC
    QString result = QCoreApplication::applicationDirPath() + "/../share/" + QCoreApplication::applicationName().toLower();
    QFileInfo file(result);
    if(file.exists() && file.isDir())
        return file.filePath();
    else
        return QCoreApplication::applicationDirPath() + "/";
#else
    return QCoreApplication::applicationDirPath() + "/../Resources/";
#endif
#endif
}

QString AsemanDevices::resourcePathQml()
{
#ifdef Q_OS_ANDROID
    return resourcePath();
#else
    return localFilesPrePath() + resourcePath();
#endif
}

QString AsemanDevices::libsPath()
{
#ifndef Q_OS_MAC
    QString result = QCoreApplication::applicationDirPath() + "/../lib/" + QCoreApplication::applicationName().toLower();
    QFileInfo file(result);
    if(file.exists() && file.isDir())
        return file.filePath();
    else
        return QCoreApplication::applicationDirPath() + "/";
#else
    return QCoreApplication::applicationDirPath() + "/../Resources/";
#endif
}

void AsemanDevices::hideKeyboard()
{
#ifndef DESKTOP_DEVICE
    if( p->hide_keyboard_timer )
        killTimer(p->hide_keyboard_timer);

    p->hide_keyboard_timer = startTimer(250);
#endif
}

void AsemanDevices::showKeyboard()
{
#ifndef DESKTOP_DEVICE
    if( p->hide_keyboard_timer )
    {
        killTimer(p->hide_keyboard_timer);
        p->hide_keyboard_timer = 0;
    }

    QGuiApplication::inputMethod()->show();
    p->keyboard_stt = true;

    emit keyboardChanged();
#endif
}

void AsemanDevices::share(const QString &subject, const QString &message)
{
#ifdef Q_OS_ANDROID
    p->java_layer->sharePaper( subject, message );
#else
    QString adrs = QString("mailto:%1?subject=%2&body=%3").arg(QString(),subject,message);
    QDesktopServices::openUrl( adrs );
#endif
}

void AsemanDevices::openFile(const QString &address)
{
#ifdef Q_OS_ANDROID
    const QMimeType & t = p->mime_db.mimeTypeForFile(address);
    p->java_layer->openFile( address, t.name() );
#else
    QDesktopServices::openUrl( QUrl(address) );
#endif
}

void AsemanDevices::shareFile(const QString &address)
{
#ifdef Q_OS_ANDROID
    const QMimeType & t = p->mime_db.mimeTypeForFile(address);
    p->java_layer->shareFile( address, t.name() );
#else
    QDesktopServices::openUrl( QUrl(address) );
#endif
}

void AsemanDevices::setClipboard(const QString &text)
{
    QGuiApplication::clipboard()->setText( text );
}

void AsemanDevices::setClipboardData(AsemanMimeData *mime)
{
    QMimeData *data = new QMimeData();
    if(mime)
    {
        data->setText(mime->text());
        data->setHtml(mime->html());
        data->setUrls(mime->urls());

        const QVariantMap &map = mime->dataMap();
        QMapIterator<QString,QVariant> i(map);
        while(i.hasNext())
        {
            i.next();
            QByteArray bytes;
            QDataStream stream(&bytes, QIODevice::WriteOnly);
            stream << i.value();

            data->setData(i.key(), bytes);
        }
    }
    QGuiApplication::clipboard()->setMimeData(data);
}

bool AsemanDevices::startCameraPicture()
{
#ifdef Q_OS_ANDROID
    return p->java_layer->startCamera( cameraLocation() + "/aseman_" + QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()) + ".jpg" );
#else
    return false;
#endif
}

bool AsemanDevices::getOpenPictures()
{
#ifdef Q_OS_ANDROID
    return p->java_layer->getOpenPictures();
#else
    QString path = AsemanDesktopTools::getOpenFileName();
    if(path.isEmpty())
        return false;

    Q_EMIT selectImageResult(path);
    return true;
#endif
}

void AsemanDevices::incoming_share(const QString &title, const QString &msg)
{
    emit incomingShare(title,msg);
}

void AsemanDevices::incoming_image(const QString &path)
{
    emit incomingImage(path);
}

void AsemanDevices::select_image_result(const QString &path)
{
    emit selectImageResult(path);
}

void AsemanDevices::activity_paused()
{
    emit activityPaused();
}

void AsemanDevices::activity_resumed()
{
    emit activityResumed();
}

void AsemanDevices::keyboard_changed()
{
    emit keyboardChanged();
}

void AsemanDevices::timerEvent(QTimerEvent *e)
{
    if( e->timerId() == p->hide_keyboard_timer )
    {
        killTimer(p->hide_keyboard_timer);
        p->hide_keyboard_timer = 0;

        QGuiApplication::inputMethod()->hide();
        p->keyboard_stt = false;

        emit keyboardChanged();
    }
}

AsemanDevices::~AsemanDevices()
{
    delete p;
}
