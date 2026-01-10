/*
Κωνσταντίνος Ιάκωβος Νιάφας
2022202100140
dit21140
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define buffer 128            //buffer χρησιμοποιείτε για την προσωρινή εκχώρηση λέξεως από το .txt file + 
#define DictionarySize 183719 //το file dictionary.txt, περιλλαμβάνει 183.719 λέξεις, θα το κάνουμε λίγο μεγαλύτερο
                              //να μην μπερδεύονται τα seeds εύκολα καθώς και να κάνουμε insert κι'άλλες λέξεις
typedef struct {
    char *table[DictionarySize]; //χρήση char* key όπως περιγράφεται στην εκφώνηση
    int count;
}hashtable;

//αυτούσιος αλγόριθμος απο την εκφώνηση
unsigned int strToHash(char* str){ 
    unsigned int hash=0; 
    while(*str){ 
        hash = *str+31*hash; 
        str++; 
    } 
    return hash%DictionarySize; 
}  

hashtable* createHashTable(){//δημιουργία hashtable με κενά κελιά
    hashtable *ht = malloc(sizeof(hashtable));
    if(ht == NULL){
        perror("can't allocate memory\n");
        exit(1);
    }

    for(int i = 0; i < DictionarySize;i++){
        ht->table[i] = NULL;
    }
    ht->count = 0;

    return ht;
}

void search(hashtable* ht){ //+linear probing
    char key[buffer];
    printf("word finder: ");
    scanf("%s", key);
    while(strstr(key, "0123456789,./;!@#$^&*") != NULL){ //strstr φέρνει pointer στην πρώτη εμφάνιση χαρακτήρα που περιλλαμβάνει τους "delimetres"
        scanf("%s", key);
    }
    unsigned int index = strToHash(key);
    unsigned int temp = index;
    int found = 0;

    while(ht->table[index] != NULL){
        if(strcmp(ht->table[index], key) == 0){
            printf("word found: %s, position in hash-table %u, hash code: %u", key, index, temp);
            found = 1;
            break;
        }
        index = (index+1) % DictionarySize;//linear probing, όταν είναι κατειλημμένο το hashcode που παρήγαγε το key, 
                                           //προσπαθεί να μπεί στο επόμενο κελί, μέχρι να βρεί κενό              
        if(index == temp) break;
    }
    if(!found) printf("%s doesn't exist here\n",key);
    return;
}   

void insert(hashtable* ht, char* key){//+linear probing
    unsigned int index = strToHash(key);
    unsigned int temp = index;

    while(ht->table[index] != NULL){
        if(strcmp(ht->table[index], key) == 0){
            return;
        }

        index = (index+1) % DictionarySize;
        if(index == temp){
            printf("bucket full");
            break;
        }
    }

    //υπάρχει κενό node τρ γίνεται malloc
    ht->table[index] = malloc(strlen(key) + 1); //αυστηρό allocation σύμφωνα με το μέγεθος της λέξης +1 λόγω "\0" στο τέλος του string
    if(ht->table[index] == NULL){
        perror("malloc failed\n");
        exit(1);
    }
    strcpy(ht->table[index], key);
    ht->count++;
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
    for(int i = 0; i < DictionarySize; i++){
        if(ht->table[i] != NULL){
            printf("%s\n", ht->table[i]);
        }
    }
}

int main(){
    printf(" _               _           _ _      _   _\n| |__   __ _ ___| |__     __| (_) ___| |_(_) ___  _ __   __ _ _ __ _   _\n| '_ \\ / _` / __| '_ \\   / _` | |/ __| __| |/ _ \\| '_ \\ / _` | '__| | | |\n| | | | (_| \\__ \\ | | | | (_| | | (__| |_| | (_) | | | | (_| | |  | |_| |\n|_| |_|\\__,_|___/_| |_|  \\__,_|_|\\___|\\__|_|\\___/|_| |_|\\__,_|_|   \\__, |\n\t\t\t\t\t\t\t\t   |___/ \n");
    hashtable* ht = createHashTable();
    addTxt(ht, "dictionary.txt");
    printWords(ht);
    search(ht);
    return 0;
}