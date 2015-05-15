#version 330
#extension GL_ARB_separate_shader_objects : enable

uniform mat4 mProjection, mModelView;

layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec4 vColor;
layout(location = 2) in vec2 vTexture;
layout(location = 3) in vec4 vNormal;

out vec4 vVertexColor;
out vec2 UV;


void main()
{
	UV = vTexture;
	vVertexColor = vColor;
	gl_Position = mProjection * mModelView * vVertex;
}