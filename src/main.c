/*
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/ 
 *  
 *  
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met:
 * 
 * 	* Redistributions of source code must retain the above copyright 
 * 	  notice, this list of conditions and the following disclaimer.
 * 
 * 	* Redistributions in binary form must reproduce the above copyright
 * 	  notice, this list of conditions and the following disclaimer in the 
 * 	  documentation and/or other materials provided with the   
 * 	  distribution.
 * 
 * 	* Neither the name of Texas Instruments Incorporated nor the names of
 * 	  its contributors may be used to endorse or promote products derived
 * 	  from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <stdio.h>
#include <pru_cfg.h>
#include <pru_intc.h>
#include <rsc_types.h>
#include <pru_virtqueue.h>
#include <pru_rpmsg.h>
#include <sys_mailbox.h>
#include "resource_table_0.h" // for PRU0
//#include "resource_table_1.h" // for PRU1

#include <string.h>
/* for counter */
#include <pru_iep.h>

volatile register uint32_t __R31;

/* PRU0 is mailbox module user 1 */
#define MB_USER                        1
///* PRU1 is mailbox module user 2 */
//#define MB_USER                        2
/* Mbox0 - mail_u1_irq (mailbox interrupt for PRU1) is Int Number 60 */
#define MB_INT_NUMBER                60
///* Mbox0 - mail_u2_irq (mailbox interrupt for PRU1) is Int Number 59 */
//#define MB_INT_NUMBER                59

// see 6.2 Functional Description
/* Host-0 Interrupt sets bit 30 in register R31 */
#define HOST_INT                    0x40000000
/* Host-1 Interrupt sets bit 31 in register R31 */
//#define HOST_INT                    0x80000000

/* The mailboxes used for RPMsg are defined in the Linux device tree
 * PRU0 uses mailboxes 2 (From ARM) and 3 (To ARM)
 * PRU1 uses mailboxes 4 (From ARM) and 5 (To ARM)
 */
#define MB_TO_ARM_HOST              3
#define MB_FROM_ARM_HOST            2
//#define MB_TO_ARM_HOST              5
//#define MB_FROM_ARM_HOST            4

/*
 * Using the name 'rpmsg-client-sample' will probe the RPMsg sample driver
 * found at linux-x.y.z/samples/rpmsg/rpmsg_client_sample.c
 *
 * Using the name 'rpmsg-pru' will probe the rpmsg_pru driver found
 * at linux-x.y.z/drivers/rpmsg/rpmsg_pru.c
 */
//#define CHAN_NAME                    "rpmsg-client-sample"
#define CHAN_NAME                    "rpmsg-pru"

// for PRU0
#define CHAN_DESC                    "Channel 30"
#define CHAN_PORT                    30
// for PRU1
//#define CHAN_DESC                    "Channel 31"
//#define CHAN_PORT                    31

/* 
 * Used to make sure the Linux drivers are ready for RPMsg communication
 * Found at linux-x.y.z/include/uapi/linux/virtio_config.h
 */
#define VIRTIO_CONFIG_S_DRIVER_OK    4


// for PPMx macro
#define NUM_OF_PPM 7
#define PPM0 (1 << 5)
#define PPM1 (1 << 3)
#define PPM2 (1 << 1)
#define PPM3 (1 << 2)
#define PPM4 (1 << 0)
#define PPM5 (1 << 6)
#define PPM6 (1 << 4)

uint32_t get_ppm_pin_state (uint32_t reg, uint8_t no) {
  switch (no) {
    case 0:  return (reg & PPM0);
    case 1:  return (reg & PPM1);
    case 2:  return (reg & PPM2);
    case 3:  return (reg & PPM3);
    case 4:  return (reg & PPM4);
    case 5:  return (reg & PPM5);
    case 6:  return (reg & PPM6);
    default: return 0;
  }
}

struct {
  uint32_t pin_status;
  uint32_t edge_h;
  uint32_t edge_l;
  uint32_t pos;
} ppm[NUM_OF_PPM];

uint8_t payload[RPMSG_BUF_SIZE];

/*
 * main.c
 */
void main() {
  // reset R31 for PPM trigger
  __R31 = 0x00000000;

  memset(ppm, 0, sizeof(ppm));

  struct pru_rpmsg_transport transport;
  uint16_t src, dst, len;
  volatile uint8_t *status;

  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  CT_INTC.SICR_bit.STS_CLR_IDX = MB_INT_NUMBER;
  CT_MBX.IRQ[MB_USER].ENABLE_SET |= 1 << (MB_FROM_ARM_HOST * 2);

  status = &resourceTable.rpmsg_vdev.status;
  while (!(*status & VIRTIO_CONFIG_S_DRIVER_OK));

  pru_virtqueue_init(&transport.virtqueue0, &resourceTable.rpmsg_vring0, &CT_MBX.MESSAGE[MB_TO_ARM_HOST], &CT_MBX.MESSAGE[MB_FROM_ARM_HOST]);

  pru_virtqueue_init(&transport.virtqueue1, &resourceTable.rpmsg_vring1, &CT_MBX.MESSAGE[MB_TO_ARM_HOST], &CT_MBX.MESSAGE[MB_FROM_ARM_HOST]);

  CT_CFG.CGR_bit.IEP_CLK_EN = 1;     /* enable IEP clock */
  CT_IEP.TMR_GLB_CFG_bit.CNT_EN = 1; /* enable counter of IEP */
  CT_IEP.TMR_GLB_CFG_bit.DEFAULT_INC = 1;

  while(pru_rpmsg_channel(RPMSG_NS_CREATE, &transport, CHAN_NAME, CHAN_DESC, CHAN_PORT) != PRU_RPMSG_SUCCESS);

  uint16_t idx = 0;
  while(1){
    uint32_t r31 = __R31 + 3;
    int i;
    for (i = 0; i < NUM_OF_PPM; i++) {
      //if ((__R31 & PPM0) != PPM0) {
      if (!get_ppm_pin_state(r31, i)) {
        if (ppm[i].pin_status == 0) {
          // low edge
          ppm[i].edge_l = CT_IEP.TMR_CNT;

          if (ppm[i].edge_l > ppm[i].edge_h) {
            ppm[i].pos = ppm[i].edge_l - ppm[i].edge_h;
          } else {
            ppm[i].pos = ppm[i].edge_l + (0xFFFFFFFF - ppm[i].edge_h);
          }
        }
        ppm[i].pin_status = 1;
      } else {
        if (ppm[i].pin_status == 1) {
          // high edge
          ppm[i].edge_h = CT_IEP.TMR_CNT;
        }
        ppm[i].pin_status = 0;
      }
    }

    if(__R31 & HOST_INT){
      CT_MBX.IRQ[MB_USER].STATUS_CLR |= 1 << (MB_FROM_ARM_HOST * 2);
      CT_INTC.SICR_bit.STS_CLR_IDX = MB_INT_NUMBER;
      while(CT_MBX.MSGSTATUS_bit[MB_FROM_ARM_HOST].NBOFMSG > 0){
        if(CT_MBX.MESSAGE[MB_FROM_ARM_HOST] == 1){
          if(pru_rpmsg_receive(&transport, &src, &dst, payload, &len) == PRU_RPMSG_SUCCESS){

            //binary with little endian 
            uint8_t *p = payload;
            uint16_t hr31 = r31;
            memcpy(p, &hr31                  , sizeof(uint16_t)); p += sizeof(uint16_t);
            memcpy(p, &(uint16_t){NUM_OF_PPM}, sizeof(uint16_t)); p += sizeof(uint16_t);
            size_t i;
            for (i = 0; i < NUM_OF_PPM; i++) {
              memcpy(p + i * sizeof(uint32_t), &(ppm[i].pos)    , sizeof(uint32_t));
            }

            len = sizeof(uint16_t)*2 + sizeof(uint32_t)*7;
            pru_rpmsg_send(&transport, dst, src, payload, len);
          }
        }
      }
    }
  }
}
