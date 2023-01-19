#version 330 compatibility

out vec4  vColor;
out float vLightIntensity; 
out vec2  vST;
out vec3  vXYZ;

void main( )
{
	vST  = gl_MultiTexCoord0.st;
	vXYZ = gl_Vertex.xyz;

    	vec3 tnorm = normalize( gl_NormalMatrix * gl_Normal );
	vec3 LightPos = vec3( 5., 10., 10. );
	vec3 ECposition = vec3( gl_ModelViewMatrix * gl_Vertex );
    	vLightIntensity  = abs( dot( normalize(LightPos - ECposition), tnorm ) );
	if( vLightIntensity < 0.2 )
		vLightIntensity = 0.2;
		
	vColor = gl_Color;
	if( gl_ProjectionMatrix[2][3] == 0. )
		vColor = vec4( 1., .5, 0., 1. );
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
