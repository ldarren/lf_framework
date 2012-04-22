	attribute vec3 tangent;
	
	
	varying vec3 lightVec;
	
	void main()
	{	
		gl_Position = ftransform();
		gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
		
		vec3 normal = gl_NormalMatrix * gl_Normal;
		vec3 binormal = cross(tangent, normal);

		mat3 tbnMatrix = mat3(tangent, binormal, normal);
		
		vec4 pos = gl_ModelViewMatrix * gl_Vertex;

		lightVec = gl_LightSource[0].position.xyz - pos.xyz;
		lightVec = tbnMatrix * lightVec;
		
		
	} 
