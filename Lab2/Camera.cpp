#include "Camera.hpp"
#include <iostream>
#include "Mesh.hpp"
#include "Model3D.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        //TODO
        this->pitchAngle = 0;
        this->yawAngle = 0;

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

    bool Camera::isBounded(glm::vec3 nextPos) {
        float minX = -7.49f, maxX = 7.49f;
        float minY = -0.1f, maxY = 5.0f;
        float minZ = -5.96f, maxZ = 5.95f;

        if (nextPos.x < minX || nextPos.x > maxX) {
            return false;
        }
        else if (nextPos.y < minY || nextPos.y > maxY) {
            return false;
        }
        else if (nextPos.z < minZ || nextPos.z > maxZ) {
            return false;
        }


        for (gps::Model3D crtBoundary : boundaries) {
            for (gps::Mesh crtMesh : crtBoundary.getMeshes()) {
                if (crtMesh.checkCollision(nextPos)) {
                    return false;
                }
            }
        }

        return true;
    }

    // update the camera internal parameters following a camera move event + check if it's boundedwwww wadsa wad awdawdaw 
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        switch (direction) {
        case MOVE_LEFT:
            if (isBounded(cameraPosition - speed * cameraRightDirection)) {
                cameraPosition -= speed * cameraRightDirection;
                cameraTarget -= speed * cameraRightDirection;
            }
            else {
                cameraPosition += 0.1f * cameraRightDirection;
                cameraTarget += 0.1f * cameraRightDirection;
            }
            break;

        case MOVE_RIGHT:
            if (isBounded(cameraPosition + speed * cameraRightDirection)) {
                cameraPosition += speed * cameraRightDirection;
                cameraTarget += speed * cameraRightDirection;
            }
            else {
                cameraPosition -= 0.1f * cameraRightDirection;
                cameraTarget -= 0.1f * cameraRightDirection;
            }
            break;

        case MOVE_FORWARD:
            if (isBounded(cameraPosition + speed * cameraFrontDirection)) {
                cameraPosition += speed * cameraFrontDirection;
                cameraTarget += speed * cameraFrontDirection;
            }
            else {
                cameraPosition -= 0.1f * cameraFrontDirection;
                cameraTarget -= 0.1f * cameraFrontDirection;
            }
            break;

        case MOVE_BACKWARD:
            if (isBounded(cameraPosition - speed * cameraFrontDirection)) {
                cameraPosition -= speed * cameraFrontDirection;
                cameraTarget -= speed * cameraFrontDirection;
            }
            else {
                cameraPosition += 0.1f * cameraFrontDirection;
                cameraTarget += 0.1f * cameraFrontDirection;
            }
            break;

        case MOVE_UP:
            if (isBounded(cameraPosition + speed * cameraUpDirection)) {
                cameraPosition += speed * cameraUpDirection;
                cameraTarget += speed * cameraUpDirection;
            }
            else {
                cameraPosition -= 0.1f * cameraUpDirection;
                cameraTarget -= 0.1f * cameraUpDirection;
            }

            break;

        case MOVE_DOWN:
            if (isBounded(cameraPosition + speed * cameraUpDirection)) {
                cameraPosition -= speed * cameraUpDirection;
                cameraTarget -= speed * cameraUpDirection;
            }
            else {
                cameraPosition += 0.1f * cameraUpDirection;
                cameraTarget += 0.1f * cameraUpDirection;
            }
            break;
        }
        this->cameraFrontDirection = normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = normalize(cross(cameraFrontDirection, cameraUpDirection));
    }

    // update the camera internal parameters following a camera rotate event
    // yaw - camera rotation around the y axis
    // pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        //TODO
        yawAngle += yaw;
        pitchAngle += pitch;

        //std::cout << "pitchAngle: " << pitchAngle << "\n";
        //glm::quat q = glm::quat(glm::vec3(yawAngle, pitchAngle, 0));

        //this->cameraTarget = cameraTarget * q;


        cameraTarget.x = cameraPosition.x + sin(pitchAngle);
        cameraTarget.z = cameraPosition.z - cos(pitchAngle);
        cameraTarget.y = cameraPosition.y + sin(yawAngle);

        this->cameraFrontDirection = normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = normalize(cross(cameraFrontDirection, cameraUpDirection));

    }

    glm::vec3 Camera::getCameraPosition() {
        return cameraPosition;
    }

    glm::vec3 Camera::getCameraTarget() {
        return cameraTarget;
    }

    glm::vec3 Camera::getCameraDirection() {
        return cameraFrontDirection;
    }

    glm::vec3 Camera::getCameraUp() {
        return cameraUpDirection;
    }

    void Camera::setCameraDirection(glm::vec3 newDir) {
        this->cameraFrontDirection = normalize(newDir);
        this->cameraRightDirection = normalize(cross(cameraFrontDirection, cameraUpDirection));
    }

    void Camera::setCameraPosition(glm::vec3 newPos) {
        this->cameraPosition = newPos;
    }

    void Camera::setCameraTarget(glm::vec3 newTarget) {
        this->cameraTarget = newTarget;
    }

    void Camera::resetCamera() {
        this->yawAngle = 0.0f;
        this->pitchAngle = 1.91f;

        this->cameraPosition = glm::vec3(-7.0f, 1.0f, -5.0f);
        this->cameraTarget = glm::vec3(7.0f, 2.0f, -5.0f);
        this->cameraUpDirection = glm::vec3(0.0f, 1.0f, 0.0f);

        this->cameraFrontDirection = normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = normalize(cross(cameraFrontDirection, cameraUpDirection));
    }

    void Camera::resetAngles() {
        this->yawAngle = 0.0f;
        this->pitchAngle = 0.0f;
    }

    void Camera::addBoundary(gps::Model3D newBoundary) {
        boundaries.push_back(newBoundary);
    }
}