#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <glob.h>
#include <sys/wait.h>

struct Config {
	const char *data;
	const char *wildcard;
};

#include "config.h"

#define LENGTH(arr) (sizeof(arr)/sizeof(arr[0]))
#define ARG(SHORT, LONG) !strcmp(argv[1], SHORT) || !strcmp(argv[1], LONG)
#define APPLY(config) apply_config(config, LENGTH(config))

#define ITOSTR(number, buffer) \
	(snprintf(buffer, sizeof(buffer), "%d", (int)number), buffer)

#define EXEC(cmd, ...) \
	do { \
		pid_t pid; \
		if ((pid = fork()) == 0) { \
			execlp(cmd, cmd, __VA_ARGS__, (char *)NULL); \
			exit(0); \
		} \
		if (pid > 0) \
			waitpid(pid, NULL, 0); \
	} while(0)

/* cpwr.c */
static void apply_config(struct Config config[], unsigned int size);
static void configure_hard_drive(char *device);
static void write_to_files_matching_wildcard(const char *data, const char *wildcard);
static void try_write_until_success(const char *data, const char *path);
static int has_appending_slash(char *str);
static void print_help(void);

int
main(int argc, char *argv[])
{
	if (argc < 2) { print_help(); return 1; }
	else if (ARG("-v", "--version")) printf("cpwr %s\n", VERSION);
	else if (ARG("-h", "--help")) print_help();
	else if (ARG("i", "init")) APPLY(init);
	else if (ARG("p", "powersave")) APPLY(powersave);
	else if (ARG("P", "performance")) APPLY(performance);
	else if (ARG("l", "lowbattery")) APPLY(lowbattery);
	else if (ARG("h", "hdd"))
		if (argc == 3)
			configure_hard_drive(argv[2]);
		else
			printf("the hdd action requires an argument.\n");
	else { print_help(); return 1; }
}

static void
apply_config(struct Config config[], unsigned int size)
{
	for (int i = 0; i < size; i++)
		write_to_files_matching_wildcard(config[i].data, config[i].wildcard);
}

static void
configure_hard_drive(char *device)
{
	char buffer[16];
	EXEC("hdparm", "-B", ITOSTR(hdparm_apm, buffer), "--", device);
	EXEC("blockdev", "--setra", ITOSTR(readahead * 2, buffer), "--", device);
}

static void
write_to_files_matching_wildcard(const char *data, const char* wildcard)
{
	glob_t gl;
	int ret;

	/* the GLOB_MARK flag makes glob append a slash to the output
	 * directory paths. we can use this to filter out directories. */
	ret = glob(wildcard, GLOB_MARK, NULL, &gl);

	if (ret != 0)
		printf("no files match \"%s\".\n", wildcard);
	else
		for (int i = 0; i < gl.gl_pathc; i++)
			if (!has_appending_slash(gl.gl_pathv[i]))
				try_write_until_success(data, gl.gl_pathv[i]);

	globfree(&gl);
}

static void
try_write_until_success(const char* data, const char* path)
{
	int i;
	FILE *file;

	printf("writing to \"%s\"...\n", path);

	for (i = 0; i < write_max_tries; i++) {
		if (i > 0)
			sleep(write_interval);
		if ((file = fopen(path, "w")) && fprintf(file, "%s\n", data) > 0)
			break;
	}

	if (file)
		fclose(file);
	printf("%s\n", i < write_max_tries ? "done." : "failed.");
}

static int
has_appending_slash(char *str)
{
	return str[strlen(str) - 1] == '/' ? 1 : 0;
}

static void
print_help(void)
{
	printf(
		"cpwr is a power management tool.\n"
		"usage: cpwr <ACTION>\n"
		"actions:\n"
		"  i, init            apply the init configs\n"
		"  p, powersave       apply the powersave configs\n"
		"  P, performance     apply the performance configs\n"
		"  l, lowbattery      apply the lowbattery configs\n"
		"  h, hdd <DEVICE>    apply the HDD configs to the given block device\n"
	);
}

/* if (!is_system_laptop()) { */
/* 	printf("this machine is not a battery powered device.\n"); */
/* 	return 1; */
/* } */
/* static int */
/* is_system_laptop(void) */
/* { */
/* 	FILE *file; */
/* 	unsigned int chassis_type; */
/* 	int is_laptop; */

/* 	if (!(file = fopen("/sys/class/dmi/id/chassis_type", "r"))) */
/* 		return 0; */

/* 	if (fscanf(file, "%u", &chassis_type) == EOF) { */
/* 		fclose(file); */
/* 		return 0; */
/* 	} */

/* 	switch(chassis_type) { */
/* 		case  9: case 10: case 11: case 14: */
/* 		case 30: case 31: case 32: */
/* 			is_laptop = 1; */
/* 			break; */
/* 		default: */
/* 			is_laptop = 0; */
/* 	} */

/* 	return is_laptop; */
/* } */
