#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "tree_cache.h"

#define lru

unsigned int hash_key(size_t key){ //HashFAQ6
	char* str = (char*)&key;

	unsigned int hash = 0;

	for (int i = 0; i < sizeof(size_t) && *str; i++, str++)
	{
		hash += (unsigned char)(*str);
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	return hash;
}

unsigned int hash_age(unsigned int age){ //HashFAQ6
	char* str = (char*)&age;

	unsigned int hash = 0;

	for (int i = 0; i < sizeof(unsigned int) && *str; i++, str++)
	{
		hash += (unsigned char)(*str);
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	return hash;
}

int find_index(Bufer* bufer, int index){
	return bufer->array[index].busy != 1 ? index : bufer->array[index + 1].busy != 1 ? index + 1 : bufer->array[index + 2].busy != 1 ? index + 2 : -1;
}

int find_real_index(Bufer* bufer, int index){
	return bufer->array[index].age < bufer->array[index + 1].age && bufer->array[index].age < bufer->array[index + 2].age ? index :
	bufer->array[index + 1].age < bufer->array[index].age && bufer->array[index + 1].age < bufer->array[index + 2].age ? index + 1 : index + 2;
}

void insert_key_bufer(Bufer* bufer, Node* node){
	int index = 3/* 3 - bucket size */ * (hash_key(node->data->key) % BUF_SIZE); //bucket number, 
	int real_index;
	real_index = find_index(bufer, index); //find free position(index || index + 1 || index + 2)(if no return -1)
	if(real_index == -1){
		real_index = find_real_index(bufer, index); //find the eldest position 
	}
	//inserting
	bufer->max_age++;
	bufer->array[real_index].busy = 1;
	bufer->array[real_index].node = node;
	bufer->array[real_index].age = bufer->max_age;
	return;
}

void insert_age_bufer(Bufer* bufer, Node* node){
	int index = 3/* 3 - bucket size */ * (hash_age(node->data->age) % BUF_SIZE); //bucket number, 
	int real_index;
	real_index = find_index(bufer, index); //find free position(index || index + 1 || index + 2)(if no return -1)
	if(real_index == -1){
		real_index = find_real_index(bufer, index); //find the eldest position 
	}
	//inserting
	bufer->max_age++;
	bufer->array[real_index].busy = 1;
	bufer->array[real_index].node = node;
	bufer->array[real_index].age = bufer->max_age;
}

Node* find_key_bufer(Bufer* bufer, size_t key){
	int index = 3 * (hash_key(key) % BUF_SIZE);
	if(bufer->array[index].busy && bufer->array[index].node->data->key == key){
		return bufer->array[index].node;
	}else if(bufer->array[index + 1].busy && bufer->array[index + 1].node->data->key == key){
		return bufer->array[index + 1].node;
	}else if(bufer->array[index + 2].busy && bufer->array[index + 2].node->data->key == key){
		return bufer->array[index + 2].node;
	}
	return NULL;
}

Node* find_age_bufer(Bufer* bufer, unsigned int age){
	int index = 3 * (hash_age(age) % BUF_SIZE);
	if(bufer->array[index].busy && bufer->array[index].node->data->age == age){
		return bufer->array[index].node;
	}else if(bufer->array[index + 1].busy && bufer->array[index + 1].node->data->age == age){
		return bufer->array[index + 1].node;
	}else if(bufer->array[index + 2].busy && bufer->array[index + 2].node->data->age == age){
		return bufer->array[index + 2].node;
	}
	return NULL;
}

void remove_key_bufer(Bufer* bufer, size_t key){
	int index = 3 * (hash_key(key) % BUF_SIZE);
	if(bufer->array[index].busy && bufer->array[index].node->data->key == key){
		bufer->array[index].busy = 0;
	}else if(bufer->array[index + 1].busy && bufer->array[index + 1].node->data->key == key){
		bufer->array[index + 1].busy = 0;
	}else if(bufer->array[index + 2].busy && bufer->array[index + 2].node->data->key == key){
		bufer->array[index + 2].busy = 0;
	}
}

void remove_age_bufer(Bufer* bufer, unsigned int age){
	int index = 3 * (hash_age(age) % BUF_SIZE);
	if(bufer->array[index].busy && bufer->array[index].node->data->age == age){
		bufer->array[index].busy = 0;
	}else if(bufer->array[index + 1].busy && bufer->array[index + 1].node->data->age == age){
		bufer->array[index + 1].busy = 0;
	}else if(bufer->array[index + 2].busy && bufer->array[index + 2].node->data->age == age){
		bufer->array[index + 2].busy = 0;
	}
}

float log_alfa(float alfa, int x){
	return (logf((float)x) / logf(1 / alfa));
}

Node* find_min_node(Node* node){
	for(;node->left; node = node->left){}
	return node;
}

Node* find_max_node(Node* node){
	for(;node->right; node = node->right){}
	return node;
}

Node* find_key_node(Node* node, size_t key){
	while(node){;
		if(key > node->data->key){
			node = node->right;
		}else if(key < node->data->key){
			node = node->left;
		}else{
			return node;
		}
	}
	return NULL;
}

Node* find_age_node(Node* node, unsigned int age){
	while(node){;
		if(age > node->data->age){
			node = node->right;
		}else if(age < node->data->age){
			node = node->left;
		}else{
			return node;
		}
	}
	return NULL;
}

Data* find_key_tree(Tree* tree, size_t key){

	Node* buf_find = find_key_bufer(&tree->bufer, key);
	if(buf_find){
		return buf_find->data;
	}

	Node* find = find_key_node(tree->root, key);
	if(!find) return NULL;

	insert_key_bufer(&tree->bufer, find);

	return find->data;
}

Node* next_node(Node* node){
	if(node->right){
		return find_min_node(node->right);
	}
	Node* par = node->par;
	for(;par && node == par->right; node = par, par = par->par){}
	return par;
}

Node* prev_node(Node* node){
	if(node->left){
		return find_max_node(node->left);
	}
	Node* par = node->par;
	for(;par && node == par->left; node = par, par = par->par){}
	return par;
}

Node* create_node(Node* par, Data* data){
	Node* node = (Node*)malloc(sizeof(Node));

	node->data = data;
	node->balance = 0;

	node->left = node->right = NULL;
	node->par = par;

	return node;
}

void direct_node(Node* node){
	node->next = next_node(node);
	if(node->next) node->next->prev = node;
	node->prev = prev_node(node);
	if(node->prev) node->prev->next = node;
}

RetInfo insert_key_node(Node** root, Data* data){
	if(!*root){
		*root = create_node(NULL, data);
		direct_node(*root);

		RetInfo retinfo;
		retinfo.height = 0;
		retinfo.new_node = *root;
		return retinfo;
	}
	Node* node = *root;
	int counter = 1;
	while(node){
		if(data->key > node->data->key){
			if(node->right){
				node = node->right;
				counter++;
			}else{
				node->right = create_node(node, data);
				direct_node(node->right);

				RetInfo retinfo;
				retinfo.height = counter;
				retinfo.new_node = node->right;
				return retinfo;
			}
		}else if(data->key < node->data->key){
			if(node->left){
				node = node->left;
				counter++;
			}else{
				node->left = create_node(node, data);
				direct_node(node->left);

				RetInfo retinfo;
				retinfo.height = counter;
				retinfo.new_node = node->left;
				return retinfo;
			}
		}else{
			RetInfo retinfo;
			retinfo.height = -1;
			return retinfo;
		}
	}
}

RetInfo insert_age_node(Node** root, Data* data){
	if(!*root){
		*root = create_node(NULL, data);
		direct_node(*root);

		RetInfo retinfo;
		retinfo.height = 0;
		retinfo.new_node = *root;
		return retinfo;
	}
	Node* node = *root;
	int counter = 1;
	while(node){
		if(data->age > node->data->age){
			if(node->right){
				node = node->right;
				counter++;
			}else{
				node->right = create_node(node, data);
				direct_node(node->right);

				RetInfo retinfo;
				retinfo.height = counter;
				retinfo.new_node = node->right;
				return retinfo;
			}
		}else if(data->age < node->data->age){
			if(node->left){
				node = node->left;
				counter++;
			}else{
				node->left = create_node(node, data);
				direct_node(node->left);

				RetInfo retinfo;
				retinfo.height = counter;
				retinfo.new_node = node->left;
				return retinfo;
			}
		}else{
			RetInfo retinfo;
			retinfo.height = -1;
			return retinfo;
		}
	}
}

int size_node(Node* node){
	if(!node){
		return 0;
	}else{
		return size_node(node->left) + size_node(node->right) + 1;
	}
}

Node* find_scapegoat(float alfa, Node* node){
	int size = 1, par_size;
	int height = 1;
	while(node->par){
		par_size = node == node->par->left ? 1 + size + size_node(node->par->right) : 1 + size + size_node(node->par->left);
		if(height > log_alfa(alfa, par_size)){
			return node->par;
		}
		height++;
		size = par_size;
		node = node->par;
	}
}

Node* tree_to_list(Node* node){
	Node* list1 = (node->left) ? tree_to_list(node->left) : NULL;
	Node* list2 = (node->right) ? tree_to_list(node->right) : NULL;
	Node* list3 = node;

	list3->right = list2;
	if (!list1) 
		return list3;
	Node* last = list1;
	while(last->right) {
		last=last->right;
	}
	last->right = list3;
	return list1;
}

void ord_list(Node* head){
	Node* par = NULL;
	while(head){
		head->par = par;
		head->left = NULL;

		par = head;
		head = head = head->right;
	}
}

Node* node_from_list(Node* head, int index){
	for(int i = 0; i < index; i++, head = head->right){}
	return head;
}

Node* rebuild_scapegoat_tree(Node* head, Node* par, int size){
	if(size <= 0){
		return NULL;
	}

	Node* sub_root = node_from_list(head, size / 2);
	sub_root->left = rebuild_scapegoat_tree(head, sub_root, size / 2);
	sub_root->right = rebuild_scapegoat_tree(sub_root->right, sub_root, size - size / 2 - 1);
	sub_root->par = par;
}

void left_rotate(Node** root, Node* node){
	Node* right_node = node->right, *parent = node->par;

	node->right = right_node->left;

	if(right_node->left)
		right_node->left->par = node;
	
	right_node->par = parent;

	if(!parent){
		*root = right_node;
	}else if(parent->left == node){
		parent->left = right_node;
	}else{
		parent->right = right_node;
	}

	right_node->left = node;
	node->par = right_node;
}

void right_rotate(Node** root, Node* node){
	Node* left_node = node->left, *parent = node->par;

	node->left = left_node->right;

	if(left_node->right)
		left_node->right->par = node;
	
	left_node->par = parent;

	if(!parent){
		*root = left_node;
	}else if(parent->right == node){
		parent->right = left_node;
	}
	else{
		parent->left = left_node;
	}

	left_node->right = node;
	node->par = left_node;
}

void insert_left_correct(Node** root, Node* new_node){
	if(new_node->balance == -1){
		new_node->balance = 0;
		new_node->par->balance = 0;
		right_rotate(root, new_node->par);
	}
	else if(new_node->right->balance == -1){
		new_node->balance = 0;
		new_node->right->balance = 0;
		new_node->par->balance = 1;
		left_rotate(root, new_node);
		right_rotate(root, new_node->par->par);
	}else if(new_node->right->balance == 1){
		new_node->balance = -1;
		new_node->right->balance = 0;
		new_node->par->balance = 0;
		left_rotate(root, new_node);
		right_rotate(root, new_node->par->par);
	}else{
		new_node->balance = 0;
		new_node->right->balance = 0;
		new_node->par->balance = 0;
		left_rotate(root, new_node);
		right_rotate(root, new_node->par->par);
	}
}

void insert_right_correct(Node** root, Node* new_node){
	if(new_node->balance == 1){
		new_node->balance = 0;
		new_node->par->balance = 0;
		left_rotate(root, new_node->par);
	}
	else if(new_node->left->balance == 1){
		new_node->balance = 0;
		new_node->left->balance = 0;
		new_node->par->balance = -1;
		right_rotate(root, new_node);
		left_rotate(root, new_node->par->par);
	}else if(new_node->left->balance == -1){
		new_node->balance = 1;
		new_node->left->balance = 0;
		new_node->par->balance = 0;
		right_rotate(root, new_node);
		left_rotate(root, new_node->par->par);
	}else{
		new_node->balance = 0;
		new_node->left->balance = 0;
		new_node->par->balance = 0;
		right_rotate(root, new_node);
		left_rotate(root, new_node->par->par);
	}
}

void insert_rebuild_tree(Node** root, Node* new_node){
	Node* parent = new_node->par;
	while(parent && !parent->balance){
		if(parent->left == new_node){
			parent->balance -= 1;
		}else{
			parent->balance += 1;
		}
		new_node = parent;
		parent = new_node->par;
	}
	if(!parent) return;
	
	if(parent->left == new_node && parent->balance == 1 || parent->right == new_node && parent->balance == -1){
		parent->balance = 0;
		return;
	}

	if(parent->left == new_node){
		insert_left_correct(root, new_node);
	}else if(parent->right == new_node){
		insert_right_correct(root, new_node);
	}
}

int insert_key_tree(Tree* tree, Data* data){
	RetInfo retinfo = insert_key_node(&tree->root, data);
	tree->root_size++;

	if(retinfo.height == -1){
		return -1;
	}
	insert_key_bufer(&tree->bufer, retinfo.new_node);

	insert_rebuild_tree(&tree->root, retinfo.new_node);
	return 0;
}

int insert_age_tree(Tree* tree, Data* data){
	RetInfo retinfo = insert_age_node(&tree->root, data);
	tree->root_size++;

	if(retinfo.height == -1){
		return -1;
	}
	insert_age_bufer(&tree->bufer, retinfo.new_node);

	insert_rebuild_tree(&tree->root, retinfo.new_node);
	return 0;
}

Node** remove_key_node(Node** element, Node* target, Bufer* bufer){
	Node* node = *element;
	Node* son;

	if(node->left && node->right){
		Node* max = find_max_node(node->left);

		Node* status = find_key_bufer(bufer, max->data->key);
		
		remove_key_bufer(bufer, node->data->key);
		remove_key_bufer(bufer, max->data->key);

		node->data = max->data;

		if(node->prev) node->prev->next = node->next;
		if(node->next) node->next->prev = node->prev;

		node->next = max->next;
		node->prev = max->prev;

		if(status){
			insert_key_bufer(bufer, node);
		}

		return remove_key_node(&max, node, bufer);

	}else if(node->left){
		if(!node->par){
			*element = node->left;
			node->left->par = NULL;
		}else if(node == node->par->left){
			node->par->left = node->left;
			node->left->par = node->par;
		}else{
			node->par->right = node->left;
			node->left->par = node->par;
		}
	}else if(node->right){
		if(!node->par){
			*element = node->right;
			node->right->par = NULL;
		}else if(node == node->par->left){
			node->par->left = node->right;
			node->right->par = node->par;
		}else{
			node->par->right = node->right;
			node->right->par = node->par;
		}
	}else{
		if(!node->par){
			*element = NULL;
		}else if(node == node->par->left){
			node->par->left = NULL;
		}else{
			node->par->right = NULL;
		}
	}
	if(target){
		if(node->prev) node->prev->next = target;
		if(node->next) node->next->prev = target;
	}else{
		if(node->prev) node->prev->next = node->next;
		if(node->next) node->next->prev = node->prev;
	}

	if(!target){
		remove_key_bufer(bufer, node->data->key);
	}

	if(node->left){
		son = node->left;
	}else if(node->right){
		son = node->right;
	}else{
		son = NULL;
	}

	Node** relatives = (Node**)malloc(2*sizeof(Node*));
	relatives[0] = son;
	relatives[1] = node->par;

	free(node);

	return relatives;
}

Node** remove_age_node(Node** element, Node* target, Bufer* bufer){
	Node* node = *element;
	Node* son;

	if(node->left && node->right){
		Node* max = find_max_node(node->left);

		Node* status = find_age_bufer(bufer, max->data->age);
		
		remove_age_bufer(bufer, node->data->age);
		remove_age_bufer(bufer, max->data->age);

		node->data = max->data;

		if(node->prev) node->prev->next = node->next;
		if(node->next) node->next->prev = node->prev;

		node->next = max->next;
		node->prev = max->prev;

		if(status){
			insert_age_bufer(bufer, node);
		}

		return remove_age_node(&max, node, bufer);

	}else if(node->left){
		if(!node->par){
			*element = node->left;
			node->left->par = NULL;
		}else if(node == node->par->left){
			node->par->left = node->left;
			node->left->par = node->par;
		}else{
			node->par->right = node->left;
			node->left->par = node->par;
		}
	}else if(node->right){
		if(!node->par){
			*element = node->right;
			node->right->par = NULL;
		}else if(node == node->par->left){
			node->par->left = node->right;
			node->right->par = node->par;
		}else{
			node->par->right = node->right;
			node->right->par = node->par;
		}
	}else{
		if(!node->par){
			*element = NULL;
		}else if(node == node->par->left){
			node->par->left = NULL;
		}else{
			node->par->right = NULL;
		}
	}
	if(target){
		if(node->prev) node->prev->next = target;
		if(node->next) node->next->prev = target;
	}else{
		if(node->prev) node->prev->next = node->next;
		if(node->next) node->next->prev = node->prev;
	}

	if(!target){
		remove_age_bufer(bufer, node->data->age);
	}

	if(node->left){
		son = node->left;
	}else if(node->right){
		son = node->right;
	}else{
		son = NULL;
	}

	Node** relatives = (Node**)malloc(2*sizeof(Node*));
	relatives[0] = son;
	relatives[1] = node->par;

	free(node);

	return relatives;
}

void delete_rebuild_tree(Node** root, Node* x, Node* parent);

void delete_left_correct(Node** root, Node* parent){
	if(parent->right->balance == 1){
		parent->balance = 0;
		parent->right->balance = 0;
		left_rotate(root, parent);
		if(parent->par){
			delete_rebuild_tree(root, parent->par, parent->par->par);
		}
	}else if(parent->right->balance == -1){
		if(parent->right->left->balance == 1){
			parent->right->left->balance = 0;
			parent->balance = -1;
			parent->right->balance = 0;
		}else if(parent->right->left->balance == 1){
			parent->right->left->balance = 0;
			parent->right->balance = 1;
			parent->balance = 0;
		}else{
			parent->right->left->balance = 0;
			parent->right->balance = 0;
			parent->balance = 0;
		}

		right_rotate(root, parent->right);
		left_rotate(root, parent);

		if(parent->par){
			delete_rebuild_tree(root, parent->par, parent->par->par);
		}
	}else{
		parent->balance = 1;
		parent->right->balance = -1;
		left_rotate(root, parent);
	}
}

void delete_right_correct(Node** root, Node* parent){
	if(parent->left->balance == -1){
		parent->balance = 0;
		parent->left->balance = 0;
		right_rotate(root, parent);
		if(parent->par){
			delete_rebuild_tree(root, parent->par, parent->par->par);
		}
	}else if(parent->left->balance == 1){
		if(parent->left->right->balance == -1){
			parent->left->right->balance = 0;
			parent->balance = 1;
			parent->left->balance = 0;
		}
		else if(parent->left->right->balance == 1){
			parent->left->right->balance = 0;
			parent->left->balance = -1;
			parent->balance = 0;
		}
		else{
			parent->left->right->balance = 0;
			parent->left->balance = 0;
			parent->balance = 0;
		}

		left_rotate(root, parent->left);
		right_rotate(root, parent);

		if(parent->par){
			delete_rebuild_tree(root, parent->par, parent->par->par);
		}
	}else{
		parent->balance = -1;
		parent->left->balance = 1;
		right_rotate(root, parent);
	}
}


void delete_rebuild_tree(Node** root, Node* node, Node* parent){
	while(parent){
		if(parent->left || parent->right){
			if(parent->left == node)
				parent->balance += 1;
			else 
				parent->balance -= 1;
		}else
			parent->balance = 0;
		
		if(abs(parent->balance) > 1){
			if(parent->right == node){
				delete_right_correct(root, parent);
			}else{
				delete_left_correct(root, parent);
			}
			break;
		}else if(abs(parent->balance) == 1){
			break;
		}else{
			node = parent;
			parent = node->par;
		}
	}
}


void delete_key_tree(Tree* tree, size_t key){

	Node* buf_find = find_key_bufer(&tree->bufer, key);
	Node* node;
	Node* son;

	if(buf_find){
		node = buf_find;
	}else{
		node = find_key_node(tree->root, key);
	}

	if(!node) return;

	Node** relatives;

	if(node == tree->root){
		relatives = remove_key_node(&tree->root, NULL, &tree->bufer);
	}
	else{
		relatives = remove_key_node(&node, NULL, &tree->bufer);
	}
	tree->root_size--;

	if(!tree->root){
		free(relatives);
		return;
	}

	delete_rebuild_tree(&tree->root, relatives[0], relatives[1]);
	free(relatives);
}

void delete_age_tree(Tree* tree, unsigned int age){
	Node* buf_find = find_age_bufer(&tree->bufer, age);
	Node* node;
	Node* son;

	if(buf_find){
		node = buf_find;
	}else{
		node = find_age_node(tree->root, age);
	}

	Node** relatives;

	if(!node) return;
	if(node == tree->root){
		relatives = remove_age_node(&tree->root, NULL, &tree->bufer);
	}
	else{
		relatives = remove_age_node(&node, NULL, &tree->bufer);
	}
	tree->root_size--;

	if(!tree->root){
		free(relatives);
		return;
	}

	delete_rebuild_tree(&tree->root, relatives[0], relatives[1]);
	free(relatives);
}

int delete_cache(Cache* cache, size_t key){
	if(!(cache->capacity)){
		return -1;
	}
	Data* data = find_key_tree(&cache->key_sorted, key);
	if(!data){
		return -1;
	}

	delete_key_tree(&cache->key_sorted, data->key);
	delete_age_tree(&cache->age_sorted, data->age);

	free(data);
	cache->capacity--;
}

int insert_cache(Cache* cache, size_t key, void* info){
	Data* finded = find_key_tree(&cache->key_sorted, key);
	if(finded){
		return -1;
	}

	if(cache->capacity == SIZE){
		Node* find = find_min_node(cache->age_sorted.root);
		Data* tmp = find->data;

		delete_key_tree(&cache->key_sorted, find->data->key);
		delete_age_tree(&cache->age_sorted, find->data->age);

		free(tmp);
		cache->capacity--;
	}

	Data* data = (Data*)malloc(sizeof(Data));
	data->info = info;
	data->key = key;
	cache->max_age++;
	data->age = cache->max_age;

	insert_key_tree(&cache->key_sorted, data);
	insert_age_tree(&cache->age_sorted, data);
	cache->capacity++;

	return 0;
}

void print_cache(Cache cache);

const void* find_cache(Cache* cache, size_t key){
	if(!cache->capacity){
		return NULL;
	}
	Data* data = find_key_tree(&cache->key_sorted, key);
	if(!data){
		return NULL;
	}

	void* info = data->info;
	delete_cache(cache, key);
	insert_cache(cache, key, info);

	return info;
}

void print_node(Node* node, int keyage){
	while(node){
		if(keyage){
			printf("%d ", node->data->age);
		}else{
			printf("%d ", node->data->key);
		}
		node = node->next;
	}
}

void print_tree(Node* root, int keyage){
	Node* start = find_min_node(root);
	print_node(start, keyage);
}

void format_print_node(Node* node, int pad, int dir, char* pos, int keyage){
	int i;

	for(i = 0; i < pad; i++){
		if((pad - i) % 4 == 0 && pos[i / 4] == 1) printf("\033[1;33m│\033[0m");
		else printf(" ");
	}
	int next_dash;
	if(dir){
		printf("\033[1;32m└──>\033[0m");
		pos[i / 4] = 0;
	}
	else {
		printf("\033[1;35m├──>\033[0m");
		pos[i / 4] = 1;
	}
	if(node){
		if(keyage){
			printf("%d ", node->data->age);
		}else{
			printf("%d ", node->data->key);
		}

		if(node->left || node->right){
			format_print_node(node->left, pad + 4, 0, pos, keyage);
			format_print_node(node->right, pad + 4, 1, pos, keyage);
		}
	}else{
		printf("\033[1;31m-\033[0m\n");
	}
}

int height_node(Node* node){
	if(!node){
		return 0;
	}else{
		int left_height = height_node(node->left);
		int right_height = height_node(node->right);
		return left_height > right_height ? left_height + 1 : right_height + 1;
	}
}

void format_print_tree(Node* root, int keyage){
	int depth = height_node(root);
	char* pos = (char*)calloc(1, depth);
	format_print_node(root, 0, 1, pos, keyage);
	free(pos);
}

void print_cache(Cache cache){
	print_tree(cache.key_sorted.root, 0);
	printf("\n");
	print_tree(cache.age_sorted.root, 1);
	printf("\n\n");
}

void init_cache(Cache* cache){
	memset(cache, 0, sizeof(Cache));
	cache->key_sorted.alfa = 0.5;
	cache->age_sorted.alfa = 0.5;

}

void clear_key_node(Node* node){
	if(node){
		free(node->data);
		clear_key_node(node->left);
		clear_key_node(node->right);
		free(node);
	}
}

void clear_key_tree(Tree* tree){
	clear_key_node(tree->root);
	tree->root = NULL;
	memset(&tree->bufer, 0, sizeof(tree->bufer));
}

void clear_age_node(Node* node){
	if(node){
		clear_age_node(node->left);
		clear_age_node(node->right);
		free(node);
	}
}

void clear_age_tree(Tree* tree){
	clear_age_node(tree->root);
	tree->root = NULL;
	memset(&tree->bufer, 0, sizeof(tree->bufer));
}

void clear_cache(Cache* cache){
	clear_key_tree(&cache->key_sorted);
	clear_age_tree(&cache->age_sorted);
	cache->capacity = 0;
	cache->max_age = 0;
}
