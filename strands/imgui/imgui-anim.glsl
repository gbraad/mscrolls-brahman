@ctype mat4 hmm_mat4

@vs vs
uniform vs_params {
    vec2 scale;
    vec2 offset;
};

in vec4 pos;
in vec4 color0;
in vec2 texcoord0;

out vec4 color;
out vec2 uv;

void main() {
    gl_Position = vec4(pos.xy * scale + offset, 0.0, 1.0);
    color = color0;
    uv = texcoord0;
}
@end

#pragma sokol @fs fs
uniform sampler2D tex;

in vec4 color;
in vec2 uv;
out vec4 frag_color;

void main() {
    frag_color = texture(tex, uv) * color;
}
@end

@program scene vs fs
