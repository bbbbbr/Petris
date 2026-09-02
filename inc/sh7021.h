#pragma once

#include <stdint.h>

/*
SH7021 peripheral module registers.
Addresses are defined as linker symbols in address_loopy.ld.
*/

/*
Short names (without module prefix) are enabled by the following defines.
You can enable each group with "#define SH7021_<module>_SHORT_NAMES",
or enable all of them with "#define SH7021_SHORT_NAMES".
NOTE: both ITU and DMAC modules have "TCR" registers,
so these registers are excluded from the short names.
You may define them yourself, e.g. "#define TCR0 ITU_TCR0".
*/

#ifdef SH7021_SHORT_NAMES
#define SH7021_SCI_SHORT_NAMES
#define SH7021_ITU_SHORT_NAMES
#define SH7021_DMAC_SHORT_NAMES
#define SH7021_INTC_SHORT_NAMES
#define SH7021_UBC_SHORT_NAMES
#define SH7021_BSC_SHORT_NAMES
#define SH7021_WDT_SHORT_NAMES
#define SH7021_PWR_SHORT_NAMES
#define SH7021_PFC_SHORT_NAMES
#define SH7021_TPC_SHORT_NAMES
#endif //SH7021_SHORT_NAMES

// Serial Communication Interface (SCI)
extern volatile uint8_t SCI_SMR0;
extern volatile uint8_t SCI_BRR0;
extern volatile uint8_t SCI_SCR0;
extern volatile uint8_t SCI_TDR0;
extern volatile uint8_t SCI_SSR0;
extern volatile uint8_t SCI_RDR0;
extern volatile uint8_t SCI_SMR1;
extern volatile uint8_t SCI_BRR1;
extern volatile uint8_t SCI_SCR1;
extern volatile uint8_t SCI_TDR1;
extern volatile uint8_t SCI_SSR1;
extern volatile uint8_t SCI_RDR1;

#ifdef SH7021_SCI_SHORT_NAMES
#define SMR0 SCI_SMR0
#define BRR0 SCI_BRR0
#define SCR0 SCI_SCR0
#define TDR0 SCI_TDR0
#define SSR0 SCI_SSR0
#define RDR0 SCI_RDR0
#define SMR1 SCI_SMR1
#define BRR1 SCI_BRR1
#define SCR1 SCI_SCR1
#define TDR1 SCI_TDR1
#define SSR1 SCI_SSR1
#define RDR1 SCI_RDR1
#endif //SH7021_SCI_SHORT_NAMES

// Integrated-Timer Pulse Unit (ITU)
extern volatile uint8_t  ITU_TSTR;
extern volatile uint8_t  ITU_TSNC;
extern volatile uint8_t  ITU_TMDR;
extern volatile uint8_t  ITU_TFCR;
extern volatile uint8_t  ITU_TCR0;
extern volatile uint8_t  ITU_TIOR0;
extern volatile uint8_t  ITU_TIER0;
extern volatile uint8_t  ITU_TSR0;
extern volatile uint16_t ITU_TCNT0;
extern volatile uint16_t ITU_GRA0;
extern volatile uint16_t ITU_GRB0;
extern volatile uint8_t  ITU_TCR1;
extern volatile uint8_t  ITU_TIOR1;
extern volatile uint8_t  ITU_TIER1;
extern volatile uint8_t  ITU_TSR1;
extern volatile uint16_t ITU_TCNT1;
extern volatile uint16_t ITU_GRA1;
extern volatile uint16_t ITU_GRB1;
extern volatile uint8_t  ITU_TCR2;
extern volatile uint8_t  ITU_TIOR2;
extern volatile uint8_t  ITU_TIER2;
extern volatile uint8_t  ITU_TSR2;
extern volatile uint16_t ITU_TCNT2;
extern volatile uint16_t ITU_GRA2;
extern volatile uint16_t ITU_GRB2;
extern volatile uint8_t  ITU_TCR3;
extern volatile uint8_t  ITU_TIOR3;
extern volatile uint8_t  ITU_TIER3;
extern volatile uint8_t  ITU_TSR3;
extern volatile uint16_t ITU_TCNT3;
extern volatile uint16_t ITU_GRA3;
extern volatile uint16_t ITU_GRB3;
extern volatile uint16_t ITU_BRA3;
extern volatile uint16_t ITU_BRB3;
extern volatile uint8_t  ITU_TOCR;
extern volatile uint8_t  ITU_TCR4;
extern volatile uint8_t  ITU_TIOR4;
extern volatile uint8_t  ITU_TIER4;
extern volatile uint8_t  ITU_TSR4;
extern volatile uint16_t ITU_TCNT4;
extern volatile uint16_t ITU_GRA4;
extern volatile uint16_t ITU_GRB4;
extern volatile uint16_t ITU_BRA4;
extern volatile uint16_t ITU_BRB4;

#ifdef SH7021_ITU_SHORT_NAMES
#define TSTR  ITU_TSTR
#define TSNC  ITU_TSNC
#define TMDR  ITU_TMDR
#define TFCR  ITU_TFCR
//#define TCR0  ITU_TCR0
#define TIOR0 ITU_TIOR0
#define TIER0 ITU_TIER0
#define TSR0  ITU_TSR0
#define TCNT0 ITU_TCNT0
#define GRA0  ITU_GRA0
#define GRB0  ITU_GRB0
//#define TCR1  ITU_TCR1
#define TIOR1 ITU_TIOR1
#define TIER1 ITU_TIER1
#define TSR1  ITU_TSR1
#define TCNT1 ITU_TCNT1
#define GRA1  ITU_GRA1
#define GRB1  ITU_GRB1
//#define TCR2  ITU_TCR2
#define TIOR2 ITU_TIOR2
#define TIER2 ITU_TIER2
#define TSR2  ITU_TSR2
#define TCNT2 ITU_TCNT2
#define GRA2  ITU_GRA2
#define GRB2  ITU_GRB2
//#define TCR3  ITU_TCR3
#define TIOR3 ITU_TIOR3
#define TIER3 ITU_TIER3
#define TSR3  ITU_TSR3
#define TCNT3 ITU_TCNT3
#define GRA3  ITU_GRA3
#define GRB3  ITU_GRB3
#define BRA3  ITU_BRA3
#define BRB3  ITU_BRB3
#define TOCR  ITU_TOCR
//#define TCR4  ITU_TCR4
#define TIOR4 ITU_TIOR4
#define TIER4 ITU_TIER4
#define TSR4  ITU_TSR4
#define TCNT4 ITU_TCNT4
#define GRA4  ITU_GRA4
#define GRB4  ITU_GRB4
#define BRA4  ITU_BRA4
#define BRB4  ITU_BRB4
#endif //SH7021_ITU_SHORT_NAMES

// DMA Controller (DMAC)
extern volatile uint32_t DMAC_SAR0;
extern volatile uint32_t DMAC_DAR0;
extern volatile uint16_t DMAC_DMAOR;
extern volatile uint16_t DMAC_TCR0;
extern volatile uint16_t DMAC_CHCR0;
extern volatile uint32_t DMAC_SAR1;
extern volatile uint32_t DMAC_DAR1;
extern volatile uint16_t DMAC_TCR1;
extern volatile uint16_t DMAC_CHCR1;
extern volatile uint32_t DMAC_SAR2;
extern volatile uint32_t DMAC_DAR2;
extern volatile uint16_t DMAC_TCR2;
extern volatile uint16_t DMAC_CHCR2;
extern volatile uint32_t DMAC_SAR3;
extern volatile uint32_t DMAC_DAR3;
extern volatile uint16_t DMAC_TCR3;
extern volatile uint16_t DMAC_CHCR3;

#ifdef SH7021_DMAC_SHORT_NAMES
#define SAR0  DMAC_SAR0
#define DAR0  DMAC_DAR0
#define DMAOR DMAC_DMAOR
//#define TCR0  DMAC_TCR0
#define CHCR0 DMAC_CHCR0
#define SAR1  DMAC_SAR1
#define DAR1  DMAC_DAR1
//#define TCR1  DMAC_TCR1
#define CHCR1 DMAC_CHCR1
#define SAR2  DMAC_SAR2
#define DAR2  DMAC_DAR2
//#define TCR2  DMAC_TCR2
#define CHCR2 DMAC_CHCR2
#define SAR3  DMAC_SAR3
#define DAR3  DMAC_DAR3
//#define TCR3  DMAC_TCR3
#define CHCR3 DMAC_CHCR3
#endif //SH7021_DMAC_SHORT_NAMES

// Interrupt Controller (INTC)
extern volatile uint16_t INTC_IPRA;
extern volatile uint16_t INTC_IPRB;
extern volatile uint16_t INTC_IPRC;
extern volatile uint16_t INTC_IPRD;
extern volatile uint16_t INTC_IPRE;
extern volatile uint16_t INTC_ICR;

#ifdef SH7021_INTC_SHORT_NAMES
#define IPRA INTC_IPRA
#define IPRB INTC_IPRB
#define IPRC INTC_IPRC
#define IPRD INTC_IPRD
#define IPRE INTC_IPRE
#define ICR  INTC_ICR
#endif //SH7021_INTC_SHORT_NAMES

// User Break Controller (UBC)
extern volatile uint16_t UBC_BARH;
extern volatile uint16_t UBC_BARL;
extern volatile uint16_t UBC_BAMRH;
extern volatile uint16_t UBC_BAMRL;
extern volatile uint16_t UBC_BBR;

#ifdef SH7021_UBC_SHORT_NAMES
#define BARH  UBC_BARH
#define BARL  UBC_BARL
#define BAMRH UBC_BAMRH
#define BAMRL UBC_BAMRL
#define BBR   UBC_BBR
#endif //SH7021_UBC_SHORT_NAMES

// Bus State Controller (BSC)
extern volatile uint16_t BSC_BCR;
extern volatile uint16_t BSC_WCR1;
extern volatile uint16_t BSC_WCR2;
extern volatile uint16_t BSC_WCR3;
extern volatile uint16_t BSC_DCR;
extern volatile uint16_t BSC_PCR;
extern volatile uint16_t BSC_RCR;
extern volatile uint16_t BSC_RTCSR;
extern volatile uint16_t BSC_RTCNT;
extern volatile uint16_t BSC_RTCOR;

#ifdef SH7021_BSC_SHORT_NAMES
#define BCR   BSC_BCR
#define WCR1  BSC_WCR1
#define WCR2  BSC_WCR2
#define WCR3  BSC_WCR3
#define DCR   BSC_DCR
#define PCR   BSC_PCR
#define RCR   BSC_RCR
#define RTCSR BSC_RTCSR
#define RTCNT BSC_RTCNT
#define RTCOR BSC_RTCOR
#endif //SH7021_BSC_SHORT_NAMES

// Watchdog Timer (WDT)
// These are a *mess*, read the datasheet...
extern volatile uint16_t WDT_TCSR_TCNT_W;
extern volatile uint8_t  WDT_TCSR_R;
extern volatile uint8_t  WDT_TCNT_R;
extern volatile uint16_t WDT_RSTCSR_W;
extern volatile uint8_t  WDT_RSTCSR_R;

#ifdef SH7021_WDT_SHORT_NAMES
#define TCSR_TCNT_W WDT_TCSR_TCNT_W
#define TCSR_R      WDT_TCSR_R
#define TCNT_R      WDT_TCNT_R
#define RSTCSR_W    WDT_RSTCSR_W
#define RSTCSR_R    WDT_RSTCSR_R
#endif //SH7021_WDT_SHORT_NAMES

// Power-down state
extern volatile uint8_t PWR_SBYCR;

#ifdef SH7021_PWR_SHORT_NAMES
#define SBYCR PWR_SBYCR
#endif //SH7021_PWR_SHORT_NAMES

// Pin Function Controller (PFC) and parallel I/O Ports
extern volatile uint16_t PFC_PADR;
extern volatile uint16_t PFC_PBDR;
extern volatile uint16_t PFC_PAIOR;
extern volatile uint16_t PFC_PBIOR;
extern volatile uint16_t PFC_PACR1;
extern volatile uint16_t PFC_PACR2;
extern volatile uint16_t PFC_PBCR1;
extern volatile uint16_t PFC_PBCR2;
extern volatile uint16_t PFC_CASCR;

#ifdef SH7021_PFC_SHORT_NAMES
#define PADR  PFC_PADR
#define PBDR  PFC_PBDR
#define PAIOR PFC_PAIOR
#define PBIOR PFC_PBIOR
#define PACR1 PFC_PACR1
#define PACR2 PFC_PACR2
#define PBCR1 PFC_PBCR1
#define PBCR2 PFC_PBCR2
#define CASCR PFC_CASCR
#endif //SH7021_PFC_SHORT_NAMES

// Programmable Timing Pattern Controller (TPC)
// Module also uses PBDR, PBCR1, PBCR2
extern volatile uint8_t TPC_TPMR;
extern volatile uint8_t TPC_TPCR;
extern volatile uint8_t TPC_NDERB;
extern volatile uint8_t TPC_NDERA;
extern volatile uint8_t TPC_NDRB1;
extern volatile uint8_t TPC_NDRA1;
extern volatile uint8_t TPC_NDRB0;
extern volatile uint8_t TPC_NDRA0;
#define TPC_NDRB TPC_NDRB1
#define TPC_NDRA TPC_NDRA1

#ifdef SH7021_TPC_SHORT_NAMES
#define TPMR  TPC_TPMR
#define TPCR  TPC_TPCR
#define NDERB TPC_NDERB
#define NDERA TPC_NDERA
#define NDRB1 TPC_NDRB1
#define NDRA1 TPC_NDRA1
#define NDRB0 TPC_NDRB0
#define NDRA0 TPC_NDRA0
#define NDRB  TPC_NDRB
#define NDRA  TPC_NDRA
#endif //SH7021_TPC_SHORT_NAMES
