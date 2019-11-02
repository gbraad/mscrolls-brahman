/*
 *
 *
 *   ______              _
 *   | ___ \            | |
 *   | |_/ / _ __  __ _ | |__   _ __ ___    __ _  _ __
 *   | ___ \| '__|/ _` || '_ \ | '_ ` _ \  / _` || '_ \
 *   | |_/ /| |  | (_| || | | || | | | | || (_| || | | |
 *   \____/ |_|   \__,_||_| |_||_| |_| |_| \__,_||_| |_|
 *
 *
 *  "The creative principle which lies realized in the whole world"
 *
 *  Copyright (c) Strand Games 2018.
 *
 *  This program is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU Lesser General Public License (LGPL) as published
 *  by the Free Software Foundation, either version 3 of the License, or (at
 *  your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 *  for more details.
 * 
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *  contact@strandgames.com
 *
 */

#pragma once


#include <QObject>
#include <QString>
#include <QDebug>

#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QAudioOutput>

#include <assert.h>
#include <string>

#include "qdefs.h"
#include "playsource.h"
#include "logged.h"

#define QREGISTER_SOUNDPLAYER  QREGISTER(QSoundPlayer)

class QSoundPlayer: public QObject
{
public:

    typedef std::string string;

    Q_OBJECT

    Q_PROPERTY(int state READ state NOTIFY stateChanged);    
    Q_PROPERTY(int loops READ loops WRITE setLoops);
    Q_PROPERTY(int fade READ fade WRITE setFade);

public:

    static const int DataSampleRateHz = 44100;
    static const int defaultFadeIn = 250;

    struct SourcePair
    {
        QSoundPlayer*   _host;
        PlayerSource*   _s[2];
        int             _current;
        double          _vol = 1.0;

        SourcePair()
        {
            _s[0] = 0;
            _s[1] = 0;
            _current = -1;
        }

        ~SourcePair()
        {
            stop(true);
        }

        void _drop(int c)
        {
            delete _s[c];
            _s[c] = 0;
        }

        void stop(bool now = false)
        {
            if (now)
            {
                _drop(0);
                _drop(1);
            }
            else if (_current >= 0)
            {
                assert(_s[_current]);
                if (_s[_current]->playing())
                {
                    //LOG3("sourcepair, stopping ", _current);
                    _s[_current]->fade(-_host->_fade);
                }
            }
        }
        
        void setVolume(double v)
        {
            _vol = v;
            if (_s[0]) _s[0]->setVolume(_vol);
            if (_s[1]) _s[1]->setVolume(_vol);
        }

        void _bind(PlayerSource* s)
        {
            // clean up

            bool p0 = _s[0] && _s[0]->playing();
            bool p1 = _s[1] && _s[1]->playing();

            if (p0 && p1)
            {
                // both playing. one is fading out.
                assert(_current >= 0);
            }
            else if (p0)
            {
                // s0 playing, s1 not playing
                _current = 0;
            }
            else if (p1)
            {
                // s1 playing, s0 not playing
                _current = 1;
            }
            else
            {
                // none playing
                _current = 1;
                _drop(1);
            }

            if (_s[_current])
                _s[_current]->fade(-_host->_fade);

            // kill other
            _current = 1 - _current;
            _drop(_current);
            
            // becomes current
            s->setVolume(_vol);
            _s[_current] = s;
            
            //LOG3("bind soundplayer to ", _current);
        }

        bool play(const string& name, QAudioOutput* aout)
        {
            if (equalsIgnoreCase(suffixOf(name), ".ogg"))
            {
                
                extern PlayerSource* createPlaySourceOgg(QObject* parent,
                                                         QAudioOutput* out,
                                                         const QAudioFormat&,
                                                         const string& filename);
                
                PlayerSource* s = createPlaySourceOgg(_host,
                                                      aout,
                                                      _host->_format, name);


                if (!s) return false;
                
                _bind(s);

                s->loops(_host->_loops);
                s->fade(_host->_fade);
                QIODevice* out = aout->start();
                s->start(out);
                return true;
            }
            else
            {
                LOG2("SoundPlayer, unknown format ", name);
                delete aout;
            }
            return false;
        }

    };


    int                  _loops = 1;
    int                  _fade = defaultFadeIn;
    int                  _state = QAudio::StoppedState;

    QSoundPlayer() { _init(); }

    int state() const { return _state; }
    int loops() const { return _loops; }

    void setLoops(int l)
    {
        _loops = l;
    }

    int fade() const { return _fade; }
    void setFade(int f) 
    {
        _fade = f;
    }

    Q_INVOKABLE bool play(int chan, const QString& qname)
    {
        bool r = false;
        string name = STRQ(qname);
        if (chan < _maxPairs)
        {
            QAudioOutput* aout = new QAudioOutput(_device, _format, this);
            
            r = _pairs[chan].play(name, aout); // aout consumed
            if (r)
            {
                LOG3("Playing ", name << " channel " << chan);
            }
        }
        
        if (!r)
        {
            LOG1("Play ", name << " failed");
        }
        return r;
    }

    Q_INVOKABLE bool stop(int chan)
    {
        bool r = false;
        if (chan < _maxPairs)
        {
            _pairs[chan].stop();
            r = true;
        }
        return r;
    }

    Q_INVOKABLE void setVolume(int chan, qreal v)
    {
        if (chan < _maxPairs)
        {
            _pairs[chan].setVolume((double)v);
        }
    }

    /*
    Q_INVOKABLE bool start()
    {
        LOG4("SoundPLayer, ", "start");
        
        bool r = false;
        
        int s = state();

        if (s == QAudio::ActiveState || _out) return true; // already playing
        
        if (s == QAudio::SuspendedState)
        {
            _audioOutput->resume();
            r = true;
        }
        else
        {
        // push mode
            _audioOutput->setBufferSize(100*1024);
            _out = _audioOutput->start();
                
            r = _out != 0;
            
        }
        return r;
    }

    Q_INVOKABLE void pause()
    {
        if (state() == QAudio::ActiveState)
        {
            _audioOutput->suspend();
        }
    }

    Q_INVOKABLE void stop()
    {
        LOG4("SoundPLayer, ", "stop");
        if (state() == QAudio::ActiveState)
        {
            _audioOutput->stop();
        }
    }
    */

public slots:

    void updateState(QAudio::State s)
    {
        if (_state != s)
        {
            _state = s;
            if (_state == QAudio::StoppedState)
            {
                //if (_playerSource) _playerSource->stop();
            }
            emit stateChanged();
        }
    }

signals:

    void stateChanged();

private:

    QAudioDeviceInfo    _device;
    QAudioFormat        _format;

    static const int    _maxPairs = 2;
    SourcePair          _pairs[_maxPairs];

    void _init()
    {
        _initAudio();
        for (int i = 0; i < _maxPairs; ++i) _pairs[i]._host = this;
    }

    void _initAudio()
    {
        _format.setSampleRate(DataSampleRateHz);
        _format.setChannelCount(2);
        _format.setSampleSize(16);
        _format.setCodec("audio/pcm");
        _format.setByteOrder(QAudioFormat::LittleEndian);
        _format.setSampleType(QAudioFormat::SignedInt);

        const QAudioDeviceInfo &defaultDeviceInfo = QAudioDeviceInfo::defaultOutputDevice();
        _device = defaultDeviceInfo;

        QAudioDeviceInfo info(_device);
        
        if (!info.isFormatSupported(_format))
        {
            LOG2("SoundPLayer, Default format not supported - trying to use nearest", "");
            _format = info.nearestFormat(_format);
        }

        //_audioOutput = new QAudioOutput(_device, _format, this);
        //connect(_audioOutput,SIGNAL(stateChanged(QAudio::State)),this, SLOT(updateState(QAudio::State)));

    }

    
};
