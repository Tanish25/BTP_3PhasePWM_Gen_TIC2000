//###########################################################################
//
// FILE:   epwm_up_aq_cpu01.c
//
// TITLE:  Action Qualifier Module - Using up count.
//
//! \addtogroup cpu01_example_list
//! <h1> EPWM Action Qualifier (epwm_up_aq)</h1>
//!
//! This example configures ePWM1, ePWM2, ePWM3 to produce an
//! waveform with independent modulation on EPWMxA and
//! EPWMxB.
//!
//! The compare values CMPA and CMPB are modified within the ePWM's ISR.
//!
//! The TB counter is in up count mode for this example.
//!
//! View the EPWM1A/B(PA0_GPIO0 & PA1_GPIO1), EPWM2A/B(PA2_GPIO2 & PA3_GPIO3)
//! and EPWM3A/B(PA4_GPIO4 & PA5_GPIO5) waveforms via an oscilloscope.
//!
//
//###########################################################################
// $TI Release: F2837xD Support Library v3.04.00.00 $
// $Release Date: Sun Mar 25 13:26:04 CDT 2018 $
// $Copyright:
// Copyright (C) 2013-2018 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//###########################################################################

//
// Included Files
//
#include "F28x_Project.h"

//
// Defines
//
#define EPWM1_TIMER_TBPRD  400  // Period register
#define EPWM1_MAX_CMPA     360
#define EPWM1_MIN_CMPA     1
#define EPWM1_MAX_CMPB     360
#define EPWM1_MIN_CMPB     1

#define EPWM2_TIMER_TBPRD  400  // Period register
#define EPWM2_MAX_CMPA     360
#define EPWM2_MIN_CMPA     1
#define EPWM2_MAX_CMPB     360
#define EPWM2_MIN_CMPB     2

#define EPWM3_TIMER_TBPRD  400  // Period register
#define EPWM3_MAX_CMPA     360
#define EPWM3_MIN_CMPA     1
#define EPWM3_MAX_CMPB     360
#define EPWM3_MIN_CMPB     3

#define EPWM_CMP_UP           1
#define EPWM_CMP_DOWN         0

#define EPWM1_OFFSET 1
#define EPWM2_OFFSET 2
#define EPWM3_OFFSET 3

//
// Globals
//
typedef struct
{
    volatile struct EPWM_REGS *EPwmRegHandle;
    Uint16 EPwm_CMPA_Direction;
    Uint16 EPwm_CMPB_Direction;
    Uint16 EPwmTimerIntCount;
    Uint16 EPwmMaxCMPA;
    Uint16 EPwmMinCMPA;
    Uint16 EPwmMaxCMPB;
    Uint16 EPwmMinCMPB;
    Uint16 Offset;
}EPWM_INFO;

EPWM_INFO epwm1_info;
EPWM_INFO epwm2_info;
EPWM_INFO epwm3_info;

//
//  Function Prototypes
//
void InitEPwm1Example(void);
void InitEPwm2Example(void);
void InitEPwm3Example(void);
__interrupt void epwm1_isr(void);
__interrupt void epwm2_isr(void);
__interrupt void epwm3_isr(void);
void update_compare(EPWM_INFO*);

Uint16 LuT[360]={180,183,186,189,193,196,199,202,205,208,211,214,217,220,224,227,230,233,236,239,242,245,247,250,253,256,259,262,265,267,
                 270,273,275,278,281,283,286,288,291,293,296,298,300,303,305,307,309,312,314,316,318,320,322,324,326,327,329,331,333,334,
                 336,337,339,340,342,343,344,346,347,348,349,350,351,352,353,354,355,355,356,357,357,358,358,359,359,359,360,360,360,360,
                 360,360,360,360,360,359,359,359,358,358,357,357,356,355,355,354,353,352,351,350,349,348,347,346,344,343,342,340,339,337,
                 336,334,333,331,329,327,326,324,322,320,318,316,314,312,309,307,305,303,300,298,296,293,291,288,286,283,281,278,275,273,
                 270,267,265,262,259,256,253,250,247,245,242,239,236,233,230,227,224,220,217,214,211,208,205,202,199,196,193,189,186,183,
                 180,177,174,171,167,164,161,158,155,152,149,146,143,140,136,133,130,127,124,121,118,115,113,110,107,104,101,98,95,93,
                 90,87,85,82,79,77,74,72,69,67,64,62,60,57,55,53,51,48,46,44,42,40,38,36,34,33,31,29,27,26,
                 24,23,21,20,18,17,16,14,13,12,11,10,9,8,7,6,5,5,4,3,3,2,2,1,1,1,0,0,0,0,
                 0,0,0,0,0,1,1,1,2,2,3,3,4,5,5,6,7,8,9,10,11,12,13,14,16,17,18,20,21,23,
                 24,26,27,29,31,33,34,36,38,40,42,44,46,48,51,53,55,57,60,62,64,67,69,72,74,77,79,82,85,87,
                 90,93,95,98,101,104,107,110,113,115,118,121,124,127,130,133,136,140,143,146,149,152,155,158,161,164,167,171,174,177};

/*Uint16 LuT_2[360]={336,334,333,331,329,327,326,324,322,320,318,316,314,312,309,307,305,303,300,298,296,293,291,288,286,283,281,278,275,273,
                 270,267,265,262,259,256,253,250,247,245,242,239,236,233,230,227,224,220,217,214,211,208,205,202,199,196,193,189,186,183,
                 180,177,174,171,167,164,161,158,155,152,149,146,143,140,136,133,130,127,124,121,118,115,113,110,107,104,101,98,95,93,
                 90,87,85,82,79,77,74,72,69,67,64,62,60,57,55,53,51,48,46,44,42,40,38,36,34,33,31,29,27,26,
                 24,23,21,20,18,17,16,14,13,12,11,10,9,8,7,6,5,5,4,3,3,2,2,1,1,1,0,0,0,0,
                 0,0,0,0,0,1,1,1,2,2,3,3,4,5,5,6,7,8,9,10,11,12,13,14,16,17,18,20,21,23,
                 24,26,27,29,31,33,34,36,38,40,42,44,46,48,51,53,55,57,60,62,64,67,69,72,74,77,79,82,85,87,
                 90,93,95,98,101,104,107,110,113,115,118,121,124,127,130,133,136,140,143,146,149,152,155,158,161,164,167,171,174,177,180,183,186,189,193,196,199,202,205,208,211,214,217,220,224,227,230,233,236,239,242,245,247,250,253,256,259,262,265,267,
                 270,273,275,278,281,283,286,288,291,293,296,298,300,303,305,307,309,312,314,316,318,320,322,324,326,327,329,331,333,334,
                 336,337,339,340,342,343,344,346,347,348,349,350,351,352,353,354,355,355,356,357,357,358,358,359,359,359,360,360,360,360,
                 360,360,360,360,360,359,359,359,358,358,357,357,356,355,355,354,353,352,351,350,349,348,347,346,344,343,342,340,339,337};

Uint16 LuT_3[360]={
                 24,23,21,20,18,17,16,14,13,12,11,10,9,8,7,6,5,5,4,3,3,2,2,1,1,1,0,0,0,0,
                 0,0,0,0,0,1,1,1,2,2,3,3,4,5,5,6,7,8,9,10,11,12,13,14,16,17,18,20,21,23,
                 24,26,27,29,31,33,34,36,38,40,42,44,46,48,51,53,55,57,60,62,64,67,69,72,74,77,79,82,85,87,
                 90,93,95,98,101,104,107,110,113,115,118,121,124,127,130,133,136,140,143,146,149,152,155,158,161,164,167,171,174,177,
                 180,183,186,189,193,196,199,202,205,208,211,214,217,220,224,227,230,233,236,239,242,245,247,250,253,256,259,262,265,267,
                                  270,273,275,278,281,283,286,288,291,293,296,298,300,303,305,307,309,312,314,316,318,320,322,324,326,327,329,331,333,334,
                                  336,337,339,340,342,343,344,346,347,348,349,350,351,352,353,354,355,355,356,357,357,358,358,359,359,359,360,360,360,360,
                                  360,360,360,360,360,359,359,359,358,358,357,357,356,355,355,354,353,352,351,350,349,348,347,346,344,343,342,340,339,337,
                                  336,334,333,331,329,327,326,324,322,320,318,316,314,312,309,307,305,303,300,298,296,293,291,288,286,283,281,278,275,273,
                                  270,267,265,262,259,256,253,250,247,245,242,239,236,233,230,227,224,220,217,214,211,208,205,202,199,196,193,189,186,183,
                                  180,177,174,171,167,164,161,158,155,152,149,146,143,140,136,133,130,127,124,121,118,115,113,110,107,104,101,98,95,93,
                                  90,87,85,82,79,77,74,72,69,67,64,62,60,57,55,53,51,48,46,44,42,40,38,36,34,33,31,29,27,26};

*/
Uint16 index=0;

//
// Main
//
void main(void)
{
//
// Step 1. Initialize System Control:
// PLL, WatchDog, enable Peripheral Clocks
// This example function is found in the F2837xD_SysCtrl.c file.
//
   InitSysCtrl();

//
// Step 2. Initialize GPIO:
// This example function is found in the F2837xD_Gpio.c file and
// illustrates how to set the GPIO to it's default state.
//
//    InitGpio();

//
// Enable PWM1, PWM2 and PWM3
//
    CpuSysRegs.PCLKCR2.bit.EPWM1=1;
    CpuSysRegs.PCLKCR2.bit.EPWM2=1;
    CpuSysRegs.PCLKCR2.bit.EPWM3=1;

//
// For this case just init GPIO pins for ePWM1, ePWM2, ePWM3
// These functions are in the F2837xD_EPwm.c file
//
    InitEPwm1Gpio();
    InitEPwm2Gpio();
    InitEPwm3Gpio();

//
// Step 3. Clear all interrupts and initialize PIE vector table:
// Disable CPU interrupts
//
    DINT;

//
// Initialize the PIE control registers to their default state.
// The default state is all PIE interrupts disabled and flags
// are cleared.
// This function is found in the F2837xD_PieCtrl.c file.
//
    InitPieCtrl();

//
// Disable CPU interrupts and clear all CPU interrupt flags:
//
    IER = 0x0000;
    IFR = 0x0000;

//
// Initialize the PIE vector table with pointers to the shell Interrupt
// Service Routines (ISR).
// This will populate the entire table, even if the interrupt
// is not used in this example.  This is useful for debug purposes.
// The shell ISR routines are found in F2837xD_DefaultIsr.c.
// This function is found in F2837xD_PieVect.c.
//
    InitPieVectTable();

//
// Interrupts that are used in this example are re-mapped to
// ISR functions found within this file.
//
    EALLOW; // This is needed to write to EALLOW protected registers
    PieVectTable.EPWM1_INT = &epwm1_isr;
    PieVectTable.EPWM2_INT = &epwm2_isr;
    PieVectTable.EPWM3_INT = &epwm3_isr;
    EDIS;   // This is needed to disable write to EALLOW protected registers

//
// For this example, only initialize the ePWM
//
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    EDIS;

    InitEPwm1Example();
    InitEPwm2Example();
    InitEPwm3Example();

    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;

//
// Step 4. User specific code, enable interrupts:
//
// Enable CPU INT3 which is connected to EPWM1-3 INT:
//
    IER |= M_INT3;

//
// Enable EPWM INTn in the PIE: Group 3 interrupt 1-3
//
    PieCtrlRegs.PIEIER3.bit.INTx1 = 1;
    PieCtrlRegs.PIEIER3.bit.INTx2 = 1;
    PieCtrlRegs.PIEIER3.bit.INTx3 = 1;

//
// Enable global Interrupts and higher priority real-time debug events:
//
    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global realtime interrupt DBGM

//
// Step 5. IDLE loop. Just sit and loop forever (optional):
//
    for(;;)
    {
        asm ("  NOP");
    }
}

//
// epwm1_isr - EPWM1 ISR to update compare values
//
__interrupt void epwm1_isr(void)
{
    //
    // Update the CMPA and CMPB values
    //
    update_compare(&epwm1_info);

    //
    // Clear INT flag for this timer
    //
    EPwm1Regs.ETCLR.bit.INT = 1;

    //
    // Acknowledge this interrupt to receive more interrupts from group 3
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

//
// epwm2_isr - EPWM2 ISR to update compare values
//
__interrupt void epwm2_isr(void)
{
    //
    // Update the CMPA and CMPB values
    //
    update_compare(&epwm2_info);

    //
    // Clear INT flag for this timer
    //
    EPwm2Regs.ETCLR.bit.INT = 1;

    //
    // Acknowledge this interrupt to receive more interrupts from group 3
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

//
// epwm3_isr - EPWM3 ISR to update compare values
//
__interrupt void epwm3_isr(void)
{
    //
    // Update the CMPA and CMPB values
    //
    update_compare(&epwm3_info);

    //
    // Clear INT flag for this timer
    //
    EPwm3Regs.ETCLR.bit.INT = 1;

    //
    // Acknowledge this interrupt to receive more interrupts from group 3
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

//
// InitEPwm1Example - Initialize EPWM1 values
//
void InitEPwm1Example()
{
   //
   // Setup TBCLK
   //
   EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
   EPwm1Regs.TBPRD = EPWM1_TIMER_TBPRD;       // Set timer period
   EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
   EPwm1Regs.TBPHS.bit.TBPHS = 0x0000;        // Phase is 0
   EPwm1Regs.TBCTR = 0x0000;                  // Clear counter
   EPwm1Regs.TBCTL.bit.HSPCLKDIV =0x01;//TB_DIV4;   // Clock ratio to SYSCLKOUT
   EPwm1Regs.TBCTL.bit.CLKDIV = 0x01;//TB_DIV4;

   //
   // Setup shadow register load on ZERO
   //
   EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
   EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

   //
   // Set Compare values
   //
   EPwm1Regs.CMPA.bit.CMPA = LuT[EPWM1_MIN_CMPA-1];      // Set compare A value with offset
   EPwm1Regs.CMPB.bit.CMPB = LuT[EPWM1_MIN_CMPB-1];      // Set Compare B value with offset

   //
   EPwm1Regs.AQCTLA.bit.ZRO = AQ_SET;            // Set PWM2A on Zero
      EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;          // Clear PWM2A on event A,
                                                    // up count

      EPwm1Regs.AQCTLB.bit.ZRO = AQ_CLEAR;            // Clear PWM2B on Zero
      EPwm1Regs.AQCTLB.bit.CBU = AQ_SET;          // Set PWM2B on event B,
                                                    // up count
   // Interrupt where we will change the Compare Values
   //
   EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;       // Select INT on Zero event
   EPwm1Regs.ETSEL.bit.INTEN = 1;                  // Enable INT
   EPwm1Regs.ETPS.bit.INTPRD = ET_3RD;             // Generate INT on 3rd event

   //
   // Information this example uses to keep track
   // of the direction the CMPA/CMPB values are
   // moving, the min and max allowed values and
   // a pointer to the correct ePWM registers
   //
   epwm1_info.EPwm_CMPA_Direction = EPWM_CMP_UP;   // Start by increasing CMPA
   epwm1_info.EPwm_CMPB_Direction = EPWM_CMP_UP; // and increasing CMPB
   epwm1_info.EPwmTimerIntCount = 0;               // Zero the interrupt
                                                   // counter
   epwm1_info.EPwmRegHandle = &EPwm1Regs;          // Set the pointer to the
                                                   // ePWM module
   epwm1_info.EPwmMaxCMPA = EPWM1_MAX_CMPA;        // Setup min/max
                                                   // CMPA/CMPB values index
   epwm1_info.EPwmMinCMPA = EPWM1_MIN_CMPA;
   epwm1_info.EPwmMaxCMPB = EPWM1_MAX_CMPB;
   epwm1_info.EPwmMinCMPB = EPWM1_MIN_CMPB;
   epwm1_info.Offset= EPWM1_OFFSET;
}

//
// InitEPwm2Example - Initialize EPWM2 values
//
void InitEPwm2Example()
{
   //
   // Setup TBCLK
   //
   EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
   EPwm2Regs.TBPRD = EPWM2_TIMER_TBPRD;       // Set timer period
   EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
   EPwm2Regs.TBPHS.bit.TBPHS = 0x0000;        // Phase is 0
   EPwm2Regs.TBCTR = 0x0000;                  // Clear counter
   EPwm2Regs.TBCTL.bit.HSPCLKDIV = 0x01;//TB_DIV4;   // Clock ratio to SYSCLKOUT
   EPwm2Regs.TBCTL.bit.CLKDIV = 0x01;//TB_DIV4;

   //
   // Setup shadow register load on ZERO
   //
   EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
   EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

   //
   // Set Compare values
   //
   EPwm2Regs.CMPA.bit.CMPA = LuT[EPWM2_MIN_CMPA+120];      // Set compare A value with offset
   EPwm2Regs.CMPB.bit.CMPB = LuT[EPWM2_MIN_CMPB+120];      // Set Compare B value with offset

   //
   EPwm2Regs.AQCTLA.bit.ZRO = AQ_SET;            // Set PWM2A on Zero
      EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR;          // Clear PWM2A on event A,
                                                    // up count

      EPwm2Regs.AQCTLB.bit.ZRO = AQ_CLEAR;            // Clear PWM2B on Zero
      EPwm2Regs.AQCTLB.bit.CBU = AQ_SET;          // Set PWM2B on event B,
                                                    // up count
   // Interrupt where we will change the Compare Values
   //
   EPwm2Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;       // Select INT on Zero event
   EPwm2Regs.ETSEL.bit.INTEN = 1;                  // Enable INT
   EPwm2Regs.ETPS.bit.INTPRD = ET_3RD;             // Generate INT on 3rd event

   //
   // Information this example uses to keep track
   // of the direction the CMPA/CMPB values are
   // moving, the min and max allowed values and
   // a pointer to the correct ePWM registers
   //
   epwm2_info.EPwm_CMPA_Direction = EPWM_CMP_UP;   // Start by increasing CMPA
   epwm2_info.EPwm_CMPB_Direction = EPWM_CMP_UP; // and increasing CMPB
   epwm2_info.EPwmTimerIntCount = 0;               // Zero the interrupt
                                                   // counter
   epwm2_info.EPwmRegHandle = &EPwm2Regs;          // Set the pointer to the
                                                   // ePWM module
   epwm2_info.EPwmMaxCMPA =EPWM2_MAX_CMPA;        // Setup min/max
                                                   // CMPA/CMPB index values
   epwm2_info.EPwmMinCMPA = EPWM2_MIN_CMPA;
   epwm2_info.EPwmMaxCMPB = EPWM2_MAX_CMPB;
   epwm2_info.EPwmMinCMPB = EPWM2_MIN_CMPB;
   epwm2_info.Offset= EPWM2_OFFSET;
}

//
// InitEPwm3Example - Initialize EPWM3 values
//
void InitEPwm3Example()
{
   //
   // Setup TBCLK
   //
   EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
   EPwm3Regs.TBPRD = EPWM3_TIMER_TBPRD;       // Set timer period
   EPwm3Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
   EPwm3Regs.TBPHS.bit.TBPHS = 0x0000;        // Phase is 0
   EPwm3Regs.TBCTR = 0x0000;                  // Clear counter
   EPwm3Regs.TBCTL.bit.HSPCLKDIV = 0x01;//TB_DIV4;   // Clock ratio to SYSCLKOUT
   EPwm3Regs.TBCTL.bit.CLKDIV = 0x01;//TB_DIV4;

   //
   // Setup shadow register load on ZERO
   //
   EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
   EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
   EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

   //
   // Set Compare values
   //
   EPwm3Regs.CMPA.bit.CMPA = LuT[EPWM3_MIN_CMPA+240-1];      // Set compare A value with offset
   EPwm3Regs.CMPB.bit.CMPB = LuT[EPWM3_MIN_CMPB+240-1];      // Set Compare B value with offset

   //
   EPwm3Regs.AQCTLA.bit.ZRO = AQ_SET;            // Set PWM2A on Zero
      EPwm3Regs.AQCTLA.bit.CAU = AQ_CLEAR;          // Clear PWM2A on event A,
                                                    // up count

      EPwm3Regs.AQCTLB.bit.ZRO = AQ_CLEAR;            // Clear PWM2B on Zero
      EPwm3Regs.AQCTLB.bit.CBU = AQ_SET;          // Set PWM2B on event B,
                                                    // up count
   // Interrupt where we will change the Compare Values
   //
   EPwm3Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;       // Select INT on Zero event
   EPwm3Regs.ETSEL.bit.INTEN = 1;                  // Enable INT
   EPwm3Regs.ETPS.bit.INTPRD = ET_3RD;             // Generate INT on 3rd event

   //
   // Information this example uses to keep track
   // of the direction the CMPA/CMPB values are
   // moving, the min and max allowed values and
   // a pointer to the correct ePWM registers
   //
   epwm3_info.EPwm_CMPA_Direction = EPWM_CMP_UP;   // Start by increasing CMPA
   epwm3_info.EPwm_CMPB_Direction = EPWM_CMP_UP; // and increasing CMPB
   epwm3_info.EPwmTimerIntCount = 0;               // Zero the interrupt
                                                   // counter
   epwm3_info.EPwmRegHandle = &EPwm3Regs;          // Set the pointer to the
                                                   // ePWM module
   epwm3_info.EPwmMaxCMPA = EPWM3_MAX_CMPA;        // Setup min/max
                                                   // CMPA/CMPB index values
   epwm3_info.EPwmMinCMPA = EPWM3_MIN_CMPA;
   epwm3_info.EPwmMaxCMPB = EPWM3_MAX_CMPB;
   epwm3_info.EPwmMinCMPB = EPWM3_MIN_CMPB;
   epwm3_info.Offset= EPWM3_OFFSET;
}

//
// update_compare - Update the compare values for the specified EPWM
//
void update_compare(EPWM_INFO *epwm_info)
{
if(epwm_info->EPwmMinCMPB == 2)
{

   //
   // Every nth interrupt, change the CMPA/CMPB values
   //
    if(epwm_info->EPwmTimerIntCount == 2)
    {
        epwm_info->EPwmTimerIntCount = 0;
        //index=0;
        //index=index+epwm_info->Offset;
        //if(index==359)
        //{index=0;}
        //index=0;

        //
        // If we were increasing CMPA, check to see if
        // we reached the max value.  If not, increase CMPA
        // else, change directions and decrease CMPA
        //
        if(epwm_info->EPwm_CMPA_Direction == EPWM_CMP_UP)
        {
            //if(epwm_info->EPwmRegHandle->CMPA.bit.CMPA < 361)
            if(index+120<480)
            {
                if(index+120<360)
                {
                index=index+1;
               epwm_info->EPwmRegHandle->CMPA.bit.CMPA=LuT[index+120];
                }
                else
                {
                    index=index+1;
                                   epwm_info->EPwmRegHandle->CMPA.bit.CMPA=LuT[index+120-360];

                }
            }

            else
            {
                index=0;
               epwm_info->EPwm_CMPA_Direction = EPWM_CMP_DOWN;
               //index=180;
               //epwm_info->EPwmRegHandle->CMPA.bit.CMPA=LuT[index];
            }
        }

        //
        // If we were decreasing CMPA, check to see if
        // we reached the min value.  If not, decrease CMPA
        // else, change directions and increase CMPA
        //
        else
        {

                         index=0;
                         epwm_info->EPwm_CMPA_Direction = EPWM_CMP_UP;


            /*
            if(epwm_info->EPwmRegHandle->CMPA.bit.CMPA == 180)
            {
               epwm_info->EPwm_CMPA_Direction = EPWM_CMP_UP;
               index=index+1;
               epwm_info->EPwmRegHandle->CMPA.bit.CMPA=LuT[index];
            }
            */
            /*
            else
            {
                index=index-1;
               epwm_info->EPwmRegHandle->CMPA.bit.CMPA=LuT[index];
            }
            */
        }

        //
        // If we were increasing CMPB, check to see if
        // we reached the max value.  If not, increase CMPB
        // else, change directions and decrease CMPB
        //
        if(epwm_info->EPwm_CMPB_Direction == EPWM_CMP_UP)
               {
                   //if(epwm_info->EPwmRegHandle->CMPA.bit.CMPA < 361)
                   if(index+120<480)
                   {
                       if(index+120<360)
                       {
                       index=index+1;
                      epwm_info->EPwmRegHandle->CMPB.bit.CMPB=LuT[index+120];
                       }
                       else
                       {
                           index=index+1;
                                          epwm_info->EPwmRegHandle->CMPB.bit.CMPB=LuT[index+120-360];

                       }
                   }

                   else
                   {
                       index=0;
                      epwm_info->EPwm_CMPB_Direction = EPWM_CMP_DOWN;
                      //index=180;
                      //epwm_info->EPwmRegHandle->CMPA.bit.CMPA=LuT[index];
                   }
               }

               //
               // If we were decreasing CMPA, check to see if
               // we reached the min value.  If not, decrease CMPA
               // else, change directions and increase CMPA
               //
               else
               {

                                index=0;
                                epwm_info->EPwm_CMPB_Direction = EPWM_CMP_UP;


                   /*
                   if(epwm_info->EPwmRegHandle->CMPA.bit.CMPA == 180)
                   {
                      epwm_info->EPwm_CMPA_Direction = EPWM_CMP_UP;
                      index=index+1;
                      epwm_info->EPwmRegHandle->CMPA.bit.CMPA=LuT[index];
                   }
                   */
                   /*
                   else
                   {
                       index=index-1;
                      epwm_info->EPwmRegHandle->CMPA.bit.CMPA=LuT[index];
                   }
                   */
               }

               //
               // If we were increasing CMPB, check to see if
               // we reached the max value.  If not, increase CMPB
               // else, change directions and decrease CMPB
               //
    }
   else
   {
      epwm_info->EPwmTimerIntCount++;
   }

   return;
}
if(epwm_info->EPwmMinCMPB == 1)
{

   //
   // Every nth interrupt, change the CMPA/CMPB values
   //
    if(epwm_info->EPwmTimerIntCount == 2)
    {
        epwm_info->EPwmTimerIntCount = 0;
        //index=0;
        //index=index+epwm_info->Offset;
        //if(index==359)
        //{index=0;}
        //index=0;

        //
        // If we were increasing CMPA, check to see if
        // we reached the max value.  If not, increase CMPA
        // else, change directions and decrease CMPA
        //
        if(epwm_info->EPwm_CMPA_Direction == EPWM_CMP_UP)
        {
            //if(epwm_info->EPwmRegHandle->CMPA.bit.CMPA < 361)
            if(index+0<360)
            {
                if(index+0<360)
                {
                index=index+1;
               epwm_info->EPwmRegHandle->CMPA.bit.CMPA=LuT[index+0];
                }
                //else
                //{
                 //   index=index+1;
                   //                epwm_info->EPwmRegHandle->CMPA.bit.CMPA=LuT[index+120-360];

               // }
            }

            else
            {
                index=0;
               epwm_info->EPwm_CMPA_Direction = EPWM_CMP_DOWN;
               //index=180;
               //epwm_info->EPwmRegHandle->CMPA.bit.CMPA=LuT[index];
            }
        }

        //
        // If we were decreasing CMPA, check to see if
        // we reached the min value.  If not, decrease CMPA
        // else, change directions and increase CMPA
        //
        else
        {

                         index=0;
                         epwm_info->EPwm_CMPA_Direction = EPWM_CMP_UP;


            /*
            if(epwm_info->EPwmRegHandle->CMPA.bit.CMPA == 180)
            {
               epwm_info->EPwm_CMPA_Direction = EPWM_CMP_UP;
               index=index+1;
               epwm_info->EPwmRegHandle->CMPA.bit.CMPA=LuT[index];
            }
            */
            /*
            else
            {
                index=index-1;
               epwm_info->EPwmRegHandle->CMPA.bit.CMPA=LuT[index];
            }
            */
        }

        //
        // If we were increasing CMPB, check to see if
        // we reached the max value.  If not, increase CMPB
        // else, change directions and decrease CMPB
        //
        if(epwm_info->EPwm_CMPB_Direction == EPWM_CMP_UP)
               {
                   //if(epwm_info->EPwmRegHandle->CMPA.bit.CMPA < 361)
                   if(index+0<360)
                   {
                       if(index+0<360)
                       {
                       index=index+1;
                      epwm_info->EPwmRegHandle->CMPB.bit.CMPB=LuT[index+0];
                       }
                      /* else
                       {
                           index=index+1;
                                          epwm_info->EPwmRegHandle->CMPB.bit.CMPB=LuT[index+120-360];

                       }*/
                   }

                   else
                   {
                       index=0;
                      epwm_info->EPwm_CMPB_Direction = EPWM_CMP_DOWN;
                      //index=180;
                      //epwm_info->EPwmRegHandle->CMPA.bit.CMPA=LuT[index];
                   }
               }

               //
               // If we were decreasing CMPA, check to see if
               // we reached the min value.  If not, decrease CMPA
               // else, change directions and increase CMPA
               //
               else
               {

                                index=0;
                                epwm_info->EPwm_CMPB_Direction = EPWM_CMP_UP;


                   /*
                   if(epwm_info->EPwmRegHandle->CMPA.bit.CMPA == 180)
                   {
                      epwm_info->EPwm_CMPA_Direction = EPWM_CMP_UP;
                      index=index+1;
                      epwm_info->EPwmRegHandle->CMPA.bit.CMPA=LuT[index];
                   }
                   */
                   /*
                   else
                   {
                       index=index-1;
                      epwm_info->EPwmRegHandle->CMPA.bit.CMPA=LuT[index];
                   }
                   */
               }

               //
               // If we were increasing CMPB, check to see if
               // we reached the max value.  If not, increase CMPB
               // else, change directions and decrease CMPB
               //
    }
   else
   {
      epwm_info->EPwmTimerIntCount++;
   }

   return;
}

if(epwm_info->EPwmMinCMPB == 3)
{

   //
   // Every nth interrupt, change the CMPA/CMPB values
   //
    if(epwm_info->EPwmTimerIntCount == 2)
    {
        epwm_info->EPwmTimerIntCount = 0;
        //index=0;
        //index=index+epwm_info->Offset;
        //if(index==359)
        //{index=0;}
        //index=0;

        //
        // If we were increasing CMPA, check to see if
        // we reached the max value.  If not, increase CMPA
        // else, change directions and decrease CMPA
        //
        if(epwm_info->EPwm_CMPA_Direction == EPWM_CMP_UP)
        {
            //if(epwm_info->EPwmRegHandle->CMPA.bit.CMPA < 361)
            if(index+240<600)
            {
                if(index+240<360)
                {
                index=index+1;
               epwm_info->EPwmRegHandle->CMPA.bit.CMPA=LuT[index+240];
                }
                else
                {
                    index=index+1;
                                   epwm_info->EPwmRegHandle->CMPA.bit.CMPA=LuT[index+240-360];

                }
            }

            else
            {
                index=0;
               epwm_info->EPwm_CMPA_Direction = EPWM_CMP_DOWN;
               //index=180;
               //epwm_info->EPwmRegHandle->CMPA.bit.CMPA=LuT[index];
            }
        }

        //
        // If we were decreasing CMPA, check to see if
        // we reached the min value.  If not, decrease CMPA
        // else, change directions and increase CMPA
        //
        else
        {

                         index=0;
                         epwm_info->EPwm_CMPA_Direction = EPWM_CMP_UP;


            /*
            if(epwm_info->EPwmRegHandle->CMPA.bit.CMPA == 180)
            {
               epwm_info->EPwm_CMPA_Direction = EPWM_CMP_UP;
               index=index+1;
               epwm_info->EPwmRegHandle->CMPA.bit.CMPA=LuT[index];
            }
            */
            /*
            else
            {
                index=index-1;
               epwm_info->EPwmRegHandle->CMPA.bit.CMPA=LuT[index];
            }
            */
        }

        //
        // If we were increasing CMPB, check to see if
        // we reached the max value.  If not, increase CMPB
        // else, change directions and decrease CMPB
        //
        if(epwm_info->EPwm_CMPB_Direction == EPWM_CMP_UP)
               {
                   //if(epwm_info->EPwmRegHandle->CMPA.bit.CMPA < 361)
                   if(index+240<600)
                   {
                       if(index+240<360)
                       {
                       index=index+1;
                      epwm_info->EPwmRegHandle->CMPB.bit.CMPB=LuT[index+240];
                       }
                       else
                       {
                           index=index+1;
                                          epwm_info->EPwmRegHandle->CMPB.bit.CMPB=LuT[index+240-360];

                       }
                   }

                   else
                   {
                       index=0;
                      epwm_info->EPwm_CMPB_Direction = EPWM_CMP_DOWN;
                      //index=180;
                      //epwm_info->EPwmRegHandle->CMPA.bit.CMPA=LuT[index];
                   }
               }

               //
               // If we were decreasing CMPA, check to see if
               // we reached the min value.  If not, decrease CMPA
               // else, change directions and increase CMPA
               //
               else
               {

                                index=0;
                                epwm_info->EPwm_CMPB_Direction = EPWM_CMP_UP;


                   /*
                   if(epwm_info->EPwmRegHandle->CMPA.bit.CMPA == 180)
                   {
                      epwm_info->EPwm_CMPA_Direction = EPWM_CMP_UP;
                      index=index+1;
                      epwm_info->EPwmRegHandle->CMPA.bit.CMPA=LuT[index];
                   }
                   */
                   /*
                   else
                   {
                       index=index-1;
                      epwm_info->EPwmRegHandle->CMPA.bit.CMPA=LuT[index];
                   }
                   */
               }

               //
               // If we were increasing CMPB, check to see if
               // we reached the max value.  If not, increase CMPB
               // else, change directions and decrease CMPB
               //
    }
   else
   {
      epwm_info->EPwmTimerIntCount++;
   }

   return;
}

}


