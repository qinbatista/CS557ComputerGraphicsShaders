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


void main() {
    vec4 newcolor;
    newcolor = texture2D(uImageUnit, gl_TexCoord[1].st);
    gl_FragColor = newcolor;
}