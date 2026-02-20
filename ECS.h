// ECS.h
#pragma once
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <memory>
#include "VEInclude.h"
#include <cstdint>

namespace ve{

using EntityID = uint32_t;

class ECS;
extern ECS g_ecs;
extern EntityID g_player;
extern EntityID g_rifle;

struct Component {
    virtual ~Component() {}
};

struct TransformComponent : Component {
    glm::vec3 position;
};

struct RenderComponent : Component {
    VESceneNode* node = nullptr;
};

struct PickupComponent : Component {
    bool collected = false;
};

struct WeaponComponent : Component {
    bool equipped = false;
};

struct PlayerComponent : Component {
    int hp = 72;
    int maxHP = 100;
    int ammo = 0;
    int maxAmmo = 150;
    glm::vec3 size = glm::vec3(1.0f, 1.8f, 1.0f);
    bool hasWeapon = false;
    int keys = 0;
};

struct PotionComponent : Component {
    int healAmount = 15;
};

struct AmmoComponent : Component {
    int Ammo = 15;
};

/*struct ArmorComponent : Component {
    int Armor = 20;
};*/

struct KeyComponent : Component {
    bool pickedUp = false;
};

/*struct ModComponent : Component {
    bool mod = true;
};*/

enum class GameState {
    Playing,
    GameOver,
    Win
};

struct GUIComponent : Component {
    std::string message;
    float displayTime = 3.0f; // seconds
};

struct WeaponStateComponent : Component {
    bool isFiring = false;
    float fireTimer = 1.0f; 
    glm::vec3 originalOffset = glm::vec3(0.35f, -0.35f, 0.8f); 
    glm::vec3 kickbackOffset = glm::vec3(0.0f, 0.0f, -0.3f);  
};

struct EnemyComponent : Component {
    int hp = 100;

    float visionRange = 15.0f;
    float attackRange = 15.0f;

    float shootCooldown = 1.5f;
    float shootTimer = 0.0f;
};

struct HitboxComponent : Component {
    glm::vec3 size; // AABB
};

enum class BulletOwner {
    Player,
    Enemy
};

struct BulletComponent : Component {
    glm::vec3 direction;
    float speed = 40.0f;
    float traveled = 0.0f;
    float maxDistance = 50.0f;
    int damage = 20;
    BulletOwner owner;
};




class ECS {
public:
    EntityID createEntity() {
        return ++m_lastID;
    }

    template<typename T, typename... Args>
    T* addComponent(EntityID e, Args&&... args) {
        auto comp = std::make_unique<T>(std::forward<Args>(args)...);
        T* ptr = comp.get();
        m_components[e][typeid(T)] = std::move(comp);
        return ptr;
    }

    template<typename T>
    T* get(EntityID e) {
        auto it = m_components[e].find(typeid(T));
        if (it == m_components[e].end()) return nullptr;
        return static_cast<T*>(it->second.get());
    }

    template<typename T>
    bool has(EntityID e) {
        return get<T>(e) != nullptr;
    }

    std::vector<EntityID> entities() {
        std::vector<EntityID> out;
        for (auto& p : m_components) out.push_back(p.first);
        return out;
    }

    void deleteEntity(EntityID e) {
        m_components.erase(e);
    }

private:
    EntityID m_lastID = 0;
    std::unordered_map<
        EntityID,
        std::unordered_map<std::type_index, std::unique_ptr<Component>>
    > m_components;
};


glm::vec3 m_moveInput = glm::vec3(0.0f);

std::vector<EntityID> g_levelEntities;

}