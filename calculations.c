#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#define SCX 1
#define SCY 1
#define SCZ 1
#define CX 3
#define CY 3
#define CZ 3

struct vec3_t
{
	double x;
	double y;
	double z;
};

struct vec3_int_t
{
	int x;
	int y;
	int z;
};

struct vec3_t vec3_sub(struct vec3_t a, struct vec3_t b)
{
	struct vec3_t out;
	out.x = a.x - b.x;
	out.y = a.y - b.y;
	out.z = a.z - b.z;
	return out;
}

struct vec3_t vec3_mul(struct vec3_t a, struct vec3_t b)
{
	struct vec3_t out;
	out.x = a.x * b.x;
	out.y = a.y * b.y;
	out.z = a.z * b.z;
	return out;
}

struct vec3_t vec3_add(struct vec3_t a, struct vec3_t b)
{
	struct vec3_t out;
	out.x = a.x + b.x;
	out.y = a.y + b.y;
	out.z = a.z + b.z;
	return out;
}

uint8_t get_3d_array_entry(uint8_t *array, struct vec3_t index, struct vec3_t max)
{
	return array[(int)(floor(index.x)) + (int)(floor(max.x) * floor(index.y)) + (int)(floor(max.x) * floor(max.y) * floor(index.z))];
}

void set_3d_array_entry(uint8_t *array, struct vec3_t index, struct vec3_t max, uint8_t val)
{
	array[(int)(floor(index.x)) + (int)(floor(max.x) * floor(index.y)) + (int)(floor(max.x) * floor(max.y) * floor(index.z))] = val;
}

struct vec3_t get_lookat(double z_rot, double y_rot)
{
	struct vec3_t lookat = { 0.0 };
	lookat.y = y_rot;
	lookat.z = z_rot;
	return lookat;
}

double get_distance_to_block(struct vec3_t block, struct vec3_t camera)
{
	struct vec3_t block_cam_dif_vec = vec3_sub(camera, block);

	double xy_dis = sqrt(pow(block_cam_dif_vec.x, 2) + pow(block_cam_dif_vec.y, 2));
	double xyz_dis = sqrt(pow(block_cam_dif_vec.z, 2) + pow(xy_dis, 2));

	//This won't work if the xyz distance is the same as the z displacement
	double dis_in_block = 0.5; //Half the cube width of 1 because if x and y are 0 some stuff fails
	if(block_cam_dif_vec.z != xyz_dis)
	{	
		double theta = asin(block_cam_dif_vec.z / xyz_dis);
		dis_in_block = 0.5 / cos(theta);
	}

	dis_in_block = 0.0; //Just check some stuff
	return xyz_dis - dis_in_block;
}

struct vec3_t get_block(struct vec3_t lookat, struct vec3_t position, int ray_length, uint8_t *blk, struct vec3_t max)
{
	struct vec3_t testpos = position;
	struct vec3_t step = { 0.1, 0.1, 0.1 };
	struct vec3_t radian = { (M_PI / 180) , (M_PI / 180), (M_PI / 180)};

	for(int i = 0; i < ray_length; i++)
	{
		testpos = vec3_add(testpos, vec3_mul(vec3_mul(lookat, radian), step));
		
		printf("%lf, %lf, %lf\n", testpos.x, testpos.y, testpos.z);

		/* If a block isn't air the ray has collided with something */
		/* Air is index 0 */
		if(get_3d_array_entry(blk, testpos, max))
		{
			printf("%d\n", get_3d_array_entry(blk, testpos, max));
			break;
		}
	}

	return testpos;
}

int main()
{
	struct vec3_t camera = { 1, 1, 1 };
	struct vec3_t lookat = { 0, 15, 0 };

	uint8_t *blk_array = malloc(sizeof(uint8_t) * SCX * CX * SCY * CY * SCZ * CZ);
	memset(blk_array, 0, sizeof(uint8_t) * SCX * CX * SCY * CY * SCZ * CZ);
	struct vec3_t blk_array_max = { (double)(SCX * CX), (double)(SCY * CY), (double)(SCZ * CZ) };

	for(int x = 0; x < 3; x++)
	{
		for(int y = 0; y < 3; y++)
		{
			for(int z = 0; z < 3; z++)
			{
				struct vec3_t temp = { x, y, z };
				if(x == 1 && y == 1 && z == 1)
				{
					set_3d_array_entry(blk_array, temp, blk_array_max, 0);
				}
				else
				{
					set_3d_array_entry(blk_array, temp, blk_array_max, 16);
				}
			}
		}
	}

	struct vec3_t temp = { 1, 1, 1 };
	set_3d_array_entry(blk_array, temp, blk_array_max, 0);

	struct vec3_t block = get_block(lookat, camera, 1000, blk_array, blk_array_max);
	printf("[INFO] Distance is: %lf\n", get_distance_to_block(block, camera));
	printf("[INFO] Block Coordinates are: %lf, %lf, %lf\n[INFO] Blocktype is: %d\n", block.x, block.y, block.z, get_3d_array_entry(blk_array, block, blk_array_max));

	free(blk_array);

	return 0;
}
