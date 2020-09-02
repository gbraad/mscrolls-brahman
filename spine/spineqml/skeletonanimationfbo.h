/******************************************************************************
 * Spine Runtimes Software License
 * Version 2.1
 *
 * Copyright (c) 2013, Esoteric Software
 * All rights reserved.
 *
 * You are granted a perpetual, non-exclusive, non-sublicensable and
 * non-transferable license to install, execute and perform the Spine Runtimes
 * Software (the "Software") solely for internal use. Without the written
 * permission of Esoteric Software (typically granted by licensing Spine), you
 * may not (a) modify, translate, adapt or otherwise create derivative works,
 * improvements of the Software or develop new applications using the Software
 * or (b) remove, delete, alter or obscure any trademarks or any copyright,
 * trademark, patent or other intellectual property or proprietary rights
 * notices on or in the Software, including any copy thereof. Redistributions
 * in binary or source form must include this license and terms.
 *
 * THIS SOFTWARE IS PROVIDED BY ESOTERIC SOFTWARE "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ESOTERIC SOFTARE BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#ifndef SKELETONANIMATION_H
#define SKELETONANIMATION_H

#define QT_NO_KEYWORDS


#include <QQuickFramebufferObject>
#include <QUrl>
#include <spine/spine.h>
#include <QElapsedTimer>
#include <QList>
#include "spineevent.h"

QT_FORWARD_DECLARE_CLASS(RenderCmdsCache)
QT_FORWARD_DECLARE_CLASS(Texture)

class SkeletonAnimationFbo : public QQuickFramebufferObject
{
    Q_OBJECT
    Q_DISABLE_COPY(SkeletonAnimationFbo)

    Q_PROPERTY(QUrl skeletonDataFile READ skeletonDataFile WRITE setSkeletonDataFile NOTIFY skeletonDataFileChanged)
    Q_PROPERTY(QUrl atlasFile READ atlasFile WRITE setAtlasFile NOTIFY atlasFileChanged)
    Q_PROPERTY(float scale READ scale WRITE setScale NOTIFY scaleChanged)
    Q_PROPERTY(QString skin READ skin WRITE setSkin NOTIFY skinChanged)
    Q_PROPERTY(float timeScale READ timeScale WRITE setTimeScale NOTIFY timeScaleChanged)
    Q_PROPERTY(bool premultipliedAlapha READ premultipliedAlapha WRITE setPremultipliedAlapha NOTIFY premultipliedAlaphaChanged)
    Q_PROPERTY(bool debugSlots READ debugSlots WRITE setDebugSlots NOTIFY debugSlotsChanged)
    Q_PROPERTY(bool debugBones READ debugBones WRITE setDebugBones NOTIFY debugBonesChanged)
    Q_PROPERTY(QSize sourceSize READ sourceSize NOTIFY sourceSizeChanged)

Q_SIGNALS:
    void skeletonStart(int trackIndex);

    // EXTRA
    // termname added
    void skeletonEnd(int trackIndex, const QString& termname);
    void skeletonComplete(int trackIndex, int loopCount, const QString& termname);

    void skeletonEvent(int trackIndex, SpineEvent* event);

    void skeletonDataFileChanged(const QUrl&);
    void atlasFileChanged(const QUrl&);
    void scaleChanged();
    void skinChanged();
    void timeScaleChanged();
    void premultipliedAlaphaChanged();
    void debugSlotsChanged();
    void debugBonesChanged();
    void sourceSizeChanged();

public:
    explicit SkeletonAnimationFbo(QQuickItem *parent = 0);
    ~SkeletonAnimationFbo();

    Q_INVOKABLE void setToSetupPose();
    Q_INVOKABLE void setBonesToSetupPose();
    Q_INVOKABLE void setSlotsToSetupPose();
    Q_INVOKABLE bool setAttachment(const QString& slotName, const QString& attachmentName);
    Q_INVOKABLE void setMix(const QString& fromAnimation, const QString& toAnimation, float duration);
    Q_INVOKABLE void setAnimation (int trackIndex, const QString& name, bool loop);
    Q_INVOKABLE void addAnimation (int trackIndex, const QString& name, bool loop, float delay = 0);
    Q_INVOKABLE void clearTracks ();
    Q_INVOKABLE void clearTrack(int trackIndex = 0);

    QUrl skeletonDataFile()const { return mSkeletonDataFile; }
    void setSkeletonDataFile(const QUrl&);

    QUrl atlasFile()const { return mAtlasFile;}
    void setAtlasFile(const QUrl&);

    float scale() const { return mScale;}
    void setScale(float);

    QString skin() const { return mSkin;}
    void setSkin(const QString&);

    float timeScale() const { return mTimeScale;}
    void setTimeScale(float);

    bool premultipliedAlapha()const { return mPremultipliedAlapha;}
    void setPremultipliedAlapha(bool);

    bool debugSlots() const { return mDebugSlots;}
    void setDebugSlots(bool);

    bool debugBones() const { return mDebugBones;}
    void setDebugBones(bool);

    QSize sourceSize()const { return mSourceSize;}
    void setSourceSize(const QSize&);

    virtual Renderer* createRenderer() const;
    void renderToCache(QQuickFramebufferObject::Renderer*, RenderCmdsCache*);

    virtual void onAnimationStateEvent (spEventType type, spTrackEntry* entry, spEvent* event);
    virtual void onTrackEntryEvent (int trackIndex, spEventType type, spEvent* event, int loopCount);

public Q_SLOTS:
    void updateSkeletonAnimation();

protected:
    void loadSkeletonAndAtlasData();
    QRectF calculateSkeletonRect();
    Texture* getTexture(spRegionAttachment* attachment) const;
    Texture* getTexture(spMeshAttachment* attachment) const;
    //Texture* getTexture(spSkinnedMeshAttachment* attachment) const;

    bool isSkeletonValid();
    void releaseSkeletonRelatedData();
    virtual void componentComplete();

private:
    QUrl mSkeletonDataFile;
    QUrl mAtlasFile;
    float mScale;
    QString mSkin;
    float mTimeScale;
    bool mPremultipliedAlapha;
    bool mDebugSlots;
    bool mDebugBones;
    QSize mSourceSize;

    QElapsedTimer mTimer;

    bool mShouldRelaseCacheTexture;
    bool mSkeletonLoaded;

    spSkeleton* mspSkeleton;
    spAtlas* mspAtlas;
    spBone* mspRootBone;
    spAnimationState* mspAnimationState;
    float* mWorldVertices;

    QList<SpineEvent*> mEventCache;

    //////////// EXTRA //////////////////
public:

    static void _purgeUserData(spTrackEntry* t)
    {
        delete (char*)t->userData;
        t->userData = 0;
    }

    void _purgeAllUserData()
    {
        assert(mspAnimationState);
        int n = mspAnimationState->tracksCount;
        for (int i = 0; i < n; ++i)
        {
            spTrackEntry* t = spAnimationState_getCurrent(mspAnimationState, i);
            assert(t);
            _purgeUserData(t);
        }
    }
    
    void setTrackData(int ti, void* data)
    {
        // add user data
        // ASSUME data will remain valid
        // ASSUME we can just `free` data.
        assert(mspAnimationState);
        spTrackEntry* t = spAnimationState_getCurrent(mspAnimationState, ti);
        assert(t);

        _purgeUserData(t);
        t->userData = data;
    }

    Q_INVOKABLE void setTrackDataTerm(int ti, const QString& termname)
    {
        std::string s = termname.toStdString();

        // make a copy of the string content as the user data
        // to live with the track 
        setTrackData(ti, strcpy(new char[s.size()+1], s.c_str()));
    }

};

#endif // SKELETONANIMATION_H

