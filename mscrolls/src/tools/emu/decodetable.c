/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
 *                                                       
 * Copyright (c) 2016-2018 Voidware Ltd.  All Rights Reserved.
 *
 * This file contains Original Code and/or Modifications of Original Code as
 * defined in and that are subject to the Voidware Public Source Licence version
 * 1.0 (the 'Licence'). You may not use this file except in compliance with the
 * Licence or with expressly written permission from Voidware.  Please obtain a
 * copy of the Licence at https://strandgames.com/legal/strandPSL1.txt and read
 * it before using this file.
 *
 * THIS IS NOT FREE SOFTWARE.
 * 
 * The Original Code and all software distributed under the Licence are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS
 * OR IMPLIED, AND VOIDWARE HEREBY DISCLAIMS ALL SUCH WARRANTIES, INCLUDING
 * WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 *
 * Please see the Licence for the specific language governing rights and 
 * limitations under the Licence.
 *
 * contact@strandgames.com
 *
 * This software was originally written and owned By Magnetic Scrolls Ltd.
 * It is being released under a Public Source Licence for historical reference
 * and educational purposes.
 */

/* Note : these are functions really */

extern int ALINE;
extern int FLINE;
extern int iABCD_AND_BfDn;
extern int iADDA_L;
extern int iADDA_W;
extern int iADDI_B;
extern int iADDI_L;
extern int iADDI_W;
extern int iADDQ_B;
extern int iADDQ_L;
extern int iADDQ_W;
extern int iADDX_B_ADD_BfDn;
extern int iADDX_L_ADD_LfDn;
extern int iADDX_W_ADD_WfDn;
extern int iADD_BtDn;
extern int iADD_LtDn;
extern int iADD_WtDn;
extern int iANDI_B;
extern int iANDI_L;
extern int iANDI_W;
extern int iAND_BtDn;
extern int iAND_LtDn;
extern int iAND_WtDn;
extern int iASL;
extern int iASR;
extern int iBCC;
extern int iBCHGDn;
extern int iBCHGI;
extern int iBCLRDn;
extern int iBCLRI;
extern int iBCS;
extern int iBEQ;
extern int iBGE;
extern int iBGT;
extern int iBHI;
extern int iBLE;
extern int iBLS;
extern int iBLT;
extern int iBMI;
extern int iBNE;
extern int iBPL;
extern int iBRA;
extern int iBSETDn;
extern int iBSETI;
extern int iBSR;
extern int iBTSTDn;
extern int iBTSTI;
extern int iBVC;
extern int iBVS;
extern int iCHK;
extern int iCLR_B;
extern int iCLR_L;
extern int iCLR_W;
extern int iCMPA_L;
extern int iCMPA_W;
extern int iCMPI_B;
extern int iCMPI_L;
extern int iCMPI_W;
extern int iCMP_B;
extern int iCMP_L;
extern int iCMP_W;
extern int iDIVS;
extern int iDIVU;
extern int iEORI_B;
extern int iEORI_L;
extern int iEORI_W;
extern int iEOR_B;
extern int iEOR_L_CMPM_L;
extern int iEOR_W_CMPM_W;
extern int iEXG1_AND_WfDn;
extern int iEXG2_AND_LfDn;
extern int iEXT_L_MMfreg_L;
extern int iEXT_W_MMfreg_W;
extern int iJMP;
extern int iJSR;
extern int iLEA;
extern int iLSL;
extern int iLSR;
extern int iMMfreg_L;
extern int iMMfreg_W;
extern int iMMtreg_L;
extern int iMMtreg_W;
extern int iMOVEQ;
extern int iMOVE_B;
extern int iMOVE_L;
extern int iMOVE_W;
extern int iMOVEfSR;
extern int iMOVEtCCR;
extern int iMOVEtSR;
extern int iMULS;
extern int iMULU;
extern int iNBCD;
extern int iNEGX_B;
extern int iNEGX_L;
extern int iNEGX_W;
extern int iNEG_B;
extern int iNEG_L;
extern int iNEG_W;
extern int iNOT_B;
extern int iNOT_L;
extern int iNOT_W;
extern int iORI_B;
extern int iORI_L;
extern int iORI_W;
extern int iOR_BtDn;
extern int iOR_LfDn;
extern int iOR_LtDn;
extern int iOR_WfDn;
extern int iOR_WtDn;
extern int iROL;
extern int iROR;
extern int iROXL;
extern int iROXR;
extern int iRTS;
extern int iSBCD_OR_BfDn;
extern int iSL_B_ROL_B;
extern int iSL_L_ROL_L;
extern int iSL_W_ROL_W;
extern int iSR_B_ROR_B;
extern int iSR_L_ROR_L;
extern int iSR_W_ROR_W;
extern int iSUBA_L;
extern int iSUBA_W;
extern int iSUBI_B;
extern int iSUBI_L;
extern int iSUBI_W;
extern int iSUBQ_B;
extern int iSUBQ_L;
extern int iSUBQ_W;
extern int iSUB_BfDn;
extern int iSUB_BtDn;
extern int iSUB_LfDn;
extern int iSUB_LtDn;
extern int iSUB_WfDn;
extern int iSUB_WtDn;
extern int iSWAP_PEA;
extern int iS_DBCC;
extern int iS_DBCS;
extern int iS_DBEQ;
extern int iS_DBF;
extern int iS_DBGE;
extern int iS_DBGT;
extern int iS_DBHI;
extern int iS_DBLE;
extern int iS_DBLS;
extern int iS_DBLT;
extern int iS_DBMI;
extern int iS_DBNE;
extern int iS_DBPL;
extern int iS_DBT;
extern int iS_DBVC;
extern int iS_DBVS;
extern int iTAS;
extern int iTST_B;
extern int iTST_L;
extern int iTST_W;
extern int illegal;
extern int imisc4E;

int *decodetable[] = {
	&iORI_B,		/* 0,0,0 */
	&iORI_W,		/* 0,0,1 */
	&iORI_L,		/* 0,0,2 */
	&illegal,		/* 0,0,3 */
	&iBTSTDn,		/* 0,0,4 */
	&iBCHGDn,		/* 0,0,5 */
	&iBCLRDn,		/* 0,0,6 */
	&iBSETDn,		/* 0,0,7 */
	&iANDI_B,		/* 0,1,0 */
	&iANDI_W,		/* 0,1,1 */
	&iANDI_L,		/* 0,1,2 */
	&illegal,		/* 0,1,3 */
	&iBTSTDn,		/* 0,1,4 */
	&iBCHGDn,		/* 0,1,5 */
	&iBCLRDn,		/* 0,1,6 */
	&iBSETDn,		/* 0,1,7 */
	&iSUBI_B,		/* 0,2,0 */
	&iSUBI_W,		/* 0,2,1 */
	&iSUBI_L,		/* 0,2,2 */
	&illegal,		/* 0,2,3 */
	&iBTSTDn,		/* 0,2,4 */
	&iBCHGDn,		/* 0,2,5 */
	&iBCLRDn,		/* 0,2,6 */
	&iBSETDn,		/* 0,2,7 */
	&iADDI_B,		/* 0,3,0 */
	&iADDI_W,		/* 0,3,1 */
	&iADDI_L,		/* 0,3,2 */
	&illegal,		/* 0,3,3 */
	&iBTSTDn,		/* 0,3,4 */
	&iBCHGDn,		/* 0,3,5 */
	&iBCLRDn,		/* 0,3,6 */
	&iBSETDn,		/* 0,3,7 */
	&iBTSTI,		/* 0,4,0 */
	&iBCHGI,		/* 0,4,1 */
	&iBCLRI,		/* 0,4,2 */
	&iBSETI,		/* 0,4,3 */
	&iBTSTDn,		/* 0,4,4 */
	&iBCHGDn,		/* 0,4,5 */
	&iBCLRDn,		/* 0,4,6 */
	&iBSETDn,		/* 0,4,7 */
	&iEORI_B,		/* 0,5,0 */
	&iEORI_W,		/* 0,5,1 */
	&iEORI_L,		/* 0,5,2 */
	&illegal,		/* 0,5,3 */
	&iBTSTDn,		/* 0,5,4 */
	&iBCHGDn,		/* 0,5,5 */
	&iBCLRDn,		/* 0,5,6 */
	&iBSETDn,		/* 0,5,7 */
	&iCMPI_B,		/* 0,6,0 */
	&iCMPI_W,		/* 0,6,1 */
	&iCMPI_L,		/* 0,6,2 */
	&illegal,		/* 0,6,3 */
	&iBTSTDn,		/* 0,6,4 */
	&iBCHGDn,		/* 0,6,5 */
	&iBCLRDn,		/* 0,6,6 */
	&iBSETDn,		/* 0,6,7 */
	&illegal,		/* 0,7,0 */
	&illegal,		/* 0,7,1 */
	&illegal,		/* 0,7,2 */
	&illegal,		/* 0,7,3 */
	&iBTSTDn,		/* 0,7,4 */
	&iBCHGDn,		/* 0,7,5 */
	&iBCLRDn,		/* 0,7,6 */
	&iBSETDn,		/* 0,7,7 */
	&iMOVE_B,		/* 1,0,0 */
	&iMOVE_B,		/* 1,0,1 */
	&iMOVE_B,		/* 1,0,2 */
	&iMOVE_B,		/* 1,0,3 */
	&iMOVE_B,		/* 1,0,4 */
	&iMOVE_B,		/* 1,0,5 */
	&iMOVE_B,		/* 1,0,6 */
	&iMOVE_B,		/* 1,0,7 */
	&iMOVE_B,		/* 1,1,0 */
	&iMOVE_B,		/* 1,1,1 */
	&iMOVE_B,		/* 1,1,2 */
	&iMOVE_B,		/* 1,1,3 */
	&iMOVE_B,		/* 1,1,4 */
	&iMOVE_B,		/* 1,1,5 */
	&iMOVE_B,		/* 1,1,6 */
	&iMOVE_B,		/* 1,1,7 */
	&iMOVE_B,		/* 1,2,0 */
	&iMOVE_B,		/* 1,2,1 */
	&iMOVE_B,		/* 1,2,2 */
	&iMOVE_B,		/* 1,2,3 */
	&iMOVE_B,		/* 1,2,4 */
	&iMOVE_B,		/* 1,2,5 */
	&iMOVE_B,		/* 1,2,6 */
	&iMOVE_B,		/* 1,2,7 */
	&iMOVE_B,		/* 1,3,0 */
	&iMOVE_B,		/* 1,3,1 */
	&iMOVE_B,		/* 1,3,2 */
	&iMOVE_B,		/* 1,3,3 */
	&iMOVE_B,		/* 1,3,4 */
	&iMOVE_B,		/* 1,3,5 */
	&iMOVE_B,		/* 1,3,6 */
	&iMOVE_B,		/* 1,3,7 */
	&iMOVE_B,		/* 1,4,0 */
	&iMOVE_B,		/* 1,4,1 */
	&iMOVE_B,		/* 1,4,2 */
	&iMOVE_B,		/* 1,4,3 */
	&iMOVE_B,		/* 1,4,4 */
	&iMOVE_B,		/* 1,4,5 */
	&iMOVE_B,		/* 1,4,6 */
	&iMOVE_B,		/* 1,4,7 */
	&iMOVE_B,		/* 1,5,0 */
	&iMOVE_B,		/* 1,5,1 */
	&iMOVE_B,		/* 1,5,2 */
	&iMOVE_B,		/* 1,5,3 */
	&iMOVE_B,		/* 1,5,4 */
	&iMOVE_B,		/* 1,5,5 */
	&iMOVE_B,		/* 1,5,6 */
	&iMOVE_B,		/* 1,5,7 */
	&iMOVE_B,		/* 1,6,0 */
	&iMOVE_B,		/* 1,6,1 */
	&iMOVE_B,		/* 1,6,2 */
	&iMOVE_B,		/* 1,6,3 */
	&iMOVE_B,		/* 1,6,4 */
	&iMOVE_B,		/* 1,6,5 */
	&iMOVE_B,		/* 1,6,6 */
	&iMOVE_B,		/* 1,6,7 */
	&iMOVE_B,		/* 1,7,0 */
	&iMOVE_B,		/* 1,7,1 */
	&iMOVE_B,		/* 1,7,2 */
	&iMOVE_B,		/* 1,7,3 */
	&iMOVE_B,		/* 1,7,4 */
	&iMOVE_B,		/* 1,7,5 */
	&iMOVE_B,		/* 1,7,6 */
	&iMOVE_B,		/* 1,7,7 */
	&iMOVE_L,		/* 2,0,0 */
	&iMOVE_L,		/* 2,0,1 */
	&iMOVE_L,		/* 2,0,2 */
	&iMOVE_L,		/* 2,0,3 */
	&iMOVE_L,		/* 2,0,4 */
	&iMOVE_L,		/* 2,0,5 */
	&iMOVE_L,		/* 2,0,6 */
	&iMOVE_L,		/* 2,0,7 */
	&iMOVE_L,		/* 2,1,0 */
	&iMOVE_L,		/* 2,1,1 */
	&iMOVE_L,		/* 2,1,2 */
	&iMOVE_L,		/* 2,1,3 */
	&iMOVE_L,		/* 2,1,4 */
	&iMOVE_L,		/* 2,1,5 */
	&iMOVE_L,		/* 2,1,6 */
	&iMOVE_L,		/* 2,1,7 */
	&iMOVE_L,		/* 2,2,0 */
	&iMOVE_L,		/* 2,2,1 */
	&iMOVE_L,		/* 2,2,2 */
	&iMOVE_L,		/* 2,2,3 */
	&iMOVE_L,		/* 2,2,4 */
	&iMOVE_L,		/* 2,2,5 */
	&iMOVE_L,		/* 2,2,6 */
	&iMOVE_L,		/* 2,2,7 */
	&iMOVE_L,		/* 2,3,0 */
	&iMOVE_L,		/* 2,3,1 */
	&iMOVE_L,		/* 2,3,2 */
	&iMOVE_L,		/* 2,3,3 */
	&iMOVE_L,		/* 2,3,4 */
	&iMOVE_L,		/* 2,3,5 */
	&iMOVE_L,		/* 2,3,6 */
	&iMOVE_L,		/* 2,3,7 */
	&iMOVE_L,		/* 2,4,0 */
	&iMOVE_L,		/* 2,4,1 */
	&iMOVE_L,		/* 2,4,2 */
	&iMOVE_L,		/* 2,4,3 */
	&iMOVE_L,		/* 2,4,4 */
	&iMOVE_L,		/* 2,4,5 */
	&iMOVE_L,		/* 2,4,6 */
	&iMOVE_L,		/* 2,4,7 */
	&iMOVE_L,		/* 2,5,0 */
	&iMOVE_L,		/* 2,5,1 */
	&iMOVE_L,		/* 2,5,2 */
	&iMOVE_L,		/* 2,5,3 */
	&iMOVE_L,		/* 2,5,4 */
	&iMOVE_L,		/* 2,5,5 */
	&iMOVE_L,		/* 2,5,6 */
	&iMOVE_L,		/* 2,5,7 */
	&iMOVE_L,		/* 2,6,0 */
	&iMOVE_L,		/* 2,6,1 */
	&iMOVE_L,		/* 2,6,2 */
	&iMOVE_L,		/* 2,6,3 */
	&iMOVE_L,		/* 2,6,4 */
	&iMOVE_L,		/* 2,6,5 */
	&iMOVE_L,		/* 2,6,6 */
	&iMOVE_L,		/* 2,6,7 */
	&iMOVE_L,		/* 2,7,0 */
	&iMOVE_L,		/* 2,7,1 */
	&iMOVE_L,		/* 2,7,2 */
	&iMOVE_L,		/* 2,7,3 */
	&iMOVE_L,		/* 2,7,4 */
	&iMOVE_L,		/* 2,7,5 */
	&iMOVE_L,		/* 2,7,6 */
	&iMOVE_L,		/* 2,7,7 */
	&iMOVE_W,		/* 3,0,0 */
	&iMOVE_W,		/* 3,0,1 */
	&iMOVE_W,		/* 3,0,2 */
	&iMOVE_W,		/* 3,0,3 */
	&iMOVE_W,		/* 3,0,4 */
	&iMOVE_W,		/* 3,0,5 */
	&iMOVE_W,		/* 3,0,6 */
	&iMOVE_W,		/* 3,0,7 */
	&iMOVE_W,		/* 3,1,0 */
	&iMOVE_W,		/* 3,1,1 */
	&iMOVE_W,		/* 3,1,2 */
	&iMOVE_W,		/* 3,1,3 */
	&iMOVE_W,		/* 3,1,4 */
	&iMOVE_W,		/* 3,1,5 */
	&iMOVE_W,		/* 3,1,6 */
	&iMOVE_W,		/* 3,1,7 */
	&iMOVE_W,		/* 3,2,0 */
	&iMOVE_W,		/* 3,2,1 */
	&iMOVE_W,		/* 3,2,2 */
	&iMOVE_W,		/* 3,2,3 */
	&iMOVE_W,		/* 3,2,4 */
	&iMOVE_W,		/* 3,2,5 */
	&iMOVE_W,		/* 3,2,6 */
	&iMOVE_W,		/* 3,2,7 */
	&iMOVE_W,		/* 3,3,0 */
	&iMOVE_W,		/* 3,3,1 */
	&iMOVE_W,		/* 3,3,2 */
	&iMOVE_W,		/* 3,3,3 */
	&iMOVE_W,		/* 3,3,4 */
	&iMOVE_W,		/* 3,3,5 */
	&iMOVE_W,		/* 3,3,6 */
	&iMOVE_W,		/* 3,3,7 */
	&iMOVE_W,		/* 3,4,0 */
	&iMOVE_W,		/* 3,4,1 */
	&iMOVE_W,		/* 3,4,2 */
	&iMOVE_W,		/* 3,4,3 */
	&iMOVE_W,		/* 3,4,4 */
	&iMOVE_W,		/* 3,4,5 */
	&iMOVE_W,		/* 3,4,6 */
	&iMOVE_W,		/* 3,4,7 */
	&iMOVE_W,		/* 3,5,0 */
	&iMOVE_W,		/* 3,5,1 */
	&iMOVE_W,		/* 3,5,2 */
	&iMOVE_W,		/* 3,5,3 */
	&iMOVE_W,		/* 3,5,4 */
	&iMOVE_W,		/* 3,5,5 */
	&iMOVE_W,		/* 3,5,6 */
	&iMOVE_W,		/* 3,5,7 */
	&iMOVE_W,		/* 3,6,0 */
	&iMOVE_W,		/* 3,6,1 */
	&iMOVE_W,		/* 3,6,2 */
	&iMOVE_W,		/* 3,6,3 */
	&iMOVE_W,		/* 3,6,4 */
	&iMOVE_W,		/* 3,6,5 */
	&iMOVE_W,		/* 3,6,6 */
	&iMOVE_W,		/* 3,6,7 */
	&iMOVE_W,		/* 3,7,0 */
	&iMOVE_W,		/* 3,7,1 */
	&iMOVE_W,		/* 3,7,2 */
	&iMOVE_W,		/* 3,7,3 */
	&iMOVE_W,		/* 3,7,4 */
	&iMOVE_W,		/* 3,7,5 */
	&iMOVE_W,		/* 3,7,6 */
	&iMOVE_W,		/* 3,7,7 */
	&iNEGX_B,		/* 4,0,0 */
	&iNEGX_W,		/* 4,0,1 */
	&iNEGX_L,		/* 4,0,2 */
	&iMOVEfSR,		/* 4,0,3 */
	&illegal,		/* 4,0,4 */
	&illegal,		/* 4,0,5 */
	&iCHK,		/* 4,0,6 */
	&iLEA,		/* 4,0,7 */
	&iCLR_B,		/* 4,1,0 */
	&iCLR_W,		/* 4,1,1 */
	&iCLR_L,		/* 4,1,2 */
	&illegal,		/* 4,1,3 */
	&illegal,		/* 4,1,4 */
	&illegal,		/* 4,1,5 */
	&iCHK,		/* 4,1,6 */
	&iLEA,		/* 4,1,7 */
	&iNEG_B,	/* 4,2,0 */
	&iNEG_W,	/* 4,2,1 */
	&iNEG_L,	/* 4,2,2 */
	&iMOVEtCCR,	/* 4,2,3 */
	&illegal,		/* 4,2,4 */
	&illegal,		/* 4,2,5 */
	&iCHK,		/* 4,2,6 */
	&iLEA,		/* 4,2,7 */
	&iNOT_B,	/* 4,3,0 */
	&iNOT_W,	/* 4,3,1 */
	&iNOT_L,	/* 4,3,2 */
	&iMOVEtSR,	/* 4,3,3 */
	&illegal,		/* 4,3,4 */
	&illegal,		/* 4,3,5 */
	&iCHK,		/* 4,3,6 */
	&iLEA,		/* 4,3,7 */
	&iNBCD,		/* 4,4,0 */
	&iSWAP_PEA,	/* 4,4,1 */
	&iEXT_W_MMfreg_W, /* 4,4,2 */
	&iEXT_L_MMfreg_L, /* 4,4,3 */
	&illegal,		/* 4,4,4 */
	&illegal,		/* 4,4,5 */
	&iCHK,		/* 4,4,6 */
	&iLEA,		/* 4,4,7 */
	&iTST_B,	/* 4,5,0 */
	&iTST_W,	/* 4,5,1 */
	&iTST_L,	/* 4,5,2 */
	&iTAS,		/* 4,5,3 */
	&illegal,		/* 4,5,4 */
	&illegal,		/* 4,5,5 */
	&iCHK,		/* 4,5,6 */
	&iLEA,		/* 4,5,7 */
	&illegal,		/* 4,6,0 */
	&illegal,		/* 4,6,1 */
	&iMMtreg_W,	/* 4,6,2 */
	&iMMtreg_L,	/* 4,6,3 */
	&illegal,		/* 4,6,4 */
	&illegal,		/* 4,6,5 */
	&iCHK,		/* 4,6,6 */
	&iLEA,		/* 4,6,7 */
	&illegal,		/* 4,7,0 */
	&imisc4E,	/* 4,7,1 */
	&iJSR,		/* 4,7,2 */
	&iJMP,		/* 4,7,3 */
	&illegal,		/* 4,7,4 */
	&illegal,		/* 4,7,5 */
	&iCHK,		/* 4,7,6 */
	&iLEA,		/* 4,7,7 */
	&iADDQ_B,	/* 5,0,0 */
	&iADDQ_W,	/* 5,0,1 */
	&iADDQ_L,	/* 5,0,2 */
	&iS_DBT,	/* 5,0,3 */
	&iSUBQ_B,	/* 5,0,4 */
	&iSUBQ_W,	/* 5,0,5 */
	&iSUBQ_L,	/* 5,0,6 */
	&iS_DBF,	/* 5,0,7 */
	&iADDQ_B,	/* 5,1,0 */
	&iADDQ_W,	/* 5,1,1 */
	&iADDQ_L,	/* 5,1,2 */
	&iS_DBHI,	/* 5,1,3 */
	&iSUBQ_B,	/* 5,1,4 */
	&iSUBQ_W,	/* 5,1,5 */
	&iSUBQ_L,	/* 5,1,6 */
	&iS_DBLS,	/* 5,1,7 */
	&iADDQ_B,	/* 5,2,0 */
	&iADDQ_W,	/* 5,2,1 */
	&iADDQ_L,	/* 5,2,2 */
	&iS_DBCC,	/* 5,2,3 */
	&iSUBQ_B,	/* 5,2,4 */
	&iSUBQ_W,	/* 5,2,5 */
	&iSUBQ_L,	/* 5,2,6 */
	&iS_DBCS,	/* 5,2,7 */
	&iADDQ_B,	/* 5,3,0 */
	&iADDQ_W,	/* 5,3,1 */
	&iADDQ_L,	/* 5,3,2 */
	&iS_DBNE,	/* 5,3,3 */
	&iSUBQ_B,	/* 5,3,4 */
	&iSUBQ_W,	/* 5,3,5 */
	&iSUBQ_L,	/* 5,3,6 */
	&iS_DBEQ,	/* 5,3,7 */
	&iADDQ_B,	/* 5,4,0 */
	&iADDQ_W,	/* 5,4,1 */
	&iADDQ_L,	/* 5,4,2 */
	&iS_DBVC,	/* 5,4,3 */
	&iSUBQ_B,	/* 5,4,4 */
	&iSUBQ_W,	/* 5,4,5 */
	&iSUBQ_L,	/* 5,4,6 */
	&iS_DBVS,	/* 5,4,7 */
	&iADDQ_B,	/* 5,5,0 */
	&iADDQ_W,	/* 5,5,1 */
	&iADDQ_L,	/* 5,5,2 */
	&iS_DBPL,	/* 5,5,3 */
	&iSUBQ_B,	/* 5,5,4 */
	&iSUBQ_W,	/* 5,5,5 */
	&iSUBQ_L,	/* 5,5,6 */
	&iS_DBMI,	/* 5,5,7 */
	&iADDQ_B,	/* 5,6,0 */
	&iADDQ_W,	/* 5,6,1 */
	&iADDQ_L,	/* 5,6,2 */
	&iS_DBGE,	/* 5,6,3 */
	&iSUBQ_B,	/* 5,6,4 */
	&iSUBQ_W,	/* 5,6,5 */
	&iSUBQ_L,	/* 5,6,6 */
	&iS_DBLT,	/* 5,6,7 */
	&iADDQ_B,	/* 5,7,0 */
	&iADDQ_W,	/* 5,7,1 */
	&iADDQ_L,	/* 5,7,2 */
	&iS_DBGT,	/* 5,7,3 */
	&iSUBQ_B,	/* 5,7,4 */
	&iSUBQ_W,	/* 5,7,5 */
	&iSUBQ_L,	/* 5,7,6 */
	&iS_DBLE,	/* 5,7,7 */
	&iBRA,		/* 6,0,0 */
	&iBRA,		/* 6,0,1 */
	&iBRA,		/* 6,0,2 */
	&iBRA,		/* 6,0,3 */
	&iBSR,		/* 6,0,4 */
	&iBSR,		/* 6,0,5 */
	&iBSR,		/* 6,0,6 */
	&iBSR,		/* 6,0,7 */
	&iBHI,		/* 6,1,0 */
	&iBHI,		/* 6,1,1 */
	&iBHI,		/* 6,1,2 */
	&iBHI,		/* 6,1,3 */
	&iBLS,		/* 6,1,4 */
	&iBLS,		/* 6,1,5 */
	&iBLS,		/* 6,1,6 */
	&iBLS,		/* 6,1,7 */
	&iBCC,		/* 6,2,0 */
	&iBCC,		/* 6,2,1 */
	&iBCC,		/* 6,2,2 */
	&iBCC,		/* 6,2,3 */
	&iBCS,		/* 6,2,4 */
	&iBCS,		/* 6,2,5 */
	&iBCS,		/* 6,2,6 */
	&iBCS,		/* 6,2,7 */
	&iBNE,		/* 6,3,0 */
	&iBNE,		/* 6,3,1 */
	&iBNE,		/* 6,3,2 */
	&iBNE,		/* 6,3,3 */
	&iBEQ,		/* 6,3,4 */
	&iBEQ,		/* 6,3,5 */
	&iBEQ,		/* 6,3,6 */
	&iBEQ,		/* 6,3,7 */
	&iBVC,		/* 6,4,0 */
	&iBVC,		/* 6,4,1 */
	&iBVC,		/* 6,4,2 */
	&iBVC,		/* 6,4,3 */
	&iBVS,		/* 6,4,4 */
	&iBVS,		/* 6,4,5 */
	&iBVS,		/* 6,4,6 */
	&iBVS,		/* 6,4,7 */
	&iBPL,		/* 6,5,0 */
	&iBPL,		/* 6,5,1 */
	&iBPL,		/* 6,5,2 */
	&iBPL,		/* 6,5,3 */
	&iBMI,		/* 6,5,4 */
	&iBMI,		/* 6,5,5 */
	&iBMI,		/* 6,5,6 */
	&iBMI,		/* 6,5,7 */
	&iBGE,		/* 6,6,0 */
	&iBGE,		/* 6,6,1 */
	&iBGE,		/* 6,6,2 */
	&iBGE,		/* 6,6,3 */
	&iBLT,		/* 6,6,4 */
	&iBLT,		/* 6,6,5 */
	&iBLT,		/* 6,6,6 */
	&iBLT,		/* 6,6,7 */
	&iBGT,		/* 6,7,0 */
	&iBGT,		/* 6,7,1 */
	&iBGT,		/* 6,7,2 */
	&iBGT,		/* 6,7,3 */
	&iBLE,		/* 6,7,4 */
	&iBLE,		/* 6,7,5 */
	&iBLE,		/* 6,7,6 */
	&iBLE,		/* 6,7,7 */
	&iMOVEQ,	/* 7,0,0 */
	&iMOVEQ,	/* 7,0,1 */
	&iMOVEQ,	/* 7,0,2 */
	&iMOVEQ,	/* 7,0,3 */
	&iMOVEQ,	/* 7,0,4 */
	&iMOVEQ,	/* 7,0,5 */
	&iMOVEQ,	/* 7,0,6 */
	&iMOVEQ,	/* 7,0,7 */
	&iMOVEQ,	/* 7,1,0 */
	&iMOVEQ,	/* 7,1,1 */
	&iMOVEQ,	/* 7,1,2 */
	&iMOVEQ,	/* 7,1,3 */
	&iMOVEQ,	/* 7,1,4 */
	&iMOVEQ,	/* 7,1,5 */
	&iMOVEQ,	/* 7,1,6 */
	&iMOVEQ,	/* 7,1,7 */
	&iMOVEQ,	/* 7,2,0 */
	&iMOVEQ,	/* 7,2,1 */
	&iMOVEQ,	/* 7,2,2 */
	&iMOVEQ,	/* 7,2,3 */
	&iMOVEQ,	/* 7,2,4 */
	&iMOVEQ,	/* 7,2,5 */
	&iMOVEQ,	/* 7,2,6 */
	&iMOVEQ,	/* 7,2,7 */
	&iMOVEQ,	/* 7,3,0 */
	&iMOVEQ,	/* 7,3,1 */
	&iMOVEQ,	/* 7,3,2 */
	&iMOVEQ,	/* 7,3,3 */
	&iMOVEQ,	/* 7,3,4 */
	&iMOVEQ,	/* 7,3,5 */
	&iMOVEQ,	/* 7,3,6 */
	&iMOVEQ,	/* 7,3,7 */
	&iMOVEQ,	/* 7,4,0 */
	&iMOVEQ,	/* 7,4,1 */
	&iMOVEQ,	/* 7,4,2 */
	&iMOVEQ,	/* 7,4,3 */
	&iMOVEQ,	/* 7,4,4 */
	&iMOVEQ,	/* 7,4,5 */
	&iMOVEQ,	/* 7,4,6 */
	&iMOVEQ,	/* 7,4,7 */
	&iMOVEQ,	/* 7,5,0 */
	&iMOVEQ,	/* 7,5,1 */
	&iMOVEQ,	/* 7,5,2 */
	&iMOVEQ,	/* 7,5,3 */
	&iMOVEQ,	/* 7,5,4 */
	&iMOVEQ,	/* 7,5,5 */
	&iMOVEQ,	/* 7,5,6 */
	&iMOVEQ,	/* 7,5,7 */
	&iMOVEQ,	/* 7,6,0 */
	&iMOVEQ,	/* 7,6,1 */
	&iMOVEQ,	/* 7,6,2 */
	&iMOVEQ,	/* 7,6,3 */
	&iMOVEQ,	/* 7,6,4 */
	&iMOVEQ,	/* 7,6,5 */
	&iMOVEQ,	/* 7,6,6 */
	&iMOVEQ,	/* 7,6,7 */
	&iMOVEQ,	/* 7,7,0 */
	&iMOVEQ,	/* 7,7,1 */
	&iMOVEQ,	/* 7,7,2 */
	&iMOVEQ,	/* 7,7,3 */
	&iMOVEQ,	/* 7,7,4 */
	&iMOVEQ,	/* 7,7,5 */
	&iMOVEQ,	/* 7,7,6 */
	&iMOVEQ,	/* 7,7,7 */
	&iOR_BtDn,	/* 8,0,0 */
	&iOR_WtDn,	/* 8,0,1 */
	&iOR_LtDn,	/* 8,0,2 */
	&iDIVU,		/* 8,0,3 */
	&iSBCD_OR_BfDn,	/* 8,0,4 */
	&iOR_WfDn,	/* 8,0,5 */
	&iOR_LfDn,	/* 8,0,6 */
	&iDIVS,		/* 8,0,7 */
	&iOR_BtDn,	/* 8,1,0 */
	&iOR_WtDn,	/* 8,1,1 */
	&iOR_LtDn,	/* 8,1,2 */
	&iDIVU,		/* 8,1,3 */
	&iSBCD_OR_BfDn,	/* 8,1,4 */
	&iOR_WfDn,	/* 8,1,5 */
	&iOR_LfDn,	/* 8,1,6 */
	&iDIVS,		/* 8,1,7 */
	&iOR_BtDn,	/* 8,2,0 */
	&iOR_WtDn,	/* 8,2,1 */
	&iOR_LtDn,	/* 8,2,2 */
	&iDIVU,		/* 8,2,3 */
	&iSBCD_OR_BfDn,	/* 8,2,4 */
	&iOR_WfDn,	/* 8,2,5 */
	&iOR_LfDn,	/* 8,2,6 */
	&iDIVS,		/* 8,2,7 */
	&iOR_BtDn,	/* 8,3,0 */
	&iOR_WtDn,	/* 8,3,1 */
	&iOR_LtDn,	/* 8,3,2 */
	&iDIVU,		/* 8,3,3 */
	&iSBCD_OR_BfDn,	/* 8,3,4 */
	&iOR_WfDn,	/* 8,3,5 */
	&iOR_LfDn,	/* 8,3,6 */
	&iDIVS,		/* 8,3,7 */
	&iOR_BtDn,	/* 8,4,0 */
	&iOR_WtDn,	/* 8,4,1 */
	&iOR_LtDn,	/* 8,4,2 */
	&iDIVU,		/* 8,4,3 */
	&iSBCD_OR_BfDn,	/* 8,4,4 */
	&iOR_WfDn,	/* 8,4,5 */
	&iOR_LfDn,	/* 8,4,6 */
	&iDIVS,		/* 8,4,7 */
	&iOR_BtDn,	/* 8,5,0 */
	&iOR_WtDn,	/* 8,5,1 */
	&iOR_LtDn,	/* 8,5,2 */
	&iDIVU,		/* 8,5,3 */
	&iSBCD_OR_BfDn,	/* 8,5,4 */
	&iOR_WfDn,	/* 8,5,5 */
	&iOR_LfDn,	/* 8,5,6 */
	&iDIVS,		/* 8,5,7 */
	&iOR_BtDn,	/* 8,6,0 */
	&iOR_WtDn,	/* 8,6,1 */
	&iOR_LtDn,	/* 8,6,2 */
	&iDIVU,		/* 8,6,3 */
	&iSBCD_OR_BfDn,	/* 8,6,4 */
	&iOR_WfDn,	/* 8,6,5 */
	&iOR_LfDn,	/* 8,6,6 */
	&iDIVS,		/* 8,6,7 */
	&iOR_BtDn,	/* 8,7,0 */
	&iOR_WtDn,	/* 8,7,1 */
	&iOR_LtDn,	/* 8,7,2 */
	&iDIVU,		/* 8,7,3 */
	&iSBCD_OR_BfDn,	/* 8,7,4 */
	&iOR_WfDn,	/* 8,7,5 */
	&iOR_LfDn,	/* 8,7,6 */
	&iDIVS,		/* 8,7,7 */
	&iSUB_BtDn,	/* 9,0,0 */
	&iSUB_WtDn,	/* 9,0,1 */
	&iSUB_LtDn,	/* 9,0,2 */
	&iSUBA_W,	/* 9,0,3 */
	&iSUB_BfDn,	/* 9,0,4 */
	&iSUB_WfDn,	/* 9,0,5 */
	&iSUB_LfDn,	/* 9,0,6 */
	&iSUBA_L,	/* 9,0,7 */
	&iSUB_BtDn,	/* 9,1,0 */
	&iSUB_WtDn,	/* 9,1,1 */
	&iSUB_LtDn,	/* 9,1,2 */
	&iSUBA_W,	/* 9,1,3 */
	&iSUB_BfDn,	/* 9,1,4 */
	&iSUB_WfDn,	/* 9,1,5 */
	&iSUB_LfDn,	/* 9,1,6 */
	&iSUBA_L,	/* 9,1,7 */
	&iSUB_BtDn,	/* 9,2,0 */
	&iSUB_WtDn,	/* 9,2,1 */
	&iSUB_LtDn,	/* 9,2,2 */
	&iSUBA_W,	/* 9,2,3 */
	&iSUB_BfDn,	/* 9,2,4 */
	&iSUB_WfDn,	/* 9,2,5 */
	&iSUB_LfDn,	/* 9,2,6 */
	&iSUBA_L,	/* 9,2,7 */
	&iSUB_BtDn,	/* 9,3,0 */
	&iSUB_WtDn,	/* 9,3,1 */
	&iSUB_LtDn,	/* 9,3,2 */
	&iSUBA_W,	/* 9,3,3 */
	&iSUB_BfDn,	/* 9,3,4 */
	&iSUB_WfDn,	/* 9,3,5 */
	&iSUB_LfDn,	/* 9,3,6 */
	&iSUBA_L,	/* 9,3,7 */
	&iSUB_BtDn,	/* 9,4,0 */
	&iSUB_WtDn,	/* 9,4,1 */
	&iSUB_LtDn,	/* 9,4,2 */
	&iSUBA_W,	/* 9,4,3 */
	&iSUB_BfDn,	/* 9,4,4 */
	&iSUB_WfDn,	/* 9,4,5 */
	&iSUB_LfDn,	/* 9,4,6 */
	&iSUBA_L,	/* 9,4,7 */
	&iSUB_BtDn,	/* 9,5,0 */
	&iSUB_WtDn,	/* 9,5,1 */
	&iSUB_LtDn,	/* 9,5,2 */
	&iSUBA_W,	/* 9,5,3 */
	&iSUB_BfDn,	/* 9,5,4 */
	&iSUB_WfDn,	/* 9,5,5 */
	&iSUB_LfDn,	/* 9,5,6 */
	&iSUBA_L,	/* 9,5,7 */
	&iSUB_BtDn,	/* 9,6,0 */
	&iSUB_WtDn,	/* 9,6,1 */
	&iSUB_LtDn,	/* 9,6,2 */
	&iSUBA_W,	/* 9,6,3 */
	&iSUB_BfDn,	/* 9,6,4 */
	&iSUB_WfDn,	/* 9,6,5 */
	&iSUB_LfDn,	/* 9,6,6 */
	&iSUBA_L,	/* 9,6,7 */
	&iSUB_BtDn,	/* 9,7,0 */
	&iSUB_WtDn,	/* 9,7,1 */
	&iSUB_LtDn,	/* 9,7,2 */
	&iSUBA_W,	/* 9,7,3 */
	&iSUB_BfDn,	/* 9,7,4 */
	&iSUB_WfDn,	/* 9,7,5 */
	&iSUB_LfDn,	/* 9,7,6 */
	&iSUBA_L,	/* 9,7,7 */
	&ALINE,		/* a,0,0 */
	&ALINE,		/* a,0,1 */
	&ALINE,		/* a,0,2 */
	&ALINE,		/* a,0,3 */
	&ALINE,		/* a,0,4 */
	&ALINE,		/* a,0,5 */
	&ALINE,		/* a,0,6 */
	&ALINE,		/* a,0,7 */
	&iRTS,		/* a,1,0 */
	&illegal,		/* a,1,1 */
	&illegal,		/* a,1,2 */
	&illegal,		/* a,1,3 */
	&illegal,		/* a,1,4 */
	&illegal,		/* a,1,5 */
	&illegal,		/* a,1,6 */
	&illegal,		/* a,1,7 */
	&iBSR,		/* a,2,0 */
	&iBSR,		/* a,2,1 */
	&iBSR,		/* a,2,2 */
	&iBSR,		/* a,2,3 */
	&illegal,		/* a,2,4 */
	&illegal,		/* a,2,5 */
	&illegal,		/* a,2,6 */
	&illegal,		/* a,2,7 */
	&iTST_B,		/* a,3,0 */
	&iTST_W,		/* a,3,1 */
	&iTST_L,		/* a,3,2 */
	&illegal,		/* a,3,3 */
	&illegal,		/* a,3,4 */
	&illegal,		/* a,3,5 */
	&illegal,		/* a,3,6 */
	&illegal,		/* a,3,7 */
	&illegal,		/* a,4,0 */
	&illegal,		/* a,4,1 */
	&iMMfreg_W,	/* a,4,2 */
	&iMMfreg_L,	/* a,4,3 */
	&illegal,		/* a,4,4 */
	&illegal,		/* a,4,5 */
	&illegal,		/* a,4,6 */
	&illegal,		/* a,4,7 */
	&illegal,		/* a,5,0 */
	&illegal,		/* a,5,1 */
	&iMMtreg_W,	/* a,5,2 */
	&iMMtreg_L,	/* a,5,3 */
	&illegal,		/* a,5,4 */
	&illegal,		/* a,5,5 */
	&illegal,		/* a,5,6 */
	&illegal,		/* a,5,7 */
	&illegal,		/* a,6,0 */
	&illegal,		/* a,6,1 */
	&illegal,		/* a,6,2 */
	&illegal,		/* a,6,3 */
	&illegal,		/* a,6,4 */
	&illegal,		/* a,6,5 */
	&illegal,		/* a,6,6 */
	&illegal,		/* a,6,7 */
	&illegal,		/* a,7,0 */
	&illegal,		/* a,7,1 */
	&illegal,		/* a,7,2 */
	&illegal,		/* a,7,3 */
	&illegal,		/* a,7,4 */
	&illegal,		/* a,7,5 */
	&illegal,		/* a,7,6 */
	&illegal,		/* a,7,7 */
	&iCMP_B,	/* b,0,0 */
	&iCMP_W,	/* b,0,1 */
	&iCMP_L,	/* b,0,2 */
	&iCMPA_W,	/* b,0,3 */
	&iEOR_B,	/* b,0,4 */
	&iEOR_W_CMPM_W,	/* b,0,5 */
	&iEOR_L_CMPM_L,	/* b,0,6 */
	&iCMPA_L,	/* b,0,7 */
	&iCMP_B,	/* b,1,0 */
	&iCMP_W,	/* b,1,1 */
	&iCMP_L,	/* b,1,2 */
	&iCMPA_W,	/* b,1,3 */
	&iEOR_B,	/* b,1,4 */
	&iEOR_W_CMPM_W,	/* b,1,5 */
	&iEOR_L_CMPM_L,	/* b,1,6 */
	&iCMPA_L,	/* b,1,7 */
	&iCMP_B,	/* b,2,0 */
	&iCMP_W,	/* b,2,1 */
	&iCMP_L,	/* b,2,2 */
	&iCMPA_W,	/* b,2,3 */
	&iEOR_B,	/* b,2,4 */
	&iEOR_W_CMPM_W,	/* b,2,5 */
	&iEOR_L_CMPM_L,	/* b,2,6 */
	&iCMPA_L,	/* b,2,7 */
	&iCMP_B,	/* b,3,0 */
	&iCMP_W,	/* b,3,1 */
	&iCMP_L,	/* b,3,2 */
	&iCMPA_W,	/* b,3,3 */
	&iEOR_B,	/* b,3,4 */
	&iEOR_W_CMPM_W,	/* b,3,5 */
	&iEOR_L_CMPM_L,	/* b,3,6 */
	&iCMPA_L,	/* b,3,7 */
	&iCMP_B,	/* b,4,0 */
	&iCMP_W,	/* b,4,1 */
	&iCMP_L,	/* b,4,2 */
	&iCMPA_W,	/* b,4,3 */
	&iEOR_B,	/* b,4,4 */
	&iEOR_W_CMPM_W,	/* b,4,5 */
	&iEOR_L_CMPM_L,	/* b,4,6 */
	&iCMPA_L,	/* b,4,7 */
	&iCMP_B,	/* b,5,0 */
	&iCMP_W,	/* b,5,1 */
	&iCMP_L,	/* b,5,2 */
	&iCMPA_W,	/* b,5,3 */
	&iEOR_B,	/* b,5,4 */
	&iEOR_W_CMPM_W,	/* b,5,5 */
	&iEOR_L_CMPM_L,	/* b,5,6 */
	&iCMPA_L,	/* b,5,7 */
	&iCMP_B,	/* b,6,0 */
	&iCMP_W,	/* b,6,1 */
	&iCMP_L,	/* b,6,2 */
	&iCMPA_W,	/* b,6,3 */
	&iEOR_B,	/* b,6,4 */
	&iEOR_W_CMPM_W,	/* b,6,5 */
	&iEOR_L_CMPM_L,	/* b,6,6 */
	&iCMPA_L,	/* b,6,7 */
	&iCMP_B,	/* b,7,0 */
	&iCMP_W,	/* b,7,1 */
	&iCMP_L,	/* b,7,2 */
	&iCMPA_W,	/* b,7,3 */
	&iEOR_B,	/* b,7,4 */
	&iEOR_W_CMPM_W,	/* b,7,5 */
	&iEOR_L_CMPM_L,	/* b,7,6 */
	&iCMPA_L,	/* b,7,7 */
	&iAND_BtDn,	/* c,0,0 */
	&iAND_WtDn,	/* c,0,1 */
	&iAND_LtDn,	/* c,0,2 */
	&iMULU,		/* c,0,3 */
	&iABCD_AND_BfDn,	/* c,0,4 */
	&iEXG1_AND_WfDn,	/* c,0,5 */
	&iEXG2_AND_LfDn,	/* c,0,6 */
	&iMULS,		/* c,0,7 */
	&iAND_BtDn,	/* c,1,0 */
	&iAND_WtDn,	/* c,1,1 */
	&iAND_LtDn,	/* c,1,2 */
	&iMULU,		/* c,1,3 */
	&iABCD_AND_BfDn,	/* c,1,4 */
	&iEXG1_AND_WfDn,	/* c,1,5 */
	&iEXG2_AND_LfDn,	/* c,1,6 */
	&iMULS,		/* c,1,7 */
	&iAND_BtDn,	/* c,2,0 */
	&iAND_WtDn,	/* c,2,1 */
	&iAND_LtDn,	/* c,2,2 */
	&iMULU,		/* c,2,3 */
	&iABCD_AND_BfDn,	/* c,2,4 */
	&iEXG1_AND_WfDn,	/* c,2,5 */
	&iEXG2_AND_LfDn,	/* c,2,6 */
	&iMULS,		/* c,2,7 */
	&iAND_BtDn,	/* c,3,0 */
	&iAND_WtDn,	/* c,3,1 */
	&iAND_LtDn,	/* c,3,2 */
	&iMULU,		/* c,3,3 */
	&iABCD_AND_BfDn,	/* c,3,4 */
	&iEXG1_AND_WfDn,	/* c,3,5 */
	&iEXG2_AND_LfDn,	/* c,3,6 */
	&iMULS,		/* c,3,7 */
	&iAND_BtDn,	/* c,4,0 */
	&iAND_WtDn,	/* c,4,1 */
	&iAND_LtDn,	/* c,4,2 */
	&iMULU,		/* c,4,3 */
	&iABCD_AND_BfDn,	/* c,4,4 */
	&iEXG1_AND_WfDn,	/* c,4,5 */
	&iEXG2_AND_LfDn,	/* c,4,6 */
	&iMULS,		/* c,4,7 */
	&iAND_BtDn,	/* c,5,0 */
	&iAND_WtDn,	/* c,5,1 */
	&iAND_LtDn,	/* c,5,2 */
	&iMULU,		/* c,5,3 */
	&iABCD_AND_BfDn,	/* c,5,4 */
	&iEXG1_AND_WfDn,	/* c,5,5 */
	&iEXG2_AND_LfDn,	/* c,5,6 */
	&iMULS,		/* c,5,7 */
	&iAND_BtDn,	/* c,6,0 */
	&iAND_WtDn,	/* c,6,1 */
	&iAND_LtDn,	/* c,6,2 */
	&iMULU,		/* c,6,3 */
	&iABCD_AND_BfDn,	/* c,6,4 */
	&iEXG1_AND_WfDn,	/* c,6,5 */
	&iEXG2_AND_LfDn,	/* c,6,6 */
	&iMULS,		/* c,6,7 */
	&iAND_BtDn,	/* c,7,0 */
	&iAND_WtDn,	/* c,7,1 */
	&iAND_LtDn,	/* c,7,2 */
	&iMULU,		/* c,7,3 */
	&iABCD_AND_BfDn,	/* c,7,4 */
	&iEXG1_AND_WfDn,	/* c,7,5 */
	&iEXG2_AND_LfDn,	/* c,7,6 */
	&iMULS,		/* c,7,7 */
	&iADD_BtDn,	/* d,0,0 */
	&iADD_WtDn,	/* d,0,1 */
	&iADD_LtDn,	/* d,0,2 */
	&iADDA_W,	/* d,0,3 */
	&iADDX_B_ADD_BfDn,	/* d,0,4 */
	&iADDX_W_ADD_WfDn,	/* d,0,5 */
	&iADDX_L_ADD_LfDn,	/* d,0,6 */
	&iADDA_L,	/* d,0,7 */
	&iADD_BtDn,	/* d,1,0 */
	&iADD_WtDn,	/* d,1,1 */
	&iADD_LtDn,	/* d,1,2 */
	&iADDA_W,	/* d,1,3 */
	&iADDX_B_ADD_BfDn,	/* d,1,4 */
	&iADDX_W_ADD_WfDn,	/* d,1,5 */
	&iADDX_L_ADD_LfDn,	/* d,1,6 */
	&iADDA_L,	/* d,1,7 */
	&iADD_BtDn,	/* d,2,0 */
	&iADD_WtDn,	/* d,2,1 */
	&iADD_LtDn,	/* d,2,2 */
	&iADDA_W,	/* d,2,3 */
	&iADDX_B_ADD_BfDn,	/* d,2,4 */
	&iADDX_W_ADD_WfDn,	/* d,2,5 */
	&iADDX_L_ADD_LfDn,	/* d,2,6 */
	&iADDA_L,	/* d,2,7 */
	&iADD_BtDn,	/* d,3,0 */
	&iADD_WtDn,	/* d,3,1 */
	&iADD_LtDn,	/* d,3,2 */
	&iADDA_W,	/* d,3,3 */
	&iADDX_B_ADD_BfDn,	/* d,3,4 */
	&iADDX_W_ADD_WfDn,	/* d,3,5 */
	&iADDX_L_ADD_LfDn,	/* d,3,6 */
	&iADDA_L,	/* d,3,7 */
	&iADD_BtDn,	/* d,4,0 */
	&iADD_WtDn,	/* d,4,1 */
	&iADD_LtDn,	/* d,4,2 */
	&iADDA_W,	/* d,4,3 */
	&iADDX_B_ADD_BfDn,	/* d,4,4 */
	&iADDX_W_ADD_WfDn,	/* d,4,5 */
	&iADDX_L_ADD_LfDn,	/* d,4,6 */
	&iADDA_L,	/* d,4,7 */
	&iADD_BtDn,	/* d,5,0 */
	&iADD_WtDn,	/* d,5,1 */
	&iADD_LtDn,	/* d,5,2 */
	&iADDA_W,	/* d,5,3 */
	&iADDX_B_ADD_BfDn,	/* d,5,4 */
	&iADDX_W_ADD_WfDn,	/* d,5,5 */
	&iADDX_L_ADD_LfDn,	/* d,5,6 */
	&iADDA_L,	/* d,5,7 */
	&iADD_BtDn,	/* d,6,0 */
	&iADD_WtDn,	/* d,6,1 */
	&iADD_LtDn,	/* d,6,2 */
	&iADDA_W,	/* d,6,3 */
	&iADDX_B_ADD_BfDn,	/* d,6,4 */
	&iADDX_W_ADD_WfDn,	/* d,6,5 */
	&iADDX_L_ADD_LfDn,	/* d,6,6 */
	&iADDA_L,	/* d,6,7 */
	&iADD_BtDn,	/* d,7,0 */
	&iADD_WtDn,	/* d,7,1 */
	&iADD_LtDn,	/* d,7,2 */
	&iADDA_W,	/* d,7,3 */
	&iADDX_B_ADD_BfDn,	/* d,7,4 */
	&iADDX_W_ADD_WfDn,	/* d,7,5 */
	&iADDX_L_ADD_LfDn,	/* d,7,6 */
	&iADDA_L,	/* d,7,7 */
	&iSR_B_ROR_B,	/* e,0,0 */
	&iSR_W_ROR_W,	/* e,0,1 */
	&iSR_L_ROR_L,	/* e,0,2 */
	&iASR,		/* e,0,3 */
	&iSL_B_ROL_B,	/* e,0,4 */
	&iSL_W_ROL_W,	/* e,0,5 */
	&iSL_L_ROL_L,	/* e,0,6 */
	&iASL,		/* e,0,7 */
	&iSR_B_ROR_B,	/* e,1,0 */
	&iSR_W_ROR_W,	/* e,1,1 */
	&iSR_L_ROR_L,	/* e,1,2 */
	&iLSR,		/* e,1,3 */
	&iSL_B_ROL_B,	/* e,1,4 */
	&iSL_W_ROL_W,	/* e,1,5 */
	&iSL_L_ROL_L,	/* e,1,6 */
	&iLSL,		/* e,1,7 */
	&iSR_B_ROR_B,	/* e,2,0 */
	&iSR_W_ROR_W,	/* e,2,1 */
	&iSR_L_ROR_L,	/* e,2,2 */
	&iROXR,		/* e,2,3 */
	&iSL_B_ROL_B,	/* e,2,4 */
	&iSL_W_ROL_W,	/* e,2,5 */
	&iSL_L_ROL_L,	/* e,2,6 */
	&iROXL,		/* e,2,7 */
	&iSR_B_ROR_B,	/* e,3,0 */
	&iSR_W_ROR_W,	/* e,3,1 */
	&iSR_L_ROR_L,	/* e,3,2 */
	&iROR,		/* e,3,3 */
	&iSL_B_ROL_B,	/* e,3,4 */
	&iSL_W_ROL_W,	/* e,3,5 */
	&iSL_L_ROL_L,	/* e,3,6 */
	&iROL,		/* e,3,7 */
	&iSR_B_ROR_B,	/* e,4,0 */
	&iSR_W_ROR_W,	/* e,4,1 */
	&iSR_L_ROR_L,	/* e,4,2 */
	&illegal,		/* e,4,3 */
	&iSL_B_ROL_B,	/* e,4,4 */
	&iSL_W_ROL_W,	/* e,4,5 */
	&iSL_L_ROL_L,	/* e,4,6 */
	&illegal,		/* e,4,7 */
	&iSR_B_ROR_B,	/* e,5,0 */
	&iSR_W_ROR_W,	/* e,5,1 */
	&iSR_L_ROR_L,	/* e,5,2 */
	&illegal,		/* e,5,3 */
	&iSL_B_ROL_B,	/* e,5,4 */
	&iSL_W_ROL_W,	/* e,5,5 */
	&iSL_L_ROL_L,	/* e,5,6 */
	&illegal,		/* e,5,7 */
	&iSR_B_ROR_B,	/* e,6,0 */
	&iSR_W_ROR_W,	/* e,6,1 */
	&iSR_L_ROR_L,	/* e,6,2 */
	&illegal,		/* e,6,3 */
	&iSL_B_ROL_B,	/* e,6,4 */
	&iSL_W_ROL_W,	/* e,6,5 */
	&iSL_L_ROL_L,	/* e,6,6 */
	&illegal,		/* e,6,7 */
	&iSR_B_ROR_B,	/* e,7,0 */
	&iSR_W_ROR_W,	/* e,7,1 */
	&iSR_L_ROR_L,	/* e,7,2 */
	&illegal,		/* e,7,3 */
	&iSL_B_ROL_B,	/* e,7,4 */
	&iSL_W_ROL_W,	/* e,7,5 */
	&iSL_L_ROL_L,	/* e,7,6 */
	&illegal,		/* e,7,7 */
	&FLINE,		/* f,0,0 */
	&FLINE,		/* f,0,1 */
	&FLINE,		/* f,0,2 */
	&FLINE,		/* f,0,3 */
	&FLINE,		/* f,0,4 */
	&FLINE,		/* f,0,5 */
	&FLINE,		/* f,0,6 */
	&FLINE,		/* f,0,7 */
	&FLINE,		/* f,1,0 */
	&FLINE,		/* f,1,1 */
	&FLINE,		/* f,1,2 */
	&FLINE,		/* f,1,3 */
	&FLINE,		/* f,1,4 */
	&FLINE,		/* f,1,5 */
	&FLINE,		/* f,1,6 */
	&FLINE,		/* f,1,7 */
	&FLINE,		/* f,2,0 */
	&FLINE,		/* f,2,1 */
	&FLINE,		/* f,2,2 */
	&FLINE,		/* f,2,3 */
	&FLINE,		/* f,2,4 */
	&FLINE,		/* f,2,5 */
	&FLINE,		/* f,2,6 */
	&FLINE,		/* f,2,7 */
	&FLINE,		/* f,3,0 */
	&FLINE,		/* f,3,1 */
	&FLINE,		/* f,3,2 */
	&FLINE,		/* f,3,3 */
	&FLINE,		/* f,3,4 */
	&FLINE,		/* f,3,5 */
	&FLINE,		/* f,3,6 */
	&FLINE,		/* f,3,7 */
	&FLINE,		/* f,4,0 */
	&FLINE,		/* f,4,1 */
	&FLINE,		/* f,4,2 */
	&FLINE,		/* f,4,3 */
	&FLINE,		/* f,4,4 */
	&FLINE,		/* f,4,5 */
	&FLINE,		/* f,4,6 */
	&FLINE,		/* f,4,7 */
	&FLINE,		/* f,5,0 */
	&FLINE,		/* f,5,1 */
	&FLINE,		/* f,5,2 */
	&FLINE,		/* f,5,3 */
	&FLINE,		/* f,5,4 */
	&FLINE,		/* f,5,5 */
	&FLINE,		/* f,5,6 */
	&FLINE,		/* f,5,7 */
	&FLINE,		/* f,6,0 */
	&FLINE,		/* f,6,1 */
	&FLINE,		/* f,6,2 */
	&FLINE,		/* f,6,3 */
	&FLINE,		/* f,6,4 */
	&FLINE,		/* f,6,5 */
	&FLINE,		/* f,6,6 */
	&FLINE,		/* f,6,7 */
	&FLINE,		/* f,7,0 */
	&FLINE,		/* f,7,1 */
	&FLINE,		/* f,7,2 */
	&FLINE,		/* f,7,3 */
	&FLINE,		/* f,7,4 */
	&FLINE,		/* f,7,5 */
	&FLINE,		/* f,7,6 */
	&FLINE,		/* f,7,7 */
};

extern int iTRAP, iLINK_W, iUNLK, iMOVEtUSP, iMOVEfUSP,imisc4E_2, iMOVEC;
extern int iRESET, iNOP, iSTOP, iRTE, iRTD, iRTS, iTRAPV, iRTR;

int *table1[] = {
	&iTRAP, &iTRAP, &iLINK_W, &iUNLK,
	&iMOVEtUSP, &iMOVEfUSP, &imisc4E_2, &iMOVEC
};

int *table2[] = {
	&iRESET, &iNOP, &iSTOP, &iRTE,
	&iRTD, &iRTS, &iTRAPV, &iRTR
};

extern int SEARCH;
extern int GETDATA;
extern int SETNOUNS;
extern int NOUNSCAN;
extern int WORDSCAN;
extern int GETNOUNS;
extern int QCARRIED;
extern int TEXT;
extern int RANDOM1;
extern int RANDOM2;
extern int LOADFILE;
extern int SAVEFILE;
extern int CHAROUT;
extern int GETTIED;
extern int FINDZERO;
extern int GRAPHICS;
extern int PRINTER;
extern int RESTART;
extern int QUIT;
extern int GETWTAB;
extern int PUTWTAB;
extern int PRINTWORD;
extern int COPYWORD;
extern int SETNE;
extern int SETEQ;
extern int SETNE_RET;
extern int SETEQ_RET;
extern int POPSP_RET;
extern int MAKE_DIFF;
extern int UNDO_DIFF;
extern int GET_NEXT_EV;
extern int PROMPT_EV;
extern int NOTIFY;

int *aline_table[] = {
	&SEARCH,
	&GETDATA,
	&SETNOUNS,
	&NOUNSCAN,
	&WORDSCAN,
	&GETNOUNS,
	&QCARRIED,
	&TEXT,
	&RANDOM1,
	&RANDOM2,
	&LOADFILE,
	&SAVEFILE,
	&CHAROUT,
	&GETTIED,
	&FINDZERO,
	&GRAPHICS,
	&PRINTER,
	&RESTART,
	&QUIT,
	&GETWTAB,
	&PUTWTAB,
	&PRINTWORD,
	&COPYWORD,
	&SETNE,
	&SETEQ,
	&SETNE_RET,
	&SETEQ_RET,
	&POPSP_RET,
	&MAKE_DIFF,
	&UNDO_DIFF,
	&GET_NEXT_EV,
	&PROMPT_EV,
	&NOTIFY,
};
