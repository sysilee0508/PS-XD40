//----------------------------------------------------------------------------------------------------------------------
// (C) Copyright 2008  Macro Image Technology Co., LTd. , All rights reserved
// 
// This source code is the property of Macro Image Technology and is provided
// pursuant to a Software License Agreement. This code's reuse and distribution
// without Macro Image Technology's permission is strictly limited by the confidential
// information provisions of the Software License Agreement.
//-----------------------------------------------------------------------------------------------------------------------
//
// File Name   		:  MFCCOEF.H
// Description 		:  This file contains typedefine for the driver files	
// Ref. Docment		: 
// Revision History 	:

#ifndef		__MFCCOEF_H__
#define		__MFCCOEF_H__

// -----------------------------------------------------------------------------
// Include files
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Struct/Union Types and define
// -----------------------------------------------------------------------------

// ----------------------------------------------------------------------
// Static Global Data section variables
// ----------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------
// Lanczos, 2-tap, 64-phase for interpolation filter coefficients
static ROMDATA MDIN_MFCFILT_COEF EDK_MFCFilter_Lanczos_4		= {
	{(WORD) 2048, (WORD)    0,		// phase 00
	 (WORD) 2043, (WORD)  -38,		// phase 02
	 (WORD) 2030, (WORD)  -71,		// phase 04
	 (WORD) 2007, (WORD)  -99,		// phase 06
	 (WORD) 1977, (WORD) -123,		// phase 08
	 (WORD) 1938, (WORD) -141,		// phase 10
	 (WORD) 1892, (WORD) -156,		// phase 12
	 (WORD) 1838, (WORD) -165,		// phase 14
	 (WORD) 1779, (WORD) -172,		// phase 16
	 (WORD) 1714, (WORD) -175,		// phase 18
	 (WORD) 1643, (WORD) -174,		// phase 20
	 (WORD) 1569, (WORD) -171,		// phase 22
	 (WORD) 1490, (WORD) -166,		// phase 24
	 (WORD) 1409, (WORD) -158,		// phase 26
	 (WORD) 1325, (WORD) -150,		// phase 28
	 (WORD) 1239, (WORD) -139,		// phase 30
	 (WORD) 1152, (WORD) -128,		// phase 32
	 (WORD) 1064, (WORD) -116,		// phase 34
	 (WORD)  977, (WORD) -104,		// phase 36
	 (WORD)  889, (WORD)  -92,		// phase 38
	 (WORD)  803, (WORD)  -79,		// phase 40
	 (WORD)  718, (WORD)  -68,		// phase 42
	 (WORD)  635, (WORD)  -56,		// phase 44
	 (WORD)  555, (WORD)  -46,		// phase 46
	 (WORD)  477, (WORD)  -36,		// phase 48
	 (WORD)  403, (WORD)  -28,		// phase 50
	 (WORD)  332, (WORD)  -20,		// phase 52
	 (WORD)  265, (WORD)  -14,		// phase 54
	 (WORD)  203, (WORD)   -9,		// phase 56
	 (WORD)  145, (WORD)   -5,		// phase 58
	 (WORD)   91, (WORD)   -2,		// phase 60
	 (WORD)   43, (WORD)    0 },	// phase 62

// Spline, B=1.00, C=0.00, 2-tap, upper 32-phase
	{(WORD) 1365, (WORD)  341,		// phase 00
	 (WORD) 1364, (WORD)  310,		// phase 02
	 (WORD) 1358, (WORD)  281,		// phase 04
	 (WORD) 1348, (WORD)  254,		// phase 06
	 (WORD) 1335, (WORD)  229,		// phase 08
	 (WORD) 1319, (WORD)  205,		// phase 10
	 (WORD) 1300, (WORD)  183,		// phase 12
	 (WORD) 1278, (WORD)  163,		// phase 14
	 (WORD) 1253, (WORD)  144,		// phase 16
	 (WORD) 1226, (WORD)  127,		// phase 18
	 (WORD) 1197, (WORD)  111,		// phase 20
	 (WORD) 1165, (WORD)   96,		// phase 22
	 (WORD) 1131, (WORD)   83,		// phase 24
	 (WORD) 1096, (WORD)   71,		// phase 26
	 (WORD) 1059, (WORD)   61,		// phase 28
	 (WORD) 1021, (WORD)   51,		// phase 30
	 (WORD)  981, (WORD)   43,		// phase 32
	 (WORD)  941, (WORD)   35,		// phase 34
	 (WORD)  900, (WORD)   29,		// phase 36
	 (WORD)  858, (WORD)   23,		// phase 38
	 (WORD)  815, (WORD)   18,		// phase 40
	 (WORD)  773, (WORD)   14,		// phase 42
	 (WORD)  730, (WORD)   10,		// phase 44
	 (WORD)  687, (WORD)    8,		// phase 46
	 (WORD)  645, (WORD)    5,		// phase 48
	 (WORD)  604, (WORD)    3,		// phase 50
	 (WORD)  563, (WORD)    2,		// phase 52
	 (WORD)  523, (WORD)    1,		// phase 54
	 (WORD)  483, (WORD)    1,		// phase 56
	 (WORD)  446, (WORD)    0,		// phase 58
	 (WORD)  409, (WORD)    0,		// phase 60
	 (WORD)  374, (WORD)    0 }		// phase 62
};

// Spline, B=0.00, C=1.00, 2-tap, 64-phase for interpolation filter coefficients
static ROMDATA MDIN_MFCFILT_COEF EDK_MFCFilter_Spline_1_00		= {
// Spline, B=0.00, C=1.00, 2-tap, lower 32-phase
	{(WORD) 2048, (WORD)    0,		// phase 00
	 (WORD) 2044, (WORD)  -60,		// phase 02
	 (WORD) 2033, (WORD) -113,		// phase 04
	 (WORD) 2014, (WORD) -158,		// phase 06
	 (WORD) 1988, (WORD) -196,		// phase 08
	 (WORD) 1956, (WORD) -228,		// phase 10
	 (WORD) 1918, (WORD) -254,		// phase 12
	 (WORD) 1873, (WORD) -273,		// phase 14
	 (WORD) 1824, (WORD) -288,		// phase 16
	 (WORD) 1770, (WORD) -298,		// phase 18
	 (WORD) 1711, (WORD) -303,		// phase 20
	 (WORD) 1647, (WORD) -303,		// phase 22
	 (WORD) 1580, (WORD) -300,		// phase 24
	 (WORD) 1509, (WORD) -293,		// phase 26
	 (WORD) 1436, (WORD) -284,		// phase 28
	 (WORD) 1359, (WORD) -271,		// phase 30
	 (WORD) 1280, (WORD) -256,		// phase 32
	 (WORD) 1199, (WORD) -239,		// phase 34
	 (WORD) 1117, (WORD) -221,		// phase 36
	 (WORD) 1033, (WORD) -201,		// phase 38
	 (WORD)  948, (WORD) -180,		// phase 40
	 (WORD)  863, (WORD) -159,		// phase 42
	 (WORD)  778, (WORD) -138,		// phase 44
	 (WORD)  692, (WORD) -116,		// phase 46
	 (WORD)  608, (WORD)  -96,		// phase 48
	 (WORD)  525, (WORD)  -77,		// phase 50
	 (WORD)  443, (WORD)  -59,		// phase 52
	 (WORD)  362, (WORD)  -42,		// phase 54
	 (WORD)  284, (WORD)  -28,		// phase 56
	 (WORD)  208, (WORD)  -16,		// phase 58
	 (WORD)  136, (WORD)   -8,		// phase 60
	 (WORD)   66, (WORD)   -2 },	// phase 62

// Spline, B=1.00, C=0.00, 2-tap, upper 32-phase
	{(WORD) 1365, (WORD)  341,		// phase 00
	 (WORD) 1364, (WORD)  310,		// phase 02
	 (WORD) 1358, (WORD)  281,		// phase 04
	 (WORD) 1348, (WORD)  254,		// phase 06
	 (WORD) 1335, (WORD)  229,		// phase 08
	 (WORD) 1319, (WORD)  205,		// phase 10
	 (WORD) 1300, (WORD)  183,		// phase 12
	 (WORD) 1278, (WORD)  163,		// phase 14
	 (WORD) 1253, (WORD)  144,		// phase 16
	 (WORD) 1226, (WORD)  127,		// phase 18
	 (WORD) 1197, (WORD)  111,		// phase 20
	 (WORD) 1165, (WORD)   96,		// phase 22
	 (WORD) 1131, (WORD)   83,		// phase 24
	 (WORD) 1096, (WORD)   71,		// phase 26
	 (WORD) 1059, (WORD)   61,		// phase 28
	 (WORD) 1021, (WORD)   51,		// phase 30
	 (WORD)  981, (WORD)   43,		// phase 32
	 (WORD)  941, (WORD)   35,		// phase 34
	 (WORD)  900, (WORD)   29,		// phase 36
	 (WORD)  858, (WORD)   23,		// phase 38
	 (WORD)  815, (WORD)   18,		// phase 40
	 (WORD)  773, (WORD)   14,		// phase 42
	 (WORD)  730, (WORD)   10,		// phase 44
	 (WORD)  687, (WORD)    8,		// phase 46
	 (WORD)  645, (WORD)    5,		// phase 48
	 (WORD)  604, (WORD)    3,		// phase 50
	 (WORD)  563, (WORD)    2,		// phase 52
	 (WORD)  523, (WORD)    1,		// phase 54
	 (WORD)  483, (WORD)    1,		// phase 56
	 (WORD)  446, (WORD)    0,		// phase 58
	 (WORD)  409, (WORD)    0,		// phase 60
	 (WORD)  374, (WORD)    0 } 	// phase 62
};

// Spline, B=0.00, C=0.75, 2-tap, 64-phase for interpolation filter coefficients
static ROMDATA MDIN_MFCFILT_COEF EDK_MFCFilter_Spline_0_75		= {
// Spline, B=0.00, C=0.75, 2-tap, lower 32-phase
	{(WORD) 2048, (WORD)    0,		// phase 00
	 (WORD) 2044, (WORD)  -45,		// phase 02
	 (WORD) 2031, (WORD)  -84,		// phase 04
	 (WORD) 2009, (WORD) -118,		// phase 06
	 (WORD) 1981, (WORD) -147,		// phase 08
	 (WORD) 1945, (WORD) -171,		// phase 10
	 (WORD) 1903, (WORD) -190,		// phase 12
	 (WORD) 1854, (WORD) -205,		// phase 14
	 (WORD) 1800, (WORD) -216,		// phase 16
	 (WORD) 1740, (WORD) -223,		// phase 18
	 (WORD) 1676, (WORD) -227,		// phase 20
	 (WORD) 1607, (WORD) -227,		// phase 22
	 (WORD) 1535, (WORD) -225,		// phase 24
	 (WORD) 1459, (WORD) -220,		// phase 26
	 (WORD) 1380, (WORD) -213,		// phase 28
	 (WORD) 1299, (WORD) -203,		// phase 30
	 (WORD) 1216, (WORD) -192,		// phase 32
	 (WORD) 1131, (WORD) -179,		// phase 34
	 (WORD) 1046, (WORD) -165,		// phase 36
	 (WORD)  959, (WORD) -150,		// phase 38
	 (WORD)  873, (WORD) -135,		// phase 40
	 (WORD)  787, (WORD) -119,		// phase 42
	 (WORD)  702, (WORD) -103,		// phase 44
	 (WORD)  618, (WORD)  -87,		// phase 46
	 (WORD)  536, (WORD)  -72,		// phase 48
	 (WORD)  456, (WORD)  -57,		// phase 50
	 (WORD)  379, (WORD)  -44,		// phase 52
	 (WORD)  305, (WORD)  -31,		// phase 54
	 (WORD)  235, (WORD)  -21,		// phase 56
	 (WORD)  169, (WORD)  -12,		// phase 58
	 (WORD)  107, (WORD)   -6,		// phase 60
	 (WORD)   51, (WORD)   -2 },	// phase 62

// Spline, B=1.00, C=0.00, 2-tap, upper 32-phase
	{(WORD) 1365, (WORD)  341,		// phase 00
	 (WORD) 1364, (WORD)  310,		// phase 02
	 (WORD) 1358, (WORD)  281,		// phase 04
	 (WORD) 1348, (WORD)  254,		// phase 06
	 (WORD) 1335, (WORD)  229,		// phase 08
	 (WORD) 1319, (WORD)  205,		// phase 10
	 (WORD) 1300, (WORD)  183,		// phase 12
	 (WORD) 1278, (WORD)  163,		// phase 14
	 (WORD) 1253, (WORD)  144,		// phase 16
	 (WORD) 1226, (WORD)  127,		// phase 18
	 (WORD) 1197, (WORD)  111,		// phase 20
	 (WORD) 1165, (WORD)   96,		// phase 22
	 (WORD) 1131, (WORD)   83,		// phase 24
	 (WORD) 1096, (WORD)   71,		// phase 26
	 (WORD) 1059, (WORD)   61,		// phase 28
	 (WORD) 1021, (WORD)   51,		// phase 30
	 (WORD)  981, (WORD)   43,		// phase 32
	 (WORD)  941, (WORD)   35,		// phase 34
	 (WORD)  900, (WORD)   29,		// phase 36
	 (WORD)  858, (WORD)   23,		// phase 38
	 (WORD)  815, (WORD)   18,		// phase 40
	 (WORD)  773, (WORD)   14,		// phase 42
	 (WORD)  730, (WORD)   10,		// phase 44
	 (WORD)  687, (WORD)    8,		// phase 46
	 (WORD)  645, (WORD)    5,		// phase 48
	 (WORD)  604, (WORD)    3,		// phase 50
	 (WORD)  563, (WORD)    2,		// phase 52
	 (WORD)  523, (WORD)    1,		// phase 54
	 (WORD)  483, (WORD)    1,		// phase 56
	 (WORD)  446, (WORD)    0,		// phase 58
	 (WORD)  409, (WORD)    0,		// phase 60
	 (WORD)  374, (WORD)    0 }		// phase 62
};

// Spline, B=0.00, C=0.50, 2-tap, 64-phase for interpolation filter coefficients
static ROMDATA MDIN_MFCFILT_COEF EDK_MFCFilter_Spline_0_50		= {
	{(WORD) 2048, (WORD)    0,		// phase 00
	 (WORD) 2043, (WORD)  -30,		// phase 02
	 (WORD) 2029, (WORD)  -56,		// phase 04
	 (WORD) 2006, (WORD)  -79,		// phase 06
	 (WORD) 1974, (WORD)  -98,		// phase 08
	 (WORD) 1935, (WORD) -114,		// phase 10
	 (WORD) 1888, (WORD) -127,		// phase 12
	 (WORD) 1835, (WORD) -137,		// phase 14
	 (WORD) 1776, (WORD) -144,		// phase 16
	 (WORD) 1711, (WORD) -149,		// phase 18
	 (WORD) 1642, (WORD) -151,		// phase 20
	 (WORD) 1568, (WORD) -152,		// phase 22
	 (WORD) 1490, (WORD) -150,		// phase 24
	 (WORD) 1409, (WORD) -147,		// phase 26
	 (WORD) 1325, (WORD) -142,		// phase 28
	 (WORD) 1239, (WORD) -135,		// phase 30
	 (WORD) 1152, (WORD) -128,		// phase 32
	 (WORD) 1064, (WORD) -120,		// phase 34
	 (WORD)  975, (WORD) -110,		// phase 36
	 (WORD)  886, (WORD) -100,		// phase 38
	 (WORD)  798, (WORD)  -90,		// phase 40
	 (WORD)  711, (WORD)  -79,		// phase 42
	 (WORD)  626, (WORD)  -69,		// phase 44
	 (WORD)  544, (WORD)  -58,		// phase 46
	 (WORD)  464, (WORD)  -48,		// phase 48
	 (WORD)  388, (WORD)  -38,		// phase 50
	 (WORD)  316, (WORD)  -29,		// phase 52
	 (WORD)  248, (WORD)  -21,		// phase 54
	 (WORD)  186, (WORD)  -14,		// phase 56
	 (WORD)  129, (WORD)   -8,		// phase 58
	 (WORD)   79, (WORD)   -4,		// phase 60
	 (WORD)   36, (WORD)   -1 },	// phase 62

// Spline, B=1.00, C=0.00, 2-tap, upper 32-phase
	{(WORD) 1365, (WORD)  341,		// phase 00
	 (WORD) 1364, (WORD)  310,		// phase 02
	 (WORD) 1358, (WORD)  281,		// phase 04
	 (WORD) 1348, (WORD)  254,		// phase 06
	 (WORD) 1335, (WORD)  229,		// phase 08
	 (WORD) 1319, (WORD)  205,		// phase 10
	 (WORD) 1300, (WORD)  183,		// phase 12
	 (WORD) 1278, (WORD)  163,		// phase 14
	 (WORD) 1253, (WORD)  144,		// phase 16
	 (WORD) 1226, (WORD)  127,		// phase 18
	 (WORD) 1197, (WORD)  111,		// phase 20
	 (WORD) 1165, (WORD)   96,		// phase 22
	 (WORD) 1131, (WORD)   83,		// phase 24
	 (WORD) 1096, (WORD)   71,		// phase 26
	 (WORD) 1059, (WORD)   61,		// phase 28
	 (WORD) 1021, (WORD)   51,		// phase 30
	 (WORD)  981, (WORD)   43,		// phase 32
	 (WORD)  941, (WORD)   35,		// phase 34
	 (WORD)  900, (WORD)   29,		// phase 36
	 (WORD)  858, (WORD)   23,		// phase 38
	 (WORD)  815, (WORD)   18,		// phase 40
	 (WORD)  773, (WORD)   14,		// phase 42
	 (WORD)  730, (WORD)   10,		// phase 44
	 (WORD)  687, (WORD)    8,		// phase 46
	 (WORD)  645, (WORD)    5,		// phase 48
	 (WORD)  604, (WORD)    3,		// phase 50
	 (WORD)  563, (WORD)    2,		// phase 52
	 (WORD)  523, (WORD)    1,		// phase 54
	 (WORD)  483, (WORD)    1,		// phase 56
	 (WORD)  446, (WORD)    0,		// phase 58
	 (WORD)  409, (WORD)    0,		// phase 60
	 (WORD)  374, (WORD)    0 }		// phase 62
};

// Bi-linear (2-tap), 32-phase for interpolation filter coefficients
static ROMDATA MDIN_MFCFILT_COEF EDK_MFCFilter_Bilinear			= {
	{(WORD) 2048, (WORD)    0,		// phase 00
	 (WORD) 1984, (WORD)    0,		// phase 02
	 (WORD) 1920, (WORD)    0,		// phase 04
	 (WORD) 1856, (WORD)    0,		// phase 06
	 (WORD) 1792, (WORD)    0,		// phase 08
	 (WORD) 1728, (WORD)    0,		// phase 10
	 (WORD) 1664, (WORD)    0,		// phase 12
	 (WORD) 1600, (WORD)    0,		// phase 14
	 (WORD) 1536, (WORD)    0,		// phase 16
	 (WORD) 1472, (WORD)    0,		// phase 18
	 (WORD) 1408, (WORD)    0,		// phase 20
	 (WORD) 1344, (WORD)    0,		// phase 22
	 (WORD) 1280, (WORD)    0,		// phase 24
	 (WORD) 1216, (WORD)    0,		// phase 26
	 (WORD) 1152, (WORD)    0,		// phase 28
	 (WORD) 1088, (WORD)    0,		// phase 30
	 (WORD) 1024, (WORD)    0,		// phase 32
	 (WORD)  960, (WORD)    0,		// phase 34
	 (WORD)  896, (WORD)    0,		// phase 36
	 (WORD)  832, (WORD)    0,		// phase 38
	 (WORD)  768, (WORD)    0,		// phase 40
	 (WORD)  704, (WORD)    0,		// phase 42
	 (WORD)  640, (WORD)    0,		// phase 44
	 (WORD)  576, (WORD)    0,		// phase 46
	 (WORD)  512, (WORD)    0,		// phase 48
	 (WORD)  448, (WORD)    0,		// phase 50
	 (WORD)  384, (WORD)    0,		// phase 52
	 (WORD)  320, (WORD)    0,		// phase 54
	 (WORD)  256, (WORD)    0,		// phase 56
	 (WORD)  192, (WORD)    0,		// phase 58
	 (WORD)  128, (WORD)    0,		// phase 60
	 (WORD)   64, (WORD)    0 },	// phase 62

// Spline, B=1.00, C=0.00, 2-tap, upper 32-phase
	{(WORD) 1365, (WORD)  341,		// phase 00
	 (WORD) 1364, (WORD)  310,		// phase 02
	 (WORD) 1358, (WORD)  281,		// phase 04
	 (WORD) 1348, (WORD)  254,		// phase 06
	 (WORD) 1335, (WORD)  229,		// phase 08
	 (WORD) 1319, (WORD)  205,		// phase 10
	 (WORD) 1300, (WORD)  183,		// phase 12
	 (WORD) 1278, (WORD)  163,		// phase 14
	 (WORD) 1253, (WORD)  144,		// phase 16
	 (WORD) 1226, (WORD)  127,		// phase 18
	 (WORD) 1197, (WORD)  111,		// phase 20
	 (WORD) 1165, (WORD)   96,		// phase 22
	 (WORD) 1131, (WORD)   83,		// phase 24
	 (WORD) 1096, (WORD)   71,		// phase 26
	 (WORD) 1059, (WORD)   61,		// phase 28
	 (WORD) 1021, (WORD)   51,		// phase 30
	 (WORD)  981, (WORD)   43,		// phase 32
	 (WORD)  941, (WORD)   35,		// phase 34
	 (WORD)  900, (WORD)   29,		// phase 36
	 (WORD)  858, (WORD)   23,		// phase 38
	 (WORD)  815, (WORD)   18,		// phase 40
	 (WORD)  773, (WORD)   14,		// phase 42
	 (WORD)  730, (WORD)   10,		// phase 44
	 (WORD)  687, (WORD)    8,		// phase 46
	 (WORD)  645, (WORD)    5,		// phase 48
	 (WORD)  604, (WORD)    3,		// phase 50
	 (WORD)  563, (WORD)    2,		// phase 52
	 (WORD)  523, (WORD)    1,		// phase 54
	 (WORD)  483, (WORD)    1,		// phase 56
	 (WORD)  446, (WORD)    0,		// phase 58
	 (WORD)  409, (WORD)    0,		// phase 60
	 (WORD)  374, (WORD)    0 }		// phase 62
};

static ROMDATA MDIN_MFCFILT_COEF EDK_MFCFilter_BSpline_Most_Blur	= {
	{(WORD) 1365, (WORD)  341,		// phase 00
	 (WORD) 1364, (WORD)  310,		// phase 02
	 (WORD) 1358, (WORD)  281,		// phase 04
	 (WORD) 1348, (WORD)  254,		// phase 06
	 (WORD) 1335, (WORD)  229,		// phase 08
	 (WORD) 1319, (WORD)  205,		// phase 10
	 (WORD) 1300, (WORD)  183,		// phase 12
	 (WORD) 1278, (WORD)  163,		// phase 14
	 (WORD) 1253, (WORD)  144,		// phase 16
	 (WORD) 1226, (WORD)  127,		// phase 18
	 (WORD) 1197, (WORD)  111,		// phase 20
	 (WORD) 1165, (WORD)   96,		// phase 22
	 (WORD) 1131, (WORD)   83,		// phase 24
	 (WORD) 1096, (WORD)   71,		// phase 26
	 (WORD) 1059, (WORD)   61,		// phase 28
	 (WORD) 1021, (WORD)   51,		// phase 30
	 (WORD)  981, (WORD)   43,		// phase 32
	 (WORD)  941, (WORD)   35,		// phase 34
	 (WORD)  900, (WORD)   29,		// phase 36
	 (WORD)  858, (WORD)   23,		// phase 38
	 (WORD)  815, (WORD)   18,		// phase 40
	 (WORD)  773, (WORD)   14,		// phase 42
	 (WORD)  730, (WORD)   10,		// phase 44
	 (WORD)  687, (WORD)    8,		// phase 46
	 (WORD)  645, (WORD)    5,		// phase 48
	 (WORD)  604, (WORD)    3,		// phase 50
	 (WORD)  563, (WORD)    2,		// phase 52
	 (WORD)  523, (WORD)    1,		// phase 54
	 (WORD)  483, (WORD)    1,		// phase 56
	 (WORD)  446, (WORD)    0,		// phase 58
	 (WORD)  409, (WORD)    0,		// phase 60
	 (WORD)  374, (WORD)    0 }, 	// phase 62

// Spline, B=1.00, C=0.00, 2-tap, upper 32-phase
	{(WORD) 1365, (WORD)  341,		// phase 00
	 (WORD) 1364, (WORD)  310,		// phase 02
	 (WORD) 1358, (WORD)  281,		// phase 04
	 (WORD) 1348, (WORD)  254,		// phase 06
	 (WORD) 1335, (WORD)  229,		// phase 08
	 (WORD) 1319, (WORD)  205,		// phase 10
	 (WORD) 1300, (WORD)  183,		// phase 12
	 (WORD) 1278, (WORD)  163,		// phase 14
	 (WORD) 1253, (WORD)  144,		// phase 16
	 (WORD) 1226, (WORD)  127,		// phase 18
	 (WORD) 1197, (WORD)  111,		// phase 20
	 (WORD) 1165, (WORD)   96,		// phase 22
	 (WORD) 1131, (WORD)   83,		// phase 24
	 (WORD) 1096, (WORD)   71,		// phase 26
	 (WORD) 1059, (WORD)   61,		// phase 28
	 (WORD) 1021, (WORD)   51,		// phase 30
	 (WORD)  981, (WORD)   43,		// phase 32
	 (WORD)  941, (WORD)   35,		// phase 34
	 (WORD)  900, (WORD)   29,		// phase 36
	 (WORD)  858, (WORD)   23,		// phase 38
	 (WORD)  815, (WORD)   18,		// phase 40
	 (WORD)  773, (WORD)   14,		// phase 42
	 (WORD)  730, (WORD)   10,		// phase 44
	 (WORD)  687, (WORD)    8,		// phase 46
	 (WORD)  645, (WORD)    5,		// phase 48
	 (WORD)  604, (WORD)    3,		// phase 50
	 (WORD)  563, (WORD)    2,		// phase 52
	 (WORD)  523, (WORD)    1,		// phase 54
	 (WORD)  483, (WORD)    1,		// phase 56
	 (WORD)  446, (WORD)    0,		// phase 58
	 (WORD)  409, (WORD)    0,		// phase 60
	 (WORD)  374, (WORD)    0 }		// phase 62
};

static ROMDATA MDIN_MFCFILT_COEF EDK_MFCFilter_BSpline_Mitchell		= {
	{(WORD) 1820, (WORD)  114,		// phase 00
	 (WORD) 1817, (WORD)   83,		// phase 02
	 (WORD) 1805, (WORD)   56,		// phase 04
	 (WORD) 1786, (WORD)   32,		// phase 06
	 (WORD) 1761, (WORD)   11,		// phase 08
	 (WORD) 1730, (WORD)   -8,		// phase 10
	 (WORD) 1692, (WORD)  -23,		// phase 12
	 (WORD) 1649, (WORD)  -37,		// phase 14
	 (WORD) 1602, (WORD)  -48,		// phase 16
	 (WORD) 1549, (WORD)  -57,		// phase 18
	 (WORD) 1493, (WORD)  -64,		// phase 20
	 (WORD) 1433, (WORD)  -69,		// phase 22
	 (WORD) 1370, (WORD)  -72,		// phase 24
	 (WORD) 1304, (WORD)  -74,		// phase 26
	 (WORD) 1236, (WORD)  -74,		// phase 28
	 (WORD) 1166, (WORD)  -73,		// phase 30
	 (WORD) 1095, (WORD)  -71,		// phase 32
	 (WORD) 1022, (WORD)  -68,		// phase 34
	 (WORD)  949, (WORD)  -64,		// phase 36
	 (WORD)  876, (WORD)  -59,		// phase 38
	 (WORD)  804, (WORD)  -54,		// phase 40
	 (WORD)  732, (WORD)  -48,		// phase 42
	 (WORD)  661, (WORD)  -42,		// phase 44
	 (WORD)  591, (WORD)  -36,		// phase 46
	 (WORD)  524, (WORD)  -30,		// phase 48
	 (WORD)  460, (WORD)  -24,		// phase 50
	 (WORD)  398, (WORD)  -19,		// phase 52
	 (WORD)  340, (WORD)  -14,		// phase 54
	 (WORD)  285, (WORD)   -9,		// phase 56
	 (WORD)  235, (WORD)   -5,		// phase 58
	 (WORD)  189, (WORD)   -2,		// phase 60
	 (WORD)  149, (WORD)   -1 },	// phase 62

// Spline, B=1.00, C=0.00, 2-tap, upper 32-phase
	{(WORD) 1365, (WORD)  341,		// phase 00
	 (WORD) 1364, (WORD)  310,		// phase 02
	 (WORD) 1358, (WORD)  281,		// phase 04
	 (WORD) 1348, (WORD)  254,		// phase 06
	 (WORD) 1335, (WORD)  229,		// phase 08
	 (WORD) 1319, (WORD)  205,		// phase 10
	 (WORD) 1300, (WORD)  183,		// phase 12
	 (WORD) 1278, (WORD)  163,		// phase 14
	 (WORD) 1253, (WORD)  144,		// phase 16
	 (WORD) 1226, (WORD)  127,		// phase 18
	 (WORD) 1197, (WORD)  111,		// phase 20
	 (WORD) 1165, (WORD)   96,		// phase 22
	 (WORD) 1131, (WORD)   83,		// phase 24
	 (WORD) 1096, (WORD)   71,		// phase 26
	 (WORD) 1059, (WORD)   61,		// phase 28
	 (WORD) 1021, (WORD)   51,		// phase 30
	 (WORD)  981, (WORD)   43,		// phase 32
	 (WORD)  941, (WORD)   35,		// phase 34
	 (WORD)  900, (WORD)   29,		// phase 36
	 (WORD)  858, (WORD)   23,		// phase 38
	 (WORD)  815, (WORD)   18,		// phase 40
	 (WORD)  773, (WORD)   14,		// phase 42
	 (WORD)  730, (WORD)   10,		// phase 44
	 (WORD)  687, (WORD)    8,		// phase 46
	 (WORD)  645, (WORD)    5,		// phase 48
	 (WORD)  604, (WORD)    3,		// phase 50
	 (WORD)  563, (WORD)    2,		// phase 52
	 (WORD)  523, (WORD)    1,		// phase 54
	 (WORD)  483, (WORD)    1,		// phase 56
	 (WORD)  446, (WORD)    0,		// phase 58
	 (WORD)  409, (WORD)    0,		// phase 60
	 (WORD)  374, (WORD)    0 }		// phase 62
};

static ROMDATA MDIN_MFCFILT_COEF EDK_MFCFilter_BSpline_More_Blur	= {
	{(WORD) 1707, (WORD)  171,		// phase 00
	 (WORD) 1703, (WORD)  140,		// phase 02
	 (WORD) 1693, (WORD)  113,		// phase 04
	 (WORD) 1677, (WORD)   88,		// phase 06
	 (WORD) 1655, (WORD)   65,		// phase 08
	 (WORD) 1627, (WORD)   46,		// phase 10
	 (WORD) 1594, (WORD)   28,		// phase 12
	 (WORD) 1556, (WORD)   13,		// phase 14
	 (WORD) 1515, (WORD)    0,		// phase 16
	 (WORD) 1469, (WORD)  -11,		// phase 18
	 (WORD) 1419, (WORD)  -20,		// phase 20
	 (WORD) 1366, (WORD)  -27,		// phase 22
	 (WORD) 1311, (WORD)  -33,		// phase 24
	 (WORD) 1253, (WORD)  -38,		// phase 26
	 (WORD) 1192, (WORD)  -40,		// phase 28
	 (WORD) 1130, (WORD)  -42,		// phase 30
	 (WORD) 1067, (WORD)  -43,		// phase 32
	 (WORD) 1002, (WORD)  -42,		// phase 34
	 (WORD)  937, (WORD)  -41,		// phase 36
	 (WORD)  872, (WORD)  -39,		// phase 38
	 (WORD)  807, (WORD)  -36,		// phase 40
	 (WORD)  742, (WORD)  -33,		// phase 42
	 (WORD)  678, (WORD)  -29,		// phase 44
	 (WORD)  615, (WORD)  -25,		// phase 46
	 (WORD)  555, (WORD)  -21,		// phase 48
	 (WORD)  496, (WORD)  -17,		// phase 50
	 (WORD)  439, (WORD)  -13,		// phase 52
	 (WORD)  385, (WORD)  -10,		// phase 54
	 (WORD)  335, (WORD)   -7,		// phase 56
	 (WORD)  287, (WORD)   -4,		// phase 58
	 (WORD)  244, (WORD)   -2,		// phase 60
	 (WORD)  205, (WORD)    0 },	// phase 62

// Spline, B=1.00, C=0.00, 2-tap, upper 32-phase
	{(WORD) 1365, (WORD)  341,		// phase 00
	 (WORD) 1364, (WORD)  310,		// phase 02
	 (WORD) 1358, (WORD)  281,		// phase 04
	 (WORD) 1348, (WORD)  254,		// phase 06
	 (WORD) 1335, (WORD)  229,		// phase 08
	 (WORD) 1319, (WORD)  205,		// phase 10
	 (WORD) 1300, (WORD)  183,		// phase 12
	 (WORD) 1278, (WORD)  163,		// phase 14
	 (WORD) 1253, (WORD)  144,		// phase 16
	 (WORD) 1226, (WORD)  127,		// phase 18
	 (WORD) 1197, (WORD)  111,		// phase 20
	 (WORD) 1165, (WORD)   96,		// phase 22
	 (WORD) 1131, (WORD)   83,		// phase 24
	 (WORD) 1096, (WORD)   71,		// phase 26
	 (WORD) 1059, (WORD)   61,		// phase 28
	 (WORD) 1021, (WORD)   51,		// phase 30
	 (WORD)  981, (WORD)   43,		// phase 32
	 (WORD)  941, (WORD)   35,		// phase 34
	 (WORD)  900, (WORD)   29,		// phase 36
	 (WORD)  858, (WORD)   23,		// phase 38
	 (WORD)  815, (WORD)   18,		// phase 40
	 (WORD)  773, (WORD)   14,		// phase 42
	 (WORD)  730, (WORD)   10,		// phase 44
	 (WORD)  687, (WORD)    8,		// phase 46
	 (WORD)  645, (WORD)    5,		// phase 48
	 (WORD)  604, (WORD)    3,		// phase 50
	 (WORD)  563, (WORD)    2,		// phase 52
	 (WORD)  523, (WORD)    1,		// phase 54
	 (WORD)  483, (WORD)    1,		// phase 56
	 (WORD)  446, (WORD)    0,		// phase 58
	 (WORD)  409, (WORD)    0,		// phase 60
	 (WORD)  374, (WORD)    0 }		// phase 62
};

//--------------------------------------------------------------------------------------------------------------------------
// Coefficients for FrontNR filter
static ROMDATA MDIN_FNRFILT_COEF EDK_FNRY1HDFilter[]	= {		// added on Jun07 2010
	{ {0x0302, 0x00DB, 0x0F64, 0x0040, 0x0FF9, 0x0FE0, 0x0024, 0x0003},	// 1280->720
	  {0x0400, 0x0000, 0x0000, 0x0000}	},								// C
	{ {0x028E, 0x0124, 0x0F87, 0x0FEA, 0x003F, 0x0FDC, 0x0003, 0x0006},
	  {0x0400, 0x0000, 0x0000, 0x0000}	},								// C
	{ {0x025A, 0x0136, 0x0FAB, 0x0FC0, 0x003F, 0x0000, 0x0FE2, 0x0011},
	  {0x0400, 0x0000, 0x0000, 0x0000}	},								// C
	{ {0x023C, 0x013E, 0x0FC9, 0x0FAC, 0x002F, 0x001C, 0x0FD7, 0x000D},
	  {0x0400, 0x0000, 0x0000, 0x0000}	},								// C
	{ {0x01F6, 0x0142, 0x000A, 0x0FA0, 0x0FFD, 0x002F, 0x0FF6, 0x0FF7},
	  {0x0400, 0x0000, 0x0000, 0x0000}	},								// C
	{ {0x01C2, 0x013F, 0x0038, 0x0FAA, 0x0FD5, 0x0026, 0x001D, 0x0FE6},
	  {0x0400, 0x0000, 0x0000, 0x0000}	},								// C
	{ {0x0190, 0x012F, 0x0062, 0x0FCA, 0x0FBF, 0x0003, 0x0024, 0x0FF7},	
	  {0x0400, 0x0000, 0x0000, 0x0000}	}								// C
};

static ROMDATA MDIN_FNRFILT_COEF EDK_FNRY2HDFilter[]	= {		// added on Jun08 2010
	{ {0x0302, 0x00DB, 0x0F64, 0x0040, 0x0FF9, 0x0FE0, 0x0024, 0x0003},	// 1280->720
	  {0x0400, 0x0000, 0x0000, 0x0000}	},								// C
	{ {0x02CE, 0x00FC, 0x0F6A, 0x001D, 0x001E, 0x0FD1, 0x0013, 0x0014},
	  {0x0400, 0x0000, 0x0000, 0x0000}	},								// C
	{ {0x02A4, 0x011E, 0x0F85, 0x0FFA, 0x003B, 0x0FDC, 0x0FEC, 0x000E},
	  {0x0400, 0x0000, 0x0000, 0x0000}	},								// C
	{ {0x0272, 0x0132, 0x0FA7, 0x0FD4, 0x003D, 0x0001, 0x0FE1, 0x0FFB},
	  {0x0400, 0x0000, 0x0000, 0x0000}	},								// C
	{ {0x024A, 0x013C, 0x0FC6, 0x0FB9, 0x0032, 0x001A, 0x0FE3, 0x0FF1},
	  {0x0400, 0x0000, 0x0000, 0x0000}	},								// C
	{ {0x0200, 0x013B, 0x0FFA, 0x0FA3, 0x0001, 0x0029, 0x0009, 0x0FF5},
	  {0x0400, 0x0000, 0x0000, 0x0000}	},								// C
};


#endif	/* __MFCCOEF_H__ */
