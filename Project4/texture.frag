#version 120

uniform sampler2D TexUnit;
varying vec2 vST;

void main() {
    vec3 newcolor = texture2D(TexUnit, vST).rgb;
    gl_FragColor = vec4(newcolor, 1.);
}