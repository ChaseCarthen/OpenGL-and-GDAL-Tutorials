#ifndef CAMERA_H
#define CAMERA_H
//#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <iostream>
class camera
{
public:
    camera();
    void translate(float dir);
    void strafe(float dir);
    void flight(float dir);
    void resetHorizontalSpeed();
    void resetVerticalSpeed();
    void resetHorizontalRotation();
    void resetVerticalRotation();
    void resetFlightSpeed();
    void orbitX(float dir);
    void rotateX(float dir);
    void rotateY(float dir);
    void applyRotation();
    void update();
    glm::mat4 getProjection();
    glm::mat4 getView();
    glm::vec3 getPos()
    {
        return position;
    };

private:
    float maxAngleX;
    float maxAngleY;
    float angleX;
    float angleY;
    float angularSpeedX;
    float angularSpeedY;
    float motionSpeed;

    // translation speed
    float forwardVel, sideVel, flightVel;
    float maxForwardVel, minForwardVel, maxSideVel, minSideVel,
          maxFlightVel, minFlightVel;

    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 up;
    glm::vec3 motionvector;
    glm::mat4 view;
    glm::mat4 projection;
};

#endif // CAMERA_H
