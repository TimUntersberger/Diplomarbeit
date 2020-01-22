file(REMOVE_RECURSE
  "internal_communication.map"
  "config/sdkconfig.h"
  "config/sdkconfig.cmake"
  "bootloader/bootloader.elf"
  "bootloader/bootloader.bin"
  "bootloader/bootloader.map"
  "CMakeFiles/ldgen"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/ldgen.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
