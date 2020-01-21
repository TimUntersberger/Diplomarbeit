file(REMOVE_RECURSE
  "internal_communication.map"
  "config/sdkconfig.h"
  "config/sdkconfig.cmake"
  "bootloader/bootloader.elf"
  "bootloader/bootloader.bin"
  "bootloader/bootloader.map"
  "CMakeFiles/dummy_main_src"
  "dummy_main_src.c"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/dummy_main_src.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
