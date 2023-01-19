#version 120

uniform float	uTime;
varying  vec2  vST;		// texture coords
varying  vec3  vN;		// normal vector
varying  vec3  vL;		// vector from point to light
varying  vec3  vE;		// vector from point to eye

vec3 LightPosition = vec3(  0., 5., 5. );

const float AMP = 0.2;
const float PI = 3.14159265;
const float W = 2.;

void
main( )
{ 
	vST = gl_MultiTexCoord0.st;
	vec3 vert = gl_Vertex.xyz;
	vec3 norm = normalize( vert );
	vec4 ECposition = gl_ModelViewMatrix * vec4( vert, 1. );
	vN = normalize( gl_NormalMatrix * norm );	// normal vector
	vL = LightPosition - ECposition.xyz;		// vector from the point
							// to the light position
	vE = vec3( 0., 0., 0. ) - ECposition.xyz;		// vector from the point
							// to the eye position 
	gl_Position = gl_ModelViewProjectionMatrix * vec4( vert, 1. );
}
