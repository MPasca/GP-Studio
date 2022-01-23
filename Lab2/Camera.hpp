#ifndef Camera_hpp
#define Camera_hpp

#include "D:/Faculta/An 3/Sem 1/PG/glm/glm.hpp"	
#include "D:/Faculta/An 3/Sem 1/PG/glm/gtx/transform.hpp"
#include "D:/Faculta/An 3/Sem 1/PG/glm/gtc/quaternion.hpp"
#include "D:/Faculta/An 3/Sem 1/PG/glm/gtx/quaternion.hpp"

#include <string>
#include "Collisions.hpp"

namespace gps {

    enum MOVE_DIRECTION { MOVE_FORWARD, MOVE_BACKWARD, MOVE_RIGHT, MOVE_LEFT, MOVE_UP, MOVE_DOWN };

    class Camera
    {
    public:
        //Camera constructor
        Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp);
        //return the view matrix, using the glm::lookAt() function
        glm::mat4 getViewMatrix();
        //update the camera internal parameters following a camera move event
        void move(MOVE_DIRECTION direction, float speed);
        //update the camera internal parameters following a camera rotate event
        //yaw - camera rotation around the y axis
        //pitch - camera rotation around the x axis
        void rotate(float pitch, float yaw);

        // get camera position
        glm::vec3 getCameraPosition();

        void resetCamera();
        
        void setCameraDirection(glm::vec3 newDir);
        void setCameraPosition(glm::vec3 newPos);
        void setCameraTarget(glm::vec3 newTarget);

        void addBoundary(gps::Boundary newBoundary);


    private:
        glm::vec3 cameraPosition;
        glm::vec3 cameraTarget;
        glm::vec3 cameraFrontDirection;
        glm::vec3 cameraRightDirection;
        glm::vec3 cameraUpDirection;

        std::vector<Boundary> boundaries;
        bool isBounded(glm::vec3 nextPos);


        float yawAngle;
        float pitchAngle;
    };

}

#endif /* Camera_hpp */
