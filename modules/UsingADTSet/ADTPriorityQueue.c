///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Priority Queue μέσω σωρού.
//
///////////////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>

#include <stdio.h>

#include "ADTPriorityQueue.h"
#include "ADTVector.h"
#include "ADTSet.h"			// Η υλοποίηση του PriorityQueue χρησιμοποιεί Set

//Ένα PriorityQueue είναι pointer σε αυτό το struct
struct priority_queue {
	Set set;				    // Τα δεδομένα, σε Set ώστε να έχουμε μεταβλητό μέγεθος χωρίς κόπο
	CompareFunc compare;		// Η διάταξη
	DestroyFunc destroy_value;	// Συνάρτηση που καταστρέφει ένα στοιχείο του set.
};

PriorityQueue pqueue_create(CompareFunc compare, DestroyFunc destroy_value, Vector values){
    	assert(compare != NULL);	// LCOV_EXCL_LINE

        //Δημιουργούμε την ουρά προτεραιότητας.
        PriorityQueue pqueue = malloc(sizeof(*pqueue));
	    pqueue->compare = compare;
	    pqueue->destroy_value = destroy_value;
        pqueue->set = set_create(compare, NULL);

        //Αν υπάρχει vector για αρχικοποίηση, τότε αρχικοποιούμε την ουρά προτεραιότητας με αυτό.
        if(values != NULL){
            for(int i = 0; i < vector_size(values); i++){
                pqueue_insert(pqueue, vector_get_at(values, i));
            }
        }
        return pqueue;
}

int pqueue_size(PriorityQueue pqueue){
    return set_size(pqueue->set);
}

Pointer pqueue_max(PriorityQueue pqueue){
    //Το μέγιστο στοιχείο της ουράς προτεραιότητας βρίσκεται στο τέλος του set.
    return set_node_value(pqueue->set, set_last(pqueue->set));
}

PriorityQueueNode pqueue_insert(PriorityQueue pqueue, Pointer value){
    //Προσθέτουμε την τιμή στο set.
    set_insert(pqueue->set, value);
    //Ο κόμβος της ουράς προτεραιότητας θα είναι στην ουσία ο κόμβος του set, με cast.
    PriorityQueueNode node = (PriorityQueueNode)set_find_node(pqueue->set, value);
    return node;
}

void pqueue_remove_max(PriorityQueue pqueue){

    int last_node = pqueue_size(pqueue);
	assert(last_node != 0);		// LCOV_EXCL_LINE

    //Αφαιρούμε τον κόμβο από το set.
    Pointer value = pqueue_max(pqueue);
    set_remove(pqueue->set, set_node_value(pqueue->set, set_last(pqueue->set)));

    //Και κάνουμε destroy την τιμή, αν υπάρχει destroy_value.
    if (pqueue->destroy_value != NULL)
		pqueue->destroy_value(value);
}

DestroyFunc pqueue_set_destroy_value(PriorityQueue pqueue, DestroyFunc destroy_value){
    DestroyFunc old = pqueue->destroy_value;
	pqueue->destroy_value = destroy_value;
	return old;
}

void pqueue_destroy(PriorityQueue pqueue){
    // Αντί να κάνουμε εμείς destroy τα στοιχεία, είναι απλούστερο να
	// προσθέσουμε τη destroy_value στο set ώστε να κληθεί κατά το set_destroy.
	set_set_destroy_value(pqueue->set, pqueue->destroy_value);
	set_destroy(pqueue->set);
	free(pqueue);
}

Pointer pqueue_node_value(PriorityQueue set, PriorityQueueNode node){
    //Για να βρούμε την τιμή ενός κόμβου της ουράς προτεραιότητας, βρίσκουμε στην ουσία την τιμή
    //του κόμβου του set, αφού οι κόμβοι του set και της ουράς προτεραιότητας στην ουσία είναι το
    //ίδιο πράγμα, λόγω του cast.
    return set_node_value(set->set, (SetNode)node);
}

void pqueue_remove_node(PriorityQueue pqueue, PriorityQueueNode node){
    if(node != NULL){
        //Βρίσκουμε την τιμή value του κόμβου node.
        Pointer value = pqueue_node_value(pqueue, node);
        //Αφαιρούμε τον κόμβο με τιμή value από το set.
        set_remove(pqueue->set, value);
        //Και αποδεσμεύουμε τη μνήμη μου έχει δεσμευτεί δυναμικά.
        if (pqueue->destroy_value != NULL)
		    pqueue->destroy_value(value);
    }
}