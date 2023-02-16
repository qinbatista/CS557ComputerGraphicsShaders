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

uniform float sizeS;
uniform float sizeT;


varying vec2 vST;
uniform bool uUseCircle;
uniform float uRadius;
bool inRectangle() {
    float s = vST.s;
    float t = vST.t;
    if(s <= uSc + uDs && s >= uSc - uDs && t <= uTc + uDt && t >= uTc - uDt) {
        return true;
    }
    return false;
}

bool inCircle() {
    if(sqrt(pow((vST.s - uSc), 2) + pow((vST.t - uTc), 2)) <= uRadius) {
        return true;
    }
    return false;
}

void main() {
    vec4 newcolor;

    bool inLens = false;
    if(uUseCircle) {
        if(inCircle()) {
            inLens = true;
        }
    } else {
        if(inRectangle()) {
            inLens = true;
        }
    }
    if(inLens) {
        vec2 newVST = vec2(uSc + (vST.s - uSc) / uMagFactor, uTc + (vST.t - uTc) / uMagFactor);

        vec2 rotateVST;
        rotateVST.s = uSc + ((newVST.s - uSc) * cos(uRotAngle) - ((newVST.t - uTc) * sin(uRotAngle)));
        rotateVST.t = uTc + ((newVST.s - uSc) * sin(uRotAngle) + (newVST.t - uTc) * cos(uRotAngle));

        //magnifying.

        newcolor = texture2D(uImageUnit, rotateVST).rgb;

        // ivec2 ires = textureSize(uImageUnit, 0);
        float ResS = float(sizeS);
        float ResT = float(sizeT);

        vec2 stp0 = vec2(1. / ResS, 0.);
        vec2 st0p = vec2(0., 1. / ResT);
        vec2 stpp = vec2(1. / ResS, 1. / ResT);
        vec2 stpm = vec2(1. / ResS, -1. / ResT);
        vec3 i00 = texture2D(uImageUnit, rotateVST).rgb;
        vec3 im1m1 = texture2D(uImageUnit, rotateVST - stpp).rgb;
        vec3 ip1p1 = texture2D(uImageUnit, rotateVST + stpp).rgb;
        vec3 im1p1 = texture2D(uImageUnit, rotateVST - stpm).rgb;
        vec3 ip1m1 = texture2D(uImageUnit, rotateVST + stpm).rgb;
        vec3 im10 = texture2D(uImageUnit, rotateVST - stp0).rgb;
        vec3 ip10 = texture2D(uImageUnit, rotateVST + stp0).rgb;
        vec3 i0m1 = texture2D(uImageUnit, rotateVST - st0p).rgb;
        vec3 i0p1 = texture2D(uImageUnit, rotateVST + st0p).rgb;
        vec3 target = vec3(0., 0., 0.);
        target += 1. * (im1m1 + ip1m1 + ip1p1 + im1p1);
        target += 2. * (im10 + ip10 + i0m1 + i0p1);
        target += 4. * (i00);
        target /= 16.;
        gl_FragColor = vec4(mix(target, newcolor, uSharpFactor), 1.);

    } else {
        newcolor = texture2D(uImageUnit, vST);
        gl_FragColor = newcolor;
    }

}