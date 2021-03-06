const char* str_cl_slice_brainbow = \
"const sampler_t samp =\n" \
"	CLK_NORMALIZED_COORDS_FALSE|\n" \
"	CLK_ADDRESS_CLAMP_TO_EDGE|\n" \
"	CLK_FILTER_NEAREST;\n" \
"\n" \
"float2 vol_grad_func(image2d_t image, int2 pos)\n" \
"{\n" \
"	float2 grad1;\n" \
"	float2 grad2;\n" \
"	grad1.x = read_imagef(image, samp, pos+(int2)(1, 0)).x-\n" \
"		read_imagef(image, samp, pos+(int2)(-1, 0)).x;\n" \
"	grad1.y = read_imagef(image, samp, pos+(int2)(0, 1)).x-\n" \
"		read_imagef(image, samp, pos+(int2)(0, -1)).x;\n" \
"	grad2.x = read_imagef(image, samp, pos+(int2)(1, 1)).x-\n" \
"		read_imagef(image, samp, pos+(int2)(-1, -1)).x;\n" \
"	grad2.y = read_imagef(image, samp, pos+(int2)(1, -1)).x-\n" \
"		read_imagef(image, samp, pos+(int2)(-1, 1)).x;\n" \
"	//rotate\n" \
"	float2 grad2r;\n" \
"	grad2r.x = dot(grad2, (float2)(-0.707, 0.707));\n" \
"	grad2r.y = dot(grad2, (float2)(-0.707, -0.707));\n" \
"	return 0.586f*grad1 + 0.414f*grad2r;\n" \
"}\n" \
"\n" \
"__kernel void kernel_0(\n" \
"	__read_only image2d_t data,\n" \
"	__global unsigned int* label,\n" \
"	unsigned int nx,\n" \
"	unsigned int ny,\n" \
"	__global unsigned int* rcnt,\n" \
"	unsigned int seed,\n" \
"	float value_t,\n" \
"	float value_f,\n" \
"	float grad_f,\n" \
"	float vv_f,\n" \
"	float av_f)\n" \
"{\n" \
"	atomic_inc(rcnt);\n" \
"	int2 coord = (int2)(get_global_id(0),\n" \
"		get_global_id(1));\n" \
"	unsigned int index = nx*coord.y + coord.x;\n" \
"	unsigned int label_v = label[index];\n" \
"	if (label_v == 0)\n" \
"		return;\n" \
"	float value = read_imagef(data, samp, coord).x;\n" \
"	float grad = length(vol_grad_func(data, coord));\n" \
"	//measures\n" \
"	int2 nb_coord;\n" \
"	float nb_value;\n" \
"	float avg_value = 0.0f;\n" \
"	float2 nb_grad;\n" \
"	float2 avg_grad;\n" \
"	for (int i=-1; i<2; ++i)\n" \
"	for (int j=-1; j<2; ++j)\n" \
"	{\n" \
"		nb_coord = (int2)(coord.x+i, coord.y+j);\n" \
"		nb_value = read_imagef(data, samp, nb_coord).x;\n" \
"		avg_value += nb_value;\n" \
"		nb_grad = vol_grad_func(data, nb_coord);\n" \
"		avg_grad += nb_grad;\n" \
"	}\n" \
"	avg_value /= 9.0f;\n" \
"	avg_grad = normalize(avg_grad);\n" \
"	float value_var = 0.0f;\n" \
"	float angle_var = 0.0f;\n" \
"	for (int i=-2; i<3; ++i)\n" \
"	for (int j=-2; j<3; ++j)\n" \
"	{\n" \
"		nb_coord = (int2)(coord.x+i, coord.y+j);\n" \
"		nb_value = read_imagef(data, samp, nb_coord).x;\n" \
"		nb_grad = vol_grad_func(data, nb_coord);\n" \
"		value_var += fabs(nb_value - avg_value);\n" \
"		angle_var += length(nb_grad)*(1.0f-dot(avg_grad,\n" \
"			normalize(nb_grad))/2.0f);\n" \
"	}\n" \
"	value_var /= 25.0f;\n" \
"	angle_var /= 25.0f;\n" \
"	\n" \
"	//stop function\n" \
"	float stop =\n" \
"		(grad_f>0.0f?(grad>sqrt(grad_f)*2.12f?0.0f:exp(-grad*grad/grad_f)):1.0f)*\n" \
"		(value>value_t?1.0f:(value_f>0.0f?(value<value_t-sqrt(value_f)*2.12f?0.0f:exp(-(value-value_t)*(value-value_t)/value_f)):0.0f))*\n" \
"		(vv_f>0.0f?(value_var>sqrt(vv_f)*2.12f?0.0f:exp(-value_var*value_var/vv_f)):1.0f)*\n" \
"		(vv_f>0.0f?(avg_value>sqrt(vv_f)*2.12f?0.0f:exp(-avg_value*avg_value/vv_f)):1.0f)*\n" \
"		(av_f>0.0f?(angle_var>sqrt(av_f)*2.12f?0.0f:exp(-angle_var*angle_var/av_f)):1.0f);\n" \
"	\n" \
"	//max filter\n" \
"	float random = (float)((*rcnt) % seed)/(float)(seed)+0.001f;\n" \
"	if (stop < random)\n" \
"		return;\n" \
"	unsigned int label_value = label[index];\n" \
"	int2 max_nb_coord = coord;\n" \
"	unsigned int nb_index;\n" \
"	unsigned int m;\n" \
"	for (int i=-1; i<2; ++i)\n" \
"	for (int j=-1; j<2; ++j)\n" \
"	{\n" \
"		nb_coord = (int2)(coord.x+i, coord.y+j);\n" \
"		nb_index = nx*nb_coord.y + nb_coord.x;\n" \
"		m = label[nb_index];\n" \
"		if (m > label_value)\n" \
"		{\n" \
"			label_value = m;\n" \
"			max_nb_coord = nb_coord;\n" \
"		}\n" \
"	}\n" \
"	if (grad_f > 0.0f)\n" \
"	{\n" \
"		float xc = value;\n" \
"		float xn = read_imagef(data, samp, max_nb_coord).x + grad_f;\n" \
"		if (xn < xc || xn - xc > 2.0f*grad_f)\n" \
"			return;\n" \
"	}\n" \
"\n" \
"	label[index] = label_value;\n" \
"}\n";

const char* str_cl_brainbow_3d = \
"const sampler_t samp =\n" \
"	CLK_NORMALIZED_COORDS_FALSE|\n" \
"	CLK_ADDRESS_CLAMP_TO_EDGE|\n" \
"	CLK_FILTER_NEAREST;\n" \
"\n" \
"float3 vol_grad_func(image3d_t image, int4 pos)\n" \
"{\n" \
"	float3 grad;\n" \
"	grad.x = read_imagef(image, samp, pos+(int4)(1, 0, 0, 0)).x-\n" \
"		read_imagef(image, samp, pos+(int4)(-1, 0, 0, 0)).x;\n" \
"	grad.y = read_imagef(image, samp, pos+(int4)(0, 1, 0, 0)).x-\n" \
"		read_imagef(image, samp, pos+(int4)(0, -1, 0, 0)).x;\n" \
"	grad.z = read_imagef(image, samp, pos+(int4)(0, 0, 1, 0)).x-\n" \
"		read_imagef(image, samp, pos+(int4)(0, 0, -1, 0)).x;\n" \
"	return grad;\n" \
"}\n" \
"\n" \
"unsigned int __attribute((always_inline)) reverse_bit(unsigned int val, unsigned int len)\n" \
"{\n" \
"	unsigned int res = val;\n" \
"	int s = len - 1;\n" \
"	for (val >>= 1; val; val >>= 1)\n" \
"	{\n" \
"		res <<= 1;\n" \
"		res |= val & 1;\n" \
"		s--;\n" \
"	}\n" \
"	res <<= s;\n" \
"	res <<= 32-len;\n" \
"	res >>= 32-len;\n" \
"	return res;\n" \
"}\n" \
"float get_2d_density(image3d_t image, int4 pos, int r)\n" \
"{\n" \
"	float sum = 0.0f;\n" \
"	int d = 2*r+1;\n" \
"	for (int i=-r; i<=r; ++i)\n" \
"	for (int j=-r; j<=r; ++j)\n" \
"		sum += read_imagef(image, samp, pos+(int4)(i, j, 0, 0)).x;\n" \
"	return sum / (float)(d * d);\n" \
"}\n" \
"__kernel void kernel_0(\n" \
"	__read_only image3d_t data,\n" \
"	__global unsigned int* label,\n" \
"	unsigned int nx,\n" \
"	unsigned int ny,\n" \
"	unsigned int nz,\n" \
"	__global unsigned int* rcnt,\n" \
"	unsigned int seed,\n" \
"	float value_t,\n" \
"	float value_f,\n" \
"	float grad_f,\n" \
"	float density)\n" \
"{\n" \
"	atomic_inc(rcnt);\n" \
"	int3 coord = (int3)(get_global_id(0),\n" \
"		get_global_id(1), get_global_id(2));\n" \
"	unsigned int index = nx*ny*coord.z + nx*coord.y + coord.x;\n" \
"	unsigned int label_v = label[index];\n" \
"	if (label_v == 0)\n" \
"		return;\n" \
"	//break if low density\n" \
"	if (density > 0.0f &&\n" \
"		get_2d_density(data, (int4)(coord, 1), 3) < density)\n" \
"		return;\n" \
"	float value = read_imagef(data, samp, (int4)(coord, 1)).x;\n" \
"	float grad = length(vol_grad_func(data, (int4)(coord, 1)));\n" \
"	//stop function\n" \
"	float stop =\n" \
"		(grad_f>0.0f?(grad>sqrt(grad_f)*2.12f?0.0f:exp(-grad*grad/grad_f)):1.0f)*\n" \
"		(value>value_t?1.0f:(value_f>0.0f?(value<value_t-sqrt(value_f)*2.12f?0.0f:exp(-(value-value_t)*(value-value_t)/value_f)):0.0f));\n" \
"	\n" \
"	//max filter\n" \
"	float random = (float)((*rcnt) % seed)/(float)(seed)+0.001f;\n" \
"	if (stop < random)\n" \
"		return;\n" \
"	unsigned int label_value = label[index];\n" \
"	int3 nb_coord;\n" \
"	unsigned int nb_index;\n" \
"	unsigned int m;\n" \
"	for (int i=-1; i<2; ++i)\n" \
"	for (int j=-1; j<2; ++j)\n" \
"	for (int k=-1; k<2; ++k)\n" \
"	{\n" \
"		nb_coord = (int3)(coord.x+i, coord.y+j, coord.z+k);\n" \
"		if (nb_coord.z < 0 || nb_coord.z > nz-1)\n" \
"			continue;\n" \
"		nb_index = nx*ny*nb_coord.z + nx*nb_coord.y + nb_coord.x;\n" \
"		m = label[nb_index];\n" \
"		if (m > label_value)\n" \
"			label_value = m;\n" \
"	}\n" \
"	label[index] = label_value;\n" \
"}\n" \
"__kernel void kernel_1(\n" \
"	__global unsigned int* mask,\n" \
"	__global unsigned int* label,\n" \
"	unsigned int nx,\n" \
"	unsigned int ny,\n" \
"	unsigned int nz,\n" \
"	unsigned int len)\n" \
"{\n" \
"	unsigned int i = (unsigned int)(get_global_id(0));\n" \
"	unsigned int j = (unsigned int)(get_global_id(1));\n" \
"	unsigned int k = (unsigned int)(get_global_id(2));\n" \
"	unsigned int index = nx*ny*k + nx*j + i;\n" \
"	unsigned int value_l = label[index];\n" \
"	if (value_l == 0)\n" \
"		return;\n" \
"	unsigned int res = nx*ny*nz - value_l;\n" \
"	unsigned int x = 0;\n" \
"	unsigned int y = 0;\n" \
"	unsigned int z = 0;\n" \
"	unsigned int ii;\n" \
"	for (ii=0; ii<len; ++ii)\n" \
"	{\n" \
"		x |= (1<<(3*ii) & res)>>(2*ii);\n" \
"		y |= (1<<(3*ii+1) & res)>>(2*ii+1);\n" \
"		z |= (1<<(3*ii+2) & res)>>(2*ii+2);\n" \
"	}\n" \
"	x = reverse_bit(x, len);\n" \
"	y = reverse_bit(y, len);\n" \
"	z = reverse_bit(z, len);\n" \
"	index = nx*ny*z + nx*y + x;\n" \
"	atomic_inc(&(mask[index]));\n" \
"}\n" \
"__kernel void kernel_2(\n" \
"	__global unsigned int* mask,\n" \
"	__global unsigned int* label,\n" \
"	unsigned int nx,\n" \
"	unsigned int ny,\n" \
"	unsigned int nz,\n" \
"	unsigned int len)\n" \
"{\n" \
"	unsigned int i = (unsigned int)(get_global_id(0));\n" \
"	unsigned int j = (unsigned int)(get_global_id(1));\n" \
"	unsigned int k = (unsigned int)(get_global_id(2));\n" \
"	unsigned int index = nx*ny*k + nx*j + i;\n" \
"	unsigned int value_l = label[index];\n" \
"	if (value_l == 0)\n" \
"		return;\n" \
"	unsigned int res = nx*ny*nz - value_l;\n" \
"	unsigned int x = 0;\n" \
"	unsigned int y = 0;\n" \
"	unsigned int z = 0;\n" \
"	unsigned int ii;\n" \
"	for (ii=0; ii<len; ++ii)\n" \
"	{\n" \
"		x |= (1<<(3*ii) & res)>>(2*ii);\n" \
"		y |= (1<<(3*ii+1) & res)>>(2*ii+1);\n" \
"		z |= (1<<(3*ii+2) & res)>>(2*ii+2);\n" \
"	}\n" \
"	x = reverse_bit(x, len);\n" \
"	y = reverse_bit(y, len);\n" \
"	z = reverse_bit(z, len);\n" \
"	unsigned int index2 = nx*ny*z + nx*y + x;\n" \
"	if (index != index2)\n" \
"		mask[index] = mask[index2];\n" \
"}\n" \
"__kernel void kernel_3(\n" \
"	__read_only image3d_t data,\n" \
"	__global unsigned int* mask,\n" \
"	__global unsigned int* label,\n" \
"	unsigned int nx,\n" \
"	unsigned int ny,\n" \
"	unsigned int nz,\n" \
"	unsigned int thresh)\n" \
"{\n" \
"	unsigned int i = (unsigned int)(get_global_id(0));\n" \
"	unsigned int j = (unsigned int)(get_global_id(1));\n" \
"	unsigned int k = (unsigned int)(get_global_id(2));\n" \
"	unsigned int index = nx*ny*k + nx*j + i;\n" \
"	//break if large enough\n" \
"	if (label[index]==0 ||\n" \
"		mask[index] > thresh)\n" \
"		return;\n" \
"	float value = read_imagef(data, samp, (int4)(i, j, k, 1)).x;\n" \
"	unsigned int nb_index;\n" \
"	unsigned int min_dist = 10;\n" \
"	unsigned int dist;\n" \
"	unsigned int max_nb_index;\n" \
"	float nb_value;\n" \
"	for (int ni=-1; ni<2; ++ni)\n" \
"	for (int nj=-1; nj<2; ++nj)\n" \
"	for (int nk=-1; nk<2; ++nk)\n" \
"	{\n" \
"		if ((k==0 && nk==-1) ||\n" \
"			(k==nz-1 && nk==1))\n" \
"			continue;\n" \
"		nb_index = nx*ny*(k+nk) + nx*(j+nj) + i+ni;\n" \
"		dist = abs(nk) + abs(nj) + abs(ni);\n" \
"		if (mask[nb_index]>thresh &&\n" \
"			dist < min_dist)\n" \
"		{\n" \
"			nb_value = read_imagef(data, samp, (int4)(i+ni, j+nj, k+nk, 1)).x;\n" \
"			if (nb_value < value)\n" \
"				continue;\n" \
"			min_dist = dist;\n" \
"			max_nb_index = nb_index;\n" \
"		}\n" \
"	}\n" \
"	if (min_dist < 10)\n" \
"		label[index] = label[max_nb_index];\n" \
"}\n";


const char* str_cl_brainbow_3d_sized = \
"const sampler_t samp =\n" \
"	CLK_NORMALIZED_COORDS_FALSE|\n" \
"	CLK_ADDRESS_CLAMP_TO_EDGE|\n" \
"	CLK_FILTER_NEAREST;\n" \
"\n" \
"float3 vol_grad_func(image3d_t image, int4 pos)\n" \
"{\n" \
"	float3 grad;\n" \
"	grad.x = read_imagef(image, samp, pos+(int4)(1, 0, 0, 0)).x-\n" \
"		read_imagef(image, samp, pos+(int4)(-1, 0, 0, 0)).x;\n" \
"	grad.y = read_imagef(image, samp, pos+(int4)(0, 1, 0, 0)).x-\n" \
"		read_imagef(image, samp, pos+(int4)(0, -1, 0, 0)).x;\n" \
"	grad.z = read_imagef(image, samp, pos+(int4)(0, 0, 1, 0)).x-\n" \
"		read_imagef(image, samp, pos+(int4)(0, 0, -1, 0)).x;\n" \
"	return grad;\n" \
"}\n" \
"\n" \
"unsigned int __attribute((always_inline)) reverse_bit(unsigned int val, unsigned int len)\n" \
"{\n" \
"	unsigned int res = val;\n" \
"	int s = len - 1;\n" \
"	for (val >>= 1; val; val >>= 1)\n" \
"	{\n" \
"		res <<= 1;\n" \
"		res |= val & 1;\n" \
"		s--;\n" \
"	}\n" \
"	res <<= s;\n" \
"	res <<= 32-len;\n" \
"	res >>= 32-len;\n" \
"	return res;\n" \
"}\n" \
"float get_2d_density(image3d_t image, int4 pos, int r)\n" \
"{\n" \
"	float sum = 0.0f;\n" \
"	int d = 2*r+1;\n" \
"	for (int i=-r; i<=r; ++i)\n" \
"	for (int j=-r; j<=r; ++j)\n" \
"		sum += read_imagef(image, samp, pos+(int4)(i, j, 0, 0)).x;\n" \
"	return sum / (float)(d * d);\n" \
"}\n" \
"__kernel void kernel_0(\n" \
"	__global unsigned int* mask,\n" \
"	__global unsigned int* label,\n" \
"	unsigned int nx,\n" \
"	unsigned int ny,\n" \
"	unsigned int nz,\n" \
"	unsigned int len)\n" \
"{\n" \
"	unsigned int i = (unsigned int)(get_global_id(0));\n" \
"	unsigned int j = (unsigned int)(get_global_id(1));\n" \
"	unsigned int k = (unsigned int)(get_global_id(2));\n" \
"	unsigned int index = nx*ny*k + nx*j + i;\n" \
"	unsigned int value_l = label[index];\n" \
"	if (value_l == 0)\n" \
"		return;\n" \
"	unsigned int res = nx*ny*nz - value_l;\n" \
"	unsigned int x = 0;\n" \
"	unsigned int y = 0;\n" \
"	unsigned int z = 0;\n" \
"	unsigned int ii;\n" \
"	for (ii=0; ii<len; ++ii)\n" \
"	{\n" \
"		x |= (1<<(3*ii) & res)>>(2*ii);\n" \
"		y |= (1<<(3*ii+1) & res)>>(2*ii+1);\n" \
"		z |= (1<<(3*ii+2) & res)>>(2*ii+2);\n" \
"	}\n" \
"	x = reverse_bit(x, len);\n" \
"	y = reverse_bit(y, len);\n" \
"	z = reverse_bit(z, len);\n" \
"	index = nx*ny*z + nx*y + x;\n" \
"	atomic_inc(&(mask[index]));\n" \
"}\n" \
"__kernel void kernel_1(\n" \
"	__global unsigned int* mask,\n" \
"	__global unsigned int* label,\n" \
"	unsigned int nx,\n" \
"	unsigned int ny,\n" \
"	unsigned int nz,\n" \
"	unsigned int len)\n" \
"{\n" \
"	unsigned int i = (unsigned int)(get_global_id(0));\n" \
"	unsigned int j = (unsigned int)(get_global_id(1));\n" \
"	unsigned int k = (unsigned int)(get_global_id(2));\n" \
"	unsigned int index = nx*ny*k + nx*j + i;\n" \
"	unsigned int value_l = label[index];\n" \
"	if (value_l == 0)\n" \
"		return;\n" \
"	unsigned int res = nx*ny*nz - value_l;\n" \
"	unsigned int x = 0;\n" \
"	unsigned int y = 0;\n" \
"	unsigned int z = 0;\n" \
"	unsigned int ii;\n" \
"	for (ii=0; ii<len; ++ii)\n" \
"	{\n" \
"		x |= (1<<(3*ii) & res)>>(2*ii);\n" \
"		y |= (1<<(3*ii+1) & res)>>(2*ii+1);\n" \
"		z |= (1<<(3*ii+2) & res)>>(2*ii+2);\n" \
"	}\n" \
"	x = reverse_bit(x, len);\n" \
"	y = reverse_bit(y, len);\n" \
"	z = reverse_bit(z, len);\n" \
"	unsigned int index2 = nx*ny*z + nx*y + x;\n" \
"	if (index != index2)\n" \
"		mask[index] = mask[index2];\n" \
"}\n" \
"__kernel void kernel_2(\n" \
"	__read_only image3d_t data,\n" \
"	__global unsigned int* mask,\n" \
"	__global unsigned int* label,\n" \
"	unsigned int nx,\n" \
"	unsigned int ny,\n" \
"	unsigned int nz,\n" \
"	__global unsigned int* rcnt,\n" \
"	unsigned int seed,\n" \
"	float value_t,\n" \
"	float value_f,\n" \
"	float grad_f,\n" \
"	unsigned int thresh,\n" \
"	float density)\n" \
"{\n" \
"	atomic_inc(rcnt);\n" \
"	int3 coord = (int3)(get_global_id(0),\n" \
"		get_global_id(1), get_global_id(2));\n" \
"	unsigned int index = nx*ny*coord.z + nx*coord.y + coord.x;\n" \
"	//break if large enough\n" \
"	if (mask[index] > thresh)\n" \
"		return;\n" \
"	//break if low density\n" \
"	if (density > 0.0f &&\n" \
"		get_2d_density(data, (int4)(coord, 1), 3) < density)\n" \
"		return;\n" \
"	unsigned int label_v = label[index];\n" \
"	if (label_v == 0)\n" \
"		return;\n" \
"	float value = read_imagef(data, samp, (int4)(coord, 1)).x;\n" \
"	float grad = length(vol_grad_func(data, (int4)(coord, 1)));\n" \
"	//stop function\n" \
"	float stop =\n" \
"		(grad_f>0.0f?(grad>sqrt(grad_f)*2.12f?0.0f:exp(-grad*grad/grad_f)):1.0f)*\n" \
"		(value>value_t?1.0f:(value_f>0.0f?(value<value_t-sqrt(value_f)*2.12f?0.0f:exp(-(value-value_t)*(value-value_t)/value_f)):0.0f));\n" \
"	\n" \
"	//max filter\n" \
"	float random = (float)((*rcnt) % seed)/(float)(seed)+0.001f;\n" \
"	if (stop < random)\n" \
"		return;\n" \
"	unsigned int label_value = label[index];\n" \
"	int3 nb_coord;\n" \
"	unsigned int nb_index;\n" \
"	unsigned int m;\n" \
"	for (int i=-1; i<2; ++i)\n" \
"	for (int j=-1; j<2; ++j)\n" \
"	for (int k=-1; k<2; ++k)\n" \
"	{\n" \
"		nb_coord = (int3)(coord.x+i, coord.y+j, coord.z+k);\n" \
"		if (nb_coord.z < 0 || nb_coord.z > nz-1)\n" \
"			continue;\n" \
"		nb_index = nx*ny*nb_coord.z + nx*nb_coord.y + nb_coord.x;\n" \
"		m = label[nb_index];\n" \
"		if (m > label_value)\n" \
"			label_value = m;\n" \
"	}\n" \
"	label[index] = label_value;\n" \
"}\n" \
"__kernel void kernel_3(\n" \
"	__read_only image3d_t data,\n" \
"	__global unsigned int* mask,\n" \
"	__global unsigned int* label,\n" \
"	unsigned int nx,\n" \
"	unsigned int ny,\n" \
"	unsigned int nz,\n" \
"	unsigned int thresh)\n" \
"{\n" \
"	unsigned int i = (unsigned int)(get_global_id(0));\n" \
"	unsigned int j = (unsigned int)(get_global_id(1));\n" \
"	unsigned int k = (unsigned int)(get_global_id(2));\n" \
"	unsigned int index = nx*ny*k + nx*j + i;\n" \
"	//break if large enough\n" \
"	if (label[index]==0 ||\n" \
"		mask[index] > thresh)\n" \
"		return;\n" \
"	float value = read_imagef(data, samp, (int4)(i, j, k, 1)).x;\n" \
"	unsigned int nb_index;\n" \
"	unsigned int min_dist = 10;\n" \
"	unsigned int dist;\n" \
"	unsigned int max_nb_index;\n" \
"	float nb_value;\n" \
"	for (int ni=-1; ni<2; ++ni)\n" \
"	for (int nj=-1; nj<2; ++nj)\n" \
"	for (int nk=-1; nk<2; ++nk)\n" \
"	{\n" \
"		if ((k==0 && nk==-1) ||\n" \
"			(k==nz-1 && nk==1))\n" \
"			continue;\n" \
"		nb_index = nx*ny*(k+nk) + nx*(j+nj) + i+ni;\n" \
"		dist = abs(nk) + abs(nj) + abs(ni);\n" \
"		if (mask[nb_index]>thresh &&\n" \
"			dist < min_dist)\n" \
"		{\n" \
"			nb_value = read_imagef(data, samp, (int4)(i+ni, j+nj, k+nk, 1)).x;\n" \
"			if (nb_value < value)\n" \
"				continue;\n" \
"			min_dist = dist;\n" \
"			max_nb_index = nb_index;\n" \
"		}\n" \
"	}\n" \
"	if (min_dist < 10)\n" \
"		label[index] = label[max_nb_index];\n" \
"}\n";


const char* str_cl_shuffle_id_3d = \
"const sampler_t samp =\n" \
"	CLK_NORMALIZED_COORDS_FALSE|\n" \
"	CLK_ADDRESS_CLAMP_TO_EDGE|\n" \
"	CLK_FILTER_NEAREST;\n" \
"\n" \
"unsigned int __attribute((always_inline)) reverse_bit(unsigned int val, unsigned int len)\n" \
"{\n" \
"	unsigned int res = val;\n" \
"	int s = len - 1;\n" \
"	for (val >>= 1; val; val >>= 1)\n" \
"	{\n" \
"		res <<= 1;\n" \
"		res |= val & 1;\n" \
"		s--;\n" \
"	}\n" \
"	res <<= s;\n" \
"	res <<= 32-len;\n" \
"	res >>= 32-len;\n" \
"	return res;\n" \
"}\n" \
"\n" \
"__kernel void kernel_0(\n" \
"	__read_only image3d_t data,\n" \
"	__global unsigned int* label,\n" \
"	unsigned int nx,\n" \
"	unsigned int ny,\n" \
"	unsigned int nz,\n" \
"	unsigned int len)\n" \
"{\n" \
"	unsigned int res;\n" \
"	unsigned int x, y, z, ii;\n" \
"	unsigned int i = (unsigned int)(get_global_id(0));\n" \
"	unsigned int j = (unsigned int)(get_global_id(1));\n" \
"	unsigned int k = (unsigned int)(get_global_id(2));\n" \
"	unsigned int index = nx*ny*k + nx*j + i;\n" \
"	float value = read_imagef(data, samp, (int4)(i, j, k, 1)).x;\n" \
"	if (value < 0.001)\n" \
"		label[index] = 0;\n" \
"	else if (i<1 || i>nx-2 ||\n" \
"			j<1 || j>ny-2)\n" \
"		label[index] = 0;\n" \
"	else\n" \
"	{\n" \
"		x = reverse_bit(i, len);\n" \
"		y = reverse_bit(j, len);\n" \
"		z = reverse_bit(k, len);\n" \
"		res = 0;\n" \
"		for (ii=0; ii<len; ++ii)\n" \
"		{\n" \
"			res |= (1<<ii & x)<<(2*ii);\n" \
"			res |= (1<<ii & y)<<(2*ii+1);\n" \
"			res |= (1<<ii & z)<<(2*ii+2);\n" \
"		}\n" \
"		label[index] = nx*ny*nz - res;\n" \
"	}\n" \
"}\n";

const char* str_cl_order_id_2d = \
"const sampler_t samp =\n" \
"	CLK_NORMALIZED_COORDS_FALSE|\n" \
"	CLK_ADDRESS_CLAMP_TO_EDGE|\n" \
"	CLK_FILTER_NEAREST;\n" \
"\n" \
"\n" \
"__kernel void kernel_0(\n" \
"	__read_only image3d_t data,\n" \
"	__global unsigned int* label,\n" \
"	unsigned int nx,\n" \
"	unsigned int ny,\n" \
"	unsigned int nz)\n" \
"{\n" \
"	unsigned int i = (unsigned int)(get_global_id(0));\n" \
"	unsigned int j = (unsigned int)(get_global_id(1));\n" \
"	unsigned int k = (unsigned int)(get_global_id(2));\n" \
"	unsigned int index = nx*ny*k + nx*j + i;\n" \
"	float value = read_imagef(data, samp, (int4)(i, j, k, 1)).x;\n" \
"	if (value < 0.001)\n" \
"		label[index] = 0;\n" \
"	else if (i<1 || i>nx-2 ||\n" \
"			j<1 || j>ny-2)\n" \
"		label[index] = 0;\n" \
"	else\n" \
"		label[index] = index + 1;\n" \
"}\n";

const char* str_cl_shuffle_id_2d = \
"const sampler_t samp =\n" \
"	CLK_NORMALIZED_COORDS_FALSE|\n" \
"	CLK_ADDRESS_CLAMP_TO_EDGE|\n" \
"	CLK_FILTER_NEAREST;\n" \
"\n" \
"unsigned int __attribute((always_inline)) reverse_bit(unsigned int val, unsigned int len)\n" \
"{\n" \
"	unsigned int res = val;\n" \
"	int s = len - 1;\n" \
"	for (val >>= 1; val; val >>= 1)\n" \
"	{\n" \
"		res <<= 1;\n" \
"		res |= val & 1;\n" \
"		s--;\n" \
"	}\n" \
"	res <<= s;\n" \
"	res <<= 32-len;\n" \
"	res >>= 32-len;\n" \
"	return res;\n" \
"}\n" \
"\n" \
"__kernel void kernel_0(\n" \
"	__read_only image3d_t data,\n" \
"	__global unsigned int* label,\n" \
"	unsigned int nx,\n" \
"	unsigned int ny,\n" \
"	unsigned int nz,\n" \
"	unsigned int len)\n" \
"{\n" \
"	unsigned int x, y, res, ii;\n" \
"	unsigned int i = (unsigned int)(get_global_id(0));\n" \
"	unsigned int j = (unsigned int)(get_global_id(1));\n" \
"	unsigned int k = (unsigned int)(get_global_id(2));\n" \
"	unsigned int index = nx*ny*k + nx*j + i;\n" \
"	float value = read_imagef(data, samp, (int4)(i, j, k, 1)).x;\n" \
"	if (value < 0.001)\n" \
"		label[index] = 0;\n" \
"	else if (i<1 || i>nx-2 ||\n" \
"			j<1 || j>ny-2)\n" \
"		label[index] = 0;\n" \
"	else\n" \
"	{\n" \
"		x = reverse_bit(i, len);\n" \
"		y = reverse_bit(j, len);\n" \
"		res = 0;\n" \
"		res |= k<<(2*len);\n" \
"		for (ii=0; ii<len; ++ii)\n" \
"		{\n" \
"			res |= (1<<ii & x)<<(ii);\n" \
"			res |= (1<<ii & y)<<(ii+1);\n" \
"		}\n" \
"		label[index] = nx*ny - res;\n" \
"	}\n" \
"}\n";

const char* str_cl_grow_size = \
"const sampler_t samp =\n" \
"	CLK_NORMALIZED_COORDS_FALSE|\n" \
"	CLK_ADDRESS_CLAMP_TO_EDGE|\n" \
"	CLK_FILTER_NEAREST;\n" \
"\n" \
"float2 vol_grad_func(image2d_t image, int2 pos)\n" \
"{\n" \
"	float2 grad1;\n" \
"	float2 grad2;\n" \
"	grad1.x = read_imagef(image, samp, pos+(int2)(1, 0)).x-\n" \
"		read_imagef(image, samp, pos+(int2)(-1, 0)).x;\n" \
"	grad1.y = read_imagef(image, samp, pos+(int2)(0, 1)).x-\n" \
"		read_imagef(image, samp, pos+(int2)(0, -1)).x;\n" \
"	grad2.x = read_imagef(image, samp, pos+(int2)(1, 1)).x-\n" \
"		read_imagef(image, samp, pos+(int2)(-1, -1)).x;\n" \
"	grad2.y = read_imagef(image, samp, pos+(int2)(1, -1)).x-\n" \
"		read_imagef(image, samp, pos+(int2)(-1, 1)).x;\n" \
"	//rotate\n" \
"	float2 grad2r;\n" \
"	grad2r.x = dot(grad2, (float2)(-0.707, 0.707));\n" \
"	grad2r.y = dot(grad2, (float2)(-0.707, -0.707));\n" \
"	return 0.586f*grad1 + 0.414f*grad2r;\n" \
"}\n" \
"\n" \
"unsigned int __attribute((always_inline)) reverse_bit(unsigned int val, unsigned int len)\n" \
"{\n" \
"	unsigned int res = val;\n" \
"	int s = len - 1;\n" \
"	for (val >>= 1; val; val >>= 1)\n" \
"	{\n" \
"		res <<= 1;\n" \
"		res |= val & 1;\n" \
"		s--;\n" \
"	}\n" \
"	res <<= s;\n" \
"	res <<= 32-len;\n" \
"	res >>= 32-len;\n" \
"	return res;\n" \
"}\n" \
"__kernel void kernel_0(\n" \
"	__global unsigned int* mask,\n" \
"	__global unsigned int* label,\n" \
"	unsigned int nx,\n" \
"	unsigned int ny,\n" \
"	unsigned int len)\n" \
"{\n" \
"	unsigned int i = (unsigned int)(get_global_id(0));\n" \
"	unsigned int j = (unsigned int)(get_global_id(1));\n" \
"	unsigned int index = nx*j + i;\n" \
"	unsigned int value_l = label[index];\n" \
"	if (value_l == 0)\n" \
"		return;\n" \
"	unsigned int res = nx*ny - value_l;\n" \
"	unsigned int x = 0;\n" \
"	unsigned int y = 0;\n" \
"	unsigned int ii;\n" \
"	for (ii=0; ii<len; ++ii)\n" \
"	{\n" \
"		x |= (1<<(2*ii) & res)>>(ii);\n" \
"		y |= (1<<(2*ii+1) & res)>>(ii+1);\n" \
"	}\n" \
"	x = reverse_bit(x, len);\n" \
"	y = reverse_bit(y, len);\n" \
"	index = nx*y + x;\n" \
"	atomic_inc(&(mask[index]));\n" \
"}\n" \
"__kernel void kernel_1(\n" \
"	__global unsigned int* mask,\n" \
"	__global unsigned int* label,\n" \
"	unsigned int nx,\n" \
"	unsigned int ny,\n" \
"	unsigned int len)\n" \
"{\n" \
"	unsigned int i = (unsigned int)(get_global_id(0));\n" \
"	unsigned int j = (unsigned int)(get_global_id(1));\n" \
"	unsigned int index = nx*j + i;\n" \
"	unsigned int value_l = label[index];\n" \
"	if (value_l == 0)\n" \
"		return;\n" \
"	unsigned int res = nx*ny - value_l;\n" \
"	unsigned int x = 0;\n" \
"	unsigned int y = 0;\n" \
"	unsigned int ii;\n" \
"	for (ii=0; ii<len; ++ii)\n" \
"	{\n" \
"		x |= (1<<(2*ii) & res)>>(ii);\n" \
"		y |= (1<<(2*ii+1) & res)>>(ii+1);\n" \
"	}\n" \
"	x = reverse_bit(x, len);\n" \
"	y = reverse_bit(y, len);\n" \
"	unsigned int index2 = nx*y + x;\n" \
"	if (index != index2)\n" \
"		mask[index] = mask[index2];\n" \
"}\n" \
"__kernel void kernel_2(\n" \
"	__read_only image2d_t data,\n" \
"	__global unsigned int* mask,\n" \
"	__global unsigned int* label,\n" \
"	unsigned int nx,\n" \
"	unsigned int ny,\n" \
"	__global unsigned int* rcnt,\n" \
"	unsigned int seed,\n" \
"	float value_t,\n" \
"	float value_f,\n" \
"	float grad_f,\n" \
"	float vv_f,\n" \
"	float av_f,\n" \
"	unsigned int thresh)\n" \
"{\n" \
"	atomic_inc(rcnt);\n" \
"	int2 coord = (int2)(get_global_id(0),\n" \
"		get_global_id(1));\n" \
"	unsigned int index = nx*coord.y + coord.x;\n" \
"	//break if large enough\n" \
"	if (mask[index] > thresh)\n" \
"		return;\n" \
"	unsigned int label_v = label[index];\n" \
"	if (label_v == 0)\n" \
"		return;\n" \
"	float value = read_imagef(data, samp, coord).x;\n" \
"	float grad = length(vol_grad_func(data, coord));\n" \
"	//measures\n" \
"	int2 nb_coord;\n" \
"	float nb_value;\n" \
"	float avg_value = 0.0f;\n" \
"	float2 nb_grad;\n" \
"	float2 avg_grad;\n" \
"	for (int i=-1; i<2; ++i)\n" \
"	for (int j=-1; j<2; ++j)\n" \
"	{\n" \
"		nb_coord = (int2)(coord.x+i, coord.y+j);\n" \
"		nb_value = read_imagef(data, samp, nb_coord).x;\n" \
"		avg_value += nb_value;\n" \
"		nb_grad = vol_grad_func(data, nb_coord);\n" \
"		avg_grad += nb_grad;\n" \
"	}\n" \
"	avg_value /= 9.0f;\n" \
"	avg_grad = normalize(avg_grad);\n" \
"	float value_var = 0.0f;\n" \
"	float angle_var = 0.0f;\n" \
"	for (int i=-2; i<3; ++i)\n" \
"	for (int j=-2; j<3; ++j)\n" \
"	{\n" \
"		nb_coord = (int2)(coord.x+i, coord.y+j);\n" \
"		nb_value = read_imagef(data, samp, nb_coord).x;\n" \
"		nb_grad = vol_grad_func(data, nb_coord);\n" \
"		value_var += fabs(nb_value - avg_value);\n" \
"		angle_var += length(nb_grad)*(1.0f-dot(avg_grad,\n" \
"			normalize(nb_grad))/2.0f);\n" \
"	}\n" \
"	value_var /= 25.0f;\n" \
"	angle_var /= 25.0f;\n" \
"	\n" \
"	//stop function\n" \
"	float stop =\n" \
"		(grad_f>0.0f?(grad>sqrt(grad_f)*2.12f?0.0f:exp(-grad*grad/grad_f)):1.0f)*\n" \
"		(value>value_t?1.0f:(value_f>0.0f?(value<value_t-sqrt(value_f)*2.12f?0.0f:exp(-(value-value_t)*(value-value_t)/value_f)):0.0f))*\n" \
"		(vv_f>0.0f?(value_var>sqrt(vv_f)*2.12f?0.0f:exp(-value_var*value_var/vv_f)):1.0f)*\n" \
"		(vv_f>0.0f?(avg_value>sqrt(vv_f)*2.12f?0.0f:exp(-avg_value*avg_value/vv_f)):1.0f)*\n" \
"		(av_f>0.0f?(angle_var>sqrt(av_f)*2.12f?0.0f:exp(-angle_var*angle_var/av_f)):1.0f);\n" \
"	\n" \
"	//max filter\n" \
"	float random = (float)((*rcnt) % seed)/(float)(seed)+0.001f;\n" \
"	if (stop < random)\n" \
"		return;\n" \
"	unsigned int label_value = label[index];\n" \
"	int2 max_nb_coord = coord;\n" \
"	unsigned int nb_index;\n" \
"	unsigned int m;\n" \
"	for (int i=-1; i<2; ++i)\n" \
"	for (int j=-1; j<2; ++j)\n" \
"	{\n" \
"		nb_coord = (int2)(coord.x+i, coord.y+j);\n" \
"		nb_index = nx*nb_coord.y + nb_coord.x;\n" \
"		m = label[nb_index];\n" \
"		if (m > label_value)\n" \
"		{\n" \
"			label_value = m;\n" \
"			max_nb_coord = nb_coord;\n" \
"		}\n" \
"	}\n" \
"	if (grad_f > 0.0f)\n" \
"	{\n" \
"		float xc = value;\n" \
"		float xn = read_imagef(data, samp, max_nb_coord).x + grad_f;\n" \
"		if (xn < xc || xn - xc > 2.0f*grad_f)\n" \
"			return;\n" \
"	}\n" \
"\n" \
"	label[index] = label_value;\n" \
"}\n";

const char*str_cl_clean_up = \
"__kernel void kernel_0(\n" \
"	__global unsigned int* mask,\n" \
"	__global unsigned int* label,\n" \
"	unsigned int nx,\n" \
"	unsigned int ny,\n" \
"	unsigned int thresh)\n" \
"{\n" \
"	unsigned int i = (unsigned int)(get_global_id(0));\n" \
"	unsigned int j = (unsigned int)(get_global_id(1));\n" \
"	unsigned int index = nx*j + i;\n" \
"	//break is large enough\n" \
"	if (label[index]==0 ||\n" \
"		mask[index] > thresh)\n" \
"		return;\n" \
"	unsigned int nb_index;\n" \
"	unsigned int max_size = 0;\n" \
"	unsigned int max_nb_index;\n" \
"	for (int ni=-1; ni<2; ++ni)\n" \
"	for (int nj=-1; nj<2; ++nj)\n" \
"	{\n" \
"		nb_index = nx*(j+nj) + i+ni;\n" \
"		if (mask[nb_index]>thresh &&\n" \
"			mask[nb_index]>max_size)\n" \
"		{\n" \
"			max_size = mask[nb_index];\n" \
"			max_nb_index = nb_index;\n" \
"		}\n" \
"	}\n" \
"	if (max_size > 0)\n" \
"		label[index] = label[max_nb_index];\n" \
"}\n";

const char* str_cl_match_slices = \
"__kernel void kernel_0(\n" \
"	__global unsigned int* mask,\n" \
"	__global unsigned int* label1,\n" \
"	__global unsigned int* label2,\n" \
"	__global bool* flag,\n" \
"	unsigned int nx,\n" \
"	unsigned int ny)\n" \
"{\n" \
"	unsigned int i = (unsigned int)(get_global_id(0));\n" \
"	unsigned int j = (unsigned int)(get_global_id(1));\n" \
"	unsigned int index = nx*j + i;\n" \
"	if (flag[index])\n" \
"		return;\n" \
"	unsigned int value_l1 = label1[index];\n" \
"	unsigned int value_l2 = label2[index];\n" \
"	if (!value_l1 || !value_l2)\n" \
"		return;\n" \
"	unsigned int svl1, svl2, sidx;\n" \
"	unsigned int size = 0;\n" \
"	for (unsigned int ii=0; ii<nx; ++ii)\n" \
"	for (unsigned int jj=0; jj<ny; ++jj)\n" \
"	{\n" \
"		sidx = nx*jj + ii;\n" \
"		svl1 = label1[sidx];\n" \
"		svl2 = label2[sidx];\n" \
"		if (svl1!=value_l1 || svl2!=value_l2)\n" \
"			continue;\n" \
"		if (flag[sidx])\n" \
"			return;\n" \
"		flag[sidx] = true;\n" \
"		size++;\n" \
"	}\n" \
"	for (unsigned int ii=0; ii<nx; ++ii)\n" \
"	for (unsigned int jj=0; jj<ny; ++jj)\n" \
"	{\n" \
"		sidx = nx*jj + ii;\n" \
"		svl1 = label1[sidx];\n" \
"		svl2 = label2[sidx];\n" \
"		if (svl1!=value_l1 || svl2!=value_l2)\n" \
"			continue;\n" \
"		mask[sidx] = size;\n" \
"	}\n" \
"}\n" \
"\n" \
"__kernel void kernel_1(\n" \
"	__global unsigned int* mask1,\n" \
"	__global unsigned int* mask2,\n" \
"	__global unsigned int* mask_and,\n" \
"	__global unsigned int* label1,\n" \
"	__global unsigned int* label2,\n" \
"	unsigned int nx,\n" \
"	unsigned int ny,\n" \
"	unsigned int thresh)\n" \
"{\n" \
"	unsigned int i = (unsigned int)(get_global_id(0));\n" \
"	unsigned int j = (unsigned int)(get_global_id(1));\n" \
"	unsigned int index = nx*j +i;\n" \
"	unsigned int value_l1 = label1[index];\n" \
"	unsigned int value_l2 = label2[index];\n" \
"	if (!value_l1 || !value_l2 || value_l1==value_l2)\n" \
"		return;\n" \
"	unsigned int size1 = mask1[index];\n" \
"	unsigned int size2 = mask2[index];\n" \
"	if (size1<=thresh || size2<=thresh)\n" \
"		return;\n" \
"	unsigned int size_and = mask_and[index];\n" \
"	if ((float)size_and/(float)size1 +\n" \
"		(float)size_and/(float)size2 <= 1.0)\n" \
"		return;\n" \
"	unsigned int sidx;\n" \
"	for (unsigned int ii=0; ii<nx; ++ii)\n" \
"	for (unsigned int jj=0; jj<ny; ++jj)\n" \
"	{\n" \
"		sidx = nx*jj + ii;\n" \
"		if (label2[sidx] == value_l2)\n" \
"			label2[sidx] = value_l1;\n" \
"	}\n" \
"}\n";
