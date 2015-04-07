
#define NOMINMAX

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include <string>
#include <map>
#include <vector>
#include <iterator>
#include <limits>
#include <algorithm>

#include "minisnsf.h"
#include "PSFFile.h"
#include "cpath.h"

#ifdef WIN32
#include <direct.h>
#include <float.h>
#define getcwd _getcwd
#define chdir _chdir
#define isnan _isnan
#define strcasecmp _stricmp
#else
#include <unistd.h>
#endif

#define APP_NAME    "minisnsf"
#define APP_VER     "[2015-04-07]"
#define APP_URL     "http://github.com/loveemu/minisnsf"

#define SNSF_PSF_VERSION        0x23
#define SNSF_EXE_HEADER_SIZE    8

static void writeInt(uint8_t * buf, uint32_t value)
{
	buf[0] = value & 0xff;
	buf[1] = (value >> 8) & 0xff;
	buf[2] = (value >> 16) & 0xff;
	buf[3] = (value >> 24) & 0xff;
}

bool exe2snsf(const char * snsf_path, uint8_t * exe, uint32_t exe_size, std::map<std::string, std::string> tags)
{
	ZlibWriter zlib_exe(Z_BEST_COMPRESSION);
	zlib_exe.write(exe, exe_size);

	if (!PSFFile::save(snsf_path, SNSF_PSF_VERSION, NULL, 0, zlib_exe, tags)) {
		return false;
	}

	return true;
}

bool make_minisnsf(const char * snsf_path, uint32_t address, uint32_t size, uint32_t num, std::map<std::string, std::string>& tags)
{
	uint8_t exe[SNSF_EXE_HEADER_SIZE + 256];
	memset(exe, 0, SNSF_EXE_HEADER_SIZE + 256);

	// limit size
	if (size > 256) {
		return false;
	}

	// make exe
	writeInt(&exe[0], address);
	writeInt(&exe[4], size);
	writeInt(&exe[8], num);

	// write mini2sf file
	return exe2snsf(snsf_path, exe, SNSF_EXE_HEADER_SIZE + size, tags);
}

static void usage(const char * progname)
{
	printf("%s %s\n", APP_NAME, APP_VER);
	printf("<%s>\n", APP_URL);
	printf("\n");
	printf("Usage\n");
	printf("-----\n");
	printf("\n");
	printf("Syntax: `%s (options) [Base name] [Offset] [Size] [Count]`\n", progname);
	printf("\n");

	printf("### Options\n");
	printf("\n");
	printf("`--help`\n");
	printf("  : Show help\n");
	printf("\n");
	printf("`--psfby`, `--snsfby`\n");
	printf("  : Set creator of SNSF\n");
	printf("\n");
}

int main(int argc, char *argv[])
{
	if (argc == 1) {
		usage(argv[0]);
		return EXIT_FAILURE;
	}

	long longval;
	char *endptr = NULL;

	char *psfby = NULL;

	int argi = 1;
	while (argi < argc && argv[argi][0] == '-') {
		if (strcmp(argv[argi], "--help") == 0) {
			usage(argv[0]);
			return EXIT_FAILURE;
		}
		else if (strcmp(argv[argi], "--psfby") == 0 || strcmp(argv[argi], "--snsfby") == 0) {
			if (argi + 1 >= argc) {
				fprintf(stderr, "Error: Too few arguments for \"%s\"\n", argv[argi]);
				return EXIT_FAILURE;
			}

			psfby = argv[argi + 1];
			argi++;
		}
		else {
			fprintf(stderr, "Error: Unknown option \"%s\"\n", argv[argi]);
			return EXIT_FAILURE;
		}
		argi++;
	}

	int argnum = argc - argi;
	if (argnum != 4) {
		fprintf(stderr, "Error: Too few/more arguments\n");
		return EXIT_FAILURE;
	}

	char * snsf_basename = argv[argi];

	char libname[PATH_MAX];
	sprintf(libname, "%s.snsflib", snsf_basename);

	longval = strtol(argv[argi + 1], &endptr, 16);
	if (*endptr != '\0' || errno == ERANGE || longval < 0) {
		fprintf(stderr, "Error: Number format error \"%s\"\n", argv[argi + 1]);
		return EXIT_FAILURE;
	}
	uint32_t offset = (uint32_t)longval;

	longval = strtol(argv[argi + 2], &endptr, 10);
	if (*endptr != '\0' || errno == ERANGE || longval < 0) {
		fprintf(stderr, "Error: Number format error \"%s\"\n", argv[argi + 2]);
		return EXIT_FAILURE;
	}
	size_t size = (size_t)longval;

	longval = strtol(argv[argi + 3], &endptr, 10);
	if (*endptr != '\0' || errno == ERANGE || longval < 0) {
		fprintf(stderr, "Error: Number format error \"%s\"\n", argv[argi + 3]);
		return EXIT_FAILURE;
	}
	uint32_t count = (uint32_t)longval;

	int num_error = 0;
	for (uint32_t num = 0; num < count; num++) {
		std::map<std::string, std::string> tags;
		tags["_lib"] = libname;

		if (psfby != NULL && strcmp(psfby, "") != 0) {
			tags["snsfby"] = psfby;
		}

		char snsf_path[PATH_MAX];
		sprintf(snsf_path, "%s-%04d.minisnsf", snsf_basename, num);

		if (make_minisnsf(snsf_path, offset, (uint32_t)size, num, tags)) {
			printf("Created %s\n", snsf_path);
		}
		else {
			printf("Error: Unable to create %s\n", snsf_path);
			num_error++;
		}
	}

	return (num_error == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
