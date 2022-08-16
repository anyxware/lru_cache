#include <stdio.h>
#include "tree_cache.h"

int main(int argc, char const *argv[])
{
	Cache cache;
	init_cache(&cache);

	printf("bench(1)/test(2)\n");
	
	int answ;
	scanf("%d", &answ);

	if(answ == 2){
		for(;;){
			printf("find(1)/insert(2)/delete(3): ");
			int ans;
			int dd = scanf("%d", &ans);
			if(dd < 0) break;
	
			if(ans == 2){
				int size;	
				printf("key: ");
				int d = scanf("%d", &size);
				if(d < 0) break;
				insert_cache(&cache, size, (void*)"6th laba");

			}
			if(ans == 1){
				int size;	
				printf("key: ");
				int d = scanf("%d", &size);
				if(d < 0) break;
				const void* info = find_cache(&cache, size);
				printf("%s\n\n", (char*)info);
			}
			if(ans == 3){
				int size;	
				printf("key: ");
				int d = scanf("%d", &size);
				if(d < 0) break;
				delete_cache(&cache, size);
			}
	
			if(cache.capacity) print_cache(cache);
		}
	}else if(answ == 1){
		int num;
		srand(time(NULL));

		for(int i = 100000; i <= 1000000; i+= 50000){
			num = i;
			printf("%d\n", i);
			for(int j = 0; j < num; j++){
				size_t key = rand() % 10000000;
				clock_t time = clock();
				insert_cache(&cache, key, (void*)"qwertyuiopasdfghjkl");
			}
		
			clock_t insert_t = 0;
			clock_t find_t = 0;
			clock_t delete_t = 0;

			for(int j = 0; j < 1000; j++){
				size_t key = rand() % 10000000;
			
				clock_t time = clock();
				insert_cache(&cache, key, (void*)3);
				insert_t += clock() - time;
						
				time = clock();
				find_cache(&cache, key);
				find_t += clock() - time;
						
				time = clock();
				delete_cache(&cache, key);
				delete_t += clock() - time;
			}
		
			printf("%lu\n", insert_t / 1);
			printf("%lu\n", find_t / 1);
			printf("%lu\n\n", delete_t / 1);

			clear_cache(&cache);
		}
	}

	clear_cache(&cache);
	
	return 0;
}