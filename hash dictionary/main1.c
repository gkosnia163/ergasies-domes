#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define buffer 128            //buffer χρησιμοποιείτε για την προσωρινή εκχώρηση λέξεως από το .txt file + 
#define DictionarySize 190000 //το file dictionary.txt, περιλλαμβάνει 183.719 λέξεις, θα το κάνουμε λίγο μεγαλύτερο
                              //να μην μπερδεύονται τα seeds εύκολα καθώς και να κάνουμε insert κι'άλλες λέξεις
typedef struct {
    char *table[DictionarySize]; //or key, περιλλαμβάνει την String λέξη για το dictionary
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


hashtable* createHashTable(){//δημιουργία hashtable με κενά κελιά node
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

void insert(hashtable* ht, char* word){
    unsigned int index = strToHash(word);
    unsigned int temp = index;  

    //έλεγχοι
    while(ht->table[index] != NULL){
        if(strcmp(ht->table[index], word) == 0){//έλεγχος αμα υπάρχει seed ίδιο
            printf("word already exists\n");    
            return;
        }
        index = (index + 1) % DictionarySize;
        if(index == temp){ //"γραμμική διερεύνηση", όταν index φτάνει προς το τέλος του κόμβου  
            printf("bucket full\n");
            return;           
        }
    }

    //υπάρχει κενό node τρ γίνεται malloc
    ht->table[index] = malloc(strlen(word) + 1); //αυστηρό allocation σύμφωνα με το μέγεθος της λέξης +1 λόγω "\0" στο τέλος του string
    if(ht->table[index] == NULL){
        perror("malloc failed\n");
        exit(1);
    }
    strcpy(ht->table[index], word);
    ht->count++;
}

void addTxt(hashtable* ht, char *filename, int n){
    FILE* f = fopen(filename, "r");
    if(f == NULL){
        perror("Make sure the file exists in the same path as this .c file\n");
        exit(1);
    }
    char word[buffer];
    int i = 0;

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
    printf("welcome to city dictionary\n\n");
    hashtable* ht = createHashTable();
    addTxt(ht, "dictionary.txt",120);
    printWords(ht);

    return 0;
}