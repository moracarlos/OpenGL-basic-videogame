#version 330
#extension GL_ARB_separate_shader_objects :enable

uniform sampler2D myTextureSampler;

in vec4 vVertexColor;
in vec2 UV;

layout(location = 0) out vec4 vFragColor;

void main(void)
{
	//vFragColor = vec4(UV,0.0,1.0);
	vFragColor = texture(myTextureSampler, UV);
}