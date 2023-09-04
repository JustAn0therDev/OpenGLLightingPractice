#version 330 core

struct Material {
   sampler2D diffuse;
   sampler2D specular;
   sampler2D emission;
   float shininess;
};

uniform Material material;

struct Light {
    vec3 direction;
    vec3 position;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float quadratic;
    float linear;
};

uniform Light light;

uniform float ambientLighting;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float time;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

vec4 calculateLighting()
{
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;  
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;  
	
	vec3 emission = texture(material.emission, TexCoords - vec2(0.0, time * 3.0)).rgb;
    
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Comment to make spotlight work
    //ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    float theta     = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);    

    diffuse *= intensity;
    specular *= intensity;

    vec3 result = ambient + diffuse + specular + emission;
    
    return vec4(result, 1.0);
}

void main()
{
    // Uncomment to see spotlight
    vec3 lightDir = normalize(light.position - FragPos);

    float theta = dot(lightDir, normalize(-light.direction));
    
    if(theta > light.cutOff)
    {       
      FragColor = calculateLighting();
    }
    else  // else, use ambient light so scene isn't completely dark outside the spotlight.
      FragColor = vec4(light.ambient * vec3(texture(material.diffuse, TexCoords)), 1.0);

}
