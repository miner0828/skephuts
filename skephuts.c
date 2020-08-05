#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include "cubiomes/finders.h"

#define DIST_FLAG (1 << 0)
#define ORIGIN_DIST_FLAG (1 << 1)

int count = 0;
unsigned int flags = 0;

void possible(Pos pos1, Pos pos2) {
	//printf("(%d, %d)\n", pos1.x, pos1.z);
	double dist = sqrt((pos2.x - pos1.x)*(pos2.x - pos1.x) + (pos2.z - pos1.z)*(pos2.z - pos1.z));
	
	if (dist < 256) {
		printf("(%d, %d), (%d, %d)", pos1.x, pos1.z, pos2.x, pos2.z);
		if (flags & DIST_FLAG)
			printf(", %lf", dist);
		if (flags & ORIGIN_DIST_FLAG) {
			double originDist = sqrt((((long)pos1.x)*pos1.x) + (((long)pos1.z)*pos1.z));
			printf(", %lf", originDist);
		}
		printf("\n");

		count++;
	}
}

long toNum(char* str) {
	errno = 0;
	long out = strtol(str, NULL, 10);
	if (errno) {
		printf("An error occured while parsing the input.\n");
		return 1;
	}
	return out;
}

int main(int argc, char** argv) {
	long seed = 0;
	char seedSet = 0;
	int minX = -100, minZ = -100, maxX = 100, maxZ = 100;

	for (int i=1; i<argc; i++) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) { 
			case 'h':
				printf(
				"Skephuts- Minecraft 1.16 Witch Hut finder\n"
				"Note: String seeds are not yet supported, only numeric.\n"
				"Usage: skephuts [options] <seed>\n"
				"Valid options:\n"
				"   -h      Prints this message.\n"
				"   -d      Includes the distance from one witch hut to the other\n"
				"   -o      Includes the distance from the first witch hut to the origin.\n"
				"   -x      Set minimum X value that will be searched (in regions of 512x512 blocks). Default: -100\n"
				"   -X      Set maximum X value that will be searched (in regions of 512x512 blocks). Default: 100\n"
				"   -z      Set minimum Z value that will be searched (in regions of 512x512 blocks). Default: -100\n"
				"   -Z      Set maximum Z value that will be searched (in regions of 512x512 blocks). Default: 100\n"
				);
				return 0;
			case 'd':
				flags |= DIST_FLAG;
				break;
			case 'o':
				flags |= ORIGIN_DIST_FLAG;
				break;
			case 'x':
				if (argv[i][2] != 0)
					minX = (int) toNum(argv[i]+2);
				else {
					i++;
					minX = (int) toNum(argv[i]);
				}
				break;
			case 'X':
				if (argv[i][2] != 0)
					maxX = (int) toNum(argv[i]+2);
				else {
					i++;
					maxX = (int) toNum(argv[i]);
				}
				break;
			case 'z':
				if (argv[i][2] != 0)
					minZ = (int) toNum(argv[i]+2);
				else {
					i++;
					minZ = (int) toNum(argv[i]);
				}
				break;
			case 'Z':
				if (argv[i][2] != 0)
					maxZ = (int) toNum(argv[i]+2);
				else {
					i++;
					maxZ = (int) toNum(argv[i]);
				}
				break;
			default:
				printf("Unknown flag '-%c'. Use 'skephuts -h' for usage.\n", argv[i][1]);
				return 1;
			}
		} else if (seedSet == 0) {
			seedSet = 1;
			seed = toNum(argv[i]);
		} else {
			printf("Unknown parameter %s. Use 'skephuts -h' for usage.\n", argv[i]);
			return 1;
		}
	}
	if (seedSet == 0) {
		printf("No seed given! Use 'skephuts -h' for usage.\n");
		return 1;
	}

	if ((maxX < minX) || (maxZ < minZ)) {
		printf("Max values cannot be less than min values.\n");
		return 1;
	}
	
	printf("Witch hut locations	");
	if (flags & DIST_FLAG)
		printf("Distance between huts	");
	if (flags & ORIGIN_DIST_FLAG)
		printf("Distance to origin");
	printf("\n");

	initBiomes();
	
	clock_t start_time = clock();
	LayerStack g = setupGenerator(MC_1_16);
	applySeed(&g, seed);

	//1 extra on each side for padding so accessing it for checking doesnt go out of bounds
	size_t DRHHSize = ((maxX-minX)+1)*((maxZ-minZ)+1)*sizeof(int);
	int (*doesRegionHaveHut)[(maxZ-minZ)+1] = malloc(DRHHSize);
	memset(doesRegionHaveHut, 0, DRHHSize);

	int offsetX = (-minX)+1, offsetZ = (-minZ)+1;

	for (int x = minX; x < maxX; x++) {
		for (int z = minZ; z < maxZ; z++) {
			//printf("(%d, %d)\n", x, z);
			Pos	pos = getStructurePos(SWAMP_HUT_CONFIG, seed, x, z);
			if (isViableStructurePos(SWAMP_HUT_CONFIG, MC_1_16, &g, seed, pos.x, pos.z)) {
				//printf("(%d, %d)\n", pos.x, pos.z);
				doesRegionHaveHut[x+offsetX][z+offsetZ] = 1;
				if (doesRegionHaveHut[x+offsetX-1][z+offsetZ] == 1)
					possible(pos, getStructurePos(SWAMP_HUT_CONFIG, seed, x-1, z));
				if (doesRegionHaveHut[x+offsetX][z+offsetZ-1] == 1)
					possible(pos, getStructurePos(SWAMP_HUT_CONFIG, seed, x, z-1));
				if (doesRegionHaveHut[x+offsetX-1][z+offsetZ-1] == 1)
					possible(pos, getStructurePos(SWAMP_HUT_CONFIG, seed, x-1, z-1));
				if (doesRegionHaveHut[x+offsetX-1][z+offsetZ+1] == 1)
					possible(pos, getStructurePos(SWAMP_HUT_CONFIG, seed, x-1, z+1));
			}
		}
	}

	freeGenerator(g);
	free(doesRegionHaveHut);
	clock_t end_time = clock();
	printf("Found %d double huts in %lf seconds\n", count, ((double) end_time - start_time) / CLOCKS_PER_SEC);

	return 0;
}
