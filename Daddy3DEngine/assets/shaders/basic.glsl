#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;       // Matches Vertex::Position (3 floats)
layout (location = 1) in vec3 aNormal;    // Matches Vertex::Normal (3 floats)
layout (location = 2) in vec2 aTexCoords; // Matches Vertex::TexCoords (2 floats)
layout (location = 3) in vec3 aColor;	  // Matches Vertex::Color (3 floats)

out vec3 vColor;
out vec3 vNormal;
out vec3 vFragPos;
out vec2 vTexCoords;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

void main()
{
	vColor = aColor;
	vNormal = mat3(transpose(inverse(u_Model))) * aNormal;

	vec4 worldPos = u_Model * vec4(aPos, 1.0f);
	vFragPos = worldPos.xyz;

	vTexCoords = aTexCoords;

	gl_Position = u_ViewProjection * worldPos;
}

#shader fragment
#version 330 core

in vec3 vColor;
in vec3 vNormal;
in vec3 vFragPos;
in vec2 vTexCoords;

out vec4 FragColor;

// Material Uniforms (Matches Material::Bind)
struct Material 
{
    vec3 AlbedoColor;

    int UseAlbedoMap;                      // 1 = True, 0 = False
    sampler2D AlbedoMap;

	int UseSpecularMap;
    sampler2D SpecularMap;

	float SpecularIntensity;
	float Shininess;
};

uniform Material u_Material;

uniform vec3 u_LightColor;
uniform float u_AmbientStrength;
uniform float u_Intensity;

uniform vec3 u_LightDirection;

void main()
{
	vec3 albedo;
	if(u_Material.UseAlbedoMap == 1)
	{
		albedo = texture(u_Material.AlbedoMap, vTexCoords).rgb * u_Material.AlbedoColor;	}
	else
	{
		albedo = vColor * u_Material.AlbedoColor;
	}

	//  Ambient Calculation (Base background light)
	vec3 ambient = u_AmbientStrength * u_LightColor;

	// Diffuse
	vec3 normal = normalize(vNormal);
	vec3 lightDirection = normalize(-u_LightDirection);
	float diffFactor = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = u_LightColor * diffFactor * u_Intensity;

	vec3 finalResult = (ambient + diffuse) * albedo;
	FragColor = vec4(finalResult, 1.0f);
}