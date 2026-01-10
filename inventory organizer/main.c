//github repo: https://github.com/gkosnia163/ergasies-domes

/*
Κωνσταντίνος Ιάκωβος Νιάφας
2022202100140
dit21140@go.uop.gr

Κωνσταντίνος Γεραμάνης
2022202200018
dit22018@go.uop.gr
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define name_length 50
#define EAN_maxSize 14

//απλή singley linked λίστα που περιέχει στοιχεία τύπου inventory
typedef struct inventory{
    char EAN[EAN_maxSize];
    char name[name_length];
    int quantity;
    double price; 
    struct inventory* next;
}inv;

//απλή singley linked λίστα που περιέχει στοιχεία τύπου οrder
typedef struct order{
    char costumer_id[8];
    char buisness_name[name_length];
    char EAN[EAN_maxSize];
    int quantity;
    int priority;
    struct order* next;
}order;

//αυτό το struct αντιπροσωπεύει ουρά προτεραιότητας, θα χρησιμοποιείται μόνο για enqueue, οπότε το *head αρκεί.
typedef struct orderPriority{ 
    order* head;
}orderPriority;

//extra μονάδα λίστα (singly linked) των πελατών
typedef struct costumer{
    char costumer_id[8];
    char buisness_name[name_length];
    char phone[10];
    char address[name_length];
    struct costumer* next; 
}costumer;

//χρησιμοποιείται στην insert για την δημιουργία νέων node (λόγω "καθαρότητας")
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

//παρομοίως χρησιμοποιείται στην new order
order* newOrderNode(char id[], char costumer_name[], char EAN[], int quantity, int priority){
    order* node = (order*)malloc(sizeof(order));
    if(!node) return NULL;
    
    strcpy(node->buisness_name, costumer_name);
    strcpy(node->costumer_id, id);
    strcpy(node->EAN, EAN);
    node->quantity = quantity;              
    node->priority = priority;
    node->next = NULL;

    return node;
}

//χρησιμοποιείται στην insertInventory σε περίπτωση που ο χρήστης εισάγει 4-13 ψηφεία
void recommend(inv** head, char EAN[]){ 
    int found = 0;
    size_t counter = strlen(EAN);

    inv* temp = *head;                         //αρχικοποίηση temp στο πρώτο node της λίστας
    while(temp != NULL){ 
        if(strncmp(temp->EAN, EAN,counter) == 0){
            printf("EAN: %s, name: %s, quantity: %d\n",temp->EAN,temp->name,temp->quantity);
            found = 1;
        }
        temp = temp -> next;
    }
    if(!found) printf("No products found with the given EAN(barcode) code\n");
}

//έλεγχος της sscanf άμα δόθηκαν σωστά οι createAccount() παράμετροι 
int checkAcc(costumer** head, char id[], char name[]){
    size_t idLength = strlen(id), nameLength = strlen(name);

    if(idLength > 8 || nameLength >= name_length || idLength == 0 || nameLength == 0){
        printf("invalid id/name\n");
        return -1;
    }

    costumer* temp = *head;
    while(temp != NULL){
        if(strcmp(temp->buisness_name, name) == 0){
            printf("username taken\n");
            return -1;
        }
        temp = temp->next;
    }
    return 1;
}

//έλεγχος της sscanf της insert ... επιλογής
int checkInsert(inv** head, char EAN[], char name[]){
    size_t eanLength = strlen(EAN);
    if(eanLength < 4 || eanLength > 13){      //έλεγχος πλήθους ψηφίων ΕΑΝ
        printf("insufficient amount of EAN/barcode digit\n");
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
    inv* temp = *head;
    while(temp != NULL){                      
        if(strcmp(name, temp->name) == 0 && strcmp(EAN, temp->EAN) != 0){   //έλγχος άμα υπάρχει
            printf("Product : '%s', already exists with EAN: %s\n", name, temp->EAN);
            return -1;
        }
        temp = temp->next;
    }
    return 1;                                 
}

//δημιουργεί λογαριασμού, δημιουργείται νεο node τύπου costumer και εισάγονται τα δεδομένα μέσα στο node
//χρησιμοποιείται checkAcc() συνάρτηση
void createAccount(costumer** head, char id[], char name[], char phone[], char address[]){
    costumer* node = (costumer*)malloc(sizeof(costumer));
    if(!node) return;
    if(checkAcc(head, id, name) == -1) {
        free(node);
        return;
    }

    strcpy(node->costumer_id, id);
    strcpy(node->buisness_name, name);
    strcpy(node->phone, phone);
    strcpy(node->address, address);
    node->next = *head;
    *head = node;                             
    
    printf("account: '%s' created\n",node->buisness_name);
    return;
}

//insert new node (τύπου inventory) + κάνει update και ήδη υπάρχοντα προϊόντα.
//χρησιμοποιούνται checkInsert(), newInventoryNode(), recommend() συναρτήσεις
void insertInventory(inv** invHead, char EAN[], char name[], int quantity, double price){ 
    size_t eanLength = strlen(EAN);
    
    if(checkInsert(invHead, EAN, name) == -1) return;      //ελεγχος length κωδικού ΕΑΝ και ονόματος

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
    //inv* με την βοήθεια της newInventoryNode() συνάρτησης    
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
        printf("Product: '%s' (EAN: %s), added to inventory\n", name, EAN);
    }
}

//δημιουργία νέου node τύπου order με newOrderNode συνάρτηση
//κάνουμε enqueue απευθείας στην newOrder, ώστε να γίνεται sort η λίστα (των τύπου order μεταβλητών) με ουρά προτεραιότητας
//χρησιμοποιείται newOrderNode() συνάρτηση
void newOrder(orderPriority* oP, inv* invHead, costumer* accHead, char id[], char buisnessName[], char name[],int quantity, int priority){
    int found = 0;
    costumer* tempAcc = accHead;
    while(tempAcc != NULL){
        if(strcmp(tempAcc->costumer_id, id) == 0 && strcmp(tempAcc->buisness_name, buisnessName) == 0){
            found = 1;
            break;
        }
        tempAcc = tempAcc->next;
    }
    if(!found){
        printf("Order rejected: wrong id: %s or name: %s\n", id, buisnessName);
        return;
    }

    inv* temp = invHead;
    while(temp != NULL){
            if(strcmp(name, temp->name) == 0 || strcmp(name, temp->EAN) == 0){              //έλεγχος ΕΑΝ εαν εντάχθηκε σωστά (μαζί και με όνομα)
                order* node = newOrderNode(id, buisnessName,temp->EAN,quantity, priority);                  

                if(oP->head == NULL || priority > oP->head->priority){                      //έλεγχος αν η ουρά είναι άδεια ή υπάρχει μεγαλύτερο priority
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
//χρησιμοποιείται συνάρτηση recommend()
void print(inv** invHeadPtr, order* orderHead,costumer* accHead, char choice[name_length]){
    inv* tempInv = *invHeadPtr;
    if(strcmp(choice, "*") == 0){                    //τύπωση όλης της λίστας inventory
        if(tempInv == NULL){                         //έλγχος αμα είναι άδεια η λίστα
            printf("invenroty list is empty\n");
            return;
        }
        printf("inventory:\n--------------------------------------------------------\n");
        while(tempInv != NULL){
            printf("Product: %s, %s, quantity: %d, price: %.2lf\n", tempInv->name, tempInv->EAN, tempInv->quantity,  tempInv->price);
            tempInv = tempInv->next;
        }
        printf("--------------------------------------------------------\n");
        return;
    }else if(strcmp(choice, "orders") == 0){         //extra για να τεστάρω την order αλλά την άφησα, τύπωση όλης της ουρας 'προτεραιότητας' order
        order* tempOrder = orderHead;
        if(tempOrder == NULL){
            printf("order queue is empty\n");
            return;
        }
        printf("orders:\n--------------------------------------------------------\n");
        while(tempOrder != NULL){
            printf("Order EAN: %s, Quantity: %d, Priority: %d\n", tempOrder->EAN, tempOrder->quantity, tempOrder->priority);
            tempOrder = tempOrder->next;
        }
        printf("--------------------------------------------------------\n");
        return;
    }else if(strcmp(choice, "accounts") == 0){        //extra τυπώνει και όλους τους λογαριασμούς
        costumer* tempAcc = accHead;
        if(tempAcc == NULL){
            printf("account list is empty\n");
            return;
        }
        printf("accounts:\n--------------------------------------------------------\n");
        while(tempAcc != NULL){                         
            printf("Account: %s, id: %s, phone: %s, address: %s\n", tempAcc->buisness_name, tempAcc->costumer_id, tempAcc->phone, tempAcc->address);
            tempAcc = tempAcc->next;
        }
        printf("--------------------------------------------------------\n");
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

//επεξεργασία παραγγελιών
//έχει γίνει ήδη enqueue της order ουράς (απο το priority που δίνει ο χρήστης) μέσα στην newOrder συνάρτηση
void completeOrders(orderPriority* oP, inv** invHeadPtr){
    if(oP->head == NULL){
        printf("order queue is empty\n");
        return;
    }
    printf("executing..\n\n");
    
    order* currentOrder = oP->head;         //χρησιμοποιεί την κεφαλή της δομής orderPriority
    order* prevOrder = NULL;
    char lastCustomerId[8] = "";            
    double totalCost = 0;

    while(currentOrder != NULL){
        //αν αλλάξει ο πελάτης, τυπώνουμε σύνολο προηγούμενου και header νέου
        if(strcmp(currentOrder->costumer_id, lastCustomerId) != 0){
            if(strlen(lastCustomerId) > 0){                   //ελέγχουμε αμα υπήρχε προηγούμενος πελάτης, αμα strlen(lastCustomerId) == 0, είναι η πρώτη παραγγελία και δεν
                                                              //τυπώνουμε του προηγούμενου (που δεν υπάρχει)
                 printf("--------------------------------------------------------\ntotal:\t%.2lf\n\n", totalCost);
            }
            printf("receipt: %s\n--------------------------------------------------------\n", currentOrder->buisness_name);
            strcpy(lastCustomerId, currentOrder->costumer_id);//ενημέρωση ότι έχουμε δεί αυτόν τον πελάτη
            totalCost = 0;
        }

        inv* tempInv = *invHeadPtr;
        inv* prevInv = NULL;
        
        while(tempInv != NULL){
            if(strcmp(tempInv->EAN, currentOrder->EAN) == 0 ){
                double thisCost = 0;
                if(tempInv->quantity >= currentOrder->quantity){        //πλήρης κάλυψη ποσότητας παραγγελίας
                    tempInv->quantity -= currentOrder->quantity;
                    thisCost = currentOrder->quantity * tempInv->price;
                    printf("%s -- quantity: %d -- price: %.2lf\n", tempInv->name, currentOrder->quantity, thisCost);
                    currentOrder->quantity = 0; 
                }else{                                                  //μερική κάλυψη 
                    thisCost = tempInv->quantity * tempInv->price;
                    printf("%s -- %d%% of available quantity: %d units -- price: %.2lf\n", tempInv->name, (tempInv->quantity*100)/currentOrder->quantity, tempInv->quantity, thisCost);
                    currentOrder->quantity -= tempInv->quantity;        // Μείωση ποσότητας παραγγελίας
                    tempInv->quantity = 0;
                }
        
                totalCost += thisCost;
                
                //το ξέρω ότι δεν ζητήται, αλλά μου φάνηκε λογικό να διαγράφεται το συγκεκριμένο (inventory)node άμα φτάσει inventory quantity == 0
                if(tempInv->quantity == 0){
                    if(prevInv == NULL){                
                        *invHeadPtr = tempInv->next;    
                    }else{                              
                        prevInv->next = tempInv->next;
                    }
                    free(tempInv);                      
                }                                       
                break;
            }
            prevInv = tempInv;
            tempInv = tempInv->next;
        }
        
        //διαγραφή παραγγελίας ΜΟΝΟ αν ολοκληρώθηκε (quantity == 0)
        if(currentOrder->quantity == 0){
            order* toDelete = currentOrder;             //κρατάμε δείκτη στο node που θα διαγράψουμε
            if(prevOrder == NULL){              
                oP->head = currentOrder->next;
                currentOrder = oP->head;
            }else{
                prevOrder->next = currentOrder->next;   //συνδέουμε το προηγούμενο με το επόμενο
                currentOrder = currentOrder->next;      //προχωράμε στο επόμενο
            }
            free(toDelete);                             //αφαίρεση κορυφαίου node της λίστας
        }else{
            //αν υπάρχει υπόλοιπο, την κρατάμε και προχωράμε
            prevOrder = currentOrder;
            currentOrder = currentOrder->next;
        }
    }
    
    if(strlen(lastCustomerId) > 0){
         printf("--------------------------------------------------------\ntotal:\t%.2lf\n", totalCost);
    }
}

int main(){     
    printf("welcome!\n");
    inv* invHead = NULL;
    orderPriority orderHead = {NULL}; 
    costumer* accHead = NULL;
    char line[100]; //= {"nothing yet"};
    char word[20];

    //Όλες οι επιλογές γίνονται με sscanf ώστε ο χρήστης να εισάγει 1 εντολή για κάθε λειτουργία
    while(fgets(line,sizeof(line), stdin) != NULL){
        sscanf(line, "%s", word);
        
        if(strcmp(word, "create") == 0){
            char id[8];
            char buisness_name[name_length];
            char phone[10];
            char address[name_length];

            //χρειάστηκε μια extra δήλωση τύπου δεδομένων *s για να προσπεράσουμε την πρώτη λέξη 'insert'
            if(sscanf(line, "%*s %s %s %s %s", id, buisness_name, phone, address) == 4){ //άμα σκαναριστούν 4 μεταβλητές true
                createAccount(&accHead, id, buisness_name, phone, address);
            }else printf("Wrong create format, try: 'create <id> <username> <phone> <address>'\n");
        }else if(strcmp(word, "insert") == 0){
            char ean[EAN_maxSize];
            char name[name_length];
            int quantity;
            double price;
            
            if(sscanf(line,"%*s %s %s %d %lf", ean, name, &quantity, &price) == 4){ 
                insertInventory(&invHead, ean, name, quantity, price);
            }else
                printf("Wrong insert format, try: 'insert <barcode> <product name> <quantity> <price>'\n");
        
        }else if(strcmp(word, "order") == 0){
            char id[8];
            char buisnessName[name_length];
            char name[name_length];
            int quantity;
            int priority;

            if(sscanf(line, "%*s %s %s %s %d %d", id, buisnessName, name, &quantity, &priority) == 5){
                newOrder(&orderHead, invHead, accHead, id, buisnessName, name, quantity, priority);
            }else
                printf("Wrong order format, try: 'order <id> <username> <EAN> <quantity> <priority>'\n");
        
        }else if (strcmp(word, "print") == 0){
            char choice[name_length];   

            if(sscanf(line, "%*s %s",choice) == 1){
                print(&invHead, orderHead.head, accHead, choice);        //συνάρτηση εκτύπωσης επιλογών / orderHead.head λόγω της δήλωσης του struct orderPriority
            }else
                printf("Wrong print format, try: print <name/EAN OR 'orders' OR '*'>\n");    

        }else if(strcmp(word, "execute") == 0){
            completeOrders(&orderHead, &invHead);
            
        }else if(strcmp(word, "exit") == 0){
            inv* tempInv;
            while(invHead != NULL){
                tempInv = invHead;
                invHead = invHead->next;
                free(tempInv);
            }
            order* tempOrder;
            while(orderHead.head != NULL){
                tempOrder = orderHead.head;
                orderHead.head = orderHead.head->next;
                free(tempOrder);
            }
            costumer* tempAcc;
            while(accHead != NULL){
                tempAcc = accHead;
                accHead = accHead->next;
                free(tempAcc);
            }
            break;

        }else if(strcmp(word, "help") == 0){
            printf("Command list:\n\n\t- create <id> <username> <phone> <address>\n\t- insert <barcode> <name> <quantity> <price>\n\t- print <name/EAN OR 'orders' OR 'acounts' OR '*'>\n\t- order <id> <username> <EAN OR name> <quantity> <priority>\n\t- exit\n");
        }else 
            printf("Cannot recognise command, type help\n"); 
    }

    return 0;   
}