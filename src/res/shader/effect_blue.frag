varying highp vec2 coord;
uniform sampler2D src;
uniform lowp float qt_Opacity;
void main() {
    lowp vec4 tex = texture2D(src, coord);
    float b = 0.5 * tex.r + 0.5 * tex.g;
    gl_FragColor = vec4(vec3(tex.r,tex.g,b),tex.a) * qt_Opacity;
}
