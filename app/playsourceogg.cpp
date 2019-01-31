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


#include <string>

#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <assert.h>

#include "playsource.h"
#include "vorbis.h"

class PlayerSourceOgg : public PlayerSource
{
public:

    typedef std::thread thread;
    typedef std::mutex mutex;
    typedef std::condition_variable condition_variable;

    int         _channelCount;
    int         _sampleSize; // bytes per sample
    int         _sampleRate;

    stb_vorbis* _vorbis = 0; // current decoding

    // play for loops times, -1 to repeat
    int         _loops = 1; 

    float       _gain = 1.0f;
    float       _dgain = 0;

    QIODevice*  _pushOut = 0;
    thread      _task;
    bool        _running = false;
    bool        _taskValid = false;

    // between start and running
    bool        _starting;
    bool        _shutdown = false;

    // during a push
    uint        _bytesWritten;

    PlayerSourceOgg(QObject* parent, QAudioOutput* out) 
        : PlayerSource(parent, out) {}

    ~PlayerSourceOgg()
    {
        stop();
    }

    bool setFormat(const QAudioFormat &format)
    {
        bool r = format.isValid();

        if (r)
        {
            _channelCount = format.channelCount();
            _sampleSize = format.sampleSize()/8; // bytes per sample
            _sampleRate = format.sampleRate();
            
            //LOG3("PlayerSource format; chan:", _channelCount << " rate:" << _sampleRate);
            
            r = _sampleSize == 2 && format.sampleType() == QAudioFormat::SignedInt && format.byteOrder() == QAudioFormat::LittleEndian;
        }

        if (!r)
        {
            LOG1("PlayerSource ", "out format bad");
        }

        return r;
    }

    void fade(int ms) override
    {
        if (ms)
        {
            // samples over which to fade < 0 => fade out
            int n = (_sampleRate * ms)/1000;
        
            // gain per sample
            _dgain = 1.0f/n;

            // leave gain alone if already changing
            if (_dgain > 0)
            {
                if (_gain == 1.0f) _gain = 0;
            }
            else 
            {
                if (_gain == 0.0f) _gain = 1.0f;
            }

        }
    }

    void loops(int l) override
    {
        _loops = l;
    }

    bool playing() const override
    {
        return _pushOut != 0;
    }
    
    bool start(QIODevice* push) override
    {
        if (!push) return false;
        
        if (_pushOut) 
        {
            LOG3("playsourceogg, start ", "already started");
            return false;
        }

        LOG4("playsourceogg, ", "start");

        _pushOut = push;
        _decodeBegin();
        startPushing();
        return true;
    }

    void stop() override
    {
        if (_pushOut)
        {
            LOG4("playsourceogg, ", "stop");
            stopPushing();
            _purge();

            // reset any gains
            _gain = 1.0f;
            _dgain = 0;
            _pushOut = 0;
        }
    }

    void startPushing()
    {
        // interval between starting thread and it running
        if (_running) stop();
        
        _starting = true;
        _task = std::thread(&PlayerSourceOgg::runTask, this);
        _taskValid = true;
    }

    void stopPushing()
    {
        if (_running) _shutdown = true;
        if (_taskValid)
        {
            _task.join();
            _taskValid = false;
        }

        LOG4("playsourceogg, ", "stopped");
        _shutdown = false;
    }

    int bytesToMS(int b) const
    {
        int nsamples = b / (_channelCount * _sampleSize);

        // milliseconds of data written
        int ms = nsamples*1000/_sampleRate;

        return ms;
    }

    int msToBytes(int ms) const
    {
        int nsamples = (ms * _sampleRate)/1000;
        return nsamples * (_channelCount * _sampleSize);
    }

    void runTask()
    {
        _running = true;
        _starting = false; // we've started now we're running

        for (;;)
        {
            _running = !_shutdown && pushData();
            if (!_running) break;

            int t = 10;

            if (_bytesWritten)
            {
                // milliseconds of data written
                int ms = bytesToMS(_bytesWritten);
                
                //LOG3("PlayerSource ", _sourceFile << ", wrote ms:" << ms);

                int t = ms/2;
                if (t < 10) t = 10;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(t));
        }

        assert(_audioOutput);
        _audioOutput->stop();
        
        LOG4("PlayerSource pushing over, ", (_shutdown ? "shutdown" : ""));
    }

    bool pushData()
    {
        _bytesWritten = 0;

        bool res = _pushOut && _audioOutput
            && (_audioOutput->state() == QAudio::ActiveState 
                || _audioOutput->state() == QAudio::IdleState);

        if (res)
        {
            int bz = _audioOutput->bytesFree();
            int pz = _audioOutput->periodSize();

            if (!bz || !pz) return res; // bail

            int ms = bytesToMS(bz);
            if (ms > 200)
            {
                // dont fill more than 200ms
                bz = msToBytes(200);
            }

            //LOG3("Playersource push size:", bz << " period size:" << pz);
            
            int len = bz/pz;
            if (!len) len = 1; // at least one
            len *= pz;

            while (res && len > 0)
            {
                if (!_gain && !_dgain)
                {
                    // can stop now
                    //LOG3("sourceogg, ", "fade to zero");
                    res = false;
                    break;
                }
                
                // if we have less than one period, decode some
                while ((int)_bufSize < pz)
                {
                    int r = _decodeNext();
                
                    if (!r)
                    {
                        // end of data. do we want to loop?
                        if (_loops > 0) --_loops;
                        if (_loops)
                        {
                            if (_decodeBegin()) r = _decodeNext();
                        }
                    }
                    
                    if (!r) 
                    {
                        res = false; // will stop pusher
                        
                        // pad with zeros to end
                        if (_bufSize && (int)_bufSize < pz)
                        {
                            memset(_buffer + _bufSize, 0, pz - _bufSize);
                            _bufSize = pz;
                        }
                    }
                }

                // write periods until buf reduced
                while ((int)_bufSize >= pz)
                {
                    int a = _pushOut->write((const char*)_buffer, pz);

                    if (a < 0)
                    {
                        LOG2("PlayerSource push write error ", a);
                        res = false;
                        break;
                    }
                    else
                    {
                        _bytesWritten += a;

                        if (a)
                        {
                            int r = _bufSize - a;
                            memmove(_buffer, _buffer + a, r);
                            _bufSize = r;
                            len -= a;

                            if (len <= 0) break;
                        }

                        if (a != pz)
                        {
                            //LOG3("Playersource push full ", a << " from " << pz);
                            
                            // do not fail, but fall out and wait
                            return true;
                        }
                    }
                }
            }
        }
        return res;
    }

    qint64 bytesAvailable() const override
    {
        return QIODevice::bytesAvailable() + _bufSize;
    }

private:

    void _fill()
    {
        int r = _dbufSize - _dbufUsed;
        memmove(_dbuf, _dbuf + _dbufUsed, r);

        _dbufSize = r;
        _dbufUsed = 0;
        
        size_t nread = _source.read((char*)_dbuf + r, _dbufSpace - r);
        if (nread != (size_t)-1) _dbufSize += nread;
    }

    bool _decodeBegin()
    {
        _purge();
        
        if (_source.isOpen())
        {
            if (!_source.seek(0)) return false;
                    
            _fill();

            int error;            
            if (_dbufSize)
            {
                _vorbis = stb_vorbis_open_pushdata(_dbuf, _dbufSize,
                                                   &_dbufUsed, &error, 0);
            }

            if (!_vorbis)
            {
                LOG1("PlaySource, decode begin fail ", error);
                _purge();
            }
        }
        return _vorbis != 0;
    }

    int _decodeNext()
    {
        // return buffer size decoded
        int total = 0;
        if (_vorbis)
        {
            for (;;)
            {
                int r = _dbufSize - _dbufUsed;
            
                int nchan;
                float** output;
                int nsamples;
                int used;
                used = stb_vorbis_decode_frame_pushdata(_vorbis,
                                                        _dbuf + _dbufUsed, r,
                                                        &nchan,
                                                        &output,
                                                        &nsamples);

                if (!used)
                {
                    int e = stb_vorbis_get_error(_vorbis);

                    if (e > 1)
                    {
                        LOG1("PlayerSource vorbis error ", e);
                        break;
                    }

                    // need more data
                    _fill();

                    if (!_dbufSize)
                    {
                        // end of data
                        LOG4("PlayerSource end of data", "");
                        break;
                    }
                }
                else
                {
                    _dbufUsed += used;

                    if (!nsamples)
                    {
                        // resync
                    }
                    else
                    {
                        // output!

                        uchar* p = _buffer + _bufSize;

                        // ensure output buffer is big enough
                        uint size = nsamples*nchan*sizeof(short);

                        uint sz = _bufSize + size;

                        if (sz > _bufSpace)
                        {
                            // truncate to what we have if buffer not enough
                            LOG3("Bufsize too small, need ", sz << " nsamples:" << nsamples << " nchan:" << nchan);
                            size = _bufSpace - _bufSize;
                            nsamples = size/(nchan*sizeof(short));
                        }

                        _bufSize += size;
                        total += size;
                        
                        // convert to signed int
                        for (int i = 0; i < nsamples; ++i)
                        {
                            for (int j = 0; j < nchan; ++j)
                            {
                                float v = output[j][i];

                                if (_dgain)
                                {
                                    // fading in or out
                                    _gain += _dgain;

                                    if (_gain > 1.0f)
                                    {
                                        _gain = 1.0f;
                                        _dgain = 0; // over
                                    }
                                    else if (_gain < 0.0f)
                                    {
                                        _gain = 0.0f;
                                        _dgain = 0; // over
                                    }

                                    v *= _gain;
                                }
                                else
                                {
                                    // if not fading then gain is 1 or 0
                                    if (!_gain) v = 0;
                                }

                                // clamp
                                if (v > 1.0f) v = 1.0f;
                                else if (v < -1.0f) v = -1.0f;

                                int vi = v*32767;

                                // little endian
                                *p++ = vi;
                                *p++ = vi >> 8;
                            }
                        }
                        break;
                    }
                }
            }
        }
        return total;
    }

    void _purge()
    {
        _bufSize = 0;

        _dbufSize = 0;
        _dbufUsed = 0;

        if (_vorbis)
        {
            stb_vorbis_close(_vorbis);
            _vorbis = 0;
        }
    }

    // decode buffer
    static const uint   _dbufSpace = 64*1024;
    uchar               _dbuf[_dbufSpace];
    uint                _dbufSize = 0;
    int                 _dbufUsed;    


    // samples buffer
    static const uint   _bufSpace = 128*1024;
    uchar               _buffer[_bufSpace];
    uint                _bufSize = 0;

};


PlayerSource* createPlaySourceOgg(QObject* parent,
                                  QAudioOutput* out,
                                  const QAudioFormat& format,
                                  const std::string& filename)
{
    PlayerSourceOgg* p = new PlayerSourceOgg(parent, out);
    bool r = p->setFormat(format) &&  p->setSource(filename);
    if (!r)
    {
        delete p;
        p = 0;
    }
    return p;
}
