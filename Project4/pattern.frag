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
uniform float uAlpha;			// vector from point to eye


uniform float uEta;
uniform float uMix;
uniform float uWhiteMix;
uniform samplerCube uReflectUnit;
uniform samplerCube uRefractUnit;


const vec4 WHITE = vec4(1., 1., 1., 1.);

vec3 RotateNormal(float angx, float angy, vec3 n) {
    float cx = cos(angx);
    float sx = sin(angx);
    float cy = cos(angy);
    float sy = sin(angy);

	// rotate about x:
    float yp = n.y * cx - n.z * sx;	// y'
    n.z = n.y * sx + n.z * cx;	// z'
    n.y = yp;
	// n.x      =  n.x;

	// rotate about y:
    float xp = n.x * cy + n.z * sy;	// x'
    n.z = -n.x * sy + n.z * cy;	// z'
    n.x = xp;
	// n.y      =  n.y;

    return normalize(n);
}

void main( )
{
	vec3 Normal = vN;	// remember to unitize this
	vec3 Eye =    vE;	// remember to unitize this

	vec4 nvx = texture3D( Noise3, uNoiseFreq*vMCposition );
	vec4 nvy = texture3D( Noise3, uNoiseFreq*vec3(vMCposition.xy,vMCposition.z+0.5) );

	float angx = nvx.r + nvx.g + nvx.b + nvx.a;	//  1. -> 3.
	angx = angx - 2.;				// -1. -> 1.
	angx *= uNoiseMag;

	float angy = nvy.r + nvy.g + nvy.b + nvy.a;	//  1. -> 3.
	angy = angy - 2.;				// -1. -> 1.
	angy *= uNoiseMag;

	Normal = RotateNormal( angx, angy, Normal );
	Normal = normalize( gl_NormalMatrix * Normal );

	vec3 reflectVector =  reflect( Eye, Normal );
	vec4 reflectColor =  textureCube( uReflectUnit, reflectVector );
	vec3 refractVector = refract( Eye, Normal, uEta );
	vec4 refractColor = textureCube( uRefractUnit, refractVector );

	if( all( equal( refractVector, vec3(0.,0.,0.) ) ) )
	{
		refractColor = reflectColor;
	}
	else
	{
		refractColor = textureCube( uRefractUnit, refractVector );
		refractColor = mix( refractColor, WHITE, uWhiteMix );
	}
    vec4 color = mix( refractColor, reflectColor, uMix );
    gl_FragColor = vec4( color.rgb, 1. );
	// gl_FragColor = mix( ?????
}