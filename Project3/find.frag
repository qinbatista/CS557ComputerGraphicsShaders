#version 330 compatibility

uniform bool  uUseST;
uniform float uX, uY, uZ;
uniform float uDx, uDy, uDz;
uniform float uS, uT;
uniform float uDs, uDt;

in vec2  vST;
in vec3  vXYZ;
in vec4  vColor;
in float vLightIntensity;

const vec3 RED = vec3( 1., 0., 0. );

void main( )
{
	vec3 newColor = vColor.rgb;

	if( uUseST )
	{
		if( uS-uDs <= vST.s  &&  vST.s <= uS+uDs  &&   uT-uDt <= vST.t  &&  vST.t <= uT+uDt )
			newColor = RED;
	}
	else
	{
		if( uX-uDx <= vXYZ.x  &&  vXYZ.x <= uX+uDx  &&   uY-uDy <= vXYZ.y  &&  vXYZ.y <= uY+uDy  &&  uZ-uDz <= vXYZ.z  &&  vXYZ.z <= uZ+uDz )
			newColor = RED;
	}

	newColor *= vLightIntensity;
	gl_FragColor = vec4( newColor, 1. );
}
