#pragma once

#include <cstddef>
#include <cstdint>

#ifndef FREE_HEAP
#define FREE_HEAP 100000
#endif

#ifndef MAX_ALLOC_HEAP
#define MAX_ALLOC_HEAP 80000
#endif

#ifndef FREE_PSRAM
#define FREE_PSRAM 0  // no Pseudostatic RAM
#endif

#ifndef MOCK_CHIP_ID
#define MOCK_CHIP_ID 0x4D4F434B  // MOCK in ascii
#endif

#ifndef FLASH_CHIP_SIZE
#define FLASH_CHIP_SIZE (4 * 1024 * 1024)  // 4MB
#endif

#ifndef CPU_FREQ_MHZ
#define CPU_FREQ_MHZ 240
#endif

class EspClass {
 public:
  size_t getFreeHeap() { return FREE_HEAP; }
  size_t getMaxAllocHeap() { return MAX_ALLOC_HEAP; }
  size_t getFreePsram() { return FREE_PSRAM; }
  void restart() {}
  uint32_t getChipId() { return MOCK_CHIP_ID; }
  uint32_t getFlashChipSize() { return FLASH_CHIP_SIZE; }
  uint32_t getCpuFreqMHz() { return CPU_FREQ_MHZ; }
};

extern EspClass ESP;