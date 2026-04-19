// Source : https://github.com/xqemu/xqemu/blob/master/hw/xbox/nv2a_regs.h
/*
 * QEMU Geforce NV2A register definitions
 *
 * Copyright (c) 2012 espes
 * Copyright (c) 2015 Jannik Vogel
 * Copyright (c) 2018 Matt Borgerson
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 or
 * (at your option) version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses/>.
 */

#ifndef HW_NV2A_REGS_H
#define HW_NV2A_REGS_H

#define NV_NUM_BLOCKS 21
#define NV_PMC          0   /* card master control */
#define NV_PBUS         1   /* bus control */
#define NV_PFIFO        2   /* MMIO and DMA FIFO submission to PGRAPH and VPE */
#define NV_PFIFO_CACHE  3
#define NV_PRMA         4   /* access to BAR0/BAR1 from real mode */
#define NV_PVIDEO       5   /* video overlay */
#define NV_PTIMER       6   /* time measurement and time-based alarms */
#define NV_PCOUNTER     7   /* performance monitoring counters */
#define NV_PVPE         8   /* MPEG2 decoding engine */
#define NV_PTV          9   /* TV encoder */
#define NV_PRMFB        10  /* aliases VGA memory window */
#define NV_PRMVIO       11  /* aliases VGA sequencer and graphics controller registers */
#define NV_PFB          12  /* memory interface */
#define NV_PSTRAPS      13  /* straps readout / override */
#define NV_PGRAPH       14  /* accelerated 2d/3d drawing engine */
#define NV_PCRTC        15  /* more CRTC controls */
#define NV_PRMCIO       16  /* aliases VGA CRTC and attribute controller registers */
#define NV_PRAMDAC      17  /* RAMDAC, cursor, and PLL control */
#define NV_PRMDIO       18  /* aliases VGA palette registers */
#define NV_PRAMIN       19  /* RAMIN access */
#define NV_USER         20  /* PFIFO MMIO and DMA submission area */

#define NV_PMC_BOOT_0                                    0x00000000
#define NV_PMC_BOOT_1                                    0x00000004
#define NV_PMC_INTR_0                                    0x00000100
#   define NV_PMC_INTR_0_PFIFO                                 (1 << 8)
#   define NV_PMC_INTR_0_PGRAPH                               (1 << 12)
#   define NV_PMC_INTR_0_PVIDEO                               (1 << 16)
#   define NV_PMC_INTR_0_PTIMER                               (1 << 20)
#   define NV_PMC_INTR_0_PCRTC                                (1 << 24)
#   define NV_PMC_INTR_0_PCRTC2                               (1 << 25)
#   define NV_PMC_INTR_0_PBUS                                 (1 << 28)
#   define NV_PMC_INTR_0_SOFTWARE                             (1 << 31)
#define NV_PMC_INTR_EN_0                                 0x00000140
#   define NV_PMC_INTR_EN_0_HARDWARE                            1
#   define NV_PMC_INTR_EN_0_SOFTWARE                            2
#define NV_PMC_ENABLE                                    0x00000200
#   define NV_PMC_ENABLE_PFIFO                                 (1 << 8)
#   define NV_PMC_ENABLE_PGRAPH                               (1 << 12)
#   define NV_PMC_ENABLE_PFB                                  (1 << 20)
#   define NV_PMC_ENABLE_PCRTC                                (1 << 24)
#   define NV_PMC_ENABLE_PCRTC2                               (1 << 25)
#   define NV_PMC_ENABLE_PVIDEO                               (1 << 28)


#define NV_PBUS_FBIO_RAM                                 0x00000218  
#  define NV_PBUS_FBIO_RAM_TYPE                              0x00000100
#    define NV_PBUS_FBIO_RAM_TYPE_DDR                          (0 << 8)
#    define NV_PBUS_FBIO_RAM_TYPE_SDR                          (1 << 8)
/* These map approximately to the pci registers */
#define NV_PBUS_PCI_NV_0                                 0x00000800
#   define NV_PBUS_PCI_NV_0_VENDOR_ID                         0x0000FFFF
#   define NV_CONFIG_PCI_NV_0_DEVICE_ID                       0xFFFF0000
#define NV_PBUS_PCI_NV_1                                 0x00000804
#define NV_PBUS_PCI_NV_2                                 0x00000808
#   define NV_PBUS_PCI_NV_2_REVISION_ID                       0x000000FF
#   define NV_PBUS_PCI_NV_2_CLASS_CODE                        0xFFFFFF00
#define NV_PBUS_PCI_NV_3                                 0x0000080C
#define NV_PBUS_PCI_NV_4                                 0x00000810
#define NV_PBUS_PCI_NV_5                                 0x00000814
#define NV_PBUS_PCI_NV_6                                 0x00000818
#define NV_PBUS_PCI_NV_7                                 0x0000081C
#define NV_PBUS_PCI_NV_11                                0x0000082C
#define NV_PBUS_PCI_NV_12                                0x00000830
#define NV_PBUS_PCI_NV_13                                0x00000834
#define NV_PBUS_PCI_NV_14                                0x00000838
#define NV_PBUS_PCI_NV_15                                0x0000083C
#define NV_PBUS_PCI_NV_16                                0x00000840
#define NV_PBUS_PCI_NV_17                                0x00000844
#define NV_PBUS_PCI_NV_18                                0x00000848
#define NV_PBUS_PCI_NV_19                                0x0000084C
#define NV_PBUS_PCI_NV_20                                0x00000850
#define NV_PBUS_PCI_NV_21                                0x00000854
#define NV_PBUS_PCI_NV_22                                0x00000858
#define NV_PBUS_PCI_NV_23                                0x0000085C
#define NV_PBUS_PCI_NV_24                                0x00000860
#define NV_PBUS_PCI_NV_25                                0x00000864
#define NV_PBUS_PCI_NV_26                                0x00000868


#define NV_PFIFO_DELAY_0                                 0x00000040
#define NV_PFIFO_DMA_TIMESLICE                           0x00000044
#define NV_PFIFO_TIMESLICE                               0x0000004C
#define NV_PFIFO_INTR_0                                  0x00000100
#   define NV_PFIFO_INTR_0_CACHE_ERROR                          (1 << 0)
#   define NV_PFIFO_INTR_0_RUNOUT                               (1 << 4)
#   define NV_PFIFO_INTR_0_RUNOUT_OVERFLOW                      (1 << 8)
#   define NV_PFIFO_INTR_0_DMA_PUSHER                          (1 << 12)
#   define NV_PFIFO_INTR_0_DMA_PT                              (1 << 16)
#   define NV_PFIFO_INTR_0_SEMAPHORE                           (1 << 20)
#   define NV_PFIFO_INTR_0_ACQUIRE_TIMEOUT                     (1 << 24)
#define NV_PFIFO_INTR_EN_0                               0x00000140
#   define NV_PFIFO_INTR_EN_0_CACHE_ERROR                       (1 << 0)
#   define NV_PFIFO_INTR_EN_0_RUNOUT                            (1 << 4)
#   define NV_PFIFO_INTR_EN_0_RUNOUT_OVERFLOW                   (1 << 8)
#   define NV_PFIFO_INTR_EN_0_DMA_PUSHER                       (1 << 12)
#   define NV_PFIFO_INTR_EN_0_DMA_PT                           (1 << 16)
#   define NV_PFIFO_INTR_EN_0_SEMAPHORE                        (1 << 20)
#   define NV_PFIFO_INTR_EN_0_ACQUIRE_TIMEOUT                  (1 << 24)
#define NV_PFIFO_RAMHT                                   0x00000210
//#   define NV_PFIFO_RAMHT_BASE_ADDRESS                        0x000001F0
#   define NV_PFIFO_RAMHT_BASE_ADDRESS_MASK                   0x000001F0
#   define NV_PFIFO_RAMHT_BASE_ADDRESS_SHIFT                  4
#   define NV_PFIFO_RAMHT_BASE_ADDRESS_MOVE                   12
//#   define NV_PFIFO_RAMHT_SIZE                                0x00030000
#   define NV_PFIFO_RAMHT_SIZE_MASK                           0x00030000
#   define NV_PFIFO_RAMHT_SIZE_SHIFT                          16
#       define NV_PFIFO_RAMHT_SIZE_4K                             0
#       define NV_PFIFO_RAMHT_SIZE_8K                             1
#       define NV_PFIFO_RAMHT_SIZE_16K                            2
#       define NV_PFIFO_RAMHT_SIZE_32K                            3
//#   define NV_PFIFO_RAMHT_SEARCH                              0x03000000
#   define NV_PFIFO_RAMHT_SEARCH_MASK                         0x03000000
#   define NV_PFIFO_RAMHT_SEARCH_SHIFT                        24
#       define NV_PFIFO_RAMHT_SEARCH_16                           0
#       define NV_PFIFO_RAMHT_SEARCH_32                           1
#       define NV_PFIFO_RAMHT_SEARCH_64                           2
#       define NV_PFIFO_RAMHT_SEARCH_128                          3
#define NV_PFIFO_RAMFC                                   0x00000214
//#   define NV_PFIFO_RAMFC_BASE_ADDRESS1                       0x000001FC
#   define NV_PFIFO_RAMFC_BASE_ADDRESS1_MASK                  0x000001FC
#   define NV_PFIFO_RAMFC_BASE_ADDRESS1_SHIFT                 2
#   define NV_PFIFO_RAMFC_BASE_ADDRESS1_MOVE                  10
//#   define NV_PFIFO_RAMFC_SIZE                                0x00010000
#   define NV_PFIFO_RAMFC_SIZE_MASK                           0x00010000
#   define NV_PFIFO_RAMFC_SIZE_1K                             0x00000000
#   define NV_PFIFO_RAMFC_SIZE_2K                             0x00010000
//#   define NV_PFIFO_RAMFC_BASE_ADDRESS2                       0x00FE0000
#   define NV_PFIFO_RAMFC_BASE_ADDRESS2_MASK                  0x00FE0000
#   define NV_PFIFO_RAMFC_BASE_ADDRESS2_SHIFT                 17
#   define NV_PFIFO_RAMFC_BASE_ADDRESS2_MOVE                  10
#define NV_PFIFO_RAMRO                                   0x00000218
#   define NV_PFIFO_RAMRO_BASE_ADDRESS                        0x000001FE
#   define NV_PFIFO_RAMRO_SIZE                                0x00010000
#define NV_PFIFO_RUNOUT_STATUS                           0x00000400
#   define NV_PFIFO_RUNOUT_STATUS_RANOUT                       (1 << 0)
#   define NV_PFIFO_RUNOUT_STATUS_LOW_MARK                     (1 << 4)
#   define NV_PFIFO_RUNOUT_STATUS_HIGH_MARK                    (1 << 8)
#define NV_PFIFO_RUNOUT_PUT_ADDRESS                      0x00000410
#define NV_PFIFO_RUNOUT_GET_ADDRESS                      0x00000420
#define NV_PFIFO_CACHES                                  0x00000500
#define NV_PFIFO_MODE                                    0x00000504
#define NV_PFIFO_DMA                                     0x00000508
#define NV_PFIFO_SIZE                                    0x0000050C
#define NV_PFIFO_CACHE0_PUSH0                            0x00001000
#define NV_PFIFO_CACHE0_PULL0                            0x00001050
#define NV_PFIFO_CACHE0_HASH                             0x00001058
#define NV_PFIFO_CACHE1_PUSH0                            0x00001200
#   define NV_PFIFO_CACHE1_PUSH0_ACCESS                         (1 << 0)
#define NV_PFIFO_CACHE1_PUSH1                            0x00001204
#   define NV_PFIFO_CACHE1_PUSH1_CHID                         0x0000001F
#   define NV_PFIFO_CACHE1_PUSH1_MODE                         0x00000100
#       define NV_PFIFO_CACHE1_PUSH1_MODE_PIO                     0
#       define NV_PFIFO_CACHE1_PUSH1_MODE_DMA                     1
#define NV_PFIFO_CACHE1_PUT                              0x00001210
#define NV_PFIFO_CACHE1_STATUS                           0x00001214
#   define NV_PFIFO_CACHE1_STATUS_LOW_MARK                      (1 << 4)
#   define NV_PFIFO_CACHE1_STATUS_HIGH_MARK                     (1 << 8)
#define NV_PFIFO_CACHE1_DMA_PUSH                         0x00001220
#   define NV_PFIFO_CACHE1_DMA_PUSH_ACCESS                      (1 << 0)
#   define NV_PFIFO_CACHE1_DMA_PUSH_STATE                       (1 << 4)
#   define NV_PFIFO_CACHE1_DMA_PUSH_BUFFER                      (1 << 8)
#   define NV_PFIFO_CACHE1_DMA_PUSH_STATUS                     (1 << 12)
#   define NV_PFIFO_CACHE1_DMA_PUSH_ACQUIRE                    (1 << 16)
#define NV_PFIFO_CACHE1_DMA_FETCH                        0x00001224
#   define NV_PFIFO_CACHE1_DMA_FETCH_TRIG                     0x000000F8
#   define NV_PFIFO_CACHE1_DMA_FETCH_SIZE                     0x0000E000
#   define NV_PFIFO_CACHE1_DMA_FETCH_MAX_REQS                 0x001F0000
#define NV_PFIFO_CACHE1_DMA_STATE                        0x00001228
#   define NV_PFIFO_CACHE1_DMA_STATE_METHOD_TYPE                (1 << 0)
#       define NV_PFIFO_CACHE1_DMA_STATE_METHOD_TYPE_INC          0
#       define NV_PFIFO_CACHE1_DMA_STATE_METHOD_TYPE_NON_INC      1
#   define NV_PFIFO_CACHE1_DMA_STATE_METHOD                   0x00001FFC
#   define NV_PFIFO_CACHE1_DMA_STATE_SUBCHANNEL               0x0000E000
#   define NV_PFIFO_CACHE1_DMA_STATE_METHOD_COUNT             0x1FFC0000
#   define NV_PFIFO_CACHE1_DMA_STATE_ERROR                    0xE0000000
#       define NV_PFIFO_CACHE1_DMA_STATE_ERROR_NONE               0
#       define NV_PFIFO_CACHE1_DMA_STATE_ERROR_CALL               1
#       define NV_PFIFO_CACHE1_DMA_STATE_ERROR_NON_CACHE          2
#       define NV_PFIFO_CACHE1_DMA_STATE_ERROR_RETURN             3
#       define NV_PFIFO_CACHE1_DMA_STATE_ERROR_RESERVED_CMD       4
#       define NV_PFIFO_CACHE1_DMA_STATE_ERROR_PROTECTION         6
#define NV_PFIFO_CACHE1_DMA_INSTANCE                     0x0000122C
//#   define NV_PFIFO_CACHE1_DMA_INSTANCE_ADDRESS               0x0000FFFF
#   define NV_PFIFO_CACHE1_DMA_INSTANCE_ADDRESS_MASK          0x0000FFFF
#   define NV_PFIFO_CACHE1_DMA_INSTANCE_ADDRESS_SHIFT         0
#   define NV_PFIFO_CACHE1_DMA_INSTANCE_ADDRESS_MOVE          4
#define NV_PFIFO_CACHE1_DMA_CTL                          0x00001230
#define NV_PFIFO_CACHE1_DMA_PUT                          0x00001240
#define NV_PFIFO_CACHE1_DMA_GET                          0x00001244
#define NV_PFIFO_CACHE1_REF                              0x00001248
#define NV_PFIFO_CACHE1_DMA_SUBROUTINE                   0x0000124C
#   define NV_PFIFO_CACHE1_DMA_SUBROUTINE_RETURN_OFFSET       0x1FFFFFFC
#   define NV_PFIFO_CACHE1_DMA_SUBROUTINE_STATE                (1 << 0)
#define NV_PFIFO_CACHE1_PULL0                            0x00001250
#   define NV_PFIFO_CACHE1_PULL0_ACCESS                        (1 << 0)
#define NV_PFIFO_CACHE1_PULL1                            0x00001254
#   define NV_PFIFO_CACHE1_PULL1_ENGINE                       0x00000003
#define NV_PFIFO_CACHE1_HASH                             0x00001258
#define NV_PFIFO_CACHE1_ACQUIRE_0                        0x00001260
#define NV_PFIFO_CACHE1_ACQUIRE_1                        0x00001264
#define NV_PFIFO_CACHE1_ACQUIRE_2                        0x00001268
#define NV_PFIFO_CACHE1_SEMAPHORE                        0x0000126C
#define NV_PFIFO_CACHE1_GET                              0x00001270
#define NV_PFIFO_CACHE1_ENGINE                           0x00001280
#define NV_PFIFO_CACHE1_DMA_DCOUNT                       0x000012A0
#   define NV_PFIFO_CACHE1_DMA_DCOUNT_VALUE                   0x00001FFC
#define NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW               0x000012A4
#   define NV_PFIFO_CACHE1_DMA_GET_JMP_SHADOW_OFFSET          0x1FFFFFFC
#define NV_PFIFO_CACHE1_DMA_RSVD_SHADOW                  0x000012A8
#define NV_PFIFO_CACHE1_DMA_DATA_SHADOW                  0x000012AC
#define NV_PFIFO_CACHE1_METHOD                           0x00001800
#   define NV_PFIFO_CACHE1_METHOD_TYPE                         (1 << 0)
#   define NV_PFIFO_CACHE1_METHOD_ADDRESS                     0x00001FFC
#   define NV_PFIFO_CACHE1_METHOD_SUBCHANNEL                  0x0000E000
#define NV_PFIFO_CACHE1_DATA                             0x00001804


#define NV_PGRAPH_DEBUG_0                                0x00000080
#define NV_PGRAPH_DEBUG_1                                0x00000084
#define NV_PGRAPH_DEBUG_3                                0x0000008C
#   define NV_PGRAPH_DEBUG_3_HW_CONTEXT_SWITCH                (1 << 2)
#define NV_PGRAPH_DEBUG_4                                0x00000090
#define NV_PGRAPH_DEBUG_5                                0x00000094
#define NV_PGRAPH_DEBUG_8                                0x00000098
#define NV_PGRAPH_DEBUG_9                                0x0000009C
#define NV_PGRAPH_INTR                                   0x00000100
#   define NV_PGRAPH_INTR_NOTIFY                              (1 << 0)
#   define NV_PGRAPH_INTR_MISSING_HW                          (1 << 4)
#   define NV_PGRAPH_INTR_TLB_PRESENT_DMA_R                   (1 << 6)
#   define NV_PGRAPH_INTR_TLB_PRESENT_DMA_W                   (1 << 7)
#   define NV_PGRAPH_INTR_TLB_PRESENT_TEX_A                   (1 << 8)
#   define NV_PGRAPH_INTR_TLB_PRESENT_TEX_B                   (1 << 9)
#   define NV_PGRAPH_INTR_TLB_PRESENT_VTX                    (1 << 10)
#   define NV_PGRAPH_INTR_CONTEXT_SWITCH                     (1 << 12)
#   define NV_PGRAPH_INTR_STATE3D                            (1 << 13)
#   define NV_PGRAPH_INTR_BUFFER_NOTIFY                      (1 << 16)
#   define NV_PGRAPH_INTR_ERROR                              (1 << 20)
#   define NV_PGRAPH_INTR_SINGLE_STEP                        (1 << 24)
#define NV_PGRAPH_NSOURCE                                0x00000108
#   define NV_PGRAPH_NSOURCE_NOTIFICATION                     (1 << 0)
#define NV_PGRAPH_INTR_EN                                0x00000140
#   define NV_PGRAPH_INTR_EN_NOTIFY                           (1 << 0)
#   define NV_PGRAPH_INTR_EN_MISSING_HW                       (1 << 4)
#   define NV_PGRAPH_INTR_EN_TLB_PRESENT_DMA_R                (1 << 6)
#   define NV_PGRAPH_INTR_EN_TLB_PRESENT_DMA_W                (1 << 7)
#   define NV_PGRAPH_INTR_EN_TLB_PRESENT_TEX_A                (1 << 8)
#   define NV_PGRAPH_INTR_EN_TLB_PRESENT_TEX_B                (1 << 9)
#   define NV_PGRAPH_INTR_EN_TLB_PRESENT_VTX                 (1 << 10)
#   define NV_PGRAPH_INTR_EN_CONTEXT_SWITCH                  (1 << 12)
#   define NV_PGRAPH_INTR_EN_STATE3D                         (1 << 13)
#   define NV_PGRAPH_INTR_EN_BUFFER_NOTIFY                   (1 << 16)
#   define NV_PGRAPH_INTR_EN_ERROR                           (1 << 20)
#   define NV_PGRAPH_INTR_EN_SINGLE_STEP                     (1 << 24)
#define NV_PGRAPH_CTX_CONTROL                            0x00000144
#   define NV_PGRAPH_CTX_CONTROL_MINIMUM_TIME                 0x00000003
#   define NV_PGRAPH_CTX_CONTROL_TIME                           (1 << 8)
#   define NV_PGRAPH_CTX_CONTROL_CHID                          (1 << 16)
#   define NV_PGRAPH_CTX_CONTROL_CHANGE                        (1 << 20)
#   define NV_PGRAPH_CTX_CONTROL_SWITCHING                     (1 << 24)
#   define NV_PGRAPH_CTX_CONTROL_DEVICE                        (1 << 28)
#define NV_PGRAPH_CTX_USER                               0x00000148
#   define NV_PGRAPH_CTX_USER_CHANNEL_3D                        (1 << 0)
#   define NV_PGRAPH_CTX_USER_CHANNEL_3D_VALID                  (1 << 4)
#   define NV_PGRAPH_CTX_USER_SUBCH                           0x0000E000
#   define NV_PGRAPH_CTX_USER_CHID                            0x1F000000
#   define NV_PGRAPH_CTX_USER_SINGLE_STEP                      (1 << 31)
#define NV_PGRAPH_CTX_SWITCH1                            0x0000014C
#   define NV_PGRAPH_CTX_SWITCH1_GRCLASS                      0x000000FF
#   define NV_PGRAPH_CTX_SWITCH1_CHROMA_KEY                    (1 << 12)
#   define NV_PGRAPH_CTX_SWITCH1_SWIZZLE                       (1 << 14)
#   define NV_PGRAPH_CTX_SWITCH1_PATCH_CONFIG                 0x00038000
#   define NV_PGRAPH_CTX_SWITCH1_SYNCHRONIZE                   (1 << 18)
#   define NV_PGRAPH_CTX_SWITCH1_ENDIAN_MODE                   (1 << 19)
#   define NV_PGRAPH_CTX_SWITCH1_CLASS_TYPE                    (1 << 22)
#   define NV_PGRAPH_CTX_SWITCH1_SINGLE_STEP                   (1 << 23)
#   define NV_PGRAPH_CTX_SWITCH1_PATCH_STATUS                  (1 << 24)
#   define NV_PGRAPH_CTX_SWITCH1_CONTEXT_SURFACE0              (1 << 25)
#   define NV_PGRAPH_CTX_SWITCH1_CONTEXT_SURFACE1              (1 << 26)
#   define NV_PGRAPH_CTX_SWITCH1_CONTEXT_PATTERN               (1 << 27)
#   define NV_PGRAPH_CTX_SWITCH1_CONTEXT_ROP                   (1 << 28)
#   define NV_PGRAPH_CTX_SWITCH1_CONTEXT_BETA1                 (1 << 29)
#   define NV_PGRAPH_CTX_SWITCH1_CONTEXT_BETA4                 (1 << 30)
#   define NV_PGRAPH_CTX_SWITCH1_VOLATILE_RESET                (1 << 31)
#define NV_PGRAPH_CTX_SWITCH2                            0x00000150
#define NV_PGRAPH_CTX_SWITCH3                            0x00000154
#define NV_PGRAPH_CTX_SWITCH4                            0x00000158
#   define NV_PGRAPH_CTX_SWITCH4_USER_INSTANCE                0x0000FFFF
#define NV_PGRAPH_CTX_SWITCH5                            0x0000015C
#define NV_PGRAPH_CTX_CACHE1                             0x00000160
#define NV_PGRAPH_CTX_CACHE2                             0x00000180
#define NV_PGRAPH_CTX_CACHE3                             0x000001A0
#define NV_PGRAPH_CTX_CACHE4                             0x000001C0
#define NV_PGRAPH_CTX_CACHE5                             0x000001E0
#define NV_PGRAPH_STATUS                                 0x00000700
#define NV_PGRAPH_TRAPPED_ADDR                           0x00000704
#   define NV_PGRAPH_TRAPPED_ADDR_MTHD                        0x00001FFF
#   define NV_PGRAPH_TRAPPED_ADDR_SUBCH                       0x00070000
#   define NV_PGRAPH_TRAPPED_ADDR_CHID                        0x01F00000
#   define NV_PGRAPH_TRAPPED_ADDR_DHV                         0x10000000
#define NV_PGRAPH_TRAPPED_DATA_LOW                       0x00000708
#define NV_PGRAPH_SURFACE                                0x00000710
#   define NV_PGRAPH_SURFACE_WRITE_3D                         0x00700000
#   define NV_PGRAPH_SURFACE_READ_3D                          0x07000000
#   define NV_PGRAPH_SURFACE_MODULO_3D                        0x70000000
#define NV_PGRAPH_INCREMENT                              0x0000071C
#   define NV_PGRAPH_INCREMENT_READ_BLIT                        (1 << 0)
#   define NV_PGRAPH_INCREMENT_READ_3D                          (1 << 1)
#define NV_PGRAPH_FIFO                                   0x00000720
#   define NV_PGRAPH_FIFO_ACCESS                                (1 << 0)
#define NV_PGRAPH_RDI_INDEX                              0x00000750
#   define NV_PGRAPH_RDI_INDEX_ADDRESS                        0x00001FFC
#   define NV_PGRAPH_RDI_INDEX_SELECT                         0x01FF0000
#define NV_PGRAPH_RDI_DATA                               0x00000754
#define NV_PGRAPH_FFINTFC_ST2                            0x00000764
#define NV_PGRAPH_CHANNEL_CTX_TABLE                      0x00000780
#   define NV_PGRAPH_CHANNEL_CTX_TABLE_INST                   0x0000FFFF
#define NV_PGRAPH_CHANNEL_CTX_POINTER                    0x00000784
#   define NV_PGRAPH_CHANNEL_CTX_POINTER_INST                 0x0000FFFF
#define NV_PGRAPH_CHANNEL_CTX_TRIGGER                    0x00000788
#   define NV_PGRAPH_CHANNEL_CTX_TRIGGER_READ_IN                (1 << 0)
#   define NV_PGRAPH_CHANNEL_CTX_TRIGGER_WRITE_OUT              (1 << 1)
#define NV_PGRAPH_DEBUG_2                                0x00000880
#define NV_PGRAPH_TTILE(i)                               0x00000900 + (i * 0x10)
#define NV_PGRAPH_TLIMIT(i)                              0x00000904 + (i * 0x10)
#define NV_PGRAPH_TSIZE(i)                               0x00000908 + (i * 0x10)
#define NV_PGRAPH_TSTATUS(i)                             0x0000090C + (i * 0x10)
#define NV_PGRAPH_ZCOMP(i)                               0x00000980 + (i * 4)
#define NV_PGRAPH_ZCOMP_OFFSET                           0x000009A0
#define NV_PGRAPH_FBCFG0                                 0x000009A4
#define NV_PGRAPH_FBCFG1                                 0x000009A8
#define NV_PGRAPH_PATT_COLOR0                            0x00000B10
#define NV_PGRAPH_DEBUG_6                                0x00000B80
#define NV_PGRAPH_DEBUG_7                                0x00000B84
#define NV_PGRAPH_DEBUG_10                               0x00000B88
#define NV_PGRAPH_CSV0_D                                 0x00000FB4
#   define NV_PGRAPH_CSV0_D_LIGHTS                              0x0000FFFF
#   define NV_PGRAPH_CSV0_D_LIGHT0                              0x00000003
#       define NV_PGRAPH_CSV0_D_LIGHT0_OFF                          0
#       define NV_PGRAPH_CSV0_D_LIGHT0_INFINITE                     1
#       define NV_PGRAPH_CSV0_D_LIGHT0_LOCAL                        2
#       define NV_PGRAPH_CSV0_D_LIGHT0_SPOT                         3
#   define NV_PGRAPH_CSV0_D_RANGE_MODE                          (1 << 18)
#   define NV_PGRAPH_CSV0_D_FOGENABLE                           (1 << 19)
#   define NV_PGRAPH_CSV0_D_TEXGEN_REF                          (1 << 20)
#       define NV_PGRAPH_CSV0_D_TEXGEN_REF_LOCAL_VIEWER             0
#       define NV_PGRAPH_CSV0_D_TEXGEN_REF_INFINITE_VIEWER          1
#   define NV_PGRAPH_CSV0_D_FOG_MODE                            (1 << 21)
#       define NV_PGRAPH_CSV0_D_FOG_MODE_LINEAR                     0
#       define NV_PGRAPH_CSV0_D_FOG_MODE_EXP                        1
#   define NV_PGRAPH_CSV0_D_FOGGENMODE                          0x01C00000
#       define NV_PGRAPH_CSV0_D_FOGGENMODE_SPEC_ALPHA               0
#       define NV_PGRAPH_CSV0_D_FOGGENMODE_RADIAL                   1
#       define NV_PGRAPH_CSV0_D_FOGGENMODE_PLANAR                   2
#       define NV_PGRAPH_CSV0_D_FOGGENMODE_ABS_PLANAR               3
#       define NV_PGRAPH_CSV0_D_FOGGENMODE_FOG_X                    4
#   define NV_PGRAPH_CSV0_D_MODE                                0xC0000000
#   define NV_PGRAPH_CSV0_D_SKIN                                0x1C000000
#       define NV_PGRAPH_CSV0_D_SKIN_OFF                            0
#       define NV_PGRAPH_CSV0_D_SKIN_2G                             1
#       define NV_PGRAPH_CSV0_D_SKIN_2                              2
#       define NV_PGRAPH_CSV0_D_SKIN_3G                             3
#       define NV_PGRAPH_CSV0_D_SKIN_3                              4
#       define NV_PGRAPH_CSV0_D_SKIN_4G                             5
#       define NV_PGRAPH_CSV0_D_SKIN_4                              6
#define NV_PGRAPH_CSV0_C                                 0x00000FB8
#   define NV_PGRAPH_CSV0_C_CHEOPS_PROGRAM_START                0x0000FF00
#   define NV_PGRAPH_CSV0_C_SPECULAR                            (3 << 19)
#   define NV_PGRAPH_CSV0_C_DIFFUSE                             (3 << 21)
#   define NV_PGRAPH_CSV0_C_AMBIENT                             (3 << 23)
#   define NV_PGRAPH_CSV0_C_EMISSION                            (3 << 25)
#   define NV_PGRAPH_CSV0_C_NORMALIZATION_ENABLE                (1 << 27)
#   define NV_PGRAPH_CSV0_C_LIGHTING                            (1 << 31)
#define NV_PGRAPH_CSV1_B                                 0x00000FBC
#define NV_PGRAPH_CSV1_A                                 0x00000FC0
#   define NV_PGRAPH_CSV1_A_T0_ENABLE                           (1 << 0)
#   define NV_PGRAPH_CSV1_A_T0_MODE                             (1 << 1)
#   define NV_PGRAPH_CSV1_A_T0_TEXTURE                          (1 << 2)
#       define NV_PGRAPH_CSV1_A_T0_TEXTURE_2D                       0
#       define NV_PGRAPH_CSV1_A_T0_TEXTURE_3D                       1
#   define NV_PGRAPH_CSV1_A_T0_S                                0x00000070
#       define NV_PGRAPH_CSV1_A_T0_S_DISABLE                        0
#       define NV_PGRAPH_CSV1_A_T0_S_NORMAL_MAP                     4
#       define NV_PGRAPH_CSV1_A_T0_S_REFLECTION_MAP                 5
#       define NV_PGRAPH_CSV1_A_T0_S_EYE_LINEAR                     1
#       define NV_PGRAPH_CSV1_A_T0_S_OBJECT_LINEAR                  2
#       define NV_PGRAPH_CSV1_A_T0_S_SPHERE_MAP                     3
#   define NV_PGRAPH_CSV1_A_T0_T                                0x00000380
#   define NV_PGRAPH_CSV1_A_T0_R                                0x00001C00
#   define NV_PGRAPH_CSV1_A_T0_Q                                0x0000E000
#   define NV_PGRAPH_CSV1_A_T1_ENABLE                           (1 << 16)
#   define NV_PGRAPH_CSV1_A_T1_MODE                             (1 << 17)
#   define NV_PGRAPH_CSV1_A_T1_TEXTURE                          (1 << 18)
#   define NV_PGRAPH_CSV1_A_T1_S                                0x00700000
#   define NV_PGRAPH_CSV1_A_T1_T                                0x03800000
#   define NV_PGRAPH_CSV1_A_T1_R                                0x1C000000
#   define NV_PGRAPH_CSV1_A_T1_Q                                0xE0000000
#define NV_PGRAPH_CHEOPS_OFFSET                          0x00000FC4
#   define NV_PGRAPH_CHEOPS_OFFSET_PROG_LD_PTR                  0x000000FF
#   define NV_PGRAPH_CHEOPS_OFFSET_CONST_LD_PTR                 0x0000FF00
#define NV_PGRAPH_DMA_STATE                              0x00001034
#define NV_PGRAPH_BLEND                                  0x00001804
#   define NV_PGRAPH_BLEND_EQN                                  0x00000007
#   define NV_PGRAPH_BLEND_EN                                   (1 << 3)
#   define NV_PGRAPH_BLEND_SFACTOR                              0x000000F0
#       define NV_PGRAPH_BLEND_SFACTOR_ZERO                         0
#       define NV_PGRAPH_BLEND_SFACTOR_ONE                          1
#       define NV_PGRAPH_BLEND_SFACTOR_SRC_COLOR                    2
#       define NV_PGRAPH_BLEND_SFACTOR_ONE_MINUS_SRC_COLOR          3
#       define NV_PGRAPH_BLEND_SFACTOR_SRC_ALPHA                    4
#       define NV_PGRAPH_BLEND_SFACTOR_ONE_MINUS_SRC_ALPHA          5
#       define NV_PGRAPH_BLEND_SFACTOR_DST_ALPHA                    6
#       define NV_PGRAPH_BLEND_SFACTOR_ONE_MINUS_DST_ALPHA          7
#       define NV_PGRAPH_BLEND_SFACTOR_DST_COLOR                    8
#       define NV_PGRAPH_BLEND_SFACTOR_ONE_MINUS_DST_COLOR          9
#       define NV_PGRAPH_BLEND_SFACTOR_SRC_ALPHA_SATURATE           10
#       define NV_PGRAPH_BLEND_SFACTOR_CONSTANT_COLOR               12
#       define NV_PGRAPH_BLEND_SFACTOR_ONE_MINUS_CONSTANT_COLOR     13
#       define NV_PGRAPH_BLEND_SFACTOR_CONSTANT_ALPHA               14
#       define NV_PGRAPH_BLEND_SFACTOR_ONE_MINUS_CONSTANT_ALPHA     15
#   define NV_PGRAPH_BLEND_DFACTOR                              0x00000F00
#       define NV_PGRAPH_BLEND_DFACTOR_ZERO                         0
#       define NV_PGRAPH_BLEND_DFACTOR_ONE                          1
#       define NV_PGRAPH_BLEND_DFACTOR_SRC_COLOR                    2
#       define NV_PGRAPH_BLEND_DFACTOR_ONE_MINUS_SRC_COLOR          3
#       define NV_PGRAPH_BLEND_DFACTOR_SRC_ALPHA                    4
#       define NV_PGRAPH_BLEND_DFACTOR_ONE_MINUS_SRC_ALPHA          5
#       define NV_PGRAPH_BLEND_DFACTOR_DST_ALPHA                    6
#       define NV_PGRAPH_BLEND_DFACTOR_ONE_MINUS_DST_ALPHA          7
#       define NV_PGRAPH_BLEND_DFACTOR_DST_COLOR                    8
#       define NV_PGRAPH_BLEND_DFACTOR_ONE_MINUS_DST_COLOR          9
#       define NV_PGRAPH_BLEND_DFACTOR_SRC_ALPHA_SATURATE           10
#       define NV_PGRAPH_BLEND_DFACTOR_CONSTANT_COLOR               12
#       define NV_PGRAPH_BLEND_DFACTOR_ONE_MINUS_CONSTANT_COLOR     13
#       define NV_PGRAPH_BLEND_DFACTOR_CONSTANT_ALPHA               14
#       define NV_PGRAPH_BLEND_DFACTOR_ONE_MINUS_CONSTANT_ALPHA     15
#   define NV_PGRAPH_BLEND_LOGICOP_ENABLE                       (1 << 16)
#   define NV_PGRAPH_BLEND_LOGICOP                              0x0000F000
#define NV_PGRAPH_BLENDCOLOR                             0x00001808
#define NV_PGRAPH_BORDERCOLOR0                           0x0000180C
#define NV_PGRAPH_BORDERCOLOR1                           0x00001810
#define NV_PGRAPH_BORDERCOLOR2                           0x00001814
#define NV_PGRAPH_BORDERCOLOR3                           0x00001818
#define NV_PGRAPH_BUMPOFFSET1                            0x0000184C
#define NV_PGRAPH_BUMPSCALE1                             0x00001858
#define NV_PGRAPH_CLEARRECTX                             0x00001864
#       define NV_PGRAPH_CLEARRECTX_XMIN                          0x00000FFF
#       define NV_PGRAPH_CLEARRECTX_XMAX                          0x0FFF0000
#define NV_PGRAPH_CLEARRECTY                             0x00001868
#       define NV_PGRAPH_CLEARRECTY_YMIN                          0x00000FFF
#       define NV_PGRAPH_CLEARRECTY_YMAX                          0x0FFF0000
#define NV_PGRAPH_COLORCLEARVALUE                        0x0000186C
#define NV_PGRAPH_COMBINEFACTOR0                         0x00001880
#define NV_PGRAPH_COMBINEFACTOR1                         0x000018A0
#define NV_PGRAPH_COMBINEALPHAI0                         0x000018C0
#define NV_PGRAPH_COMBINEALPHAO0                         0x000018E0
#define NV_PGRAPH_COMBINECOLORI0                         0x00001900
#define NV_PGRAPH_COMBINECOLORO0                         0x00001920
#define NV_PGRAPH_COMBINECTL                             0x00001940
#define NV_PGRAPH_COMBINESPECFOG0                        0x00001944
#define NV_PGRAPH_COMBINESPECFOG1                        0x00001948
#define NV_PGRAPH_CONTROL_0                              0x0000194C
#   define NV_PGRAPH_CONTROL_0_ALPHAREF                         0x000000FF
#   define NV_PGRAPH_CONTROL_0_ALPHAFUNC                        0x00000F00
#   define NV_PGRAPH_CONTROL_0_ALPHATESTENABLE                  (1 << 12)
#   define NV_PGRAPH_CONTROL_0_ZENABLE                          (1 << 14)
#   define NV_PGRAPH_CONTROL_0_ZFUNC                            0x000F0000
#       define NV_PGRAPH_CONTROL_0_ZFUNC_NEVER                      0
#       define NV_PGRAPH_CONTROL_0_ZFUNC_LESS                       1
#       define NV_PGRAPH_CONTROL_0_ZFUNC_EQUAL                      2
#       define NV_PGRAPH_CONTROL_0_ZFUNC_LEQUAL                     3
#       define NV_PGRAPH_CONTROL_0_ZFUNC_GREATER                    4
#       define NV_PGRAPH_CONTROL_0_ZFUNC_NOTEQUAL                   5
#       define NV_PGRAPH_CONTROL_0_ZFUNC_GEQUAL                     6
#       define NV_PGRAPH_CONTROL_0_ZFUNC_ALWAYS                     7
#   define NV_PGRAPH_CONTROL_0_DITHERENABLE                     (1 << 22)
#   define NV_PGRAPH_CONTROL_0_Z_PERSPECTIVE_ENABLE             (1 << 23)
#   define NV_PGRAPH_CONTROL_0_ZWRITEENABLE                     (1 << 24)
#   define NV_PGRAPH_CONTROL_0_STENCIL_WRITE_ENABLE             (1 << 25)
#   define NV_PGRAPH_CONTROL_0_ALPHA_WRITE_ENABLE               (1 << 26)
#   define NV_PGRAPH_CONTROL_0_RED_WRITE_ENABLE                 (1 << 27)
#   define NV_PGRAPH_CONTROL_0_GREEN_WRITE_ENABLE               (1 << 28)
#   define NV_PGRAPH_CONTROL_0_BLUE_WRITE_ENABLE                (1 << 29)
#   define NV_PGRAPH_CONTROL_0_CSCONVERT                        (3 << 30)
#define NV_PGRAPH_CONTROL_1                              0x00001950
#   define NV_PGRAPH_CONTROL_1_STENCIL_TEST_ENABLE              (1 << 0)
#   define NV_PGRAPH_CONTROL_1_STENCIL_FUNC                     0x000000F0
#       define NV_PGRAPH_CONTROL_1_STENCIL_FUNC_NEVER               0
#       define NV_PGRAPH_CONTROL_1_STENCIL_FUNC_LESS                1
#       define NV_PGRAPH_CONTROL_1_STENCIL_FUNC_EQUAL               2
#       define NV_PGRAPH_CONTROL_1_STENCIL_FUNC_LEQUAL              3
#       define NV_PGRAPH_CONTROL_1_STENCIL_FUNC_GREATER             4
#       define NV_PGRAPH_CONTROL_1_STENCIL_FUNC_NOTEQUAL            5
#       define NV_PGRAPH_CONTROL_1_STENCIL_FUNC_GEQUAL              6
#       define NV_PGRAPH_CONTROL_1_STENCIL_FUNC_ALWAYS              7
#   define NV_PGRAPH_CONTROL_1_STENCIL_REF                      0x0000FF00
#   define NV_PGRAPH_CONTROL_1_STENCIL_MASK_READ                0x00FF0000
#   define NV_PGRAPH_CONTROL_1_STENCIL_MASK_WRITE               0xFF000000
#define NV_PGRAPH_CONTROL_2                              0x00001954
#   define NV_PGRAPH_CONTROL_2_STENCIL_OP_FAIL                  0x0000000F
#   define NV_PGRAPH_CONTROL_2_STENCIL_OP_ZFAIL                 0x000000F0
#   define NV_PGRAPH_CONTROL_2_STENCIL_OP_ZPASS                 0x00000F00
#       define NV_PGRAPH_CONTROL_2_STENCIL_OP_V_KEEP                1
#       define NV_PGRAPH_CONTROL_2_STENCIL_OP_V_ZERO                2
#       define NV_PGRAPH_CONTROL_2_STENCIL_OP_V_REPLACE             3
#       define NV_PGRAPH_CONTROL_2_STENCIL_OP_V_INCRSAT             4
#       define NV_PGRAPH_CONTROL_2_STENCIL_OP_V_DECRSAT             5
#       define NV_PGRAPH_CONTROL_2_STENCIL_OP_V_INVERT              6
#       define NV_PGRAPH_CONTROL_2_STENCIL_OP_V_INCR                7
#       define NV_PGRAPH_CONTROL_2_STENCIL_OP_V_DECR                8
#define NV_PGRAPH_CONTROL_3                              0x00001958
#   define NV_PGRAPH_CONTROL_3_FOGENABLE                        (1 << 8)
#   define NV_PGRAPH_CONTROL_3_FOG_MODE                         0x00070000
#       define NV_PGRAPH_CONTROL_3_FOG_MODE_LINEAR                  0
#       define NV_PGRAPH_CONTROL_3_FOG_MODE_EXP                     1
#       define NV_PGRAPH_CONTROL_3_FOG_MODE_EXP2                    3
#       define NV_PGRAPH_CONTROL_3_FOG_MODE_EXP_ABS                 5
#       define NV_PGRAPH_CONTROL_3_FOG_MODE_EXP2_ABS                7
#       define NV_PGRAPH_CONTROL_3_FOG_MODE_LINEAR_ABS              4
#define NV_PGRAPH_FOGCOLOR                               0x00001980
#   define NV_PGRAPH_FOGCOLOR_RED                               0x00FF0000
#   define NV_PGRAPH_FOGCOLOR_GREEN                             0x0000FF00
#   define NV_PGRAPH_FOGCOLOR_BLUE                              0x000000FF
#   define NV_PGRAPH_FOGCOLOR_ALPHA                             0xFF000000
#define NV_PGRAPH_FOGPARAM0                              0x00001984
#define NV_PGRAPH_FOGPARAM1                              0x00001988
#define NV_PGRAPH_SETUPRASTER                            0x00001990
#   define NV_PGRAPH_SETUPRASTER_FRONTFACEMODE                  0x00000003
#       define NV_PGRAPH_SETUPRASTER_FRONTFACEMODE_FILL             0
#       define NV_PGRAPH_SETUPRASTER_FRONTFACEMODE_POINT            1
#       define NV_PGRAPH_SETUPRASTER_FRONTFACEMODE_LINE             2
#   define NV_PGRAPH_SETUPRASTER_BACKFACEMODE                   0x0000000C
#   define NV_PGRAPH_SETUPRASTER_POFFSETPOINTENABLE             (1 << 6)
#   define NV_PGRAPH_SETUPRASTER_POFFSETLINEENABLE              (1 << 7)
#   define NV_PGRAPH_SETUPRASTER_POFFSETFILLENABLE              (1 << 8)
#   define NV_PGRAPH_SETUPRASTER_LINESMOOTHENABLE               (1 << 10)
#   define NV_PGRAPH_SETUPRASTER_POLYSMOOTHENABLE               (1 << 11)
#   define NV_PGRAPH_SETUPRASTER_CULLCTRL                       0x00600000
#       define NV_PGRAPH_SETUPRASTER_CULLCTRL_FRONT                 1
#       define NV_PGRAPH_SETUPRASTER_CULLCTRL_BACK                  2
#       define NV_PGRAPH_SETUPRASTER_CULLCTRL_FRONT_AND_BACK        3
#   define NV_PGRAPH_SETUPRASTER_FRONTFACE                      (1 << 23)
#   define NV_PGRAPH_SETUPRASTER_CULLENABLE                     (1 << 28)
#   define NV_PGRAPH_SETUPRASTER_Z_FORMAT                       (1 << 29)
#   define NV_PGRAPH_SETUPRASTER_WINDOWCLIPTYPE                 (1 << 31)
#define NV_PGRAPH_SHADERCLIPMODE                         0x00001994
#define NV_PGRAPH_SHADERCTL                              0x00001998
#define NV_PGRAPH_SHADERPROG                             0x0000199C
#define NV_PGRAPH_SEMAPHOREOFFSET                        0x000019A0
#define NV_PGRAPH_SHADOWZSLOPETHRESHOLD                  0x000019A8
#define NV_PGRAPH_SPECFOGFACTOR0                         0x000019AC
#define NV_PGRAPH_SPECFOGFACTOR1                         0x000019B0
#define NV_PGRAPH_TEXADDRESS0                            0x000019BC
#   define NV_PGRAPH_TEXADDRESS0_ADDRU                          0x00000007
#       define NV_PGRAPH_TEXADDRESS0_ADDRU_WRAP                      1
#       define NV_PGRAPH_TEXADDRESS0_ADDRU_MIRROR                    2
#       define NV_PGRAPH_TEXADDRESS0_ADDRU_CLAMP_TO_EDGE             3
#       define NV_PGRAPH_TEXADDRESS0_ADDRU_BORDER                    4
#       define NV_PGRAPH_TEXADDRESS0_ADDRU_CLAMP_OGL                 5
#   define NV_PGRAPH_TEXADDRESS0_WRAP_U                         (1 << 4)
#   define NV_PGRAPH_TEXADDRESS0_ADDRV                          0x00000700
#   define NV_PGRAPH_TEXADDRESS0_WRAP_V                         (1 << 12)
#   define NV_PGRAPH_TEXADDRESS0_ADDRP                          0x00070000
#   define NV_PGRAPH_TEXADDRESS0_WRAP_P                         (1 << 20)
#   define NV_PGRAPH_TEXADDRESS0_WRAP_Q                         (1 << 24)
#define NV_PGRAPH_TEXADDRESS1                            0x000019C0
#define NV_PGRAPH_TEXADDRESS2                            0x000019C4
#define NV_PGRAPH_TEXADDRESS3                            0x000019C8
#define NV_PGRAPH_TEXCTL0_0                              0x000019CC
#   define NV_PGRAPH_TEXCTL0_0_ALPHAKILLEN                      (1 << 2)
#   define NV_PGRAPH_TEXCTL0_0_MAX_LOD_CLAMP                    0x0003FFC0
#   define NV_PGRAPH_TEXCTL0_0_MIN_LOD_CLAMP                    0x3FFC0000
#   define NV_PGRAPH_TEXCTL0_0_ENABLE                           (1 << 30)
#define NV_PGRAPH_TEXCTL0_1                              0x000019D0
#define NV_PGRAPH_TEXCTL0_2                              0x000019D4
#define NV_PGRAPH_TEXCTL0_3                              0x000019D8
#define NV_PGRAPH_TEXCTL1_0                              0x000019DC
#   define NV_PGRAPH_TEXCTL1_0_IMAGE_PITCH                      0xFFFF0000
#define NV_PGRAPH_TEXCTL1_1                              0x000019E0
#define NV_PGRAPH_TEXCTL1_2                              0x000019E4
#define NV_PGRAPH_TEXCTL1_3                              0x000019E8
#define NV_PGRAPH_TEXCTL2_0                              0x000019EC
#define NV_PGRAPH_TEXCTL2_1                              0x000019F0
#define NV_PGRAPH_TEXFILTER0                             0x000019F4
#   define NV_PGRAPH_TEXFILTER0_MIPMAP_LOD_BIAS                 0x00001FFF
#   define NV_PGRAPH_TEXFILTER0_CONVOLUTION_KERNEL              0x0000E000
#       define NV_PGRAPH_TEXFILTER0_CONVOLUTION_KERNEL_QUINCUNX     1
#       define NV_PGRAPH_TEXFILTER0_CONVOLUTION_KERNEL_GAUSSIAN_3   2
#   define NV_PGRAPH_TEXFILTER0_MIN                             0x003F0000
#       define NV_PGRAPH_TEXFILTER0_MIN_BOX_LOD0                    1
#       define NV_PGRAPH_TEXFILTER0_MIN_TENT_LOD0                   2
#       define NV_PGRAPH_TEXFILTER0_MIN_BOX_NEARESTLOD              3
#       define NV_PGRAPH_TEXFILTER0_MIN_TENT_NEARESTLOD             4
#       define NV_PGRAPH_TEXFILTER0_MIN_BOX_TENT_LOD                5
#       define NV_PGRAPH_TEXFILTER0_MIN_TENT_TENT_LOD               6
#       define NV_PGRAPH_TEXFILTER0_MIN_CONVOLUTION_2D_LOD0         7
#   define NV_PGRAPH_TEXFILTER0_MAG                             0x0F000000
#   define NV_PGRAPH_TEXFILTER0_ASIGNED                         (1 << 28)
#   define NV_PGRAPH_TEXFILTER0_RSIGNED                         (1 << 29)
#   define NV_PGRAPH_TEXFILTER0_GSIGNED                         (1 << 30)
#   define NV_PGRAPH_TEXFILTER0_BSIGNED                         (1 << 31)
#define NV_PGRAPH_TEXFILTER1                             0x000019F8
#define NV_PGRAPH_TEXFILTER2                             0x000019FC
#define NV_PGRAPH_TEXFILTER3                             0x00001A00
#define NV_PGRAPH_TEXFMT0                                0x00001A04
#   define NV_PGRAPH_TEXFMT0_CONTEXT_DMA                        (1 << 1)
#   define NV_PGRAPH_TEXFMT0_CUBEMAPENABLE                      (1 << 2)
#   define NV_PGRAPH_TEXFMT0_BORDER_SOURCE                      (1 << 3)
#       define NV_PGRAPH_TEXFMT0_BORDER_SOURCE_TEXTURE              0
#       define NV_PGRAPH_TEXFMT0_BORDER_SOURCE_COLOR                1
#   define NV_PGRAPH_TEXFMT0_DIMENSIONALITY                     0x000000C0
#   define NV_PGRAPH_TEXFMT0_COLOR                              0x00007F00
#   define NV_PGRAPH_TEXFMT0_MIPMAP_LEVELS                      0x000F0000
#   define NV_PGRAPH_TEXFMT0_BASE_SIZE_U                        0x00F00000
#   define NV_PGRAPH_TEXFMT0_BASE_SIZE_V                        0x0F000000
#   define NV_PGRAPH_TEXFMT0_BASE_SIZE_P                        0xF0000000
#define NV_PGRAPH_TEXFMT1                                0x00001A08
#define NV_PGRAPH_TEXFMT2                                0x00001A0C
#define NV_PGRAPH_TEXFMT3                                0x00001A10
#define NV_PGRAPH_TEXIMAGERECT0                          0x00001A14
#   define NV_PGRAPH_TEXIMAGERECT0_WIDTH                        0x1FFF0000
#   define NV_PGRAPH_TEXIMAGERECT0_HEIGHT                       0x00001FFF
#define NV_PGRAPH_TEXIMAGERECT1                          0x00001A18
#define NV_PGRAPH_TEXIMAGERECT2                          0x00001A1C
#define NV_PGRAPH_TEXIMAGERECT3                          0x00001A20
#define NV_PGRAPH_TEXOFFSET0                             0x00001A24
#define NV_PGRAPH_TEXOFFSET1                             0x00001A28
#define NV_PGRAPH_TEXOFFSET2                             0x00001A2C
#define NV_PGRAPH_TEXOFFSET3                             0x00001A30
#define NV_PGRAPH_TEXPALETTE0                            0x00001A34
#   define NV_PGRAPH_TEXPALETTE0_CONTEXT_DMA                    (1 << 0)
#   define NV_PGRAPH_TEXPALETTE0_LENGTH                         0x0000000C
#       define NV_PGRAPH_TEXPALETTE0_LENGTH_256                     0
#       define NV_PGRAPH_TEXPALETTE0_LENGTH_128                     1
#       define NV_PGRAPH_TEXPALETTE0_LENGTH_64                      2
#       define NV_PGRAPH_TEXPALETTE0_LENGTH_32                      3
#   define NV_PGRAPH_TEXPALETTE0_OFFSET                         0xFFFFFFC0
#define NV_PGRAPH_TEXPALETTE1                            0x00001A38
#define NV_PGRAPH_TEXPALETTE2                            0x00001A3C
#define NV_PGRAPH_TEXPALETTE3                            0x00001A40
#define NV_PGRAPH_WINDOWCLIPX0                           0x00001A44
#   define NV_PGRAPH_WINDOWCLIPX0_XMIN                          0x00000FFF
#   define NV_PGRAPH_WINDOWCLIPX0_XMAX                          0x0FFF0000
#define NV_PGRAPH_WINDOWCLIPX1                           0x00001A48
#define NV_PGRAPH_WINDOWCLIPX2                           0x00001A4C
#define NV_PGRAPH_WINDOWCLIPX3                           0x00001A50
#define NV_PGRAPH_WINDOWCLIPX4                           0x00001A54
#define NV_PGRAPH_WINDOWCLIPX5                           0x00001A58
#define NV_PGRAPH_WINDOWCLIPX6                           0x00001A5C
#define NV_PGRAPH_WINDOWCLIPX7                           0x00001A60
#define NV_PGRAPH_WINDOWCLIPY0                           0x00001A64
#   define NV_PGRAPH_WINDOWCLIPY0_YMIN                          0x00000FFF
#   define NV_PGRAPH_WINDOWCLIPY0_YMAX                          0x0FFF0000
#define NV_PGRAPH_WINDOWCLIPY1                           0x00001A68
#define NV_PGRAPH_WINDOWCLIPY2                           0x00001A6C
#define NV_PGRAPH_WINDOWCLIPY3                           0x00001A70
#define NV_PGRAPH_WINDOWCLIPY4                           0x00001A74
#define NV_PGRAPH_WINDOWCLIPY5                           0x00001A78
#define NV_PGRAPH_WINDOWCLIPY6                           0x00001A7C
#define NV_PGRAPH_WINDOWCLIPY7                           0x00001A80
#define NV_PGRAPH_ZSTENCILCLEARVALUE                     0x00001A88
#define NV_PGRAPH_ZCLIPMIN                               0x00001A90
#define NV_PGRAPH_ZOFFSETBIAS                            0x00001AA4
#define NV_PGRAPH_ZOFFSETFACTOR                          0x00001AA8
#define NV_PGRAPH_EYEVEC0                                0x00001AAC
#define NV_PGRAPH_EYEVEC1                                0x00001AB0
#define NV_PGRAPH_EYEVEC2                                0x00001AB4
#define NV_PGRAPH_ZCLIPMAX                               0x00001ABC


#define NV_PCRTC_INTR_0                                  0x00000100
#   define NV_PCRTC_INTR_0_VBLANK                               (1 << 0)
#define NV_PCRTC_INTR_EN_0                               0x00000140
#   define NV_PCRTC_INTR_EN_0_VBLANK                            (1 << 0)
#define NV_PCRTC_START                                   0x00000800
#define NV_PCRTC_CONFIG                                  0x00000804
#define NV_PCRTC_RASTER                                  0x00000808


#define NV_PVIDEO_DEBUG_2                                0x00000088
#define NV_PVIDEO_DEBUG_3                                0x0000008C
#define NV_PVIDEO_INTR                                   0x00000100
#   define NV_PVIDEO_INTR_BUFFER_0                              (1 << 0)
#   define NV_PVIDEO_INTR_BUFFER_1                              (1 << 4)
#define NV_PVIDEO_INTR_EN                                0x00000140
#   define NV_PVIDEO_INTR_EN_BUFFER_0                           (1 << 0)
#   define NV_PVIDEO_INTR_EN_BUFFER_1                           (1 << 4)
#define NV_PVIDEO_BUFFER                                 0x00000700
#   define NV_PVIDEO_BUFFER_0_USE                               (1 << 0)
#   define NV_PVIDEO_BUFFER_1_USE                               (1 << 4)
#define NV_PVIDEO_STOP                                   0x00000704
#define NV_PVIDEO_BASE(i)                                0x00000900 + (i * 4)
#define NV_PVIDEO_LIMIT(i)                               0x00000908 + (i * 4)
#define NV_PVIDEO_LUMINANCE(i)                           0x00000910 + (i * 4)
#define NV_PVIDEO_CHROMINANCE(i)                         0x00000918 + (i * 4)
#define NV_PVIDEO_OFFSET(i)                              0x00000920 + (i * 4)
#define NV_PVIDEO_SIZE_IN(i)                             0x00000928 + (i * 4)
#   define NV_PVIDEO_SIZE_IN_WIDTH                            0x000007FF
#   define NV_PVIDEO_SIZE_IN_HEIGHT                           0x07FF0000
#define NV_PVIDEO_POINT_IN(i)                            0x00000930 + (i * 4)
#   define NV_PVIDEO_POINT_IN_S                               0x00007FFF
#   define NV_PVIDEO_POINT_IN_T                               0xFFFE0000
#define NV_PVIDEO_DS_DX(i)                               0x00000938 + (i * 4)
#define NV_PVIDEO_DT_DY(i)                               0x00000940 + (i * 4)
#define NV_PVIDEO_POINT_OUT(i)                           0x00000948 + (i * 4)
#   define NV_PVIDEO_POINT_OUT_X                              0x00000FFF
#   define NV_PVIDEO_POINT_OUT_Y                              0x0FFF0000
#define NV_PVIDEO_SIZE_OUT(i)                            0x00000950 + (i * 4)
#   define NV_PVIDEO_SIZE_OUT_WIDTH                           0x00000FFF
#   define NV_PVIDEO_SIZE_OUT_HEIGHT                          0x0FFF0000
#define NV_PVIDEO_FORMAT(i)                              0x00000958 + (i * 4)
#   define NV_PVIDEO_FORMAT_PITCH                             0x00001FFF
#   define NV_PVIDEO_FORMAT_COLOR                             0x00030000
#       define NV_PVIDEO_FORMAT_COLOR_LE_CR8YB8CB8YA8             1
#   define NV_PVIDEO_FORMAT_DISPLAY                            (1 << 20)
#define NV_PVIDEO_COLOR_KEY(i)                           0x00000b00 + (i * 4)


#define NV_PTIMER_INTR_0                                 0x00000100
#   define NV_PTIMER_INTR_0_ALARM                               (1 << 0)
#define NV_PTIMER_INTR_EN_0                              0x00000140
#   define NV_PTIMER_INTR_EN_0_ALARM                            (1 << 0)
#define NV_PTIMER_NUMERATOR                              0x00000200
#define NV_PTIMER_DENOMINATOR                            0x00000210
#define NV_PTIMER_TIME_0                                 0x00000400
#define NV_PTIMER_TIME_1                                 0x00000410
#define NV_PTIMER_ALARM_0                                0x00000420

#define NV_PRMVIO_MISC__WRITE		0x000c03c2
#define NV_PRMVIO_SRX			0x000c03c4
#define NV_PRMVIO_SR			0x000c03c5
#	define NV_VIO_SR_RESET_INDEX		0x00
#	define NV_VIO_SR_CLOCK_INDEX		0x01
#	define NV_VIO_SR_PLANE_MASK_INDEX	0x02
#	define NV_VIO_SR_CHAR_MAP_INDEX		0x03
#	define NV_VIO_SR_MEM_MODE_INDEX		0x04
#define NV_PRMVIO_MISC__READ		0x000c03cc
#define NV_PRMVIO_GRX			0x000c03ce
#define NV_PRMVIO_GX			0x000c03cf
#	define NV_VIO_GX_SR_INDEX		0x00
#	define NV_VIO_GX_SREN_INDEX		0x01
#	define NV_VIO_GX_CCOMP_INDEX		0x02
#	define NV_VIO_GX_ROP_INDEX		0x03
#	define NV_VIO_GX_READ_MAP_INDEX		0x04
#	define NV_VIO_GX_MODE_INDEX		0x05
#	define NV_VIO_GX_MISC_INDEX		0x06
#	define NV_VIO_GX_DONT_CARE_INDEX	0x07
#	define NV_VIO_GX_BIT_MASK_INDEX		0x08

#define NV_PFB_DEBUG_0                                   0x00000080
#define NV_PFB_CFG0                                      0x00000200
#   define NV_PFB_CFG0_PART                                   0x00000003
#define NV_PFB_CFG1                                      0x00000204
#define NV_PFB_CSTATUS                                   0x0000020C
#define NV_PFB_REFCTRL                                   0x00000210
#define NV_PFB_NVM                                       0x00000214 // 	NV_PFB_NVM_MODE_DISABLE 
#define NV_PFB_PIN                                       0x00000218
#define NV_PFB_PAD                                       0x0000021C
#define NV_PFB_TIMING0                                   0x00000220
#define NV_PFB_TIMING1                                   0x00000224
#define NV_PFB_TIMING2                                   0x00000228
#define NV_PFB_TILE(i)                                   0x00000240 + (i * 0x10)
#define NV_PFB_TLIMIT(i)                                 0x00000244 + (i * 0x10)
#define NV_PFB_TSIZE(i)                                  0x00000248 + (i * 0x10)
#define NV_PFB_TSTATUS(i)                                0x0000024C + (i * 0x10)
#define NV_PFB_MRS                                       0x000002C0
#define NV_PFB_EMRS                                      0x000002C4
#define NV_PFB_MRS_EXT                                   0x000002C8
#define NV_PFB_EMRS_EXT                                  0x000002CC
#define NV_PFB_REF                                       0x000002D0
#define NV_PFB_PRE                                       0x000002D4
#define NV_PFB_ZCOMP(i)                                  0x00000300 + (i * 4)
#define NV_PFB_ZCOMP_OFFSET                              0x00000324
#define NV_PFB_ARB_PREDIVIDER                            0x00000328
#define NV_PFB_ARB_TIMEOUT                               0x0000032C
#define NV_PFB_ARB_XFER_REM                              0x00000334
#define NV_PFB_ARB_DIFF_BANK                             0x00000338
#define NV_PFB_CLOSE_PAGE0                               0x00000340
#define NV_PFB_CLOSE_PAGE1                               0x00000344
#define NV_PFB_CLOSE_PAGE2                               0x00000348
#define NV_PFB_BPARB                                     0x0000034C
#define NV_PFB_CMDQ0                                     0x00000350
#define NV_PFB_CMDQ1                                     0x00000354
#define NV_PFB_ILL_INSTR                    0x00000360
#define NV_PFB_RT                                        0x00000400
#define NV_PFB_AUTOCLOSE                                 0x00000404
#define NV_PFB_WBC                                       0x00000410
#   define NV_PFB_WBC_FLUSH                                     (1 << 16)
#define NV_PFB_CMDQ_PRT                                  0x00000418
#define NV_PFB_CPU_RRQ                                   0x00000420
#define NV_PFB_BYPASS                                    0x00000424

#define NV_PRAMIN_DMA_CLASS(i)                           0x00000000 + (i * 0x10)
#define NV_PRAMIN_DMA_LIMIT(i)                           0x00000004 + (i * 0x10)
#define NV_PRAMIN_DMA_START(i)                           0x00000008 + (i * 0x10)
#define NV_PRAMIN_DMA_ADDRESS(i)                         0x0000000C + (i * 0x10)

#define NV_PRAMDAC_NVPLL_COEFF                           0x00000500
#   define NV_PRAMDAC_NVPLL_COEFF_MDIV                        0x000000FF
#   define NV_PRAMDAC_NVPLL_COEFF_NDIV                        0x0000FF00
#   define NV_PRAMDAC_NVPLL_COEFF_PDIV                        0x00070000
#define NV_PRAMDAC_MPLL_COEFF                            0x00000504
#   define NV_PRAMDAC_MPLL_COEFF_MDIV                         0x000000FF
#   define NV_PRAMDAC_MPLL_COEFF_NDIV                         0x0000FF00
#   define NV_PRAMDAC_MPLL_COEFF_PDIV                         0x00070000
#define NV_PRAMDAC_VPLL_COEFF                            0x00000508
#   define NV_PRAMDAC_VPLL_COEFF_MDIV                         0x000000FF
#   define NV_PRAMDAC_VPLL_COEFF_NDIV                         0x0000FF00
#   define NV_PRAMDAC_VPLL_COEFF_PDIV                         0x00070000
#define NV_PRAMDAC_PLL_TEST_COUNTER                      0x00000514
#   define NV_PRAMDAC_PLL_TEST_COUNTER_NOOFIPCLKS             0x000003FF
#   define NV_PRAMDAC_PLL_TEST_COUNTER_VALUE                  0x0000FFFF
#   define NV_PRAMDAC_PLL_TEST_COUNTER_ENABLE                  (1 << 16)
#   define NV_PRAMDAC_PLL_TEST_COUNTER_RESET                   (1 << 20)
#   define NV_PRAMDAC_PLL_TEST_COUNTER_SOURCE                 0x03000000
#   define NV_PRAMDAC_PLL_TEST_COUNTER_VPLL2_LOCK              (1 << 27)
#   define NV_PRAMDAC_PLL_TEST_COUNTER_PDIV_RST                (1 << 28)
#   define NV_PRAMDAC_PLL_TEST_COUNTER_NVPLL_LOCK              (1 << 29)
#   define NV_PRAMDAC_PLL_TEST_COUNTER_MPLL_LOCK               (1 << 30)
#   define NV_PRAMDAC_PLL_TEST_COUNTER_VPLL_LOCK               (1 << 31)
#define NV_PRAMDAC_GENERAL_CONTROL                       0x00000600
#	define NV_PRAMDAC_GENERAL_CONTROL_PIXMIX_ON		(3 << 4)
#	define NV_PRAMDAC_GENERAL_CONTROL_VGA_STATE_SEL		(1 << 8)
#	define NV_PRAMDAC_GENERAL_CONTROL_ALT_MODE_SEL		(1 << 12)
#	define NV_PRAMDAC_GENERAL_CONTROL_TERMINATION_75OHM	(2 << 16)
#	define NV_PRAMDAC_GENERAL_CONTROL_BPC_8BITS		(1 << 20)
#	define NV_PRAMDAC_GENERAL_CONTROL_PIPE_LONG		(2 << 28)

#define NV_PRAMDAC_FP_VDISPLAY_END                       0x00000800
#define NV_PRAMDAC_FP_VCRTC                              0x00000808
#define NV_PRAMDAC_FP_VSYNC_END                          0x00000810
#define NV_PRAMDAC_FP_VVALID_END                         0x00000818
#define NV_PRAMDAC_FP_HDISPLAY_END                       0x00000820
#define NV_PRAMDAC_FP_HCRTC                              0x00000828
#define NV_PRAMDAC_FP_HVALID_END                         0x00000838

#define NV_PRMCIO_ARX			0x006013c0
#define NV_PRMCIO_AR__WRITE		0x006013c0
#define NV_PRMCIO_AR__READ		0x006013c1
#	define NV_CIO_AR_MODE_INDEX		0x10
#	define NV_CIO_AR_OSCAN_INDEX		0x11
#	define NV_CIO_AR_PLANE_INDEX		0x12
#	define NV_CIO_AR_HPP_INDEX		0x13
#	define NV_CIO_AR_CSEL_INDEX		0x14
#define NV_PRMCIO_INP0			0x006013c2
#define NV_PRMCIO_CRX__COLOR		0x006013d4
#define NV_PRMCIO_CR__COLOR		0x006013d5
/* Standard VGA CRTC registers */
#	define NV_CIO_CR_HDT_INDEX		0x00	/* horizontal display total */
#	define NV_CIO_CR_HDE_INDEX		0x01	/* horizontal display end */
#	define NV_CIO_CR_HBS_INDEX		0x02	/* horizontal blanking start */
#	define NV_CIO_CR_HBE_INDEX		0x03	/* horizontal blanking end */
#		define NV_CIO_CR_HBE_4_0		4:0
#	define NV_CIO_CR_HRS_INDEX		0x04	/* horizontal retrace start */
#	define NV_CIO_CR_HRE_INDEX		0x05	/* horizontal retrace end */
#		define NV_CIO_CR_HRE_4_0		4:0
#		define NV_CIO_CR_HRE_HBE_5		7:7
#	define NV_CIO_CR_VDT_INDEX		0x06	/* vertical display total */
#	define NV_CIO_CR_OVL_INDEX		0x07	/* overflow bits */
#		define NV_CIO_CR_OVL_VDT_8		0:0
#		define NV_CIO_CR_OVL_VDE_8		1:1
#		define NV_CIO_CR_OVL_VRS_8		2:2
#		define NV_CIO_CR_OVL_VBS_8		3:3
#		define NV_CIO_CR_OVL_VDT_9		5:5
#		define NV_CIO_CR_OVL_VDE_9		6:6
#		define NV_CIO_CR_OVL_VRS_9		7:7
#	define NV_CIO_CR_RSAL_INDEX		0x08	/* normally "preset row scan" */
#	define NV_CIO_CR_CELL_HT_INDEX		0x09	/* cell height?! normally "max scan line" */
#		define NV_CIO_CR_CELL_HT_VBS_9		5:5
#		define NV_CIO_CR_CELL_HT_SCANDBL	7:7
#	define NV_CIO_CR_CURS_ST_INDEX		0x0a	/* cursor start */
#	define NV_CIO_CR_CURS_END_INDEX		0x0b	/* cursor end */
#	define NV_CIO_CR_SA_HI_INDEX		0x0c	/* screen start address high */
#	define NV_CIO_CR_SA_LO_INDEX		0x0d	/* screen start address low */
#	define NV_CIO_CR_TCOFF_HI_INDEX		0x0e	/* cursor offset high */
#	define NV_CIO_CR_TCOFF_LO_INDEX		0x0f	/* cursor offset low */
#	define NV_CIO_CR_VRS_INDEX		0x10	/* vertical retrace start */
#	define NV_CIO_CR_VRE_INDEX		0x11	/* vertical retrace end */
#		define NV_CIO_CR_VRE_3_0		3:0
#	define NV_CIO_CR_VDE_INDEX		0x12	/* vertical display end */
#	define NV_CIO_CR_OFFSET_INDEX		0x13	/* sets screen pitch */
#	define NV_CIO_CR_ULINE_INDEX		0x14	/* underline location */
#	define NV_CIO_CR_VBS_INDEX		0x15	/* vertical blank start */
#	define NV_CIO_CR_VBE_INDEX		0x16	/* vertical blank end */
#	define NV_CIO_CR_MODE_INDEX		0x17	/* crtc mode control */
#	define NV_CIO_CR_LCOMP_INDEX		0x18	/* line compare */
/* Extended VGA CRTC registers */
#	define NV_CIO_CRE_RPC0_INDEX		0x19	/* repaint control 0 */
#		define NV_CIO_CRE_RPC0_OFFSET_10_8	7:5
#	define NV_CIO_CRE_RPC1_INDEX		0x1a	/* repaint control 1 */
#		define NV_CIO_CRE_RPC1_LARGE		2:2
#	define NV_CIO_CRE_FF_INDEX		0x1b	/* fifo control */
#	define NV_CIO_CRE_ENH_INDEX		0x1c	/* enhanced? */
#	define NV_CIO_SR_LOCK_INDEX		0x1f	/* crtc lock */
#		define NV_CIO_SR_UNLOCK_RW_VALUE	0x57
#		define NV_CIO_SR_LOCK_VALUE		0x99
#	define NV_CIO_CRE_FFLWM__INDEX		0x20	/* fifo low water mark */
#	define NV_CIO_CRE_21			0x21	/* vga shadow crtc lock */
#	define NV_CIO_CRE_LSR_INDEX		0x25	/* ? */
#		define NV_CIO_CRE_LSR_VDT_10		0:0
#		define NV_CIO_CRE_LSR_VDE_10		1:1
#		define NV_CIO_CRE_LSR_VRS_10		2:2
#		define NV_CIO_CRE_LSR_VBS_10		3:3
#		define NV_CIO_CRE_LSR_HBE_6		4:4
#	define NV_CIO_CR_ARX_INDEX		0x26	/* attribute index -- ro copy of 0x60.3c0 */
#	define NV_CIO_CRE_CHIP_ID_INDEX		0x27	/* chip revision */
#	define NV_CIO_CRE_PIXEL_INDEX		0x28
#		define NV_CIO_CRE_PIXEL_FORMAT		1:0
#	define NV_CIO_CRE_HEB__INDEX		0x2d	/* horizontal extra bits? */
#		define NV_CIO_CRE_HEB_HDT_8		0:0
#		define NV_CIO_CRE_HEB_HDE_8		1:1
#		define NV_CIO_CRE_HEB_HBS_8		2:2
#		define NV_CIO_CRE_HEB_HRS_8		3:3
#		define NV_CIO_CRE_HEB_ILC_8		4:4
#	define NV_CIO_CRE_2E			0x2e	/* some scratch or dummy reg to force writes to sink in */
#	define NV_CIO_CRE_HCUR_ADDR2_INDEX	0x2f	/* cursor */
#	define NV_CIO_CRE_HCUR_ADDR0_INDEX	0x30		/* pixmap */
#		define NV_CIO_CRE_HCUR_ADDR0_ADR	6:0
#		define NV_CIO_CRE_HCUR_ASI		7:7
#	define NV_CIO_CRE_HCUR_ADDR1_INDEX	0x31			/* address */
#		define NV_CIO_CRE_HCUR_ADDR1_ENABLE	0:0
#		define NV_CIO_CRE_HCUR_ADDR1_CUR_DBL	1:1
#		define NV_CIO_CRE_HCUR_ADDR1_ADR	7:2
#	define NV_CIO_CRE_LCD__INDEX		0x33
#		define NV_CIO_CRE_LCD_LCD_SELECT	0:0
#	define NV_CIO_CRE_DDC0_STATUS__INDEX	0x36
#	define NV_CIO_CRE_DDC0_WR__INDEX	0x37
#	define NV_CIO_CRE_ILACE__INDEX		0x39	/* interlace */
#	define NV_CIO_CRE_SCRATCH3__INDEX	0x3b
#	define NV_CIO_CRE_SCRATCH4__INDEX	0x3c
#	define NV_CIO_CRE_DDC_STATUS__INDEX	0x3e
#	define NV_CIO_CRE_DDC_WR__INDEX		0x3f
#	define NV_CIO_CRE_EBR_INDEX		0x41	/* extra bits ? (vertical) */
#		define NV_CIO_CRE_EBR_VDT_11		0:0
#		define NV_CIO_CRE_EBR_VDE_11		2:2
#		define NV_CIO_CRE_EBR_VRS_11		4:4
#		define NV_CIO_CRE_EBR_VBS_11		6:6
#	define NV_CIO_CRE_43			0x43
#	define NV_CIO_CRE_44			0x44	/* head control */
#	define NV_CIO_CRE_CSB			0x45	/* colour saturation boost */
#	define NV_CIO_CRE_RCR			0x46
#		define NV_CIO_CRE_RCR_ENDIAN_BIG	7:7
#	define NV_CIO_CRE_47			0x47	/* extended fifo lwm, used on nv30+ */
#	define NV_CIO_CRE_49			0x49
#	define NV_CIO_CRE_4B			0x4b	/* given patterns in 0x[2-3][a-c] regs, probably scratch 6 */
#	define NV_CIO_CRE_TVOUT_LATENCY		0x52
#	define NV_CIO_CRE_53			0x53	/* `fp_htiming' according to Haiku */
#	define NV_CIO_CRE_54			0x54	/* `fp_vtiming' according to Haiku */
#	define NV_CIO_CRE_57			0x57	/* index reg for cr58 */
#	define NV_CIO_CRE_58			0x58	/* data reg for cr57 */
#	define NV_CIO_CRE_59			0x59	/* related to on/off-chip-ness of digital outputs */
#	define NV_CIO_CRE_5B			0x5B	/* newer colour saturation reg */
#	define NV_CIO_CRE_85			0x85
#	define NV_CIO_CRE_86			0x86
#define NV_PRMCIO_INP0__COLOR		0x006013da

#define NV_USER_DMA_PUT                                  0x40
#define NV_USER_DMA_GET                                  0x44
#define NV_USER_REF                                      0x48



/* DMA objects */
#define NV_DMA_FROM_MEMORY_CLASS                         0x02
#define NV_DMA_TO_MEMORY_CLASS                           0x03
#define NV_DMA_IN_MEMORY_CLASS                           0x3d

#define NV_DMA_CLASS                                          0x00000FFF
#define NV_DMA_PAGE_TABLE                                      (1 << 12)
#define NV_DMA_PAGE_ENTRY                                      (1 << 13)
#define NV_DMA_FLAGS_ACCESS                                    (1 << 14)
#define NV_DMA_FLAGS_MAPPING_COHERENCY                         (1 << 15)
#define NV_DMA_TARGET                                         0x00030000
#   define NV_DMA_TARGET_NVM                                      0x00000000
#   define NV_DMA_TARGET_NVM_TILED                                0x00010000
#   define NV_DMA_TARGET_PCI                                      0x00020000
#   define NV_DMA_TARGET_AGP                                      0x00030000
#define NV_DMA_ADJUST                                         0xFFF00000

#define NV_DMA_ADDRESS                                        0xFFFFF000


#define NV_RAMHT_HANDLE                                       0xFFFFFFFF
#define NV_RAMHT_INSTANCE                                     0x0000FFFF
#define NV_RAMHT_ENGINE                                       0x00030000
#   define NV_RAMHT_ENGINE_SW                                     0x00000000
#   define NV_RAMHT_ENGINE_GRAPHICS                               0x00010000
#   define NV_RAMHT_ENGINE_DVD                                    0x00020000
#define NV_RAMHT_CHID                                         0x1F000000
#define NV_RAMHT_STATUS                                       0x80000000



/* graphic classes and methods */
#define NV_SET_OBJECT                                        0x00000000

#define NV_MEMORY_TO_MEMORY_FORMAT                       0x0039

#define NV_CONTEXT_PATTERN                               0x0044
#   define NV044_SET_MONOCHROME_COLOR0                        0x00000310

#define NV_CONTEXT_SURFACES_2D                           0x0062
#   define NV062_SET_OBJECT                                   0x00000000
#   define NV062_SET_CONTEXT_DMA_IMAGE_SOURCE                 0x00000184
#   define NV062_SET_CONTEXT_DMA_IMAGE_DESTIN                 0x00000188
#   define NV062_SET_COLOR_FORMAT                             0x00000300
#       define NV062_SET_COLOR_FORMAT_LE_Y8                    0x01
#       define NV062_SET_COLOR_FORMAT_LE_R5G6B5                0x04
#       define NV062_SET_COLOR_FORMAT_LE_X8R8G8B8              0x07
#       define NV062_SET_COLOR_FORMAT_LE_A8R8G8B8              0x0A
#   define NV062_SET_PITCH                                    0x00000304
#   define NV062_SET_OFFSET_SOURCE                            0x00000308
#   define NV062_SET_OFFSET_DESTIN                            0x0000030C

#define NV_IMAGE_BLIT                                    0x009F
#   define NV09F_SET_OBJECT                                   0x00000000
#   define NV09F_SET_CONTEXT_SURFACES                         0x0000019C
#   define NV09F_SET_OPERATION                                0x000002FC
#       define NV09F_SET_OPERATION_SRCCOPY                        3
#   define NV09F_CONTROL_POINT_IN                             0x00000300
#   define NV09F_CONTROL_POINT_OUT                            0x00000304
#   define NV09F_SIZE                                         0x00000308


#define NV_KELVIN_PRIMITIVE                              0x0097
#   define NV097_SET_OBJECT                                   0x00000000
#   define NV097_NO_OPERATION                                 0x00000100
#   define NV097_NOTIFY                                       0x00000104
#   define NV097_SET_WARNING_ENABLE                           0x00000108
#   define NV097_GET_STATE                                    0x0000010C
#   define NV097_WAIT_FOR_IDLE                                0x00000110
#   define NV097_SET_FLIP_READ                                0x00000120
#   define NV097_SET_FLIP_WRITE                               0x00000124
#   define NV097_SET_FLIP_MODULO                              0x00000128
#   define NV097_FLIP_INCREMENT_WRITE                         0x0000012C
#   define NV097_FLIP_STALL                                   0x00000130
#   define NV097_PM_TRIGGER                                   0x00000140
#   define NV097_SET_CONTEXT_DMA_NOTIFIES                     0x00000180
#   define NV097_SET_CONTEXT_DMA_A                            0x00000184
#   define NV097_SET_CONTEXT_DMA_B                            0x00000188
#   define NV097_SET_CONTEXT_DMA_STATE                        0x00000190
#   define NV097_SET_CONTEXT_DMA_COLOR                        0x00000194
#   define NV097_SET_CONTEXT_DMA_ZETA                         0x00000198
#   define NV097_SET_CONTEXT_DMA_VERTEX_A                     0x0000019C
#   define NV097_SET_CONTEXT_DMA_VERTEX_B                     0x000001A0
#   define NV097_SET_CONTEXT_DMA_SEMAPHORE                    0x000001A4
#   define NV097_SET_CONTEXT_DMA_REPORT                       0x000001A8
#   define NV097_SET_SURFACE_CLIP_HORIZONTAL                  0x00000200
#       define NV097_SET_SURFACE_CLIP_HORIZONTAL_X                0x0000FFFF
#       define NV097_SET_SURFACE_CLIP_HORIZONTAL_WIDTH            0xFFFF0000
#   define NV097_SET_SURFACE_CLIP_VERTICAL                    0x00000204
#       define NV097_SET_SURFACE_CLIP_VERTICAL_Y                  0x0000FFFF
#       define NV097_SET_SURFACE_CLIP_VERTICAL_HEIGHT             0xFFFF0000
#   define NV097_SET_SURFACE_FORMAT                           0x00000208
#       define NV097_SET_SURFACE_FORMAT_COLOR                     0x0000000F
#           define NV097_SET_SURFACE_FORMAT_COLOR_LE_X1R5G5B5_Z1R5G5B5     0x01
#           define NV097_SET_SURFACE_FORMAT_COLOR_LE_X1R5G5B5_O1R5G5B5     0x02
#           define NV097_SET_SURFACE_FORMAT_COLOR_LE_R5G6B5                0x03
#           define NV097_SET_SURFACE_FORMAT_COLOR_LE_X8R8G8B8_Z8R8G8B8     0x04
#           define NV097_SET_SURFACE_FORMAT_COLOR_LE_X8R8G8B8_O8R8G8B8     0x05
#           define NV097_SET_SURFACE_FORMAT_COLOR_LE_X1A7R8G8B8_Z1A7R8G8B8 0x06
#           define NV097_SET_SURFACE_FORMAT_COLOR_LE_X1A7R8G8B8_O1A7R8G8B8 0x07
#           define NV097_SET_SURFACE_FORMAT_COLOR_LE_A8R8G8B8              0x08
#           define NV097_SET_SURFACE_FORMAT_COLOR_LE_B8                    0x09
#           define NV097_SET_SURFACE_FORMAT_COLOR_LE_G8B8                  0x0A
#       define NV097_SET_SURFACE_FORMAT_ZETA                      0x000000F0
#           define NV097_SET_SURFACE_FORMAT_ZETA_Z16                       1
#           define NV097_SET_SURFACE_FORMAT_ZETA_Z24S8                     2
#       define NV097_SET_SURFACE_FORMAT_TYPE                      0x00000F00
#           define NV097_SET_SURFACE_FORMAT_TYPE_PITCH                     0x1
#           define NV097_SET_SURFACE_FORMAT_TYPE_SWIZZLE                   0x2
#       define NV097_SET_SURFACE_FORMAT_ANTI_ALIASING             0x0000F000
#           define NV097_SET_SURFACE_FORMAT_ANTI_ALIASING_CENTER_1         0
#           define NV097_SET_SURFACE_FORMAT_ANTI_ALIASING_CENTER_CORNER_2  1
#           define NV097_SET_SURFACE_FORMAT_ANTI_ALIASING_SQUARE_OFFSET_4  2
#       define NV097_SET_SURFACE_FORMAT_WIDTH                     0x00FF0000
#       define NV097_SET_SURFACE_FORMAT_HEIGHT                    0xFF000000
#   define NV097_SET_SURFACE_PITCH                            0x0000020C
#       define NV097_SET_SURFACE_PITCH_COLOR                      0x0000FFFF
#       define NV097_SET_SURFACE_PITCH_ZETA                       0xFFFF0000
#   define NV097_SET_SURFACE_COLOR_OFFSET                     0x00000210
#   define NV097_SET_SURFACE_ZETA_OFFSET                      0x00000214
#   define NV097_SET_COMBINER_ALPHA_ICW                       0x00000260 // [8]
#       define NV097_SET_COMBINER_ALPHA_ICW_A_MAP                 0xE0000000
#           define NV097_SET_COMBINER_ALPHA_ICW_A_MAP_UNSIGNED_IDENTITY 0
#           define NV097_SET_COMBINER_ALPHA_ICW_A_MAP_UNSIGNED_INVERT   1
#           define NV097_SET_COMBINER_ALPHA_ICW_A_MAP_EXPAND_NORMAL     2
#           define NV097_SET_COMBINER_ALPHA_ICW_A_MAP_EXPAND_NEGATE     3
#           define NV097_SET_COMBINER_ALPHA_ICW_A_MAP_HALFBIAS_NORMAL   4
#           define NV097_SET_COMBINER_ALPHA_ICW_A_MAP_HALFBIAS_NEGATE   5
#           define NV097_SET_COMBINER_ALPHA_ICW_A_MAP_SIGNED_IDENTITY   6
#           define NV097_SET_COMBINER_ALPHA_ICW_A_MAP_SIGNED_NEGATE     7
#       define NV097_SET_COMBINER_ALPHA_ICW_A_ALPHA               (1<<28)
#       define NV097_SET_COMBINER_ALPHA_ICW_A_SOURCE              0x0F000000
#       define NV097_SET_COMBINER_ALPHA_ICW_B_MAP                 0x00E00000
#           define NV097_SET_COMBINER_ALPHA_ICW_B_MAP_UNSIGNED_IDENTITY 0
#           define NV097_SET_COMBINER_ALPHA_ICW_B_MAP_UNSIGNED_INVERT   1
#           define NV097_SET_COMBINER_ALPHA_ICW_B_MAP_EXPAND_NORMAL     2
#           define NV097_SET_COMBINER_ALPHA_ICW_B_MAP_EXPAND_NEGATE     3
#           define NV097_SET_COMBINER_ALPHA_ICW_B_MAP_HALFBIAS_NORMAL   4
#           define NV097_SET_COMBINER_ALPHA_ICW_B_MAP_HALFBIAS_NEGATE   5
#           define NV097_SET_COMBINER_ALPHA_ICW_B_MAP_SIGNED_IDENTITY   6
#           define NV097_SET_COMBINER_ALPHA_ICW_B_MAP_SIGNED_NEGATE     7
#       define NV097_SET_COMBINER_ALPHA_ICW_B_ALPHA               (1<<20)
#       define NV097_SET_COMBINER_ALPHA_ICW_B_SOURCE              0x000F0000
#       define NV097_SET_COMBINER_ALPHA_ICW_C_MAP                 0x0000E000
#           define NV097_SET_COMBINER_ALPHA_ICW_C_MAP_UNSIGNED_IDENTITY 0
#           define NV097_SET_COMBINER_ALPHA_ICW_C_MAP_UNSIGNED_INVERT   1
#           define NV097_SET_COMBINER_ALPHA_ICW_C_MAP_EXPAND_NORMAL     2
#           define NV097_SET_COMBINER_ALPHA_ICW_C_MAP_EXPAND_NEGATE     3
#           define NV097_SET_COMBINER_ALPHA_ICW_C_MAP_HALFBIAS_NORMAL   4
#           define NV097_SET_COMBINER_ALPHA_ICW_C_MAP_HALFBIAS_NEGATE   5
#           define NV097_SET_COMBINER_ALPHA_ICW_C_MAP_SIGNED_IDENTITY   6
#           define NV097_SET_COMBINER_ALPHA_ICW_C_MAP_SIGNED_NEGATE     7
#       define NV097_SET_COMBINER_ALPHA_ICW_C_ALPHA               (1<<12)
#       define NV097_SET_COMBINER_ALPHA_ICW_C_SOURCE              0x00000F00
#       define NV097_SET_COMBINER_ALPHA_ICW_D_MAP                 0x000000E0
#           define NV097_SET_COMBINER_ALPHA_ICW_D_MAP_UNSIGNED_IDENTITY 0
#           define NV097_SET_COMBINER_ALPHA_ICW_D_MAP_UNSIGNED_INVERT   1
#           define NV097_SET_COMBINER_ALPHA_ICW_D_MAP_EXPAND_NORMAL     2
#           define NV097_SET_COMBINER_ALPHA_ICW_D_MAP_EXPAND_NEGATE     3
#           define NV097_SET_COMBINER_ALPHA_ICW_D_MAP_HALFBIAS_NORMAL   4
#           define NV097_SET_COMBINER_ALPHA_ICW_D_MAP_HALFBIAS_NEGATE   5
#           define NV097_SET_COMBINER_ALPHA_ICW_D_MAP_SIGNED_IDENTITY   6
#           define NV097_SET_COMBINER_ALPHA_ICW_D_MAP_SIGNED_NEGATE     7
#       define NV097_SET_COMBINER_ALPHA_ICW_D_ALPHA               (1<<4)
#       define NV097_SET_COMBINER_ALPHA_ICW_D_SOURCE              0x0000000F
#   define NV097_SET_COMBINER_SPECULAR_FOG_CW0                0x00000288
#       define NV097_SET_COMBINER_SPECULAR_FOG_CW0_A_INVERSE      0xE0000000
#       define NV097_SET_COMBINER_SPECULAR_FOG_CW0_A_ALPHA        (1<<28)
#       define NV097_SET_COMBINER_SPECULAR_FOG_CW0_A_SOURCE       0x0F000000
#           define NV097_SET_COMBINER_SPECULAR_FOG_CW0_A_SOURCE_REG_SPECLIT 0xE
#           define NV097_SET_COMBINER_SPECULAR_FOG_CW0_A_SOURCE_REG_EF_PROD 0xF
#       define NV097_SET_COMBINER_SPECULAR_FOG_CW0_B_INVERSE      0x00E00000
#       define NV097_SET_COMBINER_SPECULAR_FOG_CW0_B_ALPHA        (1<<20)
#       define NV097_SET_COMBINER_SPECULAR_FOG_CW0_B_SOURCE       0x000F0000
#           define NV097_SET_COMBINER_SPECULAR_FOG_CW0_B_SOURCE_REG_SPECLIT 0xE
#           define NV097_SET_COMBINER_SPECULAR_FOG_CW0_B_SOURCE_REG_EF_PROD 0xF
#       define NV097_SET_COMBINER_SPECULAR_FOG_CW0_C_INVERSE      0x0000E000
#       define NV097_SET_COMBINER_SPECULAR_FOG_CW0_C_ALPHA        (1<<12)
#       define NV097_SET_COMBINER_SPECULAR_FOG_CW0_C_SOURCE       0x00000F00
#           define NV097_SET_COMBINER_SPECULAR_FOG_CW0_C_SOURCE_REG_SPECLIT 0xE
#           define NV097_SET_COMBINER_SPECULAR_FOG_CW0_C_SOURCE_REG_EF_PROD 0xF
#       define NV097_SET_COMBINER_SPECULAR_FOG_CW0_D_INVERSE      0x000000E0
#       define NV097_SET_COMBINER_SPECULAR_FOG_CW0_D_ALPHA        (1<<4)
#       define NV097_SET_COMBINER_SPECULAR_FOG_CW0_D_SOURCE       0x0000000F
#           define NV097_SET_COMBINER_SPECULAR_FOG_CW0_D_SOURCE_REG_SPECLIT 0xE
#           define NV097_SET_COMBINER_SPECULAR_FOG_CW0_D_SOURCE_REG_EF_PROD 0xF
#   define NV097_SET_COMBINER_SPECULAR_FOG_CW1                0x0000028C
#       define NV097_SET_COMBINER_SPECULAR_FOG_CW1_E_INVERSE      0xE0000000
#       define NV097_SET_COMBINER_SPECULAR_FOG_CW1_E_ALPHA        (1<<28)
#       define NV097_SET_COMBINER_SPECULAR_FOG_CW1_E_SOURCE       0x0F000000
#       define NV097_SET_COMBINER_SPECULAR_FOG_CW1_F_INVERSE      0x00E00000
#       define NV097_SET_COMBINER_SPECULAR_FOG_CW1_F_ALPHA        (1<<20)
#       define NV097_SET_COMBINER_SPECULAR_FOG_CW1_F_SOURCE       0x000F0000
#       define NV097_SET_COMBINER_SPECULAR_FOG_CW1_G_INVERSE      0x0000E000
#       define NV097_SET_COMBINER_SPECULAR_FOG_CW1_G_ALPHA        (1<<12)
#       define NV097_SET_COMBINER_SPECULAR_FOG_CW1_G_SOURCE       0x00000F00
#       define NV097_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_CLAMP (1<<7)
#       define NV097_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_ADD_INVERT_R5 (1<<6)
#       define NV097_SET_COMBINER_SPECULAR_FOG_CW1_SPECULAR_ADD_INVERT_R12 0x0000003F
#   define NV097_SET_CONTROL0                                 0x00000290
#       define NV097_SET_CONTROL0_STENCIL_WRITE_ENABLE            (1 << 0)
#       define NV097_SET_CONTROL0_Z_FORMAT                        (1 << 12)
#       define NV097_SET_CONTROL0_Z_PERSPECTIVE_ENABLE            (1 << 16)
#       define NV097_SET_CONTROL0_COLOR_SPACE_CONVERT             (0xF << 28)
#   define NV097_SET_LIGHT_CONTROL                            0x00000294
#   define NV097_SET_COLOR_MATERIAL                           0x00000298
#   define NV097_SET_FOG_MODE                                 0x0000029C
#       define NV097_SET_FOG_MODE_V_LINEAR                        0x2601
#       define NV097_SET_FOG_MODE_V_EXP                           0x800
#       define NV097_SET_FOG_MODE_V_EXP2                          0x801
#       define NV097_SET_FOG_MODE_V_EXP_ABS                       0x802
#       define NV097_SET_FOG_MODE_V_EXP2_ABS                      0x803
#       define NV097_SET_FOG_MODE_V_LINEAR_ABS                    0x804
#   define NV097_SET_FOG_GEN_MODE                             0x000002A0
#       define NV097_SET_FOG_GEN_MODE_V_SPEC_ALPHA                0
#       define NV097_SET_FOG_GEN_MODE_V_RADIAL                    1
#       define NV097_SET_FOG_GEN_MODE_V_PLANAR                    2
#       define NV097_SET_FOG_GEN_MODE_V_ABS_PLANAR                3
#       define NV097_SET_FOG_GEN_MODE_V_FOG_X                     6
#   define NV097_SET_FOG_ENABLE                               0x000002A4
#   define NV097_SET_FOG_COLOR                                0x000002A8
#       define NV097_SET_FOG_COLOR_RED                            0x000000FF
#       define NV097_SET_FOG_COLOR_GREEN                          0x0000FF00
#       define NV097_SET_FOG_COLOR_BLUE                           0x00FF0000
#       define NV097_SET_FOG_COLOR_ALPHA                          0xFF000000
#   define NV097_SET_WINDOW_CLIP_TYPE                         0x000002B4
#   define NV097_SET_WINDOW_CLIP_HORIZONTAL                   0x000002C0 // [8]
#       define NV097_SET_WINDOW_CLIP_HORIZONTAL_XMIN              0x00000FFF
#       define NV097_SET_WINDOW_CLIP_HORIZONTAL_XMAX              0x0FFF0000
#   define NV097_SET_WINDOW_CLIP_VERTICAL                     0x000002E0 // [8]
#   define NV097_SET_ALPHA_TEST_ENABLE                        0x00000300
#   define NV097_SET_BLEND_ENABLE                             0x00000304
#   define NV097_SET_CULL_FACE_ENABLE                         0x00000308
#   define NV097_SET_DEPTH_TEST_ENABLE                        0x0000030C
#   define NV097_SET_DITHER_ENABLE                            0x00000310
#   define NV097_SET_LIGHTING_ENABLE                          0x00000314
#   define NV097_SET_POINT_PARAMS_ENABLE                      0x00000318
#   define NV097_SET_POINT_SMOOTH_ENABLE                      0x0000031C
#   define NV097_SET_LINE_SMOOTH_ENABLE                       0x00000320
#   define NV097_SET_POLY_SMOOTH_ENABLE                       0x00000324
#   define NV097_SET_SKIN_MODE                                0x00000328
#       define NV097_SET_SKIN_MODE_OFF                            0
#       define NV097_SET_SKIN_MODE_2G                             1
#       define NV097_SET_SKIN_MODE_2                              2
#       define NV097_SET_SKIN_MODE_3G                             3
#       define NV097_SET_SKIN_MODE_3                              4
#       define NV097_SET_SKIN_MODE_4G                             5
#       define NV097_SET_SKIN_MODE_4                              6
#   define NV097_SET_STENCIL_TEST_ENABLE                      0x0000032C
#   define NV097_SET_POLY_OFFSET_POINT_ENABLE                 0x00000330
#   define NV097_SET_POLY_OFFSET_LINE_ENABLE                  0x00000334
#   define NV097_SET_POLY_OFFSET_FILL_ENABLE                  0x00000338
#   define NV097_SET_ALPHA_FUNC                               0x0000033C
#   define NV097_SET_ALPHA_REF                                0x00000340
#   define NV097_SET_BLEND_FUNC_SFACTOR                       0x00000344
#       define NV097_SET_BLEND_FUNC_SFACTOR_V_ZERO                0x0000
#       define NV097_SET_BLEND_FUNC_SFACTOR_V_ONE                 0x0001
#       define NV097_SET_BLEND_FUNC_SFACTOR_V_SRC_COLOR           0x0300
#       define NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_SRC_COLOR 0x0301
#       define NV097_SET_BLEND_FUNC_SFACTOR_V_SRC_ALPHA           0x0302
#       define NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_SRC_ALPHA 0x0303
#       define NV097_SET_BLEND_FUNC_SFACTOR_V_DST_ALPHA           0x0304
#       define NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_DST_ALPHA 0x0305
#       define NV097_SET_BLEND_FUNC_SFACTOR_V_DST_COLOR           0x0306
#       define NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_DST_COLOR 0x0307
#       define NV097_SET_BLEND_FUNC_SFACTOR_V_SRC_ALPHA_SATURATE  0x0308
#       define NV097_SET_BLEND_FUNC_SFACTOR_V_CONSTANT_COLOR      0x8001
#       define NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_CONSTANT_COLOR 0x8002
#       define NV097_SET_BLEND_FUNC_SFACTOR_V_CONSTANT_ALPHA      0x8003
#       define NV097_SET_BLEND_FUNC_SFACTOR_V_ONE_MINUS_CONSTANT_ALPHA 0x8004
#   define NV097_SET_BLEND_FUNC_DFACTOR                       0x00000348
#       define NV097_SET_BLEND_FUNC_DFACTOR_V_ZERO                0x0000
#       define NV097_SET_BLEND_FUNC_DFACTOR_V_ONE                 0x0001
#       define NV097_SET_BLEND_FUNC_DFACTOR_V_SRC_COLOR           0x0300
#       define NV097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_SRC_COLOR 0x0301
#       define NV097_SET_BLEND_FUNC_DFACTOR_V_SRC_ALPHA           0x0302
#       define NV097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_SRC_ALPHA 0x0303
#       define NV097_SET_BLEND_FUNC_DFACTOR_V_DST_ALPHA           0x0304
#       define NV097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_DST_ALPHA 0x0305
#       define NV097_SET_BLEND_FUNC_DFACTOR_V_DST_COLOR           0x0306
#       define NV097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_DST_COLOR 0x0307
#       define NV097_SET_BLEND_FUNC_DFACTOR_V_SRC_ALPHA_SATURATE  0x0308
#       define NV097_SET_BLEND_FUNC_DFACTOR_V_CONSTANT_COLOR      0x8001
#       define NV097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_CONSTANT_COLOR 0x8002
#       define NV097_SET_BLEND_FUNC_DFACTOR_V_CONSTANT_ALPHA      0x8003
#       define NV097_SET_BLEND_FUNC_DFACTOR_V_ONE_MINUS_CONSTANT_ALPHA 0x8004
#   define NV097_SET_BLEND_COLOR                              0x0000034C
#   define NV097_SET_BLEND_EQUATION                           0x00000350
#       define NV097_SET_BLEND_EQUATION_V_FUNC_SUBTRACT           0x800A
#       define NV097_SET_BLEND_EQUATION_V_FUNC_REVERSE_SUBTRACT   0x800B
#       define NV097_SET_BLEND_EQUATION_V_FUNC_ADD                0x8006
#       define NV097_SET_BLEND_EQUATION_V_MIN                     0x8007
#       define NV097_SET_BLEND_EQUATION_V_MAX                     0x8008
#       define NV097_SET_BLEND_EQUATION_V_FUNC_REVERSE_SUBTRACT_SIGNED 0xF005
#       define NV097_SET_BLEND_EQUATION_V_FUNC_ADD_SIGNED         0xF006
#   define NV097_SET_DEPTH_FUNC                               0x00000354
#   define NV097_SET_COLOR_MASK                               0x00000358
#       define NV097_SET_COLOR_MASK_BLUE_WRITE_ENABLE             (1 << 0)
#       define NV097_SET_COLOR_MASK_GREEN_WRITE_ENABLE            (1 << 8)
#       define NV097_SET_COLOR_MASK_RED_WRITE_ENABLE              (1 << 16)
#       define NV097_SET_COLOR_MASK_ALPHA_WRITE_ENABLE            (1 << 24)
#   define NV097_SET_DEPTH_MASK                               0x0000035C
#   define NV097_SET_STENCIL_MASK                             0x00000360
#   define NV097_SET_STENCIL_FUNC                             0x00000364
#   define NV097_SET_STENCIL_FUNC_REF                         0x00000368
#   define NV097_SET_STENCIL_FUNC_MASK                        0x0000036C
#   define NV097_SET_STENCIL_OP_FAIL                          0x00000370
#   define NV097_SET_STENCIL_OP_ZFAIL                         0x00000374
#   define NV097_SET_STENCIL_OP_ZPASS                         0x00000378
#       define NV097_SET_STENCIL_OP_V_KEEP                        0x1E00
#       define NV097_SET_STENCIL_OP_V_ZERO                        0x0000
#       define NV097_SET_STENCIL_OP_V_REPLACE                     0x1E01
#       define NV097_SET_STENCIL_OP_V_INCRSAT                     0x1E02
#       define NV097_SET_STENCIL_OP_V_DECRSAT                     0x1E03
#       define NV097_SET_STENCIL_OP_V_INVERT                      0x150A
#       define NV097_SET_STENCIL_OP_V_INCR                        0x8507
#       define NV097_SET_STENCIL_OP_V_DECR                        0x8508
#   define NV097_SET_SHADE_MODE                               0x0000037C
#   define NV097_SET_LINE_WIDTH                               0x00000380
#   define NV097_SET_POLYGON_OFFSET_SCALE_FACTOR              0x00000384
#   define NV097_SET_POLYGON_OFFSET_BIAS                      0x00000388
#   define NV097_SET_FRONT_POLYGON_MODE                       0x0000038C
#       define NV097_SET_FRONT_POLYGON_MODE_V_POINT               0x1B00
#       define NV097_SET_FRONT_POLYGON_MODE_V_LINE                0x1B01
#       define NV097_SET_FRONT_POLYGON_MODE_V_FILL                0x1B02
#   define NV097_SET_BACK_POLYGON_MODE                        0x00000390
#   define NV097_SET_CLIP_MIN                                 0x00000394
#   define NV097_SET_CLIP_MAX                                 0x00000398
#   define NV097_SET_CULL_FACE                                0x0000039C
#       define NV097_SET_CULL_FACE_V_FRONT                         0x404
#       define NV097_SET_CULL_FACE_V_BACK                          0x405
#       define NV097_SET_CULL_FACE_V_FRONT_AND_BACK                0x408
#   define NV097_SET_FRONT_FACE                               0x000003A0
#       define NV097_SET_FRONT_FACE_V_CW                           0x900
#       define NV097_SET_FRONT_FACE_V_CCW                          0x901
#   define NV097_SET_NORMALIZATION_ENABLE                     0x000003A4
#   define NV097_SET_MATERIAL_EMISSION                        0x000003A8 // [3]
#   define NV097_SET_MATERIAL_ALPHA                           0x000003B4
#   define NV097_SET_SPECULAR_ENABLE                          0x000003B8
#   define NV097_SET_LIGHT_ENABLE_MASK                        0x000003BC
#           define NV097_SET_LIGHT_ENABLE_MASK_LIGHT0_OFF           0
#           define NV097_SET_LIGHT_ENABLE_MASK_LIGHT0_INFINITE      1
#           define NV097_SET_LIGHT_ENABLE_MASK_LIGHT0_LOCAL         2
#           define NV097_SET_LIGHT_ENABLE_MASK_LIGHT0_SPOT          3
#   define NV097_SET_TEXGEN_S                                 0x000003C0 // [4.0]
#       define NV097_SET_TEXGEN_S_DISABLE                         0x0000
#       define NV097_SET_TEXGEN_S_EYE_LINEAR                      0x2400
#       define NV097_SET_TEXGEN_S_OBJECT_LINEAR                   0x2401
#       define NV097_SET_TEXGEN_S_SPHERE_MAP                      0x2402
#       define NV097_SET_TEXGEN_S_REFLECTION_MAP                  0x8512
#       define NV097_SET_TEXGEN_S_NORMAL_MAP                      0x8511
#   define NV097_SET_TEXGEN_T                                 0x000003C4 // [4.1]
#   define NV097_SET_TEXGEN_R                                 0x000003C8 // [4.2]
#   define NV097_SET_TEXGEN_Q                                 0x000003CC // [4.3]
#   define NV097_SET_TEXTURE_MATRIX_ENABLE                    0x00000420 // [4]
#   define NV097_SET_POINT_SIZE                               0x0000043C
#   define NV097_SET_PROJECTION_MATRIX                        0x00000440 // [16]
#   define NV097_SET_MODEL_VIEW_MATRIX                        0x00000480 // [16]
#   define NV097_SET_MODEL_VIEW_MATRIX1                       0x000004C0 // [16]
#   define NV097_SET_MODEL_VIEW_MATRIX2                       0x00000500 // [16]
#   define NV097_SET_MODEL_VIEW_MATRIX3                       0x00000540 // [16]
#   define NV097_SET_INVERSE_MODEL_VIEW_MATRIX                0x00000580 // [16]
#   define NV097_SET_INVERSE_MODEL_VIEW_MATRIX1               0x000005C0 // [16]
#   define NV097_SET_INVERSE_MODEL_VIEW_MATRIX2               0x00000600 // [16]
#   define NV097_SET_INVERSE_MODEL_VIEW_MATRIX3               0x00000640 // [16]
#   define NV097_SET_COMPOSITE_MATRIX                         0x00000680 // [16]
#   define NV097_SET_TEXTURE_MATRIX                           0x000006C0 // [16]
#   define NV097_SET_TEXTURE_MATRIX1                          0x00000700 // [16]
#   define NV097_SET_TEXTURE_MATRIX2                          0x00000740 // [16]
#   define NV097_SET_TEXTURE_MATRIX3                          0x00000780 // [16]
#   define NV097_SET_TEXGEN_PLANE_S                           0x00000840 // [4.0][4]
#   define NV097_SET_TEXGEN_PLANE_T                           0x00000850 // [4.1][4]
#   define NV097_SET_TEXGEN_PLANE_R                           0x00000860 // [4.2][4]
#   define NV097_SET_TEXGEN_PLANE_Q                           0x00000870 // [4.3][4]
#   define NV097_SET_FOG_PARAMS                               0x000009C0 // [3]
#   define NV097_SET_TEXGEN_VIEW_MODEL                        0x000009CC
#       define NV097_SET_TEXGEN_VIEW_MODEL_LOCAL_VIEWER           0
#       define NV097_SET_TEXGEN_VIEW_MODEL_INFINITE_VIEWER        1
#   define NV097_SET_FOG_PLANE                                0x000009D0 // [4]
#   define NV097_SET_SPECULAR_PARAMS                          0x000009E0 // [6]
#   define NV097_SET_SWATH_WIDTH                              0x000009F8
#   define NV097_SET_FLAT_SHADE_OP                            0x000009FC
#   define NV097_SET_SCENE_AMBIENT_COLOR                      0x00000A10 // [3]
#   define NV097_SET_VIEWPORT_OFFSET                          0x00000A20 // [4]
#   define NV097_SET_POINT_PARAMS                             0x00000A30 // [8]
#   define NV097_SET_EYE_POSITION                             0x00000A50 // [4]
#   define NV097_SET_COMBINER_FACTOR0                         0x00000A60 // [8]
#   define NV097_SET_COMBINER_FACTOR1                         0x00000A80 // [8]
#   define NV097_SET_COMBINER_ALPHA_OCW                       0x00000AA0 // [8]
#       define NV097_SET_COMBINER_ALPHA_OCW_OP                    0xFFFF8000
#           define NV097_SET_COMBINER_ALPHA_OCW_OP_NOSHIFT           0
#           define NV097_SET_COMBINER_ALPHA_OCW_OP_NOSHIFT_BIAS      1
#           define NV097_SET_COMBINER_ALPHA_OCW_OP_SHIFTLEFTBY1      2
#           define NV097_SET_COMBINER_ALPHA_OCW_OP_SHIFTLEFTBY1_BIAS 3
#           define NV097_SET_COMBINER_ALPHA_OCW_OP_SHIFTLEFTBY2      4
#           define NV097_SET_COMBINER_ALPHA_OCW_OP_SHIFTRIGHTBY1     6
#       define NV097_SET_COMBINER_ALPHA_OCW_MUX_ENABLE            (1<<14)
#       define NV097_SET_COMBINER_ALPHA_OCW_SUM_DST               0x00000F00
#       define NV097_SET_COMBINER_ALPHA_OCW_AB_DST                0x000000F0
#       define NV097_SET_COMBINER_ALPHA_OCW_CD_DST                0x0000000F
#   define NV097_SET_COMBINER_COLOR_ICW                       0x00000AC0 // [8]
#       define NV097_SET_COMBINER_COLOR_ICW_A_MAP                 0xE0000000
#       define NV097_SET_COMBINER_COLOR_ICW_A_ALPHA               (1<<28)
#       define NV097_SET_COMBINER_COLOR_ICW_A_SOURCE              0x0F000000
#       define NV097_SET_COMBINER_COLOR_ICW_B_MAP                 0x00E00000
#       define NV097_SET_COMBINER_COLOR_ICW_B_ALPHA               (1<<20)
#       define NV097_SET_COMBINER_COLOR_ICW_B_SOURCE              0x000F0000
#       define NV097_SET_COMBINER_COLOR_ICW_C_MAP                 0x0000E000
#       define NV097_SET_COMBINER_COLOR_ICW_C_ALPHA               (1<<12)
#       define NV097_SET_COMBINER_COLOR_ICW_C_SOURCE              0x00000F00
#       define NV097_SET_COMBINER_COLOR_ICW_D_MAP                 0x000000E0
#       define NV097_SET_COMBINER_COLOR_ICW_D_ALPHA               (1<<4)
#       define NV097_SET_COMBINER_COLOR_ICW_D_SOURCE              0x0000000F
#   define NV097_SET_COLOR_KEY_COLOR                          0x00000AE0 // [4]
#   define NV097_SET_VIEWPORT_SCALE                           0x00000AF0 // [4]
#   define NV097_SET_TRANSFORM_PROGRAM                        0x00000B00 // [32]
#   define NV097_SET_TRANSFORM_CONSTANT                       0x00000B80 // [32]
#   define NV097_SET_BACK_LIGHT_AMBIENT_COLOR                 0x00000C00 // [8.0][3]
#   define NV097_SET_BACK_LIGHT_DIFFUSE_COLOR                 0x00000C0C // [8.1][3]
#   define NV097_SET_BACK_LIGHT_SPECULAR_COLOR                0x00000C18 // [8.2][3]
#   define NV097_SET_LIGHT_AMBIENT_COLOR                      0x00001000 // [8.0][3]
#   define NV097_SET_LIGHT_DIFFUSE_COLOR                      0x0000100C // [8.1][3]
#   define NV097_SET_LIGHT_SPECULAR_COLOR                     0x00001018 // [8.2][3]
#   define NV097_SET_LIGHT_LOCAL_RANGE                        0x00001024 // [8.3]
#   define NV097_SET_LIGHT_INFINITE_HALF_VECTOR               0x00001028 // [8.4][3]
#   define NV097_SET_LIGHT_INFINITE_DIRECTION                 0x00001034 // [8.5][3]
#   define NV097_SET_LIGHT_SPOT_FALLOFF                       0x00001040 // [8.6][3]
#   define NV097_SET_LIGHT_SPOT_DIRECTION                     0x0000104C // [8.7]
#   define NV097_SET_LIGHT_LOCAL_POSITION                     0x0000105C // [8.8][3]
#   define NV097_SET_LIGHT_LOCAL_ATTENUATION                  0x00001068 // [8.9][3]
#   define NV097_SET_STIPPLE_CONTROL                          0x0000147C
#   define NV097_SET_STIPPLE_PATTERN                          0x00001480 // [32]
#   define NV097_SET_VERTEX3F                                 0x00001500 // [3]
#   define NV097_SET_VERTEX4F                                 0x00001518 // [4]
#   define NV097_SET_VERTEX4S                                 0x00001528 // [2]
#   define NV097_SET_NORMAL3F                                 0x00001530 // [3]
#   define NV097_SET_NORMAL3S                                 0x00001540 // [2]
#   define NV097_SET_DIFFUSE_COLOR4F                          0x00001550 // [4]
#   define NV097_SET_DIFFUSE_COLOR3F                          0x00001560 // [3]
#   define NV097_SET_DIFFUSE_COLOR4UB                         0x0000156C
#   define NV097_SET_SPECULAR_COLOR4F                         0x00001570 // [4]
#   define NV097_SET_SPECULAR_COLOR3F                         0x00001580 // [3]
#   define NV097_SET_SPECULAR_COLOR4UB                        0x0000158C
#   define NV097_SET_TEXCOORD0_2F                             0x00001590 // [2]
#   define NV097_SET_TEXCOORD0_2S                             0x00001598
#   define NV097_SET_TEXCOORD0_4F                             0x000015A0 // [4]
#   define NV097_SET_TEXCOORD0_4S                             0x000015B0 // [2]
#   define NV097_SET_TEXCOORD1_2F                             0x000015B8 // [2]
#   define NV097_SET_TEXCOORD1_2S                             0x000015C0
#   define NV097_SET_TEXCOORD1_4F                             0x000015C8 // [4]
#   define NV097_SET_TEXCOORD1_4S                             0x000015D8 // [2]
#   define NV097_SET_TEXCOORD2_2F                             0x000015E0 // [2]
#   define NV097_SET_TEXCOORD2_2S                             0x000015E8
#   define NV097_SET_TEXCOORD2_4F                             0x000015F0 // [4]
#   define NV097_SET_TEXCOORD2_4S                             0x00001600 // [2]
#   define NV097_SET_TEXCOORD3_2F                             0x00001608 // [2]
#   define NV097_SET_TEXCOORD3_2S                             0x00001610
#   define NV097_SET_TEXCOORD3_4F                             0x00001620 // [4]
#   define NV097_SET_TEXCOORD3_4S                             0x00001630 // [2]
#   define NV097_SET_FOG1F                                    0x00001698
#   define NV097_SET_WEIGHT1F                                 0x0000169C
#   define NV097_SET_WEIGHT2F                                 0x000016A0 // [2]
#   define NV097_SET_WEIGHT3F                                 0x000016B0 // [3]
#   define NV097_SET_EDGE_FLAG                                0x000016BC
#   define NV097_SET_WEIGHT4F                                 0x000016C0 // [4]
#   define NV097_SET_TRANSFORM_FIXED_CONST3                   0x000016D0 // [4]
#   define NV097_SET_TRANSFORM_FIXED_CONST0                   0x000016E0 // [4]
#   define NV097_SET_TRANSFORM_FIXED_CONST1                   0x000016F0 // [4]
#   define NV097_SET_TRANSFORM_FIXED_CONST2                   0x00001700 // [4]
#   define NV097_INVALIDATE_VERTEX_CACHE_FILE                 0x00001710
#   define NV097_INVALIDATE_VERTEX_FILE                       0x00001714
#   define NV097_TL_NOP                                       0x00001718
#   define NV097_TL_SYNC                                      0x0000171C
#   define NV097_SET_VERTEX_DATA_ARRAY_OFFSET                 0x00001720 // [16]
#   define NV097_SET_VERTEX_DATA_ARRAY_FORMAT                 0x00001760 // [16]
#       define NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE            0x0000000F
#           define NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_UB_D3D     0
#           define NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S1         1
#           define NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_F          2
#           define NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_UB_OGL     4
#           define NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_S32K       5
#           define NV097_SET_VERTEX_DATA_ARRAY_FORMAT_TYPE_CMP        6
#       define NV097_SET_VERTEX_DATA_ARRAY_FORMAT_SIZE            0x000000F0
#       define NV097_SET_VERTEX_DATA_ARRAY_FORMAT_STRIDE          0xFFFFFF00
#   define NV097_SET_BACK_SCENE_AMBIENT_COLOR                 0x000017A0 // [3]
#   define NV097_SET_BACK_MATERIAL_ALPHA                      0x000017AC
#   define NV097_SET_BACK_MATERIAL_EMISSIONR                  0x000017B0 // [3]
#   define NV097_SET_LOGIC_OP_ENABLE                          0x000017BC
#   define NV097_SET_LOGIC_OP                                 0x000017C0
#   define NV097_SET_TWO_SIDED_LIGHT_EN                       0x000017C4
#   define NV097_CLEAR_REPORT_VALUE                           0x000017C8
#       define NV097_CLEAR_REPORT_VALUE_TYPE                      0xFFFFFFFF
#           define NV097_CLEAR_REPORT_VALUE_TYPE_ZPASS_PIXEL_CNT      1
#   define NV097_SET_ZPASS_PIXEL_COUNT_ENABLE                 0x000017CC
#   define NV097_GET_REPORT                                   0x000017D0
#       define NV097_GET_REPORT_OFFSET                            0x00FFFFFF
#       define NV097_GET_REPORT_TYPE                              0xFF000000
#           define NV097_GET_REPORT_TYPE_ZPASS_PIXEL_CNT              1
#   define NV097_SET_TL_CONST_ZERO                            0x000017D4 // [3]
#   define NV097_SET_EYE_DIRECTION                            0x000017E0 // [3]
#   define NV097_SET_LINEAR_FOG_CONST                         0x000017EC // [3]
#   define NV097_SET_SHADER_CLIP_PLANE_MODE                   0x000017F8
#   define NV097_SET_BEGIN_END                                0x000017FC
#       define NV097_SET_BEGIN_END_OP_END                         0x00
#       define NV097_SET_BEGIN_END_OP_POINTS                      0x01
#       define NV097_SET_BEGIN_END_OP_LINES                       0x02
#       define NV097_SET_BEGIN_END_OP_LINE_LOOP                   0x03
#       define NV097_SET_BEGIN_END_OP_LINE_STRIP                  0x04
#       define NV097_SET_BEGIN_END_OP_TRIANGLES                   0x05
#       define NV097_SET_BEGIN_END_OP_TRIANGLE_STRIP              0x06
#       define NV097_SET_BEGIN_END_OP_TRIANGLE_FAN                0x07
#       define NV097_SET_BEGIN_END_OP_QUADS                       0x08
#       define NV097_SET_BEGIN_END_OP_QUAD_STRIP                  0x09
#       define NV097_SET_BEGIN_END_OP_POLYGON                     0x0A
#   define NV097_ARRAY_ELEMENT16                              0x00001800
#   define NV097_ARRAY_ELEMENT32                              0x00001808
#   define NV097_DRAW_ARRAYS                                  0x00001810
#       define NV097_DRAW_ARRAYS_COUNT                            0xFF000000
#       define NV097_DRAW_ARRAYS_START_INDEX                      0x00FFFFFF
#   define NV097_INLINE_ARRAY                                 0x00001818
#   define NV097_SET_EYE_VECTOR                               0x0000181C // [3]
#   define NV097_INLINE_VERTEX_REUSE                          0x00001828
#   define NV097_SET_VERTEX_DATA2F_M                          0x00001880 // [16.0][2]
#   define NV097_SET_VERTEX_DATA2S                            0x00001900 // [16]
#   define NV097_SET_VERTEX_DATA4UB                           0x00001940 // [16]
#   define NV097_SET_VERTEX_DATA4S_M                          0x00001980 // [16.0][2]
#   define NV097_SET_VERTEX_DATA4F_M                          0x00001A00 // [16.0][4]
#   define NV097_SET_TEXTURE_OFFSET                           0x00001B00 // [4.0]
#   define NV097_SET_TEXTURE_FORMAT                           0x00001B04 // [4.1]
#       define NV097_SET_TEXTURE_FORMAT_CONTEXT_DMA               0x00000003
#       define NV097_SET_TEXTURE_FORMAT_CUBEMAP_ENABLE            (1 << 2)
#       define NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE             (1 << 3)
#           define NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_TEXTURE   0
#           define NV097_SET_TEXTURE_FORMAT_BORDER_SOURCE_COLOR     1
#       define NV097_SET_TEXTURE_FORMAT_DIMENSIONALITY            0x000000F0
#       define NV097_SET_TEXTURE_FORMAT_COLOR                     0x0000FF00
#           define NV097_SET_TEXTURE_FORMAT_COLOR_SZ_Y8             0x00
#           define NV097_SET_TEXTURE_FORMAT_COLOR_SZ_AY8            0x01
#           define NV097_SET_TEXTURE_FORMAT_COLOR_SZ_A1R5G5B5       0x02
#           define NV097_SET_TEXTURE_FORMAT_COLOR_SZ_X1R5G5B5       0x03
#           define NV097_SET_TEXTURE_FORMAT_COLOR_SZ_A4R4G4B4       0x04
#           define NV097_SET_TEXTURE_FORMAT_COLOR_SZ_R5G6B5         0x05
#           define NV097_SET_TEXTURE_FORMAT_COLOR_SZ_A8R8G8B8       0x06
#           define NV097_SET_TEXTURE_FORMAT_COLOR_SZ_X8R8G8B8       0x07
#           define NV097_SET_TEXTURE_FORMAT_COLOR_SZ_I8_A8R8G8B8    0x0B
#           define NV097_SET_TEXTURE_FORMAT_COLOR_L_DXT1_A1R5G5B5   0x0C
#           define NV097_SET_TEXTURE_FORMAT_COLOR_L_DXT23_A8R8G8B8  0x0E
#           define NV097_SET_TEXTURE_FORMAT_COLOR_L_DXT45_A8R8G8B8  0x0F
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A1R5G5B5 0x10
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G6B5   0x11
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8R8G8B8 0x12
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y8       0x13
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SY8      0x14
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X7SY9    0x15
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8B8     0x16
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_G8B8     0x17
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_SG8SB8   0x18
#           define NV097_SET_TEXTURE_FORMAT_COLOR_SZ_A8             0x19
#           define NV097_SET_TEXTURE_FORMAT_COLOR_SZ_A8Y8           0x1A
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_AY8      0x1B
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X1R5G5B5 0x1C
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A4R4G4B4 0x1D
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_X8R8G8B8 0x1E
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8       0x1F
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8Y8     0x20
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_CR8YB8CB8YA8 0x24
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_YB8CR8YA8CB8 0x25
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8CR8CB8Y8 0x26
#           define NV097_SET_TEXTURE_FORMAT_COLOR_SZ_R6G5B5         0x27
#           define NV097_SET_TEXTURE_FORMAT_COLOR_SZ_G8B8           0x28
#           define NV097_SET_TEXTURE_FORMAT_COLOR_SZ_R8B8           0x29
#           define NV097_SET_TEXTURE_FORMAT_COLOR_SZ_DEPTH_X8_Y24_FIXED 0x2A
#           define NV097_SET_TEXTURE_FORMAT_COLOR_SZ_DEPTH_X8_Y24_FLOAT 0x2B
#           define NV097_SET_TEXTURE_FORMAT_COLOR_SZ_DEPTH_Y16_FIXED 0x2C
#           define NV097_SET_TEXTURE_FORMAT_COLOR_SZ_DEPTH_Y16_FLOAT 0x2D
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_X8_Y24_FIXED 0x2E
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_Y16_FIXED 0x30
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_DEPTH_Y16_FLOAT 0x31
#           define NV097_SET_TEXTURE_FORMAT_COLOR_SZ_Y16            0x32
#           define NV097_SET_TEXTURE_FORMAT_COLOR_SZ_YB16YA16       0x33 // was NV097_SET_TEXTURE_FORMAT_COLOR_SZ_YB_16_YA_16
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LC_IMAGE_A4V6YB6A4U6YA6 0x34
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_Y16      0x35
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_YB16YA16 0x36
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R6G5B5   0x37
#           define NV097_SET_TEXTURE_FORMAT_COLOR_SZ_R5G5B5A1       0x38
#           define NV097_SET_TEXTURE_FORMAT_COLOR_SZ_R4G4B4A4       0x39
#           define NV097_SET_TEXTURE_FORMAT_COLOR_SZ_A8B8G8R8       0x3A
#           define NV097_SET_TEXTURE_FORMAT_COLOR_SZ_R8G8B8A8       0x3C
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R5G5B5A1 0x3D
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R4G4B4A4 0x3E
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_A8B8G8R8 0x3F
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_B8G8R8A8 0x40
#           define NV097_SET_TEXTURE_FORMAT_COLOR_LU_IMAGE_R8G8B8A8 0x41
#       define NV097_SET_TEXTURE_FORMAT_MIPMAP_LEVELS             0x000F0000
#       define NV097_SET_TEXTURE_FORMAT_BASE_SIZE_U               0x00F00000
#       define NV097_SET_TEXTURE_FORMAT_BASE_SIZE_V               0x0F000000
#       define NV097_SET_TEXTURE_FORMAT_BASE_SIZE_P               0xF0000000
#   define NV097_SET_TEXTURE_ADDRESS                          0x00001B08 // [4.2]
#   define NV097_SET_TEXTURE_CONTROL0                         0x00001B0C // [4.3]
#       define NV097_SET_TEXTURE_CONTROL0_ENABLE                 (1 << 30)
#       define NV097_SET_TEXTURE_CONTROL0_MIN_LOD_CLAMP           0x3FFC0000
#       define NV097_SET_TEXTURE_CONTROL0_MAX_LOD_CLAMP           0x0003FFC0
#   define NV097_SET_TEXTURE_CONTROL1                         0x00001B10 // [4.4]
#       define NV097_SET_TEXTURE_CONTROL1_IMAGE_PITCH             0xFFFF0000
#   define NV097_SET_TEXTURE_FILTER                           0x00001B14 // [4.5]
#       define NV097_SET_TEXTURE_FILTER_MIPMAP_LOD_BIAS           0x00001FFF
#       define NV097_SET_TEXTURE_FILTER_MIN                       0x00FF0000
#       define NV097_SET_TEXTURE_FILTER_MAG                       0x0F000000
#       define NV097_SET_TEXTURE_FILTER_ASIGNED                   (1 << 28)
#       define NV097_SET_TEXTURE_FILTER_RSIGNED                   (1 << 29)
#       define NV097_SET_TEXTURE_FILTER_GSIGNED                   (1 << 30)
#       define NV097_SET_TEXTURE_FILTER_BSIGNED                   (1 << 31)
#   define NV097_SET_TEXTURE_IMAGE_RECT                       0x00001B1C // [4.7] (.6 is reserved)
#       define NV097_SET_TEXTURE_IMAGE_RECT_WIDTH                 0xFFFF0000
#       define NV097_SET_TEXTURE_IMAGE_RECT_HEIGHT                0x0000FFFF
#   define NV097_SET_TEXTURE_PALETTE                          0x00001B20 // [4.8]
#       define NV097_SET_TEXTURE_PALETTE_CONTEXT_DMA              (1 << 0)
#       define NV097_SET_TEXTURE_PALETTE_LENGTH                   0x0000000C
#         define NV097_SET_TEXTURE_PALETTE_LENGTH_256               0
#         define NV097_SET_TEXTURE_PALETTE_LENGTH_128               1
#         define NV097_SET_TEXTURE_PALETTE_LENGTH_64                2
#         define NV097_SET_TEXTURE_PALETTE_LENGTH_32                3
#       define NV097_SET_TEXTURE_PALETTE_OFFSET                   0xFFFFFFC0
#   define NV097_SET_TEXTURE_BORDER_COLOR                     0x00001B24 // [4.9]
#   define NV097_SET_TEXTURE_SET_BUMP_ENV_MAT                 0x00001B28 // [4.10]
#   define NV097_SET_TEXTURE_SET_BUMP_ENV_MAT01               0x00001B2C // [4.11]
#   define NV097_SET_TEXTURE_SET_BUMP_ENV_MAT11               0x00001B30 // [4.12]
#   define NV097_SET_TEXTURE_SET_BUMP_ENV_MAT10               0x00001B34 // [4.13]
#   define NV097_SET_TEXTURE_SET_BUMP_ENV_SCALE               0x00001B38 // [4.14]
#   define NV097_SET_TEXTURE_SET_BUMP_ENV_OFFSET              0x00001B3C // [4.15]
#   define NV097_PARK_ATTRIBUTE                               0x00001D64
#   define NV097_UNPARK_ATTRIBUTE                             0x00001D68
#   define NV097_SET_SEMAPHORE_OFFSET                         0x00001D6C
#   define NV097_BACK_END_WRITE_SEMAPHORE_RELEASE             0x00001D70
#   define NV097_TEXTURE_READ_SEMAPHORE_RELEASE               0x00001D74
#   define NV097_SET_ZMIN_MAX_CONTROL                         0x00001D78
#   define NV097_SET_ANTI_ALIASING_CONTROL                    0x00001D7C
#   define NV097_SET_COMPRESS_ZBUFFER_EN                      0x00001D80
#   define NV097_SET_OCCLUDE_ZSTENCIL_EN                      0x00001D84
#   define NV097_SET_ZSTENCIL_CLEAR_VALUE                     0x00001D8C
#   define NV097_SET_COLOR_CLEAR_VALUE                        0x00001D90
#   define NV097_CLEAR_SURFACE                                0x00001D94
#       define NV097_CLEAR_SURFACE_Z                              (1 << 0)
#       define NV097_CLEAR_SURFACE_STENCIL                        (1 << 1)
#       define NV097_CLEAR_SURFACE_COLOR                          0x000000F0
#       define NV097_CLEAR_SURFACE_R                                (1 << 4)
#       define NV097_CLEAR_SURFACE_G                                (1 << 5)
#       define NV097_CLEAR_SURFACE_B                                (1 << 6)
#       define NV097_CLEAR_SURFACE_A                                (1 << 7)
#   define NV097_SET_CLEAR_RECT_HORIZONTAL                    0x00001D98
#   define NV097_SET_CLEAR_RECT_VERTICAL                      0x00001D9C
#   define NV097_SET_BEGIN_PATCH0                             0x00001DE0
#   define NV097_SET_BEGIN_PATCH1                             0x00001DE4
#   define NV097_SET_BEGIN_PATCH2                             0x00001DE8
#   define NV097_SET_BEGIN_PATCH3                             0x00001DEC
#   define NV097_SET_END_PATCH                                0x00001DF0
#   define NV097_SET_BEGIN_END_SWATCH                         0x00001DF4
#   define NV097_SET_BEGIN_END_CURVE                          0x00001DF8
#   define NV097_SET_CURVE_COEFFICIENTS                       0x00001E00 // [4]
#   define NV097_SET_BEGIN_TRANSITION0                        0x00001E10
#   define NV097_SET_BEGIN_TRANSITION1                        0x00001E14
#   define NV097_SET_BEGIN_TRANSITION2                        0x00001E18
#   define NV097_SET_END_TRANSITION                           0x00001E1C
#   define NV097_SET_SPECULAR_FOG_FACTOR                      0x00001E20 // [2]
#   define NV097_SET_BACK_SPECULAR_PARAMS                     0x00001E28 // [6]
#   define NV097_SET_COMBINER_COLOR_OCW                       0x00001E40 // [8]
#       define NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_AB        0xFFF80000
#       define NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_AB_DISABLE  0
#       define NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_AB_AB_DST_ENABLE 1
#       define NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_CD        (1<<18)
#       define NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_CD_DISABLE  0
#       define NV097_SET_COMBINER_COLOR_OCW_BLUETOALPHA_CD_CD_DST_ENABLE 1
#       define NV097_SET_COMBINER_COLOR_OCW_OP                    0x00038000
#           define NV097_SET_COMBINER_COLOR_OCW_OP_NOSHIFT          0
#           define NV097_SET_COMBINER_COLOR_OCW_OP_NOSHIFT_BIAS     1
#           define NV097_SET_COMBINER_COLOR_OCW_OP_SHIFTLEFTBY1     2
#           define NV097_SET_COMBINER_COLOR_OCW_OP_SHIFTLEFTBY1_BIAS 3
#           define NV097_SET_COMBINER_COLOR_OCW_OP_SHIFTLEFTBY2     4
#           define NV097_SET_COMBINER_COLOR_OCW_OP_SHIFTRIGHTBY1    6
#       define NV097_SET_COMBINER_COLOR_OCW_MUX_ENABLE            (1 << 14)
#       define NV097_SET_COMBINER_COLOR_OCW_AB_DOT_ENABLE         (1 << 13)
#       define NV097_SET_COMBINER_COLOR_OCW_CD_DOT_ENABLE         (1<<12)
#       define NV097_SET_COMBINER_COLOR_OCW_SUM_DST               0x00000F00
#       define NV097_SET_COMBINER_COLOR_OCW_AB_DST                0x000000F0
#       define NV097_SET_COMBINER_COLOR_OCW_CD_DST                0x0000000F
#   define NV097_SET_COMBINER_CONTROL                         0x00001E60
#       define NV097_SET_COMBINER_CONTROL_ITERATION_COUNT         0x000000FF
#           define NV097_SET_COMBINER_CONTROL_ITERATION_COUNT_ONE   1
#           define NV097_SET_COMBINER_CONTROL_ITERATION_COUNT_TWO   2
#           define NV097_SET_COMBINER_CONTROL_ITERATION_COUNT_THREE 3
#           define NV097_SET_COMBINER_CONTROL_ITERATION_COUNT_FOUR  4
#           define NV097_SET_COMBINER_CONTROL_ITERATION_COUNT_FIVE  5
#           define NV097_SET_COMBINER_CONTROL_ITERATION_COUNT_SIX   6
#           define NV097_SET_COMBINER_CONTROL_ITERATION_COUNT_SEVEN 7
#           define NV097_SET_COMBINER_CONTROL_ITERATION_COUNT_EIGHT 8
#       define NV097_SET_COMBINER_CONTROL_MUX_SELECT              0x00000F00
#           define NV097_SET_COMBINER_CONTROL_MUX_SELECT_LSB        0
#           define NV097_SET_COMBINER_CONTROL_MUX_SELECT_MSB        1
#       define NV097_SET_COMBINER_CONTROL_FACTOR0                 0x0000F000
#           define NV097_SET_COMBINER_CONTROL_FACTOR0_SAME_FACTOR_ALL 0
#           define NV097_SET_COMBINER_CONTROL_FACTOR0_EACH_STAGE    1
#       define NV097_SET_COMBINER_CONTROL_FACTOR1                 0xFFFF0000
#           define NV097_SET_COMBINER_CONTROL_FACTOR1_SAME_FACTOR_ALL 0
#           define NV097_SET_COMBINER_CONTROL_FACTOR1_EACH_STAGE    1
#   define NV097_SET_SHADOW_ZSLOPE_THRESHOLD                  0x00001E68
#   define NV097_SET_SHADOW_DEPTH_FUNC                        0x00001E6C
#   define NV097_SET_SHADER_STAGE_PROGRAM                     0x00001E70
#       define NV097_SET_SHADER_STAGE_PROGRAM_STAGE0              0x0000001F
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_PROGRAM_NONE   0
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_2D_PROJECTIVE  1
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_3D_PROJECTIVE  2
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_CUBE_MAP       3
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_PASS_THROUGH   4
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE0_CLIP_PLANE     5
#       define NV097_SET_SHADER_STAGE_PROGRAM_STAGE1              0x000003E0
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_PROGRAM_NONE   0x00
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_2D_PROJECTIVE  0x01
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_3D_PROJECTIVE  0x02
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_CUBE_MAP       0x03
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_PASS_THROUGH   0x04
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_CLIP_PLANE     0x05
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_BUMPENVMAP     0x06
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_BUMPENVMAP_LUMINANCE 0x07
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_DEPENDENT_AR   0x0F
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_DEPENDENT_GB   0x10
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE1_DOT_PRODUCT    0x11
#       define NV097_SET_SHADER_STAGE_PROGRAM_STAGE2              0x00007C00
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_PROGRAM_NONE   0x00
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_2D_PROJECTIVE  0x01
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_3D_PROJECTIVE  0x02
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_CUBE_MAP       0x03
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_PASS_THROUGH   0x04
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_CLIP_PLANE     0x05
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_BUMPENVMAP     0x06
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_BUMPENVMAP_LUMINANCE 0x07
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_BRDF           0x08
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_DOT_ST         0x09
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_DOT_ZW         0x0A
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_DOT_REFLECT_DIFFUSE 0x0B
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_DEPENDENT_AR   0x0F
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_DEPENDENT_GB   0x10
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE2_DOT_PRODUCT    0x11
#       define NV097_SET_SHADER_STAGE_PROGRAM_STAGE3              0x000F8000
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_PROGRAM_NONE   0x00
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_2D_PROJECTIVE  0x01
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_3D_PROJECTIVE  0x02
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_CUBE_MAP       0x03
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_PASS_THROUGH   0x04
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_CLIP_PLANE     0x05
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_BUMPENVMAP     0x06
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_BUMPENVMAP_LUMINANCE 0x07
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_BRDF           0x08
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_ST         0x09
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_ZW         0x0A
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_REFLECT_SPECULAR 0x0C
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_STR_3D     0x0D
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_STR_CUBE   0x0E
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DEPENDENT_AR   0x0F
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DEPENDENT_GB   0x10
#           define NV097_SET_SHADER_STAGE_PROGRAM_STAGE3_DOT_REFLECT_SPECULAR_CONST 0x12
#   define NV097_SET_DOT_RGBMAPPING                           0X00001E74
#   define NV097_SET_SHADER_OTHER_STAGE_INPUT                 0x00001E78
#       define NV097_SET_SHADER_OTHER_STAGE_INPUT_STAGE1          0x0000FFFF
#       define NV097_SET_SHADER_OTHER_STAGE_INPUT_STAGE1_INSTAGE_0  0
#       define NV097_SET_SHADER_OTHER_STAGE_INPUT_STAGE2          0x000F0000
#       define NV097_SET_SHADER_OTHER_STAGE_INPUT_STAGE2_INSTAGE_0  0
#       define NV097_SET_SHADER_OTHER_STAGE_INPUT_STAGE2_INSTAGE_1  1
#       define NV097_SET_SHADER_OTHER_STAGE_INPUT_STAGE3          0x00F00000
#       define NV097_SET_SHADER_OTHER_STAGE_INPUT_STAGE3_INSTAGE_0  0
#       define NV097_SET_SHADER_OTHER_STAGE_INPUT_STAGE3_INSTAGE_1  1
#       define NV097_SET_SHADER_OTHER_STAGE_INPUT_STAGE3_INSTAGE_2  2
#   define NV097_SET_TRANSFORM_DATA                           0x00001E80 // [4]
#   define NV097_LAUNCH_TRANSFORM_PROGRAM                     0x00001E90
#   define NV097_SET_TRANSFORM_EXECUTION_MODE                 0x00001E94
#       define NV097_SET_TRANSFORM_EXECUTION_MODE_MODE            0x00000003
#           define NV097_SET_TRANSFORM_EXECUTION_MODE_MODE_FIXED    0
#           define NV097_SET_TRANSFORM_EXECUTION_MODE_MODE_PROGRAM  2
#       define NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE      0xFFFFFFFC
#           define NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE_USER 0
#           define NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE_PRIV 1
#   define NV097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN           0x00001E98
#   define NV097_SET_TRANSFORM_PROGRAM_LOAD                   0x00001E9C
#   define NV097_SET_TRANSFORM_PROGRAM_START                  0x00001EA0
#   define NV097_SET_TRANSFORM_CONSTANT_LOAD                  0x00001EA4
#   define NV097_DEBUG_INIT                                   0x00001FC0 // [10]

/* vertex processing (cheops) context layout */
#define NV_IGRAPH_XF_XFCTX_CMAT0                     0x00
#define NV_IGRAPH_XF_XFCTX_PMAT0                     0x04
#define NV_IGRAPH_XF_XFCTX_MMAT0                     0x08
#define NV_IGRAPH_XF_XFCTX_IMMAT0                    0x0c
#define NV_IGRAPH_XF_XFCTX_MMAT1                     0x10
#define NV_IGRAPH_XF_XFCTX_IMMAT1                    0x14
#define NV_IGRAPH_XF_XFCTX_MMAT2                     0x18
#define NV_IGRAPH_XF_XFCTX_IMMAT2                    0x1c
#define NV_IGRAPH_XF_XFCTX_MMAT3                     0x20
#define NV_IGRAPH_XF_XFCTX_IMMAT3                    0x24
#define NV_IGRAPH_XF_XFCTX_LIT0                      0x28
#define NV_IGRAPH_XF_XFCTX_LIT1                      0x29
#define NV_IGRAPH_XF_XFCTX_LIT2                      0x2a
#define NV_IGRAPH_XF_XFCTX_LIT3                      0x2b
#define NV_IGRAPH_XF_XFCTX_LIT4                      0x2c
#define NV_IGRAPH_XF_XFCTX_LIT5                      0x2d
#define NV_IGRAPH_XF_XFCTX_LIT6                      0x2e
#define NV_IGRAPH_XF_XFCTX_LIT7                      0x2f
#define NV_IGRAPH_XF_XFCTX_SPOT0                     0x30
#define NV_IGRAPH_XF_XFCTX_SPOT1                     0x31
#define NV_IGRAPH_XF_XFCTX_SPOT2                     0x32
#define NV_IGRAPH_XF_XFCTX_SPOT3                     0x33
#define NV_IGRAPH_XF_XFCTX_SPOT4                     0x34
#define NV_IGRAPH_XF_XFCTX_SPOT5                     0x35
#define NV_IGRAPH_XF_XFCTX_SPOT6                     0x36
#define NV_IGRAPH_XF_XFCTX_SPOT7                     0x37
#define NV_IGRAPH_XF_XFCTX_EYEP                      0x38
#define NV_IGRAPH_XF_XFCTX_FOG                       0x39
#define NV_IGRAPH_XF_XFCTX_VPSCL                     0x3a
#define NV_IGRAPH_XF_XFCTX_VPOFF                     0x3b
#define NV_IGRAPH_XF_XFCTX_CONS0                     0x3c
#define NV_IGRAPH_XF_XFCTX_CONS1                     0x3d
#define NV_IGRAPH_XF_XFCTX_CONS2                     0x3e
#define NV_IGRAPH_XF_XFCTX_CONS3                     0x3f
#define NV_IGRAPH_XF_XFCTX_TG0MAT                    0x40
#define NV_IGRAPH_XF_XFCTX_T0MAT                     0x44
#define NV_IGRAPH_XF_XFCTX_TG1MAT                    0x48
#define NV_IGRAPH_XF_XFCTX_T1MAT                     0x4c
#define NV_IGRAPH_XF_XFCTX_TG2MAT                    0x50
#define NV_IGRAPH_XF_XFCTX_T2MAT                     0x54
#define NV_IGRAPH_XF_XFCTX_TG3MAT                    0x58
#define NV_IGRAPH_XF_XFCTX_T3MAT                     0x5c
#define NV_IGRAPH_XF_XFCTX_PRSPACE                   0x60

/* lighting (zoser) context layout */
#define NV_IGRAPH_XF_LTCTXA_L0_K                     0x00
#define NV_IGRAPH_XF_LTCTXA_L0_SPT                   0x01
#define NV_IGRAPH_XF_LTCTXA_L1_K                     0x02
#define NV_IGRAPH_XF_LTCTXA_L1_SPT                   0x03
#define NV_IGRAPH_XF_LTCTXA_L2_K                     0x04
#define NV_IGRAPH_XF_LTCTXA_L2_SPT                   0x05
#define NV_IGRAPH_XF_LTCTXA_L3_K                     0x06
#define NV_IGRAPH_XF_LTCTXA_L3_SPT                   0x07
#define NV_IGRAPH_XF_LTCTXA_L4_K                     0x08
#define NV_IGRAPH_XF_LTCTXA_L4_SPT                   0x09
#define NV_IGRAPH_XF_LTCTXA_L5_K                     0x0a
#define NV_IGRAPH_XF_LTCTXA_L5_SPT                   0x0b
#define NV_IGRAPH_XF_LTCTXA_L6_K                     0x0c
#define NV_IGRAPH_XF_LTCTXA_L6_SPT                   0x0d
#define NV_IGRAPH_XF_LTCTXA_L7_K                     0x0e
#define NV_IGRAPH_XF_LTCTXA_L7_SPT                   0x0f
#define NV_IGRAPH_XF_LTCTXA_EYED                     0x10
#define NV_IGRAPH_XF_LTCTXA_FR_AMB                   0x11
#define NV_IGRAPH_XF_LTCTXA_BR_AMB                   0x12
#define NV_IGRAPH_XF_LTCTXA_CM_COL                   0x13
#define NV_IGRAPH_XF_LTCTXA_BCM_COL                  0x14
#define NV_IGRAPH_XF_LTCTXA_FOG_K                    0x15
#define NV_IGRAPH_XF_LTCTXA_ZERO                     0x16
#define NV_IGRAPH_XF_LTCTXA_PT0                      0x17
#define NV_IGRAPH_XF_LTCTXA_FOGLIN                   0x18

#define NV_IGRAPH_XF_LTCTXB_L0_AMB                   0x00
#define NV_IGRAPH_XF_LTCTXB_L0_DIF                   0x01
#define NV_IGRAPH_XF_LTCTXB_L0_SPC                   0x02
#define NV_IGRAPH_XF_LTCTXB_L0_BAMB                  0x03
#define NV_IGRAPH_XF_LTCTXB_L0_BDIF                  0x04
#define NV_IGRAPH_XF_LTCTXB_L0_BSPC                  0x05
#define NV_IGRAPH_XF_LTCTXB_L1_AMB                   0x06
#define NV_IGRAPH_XF_LTCTXB_L1_DIF                   0x07
#define NV_IGRAPH_XF_LTCTXB_L1_SPC                   0x08
#define NV_IGRAPH_XF_LTCTXB_L1_BAMB                  0x09
#define NV_IGRAPH_XF_LTCTXB_L1_BDIF                  0x0a
#define NV_IGRAPH_XF_LTCTXB_L1_BSPC                  0x0b
#define NV_IGRAPH_XF_LTCTXB_L2_AMB                   0x0c
#define NV_IGRAPH_XF_LTCTXB_L2_DIF                   0x0d
#define NV_IGRAPH_XF_LTCTXB_L2_SPC                   0x0e
#define NV_IGRAPH_XF_LTCTXB_L2_BAMB                  0x0f
#define NV_IGRAPH_XF_LTCTXB_L2_BDIF                  0x10
#define NV_IGRAPH_XF_LTCTXB_L2_BSPC                  0x11
#define NV_IGRAPH_XF_LTCTXB_L3_AMB                   0x12
#define NV_IGRAPH_XF_LTCTXB_L3_DIF                   0x13
#define NV_IGRAPH_XF_LTCTXB_L3_SPC                   0x14
#define NV_IGRAPH_XF_LTCTXB_L3_BAMB                  0x15
#define NV_IGRAPH_XF_LTCTXB_L3_BDIF                  0x16
#define NV_IGRAPH_XF_LTCTXB_L3_BSPC                  0x17
#define NV_IGRAPH_XF_LTCTXB_L4_AMB                   0x18
#define NV_IGRAPH_XF_LTCTXB_L4_DIF                   0x19
#define NV_IGRAPH_XF_LTCTXB_L4_SPC                   0x1a
#define NV_IGRAPH_XF_LTCTXB_L4_BAMB                  0x1b
#define NV_IGRAPH_XF_LTCTXB_L4_BDIF                  0x1c
#define NV_IGRAPH_XF_LTCTXB_L4_BSPC                  0x1d
#define NV_IGRAPH_XF_LTCTXB_L5_AMB                   0x1e
#define NV_IGRAPH_XF_LTCTXB_L5_DIF                   0x1f
#define NV_IGRAPH_XF_LTCTXB_L5_SPC                   0x20
#define NV_IGRAPH_XF_LTCTXB_L5_BAMB                  0x21
#define NV_IGRAPH_XF_LTCTXB_L5_BDIF                  0x22
#define NV_IGRAPH_XF_LTCTXB_L5_BSPC                  0x23
#define NV_IGRAPH_XF_LTCTXB_L6_AMB                   0x24
#define NV_IGRAPH_XF_LTCTXB_L6_DIF                   0x25
#define NV_IGRAPH_XF_LTCTXB_L6_SPC                   0x26
#define NV_IGRAPH_XF_LTCTXB_L6_BAMB                  0x27
#define NV_IGRAPH_XF_LTCTXB_L6_BDIF                  0x28
#define NV_IGRAPH_XF_LTCTXB_L6_BSPC                  0x29
#define NV_IGRAPH_XF_LTCTXB_L7_AMB                   0x2a
#define NV_IGRAPH_XF_LTCTXB_L7_DIF                   0x2b
#define NV_IGRAPH_XF_LTCTXB_L7_SPC                   0x2c
#define NV_IGRAPH_XF_LTCTXB_L7_BAMB                  0x2d
#define NV_IGRAPH_XF_LTCTXB_L7_BDIF                  0x2e
#define NV_IGRAPH_XF_LTCTXB_L7_BSPC                  0x2f
#define NV_IGRAPH_XF_LTCTXB_PT1                      0x30
#define NV_IGRAPH_XF_LTCTXB_ONE                      0x31
#define NV_IGRAPH_XF_LTCTXB_VPOFFSET                 0x32

#define NV_IGRAPH_XF_LTC1_ZERO1                      0x00
#define NV_IGRAPH_XF_LTC1_l0                         0x01
#define NV_IGRAPH_XF_LTC1_Bl0                        0x02
#define NV_IGRAPH_XF_LTC1_PP                         0x03
#define NV_IGRAPH_XF_LTC1_r0                         0x04
#define NV_IGRAPH_XF_LTC1_r1                         0x05
#define NV_IGRAPH_XF_LTC1_r2                         0x06
#define NV_IGRAPH_XF_LTC1_r3                         0x07
#define NV_IGRAPH_XF_LTC1_r4                         0x08
#define NV_IGRAPH_XF_LTC1_r5                         0x09
#define NV_IGRAPH_XF_LTC1_r6                         0x0a
#define NV_IGRAPH_XF_LTC1_r7                         0x0b
#define NV_IGRAPH_XF_LTC1_L0                         0x0c
#define NV_IGRAPH_XF_LTC1_L1                         0x0d
#define NV_IGRAPH_XF_LTC1_L2                         0x0e
#define NV_IGRAPH_XF_LTC1_L3                         0x0f
#define NV_IGRAPH_XF_LTC1_L4                         0x10
#define NV_IGRAPH_XF_LTC1_L5                         0x11
#define NV_IGRAPH_XF_LTC1_L6                         0x12
#define NV_IGRAPH_XF_LTC1_L7                         0x13

/* These RDI select values appear to be named by MS.
 * nvidia seems to refer to RDI_INDEX_VTX_CONSTANTS0 by RDI_RAMSEL_XL_XFCTX.
 * However, we don't have other nvidia names; so we use these aliases for now.
 * Eventually we'll probably adopt nouveau names for these internals.
 */
#define RDI_INDEX_VTX_CONSTANTS0                     0x17
#define RDI_INDEX_VTX_CONSTANTS1                     0xCC


#define NV2A_VERTEX_ATTR_POSITION       0
#define NV2A_VERTEX_ATTR_WEIGHT         1
#define NV2A_VERTEX_ATTR_NORMAL         2
#define NV2A_VERTEX_ATTR_DIFFUSE        3
#define NV2A_VERTEX_ATTR_SPECULAR       4
#define NV2A_VERTEX_ATTR_FOG            5
#define NV2A_VERTEX_ATTR_POINT_SIZE     6
#define NV2A_VERTEX_ATTR_BACK_DIFFUSE   7
#define NV2A_VERTEX_ATTR_BACK_SPECULAR  8
#define NV2A_VERTEX_ATTR_TEXTURE0       9
#define NV2A_VERTEX_ATTR_TEXTURE1       10
#define NV2A_VERTEX_ATTR_TEXTURE2       11
#define NV2A_VERTEX_ATTR_TEXTURE3       12
#define NV2A_VERTEX_ATTR_RESERVED1      13
#define NV2A_VERTEX_ATTR_RESERVED2      14
#define NV2A_VERTEX_ATTR_RESERVED3      15

#define NV2A_CRYSTAL_FREQ 16666666 // Was 13500000
#define NV2A_NUM_CHANNELS 32
#define NV2A_NUM_SUBCHANNELS 8
#define NV2A_CACHE1_SIZE 128

#define NV2A_MAX_BATCH_LENGTH 0x1FFFF
#define NV2A_VERTEXSHADER_ATTRIBUTES 16
#define NV2A_MAX_TEXTURES 4

#define NV2A_MAX_TRANSFORM_PROGRAM_LENGTH 136
#define NV2A_VERTEXSHADER_CONSTANTS 192
#define NV2A_MAX_LIGHTS 8

#define NV2A_LTCTXA_COUNT  26
#define NV2A_LTCTXB_COUNT  52
#define NV2A_LTC1_COUNT    20


#define NV2A_RENDER_MEMORY_ALIGNMENT        64

#define NV2A_SURFACE_ALIGNMENT                   NV2A_RENDER_MEMORY_ALIGNMENT
#define NV2A_TEXTURE_ALIGNMENT              (2 * NV2A_RENDER_MEMORY_ALIGNMENT)
#define NV2A_TEXTURE_CUBEMAP_FACE_ALIGNMENT (2 * NV2A_RENDER_MEMORY_ALIGNMENT)
#define NV2A_TEXTURE_PITCH_ALIGNMENT             NV2A_RENDER_MEMORY_ALIGNMENT
#define NV2A_TEXTURE_PITCH_MIN                   NV2A_TEXTURE_PITCH_ALIGNMENT

#endif
