#ifdef GL_ES
precision highp float;
#endif
uniform vec3 diffuseColor;
uniform vec3 specular;
uniform vec3 ambient;
uniform float specularExponent;

uniform vec3 lightDirection;
uniform vec3 lightDiffuse;
uniform vec3 lightAmbient;
uniform vec3 lightSpecular;
uniform mat3 normalMatrix;

varying vec2 texCoord;
varying vec3 normal;
varying vec4 position;

void main(void) {
	vec3 normLightDirection = normalize(normalMatrix*lightDirection);
	vec3 normalizedNormal = normalize(normal);
    float diffuseContribution = max(dot(normalizedNormal, -normLightDirection), 0.0);
	vec3 halfVector = -normalize(normalize(position.xyz) + normLightDirection);
	float specularContribution = max(dot(normalizedNormal, halfVector), 0.0);
	vec3 ambientTerm = lightAmbient * ambient;
	vec3 diffuseTerm = lightDiffuse * diffuseContribution * diffuseColor;
	vec3 specularTerm = lightSpecular * pow(specularContribution, specularExponent) * specular;
	gl_FragColor = vec4(ambientTerm + diffuseTerm + specularTerm, 1.0);
}