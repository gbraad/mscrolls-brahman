/*
 * QML Material - An application framework implementing Material Design.
 *
 * Copyright (C) 2016 Michael Spencer <sonrisesoftware@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

 #ifndef DEVICE_H
 #define DEVICE_H

#include <QObject>

#include <cmath>
#include <QQmlEngine>
#include <QGuiApplication>
#include <QScreen>
#include <QTouchDevice>

#include <vector>

struct DNotifier
{
    virtual void changed() {}
};

struct DNotifierSender
{
    typedef std::vector<DNotifier*> DNotifiers;

    DNotifiers       _notifiers;

    void addNotifier(DNotifier* n) { _notifiers.push_back(n); }
    void removeNotfier(DNotifier* n)
    { 
        for (DNotifiers::iterator it = _notifiers.begin();
             it != _notifiers.end(); ++it)
        {
            if (*it == n)
            {
                _notifiers.erase(it);
                break;
            }
        }
    }

    void notify()
    {
        for (size_t i = 0; i < _notifiers.size(); ++i)
            _notifiers[i]->changed();
    }
};

class Device : public QObject, public DNotifierSender
{
    Q_OBJECT

    Q_PROPERTY(FormFactor formFactor READ formFactor NOTIFY geometryChanged)
    Q_PROPERTY(QString name READ name NOTIFY geometryChanged)
    Q_PROPERTY(QString iconName READ iconName NOTIFY geometryChanged)

    Q_PROPERTY(bool isPortrait READ isPortrait NOTIFY geometryChanged)
    Q_PROPERTY(bool isMobile READ isMobile CONSTANT)
    Q_PROPERTY(bool hasTouchScreen READ hasTouchScreen CONSTANT)
    Q_PROPERTY(bool hoverEnabled READ hoverEnabled CONSTANT)
    Q_PROPERTY(bool isLargeMobile READ isLargeMobile CONSTANT)

    Q_PROPERTY(int gridUnit READ gridUnit NOTIFY geometryChanged)

public:
    enum FormFactor {
        Phone,
        Phablet,
        Tablet,
        Computer,
        TV,
        Unknown
    };
    Q_ENUM(FormFactor)

private:
    Device(QObject *parent = nullptr);
public:

    static Device* theDevice()
    {
        static Device* _theDevice;
        if (!_theDevice) _theDevice = new Device();
        return _theDevice;
    }

    double dpScale() const { return _dpScale; }
    void dpScale(double r);

    int truedpi() const { return _truedpi; }
    void truedpi(int v) { _truedpi = v; }

    static QObject *qmlSingleton(QQmlEngine *engine, QJSEngine *scriptEngine);

    FormFactor formFactor() const;
    QString name() const;
    QString iconName() const;

    bool isPortrait() const;
    bool hasTouchScreen() const;
    bool isMobile() const;
    bool isLargeMobile() const;
    bool hoverEnabled() const;

    int gridUnit() const;
    int dpMultiplier() const;

    Q_INVOKABLE bool isAndroid() const
    {
        bool res = false;
#if defined(Q_OS_ANDROID)
        res = true;
#endif
        return res;
    }

    Q_INVOKABLE bool isMac() const
    {
        bool res = false;
#if defined(Q_OS_MAC)
        res = true;
#endif
        return res;
    }

    Q_INVOKABLE bool isIOS() const
    {
        bool res = false;
#if defined(Q_OS_IOS)
        res = true;
#endif
        return res;
    }

signals:
    void geometryChanged();

private slots:
    void screenChanged();
    void orientationChangedHandler(Qt::ScreenOrientation orient);

private:
    float calculateDiagonal() const;

    QScreen*             m_screen;
    double              _dpScale;
    int                 _truedpi;

public:
    
    static double       _fakeDp;
};

#endif // DEVICE_H
