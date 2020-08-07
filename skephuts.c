#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include "cubiomes/finders.h"

#define DIST_FLAG (1 << 0)
#define ORIGIN_DIST_FLAG (1 << 1)
#define SHOW_DUB_FLAG (1 << 2)
#define SHOW_TRI_FLAG (1 << 3)

long seed = 0;
int count = 0, coll, offsetX, offsetZ;
unsigned int flags = 0;

static inline double distance(Pos pos1, Pos pos2) {
	return sqrt((((double)pos2.x - pos1.x)*(pos2.x - pos1.x)) + (((double)pos2.z - pos1.z)*(pos2.z - pos1.z)));
}
static inline Pos blockToRegion(Pos pos) {
	if (pos.x < 0)
		pos.x -= 512;
	if (pos.z < 0)
		pos.z -= 512;
	Pos region = {((int) pos.x) / 512, ((int) pos.z) / 512};
	return region;
}

int possibleQuad(Pos pos1, char (*DRHH)[coll]) {
	Pos region1 = blockToRegion(pos1);

	if (DRHH[region1.x+offsetX][region1.z-1+offsetZ] && DRHH[region1.x-1+offsetX][region1.z+offsetZ] && DRHH[region1.x-1+offsetX][region1.z-1+offsetZ]) {
		Pos	pos2 = getStructurePos(SWAMP_HUT_CONFIG, seed, region1.x, region1.z-1);
		Pos	pos3 = getStructurePos(SWAMP_HUT_CONFIG, seed, region1.x-1, region1.z);
		Pos	pos4 = getStructurePos(SWAMP_HUT_CONFIG, seed, region1.x-1, region1.z-1);
		//printf("pos1234 (%d, %d), (%d, %d), (%d, %d), (%d, %d)\n", pos1.x, pos1.z, pos2.x, pos2.z, pos3.x, pos3.z, pos4.x, pos4.z);
	
		double dist12 = distance(pos1, pos2), dist23 = distance(pos2, pos3), dist34 = distance(pos3, pos4),
		dist13 = distance(pos1, pos3), dist24 = distance(pos2, pos4), dist14 = distance(pos1, pos4);
		//printf("dist %lf %lf %lf %lf %lf %lf\n", dist12, dist23, dist34, dist13, dist24, dist14);
		if ((dist12 < 256) && (dist23 < 256) && (dist34 < 256) && (dist13 < 256) && (dist24 < 256) && (dist14 < 256)) {
			printf("4 (%d, %d), (%d, %d), (%d, %d), (%d, %d)", pos1.x, pos1.z, pos2.x, pos2.z, pos3.x, pos3.z, pos4.x, pos4.z);
			if (flags & DIST_FLAG)
				printf(", %lf", (dist12 + dist23 + dist34 + dist13 + dist24 + dist14) / 6);
			if (flags & ORIGIN_DIST_FLAG) {
				Pos origin = {0, 0};
				double originDist = distance(pos1, origin);
				printf(", %lf", originDist);
			}
			printf("\n");
			count++;
			return 1;
		}
	}
	return 0;
}

int possibleTri(Pos pos1, Pos pos2, Pos region2, int region3x, int region3z, char (*DRHH)[coll]) {
	//printf("reg2x %d reg3x %d reg2z %d reg3z %d\n", region2.x, region3x, region2.z, region3z);
	if ((DRHH[region3x+offsetX][region3z+offsetZ]) && !((region2.x == region3x) && (region2.z == region3z))) {
		Pos	pos3 = getStructurePos(SWAMP_HUT_CONFIG, seed, region3x, region3z);

		double dist12 = distance(pos1, pos2), dist23 = distance(pos2, pos3), dist13 = distance(pos1, pos3);
		if ((dist12 < 256) && (dist23 < 256) && (dist13 < 256)) {
			if (possibleQuad(pos1, DRHH) || !(flags & SHOW_TRI_FLAG))
				return 1;
			printf("3 (%d, %d), (%d, %d), (%d, %d)", pos1.x, pos1.z, pos2.x, pos2.z, pos3.x, pos3.z);
			if (flags & DIST_FLAG)
				printf(", %lf", (dist12 + dist23 + dist13) / 3);
			if (flags & ORIGIN_DIST_FLAG) {
				Pos origin = {0, 0};
				double originDist = distance(pos1, origin);
				printf(", %lf", originDist);
			}
			printf("\n");
			count++;
			return 1;
		}
	}
	return 0;
}

int testTri(Pos pos1, Pos pos2, char (*DRHH)[coll]) {
	Pos region1 = blockToRegion(pos1);
	Pos region2 = blockToRegion(pos2);
	
	if (possibleTri(pos1, pos2, region2, region1.x, region1.z-1, DRHH)) return 1; 
	else if (possibleTri(pos1, pos2, region2, region1.x-1, region1.z-1, DRHH)) return 1; 
	else if (possibleTri(pos1, pos2, region2, region1.x-1, region1.z, DRHH)) return 1;
	else if (possibleTri(pos1, pos2, region2, region1.x-1, region1.z+1, DRHH)) return 1;
	return 0;
}

void possible(Pos pos1, Pos pos2, char (*DRHH)[coll]) {
	//printf("(%d, %d)\n", pos1.x, pos1.z);
	double dist = distance(pos1, pos2);
	if (dist < 256) {
		if (testTri(pos1, pos2, DRHH) || !(flags & SHOW_DUB_FLAG))
			return;
		printf("2 (%d, %d), (%d, %d)", pos1.x, pos1.z, pos2.x, pos2.z);
		if (flags & DIST_FLAG)
			printf(", %lf", dist);
		if (flags & ORIGIN_DIST_FLAG) {
			Pos origin = {0, 0};
			double originDist = distance(pos1, origin);
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
	char seedSet = 0;
	int minX = -100, minZ = -100, maxX = 100, maxZ = 100;

	for (int i=1; i<argc; i++) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) { 
			case '0' ... '9':
				seedSet = 1;
				seed = toNum(argv[i]);
				break;
			case 'h':
				printf(
				"Skephuts- Minecraft 1.16 Witch Hut finder\n"
				"Note: String seeds are not yet supported, only numeric.\n"
				"Usage: skephuts [options] <seed>\n"
				"Valid options:\n"
				"   -h      Prints this message.\n"
				"   -d      Includes the average distance from one witch hut to another\n"
				"   -o      Includes the distance from the first witch hut to the origin.\n"
				"   -x      Set minimum X value that will be searched (in regions of 512x512 blocks). Default: -100\n"
				"   -X      Set maximum X value that will be searched (in regions of 512x512 blocks). Default: 100\n"
				"   -z      Set minimum Z value that will be searched (in regions of 512x512 blocks). Default: -100\n"
				"   -Z      Set maximum Z value that will be searched (in regions of 512x512 blocks). Default: 100\n"
				"   -D      Show double huts\n"
				"   -T      Show triple huts\n"
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
			case 'D':
				flags |= SHOW_DUB_FLAG;
				break;
			case 'T':
				flags |= SHOW_TRI_FLAG;
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
	
	printf("# | Witch hut locations	");
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
	size_t DRHHSize = ((maxX-minX)+1)*((maxZ-minZ)+1)*sizeof(char);
	coll = (maxZ-minZ)+1;
	char (*doesRegionHaveHut)[coll] = malloc(DRHHSize);
	memset(doesRegionHaveHut, 0, DRHHSize);

	offsetX = (-minX)+1, offsetZ = (-minZ)+1;

	for (int x = minX; x < maxX; x++) {
		for (int z = minZ; z < maxZ; z++) {
			//printf("(%d, %d)\n", x, z);
			Pos	pos = getStructurePos(SWAMP_HUT_CONFIG, seed, x, z);
			if (isViableStructurePos(SWAMP_HUT_CONFIG, MC_1_16, &g, seed, pos.x, pos.z)) {
				//printf("(%d, %d)\n", pos.x, pos.z);
				doesRegionHaveHut[x+offsetX][z+offsetZ] = 1;
				if (doesRegionHaveHut[x+offsetX-1][z+offsetZ] == 1)
					possible(pos, getStructurePos(SWAMP_HUT_CONFIG, seed, x-1, z), doesRegionHaveHut);
				else if (doesRegionHaveHut[x+offsetX][z+offsetZ-1] == 1)
					possible(pos, getStructurePos(SWAMP_HUT_CONFIG, seed, x, z-1), doesRegionHaveHut);
				else if (doesRegionHaveHut[x+offsetX-1][z+offsetZ-1] == 1)
					possible(pos, getStructurePos(SWAMP_HUT_CONFIG, seed, x-1, z-1), doesRegionHaveHut);
				else if (doesRegionHaveHut[x+offsetX-1][z+offsetZ+1] == 1)
					possible(pos, getStructurePos(SWAMP_HUT_CONFIG, seed, x-1, z+1), doesRegionHaveHut);
			}
		}
	}

	freeGenerator(g);
	free(doesRegionHaveHut);
	clock_t end_time = clock();
	printf("Found %d huts in %lf seconds\n", count, ((double) end_time - start_time) / CLOCKS_PER_SEC);

	return 0;
}
