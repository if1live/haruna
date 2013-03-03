precision mediump float;
varying vec3 v_diffuse;
varying vec3 v_reflection;
varying vec3 v_viewDir;

void main() {
	vec3 diffuse = clamp(v_diffuse, 0.0, 1.0);
	
	vec3 reflection = normalize(v_reflection);
	vec3 viewDir = normalize(v_viewDir);
	vec3 specular = vec3(0.0, 0.0, 0.0);
	if(diffuse.x > 0.0) {
		float specularValue = 0;
		specularValue = clamp(dot(reflection, -viewDir), 0.0, 1.0);
		specularValue = pow(specularValue, 20.0);
		
		specular = vec3(specularValue);
	}
	vec3 ambient = vec3(0.1, 0.1, 0.1);
	
	gl_FragColor = vec4(ambient + diffuse + specular, 1.0);
}