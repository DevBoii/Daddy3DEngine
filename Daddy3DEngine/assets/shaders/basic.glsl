#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;       // Matches Vertex::Position (3 floats)
layout (location = 1) in vec3 aNormal;    // Matches Vertex::Normal (3 floats)
layout (location = 2) in vec2 aTexCoords; // Matches Vertex::TexCoords (2 floats)
layout (location = 3) in vec3 aColor;

out vec3 vColor;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;

void main()
{
	vColor = aColor;
	gl_Position = u_ViewProjection * u_Model * vec4(aPos, 1.0f);
}

#shader fragment
#version 330 core

in vec3 vColor;

out vec4 FragColor;

void main()
{
	FragColor = vec4(vColor, 1.0f);
}