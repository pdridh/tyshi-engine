#pragma once

#include <cassert>
#include <cmath>
#include <iostream>
#include <type_traits>

#include "Core.h"

/**
 * @brief    Represents 2D vectors supporting floating point and integral types
 *
 * Provides common vector math operations: magnitude, normalization, distance between vectors,
 * scaling by scalar, sum of vectors, difference of vectors.
 *
 *
 * Provides both mutating and non-mutating variation of the operations.
 *
 * Mutating variations (eg. @ref scale , @ref normalize) modify the vector in place and return
 * `*this` for chaining, Non-mutating variations (eg. @ref getScaledDown , @ref getNormalized)
 * return a new vector.
 *
 *
 * @note Only instantiable with floating point or integral types, asserted at compile time.
 * @note Floating point comparisions use @ref DIFFERENCE_TOLERATION as an epsilon to account for
 * precision errors
 *
 * @tparam T Numeric type, must be floating point or integral
 */
template <typename T>
class Vec2
{
public:
    T x;  ///< Horizontal component
    T y;  ///< Vertical component

    /// Epsilon used for floating point equality comparision
    static constexpr f32 DIFFERENCE_TOLERATION = 1e-6;

    /**
     * @brief    Constructs a Vec2 with the given components
     *
     * @note Only works for floating point types and integral types, uses `static_assert` to check
     * if the type is valid
     *
     * @param    x_arg                         Horizontal component, defaults to 0
     * @param    y_arg                         Vertical component, also defaults to 0
     */
    Vec2<T>(T x_arg = 0, T y_arg = 0) : x(x_arg), y(y_arg)
    {
        static_assert(std::is_floating_point_v<T> || std::is_integral_v<T>,
                      "Vec2 only available for floats and ints");
    }

    /**
     * @brief    Constructs a Vec2 by converting from another Vec2 template type
     *
     * Components are cast from U to T using `static_cast`
     *
     * @tparam   U                             Source numeric type
     * @param    other                         Vec2 to copy and convert from
     */
    template <typename U>
    Vec2<T>(const Vec2<U>& other) : x{static_cast<T>(other.x)}, y{static_cast<T>(other.y)}
    {
    }

    /**
     * @brief    Constructs a Vec2 by converting from a pointer to another Vec2 template type
     *
     * Components are cast from U to T using `static_cast`.
     *
     * @tparam   U                             Source numeric type
     * @param    other                         Pointer to Vec2 to copy and convert from
     */
    template <typename U>
    Vec2<T>(const Vec2<U>* other) : x{static_cast<T>(other->x)}, y{static_cast<T>(other->y)}
    {
    }

    /**
     * @brief    Returns a vector (0, 0)
     *
     * @return   Vec2<T>                       (0, 0) vector
     */
    inline static Vec2<T> ZERO() { return Vec2<T>(0, 0); }

    /**
     * @brief    Returns a vector (1, 1)
     *
     * @return   Vec2<T>                       (1, 1) vector
     */
    inline static Vec2<T> ONE() { return Vec2<T>(1, 1); }

    /**
     * @brief    Returns the components as an `std::pair`
     *
     * @return   std::pair<T, T>               Components as (x, y)
     */
    inline std::pair<T, T> pair() const { return {x, y}; }

    /**
     * @brief    Prints the vector to stdout with a tag prefix
     *
     * Prints with a `{Vec2f}` prefix for floating point or `{Vec2i}` prefix for integral types.
     *
     * @todo idk maybe an option for choosing the stream to print to instead of just stdout
     *
     * @param    tag                           Label to prefix the output with
     */
    inline void print(const char* tag) const
    {
        if (std::is_floating_point_v<T>)
        {
            printf("{Vec2f} %s: %f, %f\n", tag, x, y);
        }
        else
        {
            printf("{Vec2i} %s: %d, %d\n", tag, x, y);
        }
    }

    /**
     * @brief    Normalizes this vector
     *
     * Divides both components of this vector by the magnitude of this vector. No-op if both
     * components are 0.
     *
     * Uses @ref magnitude internally to get the magnitude of this vector for dividing the
     * components.
     *
     * @warning Modifies this vector, use @ref getNormalized to get a normalized copy of this vector
     * without modifying this vector
     *
     * @note Only works with floating point types as normalizing an integer vector does not make
     * sense (truncates to 0), asserted in compile time
     *
     * @return   Vec2<T>&                      Reference to this vector for chaining
     */
    inline Vec2<T>& normalize()
    {
        static_assert(std::is_floating_point_v<T>, "Vec2::normalize only works with floats");

        if (x || y)
        {
            f32 mag = magnitude();

            x /= mag;
            y /= mag;
        }

        return *this;
    }

    /**
     * @brief    Returns a normalized copy of this vector
     *
     *
     * Divides both components of the copy vector by its magnitude. No-op if both components are 0.
     *
     * Calls @ref normalize internally on the copy.
     *
     * @note Does not modify this vector, use @ref normalize to normalize this vector
     * @note Only works with floating point types as normalizing an integer vector does not make
     * sense (truncates to 0), asserted in compile time
     *
     * @return   Vec2<T>                       Normalized copy of this vector
     */
    inline Vec2<T> getNormalized() const
    {
        static_assert(std::is_floating_point_v<T>, "Vec2::getNormalized only works with floats");
        return Vec2<T>(*this).normalize();
    }

    /**
     * @brief    Returns the magnitude of this vector
     *
     * Slow because it calls `std::sqrt` internally, use @ref magnitudeSquared if the actual
     * magnitude is not required.
     *
     * @return   f32                           Magnitude of this vector
     */
    inline f32 magnitude() const { return std::sqrt(x * x + y * y); }

    /**
     * @brief    Returns the squared magnitude of this vector
     *
     * Faster than @ref magnitude because no `std::sqrt` call.
     * Use this for distance comparision where the actual magnitude is not required.
     *
     * @return   f32                           Squared magnitude of this vector
     */
    inline f32 magnitudeSquared() const { return (x * x) + (y * y); }

    /**
     * @brief    Returns the distance to the target vector
     *
     * Slow because it calls `std::sqrt`, use @ref distanceToSquared if the actual distance is not
     * required.
     *
     * Calls @ref magnitude internally.
     *
     * @param    target                        Vector to measure distance to
     * @return   f32                           Distance between the two vectors
     */
    inline f32 distanceTo(const Vec2<T>& target) const { return (*this - target).magnitude(); }

    /**
     * @brief    Returns the squared distance to the target vector
     *
     * Faster than @ref distanceTo because no `std::sqrt` call.
     * Use this for distance comparisions where the actual distance is not required.
     *
     * Calls @ref magnitudeSquared internally.
     *
     * @param    target                        Vector to measure distance to
     * @return   f32                           Squared distance between the two vectors
     */
    inline f32 distanceToSquared(const Vec2<T>& target) const
    {
        return (*this - target).magnitudeSquared();
    }

    /**
     * @brief    Multiplies the components of this vector by a factor
     *
     * @warning Modifies this vector, use @ref getScaled to do this operation without modifying this
     * vector.
     *
     * @param    factor                        Scalar to multiply the components by
     * @return   Vec2<T>&                      Reference to this vector for chaining
     */
    inline Vec2<T>& scale(T factor)
    {
        x *= factor;
        y *= factor;

        return *this;
    };

    /**
     * @brief    Divides the components of this vector by a factor
     *
     * Asserts if the factor is 0. IT MUST NOT BE 0.
     *
     * Casts to T before dividing so that integer divisions are only done for integers.
     *
     * @warning Modifies this vector, use @ref getScaledDown to do this operation without modifying
     * this vector
     *
     * @param    factor                        Scalar to divide the components by, must not be 0
     * @return   Vec2<T>&                      Reference to this vector for chaining
     */
    inline Vec2<T>& scaleDown(T factor)
    {
        assert(factor != 0 && "Vec2::scaleDown Cant scale down by 0, (dividing by 0)");
        return this->scale(static_cast<T>(1) / factor);
    }

    /**
     * @brief    Negates this vector
     *
     * @warning Modifies this vector, use @ref operator- on this vector to do this operation without
     * modifying it
     *
     * @return   Vec2<T>&                      Reference to this vector for chaining
     */
    inline Vec2<T>& negate()
    {
        this = -this;
        return *this;
    }

    /**
     * @brief    Returns a copy of this vector whose components are multiplied by the factor
     *
     * Uses @ref scale internally on the copy.
     *
     * @note Does not modify this vector, use @ref scale to multiply the components of this vector
     *
     * @param    factor                        Scalar to multiply both components by
     * @return   Vec2<T>                       Copy of this vector whose components are multiplied
     * by the factor
     */
    inline Vec2<T> getScaled(T factor) const { return Vec2<T>(*this).scale(factor); }

    /**
     * @brief    Returns a copy of this vector whose components are divided by the factor
     *
     * Uses @ref scaleDown internally on the copy.
     *
     * @note Does not modify this vector, use @ref scaleDown to divide the components of this vector
     *
     * @warning The factor must not be 0 (Divide by 0), asserts if it is 0
     *
     *
     * @param    factor                        Scalar to divide both components by, must not be 0
     * @return   Vec2<T>                       Copy of this vector whose components are divided by
     * the factor
     */
    inline Vec2<T> getScaledDown(T factor) const { return Vec2<T>(*this).scaleDown(factor); }

    // Operations

    /**
     * @brief    Returns a new vector that is the sum of this vector and the `right` vector
     *
     * @note Does not modify this vector
     *
     * @param    right                         The vector to the right of the operation
     * @return   Vec2<T>                       A new vector that is the sum of this vector and the
     * `right` vector
     */
    inline Vec2<T> operator+(Vec2 const& right) const { return Vec2<T>(x + right.x, y + right.y); }

    /**
     * @brief    Returns a new vector that is the difference of this vector and the `right` vector
     *
     * @note Does not modify this vector
     *
     * @param    right                         The vector to the right of the operation
     * @return   Vec2                          A new vector that is the difference of this vector
     * and the `right` vector
     */
    inline Vec2<T> operator-(Vec2<T> const& right) const
    {
        return Vec2<T>(x - right.x, y - right.y);
    }

    /**
     * @brief    Returns the negation of this vector
     *
     * @note Does not modify this vector. use @ref negate to negate this vector.
     *
     * @return   Vec2<T>                       A new vector that is the negation of this vector
     */
    inline Vec2<T> operator-() const { return Vec2<T>(-x, -y); }

    /**
     * @brief    Adds the given vector to this vector
     *
     * @warning Modifies this vector
     *
     * @param    right                         The vector to the right of the operation
     * @return   Vec2<T>&                      Reference to this vector for chaining
     *
     */
    inline Vec2<T>& operator+=(Vec2<T> const& right)
    {
        x += right.x;
        y += right.y;
        return *this;
    }

    /**
     * @brief    Subtracts the given vector from this vector
     *
     * @warning Modifies this vector
     *
     * @param    right                         The vector to the right of the operation
     * @return   Vec2<T>&                      Reference to this vector for chaining
     */
    inline Vec2<T>& operator-=(Vec2 const& right)
    {
        x -= right.x;
        y -= right.y;
        return *this;
    }

    /**
     * @brief    Compares two vectors for equality
     *
     *  For floating point types, uses an epsilon @ref DIFFERENCE_TOLERATION
     *  to account for floating point precision errors.
     *
     * @note Does not modify this vector
     *
     * @param    right                         Vector to compare this vector against
     * @return   true                          If both the vector components are equal
     * @return   false                         If any of the components are not equal
     */
    inline bool operator==(Vec2<T> const& right) const
    {
        // Floating comparision
        if (std::is_floating_point_v<T>)
        {
            f32 deltaX = std::fabs(x - right.x);
            f32 deltaY = std::fabs(y - right.y);

            return deltaX < DIFFERENCE_TOLERATION && deltaY < DIFFERENCE_TOLERATION;
        }

        return (x == right.x) && (y == right.y);
    }
};

typedef Vec2<f32> Vec2f;  ///< Floating point 2D vector for easy access
typedef Vec2<i32> Vec2i;  ///< Integer 2D vector for easy access