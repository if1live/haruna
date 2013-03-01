precision mediump float;
varying vec2 v_texcoord;
uniform sampler2D s_tex;

void main() {
	gl_FragColor = vec4(v_texcoord, 1.0, 0.0);//texture2D(s_tex, v_texcoord);
}