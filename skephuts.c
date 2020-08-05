#include <stdio.h>
#include <math.h>
#include <time.h>
#include "cubiomes/finders.h"

int count = 0;

void possible(Pos pos1, Pos pos2) {
	//printf("(%d, %d)\n", pos1.x, pos1.z);
	double dist = sqrt((pos2.x - pos1.x)*(pos2.x - pos1.x) + (pos2.z - pos1.z)*(pos2.z - pos1.z));
	if (dist < 256) {
		double originDist = sqrt((((long)pos2.x)*pos2.x) + (((long)pos2.z)*pos2.z));
		printf("(%d, %d), (%d, %d), %lf, %lf\n", pos1.x, pos1.z, pos2.x, pos2.z, dist, originDist);
		count++;
	}
}

int main() {
	initBiomes();

	long seed = 8054221181584912980;
	
	clock_t start_time = clock();
	LayerStack g = setupGenerator(MC_1_16);
	applySeed(&g, seed);

	int minX = -100, minZ = -100, maxX = 100, maxZ = 100;
	int doesRegionHaveHut[200][200] = {0};

	for (int x = minX; x < maxX; x++) {
		for (int z = minZ; z < maxZ; z++) {
			//printf("(%d, %d)\n", x, z);
			Pos	pos = getStructurePos(SWAMP_HUT_CONFIG, seed, x, z);
			if (isViableStructurePos(SWAMP_HUT_CONFIG, MC_1_16, &g, seed, pos.x, pos.z)) {
				//printf("(%d, %d)\n", pos.x, pos.z);
				doesRegionHaveHut[x+100][z+100] = 1;
			}
		}
	}

	for (int x = minX+1; x < maxX-1; x++) {
		for (int z = minZ+1; z < maxZ-1; z++) {
			Pos pos = getStructurePos(SWAMP_HUT_CONFIG, seed, x, z);
			if (doesRegionHaveHut[x+100][z+100]) {
				if (doesRegionHaveHut[x+100-1][z+100] == 1)
					possible(pos, getStructurePos(SWAMP_HUT_CONFIG, seed, x-1, z));
				if (doesRegionHaveHut[x+100][z+100-1] == 1)
					possible(pos, getStructurePos(SWAMP_HUT_CONFIG, seed, x, z-1));
				if (doesRegionHaveHut[x+100-1][z+100-1] == 1)
					possible(pos, getStructurePos(SWAMP_HUT_CONFIG, seed, x-1, z-1));
				if (doesRegionHaveHut[x+100+1][z+100] == 1)
					possible(pos, getStructurePos(SWAMP_HUT_CONFIG, seed, x+1, z));
				if (doesRegionHaveHut[x+100][z+100+1] == 1)
					possible(pos, getStructurePos(SWAMP_HUT_CONFIG, seed, x, z+1));
				if (doesRegionHaveHut[x+100+1][z+100+1] == 1)
					possible(pos, getStructurePos(SWAMP_HUT_CONFIG, seed, x+1, z+1));
				if (doesRegionHaveHut[x+100-1][z+100+1] == 1)
					possible(pos, getStructurePos(SWAMP_HUT_CONFIG, seed, x-1, z+1));
				if (doesRegionHaveHut[x+100+1][z+100-1] == 1)
					possible(pos, getStructurePos(SWAMP_HUT_CONFIG, seed, x+1, z-1));
			}
		}
	}

	freeGenerator(g);
	clock_t end_time = clock();
	printf("Found %d double huts in %lf seconds\n", count, ((double) end_time - start_time) / CLOCKS_PER_SEC);

	return 0;
}
