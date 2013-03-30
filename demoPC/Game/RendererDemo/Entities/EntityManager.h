#ifndef ILL_ENTITY_MANAGER_H_
#define ILL_ENTITY_MANAGER_H_

#include "EntityBase.h"

//TODO: I plan on adding this to illEngine itself in a bit
namespace Demo {
namespace Renderer {

class EntityManager {
public:
    EntityManager();
    virtual ~EntityManager();

    void update(float second);

private:
    void addEntity(EntityBase * entity);
    void removeEntity(EntityBase * entity);

    void activateEntity(EntityBase * entity);
    void deactivateEntity(EntityBase * entity);

    friend class EntityBase;
};

}
}

#endif