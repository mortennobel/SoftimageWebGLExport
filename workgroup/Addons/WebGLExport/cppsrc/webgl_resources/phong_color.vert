attribute vec3 vertexPosition;
attribute vec3 vertexNormal;
attribute vec2 vertexTextureCoord;

varying vec2 texCoord;
varying vec3 normal;
varying vec4 position;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;

void main(void) {
    texCoord = vertexTextureCoord;
    normal = normalMatrix * vertexNormal;
	position = modelViewMatrix * vec4(vertexPosition, 1.0);
    gl_Position = projectionMatrix * position;

}