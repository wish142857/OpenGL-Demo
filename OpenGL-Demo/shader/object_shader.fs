#version 330 core
// - ������� -
out vec4 FragColor;     // Ƭ����ɫ

// - ���ʽṹ -
struct Material {
	vec3 ambient;	    // ������
	vec3 diffuse;	    // ������
	vec3 specular;	    // �����
    float shininess;    // �����

    bool useMap;           // �Ƿ�ʹ����ͼ
    sampler2D diffuseMap;  // ��������ͼ
    sampler2D specularMap; // �������ͼ
}; 

// - ƽ�й�ṹ -
struct DirLight {
    vec3 direction;     // ��������
	
    vec3 ambient;       // ������ǿ��
    vec3 diffuse;       // ������ǿ��
    vec3 specular;      // �����ǿ��
};

// - ���Դ�ṹ -
struct PointLight {
    vec3 position;      // ��������
    
    float constant;     // ˥��ϵ��������
    float linear;       // ˥��ϵ��һ����
    float quadratic;    // ˥��ϵ��������
	
    vec3 ambient;       // ������ǿ��
    vec3 diffuse;       // ������ǿ��
    vec3 specular;      // �����ǿ��
};

// - �۹�Դ�ṹ -
struct SpotLight {
    vec3 position;      // ��������
    vec3 direction;     // ��������
    float cutOff;       // �й��
    float outerCutOff;  // ���й��
  
    float constant;     // ˥��ϵ��������
    float linear;       // ˥��ϵ��һ����
    float quadratic;    // ˥��ϵ��������
  
    vec3 ambient;       // ������ǿ��
    vec3 diffuse;       // ������ǿ��
    vec3 specular;      // �����ǿ��
};

#define NR_POINT_LIGHTS 4

// - ������� -
in vec3 FragPos;        // Ƭ����������
in vec3 Normal;         // Ƭ�η�����
in vec2 TexCoords;      // ��������

// - Uniform ���� -
uniform vec3 viewPos;                               // �۲��ӽ��������꣨����������꣩
uniform Material material;                          // ����
uniform DirLight dirLight;                          // ƽ�й�
uniform PointLight pointLights[NR_POINT_LIGHTS];    // ���Դ
uniform SpotLight spotLight;                        // �۹�Դ

uniform sampler2D texture_diffuse1;

// - �������� -
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);                   // ����ƽ�й��ǿ����
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir); // ������Դ��ǿ����
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);   // ����۹�Դ��ǿ����

// - ������ -
void main()
{
    // ��λ������
    vec3 norm = normalize(Normal);                  // ��λ������
    vec3 viewDir = normalize(viewPos - FragPos);    // ��λ��������
    // ����ƽ�й��ǿ
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // ������Դ��ǿ
    // for(int i = 0; i < NR_POINT_LIGHTS; i++)
    //    result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
    // ����۹�Դ��ǿ
    // result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
    FragColor = vec4(result, 1.0);
    // FragColor = texture(texture_diffuse1, TexCoords);
}

// - ����ƽ�й��ǿ���� -
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // ��������ɫ
    float diff = max(dot(normal, lightDir), 0.0);
    // �������ɫ
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // �ϲ����
    vec3 ambient = light.ambient * (material.useMap ? vec3(texture(material.diffuseMap, TexCoords)) : material.ambient);
    vec3 diffuse = light.diffuse * diff * (material.useMap ? vec3(texture(material.diffuseMap, TexCoords)) : material.diffuse);
    vec3 specular = light.specular * spec * (material.useMap ? vec3(texture(material.specularMap, TexCoords)) : material.specular);
    return (ambient + diffuse + specular);
}

//  - ������Դ��ǿ���� -
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // ��������ɫ
    float diff = max(dot(normal, lightDir), 0.0);
    // �������ɫ
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // ����˥��
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // �ϲ����
    vec3 ambient = light.ambient * (material.useMap ? vec3(texture(material.diffuseMap, TexCoords)) : material.ambient);
    vec3 diffuse = light.diffuse * diff * (material.useMap ? vec3(texture(material.diffuseMap, TexCoords)) : material.diffuse);
    vec3 specular = light.specular * spec * (material.useMap ? vec3(texture(material.specularMap, TexCoords)) : material.specular);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

//  - ����۹�Դ��ǿ���� -
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // ��������ɫ
    float diff = max(dot(normal, lightDir), 0.0);
    // �������ɫ
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // ����˥��
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // ����۹�ǿ��
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // �ϲ����
    vec3 ambient = light.ambient * (material.useMap ? vec3(texture(material.diffuseMap, TexCoords)) : material.ambient);
    vec3 diffuse = light.diffuse * diff * (material.useMap ? vec3(texture(material.diffuseMap, TexCoords)) : material.diffuse);
    vec3 specular = light.specular * spec * (material.useMap ? vec3(texture(material.specularMap, TexCoords)) : material.specular);
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}