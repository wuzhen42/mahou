#include "ddg.h"
#include <Eigen/Dense>

template <int ROW, int COL> using Matrix = Eigen::Matrix<float, ROW, COL, 0, ROW, COL>;

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

template <int N> constexpr Matrix<N, N> difference() {
  Matrix<N, N> matrix;
  matrix.setZero();
  for (int i = 0; i != N; ++i) {
    matrix(i, i) = -1;
    matrix(i, (i + 1) % N) = 1;
  }
  return matrix;
}

template <int N> constexpr Matrix<N, N> average() {
  Matrix<N, N> matrix;
  matrix.setZero();
  for (int i = 0; i != N; ++i) {
    matrix(i, i) = 0.5;
    matrix(i, (i + 1) % N) = 0.5;
  }
  return matrix;
}

template <int N> glm::vec3 polyvec2(const Matrix<N, 3> &positions) {
  glm::vec3 result{};
  for (int a = 0; a != N; ++a) {
    int b = (a + 1) % N;
    result += glm::cross(glm::vec3{positions(a, 0), positions(a, 1), positions(a, 2)},
                         glm::vec3{positions(b, 0), positions(b, 1), positions(b, 2)});
  }
  return result;
}

template <int N> glm::vec3 polynormal(const Matrix<N, 3> &positions) { return glm::normalize(polyvec2(positions)); }

template <int N> float polyarea(const Matrix<N, 3> &positions) { return glm::length(polyvec2(positions)) * 0.5; }

template <int N> Matrix<N, 3> edges(const Matrix<N, 3> &positions) { return difference<N>() * positions; }

template <int N> Matrix<N, 3> midpoints(const Matrix<N, 3> &positions) { return average<N>() * positions; }

template <int N> glm::vec3 center(const Matrix<N, 3> &positions) {
  glm::vec3 sum{};
  for (int a = 0; a != N; ++a) {
    sum += glm::vec3{positions(a, 0), positions(a, 1), positions(a, 2)};
  }
  return sum / float{N};
}

template <int N> Matrix<N, 3> flat(const Matrix<N, 3> &positions) {
  glm::vec3 normal = polynormal(positions);
  Eigen::Vector3f vec_norm{normal.x, normal.y, normal.z};
  return edges(positions) * (Eigen::Matrix3f::Identity() - vec_norm * vec_norm.transpose());
}

template <int N> Matrix<3, N> sharp(const Matrix<N, 3> &positions) {
  glm::vec3 normal = polynormal(positions);
  glm::vec3 cent = center(positions);
  Matrix<N, 3> mid = midpoints(positions);
  for (int i = 0; i != N; ++i) {
    mid(i, 0) -= cent.x;
    mid(i, 1) -= cent.y;
    mid(i, 2) -= cent.z;
  }
  return (1.0 / polyarea(positions)) * skew_symmetric(normal) * mid.transpose();
}

template <int N> Matrix<N, N> project(const Matrix<N, 3> &positions) {
  return Matrix<N, N>::Identity() - flat(positions) * sharp(positions);
}

template <int N> Matrix<N, N> innerproduct(const Matrix<N, 3> &positions) {
  Matrix<3, N> U = sharp(positions);
  return polyarea(positions) * (U.transpose() * U);
}

template <int N> Matrix<3, N> gradient(const Matrix<N, 3> &positions) {
  return (-1 / polyarea(positions)) * skew_symmetric(polynormal(positions)) * edges(positions).transpose() *
         average<N>();
}

template <int N> Matrix<N, N> laplace(const Matrix<N, 3> &positions) {
  Matrix<N, N> D = difference<N>();
  return D.transpose() * innerproduct(positions) * D;
}
namespace mahou {
std::vector<glm::vec3> gradient_on(const Mesh &mesh, std::function<float(int)> phi) {
  std::vector<glm::vec3> gradients;
  gradients.reserve(mesh.num_faces());

  for (int face = 0; face != mesh.num_faces(); ++face) {
    Eigen::Vector3f facegradient;
    if (mesh.is_triangle(face)) {
      Eigen::Matrix3f positions;
      Eigen::Vector3f signals;
      for (int i = 0; i != 3; ++i) {
        int vtx = mesh.vtx(face, i);
        glm::vec3 p = mesh.position(vtx);
        positions(i, 0) = p.x;
        positions(i, 1) = p.y;
        positions(i, 2) = p.z;
        signals[i] = phi(vtx);
      }
      facegradient = gradient(positions) * signals;
    } else {
      Eigen::Matrix<float, 4, 3> positions;
      Eigen::Vector4f signals;
      for (int i = 0; i != 4; ++i) {
        int vtx = mesh.vtx(face, i);
        glm::vec3 p = mesh.position(vtx);
        positions(i, 0) = p.x;
        positions(i, 1) = p.y;
        positions(i, 2) = p.z;
        signals[i] = phi(vtx);
      }
      facegradient = gradient(positions) * signals;
    }
    gradients.emplace_back(facegradient.x(), facegradient.y(), facegradient.z());
  }

  return gradients;
}

std::vector<float> laplace_on(const Mesh &mesh, std::function<float(int)> phi) {
  std::vector<float> laplacians;
  laplacians.reserve(mesh.num_faces());

  for (int face = 0; face != mesh.num_faces(); ++face) {
    if (mesh.is_triangle(face)) {
      Eigen::Matrix3f positions;
      Eigen::Vector3f signals;
      for (int i = 0; i != 3; ++i) {
        int vtx = mesh.vtx(face, i);
        glm::vec3 p = mesh.position(vtx);
        positions(i, 0) = p.x;
        positions(i, 1) = p.y;
        positions(i, 2) = p.z;
        signals[i] = phi(vtx);
      }
      laplacians.push_back(signals.transpose() * laplace(positions) * signals);
    } else {
      Eigen::Matrix<float, 4, 3> positions;
      Eigen::Vector4f signals;
      for (int i = 0; i != 4; ++i) {
        int vtx = mesh.vtx(face, i);
        glm::vec3 p = mesh.position(vtx);
        positions(i, 0) = p.x;
        positions(i, 1) = p.y;
        positions(i, 2) = p.z;
        signals[i] = phi(vtx);
      }
      laplacians.push_back(signals.transpose() * laplace(positions) * signals);
    }
  }

  return laplacians;
}
} // namespace mahou