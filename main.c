#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define name_length 50
#define EAN_maxSize 14

typedef struct inventory{
    char EAN[EAN_maxSize];
    char name[name_length];
    int quantity;
    double price; 
    struct inventory* next;//sxoliase TODO
}inv;

typedef struct order{
    char EAN[EAN_maxSize];
    int quantity;
    int priority;
    struct order* next;//sxoliase TODO
}order;

/* αυτό το struct αντιπροσωπεύει ουρά προτεραιότητας, θα χρησιμοποιείται μόνο για enqueue, οπότε το *head αρκεί.*/
typedef struct orderPriority{ 
    order* head;
}orderPriority;

//extra monada
typedef struct costumer{
    int costumer_id;
    char buisness_name[name_length];
    int phone[10];
    char address[name_length];
    struct costumer* next; //sxoliase TODO
}costumer;

inv* newInventoryNode(char EAN[], char name[], int quantity, double price){ 
    inv* node = (inv*)malloc(sizeof(inv));
    if(!node) return NULL;

    strcpy(node->EAN, EAN);
    strcpy(node->name, name);               
    node->quantity = quantity;              
    node->price = price;
    node->next = NULL;

    return node;
} 

order* newOrderNode(char EAN[], int quantity, int priority){
    order* node = (order*)malloc(sizeof(order));
    if(!node) return NULL;
    
    strcpy(node->EAN, EAN);
    node->quantity = quantity;              
    node->priority = priority;
    node->next = NULL;

    return node;
}

//χρησιμοποιείται στην insertInventory σε περίπτωση που ο χρήστης εισάγει 4-13 ψηφεία
//επρεπε να μπει πίσω απο insert, αλλιώς δεν την διάβαζε.
void recommend(inv** head, char EAN[]){ 
    int found = 0;
    size_t counter = strlen(EAN);

    inv* temp = *head;                         //αρχικοποίηση temp στο πρώτο node της λίστας
    while(temp != NULL){ //proteinie edw lathos den kserwz
        if(strncmp(temp->EAN,EAN,counter) == 0){
            printf("EAN: %s, name: %s, quantity: %d\n",temp->EAN,temp->name,temp->quantity);
            found = 1;
        }
        temp = temp -> next;
    }
    if(!found) printf("No products found with the given EAN(barcode) code\n");
}

int checkEAN(char EAN[]){
    size_t eanLength = strlen(EAN);
    if(eanLength < 4 || eanLength > 13){      //έλεγχος πλήθους ψηφίων ΕΑΝ
        perror("insufficient amount of EAN/barcode digit\n");
        return -1;
    }

    int i = 0;
    while(EAN[i] != '\0'){                      
        if(!isdigit(EAN[i])){                 //έλεγχος άμα οι χαρακτήρες της ΕΑΝ[] είναι ψηφία και τίποτα άλλο
            printf("faulty EAN code, correct code only contains digits\n");
            return -1;
        }
        i++;
    }
    return 1;                                 //όλα πήγαν καλά
}

void insertInventory(inv** invHead, char EAN[], char name[], int quantity, double price){ //δεν είναι σκέτη insert, κάνει update και ήδη υπάρχοντα προϊόντα.
    size_t eanLength = strlen(EAN);
    
    if(checkEAN(EAN) == -1) return;            //ελεγχος κωδικού ΕΑΝ με απο checkEAN συνάρτηση

    inv* temp = *invHead;                      //αρχικοποίηση στο πρώτο node της λίστας
    while(temp != NULL){                       //αναζήτηση λίστας 
        if(strcmp(temp->EAN, EAN) == 0){       //αναζήτηση ήδη υπαρκτής ΕΑΝ, αν υπάρχει
            temp->quantity += quantity;        //αυξάνει την ποσότητα μόνο
            printf("Item %s, increased inventory by %d\n",temp->name, quantity);
            return;
        }
        temp = temp->next;
    }

    //περίπτωση που δεν πληρούνται τα ψηφία όλα, αλλα υπάρχουν αρκετά για recommendation
    if(eanLength < 13){ 
        printf("\nProducts with similar EAN/barcode\n");
        recommend(invHead, EAN);
    }

    //περίπτωση που πληρούνται όλες οι προϋποθέσεις και δημιουργήτε νέο node (προϊόν), τύπου
    //inv* με την βοήθεια της newInventoryNode συνάρτησης    
    else if(eanLength == 13) { 
        inv* node = newInventoryNode(EAN, name, quantity, price);

        if(*invHead == NULL){                      //έλεγχος αμα το head βρίσκεται στο τέλος της λίστας,
            *invHead = node;
        }else{
            temp = *invHead;
            while(temp->next != NULL){             //μετατόπιση head στο τέλος της λίστας
                temp = temp->next;
            }            
            temp->next = node;                     //εισαγωγή νέου node
        }
        printf("Product : '%s' (EAN: %s), added to inventory\n", name, EAN);
    }
}

/*θα προσθέτουμε τα προϊόντα παραγγελίας με όνομα αντί να γράφει ο χρήστης 14ψήφιους αριθμούς. Update: θα βάλουμε και enqueue
απευθείας στην newOrder, ώστε να γίνεται sort η λίστα των τύπου order μεταβλητών με ουρά προτεραιότητας*/
void newOrder(orderPriority* oP, inv* invHead, char name[],int quantity, int priority){
    inv* temp = invHead;
    while(temp != NULL){
        if(strcmp(name, temp->name) == 0){
            order* node = newOrderNode(temp->EAN, quantity, priority); //δημιουργία node, 
            if(oP->head == NULL || priority > oP->head->priority){     //έλεγχος αν η ουρά είναι άδεια ή υπάρχει μεγαλύτερο priority
                node->next = oP->head;             
                oP->head = node;                     //εισαγωγή νέου node στη λίστα
            }else{                     
                order* orderHead = oP->head;             
                while(orderHead->next != NULL && orderHead->next->priority >= priority){  
                    orderHead = orderHead->next;     //μετατόπιση του head (του order) στο σωστό σημείο της λίστας.
                }                      
                node->next = orderHead->next;        //σύνδεση του νέου κόμβου
                orderHead->next = node;              //εισαγωγή νέου node στη λίστα
            }             
            printf("product added to basket: %s\n", temp->name);
            return;
        }    
        temp = temp->next;
    }
    printf("no product: %s available\n", name);
}

/*-  Εκτύπωση αποθέματος (δέχεται είτε κωδικό ή μέρος του είτε όνομα είτε * για 
εκτύπωση όλου του αποθέματος).  */
void print(inv** invHeadPtr, order* orderHead, char choice[name_length]){
    inv* tempInv = *invHeadPtr;
    if(strcmp(choice, "*") == 0){                    //τύπωση όλης της λίστας inventory
        if(tempInv == NULL){                         //έλγχος αμα είναι άδεια η λίστα
            printf("Invenroty is empty\n");
            return;
        }
        printf("inventory:\n\n");
        while(tempInv != NULL){
            printf("Product: %s, %s, quantity: %d, price: %lf\n", tempInv->name, tempInv->EAN, tempInv->quantity,  tempInv->price);
            tempInv = tempInv->next;
        }
        return;
    }else if(strcmp(choice, "orders") == 0){         //extra για να τεστάρω την order αλλά την άφησα, τύπωση όλης της ουρας 'προτεραιότητας' order
        order* tempOrder = orderHead;
        if(tempOrder == NULL){
            printf("order queue is empty\n");
            return;
        }
        printf("Orders:\n\n");
        while(tempOrder != NULL){
            printf("Order EAN: %s, Quantity: %d, Priority: %d\n", tempOrder->EAN, tempOrder->quantity, tempOrder->priority);
            tempOrder = tempOrder->next;
        }
        return;
    }else{
        int found = 0;
        while(tempInv != NULL){
            if(strcmp(choice, tempInv->EAN) == 0 || strcmp(choice, tempInv->name) == 0){
                printf("Product: %s, quantity: %d, price: %.2lf\n%s\n",tempInv->name, tempInv->quantity, tempInv->price, tempInv->EAN);
                found = 1;
            }
            tempInv = tempInv->next;
        }
        if(!found) recommend(invHeadPtr, choice);
        
        return;
    }
}

//έχει γίνει ήδη enqueue της order ουράς (απο το priority που δίνει ο χρήστης) μέσα στην newOrder συνάρτηση, οπότε βάζουμε
//αυτούσια την order* παράμετρο
void completeOrders(orderPriority* oP, inv** invHeadPtr){
    if(oP->head == NULL){
        printf("order queue is empty\n");
        return;
    }
    printf("executing..\n\n");
    while(oP->head != NULL){
        order* tempOrder = oP->head;
        oP->head = tempOrder->next;
        inv* tempInv = *invHeadPtr;
        inv* prevInv = NULL;            //Χρησιμοποιούμε την prev μόνο για να μπορούμε να κάνουμε dequeue προϊόντα τα οποία 
                                        //δεν έχουν άλλο quantity στο inventory, η prevInv θα δίνεται στην tempInv, ως top της ουράς
        while(tempInv != NULL){
            if(strcmp(tempInv->EAN, tempOrder->EAN) == 0 ){
                if(tempInv->quantity >= tempOrder->quantity){
                    tempInv->quantity -= tempOrder->quantity;
                    printf("stock: %s %s depleted by %d\n", tempInv->name, tempOrder->EAN, tempOrder->quantity);
                }else{
                    printf("stock: %s %s depleted entirely, excecuted %d quantity of the order\n", tempInv->name, tempOrder->EAN, tempInv->quantity);
                    tempInv->quantity = 0;
                }

                //Δεν ζητήται αλλά μου φάνηκε λογικό να υλοποιήσουμε dequeue, ώστε να φαίνεται πιο καθαρή η λίστα inventory
                if(tempInv->quantity == 0){
                    if(prevInv == NULL){                //άμα βρισκόμαστε στο πρώτο στοιχείο του πίνακα και το διαγράψουμε
                        *invHeadPtr = tempInv->next;    //πρέπει να ξεκινήσουμε την ουρά από το 2ο στοιχείο, ενημερώνοντας
                    }else                               //τον head pointer του inventory 
                        prevInv->next = tempInv->next;
                    free(tempInv);
                }
                break;
            }
        prevInv = tempInv;
        tempInv = tempInv->next;
        }
        free(tempOrder);
    }
}

int main(){     
    printf("welcome!\n");
    inv* invHead = NULL;
    orderPriority orderHead = {NULL}; 
    char line[100]; //= {"nothing yet"};
    char word[20];

    while(fgets(line,sizeof(line), stdin) != NULL){
        sscanf(line, "%s", word);
        
        if(strcmp(word, "insert") == 0){
            char ean[EAN_maxSize];
            char name[name_length];
            int quantity;
            double price;
            
            //bres allo solution anti gia *s
            if(sscanf(line,"%*s %s %s %d %lf", ean, name, &quantity, &price) == 4){ //χρειάστηκε μια extra δήλωση τύπου δεδομένων *s για να προσπεράσουμε την πρώτη λέξη 'insert'
                insertInventory(&invHead, ean, name, quantity, price);
            }else printf("Wrong insert format, try: 'insert <barcode> <name> <quantity> <price>'\n");
        
        }else if(strcmp(word, "order") == 0){
            char name[name_length];
            int quantity;
            int priority;

            if(sscanf(line, "%*s %s %d %d",name, &quantity, &priority) == 3){
                newOrder(&orderHead, invHead, name, quantity, priority);
            }else 
                printf("Wrong order format, try: 'order <name> <quantity> <priority>'\n");
        
        }else if (strcmp(word, "print") == 0){
            char choice[name_length];   

            if(sscanf(line, "%*s %s",choice) == 1){
                print(&invHead, orderHead.head, choice);        //συνάρτηση εκτύπωσης επιλογών / orderHead.head λόγω της δήλωσης του struct orderPriority
            }else
                printf("Wrong print format, try: print <name/EAN OR 'orders' OR '*'>\n");    

        }else if(strcmp(word, "execute") == 0){
            completeOrders(&orderHead, &invHead);
            
        }else if(strcmp(word, "exit") == 0){
            free(invHead);                  
            free(orderHead.head);        ///////////////////////////////////fucken fix that shi        
            break;
    
        }else if(strcmp(word, "help") == 0){
            printf("Command list:\n\n\t- insert <barcode> <name> <quantity> <price>\n\t- print\n\t- exit\n");
        }else 
            printf("Cannot recognise command, type help\n"); // kanto kltro grapse sxolia TODO
    }

    return 0;   
}