/*
Κωνσταντίνος Ιάκωβος Νιάφας
2022202100140
dit21140
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define buffer 64             //buffer χρησιμοποιείτε για την προσωρινή εκχώρηση λέξεως από το .txt file + 
#define hashtable_size 163632 //το file dictionary.txt, περιλλαμβάνει 183.719 λέξεις

//αλυσίδα αναγραμματισμού. Εδώ μέσα βρίσκονται τα αταξινόμητα αλφαριθμητικά τα οποία συνδέονται στο αντίστοιχο qsort-ed αναγνωριστικό τους
typedef struct link{        
    char *key;            //χρήση char* όπως περιγράφεται στην εκφώνηση
    struct link* next;    //απλή linked list για την αλυσίδα αναγραμματισμού
}link;

//περιλλαμβάνει τα κοινά αναγνωριστικά όλων των string (το πρώτο collumn σύμφωνα με το σχήμα της εκφώνησης "πίνακας κατακερματισμού")
typedef struct{
    char* sortedkey;
    link* head;
}hashtable;

//αυτούσιος αλγόριθμος απο την εκφώνηση
unsigned int strToHash(char* str){ 
    unsigned int hash=0; 
    while(*str){ 
        hash = *str+31*hash; 
        str++; 
    } 
    return hash%hashtable_size; 
}  

void* createHashTable(int size){//δημιουργία hashtable με κενά κελιά, αλλά στο μέγεθος που πρέπει
    hashtable *ht = malloc(sizeof(hashtable) * size);
    if(ht == NULL){
        perror("can't allocate memory\n");
        exit(1);
    }

    for(int i = 0; i < size; i++){
        ht[i].sortedkey = NULL;
        ht[i].head = NULL;
    }

    return ht;
}

int compare(const void *a, const void *b) {
    return strcmp((char*)a, (char *)b);
}

void insert(hashtable* ht, char key[buffer]){//+linear probing
    char* sortedkey = strdup(key); //κοινό αναγνωριστικό 
    qsort(sortedkey, strlen(sortedkey), sizeof(char), compare);
    //η quicksort μας δίνει το κοινό αναγνωριστικό sortedkey="eorrst" ΓΙΑ key="resort" || "rosted" || "storer"

    unsigned int index = strToHash(sortedkey); 
    unsigned int temp = index;

    while(ht[index].sortedkey != NULL){//εύρεση σωστού bucket
        if(strcmp(ht[index].sortedkey, sortedkey) == 0){
            free(sortedkey);
            break;
        }

        index = (index+1) % hashtable_size;

        if(index == temp){
            printf("full\n");
            free(sortedkey);
            return;
        }
    }
    
    if(ht[index].sortedkey == NULL){    //1. άμα το κελί είναι άδειο 
        ht[index].sortedkey = sortedkey;//εισαγωγή κοινού αναγνωριστικού στον πίνακα 
        ht[index].head = NULL;
    }

    //Δημιουργία αλυσίδας
    link *l = malloc(sizeof(link));//αυστηρό allocation σύμφωνα με το μέγεθος της λέξης +1 λόγω "\0" στο τέλος του string
    if(l == NULL){
        perror("malloc failed\n");
        //ftiakse free everything synarthsh
        exit(1);
    }
    l->key = strdup(key); //Big trouble maker το strcpy(), έπρεπε οποσδήποτε να το αλλάξω
    l->next = ht[index].head;
    ht[index].head = l;                 //ο πίνακας συνδέεται με την (αντίστοιχη) αλυσίδα αταξινόμητων strings 
}

void search(hashtable* ht){ //+linear probing
    char key[buffer];
    printf("word finder: ");
    scanf("%s", key);
    while(strpbrk(key, "0123456789,./;!@#$^&*") != NULL){ //strbrk επιστρέφει pointer στο χαρακτήρα που ταιριάζει απο (const)(char)* key με (const)(char)* "01234,./@κλπ" 
        printf("%s, not a word\n",key);
        scanf("%s", key);                                //ώστε να λαμβάνει υπόψιν μόνο χαρακτήρες.
    }

    char* sortedkey = strdup(key); //κοινό αναγνωριστικό    
    qsort(sortedkey, strlen(sortedkey), sizeof(char), compare);
    //η quicksort μας δίνει το κοινό αναγνωριστικό sortedkey="eorrst" ΓΙΑ key="resort" || "rosted" || "storer", sortedkey="eorrst"
    
    unsigned int index = strToHash(sortedkey);
    unsigned int temp = index;

    while(ht[index].sortedkey != NULL){
        if(strcmp(ht[index].sortedkey, sortedkey) == 0){
            printf("\nword found: %s, position in link-key %u, hash code: %u\n%s", key, index, temp, ht[index].sortedkey);
            link *l = ht[index].head;
            while(l != NULL){
                printf(" -> %s", l->key);
                l = l->next;
            }
            printf("\n");
            free(sortedkey);
            return;
        }
        index = (index+1) % hashtable_size;//linear probing, όταν είναι κατειλημμένο το hashcode που παρήγαγε το key, 
                                           //προσπαθεί να μπεί στο επόμενο κελί, μέχρι να βρεί κενό              
        if(index == temp) break;           
    }
    printf("nothing found\n");
    free(sortedkey);
}   

void addTxt(hashtable* ht, char *filename){
    FILE* f = fopen(filename, "r");
    if(f == NULL){
        perror("Make sure the file exists in the same path as this .c file\n");
        exit(1);
    }
    char word[buffer];
    int i = 0, n;
    
    printf("how many words you want inserted in the dictionary?\n");
    scanf("%d", &n);

    while(i < n &&fscanf(f,"%s", word) == 1){
        insert(ht, word);
        i++;
    }
    fclose(f);
}

void printWords(hashtable* ht){
    for(int i = 0; i < hashtable_size; i++){
        if(ht[i].sortedkey != NULL){
            printf("%s",ht[i].sortedkey);
            link* l = ht[i].head;
            while(l != NULL){
                printf(" -> %s", l->key);
                l = l->next;       
            }
            printf("\n");
        }
    }
}

void freeEverything(hashtable* ht){
    hashtable *httemp;
    link *l, *ltemp;
    while(l != NULL){
        ltemp = l;
        l = ltemp->next;
        free(ltemp);
    }
    while(ht != NULL){
        httemp = ht->head;
        ht = httemp->head->next;
        free(httemp);
    }
    free(ht); free(l);
}

int main(){
    printf(" _               _           _ _      _   _\n| |__   __ _ ___| |__     __| (_) ___| |_(_) ___  _ __   __ _ _ __ _   _\n| '_ \\ / _` / __| '_ \\   / _` | |/ __| __| |/ _ \\| '_ \\ / _` | '__| | | |\n| | | | (_| \\__ \\ | | | | (_| | | (__| |_| | (_) | | | | (_| | |  | |_| |\n|_| |_|\\__,_|___/_| |_|  \\__,_|_|\\___|\\__|_|\\___/|_| |_|\\__,_|_|   \\__, |\n\t\t\t\t\t\t\t\t   |___/ \n\n");
    hashtable* ht = (hashtable*)createHashTable(hashtable_size);
    addTxt(ht, "dictionary.txt");
    insert(ht,"cusm");
    printWords(ht);
    search(ht);
    freeEverything(ht);
    return 0;
} 