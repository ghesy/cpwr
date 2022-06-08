/* NOTE: wildcard patterns in pathnames are supported. */

/* ========================================================================
 *  settings that are applied to already existing and newly connected HDDs
 * ======================================================================== */

/* set hard drives' advanced power management parameter to this value.
 * see hdparm(8)'s -B option for more info */
static const unsigned int hdparm_apm = 64;

/* set hard drives' readahead in kilobytes to this value */
static const unsigned int readahead = 4096;

/* ===================================
 *  settings that are applied on boot
 * =================================== */

static struct Config init[] = {

	/* enable laptop mode */
	{ "5", "/proc/sys/vm/laptop_mode" },

	/* decrease disk IO frequency */
	{ "60000", "/proc/sys/vm/dirty_writeback_centisecs" },
	{ "60000", "/proc/sys/vm/dirty_expire_centisecs" },
	{ "5",     "/proc/sys/vm/dirty_background_ratio" },
	{ "50",    "/proc/sys/vm/dirty_ratio" },

	/* enable sata powersaving */
	{ "auto", "/sys/class/ata_port/ata[0-9]*/power/control" },
	{ "auto", "/sys/block/sd[a-z]*/device/power/control" },
};

/* ===========================================================
 *  settings that are applied upon laptop charger plug/unplug
 * =========================================================== */

#define CPU "/sys/devices/system/cpu"
#define NOTURBO CPU "/intel_pstate/no_turbo"
#define PERFORMANCE CPU "/cpufreq/policy"

static struct Config powersave[] = {
	{ "balance_power", PERFORMANCE },
	{ "1", NOTURBO },
};

static struct Config performance[] = {
	{ "balance_performance", PERFORMANCE },
	{ "0", NOTURBO },
};

/* ================================================================
 *  settings that are applied when the battery level gets very low
 * ================================================================ */

/* apply the following settings upon battery
 * percentage dropping to this value */
static const unsigned int low_battery_threshold = 8;

static struct Config lowbattery[] = {

	/* disable laptop mode */
	{ "0", "/proc/sys/vm/laptop_mode" },

	/* reset disk IO frequency */
	{ "1000", "/proc/sys/vm/dirty_writeback_centisecs" },
	{ "3000", "/proc/sys/vm/dirty_expire_centisecs" },
	{ "10",   "/proc/sys/vm/dirty_background_ratio" },
	{ "20",   "/proc/sys/vm/dirty_ratio" },
};

/* ================
 *  other settings
 * ================ */

static const unsigned int write_max_tries = 2;
static const unsigned int write_interval = 1;
/* static const unsigned int write_max_tries = 200; */
/* static const unsigned int write_interval = 2; */
