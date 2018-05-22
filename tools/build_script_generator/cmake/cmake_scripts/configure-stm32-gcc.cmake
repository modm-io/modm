INCLUDE(./cmake/report-build-options.cmake)

SET(CMAKE_SYSTEM_NAME Generic)
SET(CMAKE_SYSTEM_PROCESSOR arm)

# Configure toolchain

IF(NOT TARGET_TRIPLET)
    SET(TARGET_TRIPLET "arm-none-eabi")
ENDIF()

IF (WIN32)
    SET(TOOL_EXECUTABLE_SUFFIX ".exe")
ELSE()
    SET(TOOL_EXECUTABLE_SUFFIX "")
ENDIF()

IF(${CMAKE_VERSION} VERSION_LESS 3.6.0)
    INCLUDE(CMakeForceCompiler)
    CMAKE_FORCE_C_COMPILER("${TARGET_TRIPLET}-gcc${TOOL_EXECUTABLE_SUFFIX}" GNU)
    CMAKE_FORCE_CXX_COMPILER("${TARGET_TRIPLET}-g++${TOOL_EXECUTABLE_SUFFIX}" GNU)
ELSE()
    SET(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
    SET(CMAKE_C_COMPILER "${TARGET_TRIPLET}-gcc${TOOL_EXECUTABLE_SUFFIX}")
    SET(CMAKE_CXX_COMPILER "${TARGET_TRIPLET}-g++${TOOL_EXECUTABLE_SUFFIX}")
ENDIF()

SET(CMAKE_LINKER       "${TARGET_TRIPLET}-g++${TOOL_EXECUTABLE_SUFFIX}" CACHE INTERNAL "linker tool")
SET(CMAKE_ASM_COMPILER "${TARGET_TRIPLET}-gcc${TOOL_EXECUTABLE_SUFFIX}" CACHE INTERNAL "ASM compiler")
SET(CMAKE_OBJCOPY      "${TARGET_TRIPLET}-objcopy${TOOL_EXECUTABLE_SUFFIX}" CACHE INTERNAL "objcopy tool")
SET(CMAKE_OBJDUMP      "${TARGET_TRIPLET}-objdump${TOOL_EXECUTABLE_SUFFIX}" CACHE INTERNAL "objdump tool")
SET(CMAKE_SIZE         "${TARGET_TRIPLET}-size${TOOL_EXECUTABLE_SUFFIX}" CACHE INTERNAL "size tool")
SET(CMAKE_DEBUGER      "${TARGET_TRIPLET}-gdb${TOOL_EXECUTABLE_SUFFIX}" CACHE INTERNAL "debuger")
SET(CMAKE_CPPFILT      "${TARGET_TRIPLET}-c++filt${TOOL_EXECUTABLE_SUFFIX}" CACHE INTERNAL "C++filt")
SET(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
SET(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Prepare linker script

SET(LINKER_SCRIPT_NAME linkerscript.ld)
SET(LINKER_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/${LINKER_SCRIPT_NAME})
CONFIGURE_FILE(modm/link/${LINKER_SCRIPT_NAME} ${LINKER_SCRIPT})
CONFIGURE_FILE(modm/link/peripherals.ld ${CMAKE_CURRENT_BINARY_DIR}/peripherals.ld)

# FPU flags

IF(PROJECT_CORE STREQUAL "cortex-m4" OR PROJECT_CORE STREQUAL "cortext-m7")
    SET(FPU_FLAGS "-mfpu=fpv4-sp-d16 -mfloat-abi=softfp -Wdouble-promotion -fsingle-precision-constant")
ENDIF()

# Command line options

SET(COMMON_FLAGS "\
    ${FPU_FLAGS} \
    -fdata-sections \
    -ffunction-sections \
    -finline-limit=10000 \
    -fno-move-loop-invariants \
    -fno-split-wide-types \
    -fno-tree-loop-optimize \
    -fno-unwind-tables \
    -fshort-wchar \
    -funsigned-bitfields \
    -funsigned-char \
    -gdwarf-2 \
    -W \
    -Wall \
    -Wextra \
    -Wformat \
    -Winit-self \
    -Wno-long-long \
    -Wpointer-arith \
    -Wundef \
    -Wuninitialized \
    -Wunused \
    -Wunused-parameter \
    -Wwrite-strings")

SET(OPTIMIZE_FLAGS "-Os")

SET(DEBUG_FLAGS "-Og")

SET(ARCH_FLAGS "\
    -mcpu=${PROJECT_CORE} \
    -mthumb \
    -mthumb-interwork")

SET(C_LANG_FLAGS "\
    -Wimplicit \
    -Wnested-externs \
    -Wredundant-decls \
    -Wstrict-prototypes \
    -std=gnu99")

SET(CXX_LANG_FLAGS "\
    -fno-exceptions \
    -fno-rtti \
    -fno-threadsafe-statics \
    -fuse-cxa-atexit \
    -Woverloaded-virtual \
    -std=c++17")

SET(LINK_FLAGS "\
    -Llink \
    -nostartfiles \
    -T${LINKER_SCRIPT} \
    -Wl,--fatal-warnings \
    -Wl,--gc-sections \
    -Wl,--no-wchar-size-warning \
    -Wl,--relax \
    -Wl,-wrap,_calloc_r \
    -Wl,-wrap,_free_r \
    -Wl,-wrap,_malloc_r \
    -Wl,-wrap,_realloc_r \
    -Wl,-Map,${CMAKE_PROJECT_NAME}.map")

# Mix flags together

SET(CMAKE_C_FLAGS_RELEASE   "${ARCH_FLAGS} ${COMMON_FLAGS} ${C_LANG_FLAGS} ${OPTIMIZE_FLAGS}" CACHE INTERNAL "c compiler flags release")
SET(CMAKE_C_FLAGS_DEBUG     "${ARCH_FLAGS} ${COMMON_FLAGS} ${C_LANG_FLAGS} ${DEBUG_FLAGS}" CACHE INTERNAL "c compiler flags debug")
SET(CMAKE_CXX_FLAGS_RELEASE "${ARCH_FLAGS} ${COMMON_FLAGS} ${CXX_LANG_FLAGS} ${OPTIMIZE_FLAGS}" CACHE INTERNAL "cxx compiler flags debug")
SET(CMAKE_CXX_FLAGS_DEBUG   "${ARCH_FLAGS} ${COMMON_FLAGS} ${CXX_LANG_FLAGS} ${DEBUG_FLAGS}" CACHE INTERNAL "cxx compiler flags debug")
SET(CMAKE_ASM_FLAGS_RELEASE "${ARCH_FLAGS}" CACHE INTERNAL "asm compiler flags release")
SET(CMAKE_ASM_FLAGS_DEBUG   "${ARCH_FLAGS} ${DEBUG_FLAGS}" CACHE INTERNAL "asm compiler flags debug")
SET(CMAKE_EXE_LINKER_FLAGS  "--specs=nano.specs --specs=nosys.specs ${LINK_FLAGS}" CACHE INTERNAL "linker flags common")
