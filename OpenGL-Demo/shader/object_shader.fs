#version 330 core
// - 输出变量 -
out vec4 FragColor;     // 片段颜色

// - 材质结构 -
struct Material {
	vec3 ambient;	    // 环境光
	vec3 diffuse;	    // 漫反射
	vec3 specular;	    // 镜面光
    float shininess;    // 反光度

    bool useMap;           // 是否使用贴图
    sampler2D diffuseMap;  // 漫反射贴图
    sampler2D specularMap; // 镜面光贴图
}; 

// - 平行光结构 -
struct DirLight {
    vec3 direction;     // 方向向量
	
    vec3 ambient;       // 环境光强度
    vec3 diffuse;       // 漫反射强度
    vec3 specular;      // 镜面光强度
};

// - 点光源结构 -
struct PointLight {
    vec3 position;      // 世界坐标
    
    float constant;     // 衰减系数常数项
    float linear;       // 衰减系数一次项
    float quadratic;    // 衰减系数二次项
	
    vec3 ambient;       // 环境光强度
    vec3 diffuse;       // 漫反射强度
    vec3 specular;      // 镜面光强度
};

// - 聚光源结构 -
struct SpotLight {
    vec3 position;      // 世界坐标
    vec3 direction;     // 方向向量
    float cutOff;       // 切光角
    float outerCutOff;  // 外切光角
  
    float constant;     // 衰减系数常数项
    float linear;       // 衰减系数一次项
    float quadratic;    // 衰减系数二次项
  
    vec3 ambient;       // 环境光强度
    vec3 diffuse;       // 漫反射强度
    vec3 specular;      // 镜面光强度
};

#define NR_POINT_LIGHTS 4

// - 输入变量 -
in vec3 FragPos;        // 片段世界坐标
in vec3 Normal;         // 片段法向量
in vec2 TexCoords;      // 纹理坐标

// - Uniform 变量 -
uniform vec3 viewPos;                               // 观察视角世界坐标（相机世界坐标）
uniform Material material;                          // 材质
uniform DirLight dirLight;                          // 平行光
uniform PointLight pointLights[NR_POINT_LIGHTS];    // 点光源
uniform SpotLight spotLight;                        // 聚光源

uniform sampler2D texture_diffuse1;

// - 函数声明 -
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);                   // 计算平行光光强函数
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir); // 计算点光源光强函数
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);   // 计算聚光源光强函数

// - 主函数 -
void main()
{
    // 单位化向量
    vec3 norm = normalize(Normal);                  // 单位法向量
    vec3 viewDir = normalize(viewPos - FragPos);    // 单位视线向量
    // 计算平行光光强
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // 计算点光源光强
    // for(int i = 0; i < NR_POINT_LIGHTS; i++)
    //    result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
    // 计算聚光源光强
    // result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
    FragColor = vec4(result, 1.0);
    // FragColor = texture(texture_diffuse1, TexCoords);
}

// - 计算平行光光强函数 -
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // 合并结果
    vec3 ambient = light.ambient * (material.useMap ? vec3(texture(material.diffuseMap, TexCoords)) : material.ambient);
    vec3 diffuse = light.diffuse * diff * (material.useMap ? vec3(texture(material.diffuseMap, TexCoords)) : material.diffuse);
    vec3 specular = light.specular * spec * (material.useMap ? vec3(texture(material.specularMap, TexCoords)) : material.specular);
    return (ambient + diffuse + specular);
}

//  - 计算点光源光强函数 -
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // 计算衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // 合并结果
    vec3 ambient = light.ambient * (material.useMap ? vec3(texture(material.diffuseMap, TexCoords)) : material.ambient);
    vec3 diffuse = light.diffuse * diff * (material.useMap ? vec3(texture(material.diffuseMap, TexCoords)) : material.diffuse);
    vec3 specular = light.specular * spec * (material.useMap ? vec3(texture(material.specularMap, TexCoords)) : material.specular);
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

//  - 计算聚光源光强函数 -
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // 计算衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // 计算聚光强度
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // 合并结果
    vec3 ambient = light.ambient * (material.useMap ? vec3(texture(material.diffuseMap, TexCoords)) : material.ambient);
    vec3 diffuse = light.diffuse * diff * (material.useMap ? vec3(texture(material.diffuseMap, TexCoords)) : material.diffuse);
    vec3 specular = light.specular * spec * (material.useMap ? vec3(texture(material.specularMap, TexCoords)) : material.specular);
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}