#pragma once

#include <Eigen/Dense>
#include <glm/glm.hpp>
#include <vector>

namespace mahou {
Eigen::Matrix3f skew_symmetric(glm::vec3 axis) {
  Eigen::Matrix3f matrix;
  matrix(0, 0) = 0;
  matrix(0, 1) = -axis.z;
  matrix(0, 2) = axis.y;
  matrix(1, 0) = axis.z;
  matrix(1, 1) = 0;
  matrix(1, 2) = -axis.x;
  matrix(2, 0) = -axis.y;
  matrix(2, 1) = axis.x;
  matrix(2, 2) = 0;
  return matrix;
}

template <unsigned N> constexpr Eigen::Matrix<float, N, N> difference() {
  Eigen::Matrix<float, N, N> matrix;
  matrix.setZero();
  for (unsigned i = 0; i != N; ++i) {
    matrix(i, i) = -1;
    matrix(i, (i + 1) % N) = 1;
  }
  return matrix;
}

template <unsigned N> constexpr Eigen::Matrix<float, N, N> average() {
  Eigen::Matrix<float, N, N> matrix;
  matrix.setZero();
  for (unsigned i = 0; i != N; ++i) {
    matrix(i, i) = 0.5;
    matrix(i, (i + 1) % N) = 0.5;
  }
  return matrix;
}

template <unsigned N> glm::vec3 polyvec2(const Eigen::Matrix<float, N, 3> &positions) {
  glm::vec3 result{};
  for (unsigned a = 0; a != N; ++a) {
    unsigned b = (a + 1) % N;
    result += glm::cross(glm::vec3{positions(a, 0), positions(a, 1), positions(a, 2)},
                         glm::vec3{positions(b, 0), positions(b, 1), positions(b, 2)});
  }
  return result;
}

template <unsigned N> glm::vec3 polynormal(const Eigen::Matrix<float, N, 3> &positions) {
  return glm::normalize(polyvec2<N>(positions));
}

template <unsigned N> float polyarea(const Eigen::Matrix<float, N, 3> &positions) {
  return glm::length(polyvec2<N>(positions)) * 0.5;
}

template <unsigned N> Eigen::Matrix<float, N, 3> edges(const Eigen::Matrix<float, N, 3> &positions) {
  return difference<N>() * positions;
}

template <unsigned N> Eigen::Matrix<float, 3, N> gradient(const Eigen::Matrix<float, N, 3> &positions) {
  return (-1 / polyarea<N>(positions)) * skew_symmetric(polynormal<N>(positions)) * edges<N>(positions).transpose() *
         average<N>();
}

template <typename T> std::vector<glm::vec3> gradient_on(const T &mesh, std::function<float(const T &, unsigned)> phi) {
  std::vector<glm::vec3> gradients;
  gradients.reserve(mesh.num_faces());

  for (unsigned face = 0; face != mesh.num_faces(); ++face) {
    Eigen::Vector3f facegradient;
    if (mesh.is_triangle(face)) {
      Eigen::Matrix3f positions;
      Eigen::Vector3f signals;
      for (unsigned i = 0; i != 3; ++i) {
        unsigned vtx = mesh.face_vtx(face, i);
        glm::vec3 p = mesh.position(vtx);
        positions(i, 0) = p.x;
        positions(i, 1) = p.y;
        positions(i, 2) = p.z;
        signals[i] = phi(mesh, vtx);
      }
      facegradient = gradient<3>(positions) * signals;
    } else {
      Eigen::Matrix<float, 4, 3> positions;
      Eigen::Vector4f signals;
      for (unsigned i = 0; i != 4; ++i) {
        unsigned vtx = mesh.face_vtx(face, i);
        glm::vec3 p = mesh.position(vtx);
        positions(i, 0) = p.x;
        positions(i, 1) = p.y;
        positions(i, 2) = p.z;
        signals[i] = phi(mesh, vtx);
      }
      facegradient = gradient<4>(positions) * signals;
    }
    gradients.emplace_back(facegradient.x(), facegradient.y(), facegradient.z());
  }

  return gradients;
}

} // namespace mahou
