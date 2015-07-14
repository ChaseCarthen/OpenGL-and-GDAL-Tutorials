varying vec3 color;

void main(void)
{
gl_FragColor = vec4(color.xyz, 1.0);
}
