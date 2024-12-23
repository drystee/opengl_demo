#version 330 core
out vec4 FragColor;

in vec3 FragPos;  // From vertex shader
in vec2 TexCoords;

uniform sampler2D texture1;    // Texture

uniform vec3 lightPos;         // Light position
uniform vec3 lightColor;       // Light color
uniform vec3 viewPos;          // Camera/view position

uniform float maxDistance;     // Maximum distance for lighting

void main()
{
    // Compute normal dynamically for cubes
    vec3 norm = normalize(cross(dFdx(FragPos), dFdy(FragPos)));

    // Compute light direction and distance
    vec3 lightDir = normalize(lightPos - FragPos);
    float distance = length(lightPos - FragPos);

    // Attenuation based on distance
    float attenuation = clamp(1.0 - (distance / maxDistance), 0.0, 1.0);

    // Lighting calculations
    vec3 viewDir = normalize(viewPos - FragPos);

    // Ambient component
    float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse component
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular component
    float specularStrength = 0.8;
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Combine all lighting components with attenuation
    vec3 lighting = (ambient + diffuse + specular) * vec3(texture(texture1, TexCoords)) * attenuation;

    FragColor = vec4(lighting, 1.0);
}
