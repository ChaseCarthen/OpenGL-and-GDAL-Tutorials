#include "camera.h"

camera::camera()
{
    maxAngleX = 75;
    maxAngleY = 75;
    angularSpeedX = 70;
    angularSpeedY = 70;
    direction = glm::vec3(0, 0, 1);
    motionvector = glm::vec3(0, 0, 0);
    position = glm::vec3(0, 10, -10);
    angleX = 0;
    angleY = 0;
    up = glm::vec3(0, 1, 0);
    motionSpeed = 100;
    projection = glm::perspective<float>(
                     35,         // The horizontal Field of View, in degrees : the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
                     4.0f / 3.0f, // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
                     0.9f,        // Near clipping plane. Keep as big as possible, or you'll get precision issues.
                     100000.0f       // Far clipping plane. Keep as little as possible.
                 );
    update();

    maxForwardVel = 10;
    minForwardVel = -10;
    maxSideVel = 10;
    minSideVel = -10;
    maxFlightVel = 10;
    minFlightVel = -10;
    sideVel = flightVel = forwardVel = 0;
}

glm::mat4 camera::getProjection()
{
    return projection;
}

glm::mat4 camera::getView()
{
    return view;
}
void camera::orbitX(float dir)
{

    angleX += dir * angularSpeedX;
    glm::vec3 d = glm::rotate(direction, angleX, up);
    d = glm::rotate(d, angleY, glm::cross(d, up));
    if (dir > 0)
        motionvector -= motionSpeed * glm::cross(up, glm::normalize(d));
    else
        motionvector += motionSpeed * glm::cross(up, d);
}

void camera::rotateX(float dir)
{
    angleX = dir * angularSpeedX;
}

void camera::rotateY(float dir)
{
    angleY = dir * angularSpeedY;
}
void camera::applyRotation()
{
    direction = glm::rotate(direction, angleX, up);
    direction = glm::rotate(direction, angleY, glm::cross(direction, up));
}

void camera::translate(float dir)
{
    forwardVel = motionSpeed * dir;
    forwardVel = glm::clamp(forwardVel, minForwardVel, maxForwardVel);
}

void camera::strafe(float dir)
{
    sideVel = motionSpeed * dir;
    sideVel = glm::clamp(sideVel, minSideVel, maxSideVel);
}

void camera::update()
{
    applyRotation();
    motionvector += forwardVel * direction;
    motionvector += sideVel * glm::cross(up, direction);
    motionvector += flightVel * up;
    position += motionvector;
    //std::cout << position.x << " " << position.y << " " << position.z << std::endl;
    view = glm::lookAt( position, //Eye Position
                        position + direction, //Focus point
                        up); //Positive Y is up
    
    motionvector = glm::vec3(0, 0, 0);
}

void camera::resetHorizontalSpeed()
{
    sideVel = 0;
}

void camera::resetVerticalSpeed()
{
    forwardVel = 0;
}

void camera::resetHorizontalRotation()
{
    angleX = 0;
}

void camera::resetVerticalRotation()
{
    angleY = 0;
}

void camera::flight(float dir)
{
    flightVel = motionSpeed *dir;
}

void camera::resetFlightSpeed()
{
    flightVel = 0;
}