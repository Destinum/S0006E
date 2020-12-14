#version 430

out vec4 color;

in vec4 VertexPosition;
in vec2 TextureCoordinates;
in vec4 TheNormals;

uniform sampler2D Texture;
uniform vec4 CameraPosition;

struct Light 
{
    vec4 Position;
    vec4 Ambient;
    vec4 Diffuse;
    vec4 Specular;
    float Intensity;
};

uniform Light LightSource;

void main()
{
    // ambient
    vec4 TheColor = texture(Texture, TextureCoordinates); 
    vec4 Ambient = LightSource.Ambient * TheColor;

    // diffuse
    vec3 LightDirection = normalize(LightSource.Position.xyz - VertexPosition.xyz);
    float diff = max(dot(TheNormals.xyz, LightDirection), 0.0);
    vec4 diffuse = LightSource.Diffuse * LightSource.Intensity * diff * TheColor;

    // specular
    vec3 ViewDirection = normalize(CameraPosition.xyz - VertexPosition.xyz);
    vec3 ReflectDirection = reflect(-LightDirection, TheNormals.xyz);
    float spec = pow(max(dot(ViewDirection, ReflectDirection), 0.0), 64.0);
    vec4 specular = LightSource.Specular * spec;
    
	color = Ambient + diffuse + specular;
};
