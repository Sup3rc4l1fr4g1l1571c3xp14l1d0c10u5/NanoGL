#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <float.h>

#ifdef near
#undef near
#endif

#ifdef far
#undef far
#endif

typedef union  {
  float f[16];
  struct {
    float _00, _01, _02, _03;
    float _10, _11, _12, _13;
    float _20, _21, _22, _23;
    float _30, _31, _32, _33;
  };
} mat4;

typedef union  {
  float f[9];
  struct {
    float _00, _01, _02;
    float _10, _11, _12;
    float _20, _21, _22;
  };
} mat3;

typedef union  {
  float f[3];
  struct {
    float x, y, z;
  };
} vec3;

typedef union  {
  float f[4];
  struct {
    float x, y, z, w;
  };
  vec3 xyz;
} vec4;

/**
 * Takes two 4-by-4 matrices, a and b, and computes the product in the order
 * that pre-composes b with a.  In other words, the matrix returned will
 * transform by b first and then a.  Note this is subtly different from just
 * multiplying the matrices together.  For given a and b, this function returns
 * the same object in both row-major and column-major mode.
 * @param {mat4} a A matrix.
 * @param {mat4} b A matrix.
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
extern mat4 *mat4_multiply(mat4 *dst, const mat4 *a, const mat4 *b);

/**
 * adds 2 vec3s
 * @param {vec3} a a
 * @param {vec3} b b
 * @param {vec3} dst optional vector3 to store result
 * @return {vec3} dst or new vec3 if not provided
 */
extern vec3 *vec3_addVectors(vec3 *dst, const vec3 *a, const vec3 *b);

/**
 * subtracts 2 vec3s
 * @param {vec3} a a
 * @param {vec3} b b
 * @param {vec3} dst optional vector3 to store result
 * @return {vec3} dst or new vec3 if not provided
 */
extern vec3 *vec3_subtractVectors(vec3 *dst, const vec3 *a, const vec3 *b);

/**
 * normalizes a vector.
 * @param {vec3} v vector to normalzie
 * @param {vec3} dst optional vector3 to store result
 * @return {vec3} dst or new vec3 if not provided
 */
extern vec3 *vec3_normalize(vec3 *dst, const vec3 *v);

/**
 * Computes the cross product of 2 vec3s
 * @param {vec3} a a
 * @param {vec3} b b
 * @param {vec3} dst optional vector3 to store result
 * @return {vec3} dst or new vec3 if not provided
 */
extern vec3 *vec3_cross(vec3 *dst, const vec3 *a, const vec3 *b);

/**
 * Computes the dot product of two vectors; assumes both vectors have
 * three entries.
 * @param {vec3} a Operand vector.
 * @param {vec3} b Operand vector.
 * @return {number} dot product
 */
extern float vec3_dot(const vec3* a, const vec3* b);

/**
 * Computes the distance squared between 2 points
 * @param {vec3} a
 * @param {vec3} b
 * @return {nubmer} distance squared between a and b
 */
extern float vec3_distanceSq(const vec3* a, const vec3* b);

/**
 * Computes the distance between 2 points
 * @param {vec3} a
 * @param {vec3} b
 * @return {nubmer} distance between a and b
 */
extern float vec3_distance(const vec3* a, const vec3* b);

/**
 * Makes an identity matrix.
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
extern mat4 * mat4_identity(mat4 *dst);

/**
 * Transposes a matrix.
 * @param {mat4} m matrix to transpose.
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
extern mat4 * mat4_transpose(mat4 *dst, const mat4 *m);

/**
 * Creates a lookAt matrix.
 * This is a world matrix for a camera. In other words it will transform
 * from the origin to a place and orientation in the world. For a view
 * matrix take the inverse of this.
 * @param {vec3} cameraPosition position of the camera
 * @param {vec3} target position of the target
 * @param {vec3} up direction
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
extern mat4 * mat4_lookAt(mat4 *dst, const vec3 *cameraPosition, const vec3 *target, const vec3 *up);

/**
 * Computes a 4-by-4 perspective transformation matrix given the angular height
 * of the frustum, the aspect ratio, and the near and far clipping planes.  The
 * arguments define a frustum extending in the negative z direction.  The given
 * angle is the vertical angle of the frustum, and the horizontal angle is
 * determined to produce the given aspect ratio.  The arguments near and far are
 * the distances to the near and far clipping planes.  Note that near and far
 * are not z coordinates, but rather they are distances along the negative
 * z-axis.  The matrix generated sends the viewing frustum to the unit box.
 * We assume a unit box extending from -1 to 1 in the x and y dimensions and
 * from -1 to 1 in the z dimension.
 * @param {number} fieldOfViewInRadians field of view in y axis.
 * @param {number} aspect aspect of viewport (width / height)
 * @param {number} near near Z clipping plane
 * @param {number} far far Z clipping plane
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
extern mat4 * mat4_perspective(mat4 *dst, float fieldOfViewInRadians, float aspect, float near, float far);

/**
 * Computes a 4-by-4 orthographic projection matrix given the coordinates of the
 * planes defining the axis-aligned, box-shaped viewing volume.  The matrix
 * generated sends that box to the unit box.  Note that although left and right
 * are x coordinates and bottom and top are y coordinates, near and far
 * are not z coordinates, but rather they are distances along the negative
 * z-axis.  We assume a unit box extending from -1 to 1 in the x and y
 * dimensions and from -1 to 1 in the z dimension.
 * @param {number} left The x coordinate of the left plane of the box.
 * @param {number} right The x coordinate of the right plane of the box.
 * @param {number} bottom The y coordinate of the bottom plane of the box.
 * @param {number} top The y coordinate of the right plane of the box.
 * @param {number} near The negative z coordinate of the near plane of the box.
 * @param {number} far The negative z coordinate of the far plane of the box.
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
extern mat4 * mat4_orthographic(mat4 *dst, float left, float right, float bottom, float top, float near, float far);

/**
 * Computes a 4-by-4 perspective transformation matrix given the left, right,
 * top, bottom, near and far clipping planes. The arguments define a frustum
 * extending in the negative z direction. The arguments near and far are the
 * distances to the near and far clipping planes. Note that near and far are not
 * z coordinates, but rather they are distances along the negative z-axis. The
 * matrix generated sends the viewing frustum to the unit box. We assume a unit
 * box extending from -1 to 1 in the x and y dimensions and from -1 to 1 in the z
 * dimension.
 * @param {number} left The x coordinate of the left plane of the box.
 * @param {number} right The x coordinate of the right plane of the box.
 * @param {number} bottom The y coordinate of the bottom plane of the box.
 * @param {number} top The y coordinate of the right plane of the box.
 * @param {number} near The negative z coordinate of the near plane of the box.
 * @param {number} far The negative z coordinate of the far plane of the box.
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
extern mat4 * mat4_frustum(mat4 *dst, float left, float right, float bottom, float top, float near, float far);

/**
 * Makes a translation matrix
 * @param {number} tx x translation.
 * @param {number} ty y translation.
 * @param {number} tz z translation.
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
extern mat4 *mat4_translation(mat4 *dst, float tx, float ty, float tz);

/**
 * Mutliply by translation matrix.
 * @param {mat4} m matrix to multiply
 * @param {number} tx x translation.
 * @param {number} ty y translation.
 * @param {number} tz z translation.
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
extern mat4 *mat4_translate(mat4 *dst, const mat4 *m, float tx, float ty, float tz);

/**
 * Makes an x rotation matrix
 * @param {number} angleInRadians amount to rotate
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
extern mat4 * mat4_xRotation(mat4 * dst, float angleInRadians);

/**
 * Multiply by an x rotation matrix
 * @param {mat4} m matrix to multiply
 * @param {number} angleInRadians amount to rotate
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
extern mat4 * mat4_xRotate(mat4 *dst, const mat4 *m, float angleInRadians);

/**
 * Makes an y rotation matrix
 * @param {number} angleInRadians amount to rotate
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
extern mat4 * mat4_yRotation(mat4 * dst, float angleInRadians);

/**
 * Multiply by an y rotation matrix
 * @param {mat4} m matrix to multiply
 * @param {number} angleInRadians amount to rotate
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
extern mat4 * mat4_yRotate(mat4 *dst, const mat4 *m, float angleInRadians);

/**
 * Makes an z rotation matrix
 * @param {number} angleInRadians amount to rotate
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
extern mat4 * mat4_zRotation(mat4 * dst, float angleInRadians);

/**
 * Multiply by an z rotation matrix
 * @param {mat4} m matrix to multiply
 * @param {number} angleInRadians amount to rotate
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
extern mat4 *mat4_zRotate(mat4 *dst, const mat4 *m, float angleInRadians);

/**
 * Makes an rotation matrix around an arbitrary axis
 * @param {vec3} axis axis to rotate around
 * @param {number} angleInRadians amount to rotate
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
extern mat4 * mat4_axisRotation(mat4 *dst, const vec3 *axis, float angleInRadians);

/**
 * Multiply by an axis rotation matrix
 * @param {mat4} m matrix to multiply
 * @param {vec3} axis axis to rotate around
 * @param {number} angleInRadians amount to rotate
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
extern mat4 * mat4_axisRotate(mat4 *dst, const mat4 *m, const vec3 *axis, float angleInRadians);

/**
 * Makes a scale matrix
 * @param {number} sx x scale.
 * @param {number} sy y scale.
 * @param {number} sz z scale.
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
extern mat4 * mat4_scaling(mat4 *dst, float sx, float sy, float sz);

/**
 * Multiply by a scaling matrix
 * @param {mat4} m matrix to multiply
 * @param {number} sx x scale.
 * @param {number} sy y scale.
 * @param {number} sz z scale.
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
extern mat4 * mat4_scale(mat4 *dst, const mat4* m, float sx, float sy, float sz);

/**
 * Computes the inverse of a matrix.
 * @param {mat4} m matrix to compute inverse of
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
extern mat4 * mat4_inverse(mat4 *dst, const mat4* m);

/**
 * Takes a  matrix and a vector with 4 entries, transforms that vector by
 * the matrix, and returns the result as a vector with 4 entries.
 * @param {mat4} m The matrix.
 * @param {vec4} v The point in homogenous coordinates.
 * @param {vec4} dst optional vec4 to store result
 * @return {vec4} dst or new vec4 if not provided
 */
extern vec4* vec4_transformVector(vec4 *dst, const mat4 *m, const vec4 *v);

/**
 * Takes a 4-by-4 matrix and a vector with 3 entries,
 * interprets the vector as a point, transforms that point by the matrix, and
 * returns the result as a vector with 3 entries.
 * @param {mat4} m The matrix.
 * @param {vec3} v The point.
 * @param {vec4} dst optional vec4 to store result
 * @return {vec4} dst or new vec4 if not provided
 */
extern vec4* vec4_transformPoint(vec4 *dst, const mat4 *m, const vec4 *v);

/**
 * Takes a 4-by-4 matrix and a vector with 3 entries, interprets the vector as a
 * direction, transforms that direction by the matrix, and returns the result;
 * assumes the transformation of 3-dimensional space represented by the matrix
 * is parallel-preserving, i.e. any combination of rotation, scaling and
 * translation, but not a perspective distortion. Returns a vector with 3
 * entries.
 * @param {mat4} m The matrix.
 * @param {vec3} v The direction.
 * @param {vec4} dst optional vec4 to store result
 * @return {vec4} dst or new vec4 if not provided
 */
extern vec4* vec4_transformDirection(vec4 *dst, const mat4 *m, const vec3 *v);

/**
 * Takes a 4-by-4 matrix m and a vector v with 3 entries, interprets the vector
 * as a normal to a surface, and computes a vector which is normal upon
 * transforming that surface by the matrix. The effect of this function is the
 * same as transforming v (as a direction) by the inverse-transpose of m.  This
 * function assumes the transformation of 3-dimensional space represented by the
 * matrix is parallel-preserving, i.e. any combination of rotation, scaling and
 * translation, but not a perspective distortion.  Returns a vector with 3
 * entries.
 * @param {mat4} m The matrix.
 * @param {vec3} v The normal.
 * @param {vec3} [dst] The direction.
 * @return {vec3} The transformed direction.
 */
extern vec3* vec3_transformNormal(vec3 *dst, const mat4 *m, const vec3 *v);

extern mat4 * mat4_copy(mat4 *dst, const mat4*src);

extern mat3 * mat4_to_mat3(mat3 *dst, const mat4*src);

extern mat3 *mat3_invert(mat3 *dst, const mat3 *m);

#ifdef  MAT4_IMPLEMENTATION

/**
 * Takes two 4-by-4 matrices, a and b, and computes the product in the order
 * that pre-composes b with a.  In other words, the matrix returned will
 * transform by b first and then a.  Note this is subtly different from just
 * multiplying the matrices together.  For given a and b, this function returns
 * the same object in both row-major and column-major mode.
 * @param {mat4} a A matrix.
 * @param {mat4} b A matrix.
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */

mat4* mat4_multiply(mat4* dst, const mat4* a, const mat4* b) {
  dst = dst ? dst : (mat4*)calloc(1, sizeof(mat4));
    float b00 = b->f[0 * 4 + 0];
    float b01 = b->f[0 * 4 + 1];
    float b02 = b->f[0 * 4 + 2];
    float b03 = b->f[0 * 4 + 3];
    float b10 = b->f[1 * 4 + 0];
    float b11 = b->f[1 * 4 + 1];
    float b12 = b->f[1 * 4 + 2];
    float b13 = b->f[1 * 4 + 3];
    float b20 = b->f[2 * 4 + 0];
    float b21 = b->f[2 * 4 + 1];
    float b22 = b->f[2 * 4 + 2];
    float b23 = b->f[2 * 4 + 3];
    float b30 = b->f[3 * 4 + 0];
    float b31 = b->f[3 * 4 + 1];
    float b32 = b->f[3 * 4 + 2];
    float b33 = b->f[3 * 4 + 3];
    float a00 = a->f[0 * 4 + 0];
    float a01 = a->f[0 * 4 + 1];
    float a02 = a->f[0 * 4 + 2];
    float a03 = a->f[0 * 4 + 3];
    float a10 = a->f[1 * 4 + 0];
    float a11 = a->f[1 * 4 + 1];
    float a12 = a->f[1 * 4 + 2];
    float a13 = a->f[1 * 4 + 3];
    float a20 = a->f[2 * 4 + 0];
    float a21 = a->f[2 * 4 + 1];
    float a22 = a->f[2 * 4 + 2];
    float a23 = a->f[2 * 4 + 3];
    float a30 = a->f[3 * 4 + 0];
    float a31 = a->f[3 * 4 + 1];
    float a32 = a->f[3 * 4 + 2];
    float a33 = a->f[3 * 4 + 3];
    dst->f[ 0] = b00 * a00 + b01 * a10 + b02 * a20 + b03 * a30;
    dst->f[ 1] = b00 * a01 + b01 * a11 + b02 * a21 + b03 * a31;
    dst->f[ 2] = b00 * a02 + b01 * a12 + b02 * a22 + b03 * a32;
    dst->f[ 3] = b00 * a03 + b01 * a13 + b02 * a23 + b03 * a33;
    dst->f[ 4] = b10 * a00 + b11 * a10 + b12 * a20 + b13 * a30;
    dst->f[ 5] = b10 * a01 + b11 * a11 + b12 * a21 + b13 * a31;
    dst->f[ 6] = b10 * a02 + b11 * a12 + b12 * a22 + b13 * a32;
    dst->f[ 7] = b10 * a03 + b11 * a13 + b12 * a23 + b13 * a33;
    dst->f[ 8] = b20 * a00 + b21 * a10 + b22 * a20 + b23 * a30;
    dst->f[ 9] = b20 * a01 + b21 * a11 + b22 * a21 + b23 * a31;
    dst->f[10] = b20 * a02 + b21 * a12 + b22 * a22 + b23 * a32;
    dst->f[11] = b20 * a03 + b21 * a13 + b22 * a23 + b23 * a33;
    dst->f[12] = b30 * a00 + b31 * a10 + b32 * a20 + b33 * a30;
    dst->f[13] = b30 * a01 + b31 * a11 + b32 * a21 + b33 * a31;
    dst->f[14] = b30 * a02 + b31 * a12 + b32 * a22 + b33 * a32;
    dst->f[15] = b30 * a03 + b31 * a13 + b32 * a23 + b33 * a33;
  return dst;
}


/**
 * adds 2 vec3s
 * @param {vec3} a a
 * @param {vec3} b b
 * @param {vec3} dst optional vector3 to store result
 * @return {vec3} dst or new vec3 if not provided
 */
vec3* vec3_addVectors(vec3* dst, const vec3* a, const vec3* b) {
  dst = dst ? dst : (vec3*)calloc(1, sizeof(vec3));
  dst->f[0] = a->f[0] + b->f[0];
  dst->f[1] = a->f[1] + b->f[1];
  dst->f[2] = a->f[2] + b->f[2];
  return dst;
}

/**
 * subtracts 2 vec3s
 * @param {vec3} a a
 * @param {vec3} b b
 * @param {vec3} dst optional vector3 to store result
 * @return {vec3} dst or new vec3 if not provided
 */
vec3* vec3_subtractVectors(vec3* dst, const vec3* a, const vec3* b) {
  dst = dst ? dst : (vec3*)calloc(1, sizeof(vec3));
  dst->f[0] = a->f[0] - b->f[0];
  dst->f[1] = a->f[1] - b->f[1];
  dst->f[2] = a->f[2] - b->f[2];
  return dst;
}

/**
 * normalizes a vector.
 * @param {vec3} v vector to normalzie
 * @param {vec3} dst optional vector3 to store result
 * @return {vec3} dst or new vec3 if not provided
 */
vec3* vec3_normalize(vec3* dst, const vec3* v) {
  dst = dst ? dst : (vec3*)calloc(1, sizeof(vec3));
  float length = sqrtf(v->f[0] * v->f[0] + v->f[1] * v->f[1] + v->f[2] * v->f[2]);
  // make sure we don't divide by 0.
  if (length > 0.00001f) {
    dst->f[0] = v->f[0] / length;
    dst->f[1] = v->f[1] / length;
    dst->f[2] = v->f[2] / length;
  }
  return dst;
}

/**
 * Computes the cross product of 2 vec3s
 * @param {vec3} a a
 * @param {vec3} b b
 * @param {vec3} dst optional vector3 to store result
 * @return {vec3} dst or new vec3 if not provided
 */
vec3* vec3_cross(vec3* dst, const vec3* a, const vec3* b) {
  dst = dst ? dst : (vec3*)calloc(1, sizeof(vec3));
  dst->f[0] = a->f[1] * b->f[2] - a->f[2] * b->f[1];
  dst->f[1] = a->f[2] * b->f[0] - a->f[0] * b->f[2];
  dst->f[2] = a->f[0] * b->f[1] - a->f[1] * b->f[0];
  return dst;
}

/**
 * Computes the dot product of two vectors; assumes both vectors have
 * three entries.
 * @param {vec3} a Operand vector.
 * @param {vec3} b Operand vector.
 * @return {number} dot product
 */
float vec3_dot(const vec3* a, const vec3* b) {
  return (a->f[0] * b->f[0]) + (a->f[1] * b->f[1]) + (a->f[2] * b->f[2]);
}

/**
 * Computes the distance squared between 2 points
 * @param {vec3} a
 * @param {vec3} b
 * @return {nubmer} distance squared between a and b
 */
float vec3_distanceSq(const vec3* a, const vec3* b) {
  const float dx = a->f[0] - b->f[0];
  const float dy = a->f[1] - b->f[1];
  const float dz = a->f[2] - b->f[2];
  return dx * dx + dy * dy + dz * dz;
}

/**
 * Computes the distance between 2 points
 * @param {vec3} a
 * @param {vec3} b
 * @return {nubmer} distance between a and b
 */
float vec3_distance(const vec3* a, const vec3* b) {
  return sqrtf(vec3_distanceSq(a, b));
}

/**
 * Makes an identity matrix.
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
mat4* mat4_identity(mat4* dst) {
  dst = dst ? dst : (mat4*)calloc(1, sizeof(mat4));

  dst->f[0] = 1;
  dst->f[1] = 0;
  dst->f[2] = 0;
  dst->f[3] = 0;
  dst->f[4] = 0;
  dst->f[5] = 1;
  dst->f[6] = 0;
  dst->f[7] = 0;
  dst->f[8] = 0;
  dst->f[9] = 0;
  dst->f[10] = 1;
  dst->f[11] = 0;
  dst->f[12] = 0;
  dst->f[13] = 0;
  dst->f[14] = 0;
  dst->f[15] = 1;

  return dst;
}

/**
 * Transposes a matrix.
 * @param {mat4} m matrix to transpose.
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
mat4* mat4_transpose(mat4* dst, const mat4* m) {
  dst = dst ? dst : (mat4*)calloc(1, sizeof(mat4));

  dst->f[0] = m->f[0];
  dst->f[1] = m->f[4];
  dst->f[2] = m->f[8];
  dst->f[3] = m->f[12];
  dst->f[4] = m->f[1];
  dst->f[5] = m->f[5];
  dst->f[6] = m->f[9];
  dst->f[7] = m->f[13];
  dst->f[8] = m->f[2];
  dst->f[9] = m->f[6];
  dst->f[10] = m->f[10];
  dst->f[11] = m->f[14];
  dst->f[12] = m->f[3];
  dst->f[13] = m->f[7];
  dst->f[14] = m->f[11];
  dst->f[15] = m->f[15];

  return dst;
}

/**
 * Creates a lookAt matrix.
 * This is a world matrix for a camera. In other words it will transform
 * from the origin to a place and orientation in the world. For a view
 * matrix take the inverse of this.
 * @param {vec3} cameraPosition position of the camera
 * @param {vec3} target position of the target
 * @param {vec3} up direction
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
mat4* mat4_lookAt(mat4* dst, const vec3* cameraPosition, const vec3* target, const vec3* up) {
  dst = dst ? dst : (mat4*)calloc(1, sizeof(mat4));

  vec3 zAxis, tmp;
  vec3_normalize(&zAxis, vec3_subtractVectors(&tmp, cameraPosition, target));
  vec3 xAxis;
  vec3_normalize(&xAxis, vec3_cross(&tmp, up, &zAxis));
  vec3 yAxis;
  vec3_normalize(&xAxis, vec3_cross(&tmp, &zAxis, &xAxis));

  dst->f[0] = xAxis.f[0];
  dst->f[1] = xAxis.f[1];
  dst->f[2] = xAxis.f[2];
  dst->f[3] = 0;
  dst->f[4] = yAxis.f[0];
  dst->f[5] = yAxis.f[1];
  dst->f[6] = yAxis.f[2];
  dst->f[7] = 0;
  dst->f[8] = zAxis.f[0];
  dst->f[9] = zAxis.f[1];
  dst->f[10] = zAxis.f[2];
  dst->f[11] = 0;
  dst->f[12] = cameraPosition->f[0];
  dst->f[13] = cameraPosition->f[1];
  dst->f[14] = cameraPosition->f[2];
  dst->f[15] = 1;

  return dst;
}

/**
 * Computes a 4-by-4 perspective transformation matrix given the angular height
 * of the frustum, the aspect ratio, and the near and far clipping planes.  The
 * arguments define a frustum extending in the negative z direction.  The given
 * angle is the vertical angle of the frustum, and the horizontal angle is
 * determined to produce the given aspect ratio.  The arguments near and far are
 * the distances to the near and far clipping planes.  Note that near and far
 * are not z coordinates, but rather they are distances along the negative
 * z-axis.  The matrix generated sends the viewing frustum to the unit box.
 * We assume a unit box extending from -1 to 1 in the x and y dimensions and
 * from -1 to 1 in the z dimension.
 * @param {number} fieldOfViewInRadians field of view in y axis.
 * @param {number} aspect aspect of viewport (width / height)
 * @param {number} near near Z clipping plane
 * @param {number} far far Z clipping plane
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
mat4* mat4_perspective(mat4* dst, float fieldOfViewInRadians, float aspect, float near, float far) {
  dst = dst ? dst : (mat4*)calloc(1, sizeof(mat4));
  float f = tanf(M_PI * 0.5 - 0.5 * fieldOfViewInRadians);
  float rangeInv = 1.0 / (near - far);

  dst->f[0] = f / aspect;
  dst->f[1] = 0;
  dst->f[2] = 0;
  dst->f[3] = 0;
  dst->f[4] = 0;
  dst->f[5] = f;
  dst->f[6] = 0;
  dst->f[7] = 0;
  dst->f[8] = 0;
  dst->f[9] = 0;
  dst->f[10] = (near + far) * rangeInv;
  dst->f[11] = -1;
  dst->f[12] = 0;
  dst->f[13] = 0;
  dst->f[14] = near * far * rangeInv * 2;
  dst->f[15] = 0;

  return dst;
}

/**
 * Computes a 4-by-4 orthographic projection matrix given the coordinates of the
 * planes defining the axis-aligned, box-shaped viewing volume.  The matrix
 * generated sends that box to the unit box.  Note that although left and right
 * are x coordinates and bottom and top are y coordinates, near and far
 * are not z coordinates, but rather they are distances along the negative
 * z-axis.  We assume a unit box extending from -1 to 1 in the x and y
 * dimensions and from -1 to 1 in the z dimension.
 * @param {number} left The x coordinate of the left plane of the box.
 * @param {number} right The x coordinate of the right plane of the box.
 * @param {number} bottom The y coordinate of the bottom plane of the box.
 * @param {number} top The y coordinate of the right plane of the box.
 * @param {number} near The negative z coordinate of the near plane of the box.
 * @param {number} far The negative z coordinate of the far plane of the box.
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
mat4* mat4_orthographic(mat4* dst, float left, float right, float bottom, float top, float near, float far) {
  dst = dst ? dst : (mat4*)calloc(1, sizeof(mat4));

  dst->f[0] = 2 / (right - left);
  dst->f[1] = 0;
  dst->f[2] = 0;
  dst->f[3] = 0;
  dst->f[4] = 0;
  dst->f[5] = 2 / (top - bottom);
  dst->f[6] = 0;
  dst->f[7] = 0;
  dst->f[8] = 0;
  dst->f[9] = 0;
  dst->f[10] = 2 / (near - far);
  dst->f[11] = 0;
  dst->f[12] = (left + right) / (left - right);
  dst->f[13] = (bottom + top) / (bottom - top);
  dst->f[14] = (near + far) / (near - far);
  dst->f[15] = 1;

  return dst;
}

/**
 * Computes a 4-by-4 perspective transformation matrix given the left, right,
 * top, bottom, near and far clipping planes. The arguments define a frustum
 * extending in the negative z direction. The arguments near and far are the
 * distances to the near and far clipping planes. Note that near and far are not
 * z coordinates, but rather they are distances along the negative z-axis. The
 * matrix generated sends the viewing frustum to the unit box. We assume a unit
 * box extending from -1 to 1 in the x and y dimensions and from -1 to 1 in the z
 * dimension.
 * @param {number} left The x coordinate of the left plane of the box.
 * @param {number} right The x coordinate of the right plane of the box.
 * @param {number} bottom The y coordinate of the bottom plane of the box.
 * @param {number} top The y coordinate of the right plane of the box.
 * @param {number} near The negative z coordinate of the near plane of the box.
 * @param {number} far The negative z coordinate of the far plane of the box.
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
mat4* mat4_frustum(mat4* dst, float left, float right, float bottom, float top, float near, float far) {
  float dx = right - left;
  float dy = top - bottom;
  float dz = far - near;

  dst->f[0] = 2 * near / dx;
  dst->f[1] = 0;
  dst->f[2] = 0;
  dst->f[3] = 0;
  dst->f[4] = 0;
  dst->f[5] = 2 * near / dy;
  dst->f[6] = 0;
  dst->f[7] = 0;
  dst->f[8] = (left + right) / dx;
  dst->f[9] = (top + bottom) / dy;
  dst->f[10] = -(far + near) / dz;
  dst->f[11] = -1;
  dst->f[12] = 0;
  dst->f[13] = 0;
  dst->f[14] = -2 * near * far / dz;
  dst->f[15] = 0;

  return dst;
}

/**
 * Makes a translation matrix
 * @param {number} tx x translation.
 * @param {number} ty y translation.
 * @param {number} tz z translation.
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
mat4* mat4_translation(mat4* dst, float tx, float ty, float tz) {
  dst = dst ? dst : (mat4*)calloc(1, sizeof(mat4));

  dst->f[0] = 1;
  dst->f[1] = 0;
  dst->f[2] = 0;
  dst->f[3] = 0;
  dst->f[4] = 0;
  dst->f[5] = 1;
  dst->f[6] = 0;
  dst->f[7] = 0;
  dst->f[8] = 0;
  dst->f[9] = 0;
  dst->f[10] = 1;
  dst->f[11] = 0;
  dst->f[12] = tx;
  dst->f[13] = ty;
  dst->f[14] = tz;
  dst->f[15] = 1;

  return dst;
}

/**
 * Mutliply by translation matrix.
 * @param {mat4} m matrix to multiply
 * @param {number} tx x translation.
 * @param {number} ty y translation.
 * @param {number} tz z translation.
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
mat4* mat4_translate(mat4* dst, const mat4* m, float tx, float ty, float tz) {
  // This is the optimized version of
  // return multiply(m, translation(tx, ty, tz), dst);
  dst = dst ? dst : (mat4*)calloc(1, sizeof(mat4));

  float m00 = m->f[0];
  float m01 = m->f[1];
  float m02 = m->f[2];
  float m03 = m->f[3];
  float m10 = m->f[1 * 4 + 0];
  float m11 = m->f[1 * 4 + 1];
  float m12 = m->f[1 * 4 + 2];
  float m13 = m->f[1 * 4 + 3];
  float m20 = m->f[2 * 4 + 0];
  float m21 = m->f[2 * 4 + 1];
  float m22 = m->f[2 * 4 + 2];
  float m23 = m->f[2 * 4 + 3];
  float m30 = m->f[3 * 4 + 0];
  float m31 = m->f[3 * 4 + 1];
  float m32 = m->f[3 * 4 + 2];
  float m33 = m->f[3 * 4 + 3];

  if (m != dst) {
    dst->f[0] = m00;
    dst->f[1] = m01;
    dst->f[2] = m02;
    dst->f[3] = m03;
    dst->f[4] = m10;
    dst->f[5] = m11;
    dst->f[6] = m12;
    dst->f[7] = m13;
    dst->f[8] = m20;
    dst->f[9] = m21;
    dst->f[10] = m22;
    dst->f[11] = m23;
  }

  dst->f[12] = m00 * tx + m10 * ty + m20 * tz + m30;
  dst->f[13] = m01 * tx + m11 * ty + m21 * tz + m31;
  dst->f[14] = m02 * tx + m12 * ty + m22 * tz + m32;
  dst->f[15] = m03 * tx + m13 * ty + m23 * tz + m33;

  return dst;
}

/**
 * Makes an x rotation matrix
 * @param {number} angleInRadians amount to rotate
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
mat4* mat4_xRotation(mat4* dst, float angleInRadians) {
  dst = dst ? dst : (mat4*)calloc(1, sizeof(mat4));
  float c = cosf(angleInRadians);
  float s = sinf(angleInRadians);

  dst->f[0] = 1;
  dst->f[1] = 0;
  dst->f[2] = 0;
  dst->f[3] = 0;
  dst->f[4] = 0;
  dst->f[5] = c;
  dst->f[6] = s;
  dst->f[7] = 0;
  dst->f[8] = 0;
  dst->f[9] = -s;
  dst->f[10] = c;
  dst->f[11] = 0;
  dst->f[12] = 0;
  dst->f[13] = 0;
  dst->f[14] = 0;
  dst->f[15] = 1;

  return dst;
}

/**
 * Multiply by an x rotation matrix
 * @param {mat4} m matrix to multiply
 * @param {number} angleInRadians amount to rotate
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
mat4* mat4_xRotate(mat4* dst, const mat4* m, float angleInRadians) {
  // this is the optimized version of
  // return multiply(m, xRotation(angleInRadians), dst);
  dst = dst ? dst : (mat4*)calloc(1, sizeof(mat4));

  float m10 = m->f[4];
  float m11 = m->f[5];
  float m12 = m->f[6];
  float m13 = m->f[7];
  float m20 = m->f[8];
  float m21 = m->f[9];
  float m22 = m->f[10];
  float m23 = m->f[11];
  float c = cosf(angleInRadians);
  float s = sinf(angleInRadians);

  dst->f[4] = c * m10 + s * m20;
  dst->f[5] = c * m11 + s * m21;
  dst->f[6] = c * m12 + s * m22;
  dst->f[7] = c * m13 + s * m23;
  dst->f[8] = c * m20 - s * m10;
  dst->f[9] = c * m21 - s * m11;
  dst->f[10] = c * m22 - s * m12;
  dst->f[11] = c * m23 - s * m13;

  if (m != dst) {
    dst->f[0] = m->f[0];
    dst->f[1] = m->f[1];
    dst->f[2] = m->f[2];
    dst->f[3] = m->f[3];
    dst->f[12] = m->f[12];
    dst->f[13] = m->f[13];
    dst->f[14] = m->f[14];
    dst->f[15] = m->f[15];
  }

  return dst;
}

/**
 * Makes an y rotation matrix
 * @param {number} angleInRadians amount to rotate
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
mat4* mat4_yRotation(mat4* dst, float angleInRadians) {
  dst = dst ? dst : (mat4*)calloc(1, sizeof(mat4));
  float c = cosf(angleInRadians);
  float s = sinf(angleInRadians);

  dst->f[0] = c;
  dst->f[1] = 0;
  dst->f[2] = -s;
  dst->f[3] = 0;
  dst->f[4] = 0;
  dst->f[5] = 1;
  dst->f[6] = 0;
  dst->f[7] = 0;
  dst->f[8] = s;
  dst->f[9] = 0;
  dst->f[10] = c;
  dst->f[11] = 0;
  dst->f[12] = 0;
  dst->f[13] = 0;
  dst->f[14] = 0;
  dst->f[15] = 1;

  return dst;
}

/**
 * Multiply by an y rotation matrix
 * @param {mat4} m matrix to multiply
 * @param {number} angleInRadians amount to rotate
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
mat4* mat4_yRotate(mat4* dst, const mat4* m, float angleInRadians) {
  // this is the optimized verison of
  // return multiply(m, yRotation(angleInRadians), dst);
  dst = dst ? dst : (mat4*)calloc(1, sizeof(mat4));

  float m00 = m->f[0 * 4 + 0];
  float m01 = m->f[0 * 4 + 1];
  float m02 = m->f[0 * 4 + 2];
  float m03 = m->f[0 * 4 + 3];
  float m20 = m->f[2 * 4 + 0];
  float m21 = m->f[2 * 4 + 1];
  float m22 = m->f[2 * 4 + 2];
  float m23 = m->f[2 * 4 + 3];
  float c = cosf(angleInRadians);
  float s = sinf(angleInRadians);

  dst->f[0] = c * m00 - s * m20;
  dst->f[1] = c * m01 - s * m21;
  dst->f[2] = c * m02 - s * m22;
  dst->f[3] = c * m03 - s * m23;
  dst->f[8] = c * m20 + s * m00;
  dst->f[9] = c * m21 + s * m01;
  dst->f[10] = c * m22 + s * m02;
  dst->f[11] = c * m23 + s * m03;

  if (m != dst) {
    dst->f[4] = m->f[4];
    dst->f[5] = m->f[5];
    dst->f[6] = m->f[6];
    dst->f[7] = m->f[7];
    dst->f[12] = m->f[12];
    dst->f[13] = m->f[13];
    dst->f[14] = m->f[14];
    dst->f[15] = m->f[15];
  }

  return dst;
}

/**
 * Makes an z rotation matrix
 * @param {number} angleInRadians amount to rotate
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
mat4* mat4_zRotation(mat4* dst, float angleInRadians) {
  dst = dst ? dst : (mat4*)calloc(1, sizeof(mat4));
  float c = cosf(angleInRadians);
  float s = sinf(angleInRadians);

  dst->f[0] = c;
  dst->f[1] = s;
  dst->f[2] = 0;
  dst->f[3] = 0;
  dst->f[4] = -s;
  dst->f[5] = c;
  dst->f[6] = 0;
  dst->f[7] = 0;
  dst->f[8] = 0;
  dst->f[9] = 0;
  dst->f[10] = 1;
  dst->f[11] = 0;
  dst->f[12] = 0;
  dst->f[13] = 0;
  dst->f[14] = 0;
  dst->f[15] = 1;

  return dst;
}

/**
 * Multiply by an z rotation matrix
 * @param {mat4} m matrix to multiply
 * @param {number} angleInRadians amount to rotate
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
mat4* mat4_zRotate(mat4* dst, const mat4* m, float angleInRadians) {
  // This is the optimized verison of
  // return multiply(m, zRotation(angleInRadians), dst);
  dst = dst ? dst : (mat4*)calloc(1, sizeof(mat4));

  float m00 = m->f[0 * 4 + 0];
  float m01 = m->f[0 * 4 + 1];
  float m02 = m->f[0 * 4 + 2];
  float m03 = m->f[0 * 4 + 3];
  float m10 = m->f[1 * 4 + 0];
  float m11 = m->f[1 * 4 + 1];
  float m12 = m->f[1 * 4 + 2];
  float m13 = m->f[1 * 4 + 3];
  float c = cosf(angleInRadians);
  float s = sinf(angleInRadians);

  dst->f[0] = c * m00 + s * m10;
  dst->f[1] = c * m01 + s * m11;
  dst->f[2] = c * m02 + s * m12;
  dst->f[3] = c * m03 + s * m13;
  dst->f[4] = c * m10 - s * m00;
  dst->f[5] = c * m11 - s * m01;
  dst->f[6] = c * m12 - s * m02;
  dst->f[7] = c * m13 - s * m03;

  if (m != dst) {
    dst->f[8] = m->f[8];
    dst->f[9] = m->f[9];
    dst->f[10] = m->f[10];
    dst->f[11] = m->f[11];
    dst->f[12] = m->f[12];
    dst->f[13] = m->f[13];
    dst->f[14] = m->f[14];
    dst->f[15] = m->f[15];
  }

  return dst;
}

/**
 * Makes an rotation matrix around an arbitrary axis
 * @param {vec3} axis axis to rotate around
 * @param {number} angleInRadians amount to rotate
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
mat4* mat4_axisRotation(mat4* dst, const vec3* axis, float angleInRadians) {
  dst = dst ? dst : (mat4*)calloc(1, sizeof(mat4));

  float x = axis->f[0];
  float y = axis->f[1];
  float z = axis->f[2];
  float n = sqrtf(x * x + y * y + z * z);
  x /= n;
  y /= n;
  z /= n;
  float xx = x * x;
  float yy = y * y;
  float zz = z * z;
  float c = cosf(angleInRadians);
  float s = sinf(angleInRadians);
  float oneMinusCosine = 1 - c;

  dst->f[0] = xx + (1 - xx) * c;
  dst->f[1] = x * y * oneMinusCosine + z * s;
  dst->f[2] = x * z * oneMinusCosine - y * s;
  dst->f[3] = 0;
  dst->f[4] = x * y * oneMinusCosine - z * s;
  dst->f[5] = yy + (1 - yy) * c;
  dst->f[6] = y * z * oneMinusCosine + x * s;
  dst->f[7] = 0;
  dst->f[8] = x * z * oneMinusCosine + y * s;
  dst->f[9] = y * z * oneMinusCosine - x * s;
  dst->f[10] = zz + (1 - zz) * c;
  dst->f[11] = 0;
  dst->f[12] = 0;
  dst->f[13] = 0;
  dst->f[14] = 0;
  dst->f[15] = 1;

  return dst;
}

/**
 * Multiply by an axis rotation matrix
 * @param {mat4} m matrix to multiply
 * @param {vec3} axis axis to rotate around
 * @param {number} angleInRadians amount to rotate
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
mat4* mat4_axisRotate(mat4* dst, const mat4* m, const vec3* axis, float angleInRadians) {
  // This is the optimized verison of
  // return multiply(m, axisRotation(axis, angleInRadians), dst);
  dst = dst ? dst : (mat4*)calloc(1, sizeof(mat4));

  float x = axis->f[0];
  float y = axis->f[1];
  float z = axis->f[2];
  float n = sqrtf(x * x + y * y + z * z);
  x /= n;
  y /= n;
  z /= n;
  float xx = x * x;
  float yy = y * y;
  float zz = z * z;
  float c = cosf(angleInRadians);
  float s = sinf(angleInRadians);
  float oneMinusCosine = 1 - c;

  float r00 = xx + (1 - xx) * c;
  float r01 = x * y * oneMinusCosine + z * s;
  float r02 = x * z * oneMinusCosine - y * s;
  float r10 = x * y * oneMinusCosine - z * s;
  float r11 = yy + (1 - yy) * c;
  float r12 = y * z * oneMinusCosine + x * s;
  float r20 = x * z * oneMinusCosine + y * s;
  float r21 = y * z * oneMinusCosine - x * s;
  float r22 = zz + (1 - zz) * c;

  float m00 = m->f[0];
  float m01 = m->f[1];
  float m02 = m->f[2];
  float m03 = m->f[3];
  float m10 = m->f[4];
  float m11 = m->f[5];
  float m12 = m->f[6];
  float m13 = m->f[7];
  float m20 = m->f[8];
  float m21 = m->f[9];
  float m22 = m->f[10];
  float m23 = m->f[11];

  dst->f[0] = r00 * m00 + r01 * m10 + r02 * m20;
  dst->f[1] = r00 * m01 + r01 * m11 + r02 * m21;
  dst->f[2] = r00 * m02 + r01 * m12 + r02 * m22;
  dst->f[3] = r00 * m03 + r01 * m13 + r02 * m23;
  dst->f[4] = r10 * m00 + r11 * m10 + r12 * m20;
  dst->f[5] = r10 * m01 + r11 * m11 + r12 * m21;
  dst->f[6] = r10 * m02 + r11 * m12 + r12 * m22;
  dst->f[7] = r10 * m03 + r11 * m13 + r12 * m23;
  dst->f[8] = r20 * m00 + r21 * m10 + r22 * m20;
  dst->f[9] = r20 * m01 + r21 * m11 + r22 * m21;
  dst->f[10] = r20 * m02 + r21 * m12 + r22 * m22;
  dst->f[11] = r20 * m03 + r21 * m13 + r22 * m23;

  if (m != dst) {
    dst->f[12] = m->f[12];
    dst->f[13] = m->f[13];
    dst->f[14] = m->f[14];
    dst->f[15] = m->f[15];
  }

  return dst;
}

/**
 * Makes a scale matrix
 * @param {number} sx x scale.
 * @param {number} sy y scale.
 * @param {number} sz z scale.
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
mat4* mat4_scaling(mat4* dst, float sx, float sy, float sz) {
  dst = dst ? dst : (mat4*)calloc(1, sizeof(mat4));

  dst->f[0] = sx;
  dst->f[1] = 0;
  dst->f[2] = 0;
  dst->f[3] = 0;
  dst->f[4] = 0;
  dst->f[5] = sy;
  dst->f[6] = 0;
  dst->f[7] = 0;
  dst->f[8] = 0;
  dst->f[9] = 0;
  dst->f[10] = sz;
  dst->f[11] = 0;
  dst->f[12] = 0;
  dst->f[13] = 0;
  dst->f[14] = 0;
  dst->f[15] = 1;

  return dst;
}

/**
 * Multiply by a scaling matrix
 * @param {mat4} m matrix to multiply
 * @param {number} sx x scale.
 * @param {number} sy y scale.
 * @param {number} sz z scale.
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
mat4* mat4_scale(mat4* dst, const mat4* m, float sx, float sy, float sz) {
  // This is the optimized verison of
  // return multiply(m, scaling(sx, sy, sz), dst);
  dst = dst ? dst : (mat4*)calloc(1, sizeof(mat4));

  dst->f[0] = sx * m->f[0 * 4 + 0];
  dst->f[1] = sx * m->f[0 * 4 + 1];
  dst->f[2] = sx * m->f[0 * 4 + 2];
  dst->f[3] = sx * m->f[0 * 4 + 3];
  dst->f[4] = sy * m->f[1 * 4 + 0];
  dst->f[5] = sy * m->f[1 * 4 + 1];
  dst->f[6] = sy * m->f[1 * 4 + 2];
  dst->f[7] = sy * m->f[1 * 4 + 3];
  dst->f[8] = sz * m->f[2 * 4 + 0];
  dst->f[9] = sz * m->f[2 * 4 + 1];
  dst->f[10] = sz * m->f[2 * 4 + 2];
  dst->f[11] = sz * m->f[2 * 4 + 3];

  if (m != dst) {
    dst->f[12] = m->f[12];
    dst->f[13] = m->f[13];
    dst->f[14] = m->f[14];
    dst->f[15] = m->f[15];
  }

  return dst;
}

/**
 * Computes the inverse of a matrix.
 * @param {mat4} m matrix to compute inverse of
 * @param {mat4} [dst] optional matrix to store result
 * @return {mat4} dst or a new matrix if none provided
 */
mat4* mat4_inverse(mat4* dst, const mat4* m) {
  dst = dst ? dst : (mat4*)calloc(1, sizeof(mat4));
  float m00 = m->f[0 * 4 + 0];
  float m01 = m->f[0 * 4 + 1];
  float m02 = m->f[0 * 4 + 2];
  float m03 = m->f[0 * 4 + 3];
  float m10 = m->f[1 * 4 + 0];
  float m11 = m->f[1 * 4 + 1];
  float m12 = m->f[1 * 4 + 2];
  float m13 = m->f[1 * 4 + 3];
  float m20 = m->f[2 * 4 + 0];
  float m21 = m->f[2 * 4 + 1];
  float m22 = m->f[2 * 4 + 2];
  float m23 = m->f[2 * 4 + 3];
  float m30 = m->f[3 * 4 + 0];
  float m31 = m->f[3 * 4 + 1];
  float m32 = m->f[3 * 4 + 2];
  float m33 = m->f[3 * 4 + 3];
  float tmp_0 = m22 * m33;
  float tmp_1 = m32 * m23;
  float tmp_2 = m12 * m33;
  float tmp_3 = m32 * m13;
  float tmp_4 = m12 * m23;
  float tmp_5 = m22 * m13;
  float tmp_6 = m02 * m33;
  float tmp_7 = m32 * m03;
  float tmp_8 = m02 * m23;
  float tmp_9 = m22 * m03;
  float tmp_10 = m02 * m13;
  float tmp_11 = m12 * m03;
  float tmp_12 = m20 * m31;
  float tmp_13 = m30 * m21;
  float tmp_14 = m10 * m31;
  float tmp_15 = m30 * m11;
  float tmp_16 = m10 * m21;
  float tmp_17 = m20 * m11;
  float tmp_18 = m00 * m31;
  float tmp_19 = m30 * m01;
  float tmp_20 = m00 * m21;
  float tmp_21 = m20 * m01;
  float tmp_22 = m00 * m11;
  float tmp_23 = m10 * m01;

  float t0 = (tmp_0 * m11 + tmp_3 * m21 + tmp_4  * m31) - (tmp_1 * m11 + tmp_2 * m21 + tmp_5  * m31);
  float t1 = (tmp_1 * m01 + tmp_6 * m21 + tmp_9  * m31) - (tmp_0 * m01 + tmp_7 * m21 + tmp_8  * m31);
  float t2 = (tmp_2 * m01 + tmp_7 * m11 + tmp_10 * m31) - (tmp_3 * m01 + tmp_6 * m11 + tmp_11 * m31);
  float t3 = (tmp_5 * m01 + tmp_8 * m11 + tmp_11 * m21) - (tmp_4 * m01 + tmp_9 * m11 + tmp_10 * m21);

  float d = 1.0 / (m00 * t0 + m10 * t1 + m20 * t2 + m30 * t3);

  dst->f[ 0] = d * t0;
  dst->f[ 1] = d * t1;
  dst->f[ 2] = d * t2;
  dst->f[ 3] = d * t3;
  dst->f[ 4] = d * ((tmp_1  * m10 + tmp_2  * m20 + tmp_5  * m30) - (tmp_0  * m10 + tmp_3  * m20 + tmp_4  * m30));
  dst->f[ 5] = d * ((tmp_0  * m00 + tmp_7  * m20 + tmp_8  * m30) - (tmp_1  * m00 + tmp_6  * m20 + tmp_9  * m30));
  dst->f[ 6] = d * ((tmp_3  * m00 + tmp_6  * m10 + tmp_11 * m30) - (tmp_2  * m00 + tmp_7  * m10 + tmp_10 * m30));
  dst->f[ 7] = d * ((tmp_4  * m00 + tmp_9  * m10 + tmp_10 * m20) - (tmp_5  * m00 + tmp_8  * m10 + tmp_11 * m20));
  dst->f[ 8] = d * ((tmp_12 * m13 + tmp_15 * m23 + tmp_16 * m33) - (tmp_13 * m13 + tmp_14 * m23 + tmp_17 * m33));
  dst->f[ 9] = d * ((tmp_13 * m03 + tmp_18 * m23 + tmp_21 * m33) - (tmp_12 * m03 + tmp_19 * m23 + tmp_20 * m33));
  dst->f[10] = d * ((tmp_14 * m03 + tmp_19 * m13 + tmp_22 * m33) - (tmp_15 * m03 + tmp_18 * m13 + tmp_23 * m33));
  dst->f[11] = d * ((tmp_17 * m03 + tmp_20 * m13 + tmp_23 * m23) - (tmp_16 * m03 + tmp_21 * m13 + tmp_22 * m23));
  dst->f[12] = d * ((tmp_14 * m22 + tmp_17 * m32 + tmp_13 * m12) - (tmp_16 * m32 + tmp_12 * m12 + tmp_15 * m22));
  dst->f[13] = d * ((tmp_20 * m32 + tmp_12 * m02 + tmp_19 * m22) - (tmp_18 * m22 + tmp_21 * m32 + tmp_13 * m02));
  dst->f[14] = d * ((tmp_18 * m12 + tmp_23 * m32 + tmp_15 * m02) - (tmp_22 * m32 + tmp_14 * m02 + tmp_19 * m12));
  dst->f[15] = d * ((tmp_22 * m22 + tmp_16 * m02 + tmp_21 * m12) - (tmp_20 * m12 + tmp_23 * m22 + tmp_17 * m02));

  return dst;
}

/**
 * Takes a  matrix and a vector with 4 entries, transforms that vector by
 * the matrix, and returns the result as a vector with 4 entries.
 * @param {mat4} m The matrix.
 * @param {vec4} v The point in homogenous coordinates.
 * @param {vec4} dst optional vec4 to store result
 * @return {vec4} dst or new vec4 if not provided
 */
vec4* vec4_transformVector(vec4* dst, const mat4* m, const vec4* v) {
  dst = dst ? dst : (vec4*)calloc(1, sizeof(vec4));
  for (int i = 0; i < 4; ++i) {
    dst->f[i] = 0.0;
    for (int j = 0; j < 4; ++j) {
      dst->f[i] += v->f[j] * m->f[j * 4 + i];
    }
  }
  return dst;
}

/**
 * Takes a 4-by-4 matrix and a vector with 3 entries,
 * interprets the vector as a point, transforms that point by the matrix, and
 * returns the result as a vector with 3 entries.
 * @param {mat4} m The matrix.
 * @param {vec3} v The point.
 * @param {vec4} dst optional vec4 to store result
 * @return {vec4} dst or new vec4 if not provided
 */
vec4* vec4_transformPoint(vec4* dst, const mat4* m, const vec4* v) {
  dst = dst ? dst : (vec4*)calloc(1, sizeof(vec4));
  float v0 = v->f[0];
  float v1 = v->f[1];
  float v2 = v->f[2];
  float d = v0 * m->f[0 * 4 + 3] + v1 * m->f[1 * 4 + 3] + v2 * m->f[2 * 4 + 3] + m->f[3 * 4 + 3];

  dst->f[0] = (v0 * m->f[0 * 4 + 0] + v1 * m->f[1 * 4 + 0] + v2 * m->f[2 * 4 + 0] + m->f[3 * 4 + 0]) / d;
  dst->f[1] = (v0 * m->f[0 * 4 + 1] + v1 * m->f[1 * 4 + 1] + v2 * m->f[2 * 4 + 1] + m->f[3 * 4 + 1]) / d;
  dst->f[2] = (v0 * m->f[0 * 4 + 2] + v1 * m->f[1 * 4 + 2] + v2 * m->f[2 * 4 + 2] + m->f[3 * 4 + 2]) / d;

  return dst;
}

/**
 * Takes a 4-by-4 matrix and a vector with 3 entries, interprets the vector as a
 * direction, transforms that direction by the matrix, and returns the result;
 * assumes the transformation of 3-dimensional space represented by the matrix
 * is parallel-preserving, i.e. any combination of rotation, scaling and
 * translation, but not a perspective distortion. Returns a vector with 3
 * entries.
 * @param {mat4} m The matrix.
 * @param {vec3} v The direction.
 * @param {vec4} dst optional vec4 to store result
 * @return {vec4} dst or new vec4 if not provided
 */
vec4* vec4_transformDirection(vec4* dst, const mat4* m, const vec3* v) {
  dst = dst ? dst : (vec4*)calloc(1, sizeof(vec4));

  float v0 = v->f[0];
  float v1 = v->f[1];
  float v2 = v->f[2];

  dst->f[0] = v0 * m->f[0 * 4 + 0] + v1 * m->f[1 * 4 + 0] + v2 * m->f[2 * 4 + 0];
  dst->f[1] = v0 * m->f[0 * 4 + 1] + v1 * m->f[1 * 4 + 1] + v2 * m->f[2 * 4 + 1];
  dst->f[2] = v0 * m->f[0 * 4 + 2] + v1 * m->f[1 * 4 + 2] + v2 * m->f[2 * 4 + 2];

  return dst;
}

/**
 * Takes a 4-by-4 matrix m and a vector v with 3 entries, interprets the vector
 * as a normal to a surface, and computes a vector which is normal upon
 * transforming that surface by the matrix. The effect of this function is the
 * same as transforming v (as a direction) by the inverse-transpose of m.  This
 * function assumes the transformation of 3-dimensional space represented by the
 * matrix is parallel-preserving, i.e. any combination of rotation, scaling and
 * translation, but not a perspective distortion.  Returns a vector with 3
 * entries.
 * @param {mat4} m The matrix.
 * @param {vec3} v The normal.
 * @param {vec3} [dst] The direction.
 * @return {vec3} The transformed direction.
 */
vec3* vec3_transformNormal(vec3* dst, const mat4* m, const vec3* v) {
  dst = dst ? dst : (vec3*)calloc(1, sizeof(vec3));
  mat4 mi;
  mat4_inverse(&mi, m);
  float v0 = v->f[0];
  float v1 = v->f[1];
  float v2 = v->f[2];

  dst->f[0] = v0 * mi.f[0 * 4 + 0] + v1 * mi.f[0 * 4 + 1] + v2 * mi.f[0 * 4 + 2];
  dst->f[1] = v0 * mi.f[1 * 4 + 0] + v1 * mi.f[1 * 4 + 1] + v2 * mi.f[1 * 4 + 2];
  dst->f[2] = v0 * mi.f[2 * 4 + 0] + v1 * mi.f[2 * 4 + 1] + v2 * mi.f[2 * 4 + 2];

  return dst;
}

mat4* mat4_copy(mat4* dst, const mat4* src) {
  dst = dst ? dst : (mat4*)calloc(1, sizeof(mat4));

  dst->f[0] = src->f[0];
  dst->f[1] = src->f[1];
  dst->f[2] = src->f[2];
  dst->f[3] = src->f[3];
  dst->f[4] = src->f[4];
  dst->f[5] = src->f[5];
  dst->f[6] = src->f[6];
  dst->f[7] = src->f[7];
  dst->f[8] = src->f[8];
  dst->f[9] = src->f[9];
  dst->f[10] = src->f[10];
  dst->f[11] = src->f[11];
  dst->f[12] = src->f[12];
  dst->f[13] = src->f[13];
  dst->f[14] = src->f[14];
  dst->f[15] = src->f[15];

  return dst;
}

mat3 * mat4_to_mat3(mat3 *dst, const mat4*src) {
  dst = dst ? dst : (mat3*)calloc(1, sizeof(mat3));
  dst->f[0] = src->f[0];
  dst->f[1] = src->f[1];
  dst->f[2] = src->f[2];
  dst->f[3] = src->f[4];
  dst->f[4] = src->f[5];
  dst->f[5] = src->f[6];
  dst->f[6] = src->f[8];
  dst->f[7] = src->f[9];
  dst->f[8] = src->f[10];
  return dst;
}


static double mat3_det(const mat3 *m)
{
    // a11 a22 a33 + a21 a32 a13 + a31 a12 a23 - a11 a32 a23 - a31 a22 a13 - a21 a12 a33
    return m->f[0] * m->f[4] * m->f[8] + m->f[3] * m->f[7] * m->f[2] + m->f[6] * m->f[1] * m->f[5]
          -m->f[0] * m->f[7] * m->f[5] - m->f[6] * m->f[4] * m->f[2] - m->f[3] * m->f[1] * m->f[8];
}
 

mat3 *mat3_invert(mat3 *dst, const mat3 *m)
{
    double det = mat3_det(m);
    if(fabs(det) < FLT_EPSILON){
        return NULL;
    }
    else{
        dst = dst ? dst : (mat3*)calloc(1, sizeof(mat3));
        double inv_det = 1.0/det;
 
        dst->f[0] = inv_det*(m->f[4]*m->f[8]-m->f[5]*m->f[7]);
        dst->f[1] = inv_det*(m->f[2]*m->f[7]-m->f[1]*m->f[8]);
        dst->f[2] = inv_det*(m->f[1]*m->f[5]-m->f[2]*m->f[4]);
 
        dst->f[3] = inv_det*(m->f[5]*m->f[6]-m->f[3]*m->f[8]);
        dst->f[4] = inv_det*(m->f[0]*m->f[8]-m->f[2]*m->f[6]);
        dst->f[5] = inv_det*(m->f[2]*m->f[3]-m->f[0]*m->f[5]);
 
        dst->f[6] = inv_det*(m->f[3]*m->f[7]-m->f[4]*m->f[6]);
        dst->f[7] = inv_det*(m->f[1]*m->f[6]-m->f[0]*m->f[7]);
        dst->f[8] = inv_det*(m->f[0]*m->f[4]-m->f[1]*m->f[3]);
 
        return dst;
    }
}

mat3 *mat3_transpose(mat3 *dst, const mat3 *m)
{
  dst = dst ? dst : (mat3*)calloc(1, sizeof(mat3));

  dst->f[0] = m->f[0];
  dst->f[1] = m->f[3];
  dst->f[2] = m->f[6];

  dst->f[3] = m->f[1];
  dst->f[4] = m->f[4];
  dst->f[5] = m->f[7];

  dst->f[6] = m->f[2];
  dst->f[7] = m->f[5];
  dst->f[8] = m->f[8];

  return dst;
}

#endif
