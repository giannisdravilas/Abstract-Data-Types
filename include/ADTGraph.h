////////////////////////////////////////////////////////////////////////
//
// ADT Graph
//
// Abstract μη κατευθυνόμενος γράφος με (μη αρνητικά) βάρη.
//
////////////////////////////////////////////////////////////////////////

#pragma once // #include το πολύ μία φορά

#include "common_types.h"
#include "ADTList.h"			// Ορισμένες συναρτήσεις επιστρέφουν λίστες


// Ενα γράφος αναπαριστάται από τον τύπο Graph

typedef struct graph* Graph;

// Δημιουργεί και επιστρέφει ένα γράφο, στον οποίο τα στοιχεία (οι κορυφές)
// συγκρίνονται με βάση τη συνάρτηση compare. Αν destroy_vertex != NULL, τότε
// καλείται destroy_vertex(vertex) κάθε φορά που αφαιρείται μια κορυφή.

Graph graph_create(CompareFunc compare, DestroyFunc destroy_vertex);

// Επιστρέφει τον αριθμό στοιχείων (κορυφών) που περιέχει ο γράφος graph.

int graph_size(Graph graph);

// Προσθέτει μια κορυφή στο γράφο.

void graph_insert_vertex(Graph graph, Pointer vertex);

// Επιστρέφει λίστα με όλες τις κορυφές του γράφου. Η λίστα δημιουργείται σε κάθε
// κληση και είναι ευθύνη του χρήστη να κάνει list_destroy.

List graph_get_vertices(Graph graph);

// Διαγράφει μια κορυφή από τον γράφο (αν υπάρχουν ακμές διαγράφονται επίσης).

void graph_remove_vertex(Graph graph, Pointer vertex);

// Προσθέτει μια ακμή με βάρος weight στο γράφο.

void graph_insert_edge(Graph graph, Pointer vertex1, Pointer vertex2, uint weight);

// Αφαιρεί μια ακμή από το γράφο.

void graph_remove_edge(Graph graph, Pointer vertex1, Pointer vertex2);

// Επιστρέφει το βάρος της ακμής ανάμεσα στις δύο κορυφές, ή UINT_MAX αν δεν είναι γειτονικές.

uint graph_get_weight(Graph graph, Pointer vertex1, Pointer vertex2);

// Επιστρέφει λίστα με τους γείτονες μιας κορυφής. Η λίστα δημιουργείται σε κάθε
// κληση και είναι ευθύνη του χρήστη να κάνει list_destroy.

List graph_get_adjacent(Graph graph, Pointer vertex);

// Επιστρέφει (σε λίστα) το συντομότερο μονοπάτι ανάμεσα στις κορυφές source και
// target, ή κενή λίστα αν δεν υπάρχει κανένα μονοπάτι. Η λίστα δημιουργείται σε
// κάθε κληση και είναι ευθύνη του χρήστη να κάνει list_destroy.

List graph_shortest_path(Graph graph, Pointer source, Pointer target);

// Ελευθερώνει όλη τη μνήμη που δεσμεύει ο γράφος.
// Οποιαδήποτε λειτουργία πάνω στο γράφο μετά το destroy είναι μη ορισμένη.

void graph_destroy(Graph graph);



//// Για την περίπτωση που ο γράφος χρησιμοποιεί πίνακα κατακερματισμού

#include "ADTMap.h"	// for HashFunc type

// Ορίζει τη συνάρτηση κατακερματισμού hash_func για το συγκεκριμένο γράφο.
// Πρέπει να κληθεί μετά την graph_create και πριν από οποιαδήποτε άλλη συνάρτηση.

void graph_set_hash_function(Graph graph, HashFunc hash_func);