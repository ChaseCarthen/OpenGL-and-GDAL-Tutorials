attribute vec3 v_position;
attribute vec3 v_color;
varying vec3 color;
uniform mat4 mvpMatrix;
void main(void)
{
    gl_Position = mvpMatrix * vec4(v_position, 1.0);
    color = v_color;
}
