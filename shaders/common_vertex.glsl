#version 330 core
#pragma optimize(off)
#pragma debug(on)

layout(location = 1) in vec4 position;

void main(void)    {
	gl_Position = position ;
}
