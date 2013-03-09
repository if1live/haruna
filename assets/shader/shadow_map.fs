precision mediump float;

uniform sampler2D s_shadow;
uniform vec4 u_lightColor;

varying vec4 v_shadowCoord;
varying float v_diffuse;

void main() 
{
	vec3 rgb = vec3(v_diffuse, v_diffuse, v_diffuse);
	rgb = clamp(rgb, 0.0, 1.0) * u_lightColor;
	
	vec4 shadowCoord = v_shadowCoord / v_shadowCoord.w;
	shadowCoord = shadowCoord * 0.5 + 0.5;
	float currentDepth = shadowCoord.z;
	
	vec2 uv = shadowCoord.xy;
	//uv.y = -uv.y;
	//uv = uv * 0.5 + 0.5;
	
	// Used to lower moiré pattern and self-shadowing
	//currentDepth += 0.0005;
	
	float shadowDepth = texture2D(s_shadow, uv).z;
	
	float shadow = 1.0;
	if (shadowCoord.w > 0.0) {
		shadow = shadowDepth < currentDepth ? 0.5 : 1.0;
	}
	
	gl_FragColor = vec4(rgb * shadow, 1.0);
	//gl_FragColor = vec4(v, v, v, 1.0);	
}