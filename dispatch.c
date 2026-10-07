
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct unit{
    int id; 
    char type; //tipul echipajului
    int availability;
};

typedef struct incident{
    int id;
    char priority[10];
    char *description;
    char status[15];
    struct incident *next, *prev;
}CelIncident, *LIncident;

typedef struct intervention{
    struct incident *incident;
    struct unit *unit;
    struct intervention *next, *prev;
}CelIntervention, *LIntervention;

typedef struct nodeIncident
{
    LIncident incident_ref;
    struct nodeIncident *next;
}QnodeInc;

typedef struct queueInc{
    QnodeInc *front, *rear;
}QueueInc;

typedef struct nodeUnit{
    struct unit *data;
    struct nodeUnit *next;
}QnodeUnit;

typedef struct queueUnit{
    QnodeUnit *front, *rear; 
}QueueUnit;

typedef struct stackInt{
    LIntervention intervention_ref;
    struct stackInt *next;
}StackInt;

typedef struct system{
    struct unit* units;
    LIncident incidents;
    LIntervention interventions;
    int no_units;
    QueueInc *q_high, *q_medium, *q_low;
    QueueUnit *queue_available_units;
    StackInt *dispatchStack;
}System;

//functia initializeaza lista de incidente, folosind incidentul 0 ca santinela
void initIncident(System *s)
{
    s->incidents=(LIncident)malloc(sizeof(CelIncident));
    if(s->incidents == NULL) return;
    s->incidents->id=0;
    s->incidents->description=strdup("test incident");
    strcpy(s->incidents->priority, "low");
    strcpy(s->incidents->status, "solved"); 
    s->incidents->next=s->incidents;
    s->incidents->prev=s->incidents;
}
//functie initializare lista de interventii
void initIntervention(System *s)
{
    s->interventions=(LIntervention)malloc(sizeof(CelIntervention));
    if(s->interventions==NULL) return;
    s->interventions->incident=NULL;
    s->interventions->unit=NULL;
    s->interventions->next=s->interventions;
    s->interventions->prev=s->interventions;
}
//functie de initializare pentru toate cozile
void initQueues(System *s)
{
    s->q_high=(QueueInc*)malloc(sizeof(QueueInc));
    s->q_high->front=s->q_high->rear=NULL;
    s->q_medium=(QueueInc*)malloc(sizeof(QueueInc));
    s->q_medium->front=s->q_medium->rear=NULL;
    s->q_low=(QueueInc*)malloc(sizeof(QueueInc));
    s->q_low->front=s->q_low->rear=NULL;
    s->queue_available_units=(QueueUnit*)malloc(sizeof(QueueUnit));
    s->queue_available_units->front=s->queue_available_units->rear=NULL;
}
//functie de initializare a sistemului per total
void initSystem(System **s, int no_units)
{
    *s=(System*)malloc(sizeof(System));
    if(*s == NULL) return; 
    (*s)->no_units=no_units;
    (*s)->units=(struct unit*)malloc(no_units*sizeof(struct unit));
    if((*s)->units==NULL) return;
    (*s)->incidents=NULL;
    (*s)->interventions=NULL;
    (*s)->dispatchStack=NULL;
    initIncident(*s);
    initIntervention(*s);
    initQueues(*s);
}
//functie de adaugare in coada de echipaje
void enqueueUnit(struct unit *u, QueueUnit *q)
{
    QnodeUnit *new=(QnodeUnit*)malloc(sizeof(QnodeUnit));
    if(new==NULL) return;
    new->data=u;
    new->next=NULL;
    if(q->rear==NULL)
    {
        q->rear=q->front=new;
    }
    else
    {
        q->rear->next=new;
        q->rear=new;
    }

}
//functie de adaugare in coada de incidente
void enqueueIncident(LIncident l, QueueInc *q)
{
    QnodeInc *new=(QnodeInc*)malloc(sizeof(QnodeInc));
    if(new==NULL) return;
    new->incident_ref=l;
    new->next=NULL;
    if(q->rear==NULL)
    {
        q->rear=q->front=new;
    }
    else{
        q->rear->next=new;
        q->rear=new;
    }
}
//functie de eliminare din coada de echipaje
void dequeueUnit(QueueUnit *q, struct unit **echipaj)
{
    if(q->front==NULL)
    {
        *echipaj=NULL;
        return;
    }
    QnodeUnit *aux=q->front;
    *echipaj= aux->data;
    q->front=q->front->next;
    if(q->front==NULL)
        q->rear=NULL;
    free(aux);
}
//functie de eliminare din coada de incidente
void dequeueIncident(LIncident *l, QueueInc *q)
{
    if(q->front==NULL)
    {
        *l=NULL;
        return;
    }
    QnodeInc *aux=q->front;
    *l=aux->incident_ref;
    q->front=q->front->next;
    if(q->front==NULL)
        q->rear=NULL;
    free(aux);
}
//functie de adaugare in stiva de interventii
void pushIntervention(LIntervention l, StackInt **s)
{
    StackInt *new= (StackInt*)malloc(sizeof(StackInt));
    if(new == NULL) return;
    new->intervention_ref=l;
    new->next=*s;
    *s=new;
}
//functie de eliminare din stiva de interventii
void popIntervention(System *s)
{
    if(s->dispatchStack == NULL) return;
    StackInt *varf = s->dispatchStack;
    s->dispatchStack=s->dispatchStack->next;
    free(varf);
}
//functie de adaugare incident
void ADD_INCIDENT(System *s, int id, char* priority, char* description)
{
    //adaugam in lista
    LIncident new=(LIncident)malloc(sizeof(CelIncident));
    if(new==NULL) return;
    new->id=id;
    strcpy(new->status, "queued");
    strcpy(new->priority, priority);
    new->description=strdup(description);
    LIncident santinela=s->incidents;
    LIncident last=santinela->prev;
    new->next=santinela;
    new->prev=last;
    last->next=new;
    santinela->prev=new;
    //adaugam in coada corespunzatoare prioritatii
    if(strcmp(priority, "high")==0)
        enqueueIncident(new, s->q_high);
    else if(strcmp(priority, "medium")==0)
        enqueueIncident(new, s->q_medium);
    else if (strcmp(priority, "low")==0)
        enqueueIncident(new,s->q_low);

}
//functia parcurge coada de echipaje disponibile si le numara
void CHECK_UNITS_AVAILABILITY(System *s, FILE *out)
{
    QnodeUnit *aux=s->queue_available_units->front;
    int k=0;
    while(aux!=NULL)
    {
        k++;
        aux=aux->next;
    }
    fprintf(out,"Number of available units: %d\n",k);
}

void DISPATCH(System *s, FILE *out)
{   
    struct unit *echipaj = NULL;
    LIncident incident = NULL;
    //verificam daca sunt echipaje disponibile
    if(s->queue_available_units->front==NULL)
    {
        fprintf(out, "INVALID OPERATION! ERROR 404\n");
        return;
    }
    //verificam cozile de prioritate pe rand
    dequeueIncident(&incident, s->q_high);
    if(incident == NULL)
        dequeueIncident(&incident, s->q_medium);
    if(incident == NULL)
        dequeueIncident(&incident, s->q_low);
    if(incident == NULL)
    {
        fprintf(out, "INVALID OPERATION! ERROR 404\n");
        return;
    }
    //scoatem echipajul din coada si creem o interventie
    dequeueUnit(s->queue_available_units, &echipaj);
    strcpy(incident->status, "intervened");
    LIntervention interventie=(LIntervention)malloc(sizeof(CelIntervention));
    if(interventie == NULL) return;
    interventie->incident=incident;
    interventie->unit=echipaj;
    LIntervention santinela=s->interventions;
    LIntervention last = santinela->prev;
    interventie->next=santinela;
    interventie->prev=last;
    last->next=interventie;
    santinela->prev=interventie;
    //adaugam interventia in stack si marcam echipajul ca indisponibil
    echipaj->availability=0;
    pushIntervention(interventie, &(s->dispatchStack));
}

void UNDO_LAST_DISPATCH(System *s, FILE *out)
{
    if(s->dispatchStack == NULL)
    {
        fprintf(out, "INVALID OPERATION! ERROR 404\n");
        return;
    }
    while (s->dispatchStack != NULL)
    {
        StackInt *top = s->dispatchStack;
        LIntervention aux = top->intervention_ref;
        if(strcmp(aux->incident->status, "intervened") == 0)
        {
            strcpy(aux->incident->status, "queued");
            QueueInc *q = NULL;
            //Stabilim coada in care se afla incidentul
            if(strcmp(aux->incident->priority, "high")==0) q=s->q_high;
            else if (strcmp(aux->incident->priority, "medium")==0) q=s->q_medium;
            else q=s->q_low;
            //Adaugam incidentul la inceputul cozii
            QnodeInc *new = (QnodeInc*)malloc(sizeof(QnodeInc));
            new->incident_ref=aux->incident;
            if(q->rear==NULL)
                q->rear=new;
            new->next=q->front;
            q->front=new;
            //Facem echipajul disponibil din nou
            aux->unit->availability=1;
            enqueueUnit(aux->unit, s->queue_available_units);
            //Stergem interventia din stiva
            aux->prev->next = aux->next;
            aux->next->prev = aux->prev;
            free(aux);
            popIntervention(s);
            return;
        } 
        else
        {
            s->dispatchStack=top->next;
            free(top);
        }
    }
    
}

void SOLVED_INCIDENT(System *s, int ID, FILE *out)
{
    LIntervention santinela=s->interventions;
    LIntervention aux = santinela->next;
    //cautam incidentul cu ID-ul specificat
    while(aux != santinela && aux->incident->id != ID)
        aux=aux->next;
    if(aux == santinela)
    {
        fprintf(out, "INVALID OPERATION! ERROR 404\n");
        return;
    }
    //verificam daca are statusul potrivit
    if(strcmp(aux->incident->status,"intervened") != 0)
    {
        fprintf(out, "INVALID OPERATION! ERROR 404\n");
        return;
    }
    //marcam incidentul ca rezolvat si adaugam echipajul inapoi in coada
    strcpy(aux->incident->status, "solved");
    aux->unit->availability=1;
    enqueueUnit(aux->unit,s->queue_available_units);
}

void SHOW_UNIT(System *s, int ID, FILE *out)
{
   struct unit *u = NULL;
   //parcurgem vectorul si cautam echipajul
   for(int i=0; i< s->no_units; i++)
   {
        if(s->units[i].id == ID)
        {
            u=&(s->units[i]);
            break;
        }     
   }
   if(u == NULL)
   {
        fprintf(out, "INVALID OPERATION! ERROR 404\n");
        return;
   }
   //afisam detaliile despre echipaj
   if(u->availability == 1)
    fprintf(out, "Unit %d is type %c and is available\n", ID, u->type);
   else  fprintf(out, "Unit %d is type %c and is unavailable\n", ID, u->type);
}
//cautam incidentul si afisam informatiile despre el
void SHOW_INCIDENT(System *s, int ID, FILE *out)
{
    LIncident santinela=s->incidents;
    LIncident aux = santinela->next;
    while(aux != santinela && aux->id != ID)
        aux=aux->next;
    if(aux == santinela)
    {
        fprintf(out, "INVALID OPERATION! ERROR 404\n");
        return;
    }
    fprintf(out, "Incident %d has %s priority, the following description: \"%s\" and is %s\n",
         aux->id,aux->priority,aux->description,aux->status);
}

void SHOW_INTERVENTIONS(System *s, FILE *out)
{
    LIntervention santinela = s->interventions;
    //verificam daca exista interventii
    if(santinela->next == santinela)
    {
        fprintf(out, "No intervention has been initiated\n");
        return;
    }
    //parcurgem lista si afisam detaliile despre fiecare interventie
    LIntervention aux = santinela->next;
    while(aux != santinela)
    {
        fprintf(out, "Incident %d was assigned to unit %d, and has the following status: \"%s\"\n",
        aux->incident->id,aux->unit->id,aux->incident->status);
        aux=aux->next;
    }
}
//functie de eliberare a memoriei la finalul programului
void free_system(System *s)
{
    if(s==NULL)
        return;
    if(s->incidents != NULL)
    {
        LIncident aux = s->incidents->next;
        while(aux != s->incidents)
        {
            LIncident next=aux->next;
            free(aux->description);
            free(aux);
            aux=next;
        }
        free(s->incidents->description);
        free(s->incidents);
    }
    if(s->interventions != NULL)
    {
        LIntervention aux = s->interventions->next;
        while(aux != s->interventions)
        {
            LIntervention next=aux->next;
            free(aux);
            aux=next;
        }
        free(s->interventions);
    }
    if(s->q_high != NULL)
    {
        QnodeInc *aux = s->q_high->front;
        while (aux != NULL)
        {
            QnodeInc *next=aux->next;
            free(aux);
            aux=next;
        }
        free(s->q_high);
    }
    if(s->q_medium != NULL)
    {
        QnodeInc *aux = s->q_medium->front;
        while (aux != NULL)
        {
            QnodeInc *next=aux->next;
            free(aux);
            aux=next;
        }
        free(s->q_medium);
    }
    if(s->q_low != NULL)
    {
        QnodeInc *aux = s->q_low->front;
        while (aux != NULL)
        {
            QnodeInc *next=aux->next;
            free(aux);
            aux=next;
        }
        free(s->q_low);
    }
    if(s->queue_available_units != NULL)
    {
        QnodeUnit *aux = s->queue_available_units->front;
        while (aux != NULL)
        {
            QnodeUnit *next=aux->next;
            free(aux);
            aux=next;
        }
        free(s->queue_available_units);
    }
    StackInt *aux=s->dispatchStack;
    while(aux!=NULL)
    {
        StackInt *next=aux->next;
        free(aux);
        aux=next;    
    }
    free(s->units);
    free(s);
}

int main()
{
    FILE *fin = fopen("tema1.in", "r");
    FILE *fout = fopen("tema1.out", "w");
    int nr_units;
    fscanf(fin, "%d",&nr_units);
    System *s=NULL;
    initSystem(&s,nr_units);
    //adaugam pe rand fiecare echipaj
    for(int i=0; i<nr_units; i++)
    {
        int id;
        char type;
        fscanf(fin,"%d %c",&id,&type);
        s->no_units=nr_units;
        s->units[i].id=id;
        s->units[i].type=type;
        s->units[i].availability=1;
        //punem unitatea in coada de disponibilitate
        enqueueUnit(&(s->units[i]),s->queue_available_units);
    }
    //citim numarul de operatii
    int n;
    fscanf(fin,"%d",&n);
    char command[30];
    //citim fiecare comanda pe rand si apelam functia corespunzatoare
    while (n)
    {
        fscanf(fin,"%s", command);
        if(strcmp(command, "ADD_INCIDENT")==0)
        {
            int ID;
            char priority[30], desc[200];
            fscanf(fin, "%d %s ",&ID,priority);
            //sarim peste prima ghilimea
            fgetc(fin);
            //citim pana la urmatoarea ghilimea
            fscanf(fin,"%[^\"]",desc);
            //sarim peste a doua ghilimea
            fgetc(fin); 
            ADD_INCIDENT(s,ID, priority, desc);
        }
        else if(strcmp(command,"SHOW_INCIDENT")==0)
        {
            int ID;
            fscanf(fin,"%d",&ID);
            SHOW_INCIDENT(s,ID,fout);
        }
        else if(strcmp(command, "CHECK_UNITS_AVAILABILITY")==0)
        {
            CHECK_UNITS_AVAILABILITY(s,fout);
        }
        else if(strcmp(command,"SHOW_UNIT")==0)
        {
            int ID;
            fscanf(fin,"%d",&ID);
            SHOW_UNIT(s,ID,fout);
        }
        else if(strcmp(command,"DISPATCH")==0)
        {
            DISPATCH(s,fout);
        }
        else if(strcmp(command,"UNDO_LAST_DISPATCH")==0)
        {
            UNDO_LAST_DISPATCH(s,fout);
        }
        else if(strcmp(command,"SOLVED_INCIDENT")==0)
        {
            int ID;
            fscanf(fin,"%d",&ID);
            SOLVED_INCIDENT(s,ID,fout);
        }
        else if(strcmp(command,"SHOW_INTERVENTIONS")==0)
        {
            SHOW_INTERVENTIONS(s,fout);
        }
        n--;
    }
    fclose(fin);
    fclose(fout);
    free_system(s);
    return 0;
}
