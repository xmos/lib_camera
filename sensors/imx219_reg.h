
// --------- REG definitions ---------------------------------------------------------

// Sleep values and registers
#define SLEEP 0xFFFF
#define TRSTUS 200

// CSI LANE
#define CSI_LANE_MODE_REG 0x0114
#define CSI_LANE_MODE_2_LANES 1 
#define CSI_LANE_MODE_4_LANES 3

// BINNING
#define REG_BINNING_MODE 0x0174
#define BINNING_NONE	0x0000
#define BINNING_2X2		0x0101

#define BINNING_MODE BINNING_2X2

// Gain params
#define GAIN_MIN_DB       0
#define GAIN_MAX_DB      84
#define GAIN_DEFAULT_DB  50

// --------- REG GROUP definitions ----------------------------------------------------
static imx219_settings_t imx219_common_regs[] = {
    {0x0103, 0x01},   /* software_reset       1, reset the chip */
    {SLEEP, TRSTUS},  /* software_reset       1, reset the chip */


	{0x0100, 0x00},	/* Mode Select */

	/* To Access Addresses 3000-5fff, send the following commands */
	{0x30eb, 0x0c},
	{0x30eb, 0x05},
	{0x300a, 0xff},
	{0x300b, 0xff},
	{0x30eb, 0x05},
	{0x30eb, 0x09},

	/* PLL Clock Table */
	{0x0301, 0x05},	/* VTPXCK_DIV */
	{0x0303, 0x01},	/* VTSYSCK_DIV */
	{0x0304, 0x03},	/* PREPLLCK_VT_DIV 0x03 = AUTO set */
	{0x0305, 0x03}, /* PREPLLCK_OP_DIV 0x03 = AUTO set */
	{0x0306, 0x00},	/* PLL_VT_MPY */
	{0x0307, 0x39},
	{0x030b, 0x01},	/* OP_SYS_CLK_DIV */
	{0x030c, 0x00},	/* PLL_OP_MPY */
	{0x030d, 0x72},

	/* Undocumented registers */
	{0x455e, 0x00},
	{0x471e, 0x4b},
	{0x4767, 0x0f},
	{0x4750, 0x14},
	{0x4540, 0x00},
	{0x47b4, 0x14},
	{0x4713, 0x30},
	{0x478b, 0x10},
	{0x478f, 0x10},
	{0x4793, 0x10},
	{0x4797, 0x0e},
	{0x479b, 0x0e},

	/* Frame Bank Register Group "A" */
	{0x0162, 0x0d},	/* Line_Length_A */
	{0x0163, 0x78},
	{0x0170, 0x01}, /* X_ODD_INC_A */
	{0x0171, 0x01}, /* Y_ODD_INC_A */

	/* Output setup registers */
	{0x0114, 0x01},	/* CSI 2-Lane Mode */
	{0x0128, 0x00},	/* DPHY Auto Mode */
	{0x012a, 0x18},	/* EXCK_Freq */
	{0x012b, 0x00},
};




static imx219_settings_t imx219_lanes_regs[] = {
    {CSI_LANE_MODE_REG, CSI_LANE_MODE_2_LANES}
};



static imx219_settings_t mode_640_480_regs[] = {
	{0x0164, 0x03},
	{0x0165, 0xe8},
	{0x0166, 0x08},
	{0x0167, 0xe7},
	{0x0168, 0x02},
	{0x0169, 0xf0},
	{0x016a, 0x06},
	{0x016b, 0xaf},
	{0x016c, 0x02},
	{0x016d, 0x80},
	{0x016e, 0x01},
	{0x016f, 0xe0},
	{0x0624, 0x06},
	{0x0625, 0x68},
	{0x0626, 0x04},
	{0x0627, 0xd0},
};


static imx219_settings_t raw10_framefmt_regs[] = {
	{0x018c, 0x0a},
	{0x018d, 0x0a},
	{0x0309, 0x0a},
};


static imx219_settings_t binning_regs[] = {
	{REG_BINNING_MODE, BINNING_MODE}
};


static imx219_settings_t start[] = {
    {0x0100, 0x01}, /* mode select streaming on */
};

static imx219_settings_t stop[] = {
    {0x0100, 0x00}, /* mode select streaming off */
};

static imx219_settings_t start_regs[] = {
    {0x0100, 0x01}, /* mode select streaming on */
};

static imx219_settings_t stop_regs[] = {
    {0x0100, 0x00}, /* mode select streaming off */
};



// GAIN related settings
#define INTEGRATION_TIMES 41
#define ANALOGUE_GAINS 20
#define DIGITAL_GAINS 25

static uint16_t gain_integration_times[INTEGRATION_TIMES] = {
    0x00a,
    0x00b,
    0x00c,
    0x00e,
    0x010,
    0x012,
    0x014,
    0x016,
    0x019,
    0x01c,
    0x020,
    0x024,
    0x028,
    0x02d,
    0x033,
    0x039,
    0x040,
    0x048,
    0x051,
    0x05b,
    0x066,
    0x072,
    0x080,
    0x090,
    0x0a2,
    0x0b6,
    0x0cc,
    0x0e5,
    0x101,
    0x120,
    0x143,
    0x16b,
    0x197,
    0x1c9,
    0x201,
    0x23f,
    0x286,
    0x2d4,
    0x32d,
    0x390,
    0x400,
};

static uint8_t gain_analogue_gains[ANALOGUE_GAINS + 1] = {
    0,
    28,
    53,
    75,
    95,
    112,
    128,
    142,
    155,
    166,
    175,
    184,
    192,
    199,
    205,
    211,
    215,
    220,
    224,
    227,
    231,
};

static uint16_t gain_digital_gains[DIGITAL_GAINS + 1] = {
    0x0100,
    0x011f,
    0x0142,
    0x0169,
    0x0195,
    0x01c7,
    0x01fe,
    0x023d,
    0x0283,
    0x02d1,
    0x0329,
    0x038c,
    0x03fb,
    0x0477,
    0x0503,
    0x059f,
    0x064f,
    0x0714,
    0x07f1,
    0x08e9,
    0x0a00,
    0x0b38,
    0x0c96,
    0x0e20,
    0x0fd9,
};


