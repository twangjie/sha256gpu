#version 300 es

precision highp float;


in float v_posx;
// in float v_task;

out float v_percent;


void main() {
	v_percent = (1.0 + v_posx) / 2.0;
	// v_percent = v_task;
    gl_Position = vec4(v_posx, 0., 0., 1.);
}
