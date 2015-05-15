#version 330
#extension GL_ARB_separate_shader_objects :enable

in vec3 vVertexColor;
in vec3 fPosition;
in vec3 fNormal;
in vec3 lightPosFrag;
in vec3 eyePosFrag;
in vec3 ldFrag;
in vec3 lsFrag;
in vec3 ambientFrag;
in vec3 diffuseObjectFrag;
in vec3 specularObjectFrag;
in vec3 ambientObjectFrag;

layout(location = 0) out vec4 vFragColor;

void main(void)
{
    //vec3 pl = lightPosFrag;
    vec3 pl = vec3(0.0, 100.0, 0.0); //Posicion de la luz
	//vec3 eye = eyePosFrag; //Posicion del ojo
	vec3 eye=vec3(0,0,0);
	vec3 kd, ks, ld, ls;
    kd = diffuseObjectFrag;
    //kd=vec3(0.0,1.0,0.0); //Constante diffuse
    ks=specularObjectFrag;
    //ks=vec3(0.0,1.0,1.0); //Constante specular
    ld = ldFrag;
    ls = lsFrag;
    //ls=vec3(0.0,0.0,1.0);
	float sh=40.0; //Shininness

	vec3 s = normalize(pl-fPosition);
	vec3 n = normalize(fNormal);
	float dotP1 = max (0.0, dot(s,n));
    //vec3 ambient = vec3(0.0,0.5,0.7);
    vec3 ambient = ambientObjectFrag;
	if (dotP1>0){
		vec3 diffuse = kd*ld*dotP1;
		vec3 r = reflect(-s, fNormal);
		vec3 v = normalize(eye-fPosition);
		float dotP2 = max (0.0, dot(r, v));
		dotP2 = pow(dotP2, sh);
		vec3 specular = ks*ls*dotP2;
		ambient +=diffuse+specular;
	}

	vFragColor = vec4(clamp(ambient, vec3(0), vec3(1)), 1.0);
}

