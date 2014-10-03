//
// Automatically generated by gen-bit-packers, do not edit
//
#ifndef __CXXU_PACK_PACK_UNPACK_64_H__
#define __CXXU_PACK_PACK_UNPACK_64_H__

#include <stdint.h>

#include <cxxutils/config.h>

namespace cxxu {
namespace pack {

typedef
void
(*packer_64_ptr)(
    uint64_t* __restrict__ out,
    const uint64_t* __restrict__ in,
    uint32_t n
);
extern CXXUTILS_API packer_64_ptr packer_64_table[64];
extern CXXUTILS_API packer_64_ptr packer_64_simd_table[64];

typedef
void
(*unpacker_64_ptr)(
    uint64_t* __restrict__ out,
    const uint64_t* __restrict__ in,
    uint32_t n
);
extern CXXUTILS_API unpacker_64_ptr unpacker_64_table[64];
extern CXXUTILS_API unpacker_64_ptr unpacker_64_simd_table[64];

} // namespace pack
} // namespace cxxu

#endif // __CXXU_PACK_PACK_UNPACK_64_H__
