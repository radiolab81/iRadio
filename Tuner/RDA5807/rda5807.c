/*
 * RDA5807 FM radion module for Raspberry pi
 * compile with gcc -lm -lwiringPi -o rda5807 rda5807.c
 */
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <libgen.h>

enum rda5807_reg {
	RDA5807_REG_CHIPID			= 0x00,
	RDA5807_REG_CTRL			= 0x02,
	RDA5807_REG_CHAN			= 0x03,
	RDA5807_REG_IOCFG			= 0x04,
	RDA5807_REG_INTM_THRESH_VOL		= 0x05,
	RDA5807_REG_BLEND			= 0x07,
	RDA5807_REG_SEEK_RESULT			= 0x0A,
	RDA5807_REG_SIGNAL			= 0x0B,
	RDA5807_REG_RDSA			= 0x0C,
	RDA5807_REG_RDSB			= 0x0D,
	RDA5807_REG_RDSC			= 0x0E,
	RDA5807_REG_RDSD			= 0x0F
};

#define FREQ_MIN_MHZ  76.0
#define FREQ_MAX_MHZ 108.0

#define SIGNAL_WAIT_TIME 400000
#define READY_WAIT_TIME 2000
#define MAX_STATION 256
#define RADIO_STATION_INFO "/var/local/radio/radio_station"
#define TUNED_FREQ "/var/local/radio/tuned_freq"
#define MAX_STATION_NAME_LEN 128

#define CHIP_ID 0x58
#define BAND_SPACE 0x08	// 76–108 MHz (world wide), 100 kHz
#define BAND_START FREQ_MIN_MHZ	// 76.0 MHz
#define SPACE_MUL 10	// 100kHz:10, 200kHz:5, 50kHz:20, 25kHz:40

int fd16, fd8;
int dwID = 0x10; // i2c Channel the device is on
int drID = 0x11; // i2c Channel the device is on
unsigned char frequencyH = 0;
unsigned char frequencyL = 0;
unsigned int frequencyB;

char *prog_name;

int	station_info_num = 0;
struct _station_info {
	double freq;
	char name[MAX_STATION_NAME_LEN];
} station_info[MAX_STATION];

int read_swap = 1;

u_int16_t i2c_read(int offset)
{
	u_int16_t data = wiringPiI2CReadReg16(fd16, offset);
	if (read_swap) data = (data>>8) | (data<<8);
	return data;
}

int i2c_write(int offset, u_int16_t data)
{
	if (read_swap) data = (data>>8) | (data<<8);
	return wiringPiI2CWriteReg16(fd16, offset, data);
}

u_int16_t read_volume()
{
	return i2c_read(RDA5807_REG_INTM_THRESH_VOL) & 0b1111;
}

u_int16_t read_rssi()
{
	return i2c_read(RDA5807_REG_SIGNAL) & 0xfe00;
}

u_int16_t read_stc()
{
	return i2c_read(RDA5807_REG_SEEK_RESULT) & 0x4000;
}

u_int16_t read_st()
{
	return i2c_read(RDA5807_REG_SEEK_RESULT) & 0x0400;
}

int get_band()
{
	static int band = -1;

	if (band != -1)
		return band;
	band = (i2c_read(RDA5807_REG_CHAN) & 0b1100) >> 2;
	return band;
}

int get_space()
{
	static int space = -1;

	if (space != -1)
		return space;
	space = i2c_read(RDA5807_REG_CHAN) & 0b0011;
	return space;
}

double get_freq()
{
	return ((double)(i2c_read(RDA5807_REG_SEEK_RESULT) & 0x3ff) + BAND_START * 10) / SPACE_MUL;
}

int wait_ready(void)
{
	u_int16_t data;
	int loop = 0;

	data = read_stc();
	while(!data && loop < 2000000/READY_WAIT_TIME) { // max 2 sec
		usleep(READY_WAIT_TIME);
		data = read_stc();
		loop++;
	}

	if (data == 0)	{ // Ready fail
		return -1;
	}

	return 0;
}

int find_station_info(double freq)
{
	int i;

	for (i = 0; i < station_info_num; i++)
		if (freq == station_info[i].freq) return i;

	return -1;
}

int save_freq(double freq)
{
	FILE *fd;
	char s_freq[64];

	printf("save freq=%3.1f\n", freq);
	if (freq < FREQ_MIN_MHZ  || freq > FREQ_MAX_MHZ) return -1;

	if ((fd = fopen(TUNED_FREQ, "w")) == NULL)
		return -1;

	sprintf(s_freq, "%3.1f", freq);
	fputs(s_freq, fd);
	fclose(fd);

	return 0;
}

void get_status(double *freq, u_int8_t *mono, u_int8_t *bass, u_int8_t *mute, u_int8_t *vol, u_int8_t *level_adc)
{
	u_int16_t data;

	*freq = get_freq();
	data = i2c_read(RDA5807_REG_CTRL);
	*mono = (data & 0x2000) ? 1 : 0;
	*bass = (data & 0x1000) ? 1 : 0;
	*mute = (data & 0x4000) ? 0 : 1;
	*vol = (u_int8_t)i2c_read(RDA5807_REG_INTM_THRESH_VOL) & 0x0f;
	*level_adc = read_rssi() >> 9;
}

void print_status(void)
{
	double freq;
	u_int8_t mono, bass, mute, vol, level_adc;
	int	preset;

	get_status(&freq, &mono, &bass, &mute, &vol, &level_adc);
	printf("%3.1f MHz %s, %s, %s, volume:%d, Signal Strength:%d/63\n", freq,
			mono ? "mono" : "stereo",
			bass ? "bass boost" : "bass disabled",
			mute ? "mute" : "unmute",
			vol, level_adc);
	preset = find_station_info(freq);
	if (preset >= 0)
		printf("%s [%d/%d]\n", station_info[preset].name, preset + 1, station_info_num);
}

void set_bass(int mode)
{
	u_int16_t data;

	data = i2c_read(RDA5807_REG_CTRL);
	if (mode) data |= 0x1000;
	else data &= ~0x1000;
	i2c_write(RDA5807_REG_CTRL, data);
}

void set_afc(int mode)
{
	u_int16_t data;

	data = i2c_read(RDA5807_REG_IOCFG);
	if (mode) data &= ~0x0100;
	else data |= 0x0100;
	i2c_write(RDA5807_REG_IOCFG, data);
}

void set_deemphasis(int mode)
{
	u_int16_t data;

	data = i2c_read(RDA5807_REG_IOCFG);
	if (mode) data &= ~0x0800;
	else data |= 0x0800;
	i2c_write(RDA5807_REG_IOCFG, data);
}

void set_mono(int mode)
{
	u_int16_t data;

	data = i2c_read(RDA5807_REG_CTRL);
	if (mode) data |= 0x2000;
	else data &= ~0x2000;
	i2c_write(RDA5807_REG_CTRL, data);
}

void set_mute(int mode)
{
	u_int16_t data;

	data = i2c_read(RDA5807_REG_CTRL);
	if (mode) data &= ~0x4000;
	else data |= 0x4000;
	i2c_write(RDA5807_REG_CTRL, data);
}

void set_off()
{
	i2c_write(RDA5807_REG_CTRL, i2c_read(RDA5807_REG_CTRL) & 0xfffe);
}

int set_freq(double freq)
{
	u_int16_t data;
	u_int8_t wdata[4];

	data = i2c_read(2);
	data |= 0xc000; // 02H
	data &= ~0x10;
	wdata[0] = data >> 8;
	wdata[1] = data & 0xff;
	data = freq * SPACE_MUL - BAND_START * SPACE_MUL; // 03H, Freq
	data = data << 6 | 0x10 | BAND_SPACE; // 03H, Tune, band, space
	wdata[2] = data >> 8;
	wdata[3] = data & 0xff;
	write(fd8, wdata, 4);

	save_freq(freq);

	if (wait_ready() < 0) {
		fprintf(stderr, "Fail to tune!\n");
		return -1;
	}
	return 0;
}

int set_volume(int vol)
{
	u_int16_t data;

	if (vol < 0 || vol > 15)
		return -1;

	data = i2c_read(RDA5807_REG_INTM_THRESH_VOL);
	data &= 0xfff0;
	data |= vol;
	i2c_write(RDA5807_REG_INTM_THRESH_VOL, data);

	return 0;
}

int search(int dir)
{
	u_int16_t data;
	double freq;

	data = i2c_read(RDA5807_REG_CTRL);
	data |= 0xc380; // 02H
	if (!dir) data &= ~0x0200;
	i2c_write(2, data);

	while (wait_ready() < 0) {
		freq = get_freq();
		if (freq <= FREQ_MIN_MHZ || freq >= FREQ_MAX_MHZ)
			return -1;
	}

	return 0;
}

void freq_scan()
{
	double freq = FREQ_MIN_MHZ;
	u_int8_t mono, bass, mute, vol, level_adc;
	int count = 0;
	struct _radio_station {
		double freq;
		unsigned char stereo;
	} radio_station[MAX_STATION];

	set_freq(freq);
	wait_ready();
	do {
		if (search(1)) break;
		usleep(SIGNAL_WAIT_TIME);
		get_status(&freq, &mono, &bass, &mute, &vol, &level_adc);
		if (freq >= FREQ_MAX_MHZ) break;
		radio_station[count].freq = freq;
		radio_station[count].stereo = !mono;
		printf("%3.1f MHz, Signal Strength:%d/63\n", freq, level_adc);
		count++;
	} while(freq < FREQ_MAX_MHZ && count < MAX_STATION);

	printf("Total %d radio station%c\n", count, count > 1 ? 's': 0);
	if (count > 0)
		set_freq(radio_station[0].freq);
}

int get_station_info()
{
	FILE *fd;
	size_t len = 0;
	int i, si, n;
	double freq;
	char *line = NULL;
	char s_freq[64];

	station_info_num = 0;
	if ((fd = fopen(RADIO_STATION_INFO, "r")) == NULL)
		return 0;

	while((n = getline(&line, &len, fd)) != EOF) {
		if (line[0] == '#') continue;
		i = 0;

		// get station frequency
		while(isspace(line[i])) i++;
		si = i;
		while(!isspace(line[i])) i++;
		strncpy(s_freq, &line[si], i - si);
		s_freq[i] = 0;
		freq = strtod(s_freq, NULL);
		if (freq < FREQ_MIN_MHZ || freq > FREQ_MAX_MHZ)
			continue;

		station_info[station_info_num].freq = freq;

		// get station name
		while(isspace(line[i])) i++;
		si = i;
		while(line[i] != 0x0d && line[i] != 0x0a && line[i] != 0 &&
				(i - si) < MAX_STATION_NAME_LEN) i++;
		strncpy(station_info[station_info_num].name, &line[si], i - si);
		station_info[station_info_num].name[i - si] = 0;

		station_info_num++;
	}

	if (line) free(line);
	fclose(fd);

	return station_info_num;
}

int get_afc(void)
{
	return i2c_read(RDA5807_REG_IOCFG) & 0x0100;
}

int get_deemphasis(void)
{
	return i2c_read(RDA5807_REG_IOCFG) & 0x0800;
}

int get_bass()
{
	return i2c_read(RDA5807_REG_CTRL) & 0x1000;
}

int get_volume()
{
	return i2c_read(RDA5807_REG_INTM_THRESH_VOL) & 0x0f;
}

double get_tuned_freq(void)
{
	FILE *fd;
	char s_freq[16];
	double freq;

	if ((fd = fopen(TUNED_FREQ, "r")) == NULL)
		return 0;

	fscanf(fd, "%s", s_freq);
	freq = strtod(s_freq, NULL);
	fclose(fd);

	if (freq < FREQ_MIN_MHZ || freq > FREQ_MAX_MHZ)
		return 0;

	return freq;
}

void preset_move(int dir)
{
	int preset;

	if (station_info_num <= 0) return;

	preset = find_station_info(get_freq());
	if (preset >= 0) {
		if (dir) {
			if (++preset >= station_info_num) preset = 0;
		}
		else {
			if (--preset < 0) preset = station_info_num - 1;
		}
	}
	else preset = 1;

	set_freq(station_info[preset].freq);
}

void usage()
{
	fprintf(stderr, "Usage:\t%s [frequency|preset]\n", prog_name);
	fprintf(stderr, "\t%s status\n", prog_name);
	fprintf(stderr, "\t%s prev|next\n", prog_name);
	fprintf(stderr, "\t%s scan\n", prog_name);
	fprintf(stderr, "\t%s up|down\n", prog_name);
	fprintf(stderr, "\t%s stepup|stepdown\n", prog_name);
	fprintf(stderr, "\t%s stereo|mono\n", prog_name);
	fprintf(stderr, "\t%s mute|unmute|on|off\n", prog_name);
	fprintf(stderr, "\t%s afc [on|off]\n", prog_name);
	fprintf(stderr, "\t%s bass [on|off]\n", prog_name);
	fprintf(stderr, "\t%s deemphasis [75|50]\n", prog_name);
	fprintf(stderr, "\t%s volume [up|down]|[0~15]\n", prog_name);
	fprintf(stderr, "\t%s noise_blend [0~31]\n", prog_name);
	exit(1);
}

int init(void)
{
	u_int16_t data;
	u_int8_t wdata[12];

	if ((fd16 = wiringPiI2CSetup(drID)) < 0) {
		fprintf(stderr, "error opening i2c channel %d\n", drID);
		exit(2);
	}

	if ((fd8 = wiringPiI2CSetup(dwID)) < 0) {
		fprintf(stderr, "error opening i2c channel %d\n", dwID);
		exit(2);
	}

	data = wiringPiI2CReadReg16(fd16, 0);
	if ((data & 0xff) == CHIP_ID)
		read_swap = 1;
	else if ((data >> 8) == CHIP_ID)
		read_swap = 0;
	else {
		fprintf(stderr, "RDA5807 not found\n");
		exit(2);
	}

	if (i2c_read(RDA5807_REG_RDSB) == 0x5804 && i2c_read(RDA5807_REG_RDSD) == 0x5804) {
		data = 0x800d;	//02H, Audio Output: normal, RDS/RBDS enable, RDS/RBDS enable, Power Up Enable
		wdata[0] = data >> 8;
		wdata[1] = data & 0xff;
		data = 0x08;	//03H, 76–108 MHz (world wide)
		wdata[2] = data >> 8;
		wdata[3] = data & 0xff;
		data = 0x00;	//04H, De-emphasis: 75us, softmute off, AFC work
		wdata[4] = data >> 8;
		wdata[5] = data & 0xff;
		data = 0x888f;	//05H, LNAP, Volume max,
		wdata[6] = data >> 8;
		wdata[7] = data & 0xff;

		write(fd8, wdata, 8);
	}

	return 0;
}

int main(int argc, char *argv[])
{
	double freq;
	size_t len;

	prog_name = basename(argv[0]);

	init();
	get_station_info();

	if (argc < 2) {
		if ((freq = get_tuned_freq())) {
			set_freq(freq);
			usleep(SIGNAL_WAIT_TIME);
			print_status();
		}
		else usage();
		exit(0);
	}

	len = strlen(argv[1]);
	if (!strncmp(argv[1], "scan", len)) {
		freq_scan();
	}
	else if (!strncmp(argv[1], "status", len)) {
		print_status();
	}
	else if (!strncmp(argv[1], "next", len)) {
		preset_move(1);
		print_status();
	}
	else if (!strncmp(argv[1], "prev", len)) {
		preset_move(0);
		print_status();
	}
	else if (!strncmp(argv[1], "stereo", len)) {
		set_mono(0);
	}
	else if (!strncmp(argv[1], "mono", len)) {
		set_mono(1);
	}
	else if (!strncmp(argv[1], "up", len) || !strncmp(argv[1], "down", len)) {
		int dir;
		if (!strncmp(argv[1], "up", len)) dir = 1;
		else dir = 0;
		search(dir);
		print_status();
		save_freq(get_freq());
	}
	else if (!strncmp(argv[1], "stepup", len)) {
		freq = get_freq() + 0.11;
		if (freq >= FREQ_MIN_MHZ && freq <= FREQ_MAX_MHZ) {
			set_freq(freq);
			usleep(SIGNAL_WAIT_TIME);
		}
		print_status();
		save_freq(get_freq());
	}
	else if (!strncmp(argv[1], "stepdown", len)) {
		freq = get_freq() - 0.09;
		if (freq >= FREQ_MIN_MHZ && freq <= FREQ_MAX_MHZ) {
			set_freq(freq);
			usleep(SIGNAL_WAIT_TIME);
		}
		print_status();
		save_freq(get_freq());
	}
	else if (!strncmp(argv[1], "mute", len)) {
		set_mute(1);
	}
	else if (!strncmp(argv[1], "unmute", len)) {
		set_mute(0);
	}
	else if (!strncmp(argv[1], "off", len)) {
		set_off();
	}
	else if (!strncmp(argv[1], "on", len)) {
		set_off();
		init();
		if ((freq = get_tuned_freq())) {
			set_freq(freq);
			usleep(SIGNAL_WAIT_TIME);
			print_status();
		}
	}
	else if (!strncmp(argv[1], "bass", len)) {
		if (argc > 2) {
			len = strlen(argv[2]);
			if (!strncmp(argv[2], "on", len)) set_bass(1);
			else if (!strncmp(argv[2], "off", len)) set_bass(0);
			else usage();
		}
		printf("Bass Boost: %s\n", get_bass() ? "Enabled" : "Disabled");
	}
	else if (!strncmp(argv[1], "afc", len)) {
		if (argc > 2) {
			len = strlen(argv[2]);
			if (!strncmp(argv[2], "on", len)) set_afc(1);
			else if (!strncmp(argv[2], "off", len)) set_afc(0);
			else usage();
		}
		printf("afc: %s\n", get_afc() ? "off" : "on");
	}
	else if (!strncmp(argv[1], "deemphasis", len)) {
		if (argc > 2) {
			len = strlen(argv[2]);
			if (!strncmp(argv[2], "75", len)) set_deemphasis(1);
			else if (!strncmp(argv[2], "50", len)) set_deemphasis(0);
			else usage();
		}
		printf("De-emphasis: %s\n", get_deemphasis() ? "50us" : "75us");
	}
	else if (!strncmp(argv[1], "volume", len)) {
		u_int8_t vol = get_volume();
		if (argc > 2) {
			len = strlen(argv[2]);
			if (!strncmp(argv[2], "up", len)) vol++;
			else if (!strncmp(argv[2], "down", len)) vol--;
			else vol = atoi(argv[2]);
			if (vol >= 0 && vol <= 15)
				set_volume(vol);
			else usage();
		}
		printf("Volume: %d\n", get_volume());
	}
	else if (!strncmp(argv[1], "noise_blend", len)) {
		u_int16_t val;
		if (argc > 2) {
			val = atoi(argv[2]);
			if (val >= 0 && val <= 31)
				i2c_write(7, (i2c_read(RDA5807_REG_BLEND) & ~0x7c00) | (val << 10) | 0x02);
		}
		printf("Threshold for noise soft blend = %d\n", (i2c_read(RDA5807_REG_BLEND) & 0x7c00) >> 10);
	}
	else {
		int preset;

		freq = strtod(argv[1], NULL);
		preset = (int)freq - 1;

		if ((freq >= FREQ_MIN_MHZ && freq <= FREQ_MAX_MHZ) || (preset >= 0 && preset < station_info_num)) {
			if (freq >= FREQ_MIN_MHZ && freq <= FREQ_MAX_MHZ) freq = strtod(argv[1], NULL);
			else freq = station_info[preset].freq;
			set_freq(freq);
			usleep(SIGNAL_WAIT_TIME);
			print_status();
		}
		else usage();
	}

	return 0;
}

