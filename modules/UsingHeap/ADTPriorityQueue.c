///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Priority Queue μέσω σωρού.
//
///////////////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>

#include "ADTPriorityQueue.h"
#include "ADTVector.h"			// Η υλοποίηση του PriorityQueue χρησιμοποιεί Vector

// Ενα PriorityQueue είναι pointer σε αυτό το struct
struct priority_queue {
	Vector vector;				// Τα δεδομένα, σε Vector ώστε να έχουμε μεταβλητό μέγεθος χωρίς κόπο
	CompareFunc compare;		// Η διάταξη
	DestroyFunc destroy_value;	// Συνάρτηση που καταστρέφει ένα στοιχείο του vector.
};

//Ένα PriorityQueueNode είναι pointer σε αυτό το struct.
struct priority_queue_node{
	Pointer value;
	Pointer value_old;
	int pos;
};


// Βοηθητικές συναρτήσεις ////////////////////////////////////////////////////////////////////////////

// Προσοχή: στην αναπαράσταση ενός complete binary tree με πίνακα, είναι βολικό τα ids των κόμβων να
// ξεκινάνε από το 1 (ρίζα), το οποίο απλοποιεί τις φόρμουλες για εύρεση πατέρα/παιδιών. Οι θέσεις
// ενός vector όμως ξεκινάνε από το 0. Θα μπορούσαμε απλά να αφήσουμε μία θέση κενή, αλλά δεν είναι ανάγκη,
// μπορούμε απλά να αφαιρούμε 1 όταν διαβάζουμε/γράφουμε στο vector. Για απλοποίηση του κώδικα, η
// πρόσβαση στα στοιχεία του vector γίνεται από τις παρακάτω 2 βοηθητικές συναρτήσεις.

// Επιστρέφει την τιμή του κόμβου node_id

static Pointer node_value(PriorityQueue pqueue, int node_id) {
	// τα node_ids είναι 1-based, το node_id αποθηκεύεται στη θέση node_id - 1
	PriorityQueueNode node = (PriorityQueueNode)vector_get_at(pqueue->vector, node_id - 1);
	return (node->value);
}

// Ανταλλάσσει τις τιμές των κόμβων node_id1 και node_id2

static void node_swap(PriorityQueue pqueue, int node_id1, int node_id2) {
	// τα node_ids είναι 1-based, το node_id αποθηκεύεται στη θέση node_id - 1
	
	PriorityQueueNode node1 = (PriorityQueueNode)vector_get_at(pqueue->vector, node_id1-1);
	PriorityQueueNode node2 = (PriorityQueueNode)vector_get_at(pqueue->vector, node_id2-1);

	//Ανταλλάσσουμε τις μεταβλητές που αποθηκεύουν θέσεις των κόμβων.
	int temp = node1->pos;
	node1->pos = node2->pos;
	node2->pos = temp;

	//Ανταλλάσσουμε τους κόμβους στο vector.
	vector_set_at(pqueue->vector, node_id1 - 1, (Pointer)node2);
	vector_set_at(pqueue->vector, node_id2 - 1, (Pointer)node1);
}

// Αποκαθιστά την ιδιότητα του σωρού.
// Πριν: όλοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού, εκτός από
//       τον node_id που μπορεί να είναι _μεγαλύτερος_ από τον πατέρα του.
// Μετά: όλοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού.

static void bubble_up(PriorityQueue pqueue, int node_id) {
	// Αν φτάσαμε στη ρίζα, σταματάμε
	if (node_id == 1)
		return;

	int parent = node_id / 2;		// Ο πατέρας του κόμβου. Τα node_ids είναι 1-based

	// Αν ο πατέρας έχει μικρότερη τιμή από τον κόμβο, swap και συνεχίζουμε αναδρομικά προς τα πάνω
	if (pqueue->compare(node_value(pqueue, parent), node_value(pqueue, node_id)) < 0) {
		node_swap(pqueue, parent, node_id);
		bubble_up(pqueue, parent);
	}
}

// Αποκαθιστά την ιδιότητα του σωρού.
// Πριν: όλοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού, εκτός από τον
//       node_id που μπορεί να είναι _μικρότερος_ από κάποιο από τα παιδιά του.
// Μετά: όλοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού.

static void bubble_down(PriorityQueue pqueue, int node_id) {
	// βρίσκουμε τα παιδιά του κόμβου (αν δεν υπάρχουν σταματάμε)
	int left_child = 2 * node_id;
	int right_child = left_child + 1;

	int size = pqueue_size(pqueue);
	if (left_child > size)
		return;

	// βρίσκουμε το μέγιστο από τα 2 παιδιά
	int max_child = left_child;
	if (right_child <= size && pqueue->compare(node_value(pqueue, left_child), node_value(pqueue, right_child)) < 0)
			max_child = right_child;

	// Αν ο κόμβος είναι μικρότερος από το μέγιστο παιδί, swap και συνεχίζουμε προς τα κάτω
	if (pqueue->compare(node_value(pqueue, node_id), node_value(pqueue, max_child)) < 0) {
		node_swap(pqueue, node_id, max_child);
		bubble_down(pqueue, max_child);
	}
}

// static void naive_heapify(PriorityQueue pqueue, Vector values) {
// 	// Απλά κάνουμε insert τα στοιχεία ένα ένα.
// 	// TODO: υπάρχει πιο αποδοτικός τρόπος να γίνει αυτό!
// 	int size = vector_size(values);
// 	for (int i = 0; i < size; i++)
// 		pqueue_insert(pqueue, vector_get_at(values, i));
// }

// Αρχικοποιεί το σωρό από τα στοιχεία του vector values σε O(n) και όχι σε O(nlogn) όπως η naive_heapify.

static void heapify(PriorityQueue pqueue, Vector values){
	int size = vector_size(values);
	for(int i = 0; i < size; i++){
		pqueue_insert(pqueue, vector_get_at(values, i));
	}
	for(int i = pqueue_size(pqueue)/2; i > 0; i--){
		bubble_down(pqueue, i);
	}
}


// Συναρτήσεις του ADTPriorityQueue //////////////////////////////////////////////////

PriorityQueue pqueue_create(CompareFunc compare, DestroyFunc destroy_value, Vector values) {
	assert(compare != NULL);	// LCOV_EXCL_LINE

	//Δημιουργούμε την ουρά προτεραιότητας
	PriorityQueue pqueue = malloc(sizeof(*pqueue));
	pqueue->compare = compare;
	pqueue->destroy_value = destroy_value;

	// Δημιουργία του vector που αποθηκεύει τα στοιχεία.
	// ΠΡΟΣΟΧΗ: ΔΕΝ περνάμε την destroy_value στο vector!
	// Αν την περάσουμε θα καλείται όταν κάνουμε swap 2 στοιχεία, το οποίο δεν το επιθυμούμε.
	pqueue->vector = vector_create(0, NULL);

	// Αν values != NULL, αρχικοποιούμε το σωρό.
	if (values != NULL)
		heapify(pqueue, values);

	return pqueue;
}

int pqueue_size(PriorityQueue pqueue) {
	return vector_size(pqueue->vector);
}

Pointer pqueue_max(PriorityQueue pqueue) {
	return node_value(pqueue, 1);		// root
}

PriorityQueueNode pqueue_insert(PriorityQueue pqueue, Pointer value) {

	//Δημιουργούμε τον κόμβο της της ουράς προτεραιότητας
	PriorityQueueNode node = malloc(sizeof(*node));
	node->pos = pqueue_size(pqueue)+1;
	node->value = value;
	node->value_old = value;

	// Προσθέτουμε έναν δείκτη στον κόμβο της ουράς προτεραιότητας, στο τέλος του σωρού
	vector_insert_last(pqueue->vector, (Pointer)node);

 	// Ολοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού εκτός από τον τελευταίο, που μπορεί να είναι
	// μεγαλύτερος από τον πατέρα του. Αρα μπορούμε να επαναφέρουμε την ιδιότητα του σωρού καλώντας
	// τη bubble_up γα τον τελευταίο κόμβο (του οποίου το 1-based id ισούται με το νέο μέγεθος του σωρού).
	bubble_up(pqueue, pqueue_size(pqueue));

	return node;
}

void pqueue_remove_max(PriorityQueue pqueue) {
	int last_node = pqueue_size(pqueue);
	assert(last_node != 0);		// LCOV_EXCL_LINE

	// Αντικαθιστούμε τον πρώτο κόμβο με τον τελευταίο και αφαιρούμε τον τελευταίο
	node_swap(pqueue, 1, last_node);
	
	//Η διεύθυνση του κόμβου είναι αποθηκευμένη στο τελευταίο στοιχείο του vector.
	PriorityQueueNode node = vector_get_at(pqueue->vector, vector_size(pqueue->vector)-1);

	// Destroy την τιμή που αφαιρείται
	if (pqueue->destroy_value != NULL){
		(pqueue->destroy_value)(node->value);
		free(node);
	}else{
		free(node);
	}
	
	vector_remove_last(pqueue->vector);

 	// Ολοι οι κόμβοι ικανοποιούν την ιδιότητα του σωρού εκτός από τη νέα ρίζα
 	// που μπορεί να είναι μικρότερη από κάποιο παιδί της. Αρα μπορούμε να
 	// επαναφέρουμε την ιδιότητα του σωρού καλώντας τη bubble_down για τη ρίζα.
	bubble_down(pqueue, 1);
}

DestroyFunc pqueue_set_destroy_value(PriorityQueue pqueue, DestroyFunc destroy_value) {
	DestroyFunc old = pqueue->destroy_value;
	pqueue->destroy_value = destroy_value;
	return old;
}

void pqueue_destroy(PriorityQueue pqueue) {
	// Αντί να κάνουμε εμείς destroy τα στοιχεία, είναι απλούστερο να
	// προσθέσουμε τη destroy_value στο vector ώστε να κληθεί κατά το vector_destroy.
	vector_set_destroy_value(pqueue->vector, pqueue->destroy_value);
	
	int size = vector_size(pqueue->vector);
	
	//Αφαιρούμε size-1 φορές το τελευταίο στοιχείο του vector, αφού κάθε φορά έχουμε πρόσβαση
	//μόνο στο τελευταίο στοιχείο του.
	for(int i = 0; i < size; i++){
		PriorityQueueNode node = vector_get_at(pqueue->vector, vector_size(pqueue->vector)-1);
		if(pqueue->destroy_value != NULL){
			(pqueue->destroy_value)(node->value);
			vector_remove_last(pqueue->vector);
		}else{
			free(node);
			vector_remove_last(pqueue->vector);

		}
	}

	vector_destroy(pqueue->vector);

	free(pqueue);
}



//// Νέες συναρτήσεις για την εργασία 2 //////////////////////////////////////////

Pointer pqueue_node_value(PriorityQueue set, PriorityQueueNode node) {
	return node->value;
}

void pqueue_remove_node(PriorityQueue pqueue, PriorityQueueNode node) {

	int pos = node->pos;

	//Ανταλλάσσουμε τον κόμβο που θέλουμε να αφαιρέσουμε με τον τελευταίο και αφαιρούμε τον τελευταίο.
	node_swap(pqueue, pqueue_size(pqueue), node->pos);
	(pqueue->destroy_value)(node->value);
	(pqueue->destroy_value)(node);
	vector_remove_last(pqueue->vector);

	//Αποκαθιστούμε την ιδιότητα του σωρού.
	bubble_down(pqueue, pos);

}

void pqueue_update_order(PriorityQueue pqueue, PriorityQueueNode node) {
	
	//Βρίσκουμε ποια είναι η τιμή που είχε πριν ο κόμβος και ποια η τιμή που ανατέθηκε τελευταία
	//και τις συγκρίνουμε ώστε να κάνουμε ανάλογα bubble_up() ή bubble_down().
	int old_value = *(int*)node->value_old;
	if(old_value > *(int*)node->value){
		bubble_up(pqueue, node->pos);
	}else{
		bubble_down(pqueue, node->pos);
	}
}
