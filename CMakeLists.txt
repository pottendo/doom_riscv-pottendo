# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

set(DTC_OVERLAY_FILE "doom.overlay")
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(c)
zephyr_compile_definitions(ZEPHYR=1 NORMALUNIX=1)
include_directories(src)

# src/riscv/libc_backend.c  
# src/riscv/console.c  
# src/riscv/c64-lib.cpp
target_sources(app PRIVATE
  src/riscv/d_main.c 
  src/riscv/i_main.c 
  src/riscv/i_net.c 
  src/riscv/i_sound.c 
  src/riscv/i_system.c 
  src/riscv/i_video.c 
  src/riscv/s_sound.c 
  src/riscv/d_zephyr.c  
  src/riscv/mini-printf.c
  src/am_map.c 
  src/d_items.c 
  src/d_net.c 
  src/doomdef.c 
  src/doomstat.c 
  src/dstrings.c 
  src/f_finale.c 
  src/f_wipe.c 
  src/g_game.c 
  src/hu_lib.c 
  src/hu_stuff.c 
  src/info.c 
  src/m_argv.c 
  src/m_bbox.c 
  src/m_cheat.c 
  src/m_fixed.c 
  src/m_menu.c 
  src/m_misc.c 
  src/m_random.c 
  src/m_swap.c 
  src/p_ceilng.c 
  src/p_doors.c 
  src/p_enemy.c 
  src/p_floor.c 
  src/p_inter.c 
  src/p_lights.c 
  src/p_map.c 
  src/p_maputl.c 
  src/p_mobj.c 
  src/p_plats.c 
  src/p_pspr.c 
  src/p_saveg.c 
  src/p_setup.c 
  src/p_sight.c 
  src/p_spec.c 
  src/p_switch.c 
  src/p_telept.c 
  src/p_tick.c 
  src/p_user.c 
  src/r_bsp.c 
  src/r_data.c 
  src/r_draw.c 
  src/r_main.c 
  src/r_plane.c 
  src/r_segs.c 
  src/r_sky.c 
  src/r_things.c 
  src/sounds.c 
  src/st_lib.c 
  src/st_stuff.c 
  src/tables.c 
  src/v_video.c 
  src/wi_stuff.c 
  src/w_wad.c 
  src/z_zone.c 
  )
