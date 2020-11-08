#pragma once
#ifndef RAY_H
#define RAY_H

#include <algorithm>
#include <functional>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


namespace RayTracing {
	static bool RAY_TRACING_SPEED_MODE = false;		// �Ƿ�������׷�ټ����㷨

	static const float FLOAT_INF = 1e8;
	static const float FLOAT_EPS = 1e-5;

	/********************
	 * [�ṹ] ���ʽṹ
	 ********************/
	struct Material {
		std::function<glm::vec3(const glm::vec3& pos)> ambient;		// ������
		std::function<glm::vec3(const glm::vec3& pos)> diffuse;		// ������
		std::function<glm::vec3(const glm::vec3& pos)> specular;	// �����
		std::function<float(const glm::vec3& pos)> shininess;		// �����

		float kShade;			// ��ɫ��
		float kReflect;			// ������
		float kRefract;			// ������
		float refractiveIndex;	// ������
	};

	/********************
	* [��] ��Դ��
	********************/
	class Light {
	public:
		// - [�麯��] �����ǿ���� -
		virtual glm::vec3 calLight(const Material& material, const glm::vec3& fragPos, const glm::vec3& norm, const glm::vec3& viewDir) const = 0;
	};

	class DirLight : public Light {
	public:
		// - [����] �������� -
		glm::vec3 direction;     
		// - [����] ������ǿ�� -
		glm::vec3 ambient;
		// - [����] ������ǿ�� -
		glm::vec3 diffuse;
		// - [����] �����ǿ��
		glm::vec3 specular;      
		// - [����] ���캯�� -
		DirLight(glm::vec3 specular, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 direction) :
			direction(direction), ambient(ambient), diffuse(diffuse), specular(specular) { }
		// - [����] �����ǿ���� -
		glm::vec3 calLight(const Material& material, const glm::vec3& fragPos, const glm::vec3& norm, const glm::vec3& viewDir) const {
			// ��������ɫ
			glm::vec3 ambient = this->ambient * material.ambient(fragPos);
			// ��������ɫ
			glm::vec3 lightDir = glm::normalize(-direction);
			float diff = std::max(dot(norm, lightDir), 0.0f);
			glm::vec3 diffuse = this->diffuse * diff * material.diffuse(fragPos);
			// �������ɫ
			glm::vec3 reflectDir = glm::reflect(-lightDir, norm);
			float spec = glm::pow(std::max(glm::dot(viewDir, reflectDir), 0.0f), material.shininess(fragPos));
			glm::vec3 specular = this->specular * spec * material.specular(fragPos);
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
		Ray(glm::vec3 src, glm::vec3 dest) : origin(src), direction(glm::normalize(dest - src)) { }
		// - [����] ��ȡ�����ϲ���Ϊ t �ĵ� -
		inline glm::vec3 getPoint(float t) const {
			return origin + t * direction;
		}
	};

	/********************
	* [��] ʵ����
	********************/
	enum class EntityType {
		Plane = 0,		// ƽ��
		Triangle = 0,	// ������
		Sphere =0,		// ����
	};
	class Entity {
	public:
		// - [����] ���� -
		Material material;
		// - [�麯��] ��ȡʵ������ -
		virtual EntityType getEntityType() const = 0;
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
		// - [����] ��ȡʵ������ -
		EntityType getEntityType() const {
			return EntityType::Plane;
		}
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
		// - [����] ��ȡʵ������ -
		EntityType getEntityType() const {
			return EntityType::Triangle;
		}
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
		// - [����] ��ȡʵ������ -
		EntityType getEntityType() const {
			return EntityType::Sphere;
		}
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
	 * + �˲���
	 ********************/
	class Scene {
	public:
		// - [����] �㷨���� -
		// ����׷�����ݹ����
		static const unsigned int MAX_RECURSION_TIME = 3;
		// �˲���������
		static const int TREE_MAX_DEPTH = 8;			
		// ��Ч�ռ�߽� = 2^TREE_MAX_DEPTH
		static const int SPACE_BORDER = 256;
		// �ռ����걶�� = 100
		static const int SPACE_TIMES = 100;
		// - [����] ����˲������� -
		std::string calTreeCode(glm::vec3& p) {
			char code[TREE_MAX_DEPTH];
			int x = int(p.x * SPACE_TIMES) + (SPACE_BORDER >> 1), y = int(p.y * SPACE_TIMES) + (SPACE_BORDER >> 1), z = int(p.z * SPACE_TIMES) + (SPACE_BORDER >> 1);
			if (x < 0 || x >= SPACE_BORDER || y < 0 || y >= SPACE_BORDER || z < 0 || z >= SPACE_BORDER)
				return std::string();
			for (int i = TREE_MAX_DEPTH - 1; i >= 0; i--) {
				code[i] = (x & 1) + ((y & 1) << 1) + ((z & 1) << 2);
				x >>= 1; y >>= 1; z >>= 1;
			}
			return std::string(code);
		}
		// - [����] �鲢�˲������� -
		std::string mergeTreeCode(std::string& code1, std::string& code2) {
			unsigned int i = 0, l1 = code1.length(), l2 = code2.length();
			while (i < l1 && i < l2 && code1[i] == code2[i])
				i++;
			return code1.substr(0, i);
		}
		// - [����] ��ӹ�Դ���� -
		void addLight(Light* light) {
			lightList.push_back(light);
		}
		// - [����] ���ʵ�庯�� -
		bool addEntity(Entity* entity) {
			if (RAY_TRACING_SPEED_MODE) {
				if (entity->getEntityType() == EntityType::Plane) {
					// ƽ�� - �����б��м���
					entityList.push_back(entity);
					return true;
				}
				else if (entity->getEntityType() == EntityType::Triangle) {
					// ������ - ���ڰ˲�������
					Triangle* triangle = (Triangle *)(entity);
					std::string c0 = calTreeCode(triangle->vertice[0]);
					std::string c1 = calTreeCode(triangle->vertice[1]);
					std::string c2 = calTreeCode(triangle->vertice[2]);
					std::string s = mergeTreeCode(c0, c1);
					s = mergeTreeCode(s, c2);
					if (s.length() == 0)
						return false;
					// for (auto c : s)
					//   std::cout << char(c + '0');
					//std::cout << std::endl;
					entityMap.insert(std::pair<std::string, Entity*>(s, entity));
					return true;
				}
				else if (entity->getEntityType() == EntityType::Sphere) {
					// ƽ�� - �����б��м���
					entityList.push_back(entity);
					return true;
				}
			}
			else {
				entityList.push_back(entity);
				return true;
			}
		}
		// - [����] ��������볡���ཻ�� -
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
		// - [����] �����ཻ���ǿ -
		glm::vec3 calLight(const Entity& entity, glm::vec3 fragPos, const Ray& ray) {
			glm::vec3 result(0.0f);
			for (Light* light : lightList)
				result += light->calLight(entity.material, fragPos, entity.calNormal(fragPos), ray.direction);
			return result;
		}
		// - [����] ����׷�������� -
		glm::vec3 traceRay(const Ray& ray, unsigned int recursionTime = 0) {
			glm::vec3 lightIntensity(0.0f);
			if (recursionTime >= MAX_RECURSION_TIME)
				return lightIntensity;

			const auto& entityAndPoint = calIntersection(ray);
			const Entity* collidedEntity = entityAndPoint.first;
			if (!collidedEntity)
				return lightIntensity;

			glm::vec3 collidedPoint = entityAndPoint.second;
			glm::vec3 normal = glm::normalize(collidedEntity->calNormal(collidedPoint));
			bool isInEntity = collidedEntity->isRayInEntity(ray);
			if (isInEntity)
				normal = -normal;
			
			// - �ֲ�����ǿ�� -
			if (!isInEntity)
				lightIntensity = collidedEntity->material.kShade * calLight(*collidedEntity, collidedPoint, ray);
			
			// - �������ǿ�� -
			glm::vec3 reflectDirection = glm::reflect(ray.direction, normal);
			if (collidedEntity->material.kReflect > FLOAT_EPS)
				lightIntensity += collidedEntity->material.kReflect * traceRay(Ray(collidedPoint, collidedPoint + reflectDirection), recursionTime + 1);

			// - �������ǿ�� -
			float currentIndex = 1.0f;
			float nextIndex = collidedEntity->material.refractiveIndex;
			if (isInEntity)
				std::swap(currentIndex, nextIndex);
			glm::vec3 refractDirection = glm::refract(ray.direction, normal, currentIndex / nextIndex);
			if (collidedEntity->material.kRefract > FLOAT_EPS)
				lightIntensity += collidedEntity->material.kRefract * traceRay(Ray(collidedPoint, collidedPoint + refractDirection), recursionTime + 1);

			return lightIntensity;
		}
	private:
		std::vector<Light*> lightList;
		std::vector<Entity*> entityList;
		std::unordered_map<std::string, Entity*> entityMap;
	};
}

#endif
