#define _GNU_SOURCE

void __attribute__ ((constructor)) malloc_init(void);
void __attribute__ ((destructor)) print_leaks(void);

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

void *(*original_malloc)(size_t size) = NULL;
void (*original_free)(void *ptr) = NULL;

typedef struct node { 	//Define each node of the linked list
	int * p;			//Malloc'd pointer
	int size;			//Size in bytes of the allocated memory
	struct node *next;	//Pointer to the next object in the list or NULL
} node_t;

typedef struct list_tag {	//Define the list
	node_t *list_head;		//First node of the list, contains NULL data
	int list_size;			//The number of nodes in the list
	int total;				//The total number of bytes in the list
} list_t;

typedef list_t * List;

list_t * malloc_list = NULL; //Global list of malloc calls and sizes

//Constructor: allocate memory for the global linked list and set all pointers to NULL
list_t * linked_list_init(void){
	
	list_t *L = (list_t *) original_malloc(sizeof(list_t));
	L->list_head = (node_t *) original_malloc(sizeof(node_t));

	L->list_head->next = NULL;
	L->list_head->p = NULL;
	L->list_head->size = 0;

	L->list_size = 0;
	L->total = 0;
	return L;

}
//Insert a node into the linked list with the malloc'd pointer and its byte size
void insert_node (list_t * L, int malloc_size, void * ptr) {

	L->list_size++;
	L->total = L->total + malloc_size;
	node_t * new_node = original_malloc(sizeof(node_t));
	new_node->size = malloc_size;
	new_node->p = ptr;
	if (L->list_head->next == NULL) {  //Insert a node at the end of the list
		new_node->next = NULL;
		L->list_head->next = new_node;
	}
	else {
		node_t *iterator = L->list_head;
		while (iterator->next != NULL) iterator = iterator->next;
		iterator->next = new_node;
		new_node->next = NULL;
	}
}
//Remove a node from the linked list and free its pointer
void remove_node (list_t * L, void * ptr) {
	int index = 0;
	if (L->list_head->next != NULL) {
		node_t *iterator = L->list_head->next;
		node_t *iterator_prev = L->list_head;
		//Find the pointer we want free'd, unless it doesn't exist in the list
		while (iterator->p != ptr && index < L->list_size) {
			iterator = iterator->next;
			iterator_prev = iterator_prev->next;
			index++;
		}
		if (iterator->p == ptr) { //If we found the pointer, remove it from the list
			L->total = L->total - iterator->size;
			iterator_prev->next = iterator->next;
			iterator->next = NULL;
			original_free(iterator->p);
			original_free(iterator);
			L->list_size--;
		}

	}
	
	return;
}
//Destructor: print the linked list and free it
//This is the number of leaks in the child program
void print_leaks(void)
{
	node_t *iterator = malloc_list->list_head->next;
	while (iterator != NULL) {
		fprintf(stderr, "LEAK\t%d\n", iterator->size);
		node_t *free_node = iterator;
		iterator = iterator->next;
		free_node->next = NULL;
		original_free(free_node->p);
		original_free(free_node);
	}
	fprintf(stderr, "TOTAL\t%d\t%d\n", malloc_list->list_size, malloc_list->total);
	
	original_free(malloc_list->list_head->p);
	original_free(malloc_list->list_head);
	original_free(malloc_list);
	malloc_list = NULL;	
}
//Use lib.c malloc and free when we call original_malloc and original_free
void malloc_init(void)
{

	if (original_malloc == NULL)
		{
			original_malloc = dlsym(RTLD_NEXT, "malloc");
		}
	if (original_free == NULL)
		{
			original_free = dlsym(RTLD_NEXT, "free");
		}

	malloc_list = linked_list_init(); //initialize the global list of mallocs
}
//When malloc is called, use the original malloc to make the pointer and take that and the
//size and pass into the insert node function
void *malloc (size_t size)
{
	void * ptr = original_malloc(size);
	insert_node(malloc_list, size, ptr);
	return ptr;
}

//When free is called, find the pointer in the linked list, then free that pointer and 
//remove from the list
void free (void *ptr)
{
	remove_node(malloc_list, ptr);
}

