	varying vec3 normal;
	varying vec4 position;
	
	void main()
	{	
		
		gl_Position = ftransform();
		gl_TexCoord[0] = gl_MultiTexCoord0;
		
		normal = gl_NormalMatrix * gl_Normal;
		position = gl_Vertex;
		
	} 
