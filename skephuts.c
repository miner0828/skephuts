#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
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

int main(int argc, char** argv) {
	long seed;
	char seedSet = 0;

	for (int i=1; i<argc; i++) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) { 
			case 'h':
				printf(
				"Skephuts- Minecraft 1.16 Witch Hut finder\n"
				"Note: String seeds are not yet supported, only numeric.\n"
				"Usage: skephuts [options] <seed>\n"
				"Valid options:\n"
				"	-h		Prints this message.\n"
				"   -d      Includes the distance from one witch hut to the other\n"
				"   -o      Includes the distance from the first witch hut to the origin.\n");
				return 0;
			case 'd':
				flags |= DIST_FLAG;
				break;
			case 'o':
				flags |= ORIGIN_DIST_FLAG;
				break;
			default:
				printf("Unknown flag '-%c'. Use 'skephuts -h' for usage.\n", argv[i][1]);
				return 1;
			}
		} else if (seedSet == 0) {
			seedSet = 1;
			errno = 0;
			seed = strtol(argv[i], NULL, 10);
			if (errno) {
				printf("An error occured while parsing the seed.\n");
				return 1;
			}
		} else {
			printf("Unknown parameter %s. Use 'skephuts -h' for usage.\n", argv[i]);
			return 1;
		}
	}
	if (seedSet == 0) {
		printf("No seed given! Use 'skephuts -h' for usage.\n");
		return 1;
	}

	initBiomes();
	
	clock_t start_time = clock();
	LayerStack g = setupGenerator(MC_1_16);
	applySeed(&g, seed);

	int minX = -100, minZ = -100, maxX = 100, maxZ = 100;
	//1 extra on each side for padding so accessing it for checking doesnt go out of bounds
	int doesRegionHaveHut[201][201] = {0};

	for (int x = minX; x < maxX; x++) {
		for (int z = minZ; z < maxZ; z++) {
			//printf("(%d, %d)\n", x, z);
			Pos	pos = getStructurePos(SWAMP_HUT_CONFIG, seed, x, z);
			if (isViableStructurePos(SWAMP_HUT_CONFIG, MC_1_16, &g, seed, pos.x, pos.z)) {
				//printf("(%d, %d)\n", pos.x, pos.z);
				doesRegionHaveHut[x+101][z+101] = 1;
				if (doesRegionHaveHut[x+101-1][z+101] == 1)
					possible(pos, getStructurePos(SWAMP_HUT_CONFIG, seed, x-1, z));
				if (doesRegionHaveHut[x+101][z+101-1] == 1)
					possible(pos, getStructurePos(SWAMP_HUT_CONFIG, seed, x, z-1));
				if (doesRegionHaveHut[x+101-1][z+101-1] == 1)
					possible(pos, getStructurePos(SWAMP_HUT_CONFIG, seed, x-1, z-1));
				if (doesRegionHaveHut[x+101-1][z+101+1] == 1)
					possible(pos, getStructurePos(SWAMP_HUT_CONFIG, seed, x-1, z+1));
			}
		}
	}

	freeGenerator(g);
	clock_t end_time = clock();
	printf("Found %d double huts in %lf seconds\n", count, ((double) end_time - start_time) / CLOCKS_PER_SEC);

	return 0;
}
