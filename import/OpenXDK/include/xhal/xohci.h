// ******************************************************************
// * 
// * proj : OpenXDK
// *
// * desc : Open Source XBox Development Kit
// *
// * file : xohci.h
// *
// * note : XBox USB Open Host Controller Interface
// *
// ******************************************************************
#ifndef XOHCI_H
#define XOHCI_H

#if defined(__cplusplus)
extern "C"
{
#endif

// ******************************************************************
// * external functions
// ******************************************************************
void xohci_init();

// ******************************************************************
// * OHCI (PCI)
// ******************************************************************
#include "xohci_pci.h"

// ******************************************************************
// * struct : ed (Endpoint Descriptor)
// ******************************************************************
#pragma pack(16)
typedef struct _xohci_ed
{
    // ******************************************************************
    // * DWord 0
    // ******************************************************************
    uint32  m_function_addr   : 7;
    uint32  m_endpoint_numb   : 4;
    uint32  m_direction       : 2;
    uint32  m_speed           : 1;
    uint32  m_skip            : 1;
    uint32  m_format          : 1;
    uint32  m_max_packet_size : 11;
    uint32  m_reserved0       : 5;

    // ******************************************************************
    // * DWord 1
    // ******************************************************************
    uint32  m_reserved1       : 4;
    uint32  m_tail_p          : 28;

    // ******************************************************************
    // * DWord 2
    // ******************************************************************
    uint32  m_halted          : 1;
    uint32  m_toggle_carry    : 1;
    uint32  m_zero_bit        : 2;
    uint32  m_head_p          : 28;

    // ******************************************************************
    // * DWord 1
    // ******************************************************************
    uint32  m_reserved3       : 4;
    uint32  m_next_ed         : 28;
}
xohci_ed;
#pragma pack()

// ******************************************************************
// * Endpoint Descriptor Flags
// ******************************************************************
#define ED_DIRECTION_IN     0
#define ED_DIRECTION_OUT    1
#define ED_SPEED_FULL       0
#define ED_SPEED_LOW        1

// ******************************************************************
// * struct : td (Transfer Descriptor)
// ******************************************************************
// *
// * Since XBox controllers are not isochronous, we do not need
// * to support that transfer type. therefore, we also do not need
// * to align this structure to 32 bytes, as general transfer desc
// * only require 16 byte alignment
// *
// ******************************************************************
#pragma pack(16)
typedef struct _xohci_td
{
    // ******************************************************************
    // * DWord 0
    // ******************************************************************
    uint32  m_reserved18      : 18;     // (NA)
    uint32  m_buffer_rounding : 1;      // (RO) R
    uint32  m_direction_pid   : 2;      // (RO) DP
    uint32  m_delay_int       : 3;      // (RO) DI
    uint32  m_data_toggle     : 2;      // (RW) T
    uint32  m_error_count     : 2;      // (RW) EC
    uint32  m_condition_code  : 4;      // (RW) CC

    // ******************************************************************
    // * DWord 1
    // ******************************************************************
    uint32  m_current_buffer  : 32;     // (RW) CBP

    // ******************************************************************
    // * DWord 2
    // ******************************************************************
    uint32  m_zero4           : 4;      // (00) Should be Zero
    uint32  m_next_td         : 28;     // (RW) NextTD

    // ******************************************************************
    // * DWord 1
    // ******************************************************************
    uint32  m_buffer_end      : 32;     // (RO) BE
}
xohci_td;
#pragma pack()

// ******************************************************************
// * Transfer Descriptor Flags
// ******************************************************************
#define TD_DP_SETUP         0x00
#define TD_DP_OUT           0x01
#define TD_DP_IN            0x02
#define TD_T_DATA0          0x02
#define TD_T_DATA1          0x03
#define TD_T_TOGGLE         0x00

// ******************************************************************
// * Condition Code Values
// ******************************************************************
#define TD_CC_NOERROR       0x0000
#define TD_CC_CRC           0x0001
#define TD_CC_BITSTUFFING   0x0002
#define TD_CC_DATATOGGLEM   0x0003
#define TD_CC_STALL         0x0004
#define TD_DEVNOTRESP       0x0005
#define TD_PIDCHECKFAIL     0x0006
#define TD_UNEXPECTEDPID    0x0007
#define TD_DATAOVERRUN      0x0008
#define TD_DATAUNDERRUN     0x0009
#define TD_BUFFEROVERRUN    0x000C
#define TD_BUFFERUNDERRUN   0x000D
#define TD_NOTACCESSED      0x000F

// ******************************************************************
// * struct : xohci_hcca (Host Controller Communications Area)
// ******************************************************************
// *
// * Note: This must be 256-byte aligned
// *
// ******************************************************************
#pragma pack(1)
typedef struct _xohci_hcca
{
    uint32 int_table[32];           // (RO) HccaInterruptTable
    uint16 frame_number;            // (WO) HccaFrameNumber
    uint16 pad1;                    // (WO) HccaPad1
    uint32 done_head;               // (WO) HccaDoneHead
    uint08 reserved[116];           // (RW) Reserved for use by Host Controller
    // less than 256 bytes..why?
}
xohci_hcca;
#pragma pack()

// ******************************************************************
// * class : ohci_regs (OHCI Host Controller Operational Registers)
// ******************************************************************
// *
// * NOTE: These fields must be accessed using the functions:
// *
// * WRITE_REGISTER_* and READ_REGISTER_*
// *
// * NOTE: These should fall naturally on a 32 byte boundary
// *
// ******************************************************************
typedef struct _xohci_regs
{
    uint32 hc_revision;
    uint32 hc_control;
    uint32 hc_cmdstatus;
    uint32 hc_int_status;
    uint32 hc_int_enable;
    uint32 hc_int_disable;

    uint32 hc_hcca;
    uint32 hc_period_cur;
    uint32 hc_control_head;
    uint32 hc_control_cur;
    uint32 hc_bulk_head;
    uint32 hc_bulk_cur;
    uint32 hc_done_head;

    uint32 hc_fm_interval;
    uint32 hc_fm_remaining;
    uint32 hc_fm_number;
    uint32 hc_periodic_start;
    uint32 hc_ls_threshold;

    uint32 hc_rh_descriptor_a;
    uint32 hc_rh_descriptor_b;
    uint32 hc_rh_status;
    uint32 hc_rh_port_status[15];
}
xohci_regs;

// ******************************************************************
// * global pointer to OHCI Host Controller Operational Registers
// ******************************************************************
// ERROR DURING LINKING
// ERROR DURING LINKING
// ERROR DURING LINKING
// ERROR DURING LINKING
// ERROR DURING LINKING - so its definition was moved to 
//                        xohci.c
extern xohci_regs *g_xohci_regs;

// ******************************************************************
// * hc_control register masks
// ******************************************************************
#define XOHCI_CTRL_CBSR	(3 << 0)	// control/bulk service ratio
#define XOHCI_CTRL_PLE	(1 << 2)	// periodic list enable
#define XOHCI_CTRL_IE	(1 << 3)	// isochronous enable
#define XOHCI_CTRL_CLE	(1 << 4)	// control list enable
#define XOHCI_CTRL_BLE	(1 << 5)	// bulk list enable
#define XOHCI_CTRL_HCFS	(3 << 6)	// host controller functional state
#define XOHCI_CTRL_IR	(1 << 8)	// interrupt routing
#define XOHCI_CTRL_RWC	(1 << 9)	// remote wakeup connected
#define XOHCI_CTRL_RWE	(1 << 10)	// remote wakeup enable

// ******************************************************************
// * pre-shifted values for HC_CONTROL_FUNCTIONAL_STATE
// ******************************************************************
#define XOHCI_USB_RESET       (0 << 6)
#define XOHCI_USB_RESUME      (1 << 6)
#define XOHCI_USB_OPERATIONAL (2 << 6)
#define XOHCI_USB_SUSPEND     (3 << 6)

// ******************************************************************
// * hc_cmdstatus register masks
// ******************************************************************
#define XOHCI_HCR	(1 << 0)	// host controller reset
#define XOHCI_CLF  	(1 << 1)	// control list filled
#define XOHCI_BLF  	(1 << 2)	// bulk list filled
#define XOHCI_OCR  	(1 << 3)	// ownership change request
#define XOHCI_SOC  	(3 << 16)	// scheduling overrun count

// ******************************************************************
// * interrupt register masks (status/enable/disable regs)
// ******************************************************************
#define XOHCI_INTR_SO	(1 << 0)	// scheduling overrun
#define XOHCI_INTR_WDH	(1 << 1)	// writeback of done_head
#define XOHCI_INTR_SF	(1 << 2)	// start frame
#define XOHCI_INTR_RD	(1 << 3)	// resume detect
#define XOHCI_INTR_UE	(1 << 4)	// unrecoverable error
#define XOHCI_INTR_FNO	(1 << 5)	// frame number overflow
#define XOHCI_INTR_RHSC	(1 << 6)	// root hub status change
#define XOHCI_INTR_OC	(1 << 30)	// ownership change
#define XOHCI_INTR_MIE	(1 << 31)	// master interrupt enable

// ******************************************************************
// * struct : xohci
// ******************************************************************
// *
// * Packages up all of our variables
// *
// ******************************************************************
typedef struct _xohci
{
    xohci_hcca *m_hcca;
    uint32      m_hcca_dma;
    uint32      m_disabled;
}
xohci;

// ******************************************************************
// * global variables
// ******************************************************************
// ERROR DURING LINKING
// ERROR DURING LINKING
// ERROR DURING LINKING
// ERROR DURING LINKING - so its definition was moved to 
//                        xohci.c
extern xohci g_xohci;

#if defined(__cplusplus)
}
#endif

#endif
