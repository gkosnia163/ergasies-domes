#include <stdio.h>

#define name_length 50

typedef struct inventory{
    int EAN[14];
    char name[name_length];
    int quantity;
    double price; 
    struct inventory* next;//sxoliase TODO
}inv;

typedef struct orderItem{
    int EAN[14];
    int quantity;
    int priority;
    struct orderItem* next;//sxoliase TODO
}orderItem;

//το struct order χρησιμοποιεί προϊόντα τύπου orderItem, καθώς θέλω να αποθηκεύσω
//περισσότερα απο 1 δεδομένα σε κάθε προϊόν, την προτεραιότητα και ποσότητα.
typedef struct order{ 
    int costumer_id;  
    char buisness_name[name_length];
    struct orderItem* items;
    struct order* next;//sxoliase TODO
}order;

typedef struct costumer{
    int costumer_id;
    char buisness_name[name_length];
    int phone[10];
    char address[name_length];
    struct costumer* next; //sxoliase TODO
}costumer;

inv* new_Inventory_Node(int EAN[14], char ){ //δημιουργία καινούριου node     
    inv* newNode = (inv*)malloc(sizeof(inv));
    
} 

int main(){


    return 0;   
}