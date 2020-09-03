///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT BList μέσω διπλά συνδεδεμένης λίστας.
//
///////////////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>

#include "ADTBList.h"


// Ενα BList είναι pointer σε αυτό το struct
struct blist {
	BListNode dummy;			// χρησιμοποιούμε dummy κόμβο, ώστε ακόμα και η κενή λίστα να έχει έναν κόμβο.
	BListNode last;				// δείκτης στον τελευταίο κόμβο, ή στον dummy (αν η λίστα είναι κενή)
	int size;					// μέγεθος, ώστε η list_size να είναι Ο(1)
	DestroyFunc destroy_value;	// Συνάρτηση που καταστρέφει ένα στοιχείο της λίστας.
};

struct blist_node {
	BListNode previous; //Δείκτης στον προηγούμενο
	BListNode next;		// Δείκτης στον επόμενο
	Pointer value;		// Η τιμή που αποθηκεύουμε στον κόμβο
};


BList blist_create(DestroyFunc destroy_value) {
	// Πρώτα δημιουργούμε το stuct
	BList blist = malloc(sizeof(*blist));
	blist->size = 0;
	blist->destroy_value = destroy_value;

	// Χρησιμοποιούμε dummy κόμβο, ώστε ακόμα και μια άδεια λίστα να έχει ένα κόμβο
	// (απλοποιεί τους αλγορίθμους). Οπότε πρέπει να τον δημιουργήσουμε.
	//
	blist->dummy = malloc(sizeof(*blist->dummy));
	blist->dummy->previous = NULL;	//άδεια λίστα, ο dummy δεν έχει προηγούμενο
	blist->dummy->next = NULL;		// άδεια λίστα, ο dummy δεν έχει επόμενο

	// Σε μια κενή λίστα, τελευταίος κόμβος είναι επίσης ο dummy
	blist->last = blist->dummy;

	return blist;
}

int blist_size(BList blist) {
	return blist->size;
}

void blist_insert(BList blist, BListNode node, Pointer value) {

	// Δημιουργία του νέου κόμβου
	BListNode new = malloc(sizeof(*new));
	new->value = value;

	//Αν προσθέτουμε τον νέο κόμβο στο τέλος της λίστας, τότε κάνουμε τις απαραίτητες αλλαγές στους δείκτες.
	if(node == BLIST_EOF){
		new->next = BLIST_EOF;
		new->previous = blist->last;
		blist->last->next = new;
		blist->last = new;

	//Αν προσθέτουμε τον νέο κόμβο οπουδήποτε αλλού στη λίστα, τότε κάνουμε τις απαραίτητες αλλαγές στους δείκτες.
	}else{
		node->previous->next = new;
		new->next = node;
		new->previous = node->previous;
		node->previous = new;
	}

	// Ενημέρωση του size
	blist->size++;
}

void blist_remove(BList blist, BListNode node) {

	// Ο κόμβος προς διαγραφή είναι ο node, ο οποίος πρέπει να υπάρχει
	assert(node != NULL);		// LCOV_EXCL_LINE

	if (blist->destroy_value != NULL)
		blist->destroy_value(node->value);

	// Σύνδεση του node με τον επόμενο του removed πριν το free
	node->previous->next = node->next;
	if(node->next != BLIST_EOF)
		node->next->previous = node->previous;

	// Ενημέρωση των size & last
	blist->size--;
	if (blist->last == node)
		blist->last = node->previous;

	free(node);
}

Pointer blist_find(BList blist, Pointer value, CompareFunc compare) {
	BListNode node = blist_find_node(blist, value, compare);
	return node == NULL ? NULL : node->value;
}

DestroyFunc blist_set_destroy_value(BList blist, DestroyFunc destroy_value) {
	DestroyFunc old = blist->destroy_value;
	blist->destroy_value = destroy_value;
	return old;
}

void blist_destroy(BList blist) {
	// Διασχίζουμε όλη τη λίστα και κάνουμε free όλους τους κόμβους,
	// συμπεριλαμβανομένου και του dummy!
	//
	BListNode node = blist->dummy;
	while(node != NULL) {				// while αντί για for, γιατί θέλουμε να διαβάσουμε
		BListNode next = node->next;	// το node->next _πριν_ κάνουμε free!

		// Καλούμε τη destroy_value, αν υπάρχει (προσοχή, όχι στον dummy!)
		if (node != blist->dummy && blist->destroy_value != NULL)
			blist->destroy_value(node->value);

		free(node);

		node = next;
	}

	// Τέλος, free το ίδιο το struct
	free(blist);
}


// Διάσχιση της λίστας /////////////////////////////////////////////

BListNode blist_first(BList blist) {
	// Ο πρώτος κόμβος είναι ο επόμενος του dummy.
	//
	return blist->dummy->next;
}

BListNode blist_last(BList blist) {
	// Προσοχή, αν η λίστα είναι κενή το last δείχνει στον dummy, εμείς όμως θέλουμε να επιστρέψουμε NULL, όχι τον dummy!
	//
	if (blist->last == blist->dummy)
		return BLIST_EOF;		// κενή λίστα
	else
		return blist->last;
}

BListNode blist_next(BList blist, BListNode node) {
	assert(node != NULL);	// LCOV_EXCL_LINE (αγνοούμε το branch από τα coverage reports, είναι δύσκολο να τεστάρουμε το false γιατί θα κρασάρει το test)
	return node->next;
}

BListNode blist_previous(BList blist, BListNode node) {
	assert(node != NULL);	// LCOV_EXCL_LINE (αγνοούμε το branch από τα coverage reports, είναι δύσκολο να τεστάρουμε το false γιατί θα κρασάρει το test)
	
	//Ελέγχουμε αν ο προηγούμενος είναι ο dummy
	if(node->previous->previous != NULL){
		return node->previous;
	}else{
		return BLIST_EOF;
	}
}

Pointer blist_node_value(BList blist, BListNode node) {
	assert(node != NULL);	// LCOV_EXCL_LINE
	return node->value;
}

BListNode blist_find_node(BList blist, Pointer value, CompareFunc compare) {
	// διάσχιση όλης της λίστας, καλούμε την compare μέχρι να επιστρέψει 0
	//
	for(BListNode node = blist->dummy->next; node != NULL; node = node->next)
		if(compare(value, node->value) == 0)
			return node;		// βρέθηκε

	return NULL;	// δεν υπάρχει
}