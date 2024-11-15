# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

set(TARGET "$ENV{TARGET}")
message("Target is " ${TARGET})

if(${TARGET} STREQUAL "riscv")

# RiscV target, Zephyr

set(DTC_OVERLAY_FILE "doom.overlay")
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
#zephyr_compile_definitions(ZEPHYR=1 NORMALUNIX=1)
set(PLF_INC $ENV{ZEPHYR_BASE}/include)
add_compile_options(-v -g -Wall -DNORMALUNIX)

set(PLF_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/riscv/d_main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/riscv/i_main.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/riscv/i_net.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/riscv/i_sound.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/riscv/i_system.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/riscv/i_video.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/riscv/s_sound.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/riscv/d_zephyr.c  
    ${CMAKE_CURRENT_SOURCE_DIR}/src/riscv/mini-printf.c)


else()

# Unix based - e.g. luckfox target
set(SDK_PATH $ENV{LUCKFOX_SDK_PATH})

# check SDK
if(NOT SDK_PATH)
    message(WARNING "Please Set Luckfox-pico SDK Path. Such as:export LUCKFOX_SDK_PATH=/home/user/luckfox-pico")
    return()
else()
set(CMAKE_C_COMPILER "${SDK_PATH}/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/arm-rockchip830-linux-uclibcgnueabihf-gcc")
set(CMAKE_CXX_COMPILER "${SDK_PATH}/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/arm-rockchip830-linux-uclibcgnueabihf-g++")
endif()

# Opencv 4
#set(OpenCV_DIR "/work/src/luckfox/demos/luckfox_pico_rknn_example/lib/cmake/opencv4")
#find_package(OpenCV REQUIRED)
# Thread
#set(THREADS_PREFER_PTHREAD_FLAG ON)
#find_package(Threads REQUIRED)

add_compile_options(-g -Wall -DNORMALUNIX
                    -DISP_HW_V30 -DRKPLATFORM=ON -DARCH64=OFF
                    -DROCKIVA -DUAPI2
                    -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
		    -D_FILE_OFFSET_BITS=64
		    -DPTHREADS -DLUCKFOX)


set(PLF_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/luckfox/d_main.c
	      ${CMAKE_CURRENT_SOURCE_DIR}/src/luckfox/i_net.c
	      ${CMAKE_CURRENT_SOURCE_DIR}/src/luckfox/i_system.c
	      ${CMAKE_CURRENT_SOURCE_DIR}/src/luckfox/i_video.c
	      ${CMAKE_CURRENT_SOURCE_DIR}/src/luckfox/i_sound.c
	      ${CMAKE_CURRENT_SOURCE_DIR}/src/luckfox/s_sound.c)

# ${CMAKE_CURRENT_SOURCE_DIR}/src/luckfox/cv-video.cpp

endif()

# src/riscv/libc_backend.c  
# src/riscv/console.c  
# src/riscv/c64-lib.cpp

message(STATUS "Plf includes: " ${PLF_INC})
include_directories(src ${PLF_INC})
add_link_options()

set(SRC 
	${CMAKE_CURRENT_SOURCE_DIR}/src/am_map.c 
	${CMAKE_CURRENT_SOURCE_DIR}/src/d_items.c 
	${CMAKE_CURRENT_SOURCE_DIR}/src/d_net.c 
	${CMAKE_CURRENT_SOURCE_DIR}/src/doomdef.c 
	${CMAKE_CURRENT_SOURCE_DIR}/src/doomstat.c 
	${CMAKE_CURRENT_SOURCE_DIR}/src/dstrings.c
  ${CMAKE_CURRENT_SOURCE_DIR}/src/f_finale.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/f_wipe.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/g_game.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/hu_lib.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/hu_stuff.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/info.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/m_argv.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/m_bbox.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/m_cheat.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/m_fixed.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/m_menu.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/m_misc.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/m_random.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/m_swap.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/p_ceilng.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/p_doors.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/p_enemy.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/p_floor.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/p_inter.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/p_lights.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/p_map.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/p_maputl.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/p_mobj.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/p_plats.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/p_pspr.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/p_saveg.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/p_setup.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/p_sight.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/p_spec.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/p_switch.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/p_telept.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/p_tick.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/p_user.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/r_bsp.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/r_data.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/r_draw.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/r_main.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/r_plane.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/r_segs.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/r_sky.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/r_things.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/sounds.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/st_lib.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/st_stuff.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/tables.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/v_video.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/wi_stuff.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/w_wad.c 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/z_zone.c 
  )

project(${TARGET}-doom)
add_executable (${PROJECT_NAME} ${SRC} ${PLF_FILES}) 

target_sources(${PROJECT_NAME} PRIVATE ${SRC} ${PLF_FILES})
#target_sources(app PRIVATE ${SRC} ${PLF_FILES})
