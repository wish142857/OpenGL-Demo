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
	 * [�ṹ] ���ʽṹ
	 ********************/
	struct Material {
		glm::vec3 ambient;	// ������
		glm::vec3 diffuse;	// ������
		glm::vec3 specular;	// �����
		float shininess;	// �����

		float kShade;
		float kReflect;
		float kRefract;
		float refractiveIndex;
	};

	/********************
	* [�ṹ] ƽ�й�ṹ
	********************/
	struct DirLight {
		glm::vec3 direction;     // ��������

		glm::vec3 ambient;       // ������ǿ��
		glm::vec3 diffuse;       // ������ǿ��
		glm::vec3 specular;      // �����ǿ��

		// - [����] ���캯�� -
		DirLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) :
			direction(direction), ambient(ambient), diffuse(diffuse), specular(specular) { }
		// - [����] �����ǿ���� -
		glm::vec3 calLight(const Material& material, const glm::vec3& fragPos, const glm::vec3& norm, const glm::vec3& viewDir) const {
			// ��������ɫ
			glm::vec3 ambient = this->ambient * material.ambient;
			// ��������ɫ
			glm::vec3 lightDir = glm::normalize(-direction);
			float diff = std::max(dot(norm, lightDir), 0.0f);
			glm::vec3 diffuse = this->diffuse * diff * material.diffuse;
			// �������ɫ
			glm::vec3 middle = glm::normalize(-viewDir + lightDir);
			float spec = glm::pow(std::max(glm::dot(middle, norm), 0.0f), material.shininess);
			glm::vec3 specular = this->specular * spec * material.specular;
			// �ϲ����
			return (ambient + diffuse + specular);
		}
	};

	/********************
	 * [��] ������
	 ********************/
	class Ray {
	public:
		// - [����] ����ԭ�� -
		glm::vec3 origin;
		// - [����] ���߷��� -
		glm::vec3 direction;
		// - [����] ���캯�� -
		Ray(glm::vec3 origin, glm::vec3 direction) : origin(origin), direction(direction) { }
		// - [����] ��ȡ�����ϲ���Ϊ t �ĵ� -
		inline glm::vec3 getPoint(float t) const {
			return origin + t * direction;
		}
	};

	/********************
	* [��] ʵ����
	********************/
	class Entity {
	public:
		// - [����] ���� -
		Material material;
		// - [�麯��] �жϵ��Ƿ���ʵ���� -
		virtual bool isPointInEntity(const glm::vec3& p) const = 0;
		// - [�麯��] �жϹ���Դ�Ƿ���ʵ���ڲ� -
		virtual bool isRayInEntity(const Ray& ray) const = 0;
		// - [�麯��] ������㷨���� -
		virtual glm::vec3 calNormal(const glm::vec3& p) const = 0;
		// - [�麯��] �������߽��� -
		virtual float calRayCollision(const Ray& ray) const = 0;
	};

	/********************
	* [��] ƽ���ࣨʵ���ࣩ
	********************/
	class Plane : public Entity {
	public:
		// - [����] ƽ����ĳ�� -
		glm::vec3 point;
		// - [����] ƽ�淨���� -
		glm::vec3 normal;
		// - [����] ���캯�� -
		Plane(const glm::vec3& point, const glm::vec3& normal) : point(point), normal(normal) { }
		// - [����] �жϵ��Ƿ���ƽ���� -
		bool isPointInEntity(const glm::vec3& p) const {
			return glm::dot(p - point, normal) == 0;
		}
		// - [����] �жϹ���Դ�Ƿ���ʵ���ڲ� -
		bool isRayInEntity(const Ray& ray) const {
			return false;
		}
		//  - [����] ������㷨���� -
		glm::vec3 calNormal(const glm::vec3& p) const {
			return normal;
		}
		// - [����] �������߽��� -
		float calRayCollision(const Ray& ray) const {
			float v1 = glm::dot(ray.origin - point, normal);
			float v2 = glm::dot(normal, ray.direction);
			return abs(v2) < FLOAT_EPS ? -1 : -v1 / v2;
		}
	};

	/********************
	* [��] �������ࣨʵ���ࣩ
	********************/
	class Triangle : public Entity
	{
	public:
		// - [����] ���������� -
		glm::vec3 vertice[3];
		// - [����] ���Ƿ����� -
		glm::vec3 normal;
		// - [����] ��������ƽ��
		Plane plane;
		// - [����] ���캯�� -
		Triangle(const glm::vec3& A, const glm::vec3& B, const glm::vec3& C) 
			: vertice{ A, B, C }, normal(glm::normalize(glm::cross(B - A, C - A))), plane(vertice[0], normal) { }
		// - [����] �жϵ��Ƿ��������� -
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
		// - [����] �жϹ���Դ�Ƿ���ʵ���ڲ� -
		bool isRayInEntity(const Ray& ray) const {
			return false;
		}
		//  - [����] ������㷨���� -
		glm::vec3 calNormal(const glm::vec3& p) const {
			return normal;
		}
		// - [����] �������߽��� -
		float calRayCollision(const Ray& ray) const {
			float t = plane.calRayCollision(ray);
			if (t < -FLOAT_EPS)
				return -1;
			return isPointInEntity(ray.getPoint(t)) ? t : -1;
		}
	};

	/********************
	* [��] �����ࣨʵ���ࣩ
	********************/
	class Sphere : public Entity {
	public:
		// - [����] �����ĵ� -
		glm::vec3 center;
		// - [����] ��뾶 -
		float radius;
		// - [����] ���캯�� -
		Sphere(const glm::vec3& center, float radius) : center(center), radius(radius) { }
		// - [����] �жϵ��Ƿ��������� -
		bool isPointInEntity(const glm::vec3& p) const {
			return glm::distance(p, center) < radius + FLOAT_EPS;
		}
		// - [����] �жϹ���Դ�Ƿ���ʵ���ڲ� -
		bool isRayInEntity(const Ray& ray) const {
			return isPointInEntity(ray.origin) && calRayCollision(ray) > FLOAT_EPS;
		}
		//  - [����] ������㷨���� -
		glm::vec3 calNormal(const glm::vec3& p) const {
			return glm::normalize(p - center);
		}
		// - [����] �������߽��� -
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
	 * [��] ������
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