#pragma once
#ifndef RAY_H
#define RAY_H
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace RayTracing {
	static const float FLOAT_INF = 100000000.0f;
	static const float FLOAT_EPS = 1e-5;
	static const glm::vec3 NULL_POINT(FLOAT_INF, FLOAT_INF, FLOAT_INF);

	/********************
	 * [结构] 材质结构
	 ********************/
	struct Material {
		glm::vec3 ambient;	// 环境光
		glm::vec3 diffuse;	// 漫反射
		glm::vec3 specular;	// 镜面光
		float shininess;	// 反光度

		float kShade;
		float kReflect;
		float kRefract;
		float refractiveIndex;
	};

	/********************
	* [结构] 平行光结构
	********************/
	struct DirLight {
		glm::vec3 direction;     // 方向向量

		glm::vec3 ambient;       // 环境光强度
		glm::vec3 diffuse;       // 漫反射强度
		glm::vec3 specular;      // 镜面光强度

		// - [函数] 构造函数 -
		DirLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) :
			direction(direction), ambient(ambient), diffuse(diffuse), specular(specular) { }
		// - [函数] 计算光强函数 -
		glm::vec3 calLight(const Material& material, const glm::vec3& fragPos, const glm::vec3& norm, const glm::vec3& viewDir) const {
			// 环境光着色
			glm::vec3 ambient = this->ambient * material.ambient;
			// 漫反射着色
			glm::vec3 lightDir = glm::normalize(-direction);
			float diff = std::max(dot(norm, lightDir), 0.0f);
			glm::vec3 diffuse = this->diffuse * diff * material.diffuse;
			// 镜面光着色
			glm::vec3 middle = glm::normalize(-viewDir + lightDir);
			float spec = glm::pow(std::max(glm::dot(middle, norm), 0.0f), material.shininess);
			glm::vec3 specular = this->specular * spec * material.specular;
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
		Ray(glm::vec3 origin, glm::vec3 direction) : origin(origin), direction(direction) { }
		// - [函数] 获取射线上参数为 t 的点 -
		inline glm::vec3 getPoint(float t) const {
			return origin + t * direction;
		}
	};

	/********************
	* [类] 实体类
	********************/
	class Entity {
	public:
		// - [变量] 材质 -
		Material material;
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
		// - [函数] 判断点是否在平面上 -
		bool isPointInEntity(const glm::vec3& p) const {
			return glm::dot(p - point, normal) == 0;
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
		// - [函数] 判断点是否在三角上 -
		bool isPointInEntity(const glm::vec3& p) const {
			if (!plane.isPointInEntity(p))
				return false;
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
	 ********************/
	/*
	class Scene {
	public:
		Scene();
		~Scene();
		void addEntity(Entity* entity);
		void addLight(Light* light);
		glm::vec3 traceRay(const Ray& ray, unsigned int recursionTime = 0);
		std::pair<const glm::vec3&, const Entity*> getIntersection(const Ray& ray);
		glm::vec3 shade(const Entity& entity, glm::vec3 fragPos, const Ray& ray);

		static const unsigned int MAX_RECURSION_TIME;
	private:
		std::vector<Entity*> _entitys;
		std::vector<Light*> _lights;
	};
	*/
}

#endif