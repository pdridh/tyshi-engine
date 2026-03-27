#pragma once

#include <algorithm>
#include <array>
#include <random>

#include "Engine/Core.h"
#include "Engine/Vec2.h"

constexpr i32 PERMUTATION_TABLE_SIZE =
    256;  ///< Size of the permutation table, HAS TO BE power of 2
constexpr i32 PERMUTATION_TABLE_ARRAY_SIZE =
    PERMUTATION_TABLE_SIZE *
    2;  ///< Size of the array storing the permutation table twice (accounting for overflow)
constexpr i32 PERMUTATION_MASK =
    PERMUTATION_TABLE_SIZE - 1;  ///< Mask used for & to wrap values to PERMUTATION_TABLE_SIZE

/**
 * @brief Generates smooth 2D gradient noise using the Improved Perlin Noise algorithm
 *
 *
 * First of all, I need randomness for the world generation but I can't really do it fully random,
 * that would make the world look asss.
 *
 * I could define multiple parts of the world and use randomness and chance to grow from that. This
 * is a problem with the infinite world I need for Tyshi2 though, because this would have to save
 * all of the generation or calculate the generation from the defined points or areas or whatever.
 *
 * Therefore, I need something to control the randomness, to make it look natural.
 *
 * ENTER PERLINNN
 *
 * Perlin noise is basically this:
 *
 * 1) Create a permutation table and shuffle it randomly.
 *
 * 2) Get the unit square that the point is in using \c std::floor.
 *
 * 3) Wrap the value so that it is under permutation table size, & is used instead of % for
 * optimization. The caveat is that & only works if permutation table size is a power of 2.
 *
 * 4) Get the relative position of the point inside the cell by subtracting the unit square's origin
 * with P(x, y).
 *
 * 5) Hash each corner's value using the created permutation table.
 *
 * 6) Get a gradient vector for each corner using their hash value (see @ref grad).
 *
 * 7) Get vector from each corner to the point.
 *
 * 8) Get dot product of gradient vector and vector to the point for each corner, this basically
 * calculates the "influence" each corner gradient vectors have to the point.
 *
 * 9) Apply @ref fade to the relative position of the point inside the unit square to get, this
 * makes the values smooth.
 *
 * 10) Linear interpolate (@ref lerp) the dot products of the bottom corners (at y = 0 ) using
 * fade value for x.
 *
 * 11) Linear interpolate (@ref lerp) the dot products of the top corners (at y = 1)
 * using fade value for x.
 *
 * 12) Finally, linear interpolate (@ref lerp) result values vertically using fade value for y.
 *
 *
 * @warning @ref PERMUTATION_TABLE_SIZE has to be in power of 2
 *
 */
class PerlinNoise2D
{
public:
    /**
     * @brief    Initializes and shuffle the permutation table using the seed
     *
     * Uses \c std::mt19937_64 (mersenne twister).
     *
     * @param    seed                          Seed for the rng
     */
    PerlinNoise2D(u64 seed);

    /**
     * @brief    Returns the noise value for the point P(x, y)
     *
     * Process of noise calculation:
     *
     * 1) Gets the unit square that the point is in using \c std::floor then wraps the value so that
     * it is inside @ref m_permutationTable. The wrapping is done via & instead of % (FASTER) so it
     * only works if the permutation table size is a power of 2.
     *
     * 2) Calculates the relative position of the point inside the unit square. This is used later
     * for getting the influence of each gradient vector to the point.
     *
     * 3) Gets the hash value for each of the four corners using @ref m_permutationTable, this value
     * is used for getting the gradient vector for the corner.
     *
     * 4) Uses @ref grad to calculate the dot product of the gradient vector and the vector to the
     * point of each corner, this gives the influence each gradient vector has on the point.
     *
     * 5) Uses @ref fade to get a smooth value for the x, y relative to the unit square.
     *
     * 6) Linear interpolates (@ref lerp) the dot products of the bottom corners (at y = 0) using
     * fade value for x.
     *
     * 7) Linear interpolate (@ref lerp) the dot products of the top corners (at y = 1)
     * using fade value for x.
     *
     * 8) Finally, linear interpolates (@ref lerp) result values vertically using fade value for y.
     *
     * @param    x                             x coordinate of the point to query
     * @param    y                             y coordinate of the point to query
     * @return   f32                           Noise value for P(x, y)
     */
    f32 noise(f32 x, f32 y) const;

private:
    /**
     * @brief    Produces smooth values for interpolation
     *
     * Uses the equation: 6t⁵ − 15t⁴ + 10t³, this is from Ken Perlin's improved perlin noise, Perlin
     * used this equation because it has zero first and second derivatives at both t=0 and t=1.
     *
     * Meaning at the edge points of the unit square (where x and y are either 0 or 1 ): the slope
     * is flat (1st derivative) and the curvature is also flat (2nd derivative)
     *
     * This assures that the transition from one unit square grid cell to another is smooth.
     *
     * @param    t                             x or y relative to the unit square cell
     * @return   f32                           Smooth
     */
    static f32 fade(f32 t);

    /**
     * @brief    Linearly interpolates between two values using a weight t
     *
     * @param    a                             Start value
     * @param    b                             End value
     * @param    t                             Interpolation factor
     * @return   f32                           Value between a and b weighted by t
     */
    static f32 lerp(f32 a, f32 b, f32 t);

    /**
     * @brief    Returns the dot product of x and y with the gradient vector depending on hash
     *
     *
     * Uses the hash to get one of 8 gradient vectors and returns the dot product of it to (x, y).
     * The vectors COULD be random but Perlin's improved noise has reasons for using these vectors:
     *
     *  1) These dot products require no multiplications because all the vectors have 1s or 0s as
     * components, meaning this is faster.
     *
     * 2) Another reason I read from Perlin's paper was that they reduce directional bias.
     *
     *
     * Calculates the influence that the gradient vector has on the point.
     *
     * @param    hash                          Hash value of the corner to get the gradient vector
     * @param    x                             x component of the vector to the point from corner
     * @param    y                             y component of the vector to the point from corner
     * @return   f32                           Influence that the corner gradient vector has on x, y
     */
    static f32 grad(i32 hash, f32 x, f32 y);

    std::array<i32, PERMUTATION_TABLE_ARRAY_SIZE>
        m_permutationTable;  ///< Stores a random sequence to get hash values for each corner
};
