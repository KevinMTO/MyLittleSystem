//
//
//
//////////////////////////////////////////////////////////////
//CODICE DI KEVIN MATO MATRICOLA  845726
////////////////////////////////////////////////////////////////
//
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<wchar.h>
#define NUM 51
//VARIABILI GLOBALI una volta scritto su terminale il comando, il percorso e ciò che va scritto i tre campi sono salvati qui
char *extra=NULL;
char*comando=NULL;
char * supp_path=NULL;
char* oldpath=NULL;//in oldpath viene salvato il percorso appena tilizzato per essere confrontato con il successivo


//le strutture di definizione di file e directory
//i file figli e le cartelle figlie sono array di puntatori a liste di file e cartelle, ciò serve per la costruzione di una tabella hash
struct file{
    char *name;
    char * content;
    struct file* next;
};
struct dir{
    char * name;
    int figli;
    struct dir ** fdir;
    struct file **fson;
    struct dir * next;
};
typedef struct dir DIR;
typedef struct file FS;

//questa struttura viene utilizzata per salvare i percorsi delle risorse che hanno nome corrispondente al quello cercato da find
struct sentiero{
char* sentiero;
struct sentiero* next;
};
typedef struct sentiero SS;
//testa della lista di percorsi trovati da find
SS* head=NULL;
//l'ultima cartella accessa nell'eseczuine delle varie operazioni, se si hanno degli errori (stampa di no) essa torna ad essere la RADICE
DIR* touched=NULL;
//una volta stampati i percorsi trovati da find, la lista che li conteneva viene cancellata da clean(SS*testa_della_lista)
int clean(SS*in){
if(in==NULL) return 0;
SS*next=NULL;
SS*start=in;

while(start->next!=NULL){
    next=start->next;
    free(start);
    start=next;
    }
free(start);
return 0;

}

//questa funzione ha lo stesso effetto di strdup, replica esattamente una stringa in un'altra variabile.
//scritta nello stesso modo di strdup, viene utilizzata perché dava warning alla compilazione
char *duplica(const char *stringa) {
    size_t size = strlen(stringa) + 1;
    char *duplicato = malloc(size);
    if (duplicato){
        memcpy(duplicato, stringa, size);
    }
    return duplicato;
}
////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
//funzione che si occupa dell'hashing, semplicemente somma il valore di tutti i caratteri e ne calcola il resto rispetto un numero primo
int hash(char* str){
    int len=strlen(str);
    int i=0;
    int result=0;
    while(i<len){
        result= (int)str[i] +result;
        i++;
    }
    result=result%NUM;
    return result;
}
//controlla che il percorso corrente che si sta utilizzando nell'operazione non sia simile a quelo dell'operazione precedente.
//se è così visto che abbiamo l'ultima cartella accessa ci riconduciamo subito ad essa.
//si basa sul controllo del numero di slash bc sta per barcounter e sulla variazione del percorso
char* check(char*old,char*newp){
    if(oldpath==NULL){
        oldpath=(char*) duplica(newp);
        return NULL;
    }

    int i=0,j=0;//posizione nelle stringhe
    int changed=0;//se sono mai state diverse fra loro
    char* pos=NULL;//pos in quella nuova da ritornare
    int end1=0,end2=0;//quale delle due è terminata
    int bc1=0,bc2=0;//contatore barre di ciascuna stringa
    int thrown=0;//se le due stringhe sono incompatibili e quella vecchia è da buttare

    while(!end1 || !end2){

            if(old[i]=='/') bc1++;
            if(newp[j]=='/') bc2++;
            if(changed){//se c'è stata una variazione alla prossima barra di una delle due stringhe, esse saranno troppo diverse per conservare l'ultimo accesso
                if(old[i]=='/' ||  newp[j]=='/'){
                        thrown=1;
                        break;}
            }
            if(newp[j]==old[i] && newp[j]=='/') pos = newp+j;//man mano che leggiamo la nuova stringa ad ogni barra vecchia corrispondente con quella nuova ci segniamo la posizione
            if(newp[j]!=old[i]) changed=1;
            if(newp[j]=='\0') end2=1;
            else j++;
            if(old[i]=='\0')  end1=1;
            else i++;
    }

    free(oldpath);
    oldpath=NULL;
    oldpath=(char*) duplica(newp);
    if(thrown) return NULL;
    if(bc1>bc2) return NULL;

return pos;
}

//funzione che legge da terminale l'input e alloca spazio necessario per conservarlo
//ch=character, viene data una lunghezza di base iniziale attraverso dim_inizio e con calloc man mano che si leggono caratteri se il numero di essi
//supera la lunghezza della stringa fin'ora allocata, essa viene riallocata
//quando i caratteri finiscono e si legge simbolo di a capo la chiude con il terminatore e la rialloca con lo spazio giusto, ritornando il suo puntatore
char *elabora_input(FILE* file_partenza, size_t dim_inizio){

    char *stringa=NULL;
    int ch=0;
    size_t length=0;

    stringa =(char*) calloc(dim_inizio, sizeof(char));//size: la dimensione ininziale
    if(!stringa) return stringa;//controllo che ci sia stata una allocazione effettiva
    ch=fgetc(file_partenza);

    while( EOF!=ch && ch != '\n'){

            stringa[length++] = ch;
            if(length==dim_inizio){
                        dim_inizio=dim_inizio+20;
                        stringa = (char*)realloc(stringa, sizeof(char)*dim_inizio);
                        if(!stringa)return stringa;
            }
            ch=fgetc(file_partenza);
    }
    stringa[length++]='\0';

    if(length==1) return NULL;//se l'input è vuoto ritorna NULL nel senso di solo terminatore

    return (char*) realloc(stringa, sizeof(char)*length);
}

//funzione che legge l'input da terminale e lo divide nelle tre variabili comando path(detta supp_path) ed extra che sarebbe il testo da scrivere
//copio l'input, pulisco le variabili globali e lavoro sulla copia dell'input
//con strtok trovo i separatori dei tre campi spazio ,"/" e terminatore. Copio le tre porzioni nelle variabili globali. Cancello la copia dell'input.
int setup(char* ingresso){
  if(comando!=NULL) {free(comando);
                                 comando=NULL;}
  if(supp_path!=NULL){free(supp_path);
                                 supp_path=NULL;}
  if(extra!=NULL){free(extra);
                           extra=NULL;}


  char * line=(void*)duplica(ingresso);

 char* op = (char *)strtok(line, " \0");
 char *path = (char *)strtok(NULL, " \0");
 char*  text = (char *)strtok(NULL, "\"\0");

  if(op!=NULL) comando= (void*)duplica(op);
  if(path!=NULL)  supp_path= (void*)duplica(path);
  if(text!=NULL) extra= (void*)duplica(text);
  free (line);
return 0;
}


//legge la variabile comando e riconosce quale sia, ritornando un valore per lo switch nel main.
int istruzione(char* ist){
    if(ist==NULL) return 8;
    setup(ist);
    int i;
   char * commands[8]={ "create","create_dir","read","write","delete","delete_r","find","exit"};
            for(i=0; i<8;i++){
                if(strcmp(comando,commands[i])==0) return i;
            }

return 8;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//funzione per scambiare i percorsi di due nodi nella lista di percorsi.
void swap(SS *a, SS *b){
    char* temp = a->sentiero;
    a->sentiero = b->sentiero;
    b->sentiero = temp;
}
//algoritmo di ordinamento in n^2 per riordinare i percorsi in ordine lessicografico i percorsi trovati.
void bubble_sort(SS *start){
    int swapped=0;
    SS *puntatore=start;
    if (puntatore == NULL)
        return;
//l'algoritmo continua finchè swapped non viene cambiata e rimane stabile a zero che significa non ci devono essere più ordinamenti.
    do{     swapped = 0;
                while (puntatore->next != NULL){
                    if (strcmp(puntatore->sentiero,puntatore->next->sentiero)>0){
                        swap(puntatore, puntatore->next);
                        swapped = 1;
                    }
                    puntatore = puntatore->next;
                }
                puntatore = start;
    }while (swapped);
}

//funzione che aggiunge un percorso alla lista di percorsi in coda; crea il nodo e poi ci copia il sentiero. Ritorna l'indirizo dell'ultimo nodo inserito.
//in ingresso l'ultimo aggiunto a cui si appenderà il prossimo e il newpath
SS* appender(char* newp,SS*last){
    SS* temp=NULL;
        if(last==NULL){
                temp=(SS*)calloc(1,sizeof(SS));
                temp->next=NULL;
                temp->sentiero=(void*)duplica(newp);
                head=temp;
                return head;
        }
       else{ last->next=(SS*)calloc(1,sizeof(SS));
                temp=last->next;
                temp->next=NULL;
                temp->sentiero=(void*)duplica(newp);
                return temp;
       }
}
//funzione che cerca tra le cartelle figlio di una cartella, se trova la cartella col nome corrispondente
//fa in modo che il seguente del suo fratello precedente (cioè inizialmente essa stessa) diventi invece la successiva della cartella.
//in ingresso l'intera struttura di cartelle figlio, il nome e il suo hashing (h)
int fixer(DIR**in,char*name,int h){
        DIR*start=in[h];
        if(start== NULL || start->next==NULL) return 1;

        while(start->next!=NULL){
            if(strcmp(start->next->name,name)==0){
                start->next=start->next->next;
                return 0;
            }
            start=start->next;
        }
return -1;
}
//funzione che cerca tra i file figlio di una cartella, se trova il file col nome corrispondente
//fa in modo che il seguente del suo fratello precedente (cioè inizialmente il file stesso) diventi invece il successivo del file.
int fixer2(FS**in,char*name,int h){
        FS*start=in[h];
        if(start== NULL || start->next==NULL) return 1;

        while(start->next!=NULL){
            if(strcmp(start->next->name,name)==0){
                start->next=start->next->next;
                return 0;
            }
            start=start->next;

        }
return -1;
}
//rimuove la struttura array di file figlio iterativamente; utilizzato da del_r
//guarda ogni cella dell'array di liste e le cancella
int f_rem(FS**primo){
    int i=0;
    FS* in;
    while(i<NUM){
            in=primo[i];
            if(in==NULL);
            else{
            FS*next=NULL;
            FS*start=in;

            while(start->next!=NULL){
                        next=start->next;
                        free(start);
                        start=next;
                }
            free(start);
            }
            i++;
    }
return 0;
}
//rimuove i file figli e le cartelle figlie di una cartella iterativamente e anche i figli file chiamndo f_rem; utilizzato da del_r
//ha in ingresso un array di liste a cartelle, in cui scorrerà ciascuna
int d_rem(DIR**primo){
    int i=0;
    DIR* in;
    while(i<NUM){
                in=primo[i];
                if(in==NULL);
                else{
                DIR*next=NULL;
                DIR*start=in;
                while(start->next!=NULL){
                                next=start->next;
                                d_rem(start->fdir);
                                f_rem(start->fson);
                                free(start);
                                start=next;
                }
                free(start);
                }
                i++;
    }
return 0;
}
//alloca memoria per un file figlio ne definisce il nome e ritorna il suo indirizzo
FS* adder(char*name){
    FS* babyborn=(FS*)calloc(1,sizeof(FS));
                            babyborn->name=(void*)duplica(name);
                            babyborn->content=NULL;
                            babyborn->next=NULL;
                            printf("ok\n");

return babyborn;
}

//aggiunge in testa alla lista di file figli di una cartella il nuovo nodo file figlio ed aumenta il numero di figli della cartella
//controlla prima di tutto che la cartella padre abbia ancora meno di 1024 figli
int addfigli(DIR*node,char *name){
        if(node->figli<=1024){
                   FS* res=adder(name);  //devo fare un assegnamento
                   int h=hash(name);//h dice in quale lista dell'array porre il figlio
                            if(node->fson[h]==NULL){
                                node->fson[h]=res;
                                node->figli=node->figli+1;
                            }
                            else{FS* pross=node->fson[h];
                                    node->fson[h]=res;
                                    res->next=pross;
                                    node->figli=node->figli+1;}


                        return 0;


        }
        else printf("no\n");
        return 1;
 }
//alloca memoria per una cartella figlio ne definisce il nome e la struttura dei figli e ritorna il suo indirizzo
DIR* adder_dir(char *name){
     DIR* babyborn=(DIR*)calloc(1,sizeof(DIR));
                            babyborn->name=(void*)duplica(name);
                            babyborn->next=NULL;
                            babyborn->fson=(FS**)calloc(NUM,sizeof(FS*));
                            babyborn->fdir=(DIR**)calloc(NUM,sizeof(DIR*));
                            babyborn->figli=0;
                            printf("ok\n");

return babyborn;
}


//aggiunge in testa alla lista di cartelle figlo di una cartella il nuovo nodo cartella figlio ed aumenta il numero di figli della cartella
//controlla prima di tutto che la cartella padre abbia ancora meno di 1024 figli
int add_dir(DIR*node,char *name){
            if(node->figli<=1024){

                   DIR* res=adder_dir(name);  //devo fare un assegnamento
                   int h=hash(name);//h dice in quale lista dell'array porre il figlio
                            if(node->fdir[h]==NULL){
                                node->fdir[h]=res;
                                node->figli=node->figli+1;
                            }
                            else{DIR* pross=node->fdir[h];
                                    node->fdir[h]=res;
                                    res->next=pross;
                                    node->figli=node->figli+1;}
                        return 0;


        }
        else printf("no\n");
        return 1;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//aggiunge un nome di risorsa ad un percorso fornito e ritorna la stringa con il nuovo percorso
char* putadd(char* name,char* tillnow){
 //alloca spazio per la dimensione del vecchio percorso più il nome il simbolo "/" e il terminatore.
 //copia prima il vecchio percorso aggiunge "/" e poi il nome per poi chiudere con il terminatore.
char* temp=(char*)calloc((strlen(name)+strlen(tillnow)+2),sizeof(char));
int i,j;
for(i=0;tillnow[i]!='\0';i++) temp[i]=tillnow[i];
temp[i]='/';
i++;
for(j=0;name[j]!='\0';j++){
        temp[i]=name[j];
        i++;
}
temp[i]='\0';
return temp;
}

//findfile:cerca iterativamente i file figli di una cartella, se il nome del figlio corrisponde a quello che cerchiamo salva il suo percorso attraverso la funzone append
//che lo appende all'ultimo percorso salvato, append ritornerà l'indirizzo del nodo che lo contiene, verrà salvato e ritornato a faf.

SS* findfile(char*name,FS*f,char*pbn,SS*lastadd){
    if(f==NULL) return lastadd;

    SS* where=lastadd;
    if(strcmp(f->name,name)==0){
        char *myp=putadd(f->name,pbn);
        where=appender(myp,lastadd);
        return where;
    }
    return findfile(name,f->next,pbn,where); //non proprio corretto perchè non ci dovrebbero essere elementi con lo stesso nome
}
//faf:find all files serve da sostegno per cercare nella singola lista di file figli
//dopo che gli è stata passata l'intera struttura (array) dei figli
SS*faf(char*name,FS**f,char*pbn,SS*lastadd,int h){
return findfile(name,f[h],pbn,lastadd);
}

//finder fa lo stesso lavoro di findfile, solo che lo fa sulle cartelle e lancia per ogni cartella figlia, findfile sui figli file.
//pbn sarebbe path by now il percorso fino a quella cartella. Salva il suo su myp (mypath) e lo passa ai figli.
//where indica dove aggiungere i prossimi percorsi.

SS* founddir(char*name,DIR**d,char*pbn,SS*lastadd,int h);

SS* finder(char*name,DIR*d,char*pbn,SS*lastadd,int h){

    if(d==NULL) return lastadd;
    char *myp=putadd(d->name,pbn);
    SS* where=lastadd;

    if(strcmp(d->name,name)==0){
        where=appender(myp,lastadd);
    }
    where=faf(name,d->fson,myp,where,h);
    where=founddir(name,d->fdir,myp,where,h);
    free(myp);
    where=finder(name,d->next,pbn,where,h);//finder viene lanciato anche sul fratello
    ////printf("closed finder\n");
    return where;
}
//lancia finder fa lo stesso lavoro di faf solo che analizza ogni posizione dell'array di cartelle figlio,
//quindi si analizzano tutte le liste di cartelle figlio possibili
SS*founddir(char*name,DIR**d,char*pbn,SS*lastadd, int h){
    int i=0;
    SS*where=lastadd;
    while(i<NUM){
        if(d[i]!=NULL){
                where=finder(name,d[i],pbn,where,h);}
         i++;
    }
return where;
}

//seziona serve ad estrapolare la prossima risorsa da accedere. Parte da sinistra toglie lo slash copia il nome della risorsa da accedere lo ritorna e il resto del
//percorso è salvato su supp_path (support path), per le chiamate delle funzioni principali.
//es. "/foo/bar" ritorna foo e salva su supp_path "/bar".

char * seziona(char*spath){
if(spath!=NULL){
        char *p=(void*)duplica((spath+1));

        char *ret=strchr(p,'/');
        if(ret==NULL) ret=strchr(p+1,'\0');
        free(supp_path);
        supp_path=(void*)duplica(ret);

        char* key=strtok(p,"/\0");
        char *element2inspect=(void*)duplica(key);

        free(p);


         return element2inspect;
}
return NULL;
}

//stampa iterativamente la lista di percorsi salvati
int printer(SS*in){
if(in==NULL) return 0;
SS*start=in;

while(start->next!=NULL){
     printf("ok %s\n",start->sentiero);
    start=start->next;
    }
 printf("ok %s\n",start->sentiero);
return 0;
}
//guarda iterativamente se c'è un file fglio nella struttura dei figli di una cartella
// se c'è ritorna il suo indirizzo altrimenti NULL
FS* see_figli(FS**in,char*name){
        int h =hash(name);//calcola dove potrebbe essere nell'array di liste di file figlio, accede la cella e guarda la lista correlata, la scorre finchè non trova il figlio
        FS*start=in[h];
        if(start==NULL) return NULL;

        while(start->next!=NULL){
            if(strcmp(start->name,name)==0){
                return start;
            }
            start=start->next;
        }
         if(strcmp(start->name,name)==0){
                return start;
            }
return NULL;
}
//fa lo stesso di see_figli ma con le cartelle figlie
DIR* walk(DIR**in,char*name){
        int h =hash(name);
        DIR*start=in[h];
        if(start== NULL) return NULL;

        while(start->next!=NULL){
            if(strcmp(start->name,name)==0){
                return start;
            }
            start=start->next;
        }
         if(strcmp(start->name,name)==0){
                return start;
            }
return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//reset serve a cancellare oldpath se ci sono stai problemi (cioè stampe di no) e pone la radice di nuovo come ultimo acesso in modo
//che le operazioni quando dovranno analizzare il percorso partano facendolo dala radice e non dall'ultimo accesso
void reset(DIR*node){
        touched=node;
        free(oldpath);
        oldpath=NULL;
}
//Le funzioni sono tutte più o meno simili.
//la funzione parte da node che è la radice; altezza terrà conto di quanto stiamo andando in profondità
//next_pos e la risorsa estrapolata dal percorso che andremo ad ispezionare o operare su di essa se è una risorsa inesistente
//infatti se fosse una cartella da accedere, walk ci direbbe se è una cartella e il suo indirizzo verrà salvato in proxd per essere acceduto.
//last l'ultima cartella acceduta
int create(DIR *node){
 int altezza=1;
    DIR*last=node;
    char* next_pos=NULL;
    DIR* proxd=NULL;
//check qui controlla che il vecchio path e quello nuovo riguardino la stessa ultima cartella acceduta, se è così check ritorna la porzione di path utile
//all'operazione, per continuare da quella
    char *resumed_path=check(oldpath,supp_path);
                            if(resumed_path==NULL){
                                next_pos=seziona(supp_path);
                                proxd=walk(last->fdir,next_pos);
                            }
                            else{
                                //printf("%s\n",resumed_path);
                                last=touched;
                                next_pos=seziona(resumed_path);
                                proxd=walk(last->fdir,next_pos);
                            }
                            //questo while percorre tutto il path finchè non è terminato e non ci rimane che in next_pos il nome della risorsa che dobbiamo creare
                            //o eliminare o leggere...
                            //se proxd e il path non sono rispettivamente NULL e terminati contemporaneamnte significa che il percorso o non esiste
                            //o è errato, quindi stampa no e resetta oldpath e l'ultima cartella acceduta
                                    while(*supp_path!='\0'){
                                            if(proxd!=NULL){
                                                    last=proxd;
                                                    free(next_pos);
                                                    next_pos=seziona(supp_path);
                                                    proxd=walk(last->fdir,next_pos);
                                                    altezza=altezza+1;
                                                            if(!(altezza<255)){
                                                                printf("no\n");
                                                                reset(node);
                                                                return 0;
                                                            }
                                            }
                                            else{printf("no\n");
                                                     reset(node);
                                                     return 0;}
                                    }
                                    //se lo scorrimento del path è andato a buon fine salvo l'ultima cartella acceduta
                                    touched=last;
                                    //printf("%s\n",last->name);
                                    if(strlen(next_pos)<=255){//verifico che il nome della risorsa da creare non sia superiore a 255 caratteri
                                                        if(proxd==NULL){//verifico che non ci sia una cartella figlio con lo stesso nome
                                                                            if(see_figli(last->fson,next_pos)==NULL){//poi che non ci sia un file con lo stesso nome
                                                                                 addfigli(last,next_pos);//aggiungo il file
                                                                            }
                                                                            else printf("no\n");


                                                        }
                                                        else printf("no\n");

                                    }
                                    else printf("no\n");

//altrimenti se una delle tre condizioni non va si stampa no e ritorna
    free(next_pos);

return 0;
}
//create_dir è identica a create ma alla fine aggiunge una cartella figlio
int create_dir(DIR *node){
    int altezza=1;
    DIR*last=node;
    char* next_pos=NULL;
    DIR* proxd=NULL;
    char *resumed_path=check(oldpath,supp_path);
                            if(resumed_path==NULL){
                                next_pos=seziona(supp_path);
                                proxd=walk(last->fdir,next_pos);
                            }
                            else{
                                    //printf("%s\n",resumed_path);
                                last=touched;
                                next_pos=seziona(resumed_path);
                                proxd=walk(last->fdir,next_pos);
                            }
                                    while(*supp_path!='\0'){
                                            if(proxd!=NULL){
                                                    last=proxd;
                                                    free(next_pos);
                                                    next_pos=seziona(supp_path);
                                                    proxd=walk(last->fdir,next_pos);
                                                    altezza=altezza+1;
                                                            if(!(altezza<255)){
                                                                    reset(node);
                                                                printf("no\n");
                                                                return 0;
                                                            }
                                            }
                                            else{printf("no\n");
                                                    reset(node);
                                                     return 0;}
                                    }
                                    touched=last;
                                    //printf("%s\n",last->name);
                                    if(strlen(next_pos)<=255){
                                                        if(proxd==NULL){
                                                                            if(see_figli(last->fson,next_pos)==NULL){
                                                                                 add_dir(last,next_pos);
                                                                            }
                                                                            else printf("no\n");

                                                        }
                                                        else printf("no\n");
                                    }
                                    else printf("no\n");

    free(next_pos);

return 0;
}
//fa lo stesso ma arriva fino alla risorsa e ne legge il campo contenuto dopo aver verifcato che quella risorsa esista
int read(DIR *node){
 int altezza=1;
    DIR*last=node;
    char* next_pos=NULL;
    DIR* proxd=NULL;
    char *resumed_path=check(oldpath,supp_path);
                            if(resumed_path==NULL){
                                next_pos=seziona(supp_path);
                                proxd=walk(last->fdir,next_pos);
                            }
                            else{
                                last=touched;
                                next_pos=seziona(resumed_path);
                                proxd=walk(last->fdir,next_pos);
                            }
                                    while(*supp_path!='\0'){
                                            if(proxd!=NULL){
                                                    last=proxd;
                                                    free(next_pos);
                                                    next_pos=seziona(supp_path);
                                                    proxd=walk(last->fdir,next_pos);
                                                    altezza=altezza+1;
                                                            if(!(altezza<255)){
                                                                printf("no\n");
                                                                reset(node);
                                                                return 0;
                                                            }
                                            }
                                            else{printf("no\n");
                                                    reset(node);
                                                     return 0;}
                                    }
                                    touched=last;
                                    //printf("%s\n",last->name);
                                    if(strlen(next_pos)<=255){
                                            FS* toread=see_figli(last->fson,next_pos);
                                            if(toread!=NULL){
                                                                if(toread->content==NULL) printf("contenuto\n");
                                                                else  printf("contenuto %s\n",toread->content);
                                                }

                                            else printf("no\n");
                                    }
                                    else printf("no\n");

    free(next_pos);

return 0;
}

//come read ma scrive nel campo contenuto
int write(DIR *node){
    int altezza=1;
    DIR*last=node;
    char* next_pos=NULL;
    DIR* proxd=NULL;
    char *resumed_path=check(oldpath,supp_path);
                            if(resumed_path==NULL){
                                next_pos=seziona(supp_path);
                                proxd=walk(last->fdir,next_pos);
                            }
                            else{
                                last=touched;
                                next_pos=seziona(resumed_path);
                                proxd=walk(last->fdir,next_pos);
                            }
                                    while(*supp_path!='\0'){
                                            if(proxd!=NULL){
                                                    last=proxd;
                                                    free(next_pos);
                                                    next_pos=seziona(supp_path);
                                                    proxd=walk(last->fdir,next_pos);
                                                    altezza=altezza+1;
                                                            if(!(altezza<255)){
                                                                printf("no\n");
                                                                reset(node);
                                                                return 0;
                                                            }
                                            }
                                            else{printf("no\n");
                                            reset(node);
                                                     return 0;}
                                    }
                                    touched=last;
                                    //printf("%s\n",last->name);
                                    if(strlen(next_pos)<=255){
                                            FS* towrite=see_figli(last->fson,next_pos);
                                            if(towrite!=NULL){
                                                        if(towrite->content!=NULL) free(towrite->content);
                                                        towrite->content=(void*)duplica(extra);

                                                        int l=strlen(extra);
                                                        printf("ok %d\n",l);
                                                }
                                                else printf("no\n");
                                    }
                                    else printf("no\n");

    free(next_pos);

return 0;
}
//come read e write ma.. guarda più sotto
int del(DIR *node){
    int altezza=1;
    DIR*last=node;
    char* next_pos=NULL;
    DIR* proxd=NULL;
    char *resumed_path=check(oldpath,supp_path);
                            if(resumed_path==NULL){
                                next_pos=seziona(supp_path);
                                proxd=walk(last->fdir,next_pos);
                            }
                            else{
                                last=touched;
                                next_pos=seziona(resumed_path);
                                proxd=walk(last->fdir,next_pos);
                            }
                                    while(*supp_path!='\0'){
                                            if(proxd!=NULL){
                                                    last=proxd;
                                                    free(next_pos);
                                                    next_pos=seziona(supp_path);
                                                    proxd=walk(last->fdir,next_pos);
                                                    altezza=altezza+1;
                                                            if(!(altezza<255)){
                                                                printf("no\n");
                                                                reset(node);
                                                                return 0;
                                                            }
                                            }
                                            else{printf("no\n");
                                            reset(node);
                                                     return 0;}
                                    }
                                    touched=last;
                                   //printf("%s\n",last->name);
                                    if(strlen(next_pos)<=255){
                                                           if(proxd!=NULL){
                                        //arrivato qui dopo aver fatto come le altre operazioni
                                        //verifica che l'elemento da eliminare esista e che non abbia figli (se è file non lo fa ovviamente)
                                                                if(proxd->figli==0){
                                                                        int h=hash(next_pos);//accede la lista dove si trova cartella delenda
                                                                        if(last->fdir[h]==proxd) last->fdir[h]=proxd->next;//si sistema la lista in modo
                                                                        else fixer(last->fdir,next_pos,h);//che non ci siano danni dalla eliminazione e che le liste siano comunque intatte
                                                                        //ed accessibili (a ciò è utile la funzione fixer)
                                                                        free(proxd);//elimino la risorsa
                                                                        last->figli=last->figli-1;//diminuisco i figli
                                                                        printf("ok\n");
                                                                 }
                                                                 else printf("no\n");

                                                        }
                                                        else{FS* sonkill=see_figli(last->fson,next_pos);
                                                                if(sonkill!=NULL){
                                                                        int h=hash(next_pos);
                                                                        if(last->fson[h]==sonkill) last->fson[h]=sonkill->next;
                                                                        else fixer2(last->fson,next_pos,h);//identico alla cartella per i file
                                                                        free(sonkill);
                                                                        last->figli=last->figli-1;
                                                                        printf("ok\n");
                                                                    }

                                                                else printf("no\n");
                                                        }
                                    }
                                    else printf("no\n");

    free(next_pos);

return 0;
}
//come delete normale ma qui non ci si pone il problema di sistemare le liste
//ma si cancellano le strutture figlio e si usa fixer per aggiustare la lista in cui si trovava il file eliminato ricorsivamente
int del_r(DIR *node){
    int altezza=1;
    DIR*last=node;
    char* next_pos=NULL;
    DIR* proxd=NULL;
    char *resumed_path=check(oldpath,supp_path);
                            if(resumed_path==NULL){
                                next_pos=seziona(supp_path);
                                proxd=walk(last->fdir,next_pos);
                            }
                            else{
                                last=touched;
                                next_pos=seziona(resumed_path);
                                proxd=walk(last->fdir,next_pos);
                            }
                                    while(*supp_path!='\0'){
                                            if(proxd!=NULL){
                                                    last=proxd;
                                                    free(next_pos);
                                                    next_pos=seziona(supp_path);
                                                    proxd=walk(last->fdir,next_pos);
                                                    altezza=altezza+1;
                                                            if(!(altezza<255)){
                                                                printf("no\n");
                                                                reset(node);
                                                                return 0;
                                                            }
                                            }
                                            else{printf("no\n");
                                                    reset(node);
                                                     return 0;}
                                    }
                                    touched=last;
                                    //printf("%s\n",last->name);
                                    if(strlen(next_pos)<=255){
                                              if(proxd!=NULL){
                                                            int h=hash(next_pos);
                                                            if(last->fdir[h]==proxd) last->fdir[h]=proxd->next;
                                                            f_rem(proxd->fson);//elimina tutti i filgli
                                                            d_rem(proxd->fdir);//elimina le cartelle figlio
                                                            fixer(last->fdir,next_pos,h);//aggiusta la lista dell'eliminato
                                                            free(proxd);
                                                            last->figli=last->figli-1;
                                                            printf("ok\n");
                                            }
                                            else{FS* sonkill=see_figli(last->fson,next_pos);
                                                    if(sonkill!=NULL){//idem a sopra
                                                            int h=hash(next_pos);
                                                            if(last->fson[h]==sonkill) last->fson[h]=sonkill->next;
                                                            else fixer2(last->fson,next_pos,h);
                                                            free(sonkill);
                                                            last->figli=last->figli-1;
                                                            printf("ok\n");
                                                            }

                                                      else printf("no\n");

                                            }
                                    }
                                    else printf("no\n");

    free(next_pos);

return 0;
}

//find prende il nome da cercare e indirizzo radice
//e scansiona l'intera struttura costruita alla ricerca del nome
//si salveranno tutti percorsi trovati che poi verranno ordinati da bubblesort, stampati e poi la lista sarà cancellata per non occupare spazio
int find(char*name,DIR*RAD){
    int h=hash(name);
    faf(name,RAD->fson,"",head,h);
    founddir(name,RAD->fdir,"",head,h);
    bubble_sort(head);
    if(head==NULL) printf("no\n");
    else printer(head);
    clean(head);
    head=NULL;
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//nel main viene creata la radice su cui si baserà tutta la struttura
//c'è un ciclo che legge da terminale le istruzioni
//esse vengono divise in tre campi: comando, sentiero ed extra cioè il testo
//in base al comando salvato lo switch lancerà un'operazione
int main(){
DIR RADICE;
RADICE.name="r";
RADICE.figli=0;
RADICE.fdir=(DIR**)calloc(NUM,sizeof(DIR*));
RADICE.fson=(FS**)calloc(NUM,sizeof(FS*));
RADICE.next=NULL;
int what=0;//contiene il numero dell'operzione riconosciuta
char *input=NULL;//input da terminale

do{
            if(input!=NULL){free(input);//per non occupare memoria i vecchi inout sono cancellati
                                    input=NULL;}
        input=elabora_input(stdin,20);
        if(strcmp(input,"exit")==0) return 0;
        //printf("input:%s                                   t\n",input);
        what=istruzione(input);

    switch(what){

    case 0:
            create(&RADICE);
            break;
    case 1:
             create_dir(&RADICE);
            break;
    case 2:
            read(&RADICE);
            break;
    case 3:
            write(&RADICE);
            break;
    case 4:
            del(&RADICE);
            break;
    case 5:
             del_r(&RADICE);
            break;
    case 6:
            find(supp_path,&RADICE);
            break;
    case 7:
            exit(0);
    default:
            printf("\n");
            break;
    }

}while(strcmp(comando,"exit")!=0);



return 0;
}
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//CODICE DI KEVIN MATO MATRICOLA  845726
////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
