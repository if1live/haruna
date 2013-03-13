precision mediump float;
varying vec2 v_texcoord;

uniform sampler2D s_tex;
uniform vec4 u_color;

void main() 
{
	float alpha = texture2D(s_tex, v_texcoord).a;
	vec4 color = u_color;
	color.a *= alpha;
	gl_FragColor = color;
}