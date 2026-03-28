#include "Engine/Noise.h"

PerlinNoise2D::PerlinNoise2D(u64 seed)
{
    std::iota(m_permutationTable.begin(), m_permutationTable.begin() + PERMUTATION_TABLE_SIZE, 0);
    std::mt19937_64 rng(seed);
    std::shuffle(m_permutationTable.begin(), m_permutationTable.begin() + PERMUTATION_TABLE_SIZE,
                 rng);

    // repeat the table to avoid buffer overflow
    for (i32 i = 0; i < PERMUTATION_TABLE_SIZE; ++i)
        m_permutationTable[PERMUTATION_TABLE_SIZE + i] = m_permutationTable[i];
}

f32 PerlinNoise2D::noise(f32 x, f32 y) const
{
    i32 xi = static_cast<i32>(std::floor(x)) & PERMUTATION_MASK;
    i32 yi = static_cast<i32>(std::floor(y)) & PERMUTATION_MASK;

    f32 xf = x - std::floor(x);
    f32 yf = y - std::floor(y);

    i32 aa = m_permutationTable[m_permutationTable[xi] + yi];
    i32 ab = m_permutationTable[m_permutationTable[xi] + yi + 1];
    i32 ba = m_permutationTable[m_permutationTable[xi + 1] + yi];
    i32 bb = m_permutationTable[m_permutationTable[xi + 1] + yi + 1];

    f32 g_aa = grad(aa, xf, yf);
    f32 g_ba = grad(ba, xf - 1, yf);
    f32 g_ab = grad(ab, xf, yf - 1);
    f32 g_bb = grad(bb, xf - 1, yf - 1);

    f32 u = fade(xf);
    f32 v = fade(yf);

    f32 x1 = lerp(g_aa, g_ba, u);
    f32 x2 = lerp(g_ab, g_bb, u);

    return lerp(x1, x2, v);
}

f32 PerlinNoise2D::fade(f32 t) { return t * t * t * (t * (t * 6 - 15) + 10); }

f32 PerlinNoise2D::lerp(f32 a, f32 b, f32 t) { return a + t * (b - a); }

f32 PerlinNoise2D::grad(i32 hash, f32 x, f32 y)
{
    // This is the optimized way of taking the dot product of the gradient vector and x, y
    // The vectors COULD be random but Perlin's improved noise has reasons for using these vectors
    // 1) These dot products require no multiplcations because all the vectors have 1s or 0s as
    // components
    // 2) Another reason I read from Perlin's paper was that they reduce directional bias
    switch (hash & 7)
    {
        case 0:
            return x + y;
        case 1:
            return -x + y;
        case 2:
            return x - y;
        case 3:
            return -x - y;
        case 4:
            return x;
        case 5:
            return -x;
        case 6:
            return y;
        case 7:
            return -y;
        default:
            return 0;
    }
}

FractalPerlinNoise2D::FractalPerlinNoise2D(u64 seed) : m_perlinNoise{seed}, m_config{Config{}} {}

FractalPerlinNoise2D::FractalPerlinNoise2D(u64 seed, Config cfg)
    : m_perlinNoise{seed}, m_config{cfg}
{
}

void FractalPerlinNoise2D::setConfig(Config cfg) { m_config = cfg; }

f32 FractalPerlinNoise2D::noise(f32 x, f32 y) const
{
    f32 value     = 0.0;
    f32 amplitude = m_config.amplitude;
    f32 frequency = m_config.frequency;
    f32 maxValue  = 0.0;

    for (i32 i = 0; i < m_config.octaves; ++i)
    {
        f32 n = m_perlinNoise.noise(x * frequency, y * frequency);
        value += n * amplitude;
        maxValue += amplitude;
        amplitude *= m_config.persistence;
        frequency *= m_config.lacunarity;
    }

    // Normalize
    return value / maxValue;
}

f32 FractalPerlinNoise2D::warpedNoise(f32 x, f32 y, f32 warpStrength, f32 offsetXForX,
                                      f32 offsetYForX, f32 offsetXForY, f32 offsetYforY) const
{
    f32 ox = noise(x + offsetXForX, y + offsetYForX);
    f32 oy = noise(x + offsetXForY, y + offsetYforY);
    return noise(x + warpStrength * ox, y + warpStrength * oy);
}