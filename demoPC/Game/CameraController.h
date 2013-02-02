#ifndef __CAMERA_CONTROLLER_H__
#define __CAMERA_CONTROLLER_H__

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "illEngine/Util/Geometry/geomUtil.h"

#include "illEngine/Input/serial/InputContext.h"
#include "illEngine/Input/serial/Listeners/StateListener.h"
#include "illEngine/Input/serial/Listeners/StateSetListener.h"
#include "illEngine/Input/serial/Listeners/RangeListener.h"
#include "illEngine/Input/serial/InputBinding.h"
#include "illEngine/Pc/serial/sdlInputEnum.h"

#include "illEngine/Logging/logging.h"

namespace Demo {

struct CameraController {
    CameraController();
    ~CameraController() {}

    void update(double seconds);

    glm::vec3 m_eulerAngles;
    glm::mat4 m_transform;

    illInput::InputContext m_inputContext;

    float m_speed;
    float m_rollSpeed;

    bool m_forward;
    bool m_back;
    bool m_left;
    bool m_right;
    bool m_up;
    bool m_down;
    bool m_rollLeft;
    bool m_rollRight;
    bool m_sprint;

    bool m_lookMode;

    glm::mediump_float m_zoom;
        
private:
    struct HorzLook : public illInput::RangeListener {
        HorzLook() 
            : illInput::RangeListener()
        {}

        virtual ~HorzLook() {}

        void onChange(float value) {
            if(m_controller->m_lookMode) {
                m_controller->m_transform = glm::rotate(m_controller->m_transform, value, glm::vec3(0.0f, -1.0f, 0.0f));
            }
            else {                  //eueler mode
                m_controller->m_eulerAngles.y -= value;
            }
        }

        CameraController * m_controller;
    };

    struct VertLook : public illInput::RangeListener {
        VertLook()
            : illInput::RangeListener()
        {}

        virtual ~VertLook() {}

        void onChange(float value) {
            if(m_controller->m_lookMode) {
                m_controller->m_transform = glm::rotate(m_controller->m_transform, value, glm::vec3(-1.0f, 0.0f, 0.0f));
            }
            else {                  //eueler mode                
                m_controller->m_eulerAngles.x -= value;

                if(m_controller->m_eulerAngles.x > 90) {
                    m_controller->m_eulerAngles.x = 90;
                }

                if(m_controller->m_eulerAngles.x < -90) {
                    m_controller->m_eulerAngles.x = -90;
                }
            }
        }

        CameraController * m_controller;
    };
    
    struct LookMode : public illInput::StateListener {
        LookMode()
            : illInput::StateListener()
        {}

        virtual ~LookMode() {}

        void onRelease() {
            if(m_controller->m_lookMode) {      //switch from quaternion mode to eueler mode
                m_controller->m_lookMode = false;
                
                m_controller->m_eulerAngles = glm::eulerAngles(glm::toQuat(m_controller->m_transform));
            }
            else {                              //switch from eueler mode to quaternion mode
                m_controller->m_lookMode = true;
                
                glm::vec3 position = getTransformPosition(m_controller->m_transform);

                //Who the hell decided to make this function take them in this order!
                m_controller->m_transform = glm::yawPitchRoll(glm::radians(m_controller->m_eulerAngles.y), glm::radians(m_controller->m_eulerAngles.x), glm::radians(m_controller->m_eulerAngles.z));
                m_controller->m_transform = setTransformPosition(m_controller->m_transform, position);
            }
        }

        CameraController * m_controller;
    };
    
    struct ZoomIn : public illInput::StateListener {
        ZoomIn()
            : illInput::StateListener()
        {}

        virtual ~ZoomIn() {}

        void onRelease() {
            *m_zoom -= 0.05f;

            if(*m_zoom <= 0.0f) {
                *m_zoom = 0.05f;
            }
        }

        glm::mediump_float * m_zoom;
    };

    struct ZoomOut : public illInput::StateListener {
        ZoomOut()
            : illInput::StateListener()
        {}

        virtual ~ZoomOut() {}

        void onRelease() {
            *m_zoom += 0.05f;
        }

        glm::mediump_float * m_zoom;
    };

    struct ZoomDefault : public illInput::StateListener {
        ZoomDefault()
            : illInput::StateListener()
        {}

        virtual ~ZoomDefault() {}

        void onRelease() {
            *m_zoom = 1.0f;
        }

        glm::mediump_float * m_zoom;
    };
    
    HorzLook m_horzLookListener;
    VertLook m_vertLookListener;

    illInput::StateSetListener m_forwardListener;
    illInput::StateSetListener m_backListener;
    illInput::StateSetListener m_leftListener;
    illInput::StateSetListener m_rightListener;
    illInput::StateSetListener m_upListener;
    illInput::StateSetListener m_downListener;
    illInput::StateSetListener m_rollLeftListener;
    illInput::StateSetListener m_rollRightListener;
    illInput::StateSetListener m_sprintListener;

    LookMode m_lookModeListener;

    ZoomIn m_zoomInListener;
    ZoomOut m_zoomOutListener;
    ZoomDefault m_zoomDefaultListener;
};

}

#endif
