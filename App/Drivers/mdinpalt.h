// This is 256-color palette mode.
/*static ROMDATA WORD mdin3xx_palt_rgb_pal[][2]	= {
//  [   a       R       G       B ],	    ID { a   R    G    B }
	{15<<12 | 0x40, 0x40<<8 | 0x80},	// 000 {15,  64,  64, 128}
	{15<<12 | 0x20, 0x20<<8 | 0x40},	// 001 {15,  32,  32,  64}
	{15<<12 | 0x40, 0x60<<8 | 0x80},	// 002 {15,  64,  96, 128}
	{15<<12 | 0x40, 0x80<<8 | 0x80},	// 003 {15,  64, 128, 128}
	{15<<12 | 0x40, 0x80<<8 | 0xC0},	// 004 {15,  64, 128, 192}
	{15<<12 | 0x40, 0x60<<8 | 0xC0},	// 005 {15,  64,  96, 192}
	{15<<12 | 0x60, 0xA0<<8 | 0xC0},	// 006 {15,  96, 160, 192}
	{15<<12 | 0x40, 0xA0<<8 | 0xC0},	// 007 {15,  64, 160, 192}
	{15<<12 | 0x60, 0x80<<8 | 0xC0},	// 008 {15,  96, 128, 192}
	{15<<12 | 0x20, 0x80<<8 | 0xC0},	// 009 {15,  32, 128, 192}
	{15<<12 | 0xFF, 0xFB<<8 | 0xF0},	// 010 {15, 255, 251, 240}
	{15<<12 | 0xFF, 0xFF<<8 | 0xFF},	// 011 {15, 255, 255, 255}
	{15<<12 | 0x20, 0x40<<8 | 0x40},	// 012 {15,  32,  64,  64}
	{15<<12 | 0x20, 0x40<<8 | 0x80},	// 013 {15,  32,  64, 128}
	{15<<12 | 0xA6, 0xCA<<8 | 0xF0},	// 014 {15, 166, 202, 240}
	{15<<12 | 0x80, 0xC0<<8 | 0xC0},	// 015 {15, 128, 192, 192}
	{15<<12 | 0x20, 0x60<<8 | 0xC0},	// 016 {15,  32,  96, 192}
	{15<<12 | 0xA0, 0xC0<<8 | 0xC0},	// 017 {15, 160, 192, 192}
	{15<<12 | 0x80, 0xA0<<8 | 0xC0},	// 018 {15, 128, 160, 192}
	{15<<12 | 0xC0, 0xDC<<8 | 0xC0},	// 019 {15, 192, 220, 192}
	{15<<12 | 0x60, 0x60<<8 | 0x80},	// 020 {15,  96,  96, 128}
	{15<<12 | 0xC0, 0xC0<<8 | 0xC0},	// 021 {15, 192, 192, 192}
	{15<<12 | 0x80, 0x80<<8 | 0x80},	// 022 {15, 128, 128, 128}
	{15<<12 | 0x60, 0x60<<8 | 0x40},	// 023 {15,  96,  96,  64}
	{15<<12 | 0x40, 0x40<<8 | 0x40},	// 024 {15,  64,  64,  64}
	{15<<12 | 0x00, 0x00<<8 | 0x00},	// 025 {15,   0,   0,   0}
	{15<<12 | 0xA0, 0xA0<<8 | 0xA4},	// 026 {15, 160, 160, 164}
	{15<<12 | 0xA0, 0xA0<<8 | 0xC0},	// 027 {15, 160, 160, 192}
	{15<<12 | 0x40, 0x20<<8 | 0x00},	// 028 {15,  64,  32,   0}
	{15<<12 | 0x00, 0x20<<8 | 0x40},	// 029 {15,   0,  32,  64}
	{15<<12 | 0x60, 0xC0<<8 | 0xC0},	// 030 {15,  96, 192, 192}
	{15<<12 | 0x20, 0x40<<8 | 0x00},	// 031 {15,  32,  64,   0}
	{15<<12 | 0x40, 0xC0<<8 | 0xC0},	// 032 {15,  64, 192, 192}
	{15<<12 | 0x60, 0x80<<8 | 0x80},	// 033 {15,  96, 128, 128}
	{15<<12 | 0xA0, 0xA0<<8 | 0x80},	// 034 {15, 160, 160, 128}
	{15<<12 | 0xA0, 0xC0<<8 | 0x80},	// 035 {15, 160, 192, 128}
	{15<<12 | 0x60, 0xA0<<8 | 0x80},	// 036 {15,  96, 160, 128}
	{15<<12 | 0x40, 0x60<<8 | 0x40},	// 037 {15,  64,  96,  64}
	{15<<12 | 0x20, 0x60<<8 | 0x80},	// 038 {15,  32,  96, 128}
	{15<<12 | 0x80, 0xA0<<8 | 0x80},	// 039 {15, 128, 160, 128}
	{15<<12 | 0x40, 0x40<<8 | 0xC0},	// 040 {15,  64,  64, 192}
	{15<<12 | 0x20, 0x00<<8 | 0x40},	// 041 {15,  32,   0,  64}
	{15<<12 | 0x60, 0x80<<8 | 0x40},	// 042 {15,  96, 128,  64}
	{15<<12 | 0xC0, 0xE0<<8 | 0x80},	// 043 {15, 192, 224, 128}
	{15<<12 | 0x80, 0x80<<8 | 0x40},	// 044 {15, 128, 128,  64}
	{15<<12 | 0xC0, 0xC0<<8 | 0x80},	// 045 {15, 192, 192, 128}
	{15<<12 | 0x60, 0x60<<8 | 0xC0},	// 046 {15,  96,  96, 192}
	{15<<12 | 0xE0, 0xE0<<8 | 0x40},	// 047 {15, 224, 224,  64}
	{15<<12 | 0xE0, 0xE0<<8 | 0x80},	// 048 {15, 224, 224, 128}
	{15<<12 | 0xA0, 0xE0<<8 | 0x40},	// 049 {15, 160, 224,  64}
	{15<<12 | 0xA0, 0xC0<<8 | 0x40},	// 050 {15, 160, 192,  64}
	{15<<12 | 0xC0, 0xE0<<8 | 0x40},	// 051 {15, 192, 224,  64}
	{15<<12 | 0x80, 0xC0<<8 | 0x40},	// 052 {15, 128, 192,  64}
	{15<<12 | 0xC0, 0xC0<<8 | 0x40},	// 053 {15, 192, 192,  64}
	{15<<12 | 0x80, 0xA0<<8 | 0x40},	// 054 {15, 128, 160,  64}
	{15<<12 | 0x60, 0xC0<<8 | 0x40},	// 055 {15,  96, 192,  64}
	{15<<12 | 0x60, 0x40<<8 | 0x40},	// 056 {15,  96,  64,  64}
	{15<<12 | 0x40, 0x40<<8 | 0x00},	// 057 {15,  64,  64,   0}
	{15<<12 | 0x80, 0x60<<8 | 0x80},	// 058 {15, 128,  96, 128}
	{15<<12 | 0x40, 0x20<<8 | 0x40},	// 059 {15,  64,  32,  64}
	{15<<12 | 0x00, 0x00<<8 | 0x40},	// 060 {15,   0,   0,  64}
	{15<<12 | 0xA0, 0xE0<<8 | 0x80},	// 061 {15, 160, 224, 128}
	{15<<12 | 0x40, 0x80<<8 | 0x40},	// 062 {15,  64, 128,  64}
	{15<<12 | 0x20, 0xA0<<8 | 0x40},	// 063 {15,  32, 160,  64}
	{15<<12 | 0xA0, 0x80<<8 | 0x80},	// 064 {15, 160, 128, 128}
	{15<<12 | 0x60, 0xA0<<8 | 0x40},	// 065 {15,  96, 160,  64}
	{15<<12 | 0x20, 0x20<<8 | 0x80},	// 066 {15,  32,  32, 128}
};

static ROMDATA WORD mdin3xx_palt_yuv_pal[][2]	= {
//  [   a      Cr       Y      Cb ],	    ID { a   Y    Cb   Cr}
	{15<<12 | 0x7B, 0x4D<<8 | 0x9C},	// 000 {15, 123,  77, 156}
	{15<<12 | 0x7E, 0x2F<<8 | 0x8E},	// 001 {15, 126,  47, 142}
	{15<<12 | 0x70, 0x5D<<8 | 0x93},	// 002 {15, 112,  93, 147}
	{15<<12 | 0x64, 0x6E<<8 | 0x89},	// 003 {15, 100, 110, 137}
	{15<<12 | 0x5F, 0x74<<8 | 0xA6},	// 004 {15,  95, 116, 166}
	{15<<12 | 0x6B, 0x64<<8 | 0xAF},	// 005 {15, 107, 100, 175}
	{15<<12 | 0x62, 0x8C<<8 | 0x98},	// 006 {15,  98, 140, 152}
	{15<<12 | 0x54, 0x84<<8 | 0x9C},	// 007 {15,  84, 132, 156}
	{15<<12 | 0x6D, 0x7C<<8 | 0xA1},	// 008 {15, 109, 124, 161}
	{15<<12 | 0x51, 0x6C<<8 | 0xAA},	// 009 {15,  81, 108, 170}
	{15<<12 | 0x83, 0xE8<<8 | 0x7B},	// 010 {15, 131, 232, 123}
	{15<<12 | 0x80, 0xEB<<8 | 0x80},	// 011 {15, 128, 235, 128}
	{15<<12 | 0x72, 0x3F<<8 | 0x85},	// 012 {15, 114,  63, 133}
	{15<<12 | 0x6D, 0x45<<8 | 0xA1},	// 013 {15, 109,  69, 161}
	{15<<12 | 0x6D, 0xB8<<8 | 0x96},	// 014 {15, 109, 184, 150}
	{15<<12 | 0x64, 0xA4<<8 | 0x89},	// 015 {15, 100, 164, 137}
	{15<<12 | 0x5D, 0x5B<<8 | 0xB4},	// 016 {15,  93,  91, 180}
	{15<<12 | 0x72, 0xAD<<8 | 0x85},	// 017 {15, 114, 173, 133}
	{15<<12 | 0x70, 0x94<<8 | 0x93},	// 018 {15, 112, 148, 147}
	{15<<12 | 0x76, 0xC3<<8 | 0x78},	// 019 {15, 118, 195, 120}
	{15<<12 | 0x7E, 0x66<<8 | 0x8E},	// 020 {15, 126, 102, 142}
	{15<<12 | 0x80, 0xB5<<8 | 0x80},	// 021 {15, 128, 181, 128}
	{15<<12 | 0x80, 0x7E<<8 | 0x80},	// 022 {15, 128, 126, 128}
	{15<<12 | 0x82, 0x5F<<8 | 0x72},	// 023 {15, 130,  95, 114}
	{15<<12 | 0x80, 0x47<<8 | 0x80},	// 024 {15, 128,  71, 128}
	{15<<12 | 0x80, 0x10<<8 | 0x80},	// 025 {15, 128,  16, 128}
	{15<<12 | 0x80, 0x9A<<8 | 0x82},	// 026 {15, 128, 154, 130}
	{15<<12 | 0x7E, 0x9D<<8 | 0x8E},	// 027 {15, 126, 157, 142}
	{15<<12 | 0x90, 0x31<<8 | 0x6D},	// 028 {15, 144,  49, 109}
	{15<<12 | 0x70, 0x26<<8 | 0x93},	// 029 {15, 112,  38, 147}
	{15<<12 | 0x56, 0x9C<<8 | 0x8E},	// 030 {15,  86, 156, 142}
	{15<<12 | 0x76, 0x38<<8 | 0x69},	// 031 {15, 118,  56, 105}
	{15<<12 | 0x48, 0x94<<8 | 0x93},	// 032 {15,  72, 148, 147}
	{15<<12 | 0x72, 0x76<<8 | 0x85},	// 033 {15, 114, 118, 133}
	{15<<12 | 0x82, 0x96<<8 | 0x72},	// 034 {15, 130, 150, 114}
	{15<<12 | 0x76, 0xA6<<8 | 0x69},	// 035 {15, 118, 166, 105}
	{15<<12 | 0x66, 0x86<<8 | 0x7B},	// 036 {15, 102, 134, 123}
	{15<<12 | 0x74, 0x57<<8 | 0x77},	// 037 {15, 116,  87, 119}
	{15<<12 | 0x62, 0x55<<8 | 0x98},	// 038 {15,  98,  85, 152}
	{15<<12 | 0x74, 0x8E<<8 | 0x77},	// 039 {15, 116, 142, 119}
	{15<<12 | 0x77, 0x54<<8 | 0xB8},	// 040 {15, 119,  84, 184}
	{15<<12 | 0x8A, 0x1E<<8 | 0x97},	// 041 {15, 138,  30, 151}
	{15<<12 | 0x76, 0x6F<<8 | 0x69},	// 042 {15, 118, 111, 105}
	{15<<12 | 0x79, 0xBF<<8 | 0x5B},	// 043 {15, 121, 191,  91}
	{15<<12 | 0x85, 0x78<<8 | 0x64},	// 044 {15, 133, 120, 100}
	{15<<12 | 0x85, 0xAF<<8 | 0x64},	// 045 {15, 133, 175, 100}
	{15<<12 | 0x79, 0x6C<<8 | 0xAA},	// 046 {15, 121, 108, 170}
	{15<<12 | 0x8B, 0xC1<<8 | 0x3A},	// 047 {15, 139, 193,  58}
	{15<<12 | 0x87, 0xC7<<8 | 0x56},	// 048 {15, 135, 199,  86}
	{15<<12 | 0x6F, 0xB0<<8 | 0x43},	// 049 {15, 111, 176,  67}
	{15<<12 | 0x7B, 0xA0<<8 | 0x4D},	// 050 {15, 123, 160,  77}
	{15<<12 | 0x7D, 0xB9<<8 | 0x3E},	// 051 {15, 125, 185,  62}
	{15<<12 | 0x6D, 0x98<<8 | 0x51},	// 052 {15, 109, 152,  81}
	{15<<12 | 0x89, 0xA8<<8 | 0x48},	// 053 {15, 137, 168,  72}
	{15<<12 | 0x79, 0x88<<8 | 0x5B},	// 054 {15, 121, 136,  91}
	{15<<12 | 0x5F, 0x90<<8 | 0x56},	// 055 {15,  95, 144,  86}
	{15<<12 | 0x8E, 0x4F<<8 | 0x7B},	// 056 {15, 142,  79, 123}
	{15<<12 | 0x85, 0x41<<8 | 0x64},	// 057 {15, 133,  65, 100}
	{15<<12 | 0x8C, 0x6E<<8 | 0x89},	// 058 {15, 140, 110, 137}
	{15<<12 | 0x8C, 0x37<<8 | 0x89},	// 059 {15, 140,  55, 137}
	{15<<12 | 0x7B, 0x16<<8 | 0x9C},	// 060 {15, 123,  22, 156}
	{15<<12 | 0x6B, 0xB7<<8 | 0x5F},	// 061 {15, 107, 183,  95}
	{15<<12 | 0x68, 0x67<<8 | 0x6D},	// 062 {15, 104, 103, 109}
	{15<<12 | 0x4F, 0x6F<<8 | 0x69},	// 063 {15,  79, 111, 105}
	{15<<12 | 0x8E, 0x86<<8 | 0x7B},	// 064 {15, 142, 134, 123}
	{15<<12 | 0x6B, 0x80<<8 | 0x5F},	// 065 {15, 107, 128,  95}
	{15<<12 | 0x79, 0x35<<8 | 0xAA},	// 066 {15, 121,  53, 170}
};
*/

