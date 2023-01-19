#version 120

uniform float uKa, uKd, uKs;		// coefficients of each type of lighting
uniform float uShininess;		// specular exponent
uniform float uS0, uT0, uD;		// square pattern
uniform float diam, Ar, Br, uTol;		// diam
varying vec2 vST;			// texture coords
varying vec3 vN;			// normal vector
varying vec3 vL;			// vector from point to light
varying vec3 vE;			// vector from point to eye

uniform sampler3D Noise3;
uniform float uNoiseFreq, uNoiseMag;
varying vec3 vMCposition;
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

	vec4 nv = texture3D(Noise3, uNoiseFreq * vMCposition);
	float n = nv.r + nv.g + nv.b + nv.a; //rangeis1.->3.
	// n = (n - 1.) / 2.; // range is now 0. -> 1.
	n = n - 2.; // range is now -1. -> 1.
	n *= uNoiseMag;

    //circular pattern
	float sc = 0;
	float X = 0;
	float tc = 0;
	float Y = 0;
	int numins = int(vST.s / diam);
	int numint = int(vST.t / diam);
	float R = diam / 2;
	int value = numint / 2;
	int remain = numint - value * 2;
	if(remain == 1) {
		if(vST.s < numins * diam + R) {
			numins = numins - 1;
		}
		sc = numins * diam + R + R;
		tc = numint * diam + R;
		X = (vST.s+n - sc);
		Y = (vST.t+n - tc);
	} else {
		sc = numins * diam + R;
		tc = numint * diam + R;
		X = (vST.s+n - sc);
		Y = (vST.t+n - tc);
	}
	float Dist = sqrt(X / Ar * X / Ar + Y / Br * Y / Br);
	float OffsetDist = Dist + n;
	float scale = OffsetDist / Dist;
	Dist = Dist+n;
	if(Dist <= (R + uTol)) {
		if(Dist >= (R - uTol)) {
			float t = smoothstep((R - uTol), (R + uTol), Dist);
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
