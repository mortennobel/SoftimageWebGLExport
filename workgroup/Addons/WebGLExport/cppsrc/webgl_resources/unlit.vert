attribute vec3 vertexPosition;
attribute vec3 vertexNormal;
attribute vec2 vertexTextureCoord;

varying vec2 texCoord;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

void main(void) {
    texCoord = vertexTextureCoord;
    gl_Position = projectionMatrix * modelViewMatrix * vec4(vertexPosition, 1.0);
}