#ifdef GL_ES
precision highp float;
#endif
uniform sampler2D diffuseTexture;
uniform vec3 diffuseColor;

varying vec2 texCoord;

void main(void) {
    gl_FragColor = texture2D(diffuseTexture, texCoord)*vec4(diffuseColor,1.0);
}