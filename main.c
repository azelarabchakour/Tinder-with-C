#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <time.h>

typedef struct c
{
   int sens;  // 0 = recu    1 = envoyé
   char* msg;
   char* dt_heure;
   struct c *next;
}conv;

//Noeud de l'arbre des personne
typedef struct compt
{
   char* nom_pre;
   int age;
   int distance;
   int like;   //Notre choix    y= Yes     n= No
   int compatible;   // 0 = No   1=Yes
   conv* listMsg;
   struct compt *g;
	struct compt *d;
}compte;

typedef struct mElt
{
   char* titre;
   compte *t;
   int p;
   void (*func)();  //pointeur sur fonction
   char te;
}mElt;

//utilisée pour stocker les informations du fichier
typedef struct selection
{
   char* nom_pre;
   char s;    // H   -  F
   int age;
   int distance;
   int like;
   struct selection *next;
}select;

//paramètres de selection
typedef struct
{
   int age_min;
   int age_max;
   int dis_min;
   int dis_max;
   char s;    // H = Homme  |  F = Femme
}param;

typedef struct arblis
{
	compte *add;
	int pos;
	struct arblis *next;
}arblis;

/*-- Prototype des fonctions --*/
void goTo(int);
mElt* createMenu(int,int);
void gererMenu(mElt*);
void setNewParam(char s, int amin, int amax,int dmin,int dmax);
void setNewParam_S();
void aff(compte* R);
void parcourirPerson(select *lp);
int rand(void);
void addperson_inlist(arblis**,compte*);
//void addperson_inlist(select *);
/*-----------------------------*/

param list_param;   //Paramètres de selection
compte *Racine = NULL;
select *list_personne = NULL;
select *list_personne_selected = NULL;
int newselect = 0;    // 0= il faut choisir un nouveau fichier// 1= on continu le parcours de la liste chainnée
int retour = 0;
arblis *arbL = NULL;
compte *contact_supp = NULL;

/*-----------------------------*/

select* loadPerson(char* chemin)
{
   list_personne=NULL;
   FILE* p;
   p=fopen(chemin,"r");
   if(p)
      {
        do
        {
            select* np=NULL;
            np=(select*)malloc(sizeof(select));
            np->nom_pre=(char*)malloc(20);
            fscanf(p,"%s %c %d %d %d\n",np->nom_pre,&np->s,&np->age,&np->distance,&np->like);
            //Insertion au début de la liste chainée
            np->next=list_personne;
            list_personne=np;
         }while(!feof(p));
      }else{
         printf("Erreur lors de l'ouverture du fichier !");
      }
   return list_personne;
}

//Ajouter une personne à l'arbre de selection
/*void addperson_inlist(arblis **a,compte* no)
{
	static int i;
	if(*a == NULL)
   {
		*a=(arblis*)malloc(sizeof(arblis));
		(*a)->add=no;
		(*a)->pos=++i;
	}else
   {
		addperson_inlist(&((*a)->next),no);
	}
}*/

/*void addperson_inlist(select *la)
{
   select *np = NULL;
   np = (select*)malloc(sizeof(select));
   np->nom_pre = (char*)malloc(20);
   strncpy(np->nom_pre,la->nom_pre,strlen(la->nom_pre));
   np->age = la->age;
   np->distance = la->distance;
   np->like = la->like;
   np->s = la->s;
   np->next = list_personne_selected;
   list_personne_selected = np;
}*/

void myfct(compte *R)
{
   if (R)
   {
      if (R->compatible)
      {
         select *la = NULL;
         la = (select*)malloc(sizeof(select));
         la->nom_pre = malloc(20);
         la->age = R->age;
         la->distance = R->distance;
         la->like = R->like;
         strncpy(la->nom_pre,R->nom_pre,strlen(R->nom_pre));
         la->next = list_personne_selected;
         list_personne_selected = la;
      }
      myfct(R->d);
      myfct(R->g);
   }
}

void addPerson(compte *nc,compte **R)
{
   if (*R == NULL)
   {
	   *R=nc;
	}
   else {
      if (nc->age<=(*R)->age) addPerson(nc,&((*R)->g));
      else addPerson(nc,&((*R)->d));
   }
}

//Ajoute la personne p dans l'arbre Racine !
void insertPerson(select *p)
{
   compte *nc;
   int s;
   int c=userChoice();
   if ( c != -1 )
   {
      nc=malloc(sizeof(compte));
      nc->nom_pre=malloc(strlen(p->nom_pre)+1);
      strncpy(nc->nom_pre,p->nom_pre,strlen(p->nom_pre)+1);
      nc->age=p->age;
      nc->distance=p->distance;
      nc->like= c;
      if (c == 1) nc->compatible = 1;
      else nc->compatible=0;
      nc->g=NULL;
		nc->d=NULL;
      addPerson(nc,&Racine);
      //if(nc->compatible) myfct(nc);
   }
}

void parcourirPerson(select *lp)
{
	int n=0;
    while(lp){
      if (param_test(lp) == 1){ //1 = vrai
         showPerson(lp);
     }
      lp=lp->next;
   }
   system("cls");
   printf("<--quiter     |      rep�ter-->\n");
   int c=getch();
            switch(c){
             case 224:{
                 c=getch();
                 switch (c){
               case 75:{ //GAUCHE
                  gererMenu(createMenu(1,0));
                  break;
               }
               case 77:{ //DROITE
                  showSelection();
                  break;
					}
            }
         }
      }
}

void showPerson(select *p){
   //Affichage des infos..
   system("cls");
   printf("Nom & prenom : %s\n",p->nom_pre);
   printf("sexe:%c \n",p->s);
   printf("age : %d \n",p->age);
   printf("distance : %d \n",p->distance);
   printf("like : %d \n",p->like);
   insertPerson(p);
}




//Parcour la liste des personnes et affiche uniquement les personnes respectant les critères de selection
compte* shearparent(compte *R,compte *nc){
	if(nc==R->g || nc==R->d) return R;
	compte * temp;
	if(temp = shearparent(R->g,nc)){
		return temp;
	}else if( temp = shearparent(R->d,nc)){
		return temp;
	}
	return NULL;
}

compte* searchvide(compte* R){
	if(R->d == NULL && R->g == NULL)
		return R;
	else{
		compte * vide;
		if(vide = searchvide(R->g)){
			return vide;
		}else if( vide = searchvide(R->d)){
			return vide;
		}
		return NULL;
	}
}

void supall(compte *node){
   if (node == NULL) return;
   supall(node->g);
   supall(node->d);
   free(node);
}

void supupdate(compte *node){
	supall(node);
	while (arbL){
      arblis *a=arbL;
      arbL=arbL->next;
      free(a);
   }
   arbL=NULL;
	gererMenu(createMenu(4,0));
}

void supone(compte *node){
	printf("%s",contact_supp->nom_pre);
	getch();
	gererMenu(createMenu(4,0));
}

void matchAction(compte *CO)
{
   contact_supp=CO;
   gererMenu(createMenu(6,0));
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

int param_test(select *p){
      if ( p->age<list_param.age_max && p->age>list_param.age_min && p->s==list_param.s && p->distance<list_param.dis_max && p->distance>list_param.dis_min){ //1 = vrai
         return 1;
     }
   return 0;
}

int userChoice(){
   int c=0,r=-1,i=0;
   printf("Prenez une Decision !\n");
   printf("<- OUI     |      NON ->");
   while (i!=1){
        c=getch();
         switch(c){
            case 27:{
               newselect=1;
               i=1;
               break;
            }
             case 224:{
                 c=getch();
                 switch (c){
               case 75:{ //GAUCHE
                  r=1;
                  i=1;
                  break;
               }
               case 77:{ //DROITE
                  r=0;
                  i=1;
                  break;
					}
            }
         }
      }
   }
   return r;
}




void showParameters(int choix){
   param Parameters;
   Parameters=list_param;
   system("cls");
   printf("Age Max : %d \n Age Min : %d \n Distance Max : %d \n Distance Min : %d \n Sexe : %c \n",Parameters.age_max,Parameters.age_min,Parameters.dis_max,Parameters.dis_min,Parameters.s);
   printf("Pour modifier les parametres entrez 1 s non 0 \n");
   scanf("%d",&choix);
	if(choix==1){
		setNewParam_S();
	}
}

void setNewParam_S(){
   char s;
	int amin,amax,dmin,dmax;
   printf("sexe : ");
	scanf("%c",&s);
   printf("age min : ");
	scanf("%d",&amin);
   printf("age max : ");
	scanf("%d",&amax);
   printf("distance min  : ");
	scanf("%d",&dmin);
   printf("distance max : ");
	scanf("%d",&dmax);
   setNewParam(s,amin,amax,dmin,dmax);
}


void showSelection(){
   time_t t;
   char str[10];
   char s[]="fic";
   srand((unsigned) time(&t));
   snprintf(str,10,"%s%d.txt",s,rand()%3);
   parcourirPerson(loadPerson("file.txt"));
}

void setNewParam(char s, int amin, int amax,int dmin,int dmax){
   list_param.s=s;
   list_param.age_min=amin;
   list_param.age_max=amax;
   list_param.dis_min=dmin;
   list_param.dis_max=dmax;
}

/********************************************************************************************************************************************************/
/*******************************************************fonction du recherche arbre**********************************************************************/
int nbr_selec(compte *R){
  if(R==NULL) return 0;
  else return 1+nbr_selec(R->d)+nbr_selec(R->g);
}
int nbr_acc(compte *R){
	if(R==NULL) return 0;
	else if(R->like) return 1+nbr_acc(R->d)+nbr_acc(R->g);
	else return 0+nbr_acc(R->d)+nbr_acc(R->g);
}
int compati(compte *R){
	if(R==NULL) return 0;
	else if(R->compatible) return 1+nbr_acc(R->d)+nbr_acc(R->g);
	else return 0+nbr_acc(R->d)+nbr_acc(R->g);
}
void aff_comp(compte *R){
	system("cls");
	printf("Nombre des matches:\t%d",compati(R));
	printf("\npress any key to continue...");
	getch();
	gererMenu(createMenu(5,0));
}
void aff_acc(compte *R){
	system("cls");
	printf("nombre de compte acceptes:\t%d",nbr_acc(R));
	printf("\npress any key to continue...");
	getch();
	gererMenu(createMenu(5,0));
}
void aff_rej(compte *R){
	system("cls");
	printf("nombre de compte rejete:\t%d",nbr_selec(R)-nbr_acc(R));
	printf("\npress any key to continue...");
	getch();
	gererMenu(createMenu(5,0));
}
int somme_age(compte *R){
  if(R==NULL) return 0;
  else return R->age +somme_age(R->g)+somme_age(R->d);
}
void aff_sum(compte *R){
	system("cls");
	printf("Somme d'age:\t%d",somme_age(R));
	printf("\npress any key to continue...");
	getch();
	gererMenu(createMenu(5,0));
}

void aff_nbr_selec(compte *R){
	system("cls");
	printf("Nombre de compte selectione:\t%d",nbr_selec(R));
	printf("\npress any key to continue...");
	getch();
	gererMenu(createMenu(5,0));

}
int sea(compte *R,char* str){
    if(R==NULL) return 0;
    else if(strcmp(R->nom_pre,str)==0){
	 printf("Ce compte existe\nVoila ces infos:\nage:%d\ndistance:%d\ncompatibilte:%d",R->age,R->distance,R->compatible);
	 return 1;
	 }
    sea(R->d,str);
	 sea(R->g,str);
}
void aff_sea(compte *R){
	system("cls");
	char *s=malloc(30);
	printf("Entre le nom a rechercher:\n");
	scanf("%s",s);
	system("cls");
	if(sea(R,s)){
		printf("\npress any key to continue...");
		getch();
   	gererMenu(createMenu(5,0));
	}else{
		printf("Ce compte n'existe pas");
		printf("\npress any key to continue...");
		getch();
	   gererMenu(createMenu(5,0));
	}
}
int max_dis(compte *R){
	if (R == NULL) return 0;
	int res = R->distance;
	int lres = max_dis(R->g);
	int rres = max_dis(R->d);
	if (lres > res)
	res = lres;
	if (rres > res)
	res = rres;
	return res;
}
void affdis_max(compte *R){
	system("cls");
	printf("distance max:\t%d",max_dis(R));
	printf("\npress any key to continue...");
	getch();
	gererMenu(createMenu(5,0));
}
int dismin(compte *R){
    if (R == NULL) return 0;
    int res = R->distance;
    int lres = dismin(R->g);
    int rres = dismin(R->d);
    if (lres < res)
      res = lres;
    if (rres < res)
      res = rres;
    return res;
	}
void affdis_min(compte *R){
	system("cls");
	printf("distance minimal:\t%d",dismin(R));
	printf("\npress any key to continue...");
	getch();
	gererMenu(createMenu(5,0));
}
char* jeune(compte *R){
	if(R==NULL) return "arbre vide";
if (R->g!=NULL) jeune(R->g);
else  return(R->nom_pre);
}
void aff_J(compte *R){
	system("cls");
	printf("Nom du plus jeune est:%s",jeune(R));
	printf("\npress any key to continue...");
	getch();
	gererMenu(createMenu(5,0));
}
char* agee(compte *R){
	if(R==NULL) return "arbre vide";
if (R->d!=NULL) agee(R->d);
else  return(R->nom_pre);
}
void aff_A(compte *R){
	system("cls");
	printf("Nom du plus ag�e est:%s",agee(R));
	printf("\npress any key to continue...");
	getch();
	gererMenu(createMenu(5,0));
}
/********************************************************************************************************************************************************/
/********************************************************************************************************************************************************/

conv* createSms(char* dt, char* h, char* m)
{
   conv *ms=(conv*)malloc(sizeof(conv));
   if (ms)
   {
      return ms;
   }
   return NULL;
}

void showM()
{
   gererMenu(createMenu(2,0));
}

conv* createMsg(int s, char* HM, char* msg1)
{
   conv* m=malloc(sizeof(conv));
   if (m)
   {
      m->sens = s;
      m->dt_heure=malloc(strlen(HM)+1);
      strncpy(m->dt_heure,HM,strlen(HM)+1);
      m->msg=malloc(strlen(msg1)+1);
      strncpy(m->msg,msg1,strlen(msg1)+1);
      return m;
   }
   return NULL;
}

void cmsg(int nc) //nc numéro du contact
{
   createMsg("04/04/2019","15:00","Salut");
}



//-------------------------------- <DELETE SHIIT>------------------
//---------------------------------------------------------------------
void deleteAll(compte *R)
{
   if (R == NULL)
   {
      return;
   }
   deleteAll(R->g);
   deleteAll(R->d);
   free(R);
}

// REMEMBER TO CALL THIS WITH RIGHT AND LEFT
void copie(compte *S)
{
    if (S)
    {
        addPerson(S,&Racine);
        copie(S->d);
        copie(S->g);
    }
}

// REMEMBER TO CALL COPIE AFTER THIS
compte *del = NULL;
void deleteOne(compte *S)
{
    if ((S->g == NULL) && (S->d == NULL))
    {
        free(S);
    }
    del = S;
    S->d = NULL;
    S->g = NULL;
}

void delete(compte *R)
{
   //delete all
   if (R == Racine)
   {
      deleteAll(R);
      retour = 2;
   }

}
//--------------------------------------------------------------------
//-------------------------------- </DELETE SHIIT>------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------- <SHIT ABT MENU>----------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

mElt* insertMenuItem(mElt *menu, int *n, char* t, void *p,void (*f)(),char a)
{
   mElt *newM= realloc(menu,sizeof(mElt) * ++(*n));//reallocer la structure pour un nouveau menu(est inistialiser les parametre de la structure mElt)
   (newM +((*n)-1))->titre=malloc(strlen(t)+1);
   strncpy((newM +((*n)-1))->titre,t,strlen(t)+1);
   if(a=='N')
   {
   	(newM +((*n)-1))->p=*(int*)p;
	}
   if(a == 'C')
   {
		(newM +((*n)-1))->t=(compte*)p;
	}
   if (a == 'Z')
   {
      (newM +((*n)-1))->t=(select*)p;
   }

   (newM +((*n)-1))->func=f;
   (newM +((*n)-1))->te=a;
   return newM;
}

mElt* createMenu(int page,int p)
{
   mElt *menu;  //liste dynamique
   int n=0,pa=2,q=0,ret=1;
   menu=malloc(sizeof(mElt));
   switch (page)
   {
      case 0:
      {
    		system("cls");
    		exit(EXIT_SUCCESS);
			break;
		}
      case 1:
      {
         menu=insertMenuItem(menu,&n,"1- Parametre",&pa,showParameters,'N');
         pa++;
         menu=insertMenuItem(menu,&n,"2- Afficher la selection",&pa,goTo,'N');
         pa++;
         menu=insertMenuItem(menu,&n,"3- Afficher les matches",&pa,goTo,'N');
         pa++;
         menu=insertMenuItem(menu,&n,"4- Rechercher",&pa, goTo,'N');
         pa++;
         menu=insertMenuItem(menu,&n,"5- Quitter",&q, goTo,'N');
         break;
      }
      case 2:
      {
         char *titreMenu;
         for (int i = 0; i < 5; i++)
         {
            titreMenu = (char*)malloc(120);
            snprintf(titreMenu,120,"04/04/2019","15:00","Salut");
            menu = insertMenuItem(menu,&n,titreMenu,0,goTo,'C');
         }


			break;
		}
        case 3:{
            system("cls");
            showSelection();
            break;
        }
        case 4:
        {
            myfct(Racine);
        	   pa=6;
        	   system("cls");
        	   select *temp = list_personne_selected;
        	   while(temp != NULL)
            {
        	   	menu=insertMenuItem(menu,&n,temp->nom_pre,temp,matchAction,'Z');
        	   	temp=temp->next;
				}
				menu=insertMenuItem(menu,&n,"Retour",&ret,goTo,'N');
            break;
        }
        case 5:{
		   menu=insertMenuItem(menu,&n,"1 - Nbre de personnes selectionnees",Racine,aff_nbr_selec,'C');
            menu=insertMenuItem(menu,&n,"2 - Nbre de personnes Acceptes",Racine,aff_acc,'C');
            menu=insertMenuItem(menu,&n,"3 - Nbre de personnes Rejetees",Racine,aff_rej,'C');
            menu=insertMenuItem(menu,&n,"4 - Nbre de personnes Compatible",Racine,aff_comp,'C');
            menu=insertMenuItem(menu,&n,"5 - Le plus jeune",Racine,aff_J,'C');
            menu=insertMenuItem(menu,&n,"6 - Le plus agée",Racine,aff_A,'C');
            menu=insertMenuItem(menu,&n,"7 - Le somme d'age",Racine,aff_sum,'C');
            menu=insertMenuItem(menu,&n,"8 - La distance Max",Racine,affdis_max,'C');
            menu=insertMenuItem(menu,&n,"9 - La distance Min",Racine,affdis_min,'C');
            menu=insertMenuItem(menu,&n,"10 - Recherche",Racine,aff_sea,'C');
            menu=insertMenuItem(menu,&n,"11 - Retour",&ret,goTo,'N');
            break;
		  }
		  case 6:{
		  	ret=4;
         menu=insertMenuItem(menu,&n,"1- afficher Conversation",Racine,showM,'N');
		  	menu=insertMenuItem(menu,&n,"2- Supprimer",Racine,supone,'N');
		  	menu=insertMenuItem(menu,&n,"3- Supprimer Tout",Racine,deleteAll,'C');
		  	menu=insertMenuItem(menu,&n,"4- Retour",&ret,goTo,'N');
			break;
		  }
    }
    return menu;
}

void showMenu(mElt *menu,int select){
   int size=_msize(menu)/sizeof(mElt);
   system("cls");
   int i=0;
   for(; i<size; i++){
      if (i == select -1) printf("-->");
      else printf("   ");
      printf("%s\n",(menu+i)->titre);
   }
}


void gererMenu(mElt *menu){
    int c,stop=0;
    int select=1;
    showMenu(menu,select);
    //224   HAUT = 72  BAS = 80  Enter = 13
    while(stop==0){
        c=getch();
        switch(c){
         case 13:{
            	if((menu+select-1)->te=='N'){
            		(menu+select-1)->func((menu+select-1)->p);
					}else{
						(menu+select-1)->func((menu+select-1)->t);
					}
            break;
         }
         case 224:{
            switch (getch()){
               case 80:{
                  if (select == _msize(menu)/sizeof(mElt)) select=1;
                  else select++;
                  break;
                  }
               case 72:{
                  if (select == 1) select=_msize(menu)/sizeof(mElt);
                  else select--;
                  break;
               }
            }
         }
      }
      showMenu(menu,select);
   }
   free(menu); //Lib�rer l'espace oqp par le menu
}

void goTo(int page){
   gererMenu(createMenu(page,0));
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------- </SHIT ABT MENU>----------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

int main() {
   setNewParam('F',0,90,0,1000);
   gererMenu(createMenu(1,0));
   return (EXIT_SUCCESS);
}
