varying highp vec2 coord;
uniform sampler2D src;
uniform lowp float qt_Opacity;
void main() {
    lowp vec4 tex = texture2D(src, coord);
    float r = 0.5 * tex.r + 0.5 * tex.g;
    gl_FragColor = vec4(vec3(r,tex.g,tex.b),tex.a) * qt_Opacity;
}
