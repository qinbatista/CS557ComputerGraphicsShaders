#version 120

uniform float uKa, uKd, uKs;		// coefficients of each type of lighting
uniform float uShininess;		// specular exponent
uniform float uS0, uT0, uD;		// square pattern
uniform float diam, Ar, Br, uTol;		// diam
varying vec2 vST;			// texture coords
varying vec3 vN;			// normal vector
varying vec3 vL;			// vector from point to light
varying vec3 vE;			// vector from point to eye

void main() {
    vec3 Normal = normalize(vN);
    vec3 Light = normalize(vL);
    vec3 Eye = normalize(vE);

    vec3 SpecularColor = vec3(1., 1., 1.);
    vec3 myColor = vec3(1.0, 0.5, 0.0);
    vec3 myPatternColor = vec3(0.5, 0.13, 0);
    //square pattern
	// float ds = uD;
	// float dt = uD;
	// if(	uS0-ds/2. <= vST.s  &&  vST.s <= uS0+ds/2.  &&
	// 	uT0-dt/2. <= vST.t  &&  vST.t <= uT0+dt/2.  )
	// {
	// 		myColor = vec3( 1., 0., 0. );
	// }

    //circular pattern
    float sc = 0;
    float X = 0;
    float tc = 0;
    float Y = 0;
    int numins = int(vST.s / diam);
    int numint = int(vST.t / diam);
    float R = diam / 2;
    if(numint == 1 || numint == 3 || numint == 5 || numint == 7 || numint == 9 || numint == 11) {
        if(vST.s < numins * diam + R) {
            numins = numins - 1;
        }
        sc = numins * diam + R + R;
        tc = numint * diam + R;
        X = (vST.s - sc);
        Y = (vST.t - tc);
    } else {
        sc = numins * diam + R;
        tc = numint * diam + R;
        X = (vST.s - sc);
        Y = (vST.t - tc);
    }
    if(X / Ar * X / Ar + Y / Br * Y / Br <= (R + uTol) * (R + uTol)) {
        if(X / Ar * X / Ar + Y / Br * Y / Br >= (R - uTol) * (R - uTol)) {
            float t = smoothstep((R - uTol) * (R - uTol), (R + uTol) * (R + uTol), (X / Ar) * (X / Br) + (Y / Ar) * (Y / Br));
            myColor = mix(myPatternColor, myColor, t);
        } else {
            myColor = myPatternColor;
        }
    }

    vec3 ambient = uKa * myColor;

    float d = max(dot(Normal, Light), 0.);       // only do diffuse if the light can see the point
    vec3 diffuse = uKd * d * myColor;

    float s = 0.;
    if(dot(Normal, Light) > 0.)	          // only do specular if the light can see the point
    {
        vec3 ref = normalize(reflect(-Light, Normal));
        s = pow(max(dot(Eye, ref), 0.), uShininess);
    }
    vec3 specular = uKs * s * SpecularColor.rgb;
    gl_FragColor = vec4(ambient + diffuse + specular, 1.);
}
