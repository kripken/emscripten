#include <stdio.h>
#include <emscripten/emmalloc.h>

int main()
{
	void *ptr = malloc(32*1024*1024);
	void *ptr2 = malloc(4*1024*1024);
	void *ptr3 = malloc(64*1024*1024);
	void *ptr4 = malloc(16*1024);
	void *ptr5 = malloc(2*1024*1024);
	printf("%d\n", (int)(ptr && ptr2 && ptr3 && ptr4 && ptr5));
	free(ptr2);
	free(ptr4);
	printf("%d\n", emmalloc_validate_memory_regions());
	printf("%zu\n", emmalloc_dynamic_heap_size());
	printf("%zu\n", emmalloc_free_dynamic_memory());
	size_t numFreeMemoryRegions = 0;
	size_t freeMemorySizeMap[32];
	numFreeMemoryRegions = emmalloc_compute_free_dynamic_memory_fragmentation_map(freeMemorySizeMap);
	printf("%zu\n", numFreeMemoryRegions);
	for(int i = 0; i < 32; ++i)
		printf("%d ", freeMemorySizeMap[i]);
	printf("\n");
}
