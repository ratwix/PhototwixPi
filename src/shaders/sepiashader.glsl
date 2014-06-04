varying vec2 tcoord;
uniform sampler2D tex;

void main(void) 
{

	vec4 color = texture2D(tex,tcoord);
	float grey = (color[0] + color[1] + color[2]) / 3.;
	
	gl_FragColor = vec4(vec3(1.2, 1.0, 0.8) * grey, 1.0);

	 /*
	vec3 texel = texture2D(tex, tcoord).rgb;
	texel = vec3(
                  texture2D(tex, vec2(texel.r, .16666)).r,
                  texture2D(tex, vec2(texel.g, .5)).g,
                  texture2D(tex, vec2(texel.b, .83333)).b);
     
     gl_FragColor = vec4(texel, 1.0);
	*/
}
