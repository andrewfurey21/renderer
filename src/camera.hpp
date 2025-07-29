#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"


class Camera {
private:
  float fovy;
  float aspect_ratio;
  float near, far;

  float move_speed;
  float angular_speed;

  float width, height;

  glm::vec3 position;
  glm::vec3 up_vector;

  glm::mat4 proj;

  glm::vec2 previous_mouse;

  float pitch;

public:
  glm::vec3 view_direction;
  float yaw;
  Camera(float fovy, float width, float height, float near, float far,
         float move_speed, float angular_speed)
      : fovy(fovy), width(width), height(height), aspect_ratio(width / height),
        near(near), far(far), move_speed(move_speed),
        angular_speed(angular_speed), yaw(-90.0f), pitch(0.0f) {
    proj = glm::perspective(glm::radians(fovy), aspect_ratio, near, far);

    position = glm::vec3(0.0f, 0.0f, 0.0f);
    view_direction = glm::vec3(0.0f, 0.0f, -1.0f);
    up_vector = glm::vec3(0.0f, 1.0f, 0.0f);

    previous_mouse = glm::vec2(width / 2, height / 2);
  }

  void set_pos(float x, float y, float z) {
    position = glm::vec3(x, y, z);
  }

  const glm::mat4 view() {
    return glm::lookAt(position, position + view_direction, up_vector);
  }

  const glm::vec3& view_dir() const {
    return view_direction;
  }

  void clamp() {
    if (position.y < 1.0f) {
      position.y = 1.0f;
    }
  }

  glm::mat4 projection() const { return proj; }

  void forward() { position += glm::normalize(view_direction) * move_speed; clamp(); }

  void backward() { position -= glm::normalize(view_direction) * move_speed; clamp(); }

  void left() {
    glm::vec3 direction = glm::cross(view_direction, up_vector);
    position -= glm::normalize(direction) * move_speed;
    clamp();
  }

  void right() {
    glm::vec3 direction = glm::cross(view_direction, up_vector);
    position += glm::normalize(direction) * move_speed;
    clamp();
  }

  void up() { position += glm::normalize(up_vector) * move_speed; clamp(); }

  void down() { position -= glm::normalize(up_vector) * move_speed; clamp(); }

  void mouse_look(glm::vec2 new_mouse) {
    glm::vec2 mouse_delta = previous_mouse - new_mouse;
    mouse_delta *= angular_speed;
    yaw -= mouse_delta.x;
    pitch += mouse_delta.y;

    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    glm::vec3 new_view_direction;
    new_view_direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    new_view_direction.y = sin(glm::radians(pitch));
    new_view_direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    view_direction = glm::normalize(new_view_direction);

    previous_mouse = new_mouse;
  }

  glm::vec3 pos() { return position; }
};

#endif
