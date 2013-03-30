#ifndef ILL_ENTITY_BASE_H_
#define ILL_ENTITY_BASE_H_

//TODO: I plan on adding this to illEngine soon
namespace Demo {
namespace Renderer {

class EntityManager;

class EntityBase {
public:
    EntityBase(EntityManager * entityManager, bool active);
    virtual ~EntityBase();

    void setActive(bool active);
    
    inline bool getActive() const {
        return m_active;
    }

private:
    EntityManager * m_manager;
    bool m_active;
};

}
}

#endif