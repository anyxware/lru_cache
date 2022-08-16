#define N 30
#define BUF_SIZE 4
#define SIZE 1000000 //CACHE

#ifndef CACHE_H
#define CACHE_H

typedef struct{
	size_t key;
	void* info;
	unsigned int age;
}Data;

typedef struct Node{
	Data* data;
	int balance;
	struct Node* left;
	struct Node* right;
	struct Node* par;
	struct Node* next;
	struct Node* prev;
}Node;

typedef struct{
	int height;
	Node* new_node;
}RetInfo;

typedef struct{
	Node* node;
	unsigned int age;
	int busy;
}BufItem;

typedef struct{
	BufItem array[3 * BUF_SIZE];
	unsigned int max_age;
}Bufer;

typedef struct{
	Node* root;
	int root_size;
	int max_size;
	float alfa;
	Bufer bufer;
}Tree;

typedef struct{
	unsigned int max_age;
	Tree key_sorted;
	Tree age_sorted;
	unsigned int capacity;
}Cache;

#endif