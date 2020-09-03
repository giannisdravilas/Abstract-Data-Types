///////////////////////////////////////////////////////////////////
//
// ADT BList
//
// Abstract bidirectional λίστα. Παρέχει σειριακή πρόσβαση στα
// στοιχεία και στις δύο κατευθύνσεις, και προσθήκη/αφαίρεση σε
// οποιοδήποτε σημείο της λίστας.
//
///////////////////////////////////////////////////////////////////

#pragma once // #include το πολύ μία φορά

#include "common_types.h"

// Οι σταθερές αυτές συμβολίζουν εικονικούς κόμβους _πριν_ τον πρώτο και _μετά_ τον τελευταίο
#define BLIST_BOF (BListNode)0
#define BLIST_EOF (BListNode)0


// Λίστες και κόμβοι αναπαριστώνται από τους τύπους BList και BListNode. Ο χρήστης δε χρειάζεται να γνωρίζει το περιεχόμενο
// των τύπων αυτών, απλά χρησιμοποιεί τις συναρτήσεις blist_<foo> που δέχονται και επιστρέφουν BList / BListNode.
//
// Οι τύποι αυτοί ορίζινται ως pointers στα "struct blist" και "struct blist_node" των οποίων το
// περιεχόμενο είναι άγνωστο (incomplete structs), και εξαρτάται από την υλοποίηση του ADT BList.
//
typedef struct blist* BList;
typedef struct blist_node* BListNode;



// Δημιουργεί και επιστρέφει μια νέα λίστα.
// Αν destroy_value != NULL, τότε καλείται destroy_value(value) κάθε φορά που αφαιρείται ένα στοιχείο.

BList blist_create(DestroyFunc destroy_value);

// Επιστρέφει τον αριθμό στοιχείων που περιέχει η λίστα.

int blist_size(BList blist);

// Προσθέτει έναν νέο κόμβο __πριν__ τον node (δηλαδή αν ο node είχε θέση i στη λίστα, o νέος κόμβος
// παίρνει τη θέση i και ο node πηγαίνει στην i+1), ή στo τέλος αν node == BLIST_EOF, με περιεχόμενο value.

void blist_insert(BList blist, BListNode node, Pointer value);

// Αφαιρεί τον κόμβο node (πρέπει να υπάρχει στη λίστα).

void blist_remove(BList blist, BListNode node);

// Επιστρέφει την πρώτη τιμή που είναι ισοδύναμη με value
// (με βάση τη συνάρτηση compare), ή NULL αν δεν υπάρχει

Pointer blist_find(BList blist, Pointer value, CompareFunc compare);

// Αλλάζει τη συνάρτηση που καλείται σε κάθε αφαίρεση/αντικατάσταση στοιχείου σε
// destroy_value. Επιστρέφει την προηγούμενη τιμή της συνάρτησης.

DestroyFunc blist_set_destroy_value(BList blist, DestroyFunc destroy_value);

// Ελευθερώνει όλη τη μνήμη που δεσμεύει η λίστα blist.
// Οποιαδήποτε λειτουργία πάνω στη λίστα μετά το destroy είναι μη ορισμένη.

void blist_destroy(BList blist);


// Διάσχιση της λίστας /////////////////////////////////////////////
//
// Επιστρέφουν τον πρώτο και τον τελευταίο κομβο της λίστας, ή BLIST_BOF / BLIST_EOF αντίστοιχα αν η λίστα είναι κενή

BListNode blist_first(BList blist);
BListNode blist_last(BList blist);

// Επιστρέφουν τον επόμενο και τον προηγούμενο κομβο του node, ή BLIST_EOF / BLIST_BOF
// αντίστοιχα αν ο node δεν έχει επόμενο / προηγούμενο.

BListNode blist_next(BList blist, BListNode node);
BListNode blist_previous(BList blist, BListNode node);

// Επιστρέφει το περιεχόμενο του κόμβου node

Pointer blist_node_value(BList blist, BListNode node);

// Βρίσκει τo πρώτo στοιχείο που είναι ισοδύναμο με value (με βάση τη συνάρτηση compare).
// Επιστρέφει τον κόμβο του στοιχείου, ή BLIST_EOF αν δεν βρεθεί.

BListNode blist_find_node(BList blist, Pointer value, CompareFunc compare);
