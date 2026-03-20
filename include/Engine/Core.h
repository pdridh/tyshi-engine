#pragma once

#include <stdio.h>

#include <cstdint>

#if WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#endif

#ifdef DEBUG_BUILD
#define BUILD_TYPE "Debug"
#endif

#ifdef RELEASE_BUILD
#define BUILD_TYPE "Release"
#endif

#ifdef DEBUG_BUILD
#define LOG(...) fprintf(stdout, __VA_ARGS__)
#define LOG_ERROR(...) fprintf(stderr, __VA_ARGS__)
#else
#define LOG(...)
#define LOG_ERROR(...)
#endif

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float  f32;
typedef double f64;

#define Kilobytes(Value) ((Value) * 1024LL)
#define Megabytes(Value) (Kilobytes(Value) * 1024LL)
#define Gigabytes(Value) (Megabytes(Value) * 1024LL)
#define Terabytes(Value) (Gigabytes(Value) * 1024LL)
