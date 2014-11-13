#define KX 3
#define KY 3
#define KZ 3
float __constant krn[KX][KY][KZ] =
{{{1.0/9.0, 1.0/9.0, 1.0/9.0},
  {1.0/9.0, 1.0/9.0, 1.0/9.0},
  {1.0/9.0, 1.0/9.0, 1.0/9.0}},
 {{1.0/9.0, 1.0/9.0, 1.0/9.0},
  {1.0/9.0, 1.0/9.0, 1.0/9.0},
  {1.0/9.0, 1.0/9.0, 1.0/9.0}},
 {{1.0/9.0, 1.0/9.0, 1.0/9.0},
  {1.0/9.0, 1.0/9.0, 1.0/9.0},
  {1.0/9.0, 1.0/9.0, 1.0/9.0}}};
const sampler_t samp =
	CLK_NORMALIZED_COORDS_FALSE|
	CLK_ADDRESS_REPEAT|
	CLK_FILTER_NEAREST;
__kernel void main(
	__global read_only image3d_t data,
	__global unsigned char* result,
	unsigned int x,
	unsigned int y,
	unsigned int z)
{
	int4 coord = (int4)(get_global_id(0),
		get_global_id(1), get_global_id(2), 1);
	int4 kc;
	float4 dvalue;
	float rvalue = 0.0;
	for (int i=0; i<KX; ++i)
	for (int j=0; j<KY; ++j)
	for (int k=0; k<KZ; ++k)
	{
		kc = (int4)(coord.x+(i-KX/2),
				coord.y+(j-KY/2),
				coord.z+(k-KZ/2), 1);
		dvalue = read_imagef(data, samp, kc);
		rvalue += krn[i][j][k] * dvalue.x;
	}
	unsigned int index = x*y*coord.z + x*coord.y + coord.x;
	result[index] = rvalue*255.0;
}