#version 150 compatibility

in vec4 vColor;
in vec2 vST;
out vec4 fFragColor;
uniform bool teapot_fragmentos;
uniform float timeElapsed;
uniform float uFrequency, uDensity;

void main()
{
	if (teapot_fragmentos)
	{
		vec2 ctex = vST * uFrequency;
		if( fract( ctex.s ) >= uDensity && fract( ctex.t ) >= uDensity )
			discard;
		fFragColor = vColor;
	}
	else{
		fFragColor = vColor;
	}
   
}