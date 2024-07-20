#ifndef MX_COMMON_H
#define MX_COMMON_H 1

inline static uint32_t mx_ceil_to_multiples(uint32_t cnt, uint32_t n)
{
    return (cnt + (n - 1)) & (~(n - 1));
} // mx_ceil_to_multiples

inline static uint32_t mx_ceil_to_multiples_of_8(uint32_t cnt)
{
    return (cnt + (8 - 1)) & (~(8 - 1));
} // mx_ceil_to_multiples_of_8

inline static uint32_t mx_ceil_to_multiples_of_16(uint32_t cnt)
{
    return (cnt + (16 - 1)) & (~(16 - 1));
} // mx_ceil_to_multiples_of_16

inline static uint32_t mx_floor_to_multiples_of_16(uint32_t cnt)
{
    return cnt & (~(16 - 1));
} // mx_floor_to_multiples_of_16

inline static uint32_t mx_floor_to_multiples_of(uint32_t cnt, uint32_t n)
{
    return cnt & (~(n - 1));
} // mx_floor_to_multiples_of

inline static uint32_t mx_ceil_to_or_less_than_16(uint32_t cnt)
{
    return (cnt < 16) ? cnt : 16;
} // mx_ceil_to_or_less_than_16

inline static uint32_t mx_less_than_or_equal_to(uint32_t cnt, uint32_t n)
{
    return (cnt < n) ? cnt : n;
} // mx_less_than_or_equal_to

#endif // MX_COMMON_H

