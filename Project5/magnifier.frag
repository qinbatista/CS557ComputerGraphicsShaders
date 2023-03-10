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

void main() {
    vec4 newcolor;

    bool shouldDraw = false;
    if(uUseCircle) {
        if(sqrt(pow((vST.s - uSc), 2) + pow((vST.t - uTc), 2)) <= uRadius)
            shouldDraw = true;
    } else {
        float s = vST.s;
        float t = vST.t;
        if(s <= uSc + uDs && s >= uSc - uDs && t <= uTc + uDt && t >= uTc - uDt) {
            shouldDraw = true;
        }
    }
    if(shouldDraw) {
        vec2 newVST = vec2(uSc + (vST.s - uSc) / uMagFactor, uTc + (vST.t - uTc) / uMagFactor);

        vec2 rotate;
        rotate.s = uSc + ((newVST.s - uSc) * cos(uRotAngle) - ((newVST.t - uTc) * sin(uRotAngle)));
        rotate.t = uTc + ((newVST.s - uSc) * sin(uRotAngle) + (newVST.t - uTc) * cos(uRotAngle));

        newcolor = texture2D(uImageUnit, rotate).rgb;

        float ResS = float(sizeS);
        float ResT = float(sizeT);

        vec2 P = vec2(1. / ResS, 0.);
        vec2 P0 = vec2(0., 1. / ResT);
        vec2 PP = vec2(1. / ResS, 1. / ResT);
        vec2 PM = vec2(1. / ResS, -1. / ResT);
        vec3 P00 = texture2D(uImageUnit, rotate).rgb;
        vec3 im1m1 = texture2D(uImageUnit, rotate - PP).rgb;
        vec3 ip1p1 = texture2D(uImageUnit, rotate + PP).rgb;
        vec3 im1p1 = texture2D(uImageUnit, rotate - PM).rgb;
        vec3 ip1m1 = texture2D(uImageUnit, rotate + PM).rgb;
        vec3 im10 = texture2D(uImageUnit, rotate - P).rgb;
        vec3 ip10 = texture2D(uImageUnit, rotate + P).rgb;
        vec3 i0m1 = texture2D(uImageUnit, rotate - P0).rgb;
        vec3 i0p1 = texture2D(uImageUnit, rotate + P0).rgb;
        vec3 target = vec3(0., 0., 0.);
        target += 1. * (im1m1 + ip1m1 + ip1p1 + im1p1);
        target += 2. * (im10 + ip10 + i0m1 + i0p1);
        target += 4. * (P00);
        target /= 16.;
        gl_FragColor = vec4(mix(target, newcolor, uSharpFactor), 1.);

    } else {
        newcolor = texture2D(uImageUnit, vST);
        gl_FragColor = newcolor;
    }

}