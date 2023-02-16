#version 120

uniform sampler2D uImageUnit;
uniform float uSc;
uniform float uTc;
uniform float uDs;
uniform float uDt;
uniform float uRad;
uniform float uMagFactor;
uniform float uRotAngle;
uniform float uSharpFactor;
varying vec2 vST;


void main() {
    vec4 newcolor;
    newcolor = texture2D(uImageUnit, vST);
    gl_FragColor = newcolor;
}