#pragma once
/*
    #version:1# (machine generated, don't edit!)

    Generated by sokol-shdc (https://github.com/floooh/sokol-tools)

    Cmdline: sokol-shdc -i imgui-anim.glsl -o imgui-anim.glsl.h -l glsl100:glsl330

    Overview:

        Shader program 'scene':
            Get shader desc: scene_shader_desc(sg_query_backend());
            Vertex shader: vs
                Attribute slots:
                    ATTR_vs_pos = 0
                    ATTR_vs_color0 = 1
                    ATTR_vs_texcoord0 = 2
                Uniform block 'vs_params':
                    C struct: vs_params_t
                    Bind slot: SLOT_vs_params = 0
            Fragment shader: fs
                Image 'tex':
                    Type: SG_IMAGETYPE_2D
                    Component Type: SG_SAMPLERTYPE_FLOAT
                    Bind slot: SLOT_tex = 0


    Shader descriptor structs:

        sg_shader scene = sg_make_shader(scene_shader_desc(sg_query_backend()));

    Vertex attribute locations for vertex shader 'vs':

        sg_pipeline pip = sg_make_pipeline(&(sg_pipeline_desc){
            .layout = {
                .attrs = {
                    [ATTR_vs_pos] = { ... },
                    [ATTR_vs_color0] = { ... },
                    [ATTR_vs_texcoord0] = { ... },
                },
            },
            ...});

    Image bind slots, use as index in sg_bindings.vs_images[] or .fs_images[]

        SLOT_tex = 0;

    Bind slot and C-struct for uniform block 'vs_params':

        vs_params_t vs_params = {
            .scale = ...;
            .offset = ...;
        };
        sg_apply_uniforms(SG_SHADERSTAGE_[VS|FS], SLOT_vs_params, &SG_RANGE(vs_params));

*/
#include <stdint.h>
#include <stdbool.h>
#if !defined(SOKOL_SHDC_ALIGN)
  #if defined(_MSC_VER)
    #define SOKOL_SHDC_ALIGN(a) __declspec(align(a))
  #else
    #define SOKOL_SHDC_ALIGN(a) __attribute__((aligned(a)))
  #endif
#endif
#define ATTR_vs_pos (0)
#define ATTR_vs_color0 (1)
#define ATTR_vs_texcoord0 (2)
#define SLOT_tex (0)
#define SLOT_vs_params (0)
#pragma pack(push,1)
SOKOL_SHDC_ALIGN(16) typedef struct vs_params_t {
    float scale[2];
    float offset[2];
} vs_params_t;
#pragma pack(pop)
/*
    #version 330
    
    uniform vec4 vs_params[1];
    layout(location = 0) in vec4 pos;
    out vec4 color;
    layout(location = 1) in vec4 color0;
    out vec2 uv;
    layout(location = 2) in vec2 texcoord0;
    
    void main()
    {
        gl_Position = vec4((pos.xy * vs_params[0].xy) + vs_params[0].zw, 0.0, 1.0);
        color = color0;
        uv = texcoord0;
    }
    
*/
static const char vs_source_glsl330[320] = {
    0x23,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x20,0x33,0x33,0x30,0x0a,0x0a,0x75,0x6e,
    0x69,0x66,0x6f,0x72,0x6d,0x20,0x76,0x65,0x63,0x34,0x20,0x76,0x73,0x5f,0x70,0x61,
    0x72,0x61,0x6d,0x73,0x5b,0x31,0x5d,0x3b,0x0a,0x6c,0x61,0x79,0x6f,0x75,0x74,0x28,
    0x6c,0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x30,0x29,0x20,0x69,0x6e,
    0x20,0x76,0x65,0x63,0x34,0x20,0x70,0x6f,0x73,0x3b,0x0a,0x6f,0x75,0x74,0x20,0x76,
    0x65,0x63,0x34,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x6c,0x61,0x79,0x6f,0x75,
    0x74,0x28,0x6c,0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x31,0x29,0x20,
    0x69,0x6e,0x20,0x76,0x65,0x63,0x34,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x30,0x3b,0x0a,
    0x6f,0x75,0x74,0x20,0x76,0x65,0x63,0x32,0x20,0x75,0x76,0x3b,0x0a,0x6c,0x61,0x79,
    0x6f,0x75,0x74,0x28,0x6c,0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x32,
    0x29,0x20,0x69,0x6e,0x20,0x76,0x65,0x63,0x32,0x20,0x74,0x65,0x78,0x63,0x6f,0x6f,
    0x72,0x64,0x30,0x3b,0x0a,0x0a,0x76,0x6f,0x69,0x64,0x20,0x6d,0x61,0x69,0x6e,0x28,
    0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,
    0x69,0x6f,0x6e,0x20,0x3d,0x20,0x76,0x65,0x63,0x34,0x28,0x28,0x70,0x6f,0x73,0x2e,
    0x78,0x79,0x20,0x2a,0x20,0x76,0x73,0x5f,0x70,0x61,0x72,0x61,0x6d,0x73,0x5b,0x30,
    0x5d,0x2e,0x78,0x79,0x29,0x20,0x2b,0x20,0x76,0x73,0x5f,0x70,0x61,0x72,0x61,0x6d,
    0x73,0x5b,0x30,0x5d,0x2e,0x7a,0x77,0x2c,0x20,0x30,0x2e,0x30,0x2c,0x20,0x31,0x2e,
    0x30,0x29,0x3b,0x0a,0x20,0x20,0x20,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,
    0x63,0x6f,0x6c,0x6f,0x72,0x30,0x3b,0x0a,0x20,0x20,0x20,0x20,0x75,0x76,0x20,0x3d,
    0x20,0x74,0x65,0x78,0x63,0x6f,0x6f,0x72,0x64,0x30,0x3b,0x0a,0x7d,0x0a,0x0a,0x00,

};
/*
    #version 330
    
    uniform sampler2D tex;
    
    layout(location = 0) out vec4 frag_color;
    in vec2 uv;
    in vec4 color;
    
    void main()
    {
        frag_color = texture(tex, uv) * color;
    }
    
*/
static const char fs_source_glsl330[169] = {
    0x23,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x20,0x33,0x33,0x30,0x0a,0x0a,0x75,0x6e,
    0x69,0x66,0x6f,0x72,0x6d,0x20,0x73,0x61,0x6d,0x70,0x6c,0x65,0x72,0x32,0x44,0x20,
    0x74,0x65,0x78,0x3b,0x0a,0x0a,0x6c,0x61,0x79,0x6f,0x75,0x74,0x28,0x6c,0x6f,0x63,
    0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x30,0x29,0x20,0x6f,0x75,0x74,0x20,0x76,
    0x65,0x63,0x34,0x20,0x66,0x72,0x61,0x67,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,
    0x69,0x6e,0x20,0x76,0x65,0x63,0x32,0x20,0x75,0x76,0x3b,0x0a,0x69,0x6e,0x20,0x76,
    0x65,0x63,0x34,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x0a,0x76,0x6f,0x69,0x64,
    0x20,0x6d,0x61,0x69,0x6e,0x28,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x66,0x72,
    0x61,0x67,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,0x74,0x65,0x78,0x74,0x75,
    0x72,0x65,0x28,0x74,0x65,0x78,0x2c,0x20,0x75,0x76,0x29,0x20,0x2a,0x20,0x63,0x6f,
    0x6c,0x6f,0x72,0x3b,0x0a,0x7d,0x0a,0x0a,0x00,
};
/*
    #version 100
    
    uniform vec4 vs_params[1];
    attribute vec4 pos;
    varying vec4 color;
    attribute vec4 color0;
    varying vec2 uv;
    attribute vec2 texcoord0;
    
    void main()
    {
        gl_Position = vec4((pos.xy * vs_params[0].xy) + vs_params[0].zw, 0.0, 1.0);
        color = color0;
        uv = texcoord0;
    }
    
*/
static const char vs_source_glsl100[286] = {
    0x23,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x20,0x31,0x30,0x30,0x0a,0x0a,0x75,0x6e,
    0x69,0x66,0x6f,0x72,0x6d,0x20,0x76,0x65,0x63,0x34,0x20,0x76,0x73,0x5f,0x70,0x61,
    0x72,0x61,0x6d,0x73,0x5b,0x31,0x5d,0x3b,0x0a,0x61,0x74,0x74,0x72,0x69,0x62,0x75,
    0x74,0x65,0x20,0x76,0x65,0x63,0x34,0x20,0x70,0x6f,0x73,0x3b,0x0a,0x76,0x61,0x72,
    0x79,0x69,0x6e,0x67,0x20,0x76,0x65,0x63,0x34,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x3b,
    0x0a,0x61,0x74,0x74,0x72,0x69,0x62,0x75,0x74,0x65,0x20,0x76,0x65,0x63,0x34,0x20,
    0x63,0x6f,0x6c,0x6f,0x72,0x30,0x3b,0x0a,0x76,0x61,0x72,0x79,0x69,0x6e,0x67,0x20,
    0x76,0x65,0x63,0x32,0x20,0x75,0x76,0x3b,0x0a,0x61,0x74,0x74,0x72,0x69,0x62,0x75,
    0x74,0x65,0x20,0x76,0x65,0x63,0x32,0x20,0x74,0x65,0x78,0x63,0x6f,0x6f,0x72,0x64,
    0x30,0x3b,0x0a,0x0a,0x76,0x6f,0x69,0x64,0x20,0x6d,0x61,0x69,0x6e,0x28,0x29,0x0a,
    0x7b,0x0a,0x20,0x20,0x20,0x20,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,
    0x6e,0x20,0x3d,0x20,0x76,0x65,0x63,0x34,0x28,0x28,0x70,0x6f,0x73,0x2e,0x78,0x79,
    0x20,0x2a,0x20,0x76,0x73,0x5f,0x70,0x61,0x72,0x61,0x6d,0x73,0x5b,0x30,0x5d,0x2e,
    0x78,0x79,0x29,0x20,0x2b,0x20,0x76,0x73,0x5f,0x70,0x61,0x72,0x61,0x6d,0x73,0x5b,
    0x30,0x5d,0x2e,0x7a,0x77,0x2c,0x20,0x30,0x2e,0x30,0x2c,0x20,0x31,0x2e,0x30,0x29,
    0x3b,0x0a,0x20,0x20,0x20,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,0x63,0x6f,
    0x6c,0x6f,0x72,0x30,0x3b,0x0a,0x20,0x20,0x20,0x20,0x75,0x76,0x20,0x3d,0x20,0x74,
    0x65,0x78,0x63,0x6f,0x6f,0x72,0x64,0x30,0x3b,0x0a,0x7d,0x0a,0x0a,0x00,
};
/*
    #version 100
    precision mediump float;
    precision highp int;
    
    uniform highp sampler2D tex;
    
    varying highp vec2 uv;
    varying highp vec4 color;
    
    void main()
    {
        gl_FragData[0] = texture2D(tex, uv) * color;
    }
    
*/
static const char fs_source_glsl100[207] = {
    0x23,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x20,0x31,0x30,0x30,0x0a,0x70,0x72,0x65,
    0x63,0x69,0x73,0x69,0x6f,0x6e,0x20,0x6d,0x65,0x64,0x69,0x75,0x6d,0x70,0x20,0x66,
    0x6c,0x6f,0x61,0x74,0x3b,0x0a,0x70,0x72,0x65,0x63,0x69,0x73,0x69,0x6f,0x6e,0x20,
    0x68,0x69,0x67,0x68,0x70,0x20,0x69,0x6e,0x74,0x3b,0x0a,0x0a,0x75,0x6e,0x69,0x66,
    0x6f,0x72,0x6d,0x20,0x68,0x69,0x67,0x68,0x70,0x20,0x73,0x61,0x6d,0x70,0x6c,0x65,
    0x72,0x32,0x44,0x20,0x74,0x65,0x78,0x3b,0x0a,0x0a,0x76,0x61,0x72,0x79,0x69,0x6e,
    0x67,0x20,0x68,0x69,0x67,0x68,0x70,0x20,0x76,0x65,0x63,0x32,0x20,0x75,0x76,0x3b,
    0x0a,0x76,0x61,0x72,0x79,0x69,0x6e,0x67,0x20,0x68,0x69,0x67,0x68,0x70,0x20,0x76,
    0x65,0x63,0x34,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x0a,0x76,0x6f,0x69,0x64,
    0x20,0x6d,0x61,0x69,0x6e,0x28,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x67,0x6c,
    0x5f,0x46,0x72,0x61,0x67,0x44,0x61,0x74,0x61,0x5b,0x30,0x5d,0x20,0x3d,0x20,0x74,
    0x65,0x78,0x74,0x75,0x72,0x65,0x32,0x44,0x28,0x74,0x65,0x78,0x2c,0x20,0x75,0x76,
    0x29,0x20,0x2a,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x7d,0x0a,0x0a,0x00,
};
#if !defined(SOKOL_GFX_INCLUDED)
  #error "Please include sokol_gfx.h before imgui-anim.glsl.h"
#endif
static inline const sg_shader_desc* scene_shader_desc(sg_backend backend) {
  if (backend == SG_BACKEND_GLCORE33) {
    static sg_shader_desc desc;
    static bool valid;
    if (!valid) {
      valid = true;
      desc.attrs[0].name = "pos";
      desc.attrs[1].name = "color0";
      desc.attrs[2].name = "texcoord0";
      desc.vs.source = vs_source_glsl330;
      desc.vs.entry = "main";
      desc.vs.uniform_blocks[0].size = 16;
      desc.vs.uniform_blocks[0].uniforms[0].name = "vs_params";
      desc.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
      desc.vs.uniform_blocks[0].uniforms[0].array_count = 1;
      desc.fs.source = fs_source_glsl330;
      desc.fs.entry = "main";
      desc.fs.images[0].name = "tex";
      desc.fs.images[0].image_type = SG_IMAGETYPE_2D;
      desc.fs.images[0].sampler_type = SG_SAMPLERTYPE_FLOAT;
      desc.label = "scene_shader";
    };
    return &desc;
  }
  if (backend == SG_BACKEND_GLES2) {
    static sg_shader_desc desc;
    static bool valid;
    if (!valid) {
      valid = true;
      desc.attrs[0].name = "pos";
      desc.attrs[1].name = "color0";
      desc.attrs[2].name = "texcoord0";
      desc.vs.source = vs_source_glsl100;
      desc.vs.entry = "main";
      desc.vs.uniform_blocks[0].size = 16;
      desc.vs.uniform_blocks[0].uniforms[0].name = "vs_params";
      desc.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
      desc.vs.uniform_blocks[0].uniforms[0].array_count = 1;
      desc.fs.source = fs_source_glsl100;
      desc.fs.entry = "main";
      desc.fs.images[0].name = "tex";
      desc.fs.images[0].image_type = SG_IMAGETYPE_2D;
      desc.fs.images[0].sampler_type = SG_SAMPLERTYPE_FLOAT;
      desc.label = "scene_shader";
    };
    return &desc;
  }
  return 0;
}