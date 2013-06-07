#ifndef ILL_GRAPH_H_
#define ILL_GRAPH_H_

#include <list>
#include <string>
#include <fstream>
#include <chrono>

#include <glm/glm.hpp>

namespace illGraphics {
class Camera;
}

struct CrappyBmFontRenderer;

const float GRAPH_WIDTH = 500.0f;
const float GRAPH_HEIGHT = 50.0f;

/**
I may move an improved and well written version to illEngine itself.
This is a little debug drawable line graph that logs something like FPS.
*/
struct Graph {
    Graph()
        : m_maxData(500),
        m_numPoints(0),
        m_total(0.0f)
    {
        //m_startTime = m_clock.now;
    }

    inline void addDataPoint(float val) {
        m_numPoints++;
        m_total += val;
        
        m_dataPoints.push_back(val);

        if(m_dataPoints.size() > m_maxData) {
            m_dataPoints.pop_front();
        }

        if(m_outputFile.is_open()) {
            m_outputFile 
                << std::chrono::duration_cast<std::chrono::milliseconds>(m_clock.now().time_since_epoch()).count()
                << '\t' << val << std::endl;        //tab separated makes it easy to paste into excel
        }
    }

    /**
    Renders 
    */
    void render(const glm::mat4& transform, illGraphics::Camera& camera);
    
    CrappyBmFontRenderer * m_fontRenderer;

    //for computing average
    float m_total;
    unsigned int m_numPoints;

    std::string m_name;
    size_t m_maxData;
    std::list<float> m_dataPoints;

    ///The opened file to log data to
    std::chrono::high_resolution_clock m_clock;
    //std::chrono::high_resolution_clock::time_point m_startTime;
    std::ofstream m_outputFile;
};

#endif