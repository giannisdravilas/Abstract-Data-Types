///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Graph μέσω λιστών γειτνίασης.
//
///////////////////////////////////////////////////////////


// Προς υλοποίηση

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "ADTGraph.h"

#include "ADTMap.h"

#include "ADTPriorityQueue.h"

// Μια γειτονική κορυφή αποτελείται από τις ακόλουθες πληροφορίες
struct adj_list_node{
    Pointer vertex;     // Η ίδια η γειτονική κορυφή με τις πληφορορίες της
    uint weight;        // Το βάρος μεταξύ της της αρχικής κορυφής και της γειτονικής
    uint id;            // Το id της κορυφής για να μπορούμε να αναφερόμαστε σε αυτήν με μοναδικό τρόπο
};

// Μια γειτονική κορυφή (AdjListNode) είναι δείκτης σε αυτό το struct
typedef struct adj_list_node* AdjListNode;

// Ένας γράφος είναι η παρακάτω δομή
struct graph{
    int size;                       // Το μέγεθος του γράφου για να έχουμε άμεση πρόσβαση σε αυτό
    Map map;                        // Ένα Map που κρατά τις κορυφές και τις αντίστοιχες λίστες τους με γειτονικά στοιχεία
    Map map_vertices;               // Ένα Map που κρατά τις κορυφές και τα αντίστοιχα ids τους
    CompareFunc compare;            // Η συνάρτηση με βάση την οποία συγκρίνονται τα στοιχεία που περιέχουν οι κορυφές
    DestroyFunc destroy_vertex;     // Η συνάρτηση με την οποία καταστρέφεται μια κορυφή
    HashFunc hash_function;         // Η συνάρτηση με την οποία γίνεται hashing όταν ο Map χρησιμοποιεί Hash Table
    int counter;                    // Ένας μετρητής με βάση τον οποίο δημιουργείται το μοναδικό id κάθε κορυφής
};

// Βοηθητικές global μεταβλητές για την συνάρτηση compare_distances() της graph_shortest_path()
static Graph graph_global;
static int* dist_global;

Graph graph_create(CompareFunc compare, DestroyFunc destroy_vertex){

    //Δεσμεύουμε την απαραίτητη μνήμη και αρχικοποιούμε τα μέλη του struct
    Graph graph = malloc(sizeof(*graph));
    graph->size = 0;
    graph->compare = compare;
    graph->destroy_vertex = destroy_vertex;
    graph->map = map_create(graph->compare, NULL, NULL);
    graph->map_vertices = map_create(graph->compare, NULL, free);
    graph->counter = 0;
    return graph;
}

int graph_size(Graph graph){
    return graph->size;
}

// Δημιουργεί έναν pointer σε κάποιον ακέραιο (create_int1() για να μην υπάρχει σύγχυση με την create_int()
// ADTGraph_test)
int* create_int1(int value) {
	int* p = malloc(sizeof(int));
	*p = value;
	return p;
}

void graph_insert_vertex(Graph graph, Pointer vertex){

    // Αρχικά ψάχνουμε αν η κορυφή υπάρχει ήδη στο Map (στον γράφο)
    MapNode found = map_find_node(graph->map, vertex);

    // Δημιουργούμε μια νέα λίστα γειτνίασης για τη συγκεκριμένη κορυφή
    List list = list_create(NULL);

    // Αν η κορυφή υπάρχει, διαγράφουμε τις υπάρχουσες πληροφορίες της και εισάγουμε τις νέες
    if(found){
        list_destroy(map_node_value(graph->map, found));
        map_remove(graph->map, found);
        map_insert(graph->map, vertex, list);
        int* id = create_int1(*(int*)map_find(graph->map_vertices, vertex));
        map_remove(graph->map_vertices, map_find_node(graph->map_vertices, vertex));
        map_insert(graph->map_vertices, vertex, id);

    // Αν η κορυφή δεν υπάρχει τότε δημιουργούμε καινούρια και εισάγουμε τις πληροφορίες της
    }else{
        map_insert(graph->map, vertex, list);
        graph->size++;
        map_insert(graph->map_vertices, vertex, create_int1(graph->counter));
        graph->counter++;
    }
}

List graph_get_vertices(Graph graph){
    
    // Δημιουργούμε μια λίστα κι έπειτα διατρέχουμε το Map και εισάγουμε κάθε στοιχείο του στη λίστα
    List list = list_create(NULL);
    for(MapNode node = map_first(graph->map); node != MAP_EOF; node = map_next(graph->map, node)){
        list_insert_next(list, list_last(list), map_node_key(graph->map, node));
    }
    return list;

}

void graph_remove_vertex(Graph graph, Pointer vertex){
    
    // Διαγράφουμε την κορυφή από το Map με τα ids
    map_remove(graph->map_vertices, vertex);

    // Διατρέχουμε όλο το Map και ψάχνουμε αν η κορυφή που θέλουμε να διαγράψουμε υπάρχει σε κάποια από
    // τις λίστες γειτνίασης κάποιας άλλης κορυφής, οπότε και την διαγράφουμε
    for(MapNode node = map_first(graph->map); node != MAP_EOF; node = map_next(graph->map, node)){
        List list = map_node_value(graph->map, node);
        ListNode node_prev = LIST_BOF;
        for(ListNode node_list = list_first(list); node_list != LIST_EOF; node_list = list_next(list, node_list)){
            AdjListNode node_adj = list_node_value(list, node_list);
            if(graph->compare(node_adj->vertex, vertex) == 0){
                free(node_adj);
                list_remove_next(list, node_prev);
            }
            node_prev = list_next(list, node_prev);
        }
    }

    // Διαγράφουμε την κορυφή και από το Map με τις κορυφές (που αναπαριστά τον γράφο), καθώς
    // και την αντίστοιχη λίστα της
    if(map_find_node(graph->map, vertex))
        list_destroy(map_node_value(graph->map, map_find_node(graph->map, vertex)));

    // Μειώνουμε το μέγεθος του γράφου κατά 1
    if(map_remove(graph->map, vertex)){
        graph->size--;
    }
}

void graph_insert_edge(Graph graph, Pointer vertex1, Pointer vertex2, uint weight){

    // Ψάχνουμε αν οι δύο κορυφές που θέλουμε να συνδέσουμε, υπάρχουν
    MapNode node1 = map_find_node(graph->map, vertex1);
    MapNode node2 = map_find_node(graph->map, vertex2);

    // Αν υπάρχουν
    if(node1 != NULL && node2 != NULL){

        // Ψάχνουμε αν υπάρχει ήδη ακμή μεταξύ τους
        bool exists = false;
        List list1 = map_node_value(graph->map, node1);
        AdjListNode value;
        for(ListNode node = list_first(list1); node != LIST_EOF; node = list_next(list1, node)){
            if(node != NULL){
                value = list_node_value(list1, node);
                if(graph->compare(value->vertex, vertex2) == 0){
                    exists = true;
                    break;
                }
            }
        }

        // Αν δεν υπάρχει ήδη ακμή μεταξύ τους
        if(!exists){

            // Δημιουργούμε έναν AdjListNode και τον εισάγουμε στη λίστα γειτνίασης της κορυφής
            AdjListNode node_adj = malloc(sizeof(*node_adj));
            node_adj->vertex = vertex2;
            node_adj->weight = weight;
            node_adj->id = *(int*)map_find(graph->map_vertices, vertex2);
            List list = map_node_value(graph->map, node1);
            list_insert_next(list, list_last(list), node_adj);

            // Επαναλαμβάνουμε την ίδια διαδικασία και για τη δεύτερη κορυφή
            node_adj = malloc(sizeof(*node_adj));
            node_adj->vertex = vertex1;
            node_adj->weight = weight;
            node_adj->id = *(int*)map_find(graph->map_vertices, vertex1);
            list = map_node_value(graph->map, node2);
            list_insert_next(list, list_last(list), node_adj);

        // Αν υπάρχει ήδη ακμή μεταξύ των δύο κορυφών
        }else{

            // Ενημερώνουμε το βάρος στην γειτονική κορυφή που βρήκαμε πριν
            value->weight = weight;

            // Ενημερώνουμε το βάρος και στην γειτονική κορυφή της δεύτερης κορυφής, αφού πρώτα
            // την εντοπίσουμε
            List list2 = map_node_value(graph->map, node2);
            for(ListNode node = list_first(list2); node != LIST_EOF; node = list_next(list2, node)){
                value = list_node_value(list2, node);
                if(graph->compare(value->vertex, vertex1)){
                    value->weight = weight;
                    break;
                }
            }
        }
    }
}

void graph_remove_edge(Graph graph, Pointer vertex1, Pointer vertex2){

    // Ψάχνουμε αρχικά αν οι δύο κορυφές των οποίων την ακμή θέλουμε να διαγράψουμε, υπάρχουν
    MapNode node1 = map_find_node(graph->map, vertex1);
    MapNode node2 = map_find_node(graph->map, vertex2);

    // Αν υπάρχουν
    if(node1 != NULL && node2 != NULL){

        // Διατρέχουμε τη λίστα γειτνίασης καθεμιάς από τις δύο κορυφές και βρίσκουμε αν η άλλη
        // κορυφή ανήκει στη λίστα αυτή. Αν ανήκει, τότε τη διαγράφουμε από την λίστα.
        List list = map_node_value(graph->map, node1);
        ListNode node_previous = LIST_BOF;
        for(ListNode node = list_first(list); node != LIST_EOF; node = list_next(list, node)){
            AdjListNode node_adj = list_node_value(list, node);
            if(graph->compare(node_adj->vertex, vertex2) == 0){
                free(node_adj);
                list_remove_next(list, node_previous);
                break;
            }
            node_previous = node;
        }

        // Επαναλαμβάνουμε και για την άλλη κορυφή
        list = map_node_value(graph->map, node2);
        node_previous = LIST_BOF;
        for(ListNode node = list_first(list); node != LIST_EOF; node = list_next(list, node)){
            AdjListNode node_adj = list_node_value(list, node);
            if(graph->compare(node_adj->vertex, vertex1) == 0){
                free(node_adj);
                list_remove_next(list, node_previous);
                break;
            }
            node_previous = node;
        }        
    }
}

uint graph_get_weight(Graph graph, Pointer vertex1, Pointer vertex2){

    // Βρίσκουμε την πρώτη κορυφή στο Map
    MapNode node = map_find_node(graph->map, vertex1);

    // Αν υπάρχει
    if(node != NULL){

        // Διατρέχουμε τη λίστα γειτνίασής της και ψάχνουμε τη δεύτερη κορυφή
        List list = map_node_value(graph->map, node);
        for(ListNode node_list = list_first(list); node_list != LIST_EOF; node_list = list_next(list, node_list)){
            AdjListNode node_adj = list_node_value(list, node_list);

            //Αν βρεθεί, επιστρέφουμε το βάρος μεταξύ τους
            if(graph->compare(vertex2, node_adj->vertex) == 0)
                return node_adj->weight;
        }
    }

    return UINT_MAX;
}

List graph_get_adjacent(Graph graph, Pointer vertex){

    // Ψάχνουμε την κορυφή στο Map
    MapNode node_map = map_find_node(graph->map, vertex);

    // Αν υπάρχει τότε
    if(node_map != NULL){

        // Δημιουργούμε μια νέα λίστα στην οποία εισάγουμε κάθε στοιχείο της λίστας γειτνίασης
        // της αρχικής κορυφής
        List list = map_node_value(graph->map, node_map);
        List list_return = list_create(NULL);
        for(ListNode node = list_first(list); node != LIST_EOF; node = list_next(list, node)){
            AdjListNode node_adj = list_node_value(list, node);
            list_insert_next(list_return, list_last(list_return), node_adj->vertex);
        }
        return list_return;
    }

    return NULL;
}

// Συνάρτηση που συγκρίνει της κορυφές με βάση την απόσταση στον πίνακα dist_global[]. Ο πίνακας είναι global
// γιατί η compare στον κώδικα της εργασίας που δίνεται παίρνει μόνο δύο ορίσματα.
int compare_distances(Pointer a, Pointer b){
    // Συγρκίνουμε με b-a γιατί θέλουμε να πάρουμε την μικρότερη απόσταση παρ' όλο που η PriorityQueue
    // επιστρέφει το max στοιχείο.
    return dist_global[*(int*)map_find(graph_global->map_vertices, b)] - dist_global[*(int*)map_find(graph_global->map_vertices, a)];
}

List graph_shortest_path(Graph graph, Pointer source, Pointer target){

    // Δεσμεύουμε την απαραίτητη μνήμη για:
    // έναν πίνακα W[] που κάθε θέση του αντιστοιχεί σε μια κορυφή μέσω του id της και αποθηκεύει
    // 0 ή 1 ανάλογα με το αν η κορυφή βρίσκεται στο σύνολο W
    // έναν πίνακα dist_global[] που περιέχει κάθε φορά τη μικρότερη απόσταση που έχει βρεθεί για
    // μια κορυφή που αντιστοιχίζεται στη θέση του πίνακα που δείχνει το id της
    // έναν πίνακα prev[] που για κάθε κορυφή (η οποία αντιστοιχίζεται στη θέση του πίνακα που δείχνει
    // το id της) κρατά την προηγούμενη κορυφή στο βέλτιστο μονοπάτι
    List list_return = list_create(NULL);
    int* W = malloc(graph->size * sizeof(int));
    graph_global = graph;
    dist_global = malloc(graph->size * sizeof(int));
    Pointer* prev = malloc(graph->size * sizeof(Pointer));

    // Βρίσκουμε το id της κορυφής source από την οποία ξεκινάμε
    uint source_id = *(int*)map_find(graph->map_vertices, source);

    // Η απόσταση της κορυφής source από τον εαυτό της είναι 0
    dist_global[source_id] = 0;

    // Αντλούμε όλες τις κορυφές του γράφου και τις αποθηκεύουμε σε μια λίστα
    List list = graph_get_vertices(graph);

    // Αρχικοποιούμε τους πίνακες που δημιουργήσαμε νωρίτερα
    for(int i = 0; i < graph->size; i++){
        if(i != source_id){
            dist_global[i] = INT_MAX;
        }
        prev[i] = NULL;
        W[i] = 0;
    }
    
    // Δημιουργούμε μια ουρά προτεραιότητας που θα αποθηκεύει τους κόμβους του γράφου
    PriorityQueue pqueue = pqueue_create(compare_distances, NULL, NULL);

    // Δημιουργούμε έναν πίνακα που αποθηκεύει τους κόμβους της ουράς προτεραιότητας για μελλοντική
    // αναφορά σε αυτούς
    PriorityQueueNode* pqueue_nodes = malloc(graph->size * sizeof(*pqueue_nodes));

    // Αρχικοποιούμε την ουρά προτεραιότητας και τον πίνακα που αποθηκεύει τους κόμβους της με βάση
    // τη λίστα που περιλαμβάνει τις κορυφές του γράφου
    for(ListNode node = list_first(list); node != LIST_EOF; node = list_next(list, node)){
        pqueue_nodes[*(int*)map_find(graph->map_vertices, list_node_value(list, node))] = pqueue_insert(pqueue, list_node_value(list, node));
    }

    // Δε χρειαζόμαστε πλέον τη λίστα με τις κορυφές
    list_destroy(list);

    // Όσο η ουρά προτεραιότητας δεν είναι άδεια
    while(pqueue_size(pqueue)){

        // Αντλούμε το μέγιστο στοιχείο της (αυτό που θα έχει την μικρότερη απόσταση στον πίνακα
        // dist_global[] με βάση την compare_distances()), καθώς και το id του.
        Pointer w = pqueue_max(pqueue);
        pqueue_remove_max(pqueue);
        uint w_id = *(int*)map_find(graph->map_vertices, w);

        // Αν το έχουμε ελέγξει ήδη τότε συνεχίζουμε με το επόμενο στοιχείο
        if(W[w_id]){
            continue;
        }

        // Ενημερώνουμε στον πίνακα W[] ότι έχουμε ελέγξει το τρέχον στοιχείο
        W[w_id] = 1;
        
        // Αν η κορυφή που αντλήσαμε είναι ο target, δηλαδή αυτή στην οποία θέλουμε να φτάσουμε, τότε
        if(graph->compare(w, target) == 0){

            // Αν υπάρχει μονοπάτι προς αυτήν την κορυφή τότε εισάγουμε αρχικά τον target
            if(dist_global[w_id] != INT_MAX){   
                list_insert_next(list_return, LIST_BOF, w);
            }

            // Όσο υπάρχουν προηγούμενες κορυφές (δηλαδή υπάρχει μονοπάτι), προσθέτουμε τις νέες κορυφές
            // στην αρχή της λίστας (για να εισαχθούν εν τέλει με τη σωστή σειρά).
            int k = w_id;
            while(prev[k] != NULL){
                list_insert_next(list_return, LIST_BOF, prev[k]);
                k = *(int*)map_find(graph->map_vertices, prev[k]);
            }
            break;
        }

        // Αντλούμε όλες τις γειτονικές κορυφές της τρέχουσας κορυφής και τις αποθηκεύουμε σε μία λίστα
        list = graph_get_adjacent(graph, w);

        // Για κάθε γειτονική κορυφή που υπάρχει στη λίστα
        for(ListNode node = list_first(list); node != LIST_EOF; node = list_next(list, node)){

            // Βρίσκουμε ποια είναι αυτή η κορυφή
            Pointer u = list_node_value(list, node);
            uint u_id = *(int*)map_find(graph->map_vertices, u);

            // Αν έχει ελεγχθεί τότε συνεχίζουμε με την επόμενη κορυφή στη λίστα γειτνίασης
            if(W[u_id]){
                continue;
            }

            // Βρίσκουμε την νέα απόσταση 
            int alt = dist_global[w_id] + graph_get_weight(graph, w, u);

            // Αν δεν υπάρχει καταχωρισμένη απόσταση στον πίνακα dist_global[] ή η νέα απόσταση
            // είναι μικρότερη από την ήδη καταχωρισμένη
            if((dist_global[u_id] == INT_MAX) || (alt < dist_global[u_id])){

                // Ενημερώνουμε την νέα απόσταση στον σχετικό πίνακα, καθώς και τον πίνακα που
                // αποθηκεύει το μονοπάτι
                dist_global[u_id] = alt;
                prev[u_id] = w;

                // Η απόσταση μέχρι την κορυφή u έχει αλλάξει, άρα ενημερώνουμε την ουρά προτεραιότητας
                // με βάση την νέα απόσταση (η οποία καθορίζει και την διάταξη στην ουρά προτεραιότητας).
                pqueue_update_order(pqueue, pqueue_nodes[u_id]);
            }
        }

        list_destroy(list);
    }

    pqueue_destroy(pqueue);
    free(W);
    free(pqueue_nodes);
    free(dist_global);
    free(prev);
    return list_return;
}

void graph_destroy(Graph graph){

    // Για κάθε κορυφή του Map βρίσκουμε τη λίστα γειτνίασής της και καταστρέφουμε τα στοιχεία της
    for(MapNode node = map_first(graph->map); node != MAP_EOF; node = map_next(graph->map, node)){
        List list = map_node_value(graph->map, node);
        for(ListNode node_list = list_first(list); node_list != LIST_EOF; node_list = list_next(list, node_list)){
            AdjListNode node_adj = list_node_value(list, node_list);
            free(node_adj);
        }

        // Έπειτα καταστρέφουμε την ίδια τη λίστα γειτνίασης
        list_destroy(list);
    }

    // Τέλος καταστρέφουμε τα Maps
    map_destroy(graph->map);

    map_destroy(graph->map_vertices);

    free(graph);
}

void graph_set_hash_function(Graph graph, HashFunc hash_func){

    // Περνάμε την hash function στα Maps
    graph->hash_function = hash_func;
    map_set_hash_function(graph->map, hash_func);
    map_set_hash_function(graph->map_vertices, hash_func);
}