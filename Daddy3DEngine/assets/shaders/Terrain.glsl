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

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#shader fragment
#version 330 core

in vec3 vColor;
in vec3 vNormal;
in vec3 vFragPos;
in vec2 vTexCoords;

out vec4 FragColor;

uniform vec3 u_CameraPosition;

uniform sampler2D grassTexture;
uniform sampler2D dirtTexture;
uniform float u_Blend;

//Light Uniforms
struct Light
{
	vec3 u_LightColor;
	float u_AmbientStrength;
	float u_Intensity;
	vec3 u_LightDirection;
};
uniform Light u_Light;

struct Fog
{
	bool useFog;
	vec3 fogColor;
	float fogDensity;
};
uniform Fog u_Fog;

void main()
{

	vec3 finalColor;

	//-------------------------------------------------------------------------------------------------
	//------------------------------------------Material--------------------------------------------------
	//-------------------------------------------------------------------------------------------------
	vec3 albedo = mix(grassTexture, dirtTexture, u_Blend);

	//-------------------------------------------------------------------------------------------------
	//------------------------------------------Light--------------------------------------------------
	//-------------------------------------------------------------------------------------------------

	//  Ambient Calculation (Base background light)
	vec3 ambient = u_Light.u_AmbientStrength * u_Light.u_LightColor;

	// Diffuse
	vec3 normal = normalize(vNormal);
	vec3 lightDirection = normalize(-u_Light.u_LightDirection);
	float diffFactor = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = u_Light.u_LightColor * diffFactor * u_Light.u_Intensity;
	vec3 finalLightResult = (ambient + diffuse) * albedo;

	//-------------------------------------------------------------------------------------------------
	//------------------------------------------Fog--------------------------------------------------
	//-------------------------------------------------------------------------------------------------
	float distance = length(vFragPos - u_CameraPosition);
	float fogFactor = exp(-u_Fog.fogDensity * distance);
	float clampedFogFactor = clamp(fogFactor, 0.0f, 1.0f);
	vec3 finalColorWithFog = mix(u_Fog.fogColor, finalLightResult, clampedFogFactor);

	if(u_Fog.useFog)
	{
		finalColor = finalColorWithFog;
	}
	else
	{
		finalColor = finalLightResult;
	}

	FragColor = vec4(finalColor, 1.0f);
}