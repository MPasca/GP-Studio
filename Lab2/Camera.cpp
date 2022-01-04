#include "Camera.hpp"
#include <iostream>

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        //TODO
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;

        this->cameraUpDirection = cameraUp;
        this->cameraFrontDirection = normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = normalize(cross(cameraFrontDirection, cameraUpDirection));
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        //TODO
        return glm::lookAt(cameraPosition, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        switch (direction) {
        case MOVE_LEFT:
            cameraPosition -= speed * cameraRightDirection;
            cameraTarget -= speed * cameraRightDirection;
            break;

        case MOVE_RIGHT:
            cameraPosition += speed * cameraRightDirection;
            cameraTarget += speed * cameraRightDirection;
            break;

        case MOVE_FORWARD:
            cameraPosition += speed * cameraFrontDirection;
            cameraTarget += speed * cameraFrontDirection;
            break;

        case MOVE_BACKWARD:
            cameraPosition -= speed * cameraFrontDirection;
            cameraTarget -= speed * cameraFrontDirection;
            break;

        case MOVE_UP:
            cameraPosition += speed * cameraUpDirection;
            cameraTarget += speed * cameraUpDirection;
            break;

        case MOVE_DOWN:
            cameraPosition -= speed * cameraUpDirection;
            cameraTarget -= speed * cameraUpDirection;
            break;
        }
        this->cameraFrontDirection = normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = normalize(cross(cameraFrontDirection, cameraUpDirection));
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        //TODO
        yawAngle += yaw;
        pitchAngle += pitch;

        //glm::quat q = glm::quat(glm::vec3(yawAngle, pitchAngle, 0));

        //this->cameraTarget = cameraTarget * q;

        cameraTarget.x = cameraPosition.x + sin(yawAngle);
        cameraTarget.z = cameraPosition.z - cos(yawAngle);
        cameraTarget.y = cameraPosition.y + sin(pitchAngle);

        this->cameraFrontDirection = normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = normalize(cross(cameraFrontDirection, cameraUpDirection));

    }
}