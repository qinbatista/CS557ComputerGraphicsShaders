#version 120

varying float vLightIntensity;
uniform float uTime;
uniform float K;
uniform float P;
varying vec2 vST;		// texture coords
varying vec3 vN;		// normal vector
varying vec3 vL;		// vector from point to light
varying vec3 vE;		// vector from point to eye
vec3 LightPosition = vec3(0., 5., 5.);
const vec3 LIGHTPOS = vec3(-2., 0., 10.);
const float AMP = 0.2;
const float PI = 3.14159265;
const float W = 2.;
varying vec3 vMCposition;

uniform float uNoiseFreq, uNoiseMag;
uniform sampler3D Noise3;

varying vec3 vMC;
varying vec3 vNs;
varying vec3 vEs;

vec3 RotateNormal(float angx, float angy, vec3 n) {
    float cx = cos(angx);
    float sx = sin(angx);
    float cy = cos(angy);
    float sy = sin(angy);

        // rotate about x:
    float yp = n.y * cx - n.z * sx;    // y'
    n.z = n.y * sx + n.z * cx;    // z'
    n.y = yp;
        // n.x      =  n.x;

        // rotate about y:
    float xp = n.x * cy + n.z * sy;    // x'
    n.z = -n.x * sy + n.z * cy;    // z'
    n.x = xp;
        // n.y      =  n.y;

    return normalize(n);
}
void main() {
    vMCposition = gl_Vertex.xyz;
    vec4 newVertex = gl_Vertex;
	//set wave
    vST = gl_MultiTexCoord0.st;
    vec3 vert = gl_Vertex.xyz;
    vec4 nv = texture3D(Noise3, uNoiseFreq * vMCposition);
    vert.z = vert.z + K * (1 - vMCposition.y) * sin(2 * W * PI * vMCposition.x / P) + nv.z * uNoiseMag;

    vec4 ECposition = gl_ModelViewMatrix * newVertex;

	//set wave normal
    float dzdx = K * (1 - vMCposition.y) * (2. * PI / P) * cos(2. * PI * vMCposition.x / P);
    float dzdy = -K * sin(2. * PI * vMCposition.x / P);
    vec3 Tx = vec3(1., 0., dzdx);
    vec3 Ty = vec3(0., 1., dzdy);
    vec3 norm = normalize(cross(Tx, Ty));

	//set normal
    vec4 nvx = texture3D(Noise3, uNoiseFreq * vMCposition);
    float angx = nvx.r + nvx.g + nvx.b + nvx.a - 2.;	// -1. to +1.
    angx *= uNoiseMag;
    vec4 nvy = texture3D(Noise3, uNoiseFreq * vec3(vMCposition.xy, vMCposition.z + 0.5));
    float angy = nvy.r + nvy.g + nvy.b + nvy.a - 2.;	// -1. to +1.
    angy *= uNoiseMag;
    norm = RotateNormal(angx, angy, norm);

	// vec3 norm = normalize(vert); //normal normal

	ECposition = gl_ModelViewMatrix * vec4(vert, 1.);
    vN = normalize(gl_NormalMatrix * norm);	// normal vector
    vL = LightPosition - ECposition.xyz;		// vector from the point
							// to the light position
    vE = ECposition.xyz - vec3(0., 0., 0.);		// vector from the point
							// to the eye position
    gl_Position = gl_ModelViewProjectionMatrix * vec4(vert, 1.);

}
