/**
 *
 *    _    __        _      __                           
 *   | |  / /____   (_)____/ /_      __ ____ _ _____ ___ 
 *   | | / // __ \ / // __  /| | /| / // __ `// ___// _ \
 *   | |/ // /_/ // // /_/ / | |/ |/ // /_/ // /   /  __/
 *   |___/ \____//_/ \__,_/  |__/|__/ \__,_//_/    \___/ 
 *                                                       
 * Copyright (c) 2017 Voidware Ltd.  All Rights Reserved.
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
 * It is being released under a Public Source Licence for posterity and
 * historical reference.
 */

#pragma once

/**
 * Defines colour correction profiles for the game pictures
 * for use with the QtGraphicalEffects shaders in QML
 *
 * Based on the following Photoshop CC 2017 profiles
 *
 * The Pawn:
 * PicNo.   Brightness  Contrast    Saturation  Lightness
 * ------------------------------------------------------
 *  0       14          97          -19         5
 *  1       27          9           0           0
 *  2       34          50          -11         0
 *  3       27          17          0           0
 *  4       21          50          -25         0
 *  5       14          98          -30         -15
 *  6       15          30          20          0
 *  7       6           73          5           0
 *  8       21          25          0           0
 *  9       30          75          -10         0
 *  10      10          20          0           0
 *  11      -6          -50         -20         0
 *  12      10          68          -30         -10
 *  13      11          21          15          -10
 *  14      24          40          -11         -5
 *  15      39          -50         10          0
 *  16      5           100         -20         0
 *  17      20          42          -10         0
 *  18      5           70          30          -19
 *  19      49          46          -20         0
 *  20      36          50          -5          0
 *  21      7           89          -15         -14
 *  22      32          80          20          2
 *  23      22          32          0           0
 *  24      34          12          0           0
 *  25      37          31          20          0
 *  26      24          55          -28         -15
 *  27      24          60          -7          -10
 *  28      5           80          -30         0
 *  29      41          70          -10         -16
 */

#define MAX_PICS 111

// QML shader values: Contrast Brightness Saturation Lightness Gamma

static float pcc[10][MAX_PICS][5] = {
                        { // The Pawn.
                            {  0.25,  0.14,  -0.18,  -0.05, 1.0 }, //0
                            {  0.2,  0.17,  0.0,  0.0, 1.0 }, //1
                            {  0.39,  0.44,  -0.09,  0.0, 0.88 }, //2
                            {  0.17,  0.17,  0.0,  0.0, 1.0 }, //3
                            {  0.3,  0.25,  -0.30,  -0.15, 1.0 }, //4
                            {  0.2,  0.09,  -0.30,  -0.15, 1.02 }, //5
                            {  0.15,  0.05,  0.20,  0.0, 1.0 }, //6
                            {  0.2,  0.1,  0.05,  0.0, 1.0 }, //7
                            {  0.1,  0.1,  0.0,  0.0, 1.0 }, //8
                            {  0.35,  0.41,  -0.20,  0.0, 0.86 }, //9
                            {  0.15,  0.1,  -0.05,  0.0, 0.98 }, //10
                            {  0.05,  -0.06,  -0.20,  0.0, 1.0 }, //11
                            {  0.25,  0.15,  -0.30,  -0.10, 0.91 }, //12
                            {  0.08,  0.11,  0.15,  -0.09, 1.0 }, //13
                            {  0.22,  0.24,  -0.11,  -0.05, 1.0 }, //14
                            {  0.17,  0.25,  0.1,  0.0, 1.0 }, //15
                            {  0.16,  0.05,  -0.20,  0.0, 1.0 }, //16
                            {  0.24,  0.2,  -0.10,  0.0, 1.0 }, //17
                            {  0.21,  0.05,  0.30,  -0.19, 1.0 }, //18
                            {  0.46,  0.49,  -0.30,  0.0, 1.0 }, //19
                            {  0.29,  0.25,  -0.10,  0.0, 1.0 }, //20
                            {  0.21,  0.07,  -0.15,  -0.14, 1.0 }, //21
                            {  0.24,  0.21,  0.2,  0.02, 1.0 }, //22
                            {  0.14,  0.12,  0.0,  0.0, 1.0 }, //23
                            {  0.25,  0.26,  -0.21,  0.0, 1.0 }, //24
                            {  0.26,  0.28,  0.19,  0.0, 1.0 }, //25
                            {  0.28,  0.26,  -0.28,  -0.23, 1.09 }, //26
                            {  0.0,  0.0,  0.0,  0.0, 1.0 }, //27
                            {  0.18,  0.11,  -0.12,  0.0, 1.0 },
                            {  0.0,  0.0,  0.0,  0.0, 1.0 } //29
                        }
};
