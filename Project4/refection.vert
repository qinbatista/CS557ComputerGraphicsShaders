#version 120

uniform float uK, uP;

varying vec3 vMC;
varying vec3 vNs;
varying vec3 vEs;

void main() {
    vMC = gl_Vertex.xyz;
    vec4 newVertex = gl_Vertex;
	// -?????
	//set wave
    vST = gl_MultiTexCoord0.st;
    vec3 vert = gl_Vertex.xyz;
    vec4 nv = texture3D(Noise3, uNoiseFreq * vMCposition);
    vert.z = vert.z + K * (1 - vMCposition.y) * sin(2 * W * PI * vMCposition.x / P) + nv.z * uNoiseMag;

    vec4 ECposition = gl_ModelViewMatrix * newVertex;

	// float dzdx = ?????
	// float dzdy = ?????
	// vec3 xtangent = ?????
	// vec3 ytangent = ?????
	//set wave normal
    float dzdx = K * (1 - vMCposition.y) * (2. * PI / P) * cos(2. * PI * vMCposition.x / P);
    float dzdy = -K * sin(2. * PI * vMCposition.x / P);
    vec3 Tx = vec3(1., 0., dzdx);
    vec3 Ty = vec3(0., 1., dzdy);
    vec3 norm = normalize(cross(Tx, Ty));

    vec3 newNormal = normalize(gl_NormalMatrix * norm);

    vNs = newNormal;
    vEs = ECposition.xyz - vec3(0., 0., 0.);
	       		// vector from the eye position to the point

    gl_Position = gl_ModelViewProjectionMatrix * newVertex;
}
