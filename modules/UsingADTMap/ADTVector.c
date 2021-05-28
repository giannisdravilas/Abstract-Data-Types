///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Vector μέσω ADT Map.
//
///////////////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>

#include "ADTVector.h"
#include "ADTMap.h"

int* create_int(int value) {
	int* p = malloc(sizeof(int));
	*p = value;
	return p;
}

int compare_ints1(Pointer a, Pointer b){
	return *(int*)a - *(int*)b;
}

// Ενα Vector είναι pointer σε αυτό το struct
struct vector {
	Map map;
	DestroyFunc destroy_value;
};


Vector vector_create(int size, DestroyFunc destroy_value) {
	Vector vec = malloc(sizeof(*vec));
	vec->map = map_create(compare_ints1, free, destroy_value);
	for(int i = 0; i < size; i++){
		map_insert(vec->map, create_int(i), NULL);
	}
	vec->destroy_value = destroy_value;
	return vec;
}

int vector_size(Vector vec) {
	return map_size(vec->map);
}

Pointer vector_get_at(Vector vec, int pos) {
	return map_find(vec->map, &pos);
}

void vector_set_at(Vector vec, int pos, Pointer value) {
	MapNode found = map_find_node(vec->map, &pos);
	if(found){
		map_insert(vec->map, create_int(pos), value);
	}else{
		vector_insert_last(vec, value);
	}
}

void vector_insert_last(Vector vec, Pointer value) {
	Pointer ppos = create_int(map_size(vec->map));
	map_insert(vec->map, ppos, value);
}

void vector_remove_last(Vector vec) {
	int pos = map_size(vec->map)-1;
	map_remove(vec->map, &pos);
}

Pointer vector_find(Vector vec, Pointer value, CompareFunc compare) {
	for(MapNode node = map_first(vec->map); node != MAP_EOF; node = map_next(vec->map, node)){
		if(compare(map_node_value(vec->map, node), value) == 0)
			return map_node_value(vec->map, node);
	}
	return NULL;
}

DestroyFunc vector_set_destroy_value(Vector vec, DestroyFunc destroy_value) {
	DestroyFunc old_value = vec->destroy_value;
	vec->destroy_value = destroy_value;
	return old_value;
}

void vector_destroy(Vector vec) {
	map_destroy(vec->map);
	free(vec);
}


// Συναρτήσεις για διάσχιση μέσω node /////////////////////////////////////////////////////

VectorNode vector_first(Vector vec) {
	int pos = 0;
	return (VectorNode)map_find_node(vec->map, &pos);
}

VectorNode vector_last(Vector vec) {
	int pos = map_size(vec->map)-1;
	return (VectorNode)map_find_node(vec->map, &pos);
}

VectorNode vector_next(Vector vec, VectorNode node) {
	Pointer key = map_node_key(vec->map, (MapNode)node);
	int pos = *(int*)key+1;
	return (VectorNode)map_find_node(vec->map, &pos);
}

VectorNode vector_previous(Vector vec, VectorNode node) {
	Pointer key = map_node_key(vec->map, (MapNode)node);
	int pos = *(int*)key-1;
	return (VectorNode)map_find_node(vec->map, &pos);
}

Pointer vector_node_value(Vector vec, VectorNode node) {
	if(node){
		return map_node_value(vec->map, (MapNode)node);
	}
	return NULL;
}

VectorNode vector_find_node(Vector vec, Pointer value, CompareFunc compare) {
	for(MapNode node = map_first(vec->map); node != MAP_EOF; node = map_next(vec->map, node)){
		if(compare(map_node_value(vec->map, node), value) == 0){
			return (VectorNode)node;
		}
	}
	return NULL;
}