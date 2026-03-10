#version 330 core
in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightDir;
uniform vec3 viewPos;
uniform vec3 objectColor;

out vec4 FragColor;

void main() {
    vec3 norm = normalize(Normal);
    vec3 ld   = normalize(-lightDir);

    // two-sided lighting
    if (dot(norm, ld) < 0.0)
        norm = -norm;

    // ambient
    vec3 ambient = 0.15 * vec3(1.0);

    // diffuse
    float diff = max(dot(norm, ld), 0.0);
    vec3 diffuse = diff * vec3(1.0);

    // specular (Blinn-Phong)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 half_   = normalize(ld + viewDir);
    float spec   = pow(max(dot(norm, half_), 0.0), 64.0);
    vec3 specular = 0.4 * spec * vec3(1.0);

    FragColor = vec4((ambient + diffuse + specular) * objectColor, 1.0);
}
