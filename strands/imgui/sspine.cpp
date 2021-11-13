#include <spine/spine.h>
#include "sokol_gfx.h"
#include "sokol_fetch.h"
#include "imgui.h"

#include <assert.h>
#include "logged.h"
#include "fetcher.h"

#define TAG_SPINE "Spine, "

extern Fetcher fetcher;

#include "imtex.h"
#include "imanim.h"
#include "sspine.h"

using namespace std;
using namespace spine;

struct SSpineExtension: public SpineExtension
{
    void *_alloc(size_t size, const char *file, int line) override
    {
        void *ptr = 0;
        if (size)
        {
            ptr = malloc(size);
        }
	return ptr;
    }

    void *_calloc(size_t size, const char *file, int line) override
    {
        void *ptr = 0;
        if (size)
        {
            ptr = malloc(size);
            memset(ptr, 0, size);
        }
	return ptr;
    }

    void *_realloc(void *ptr, size_t size, const char *file, int line) override
    {
        void *mem = NULL;
        if (size)
        {
            if (ptr == NULL)
		mem = ::malloc(size);
            else
		mem = ::realloc(ptr, size);
        }
	return mem;
    }

    void _free(void *mem, const char *file, int line) override
    {
        if (mem) ::free(mem);
    }

    char *_readFile(const String &path, int *length) override
    {
        const char* p = path.buffer();
        LOG1(TAG_SPINE "reading file ", p);

        /* this is not used because all the data is preloaded
         * nevertheless, it could be used to load from package
         */
        char* data = 0;
	FILE *file = fopen(p, "rb");
        if (file)
        {
            fseek(file, 0, SEEK_END);
            *length = (int) ftell(file);
            fseek(file, 0, SEEK_SET);

            data = (char*)_alloc(*length, __FILE__, __LINE__);
            fread(data, 1, *length, file);
            fclose(file);
        }
	return data;
    }

};

SpineExtension* spine::getDefaultExtension()
{
    SSpineCtx* ctx = SSpineCtx::get();
    if (!ctx->_s_ext) ctx->_s_ext = new SSpineExtension();
    return (SpineExtension*)ctx->_s_ext;
}

struct STextureLoader: public TextureLoader
{
    // Called when the atlas loads the texture of a page.
    void load(AtlasPage& page, const String& path) override
    {
        ImTex t;
        const char* p = path.buffer();

        SSpineCtx* ctx = SSpineCtx::get();
        assert(ctx->_loader);

        LOG1(TAG_SPINE "requesting texture ", p);

        ImTex* tex = ctx->_loader->getTex(p);
        if (tex)
        {
            assert(tex->_tid);
            
            // store the Texture on the rendererObject so we can
            // retrieve it later for rendering.
            page.setRendererObject((void*)tex);
            
            // store the texture width and height on the spAtlasPage
            // so spine-c can calculate texture coordinates for
            // rendering.
            page.width = tex->_w;
            page.height = tex->_h;

            LOG1(TAG_SPINE "given texture ", p);
        }
    }

      // Called when the atlas is disposed and itself disposes its atlas pages.
    void unload(void* texture) override
    {
        ImTex* tex = (ImTex*)texture;
        assert(tex);
        
        SSpineCtx* ctx = SSpineCtx::get();
        assert(ctx->_loader);

        ctx->_loader->remove(tex->_name);
    }
};

struct SSpineRender
{
    enum BlendMode
        {
            // See http://esotericsoftware.com/git/spine-runtimes/blob/spine-libgdx/spine-libgdx/src/com/esotericsoftware/spine/BlendMode.java#L37
   // for how these translate to OpenGL source/destination blend modes.
            BLEND_NORMAL,
            BLEND_ADDITIVE,
            BLEND_MULTIPLY,
            BLEND_SCREEN,
        };


    int draw(AnimState& a)
    {
        static SVertex vertices[MAX_SVERTICES];
        static unsigned short indices[MAX_SVERTICES*3/2];
        static int quadIndices[] = {0, 1, 2, 2, 3, 0};

        Skeleton* sk = (Skeleton*)a._s_skel;
        assert(sk);

        int stride = sizeof(SVertex)/sizeof(float);
        int offv = 0;
        int offi = 0;

        // ensure packed!
        assert(stride == 8);

        Color skcol = sk->getColor();
        if (skcol.a == 0) return 0;

        float minx = FLT_MAX;
        float maxx = FLT_MIN;
        float miny = FLT_MAX;
        float maxy = FLT_MIN;

        size_t n = sk->getSlots().size();
        for (size_t i = 0; i < n; ++i)
        {
            Slot* slot = sk->getDrawOrder()[i];

            Attachment* attachment = slot->getAttachment();
            if (!attachment) continue;

            Color scol = slot->getColor();
            if (scol.a == 0) continue;

            // Fetch the blend mode from the slot and
            // translate it to the engine blend mode
            BlendMode engineBlendMode;
            switch (slot->getData().getBlendMode())
            {
            case BlendMode_Normal:
                engineBlendMode = BLEND_NORMAL;
                break;
            case BlendMode_Additive:
                engineBlendMode = BLEND_ADDITIVE;
                break;
            case BlendMode_Multiply:
                engineBlendMode = BLEND_MULTIPLY;
                break;
            case BlendMode_Screen:
                engineBlendMode = BLEND_SCREEN;
                break;
            default:
                // unknown Spine blend mode, fall back to
                // normal blend mode
                engineBlendMode = BLEND_NORMAL;
            }

            // Fill the vertices array, indices, and texture depending on the type of attachment
            ImTex* texture = NULL;
            
            if (attachment->getRTTI().isExactly(RegionAttachment::rtti))
            {
                // Cast to an spRegionAttachment so we can get the rendererObject
                // and compute the world vertices
                RegionAttachment* regionAttachment = (RegionAttachment*)attachment;
                

                Color acol = regionAttachment->getColor();
                if (acol.a == 0) continue; 

                float r = skcol.r * scol.r * acol.r;
                float g = skcol.g * scol.g * acol.g;
                float b = skcol.b * scol.b * acol.b;
                float a = skcol.a * scol.a * acol.a;
                
                texture = (ImTex*)((AtlasRegion*)regionAttachment->getRendererObject())->page->getRendererObject();

                assert(texture);

                // Ensure there is enough room for vertices
                SVertex* wv = vertices + offv;
                offv += 4;
                assert(offv < ASIZE(vertices));
                       
                regionAttachment->computeWorldVertices(slot->getBone(),
                                                       &wv->x,
                                                       0, stride);

                // copy color and UVs to the vertices
                Vector<float>& uvs = regionAttachment->getUVs();
                for (int j = 0, l = 0; j < 4; j++, l+=2)
                {
                    SVertex& vj = wv[j];
                    vj.r = r;
                    vj.g = g;
                    vj.b = b;
                    vj.a = a;
                    vj.u = uvs[l];
                    vj.v = uvs[l + 1];

                    if (vj.x < minx) minx = vj.x;
                    if (vj.x > maxx) maxx = vj.x;
                    if (vj.y < miny) miny = vj.y;
                    if (vj.y > maxy) maxy = vj.y;
                }

                // set the indices, 2 triangles forming a quad
                unsigned short* ip = indices + offi;
                offi += 6;
                assert(offi < ASIZE(indices));
                int vbase = offv - 4;
                for (int i = 0; i < 6; ++i)
                    *ip++ = quadIndices[i] + vbase;

            }
            else if (attachment->getRTTI().isExactly(MeshAttachment::rtti))
            {
                // Cast to an MeshAttachment so we can get the rendererObject
                // and compute the world vertices
                MeshAttachment* mesh = (MeshAttachment*)attachment;
                
                Color acol = mesh->getColor();
                if (acol.a == 0) continue; 

                float r = skcol.r * scol.r * acol.r;
                float g = skcol.g * scol.g * acol.g;
                float b = skcol.b * scol.b * acol.b;
                float a = skcol.a * scol.a * acol.a;

                // Ensure there is enough room for vertices
                int nv = mesh->getWorldVerticesLength() / 2;

                SVertex* wv = vertices + offv;
                offv += nv;
                assert(offv < ASIZE(vertices));

                texture = (ImTex*)((AtlasRegion*)mesh->getRendererObject())->page->getRendererObject();

                mesh->computeWorldVertices(*slot,
                                           0, nv*2,
                                           &wv->x, 0,
                                           stride);

                Vector<float>& uvs = mesh->getUVs();
                for (int j = 0, l = 0; j < nv; j++, l+=2)
                {
                    SVertex& vj = wv[j];
                    vj.r = r;
                    vj.g = g;
                    vj.b = b;
                    vj.a = a;
                    vj.u = uvs[l];
                    vj.v = uvs[l + 1];

                    if (vj.x < minx) minx = vj.x;
                    if (vj.x > maxx) maxx = vj.x;
                    if (vj.y < miny) miny = vj.y;
                    if (vj.y > maxy) maxy = vj.y;
                }
                
                Vector<unsigned short>& ii = mesh->getTriangles();
                int iic = ii.size();

                //LOG1(TAG_SPINE "rendering mesh verts:", nv << " ind:" << iic);
                
                unsigned short* ip = indices + offi;
                offi += iic;
                assert(offi < ASIZE(indices));
                int vbase = offv - nv;
                
                for (int i = 0; i < iic; ++i)
                    *ip++ = ii[i] + vbase;
            }
        }

        // now render
        //float sx = 2.0f/a._skel_w;
        //float maxxp = maxx*sx - (a._skel_x*sx + 1.0f);
        //LOG1(TAG_SPINE "minx: ", minx << " maxx:" << maxx << " maxxp:" << maxxp << " miny:" << miny << " maxy:" << maxy);
        
        
        //LOG1(TAG_SPINE "rendering vertices:", offv << ", indices:" << offi);

        if (offi > 0)
        {
            assert(offv > 0);

            sg_range sgr{ vertices, offv*sizeof(SVertex) };
            sg_update_buffer(state.anim.bind.vertex_buffers[0], &sgr);

            sg_range sgri{ indices, offi*sizeof(unsigned short) };
            sg_update_buffer(state.anim.bind.index_buffer, &sgri);

            // have we gone out of bounds?
            // this shouldn't happen (but it does) and so expand as needed.
            if (a._skel_x > minx) a._skel_x = minx;
            float dw = (maxx - a._skel_x) - a._skel_w;
            if (dw > 0) a._skel_w += dw;

            if (a._skel_y > miny) a._skel_y = miny;
            float dh = (maxy - a._skel_y) - a._skel_h;
            if (dh > 0) a._skel_h += dh;

        }

        return offi;
    }
};

bool SSpineCtx::update(AnimState& a)
{
    // assume loaded
    assert(a._ai);  // expect info

    bool changed = false;  // whether we need to redraw

    if (!a._begun)
    {
        a._begun = true;

        LOG1(TAG_SPINE "initialising ", a.name());
        
        // include in active states
        _states.push_back(&a);
        
        STextureLoader* tl = new STextureLoader();
        a._s_textureLoader = tl;

        SSpineRender* sr = new SSpineRender();
        a._s_renderer = sr;

        // to load from memory, pass dir as "images"
        // so that "images/foo.png" is requested by the texture loader
        // this will find this already loaded and return the
        // texture already created at this point.
        assert(a._atlas);
        Atlas* atlas = new Atlas(a._atlas, a._atlasz, "images", tl);

        if (atlas->getPages().size() == 0)
        {
            LOG1(TAG_SPINE "atlas failed ", a._ai->_atlas);
            delete atlas;
            atlas = 0;
            a._error = 1;
        }

        a._s_atlas = atlas;

        if (atlas)
        {
            LOG1(TAG_SPINE "created atlas ", a._ai->_atlas);

            SkeletonJson skj(atlas);
            skj.setScale(1);

            // we already have the skeleton json loaded 
            SkeletonData* skd = skj.readSkeletonData(a._anim);
            a._s_skeldat= skd;
        
            if (skd)
            {
                a._skel_x = skd->getX();
                a._skel_y = skd->getY();
                a._skel_w = skd->getWidth();
                a._skel_h = skd->getHeight();

                LOG1(TAG_SPINE "created skeleton ", a._ai->_anim << " w:" << a._skel_w << ", h:" << a._skel_h);

                AnimationStateData* asd = new AnimationStateData(skd);
                a._s_statedat = asd;

                // Set the default mix time between any pair of animations in seconds.
                asd->setDefaultMix(0.4f);

                // Set the mix time between from the "jump" to the "walk" animation to 0.2 seconds,
                // overwriting the default mix time for this from/to pair.
                //as->setMix("jump", "walk", 0.2f);

                Skeleton* sk = new Skeleton(skd);
                a._s_skel = sk;

                AnimationState* as = new AnimationState(asd);
                a._s_state = as;
            }
            else
            {
                a._error = 1;
                LOG1(TAG_SPINE "error reading skel ", a._ai->_anim << "; " << skj.getError().buffer());
            }
        }
    }

    if (a._ai->_done)
    {
        // do we have another queued?
        a.pop();
    }

    if (!a._ai->_done)
    {
        a._ai->_done = true; // mark handled regardless

        AnimationState* as = (AnimationState*)a._s_state;
        if (as)
        {
            SkeletonData* skd = (SkeletonData*)a._s_skeldat;
            assert(skd);

            const string& play = a._ai->_play;
            assert(!play.empty());
            
            Animation* am = skd->findAnimation(play.c_str());
            if (am)
            {
                if (a._ai->_append)
                {
                    LOG1(TAG_SPINE "appending animation ", play);
                    as->addAnimation(a._ai->_track, am,
                                     a._ai->_loop, a._ai->_delay);
                }
                else
                {
                    LOG1(TAG_SPINE "playing animation ", play);
                    as->setAnimation(a._ai->_track, am, a._ai->_loop);
                }

                if (!a._playing)
                {
                    a._tlast = a.now();
                    a._playing = true;
                }
            }
            else
            {
                LOG1(TAG_SPINE "cannot find animation ", play);
            }
        }
    }

    if (a._playing)
    {
        AnimationState* as = (AnimationState*)a._s_state;
        assert(as);

        Skeleton* sk = (Skeleton*)a._s_skel;
        assert(sk);

        AnimState::Time now = a.now();
        std::chrono::duration<double> dt = now - a._tlast;
        a._tlast = now;

        float dtf = dt.count();

        //LOG1(TAG_SPINE "animation dt ", dtf);

        if (dtf > 0)
        {
            sk->update(dtf); // needed?
            as->update(dtf);
            as->apply(*sk);

            sk->updateWorldTransform();
        
            // now render
            changed = true;
        }
    }
    
    return changed;
}


int SSpineCtx::render(AnimState& a)
{
    SSpineRender* sr = (SSpineRender*)a._s_renderer;
    assert(sr);
    return sr->draw(a);
}

bool SSpineCtx::animOp(AnimState& a)
{
    bool r = false;

    assert(a._ai);
    AnimInfo::Op op = a._ai->_op;
    assert(op);
    
    Skeleton* sk = (Skeleton*)a._s_skel;
    if (sk)
    {
        Slot* s = sk->findSlot(a._ai->_play.c_str());   // does not name animation for ops
        if (s)
        {
            switch (op)
            {
                // show and hide work by setting the slot invisible
                // this is preferred to detaching as it means the
                // slot will still be animated to position.
            case AnimInfo::op_show:
                {
                    Color& c = s->getColor();
                    c.a = 1;
                }
                break;
            case AnimInfo::op_hide:
                {
                    Color& c = s->getColor();
                    c.a = 0;
                }
                break;
            }
        }
    }

    // regardless, retire the op
    a._ai->_done = true;
    
    return r;
}
