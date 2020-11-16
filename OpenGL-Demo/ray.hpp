#pragma once
#ifndef RAY_H
#define RAY_H

#include <algorithm>
#include <functional>
#include <set>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace RayTracing {
	static bool RAY_TRACING_SPEED_MODE = true;		// 是否开启光线追踪加速算法

	static const float FLOAT_INF = 1e8f;
	static const float FLOAT_EPS = 1e-2f;

	/********************
	 * [结构] 材质结构
	 ********************/
	struct Material {
		std::function<glm::vec3(const glm::vec3& pos)> ambient;		// 环境光
		std::function<glm::vec3(const glm::vec3& pos)> diffuse;		// 漫反射
		std::function<glm::vec3(const glm::vec3& pos)> specular;	// 镜面光
		std::function<float(const glm::vec3& pos)> shininess;		// 反光度

		float kShade;			// 着色率
		float kReflect;			// 反射率
		float kRefract;			// 折射率
		float refractiveIndex;	// 屈光率
	};

	/********************
	* [类] 光源类
	********************/
	class Light {
	public:
		// - [虚函数] 计算光强函数 -
		virtual glm::vec3 calLight(const Material& material, const glm::vec3& fragPos, const glm::vec3& norm, const glm::vec3& viewDir) const = 0;
	};

	class DirLight : public Light {
	public:
		// - [变量] 方向向量 -
		glm::vec3 direction;     
		// - [变量] 环境光强度 -
		glm::vec3 ambient;
		// - [变量] 漫反射强度 -
		glm::vec3 diffuse;
		// - [变量] 镜面光强度
		glm::vec3 specular;      
		// - [函数] 构造函数 -
		DirLight(glm::vec3 specular, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 direction) :
			direction(direction), ambient(ambient), diffuse(diffuse), specular(specular) { }
		// - [函数] 计算光强函数 -
		glm::vec3 calLight(const Material& material, const glm::vec3& fragPos, const glm::vec3& norm, const glm::vec3& viewDir) const {
			// 环境光着色
			glm::vec3 ambient = this->ambient * material.ambient(fragPos);
			// 漫反射着色
			glm::vec3 lightDir = glm::normalize(-direction);
			float diff = std::max(dot(norm, lightDir), 0.0f);
			glm::vec3 diffuse = this->diffuse * diff * material.diffuse(fragPos);
			// 镜面光着色
			glm::vec3 reflectDir = glm::reflect(-lightDir, norm);
			float spec = glm::pow(std::max(glm::dot(viewDir, reflectDir), 0.0f), material.shininess(fragPos));
			glm::vec3 specular = this->specular * spec * material.specular(fragPos);
			// 合并结果
			return (ambient + diffuse + specular);
		}
	};

	/********************
	 * [类] 光线类
	 ********************/
	class Ray {
	public:
		// - [变量] 射线原点 -
		glm::vec3 origin;
		// - [变量] 射线方向 -
		glm::vec3 direction;
		// - [函数] 构造函数 -
		Ray(glm::vec3 src, glm::vec3 dest) : origin(src), direction(glm::normalize(dest - src)) { }
		// - [函数] 获取射线上参数为 t 的点 -
		inline glm::vec3 getPoint(float t) const {
			return origin + t * direction;
		}
	};

	/********************
	* [类] 实体类
	********************/
	enum class EntityType {
		Plane = 0,		// 平面
		Triangle = 1,	// 三角面
		Sphere =2,		// 球体
	};
	class Entity {
	public:
		// - [变量] 材质 -
		Material material;
		// - [虚函数] 获取实体类型 -
		virtual EntityType getEntityType() const = 0;
		// - [虚函数] 判断点是否在实体上 -
		virtual bool isPointInEntity(const glm::vec3& p) const = 0;
		// - [虚函数] 判断光线源是否在实体内部 -
		virtual bool isRayInEntity(const Ray& ray) const = 0;
		// - [虚函数] 计算过点法向量 -
		virtual glm::vec3 calNormal(const glm::vec3& p) const = 0;
		// - [虚函数] 计算射线交点 -
		virtual float calRayCollision(const Ray& ray) const = 0;
	};

	/********************
	* [类] 平面类（实体类）
	********************/
	class Plane : public Entity {
	public:
		// - [变量] 平面上某点 -
		glm::vec3 point;
		// - [变量] 平面法向量 -
		glm::vec3 normal;
		// - [函数] 构造函数 -
		Plane(const glm::vec3& point, const glm::vec3& normal) : point(point), normal(normal) { }
		// - [函数] 获取实体类型 -
		EntityType getEntityType() const {
			return EntityType::Plane;
		}
		// - [函数] 判断点是否在平面上 -
		bool isPointInEntity(const glm::vec3& p) const {
			return abs(glm::dot(p - point, normal)) < FLOAT_EPS;
		}
		// - [函数] 判断光线源是否在实体内部 -
		bool isRayInEntity(const Ray& ray) const {
			return false;
		}
		//  - [函数] 计算过点法向量 -
		glm::vec3 calNormal(const glm::vec3& p) const {
			return normal;
		}
		// - [函数] 计算射线交点 -
		float calRayCollision(const Ray& ray) const {
			float v1 = glm::dot(ray.origin - point, normal);
			float v2 = glm::dot(normal, ray.direction);
			return abs(v2) < FLOAT_EPS ? -1 : -v1 / v2;
		}
	};

	/********************
	* [类] 三角面类（实体类）
	********************/
	class Triangle : public Entity
	{
	public:
		// - [变量] 三角三顶点 -
		glm::vec3 vertice[3];
		// - [变量] 三角法向量 -
		glm::vec3 normal;
		// - [变量] 三角所在平面
		Plane plane;
		// - [函数] 构造函数 -
		Triangle(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C) 
			: vertice{ A, B, C }, normal(glm::normalize(glm::cross(B - A, C - A))), plane(vertice[0], normal) { }
		// - [函数] 获取实体类型 -
		EntityType getEntityType() const {
			return EntityType::Triangle;
		}
		// - [函数] 判断点是否在三角上 -
		bool isPointInEntity(const glm::vec3& p) const {
			//if (!plane.isPointInEntity(p))
			//	return false;
			glm::vec3 v2p[3];
			glm::vec3 c[3];
			for (int i = 0; i < 3; i++)
				v2p[i] = p - vertice[i];
			for (int i = 0; i < 3; i++)
				c[i] = glm::normalize(glm::cross(v2p[i], v2p[(i + 1) % 3]));
			return (glm::distance(c[0], c[1]) < FLOAT_EPS && glm::distance(c[1], c[2]) < FLOAT_EPS);
		}
		// - [函数] 判断光线源是否在实体内部 -
		bool isRayInEntity(const Ray& ray) const {
			return false;
		}
		//  - [函数] 计算过点法向量 -
		glm::vec3 calNormal(const glm::vec3& p) const {
			return normal;
		}
		// - [函数] 计算射线交点 -
		float calRayCollision(const Ray& ray) const {
			float t = plane.calRayCollision(ray);
			if (t < -FLOAT_EPS)
				return -1;
			return isPointInEntity(ray.getPoint(t)) ? t : -1;
		}
	};

	/********************
	* [类] 球体类（实体类）
	********************/
	class Sphere : public Entity {
	public:
		// - [变量] 球中心点 -
		glm::vec3 center;
		// - [变量] 球半径 -
		float radius;
		// - [函数] 构造函数 -
		Sphere(const glm::vec3& center, float radius) : center(center), radius(radius) { }
		// - [函数] 获取实体类型 -
		EntityType getEntityType() const {
			return EntityType::Sphere;
		}
		// - [函数] 判断点是否在球面内 -
		bool isPointInEntity(const glm::vec3& p) const {
			return glm::distance(p, center) < radius + FLOAT_EPS;
		}
		// - [函数] 判断光线源是否在实体内部 -
		bool isRayInEntity(const Ray& ray) const {
			return isPointInEntity(ray.origin) && calRayCollision(ray) > FLOAT_EPS;
		}
		//  - [函数] 计算过点法向量 -
		glm::vec3 calNormal(const glm::vec3& p) const {
			return glm::normalize(p - center);
		}
		// - [函数] 计算射线交点 -
		float calRayCollision(const Ray& ray) const {
			glm::vec3 c2o = ray.origin - center;
			float A = glm::dot(ray.direction, ray.direction);
			float B = 2 * glm::dot(c2o, ray.direction);
			float C = glm::dot(c2o, c2o) - radius * radius;
			if (abs(C) < FLOAT_EPS)
				C = 0;
			float delta = B * B - 4 * A * C;
			if (delta < FLOAT_EPS)
				return -1;
			delta = sqrt(delta);
			float t1 = (-B + delta) / 2 / A;
			float t2 = (-B - delta) / 2 / A;
			if (t1 < FLOAT_EPS && t2 < FLOAT_EPS)
				return -1;
			return t2 > FLOAT_EPS ? t2 : t1;
		}
	};

	/********************
	 * [类] 场景类
	 * + 八叉树
	 ********************/
	class Scene {
	public:
		// - [常数] 算法参数 -
		// 光线追踪最大递归次数
		static const unsigned int MAX_RECURSION_TIME = 2;
		// 八叉树最大深度
		static const int TREE_MAX_DEPTH = 10;
		// 八叉树有效深度
		static const int TREE_VALID_DEPTH = 5;
		// 有效空间边界 = 2^TREE_MAX_DEPTH
		static const int SPACE_BORDER = 1024;
		// 空间坐标倍数 = 100
		static const int SPACE_TIMES = 100;
		// - [函数] 析构函数 -
		~Scene() {
			for (Entity* entity : entityList)
				delete entity;
			for (const auto &p : entityMap)
				for (const auto& e : p.second)
					delete e;
		}
		// - [函数] 计算八叉树编码 -
		static std::string calTreeCode(glm::vec3& p) {
			char code[TREE_MAX_DEPTH + 1] = { 0 };
			int x = int(p.x * SPACE_TIMES) + (SPACE_BORDER >> 1), y = int(p.y * SPACE_TIMES) + (SPACE_BORDER >> 1), z = int(p.z * SPACE_TIMES) + (SPACE_BORDER >> 1);
			if (x < 0 || x >= SPACE_BORDER || y < 0 || y >= SPACE_BORDER || z < 0 || z >= SPACE_BORDER)
				return std::string();
			for (int i = TREE_MAX_DEPTH - 1; i >= 0; i--) {
				code[i] = (x & 1) + ((y & 1) << 1) + ((z & 1) << 2) + '0';
				x >>= 1; y >>= 1; z >>= 1;
			}
			return std::string(code).substr(0, TREE_VALID_DEPTH);
		}
		// - [函数] 归并八叉树编码 -
		std::string mergeTreeCode(std::string& code1, std::string& code2, std::string& code3) {
			size_t i = 0, l1 = code1.length(), l2 = code2.length(), l3 = code3.length();
			while (i < l1 && i < l2 && i < l3 && code1[i] == code2[i] && code1[i] == code3[i])
				i++;
			return code1.substr(0, i);
		}
		// - [函数] 添加光源函数 -
		void addLight(Light* light) {
			lightList.push_back(light);
		}
		// - [函数] 添加实体函数 -
		bool addEntity(Entity* entity) {
			if (RAY_TRACING_SPEED_MODE) {
				EntityType e = entity->getEntityType();
				if (entity->getEntityType() == EntityType::Plane) {
					// 平面 - 置于列表中即可
					entityList.push_back(entity);
					return true;
				}
				else if (entity->getEntityType() == EntityType::Triangle) {
					// 三角面 - 置于八叉树集中
					Triangle* triangle = (Triangle *)(entity);
					std::string c0 = calTreeCode(triangle->vertice[0]);
					std::string c1 = calTreeCode(triangle->vertice[1]);
					std::string c2 = calTreeCode(triangle->vertice[2]);
					std::string s = mergeTreeCode(c0, c1, c2);
					if (s.length() == 0)
						return false;
					entityMap[s].insert(entity);
					// std::cout << c0 << "--" << c1 << "--" << c2 << "--" << s << std::endl;
					return true;
				}
				else if (entity->getEntityType() == EntityType::Sphere) {
					// 平面 - 置于列表中即可
					entityList.push_back(entity);
					return true;
				}
			}
			else {
				entityList.push_back(entity);
				return true;
			}
			return false;
		}
		// - [函数] 计算光线与场景相交点 -
		std::pair<const Entity*, const glm::vec3&> calIntersection(const Ray& ray) {
			float minT = FLOAT_INF;
			const Entity* collidedEntity = nullptr;
			for (Entity* entity : entityList) {
				float t = entity->calRayCollision(ray);
				if (t > FLOAT_EPS && t < minT) {
					minT = t;
					collidedEntity = entity;
				}
			}
			return std::pair<const Entity*, const glm::vec3&>(collidedEntity, ray.getPoint(minT));
		}
		// - [函数] 计算相交点光强 -
		glm::vec3 calLight(const Entity& entity, glm::vec3 fragPos, const Ray& ray) {
			glm::vec3 result(0.0f);
			for (Light* light : lightList)
				result += light->calLight(entity.material, fragPos, entity.calNormal(fragPos), ray.direction);
			return result;
		}
		// - [函数] 光线追踪主函数 -
		glm::vec3 traceRay(const Ray& ray, unsigned int recursionTime = 0) {
			glm::vec3 lightIntensity(0.0f);
			if (recursionTime >= MAX_RECURSION_TIME)
				return lightIntensity;

			std::pair<const Entity*, const glm::vec3&> entityAndPoint = calIntersection(ray);

			const Entity* collidedEntity = entityAndPoint.first;
			if (!collidedEntity)
				return lightIntensity;

			glm::vec3 collidedPoint = entityAndPoint.second;
			glm::vec3 normal = glm::normalize(collidedEntity->calNormal(collidedPoint));
			bool isInEntity = collidedEntity->isRayInEntity(ray);
			if (isInEntity)
				normal = -normal;



			// - 局部光照强度 -
			if (!isInEntity)
				lightIntensity = collidedEntity->material.kShade * calLight(*collidedEntity, collidedPoint, ray);
			
			// - 反射光照强度 -
			glm::vec3 reflectDirection = glm::reflect(ray.direction, normal);
			if (collidedEntity->material.kReflect > FLOAT_EPS)
				lightIntensity += collidedEntity->material.kReflect * traceRay(Ray(collidedPoint, collidedPoint + reflectDirection), recursionTime + 1);

			// - 折射光照强度 -
			float currentIndex = 1.0f;
			float nextIndex = collidedEntity->material.refractiveIndex;
			if (isInEntity)
				std::swap(currentIndex, nextIndex);
			glm::vec3 refractDirection = glm::refract(ray.direction, normal, currentIndex / nextIndex);
			if (collidedEntity->material.kRefract > FLOAT_EPS)
				lightIntensity += collidedEntity->material.kRefract * traceRay(Ray(collidedPoint, collidedPoint + refractDirection), recursionTime + 1);

			//if (lightIntensity[0] < FLOAT_EPS)
			//	std::cout << "OK" << std::endl;


			return lightIntensity;
		}
	private:
		std::vector<Light*> lightList;
		std::vector<Entity*> entityList;
		std::unordered_map<std::string, std::set<Entity *>> entityMap;
	};
}

#endif
