/*
 * QML Material - An application framework implementing Material Design.
 *
 * Copyright (C) 2016 Michael Spencer <sonrisesoftware@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "units.h"

#include <QGuiApplication>
#include <QQuickItem>
//#include <QDebug.h>

#if defined(Q_OS_ANDROID)
#include <QtAndroidExtras>
#endif

#define DEFAULT_DPI 72

UnitsAttached::UnitsAttached(QObject *attachee)
        : QObject(attachee), m_screen(nullptr), m_window(nullptr), m_multiplier(1), m_dpi(0)
{
    m_attachee = qobject_cast<QQuickItem *>(attachee);

    Device* dev = Device::theDevice();
    dev->addNotifier(this);

    if (m_attachee) {
        if (m_attachee->window()) // It might not be assigned to a window yet
            windowChanged(m_attachee->window());
    } else {
        QQuickWindow *window = qobject_cast<QQuickWindow *>(attachee);
        if (window)
            windowChanged(window);
    }

    if (!m_screen)
        screenChanged(QGuiApplication::primaryScreen());
}

void UnitsAttached::windowChanged(QQuickWindow *window)
{
    if (m_window)
        disconnect(m_window, &QQuickWindow::screenChanged, this, &UnitsAttached::screenChanged);

    m_window = window;
    screenChanged(window ? window->screen() : nullptr);

    if (window)
        connect(window, &QQuickWindow::screenChanged, this, &UnitsAttached::screenChanged);
}

void UnitsAttached::screenChanged(QScreen *screen)
{
    if (screen != m_screen) {
        QScreen *oldScreen = m_screen;
        m_screen = screen;

        if (oldScreen)
            oldScreen->disconnect(this);

        if (oldScreen == nullptr || screen == nullptr ||
            screen->physicalDotsPerInch() != oldScreen->physicalDotsPerInch() ||
            screen->logicalDotsPerInch() != oldScreen->logicalDotsPerInch() ||
            screen->devicePixelRatio() != oldScreen->devicePixelRatio()) {
            updateDPI();
            //emit dpChanged();
        }
    }
}

qreal UnitsAttached::multiplier() const { return m_multiplier; }

void UnitsAttached::setMultiplier(qreal multiplier)
{
    if (m_multiplier != multiplier)
    {
        m_multiplier = multiplier;
        emit multiplierChanged();
    }
}

#define DESKTOP_DPI  100

void UnitsAttached::updateDP()
{
    Device* dev = Device::theDevice();

    qreal old_dp = m_dp;

    double dratio = 1;

    if (m_screen)
    {
        dratio = m_screen->devicePixelRatio();

        // system is multiplying automatically hidpi
        //if (dratio != 1) printf("NOTE: Device Pixel ratio: %f\n", dratio);
    }

    if (Device::_fakeDp)
    {
        // fake overrides everything. Used in sim modes
        m_dp = Device::_fakeDp;
    }
    else
    {
        // see if we have an accurate DPI
        int adpi = dev->truedpi();

        // if not, use the default
        if (!adpi) adpi = dpi();

        if (dev->isMobile())
        {
            m_dp = adpi/160.0;
        }
        else
        {
            // do not round. better smaller, than too big.
            int dp = adpi/DESKTOP_DPI; 
            m_dp = dp;
        }

        if (m_dp < 1) m_dp = 1;

        if (dratio != 1)
        {
            // system is multiplying automatically hidpi
            m_dp = 1; 
        }

        m_dp *= m_multiplier * dev->dpScale();
    }

    if (m_dp != old_dp) 
    {
        emit dpChanged();
    }
}

void UnitsAttached::updateDPI()
{
    if (m_screen == nullptr)
    {
        m_dpi = DEFAULT_DPI;
        updateDP();
        return;
    }

#if defined(Q_OS_IOS)
    // iOS integration of scaling (retina, non-retina, 4K) does itself.
    m_dpi = m_screen->physicalDotsPerInch();
#elif defined(Q_OS_ANDROID)
    // https://bugreports.qt-project.org/browse/QTBUG-35701
    // recalculate dpi for Android

    QAndroidJniEnvironment env;
    QAndroidJniObject activity = QtAndroid::androidActivity();
    QAndroidJniObject resources =
            activity.callObjectMethod("getResources", "()Landroid/content/res/Resources;");
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();

        m_dpi = DEFAULT_DPI;
        updateDP();
        return;
    }

    QAndroidJniObject displayMetrics =
            resources.callObjectMethod("getDisplayMetrics", "()Landroid/util/DisplayMetrics;");
    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();

        m_dpi = DEFAULT_DPI;
        updateDP();
        return;
    }
    m_dpi = displayMetrics.getField<int>("densityDpi");
#else
    // standard dpi
    m_dpi = m_screen->logicalDotsPerInch() * m_screen->devicePixelRatio();
#endif

    updateDP();
}

void UnitsAttached::changed() 
{
    updateDP();
}
