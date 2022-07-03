   
/* declaration de fonctionnalites supplementaires */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define NB_CAR_MAX 256 /*taille du fichier que je declare*/

/*-------- Déclarations structures ---------*/

/* Tableau ou je range les caractere du fichier a compresser selon leurs frequences */
typedef struct elem_s
{
  unsigned char car;/* pour prendre des valeurs en ascii de 0 a 255*/
  float fre;
  int occur;
} elem;

/* Noeud d'un arbre  syntaxe de creation d'un noeud d'un arbre*/
typedef struct noeud_s
{
  struct noeud_s *gauche;
  struct noeud_s *droit;
  unsigned car; /* Caractère présent dans ce noeud/feuille */
  int code; /* Code que je donnerais au caractère dans ce noeud selon l'arbre de Huffman */
  int bits; /* Nombre de bits sur lequel le caractère est codé selon Huffman */
  float prob; /* Occurence du caractère, utile pour la création de l'arbre de Huffman */
} noeud_t;

/* Dictionnaire ou seront stocké les caractères du texte avec leurs nouveaux codages */
typedef struct dictionnaire
{
  int code_dec; /* Codage du caractère en decimal*/
  int bits; /* Nombres du bits utilisé par le caractère */
}dico_t;


/* --------- Déclarations fonctions------------- */

void compresser(FILE*,char*); /* Fonction principale de compressions*/
char* recupere( FILE *fic );
int catalogage(FILE*,elem tab[]);
void  tri_tableau(int ,elem tab[]); /* Tri le tableau de caracteres en fonction de leurs frequences*/
noeud_t* creer_noeud(float,int); /*Allocation de la mémoire pour un noeud*/
noeud_t* arbre_cons(noeud_t *, noeud_t *,float, int);/*Allocation de la mémoire pour la racine puis construction de l'arbre*/
void arbre_huffman(noeud_t **,int taille);/* Construction de l'arbre de Huffman d'apres un tableau de feuilles*/
void parcour_profondeur(noeud_t *,int, int ,dico_t dico[]); /* Parcour de l'arbre de Huffman afin de determiner le nouveau code de chaque lettre*/
void convertir_nombre(int,FILE*);/* Conversion du nouveau code en binaire, écriture dans un fichier temporaire*/
void ecriture_dico(int ,FILE*,dico_t dico[],elem tab[]); /* Ecriture complète du dictionnaire */
void trouver_caractere(int ,FILE*,FILE*); /* Pour chaque caractere du fichier a compresser, ecriture du nouveau code dans un fichier temporaire*/
void encodage_binaire(FILE*,FILE*,FILE*);/*Encodage du fichier d'origine en caracteres binaire dans un fichier temporaire*/
void finalisation_compression(FILE*,char*,FILE*);/*Ecriture du texte final compressé et du dictionnaire dans le fichier compressé*/

/* ----------Fonction principale--------------- */
int main()
{
  printf("\n********************************************\n");
  printf("************ Codage de Huffman *************\n");
  printf("********************************************\n");
  printf("\nQue souhaitez vous faire ?\n");
  printf("1] coder un fichier.txt\n2] Decoder un fichier.huff\n3] Quitter\n");
  printf("Votre choix : ");
  int choix;
  scanf("%d",&choix);
  while(choix>3 || choix<0)
    {
      printf("\nMauvais choix, recommencer !\n");
      printf("Votre choix : ");
      scanf("%d",&choix);
    }
  printf("\n");
if(choix==1){
    char nom_du_fichier[30];
    printf("Attention, ce fichier ne doit contenir AUCUN caratere accentue !\n");
    printf("Indiquer le nom du fichier a compresser: ");
    scanf("%s",nom_du_fichier);
    FILE* origine=NULL;/*je declare un pointeur sur un fichier appelle origine*/ 
    origine=fopen(nom_du_fichier,"r");/*j'ouvre en lecture seule*/
    while(origine==NULL) 
  {
	  printf("\nOuverture impossible:fichier inexistant\nIndiquer de nouveau le nom du fichier :");
	  scanf("%s",nom_du_fichier);/*ici on teste a chaque fois si le fichier est vide et redemande une indication pour le nouveau nom du fichier*/
	  origine=fopen(nom_du_fichier,"r");
	} 
  char *chain = (char*)malloc(sizeof(char)*1000);
  printf("notre fichier texte est le suivant:\n\n");
  chain = recupere(origine);
  printf("%s",chain);
	printf("\n\n");
    /*la fonction  int fseek permet d'effectuer un deplacement dans le fichier grace a une distance donne en entree et elle prend 
    en parametre un pointeur sur fichier,distance, et le repere qui est la position donne du fichier*/
    fseek(origine,0,SEEK_END);/*seek end permet indique la fin du fichier*/
    long longeur=ftell(origine);/*elle permet de retourner la position actuelle dans le fichier sous forme d'un long */
    fseek(origine,0,SEEK_SET);
    if(longeur!=0)
	  compresser(origine,nom_du_fichier);
    else
	  printf("Erreur, fichier vide ! Compression impossible !\n");
    }
else if(choix==2)
    {
      printf("******************************************************************************************\n");
      printf("Attention ! Partie non fonctionnelle car partiellement implemantee ! Veuillez nous excuser\n");
      printf("******************************************************************************************\n");
      char nom_du_huff[30];
      printf("Attention, ce fichier doit etre en .huff !\n");
      printf("saisisez le nom du fichier a decompresser: ");
      scanf("%s",nom_du_huff);
      FILE* origine=NULL;
      origine=fopen(nom_du_huff,"r+");/*j'ouvre le fichier mtn en mode lecture-ecriture*/
      while(origine==NULL){
        printf("\nOuverture impossible:fichier innexistant\nIndiquer de nouveau le nom du fichier :");
        scanf("%s",nom_du_huff);
        origine=fopen(nom_du_huff,"r");
	    }
      fseek(origine,0,SEEK_END);
      long longeur=ftell(origine);
      fseek(origine,0,SEEK_SET);/*seek_set indique la fin du fichier*/
      if(longeur==0){
	    printf("Erreur, fichier vide ! Compression impossible !\n");
	    return EXIT_SUCCESS;
	    }
      /*Décompression vers un fichier texte*/
      /*ecriture dans un fichier temporaire du code binaire du fichier a décompresser */
      int i,k;
      int caractere;
      FILE* fin;
      fin=fopen("texte_court.txt.huff","r+");/* je declare un autre fichier en lecture ecriture en .huff pour ensuite deconpresser
      ce fichier*/
      FILE* tmp_file=NULL;
      tmp_file=fopen("tmp_file.txt","w");/*j'ouvre un fichier en ecriture parce que a chaque fois je vais ecrire dedans */
      caractere=fgetc(fin);
      while(cryptage(fin)){
      for(i=0;i<(sizeof(char)*8);i++)
      fprintf(tmp_file,"%d",(caractere>>i)&1);/*ici  j'effectue directement les conversions en binaire (manipulation
      des bits)*/
      caractere=fgetc(fin);
      }
      fclose(tmp_file);
      tmp_file=fopen("tmp_file.txt","r+");
      convertir_nombre(caractere,tmp_file);
      long dico_position=ftell(fin);
      /*Convertir les séquences binaires avec les caracteres correspondant selon le dictionnaire, placé a dico_position*/
      rewind(tmp_file);/*pour nous ramener au debut du fichier*/ 
      FILE *fichier=NULL;
      fichier=fopen("fichier.txt","w");
      char code_bin[30];
      char code_dico[30];
      char cara_comp;
      char lettre;
      cara_comp=fgetc(tmp_file);
      while(cara_comp!=EOF){
        sprintf(code_bin,"%c",cara_comp);/*elle fonctionne de la meme maniere que la fonction printf a la difference qu'elle concatene les caracteres
        dans la chaine de destination*/ 
        fseek(fin,dico_position,SEEK_SET);
        lettre=fgetc(fin);
        caractere=fgetc(fin);
        while(caractere!=EOF){
          if(caractere!=48 || caractere!=49){ 
            caractere=fgetc(fin);
          }else{
          while(caractere==48 || caractere==49){
            sprintf(code_dico,"%c",caractere);
            caractere=fgetc(fin);
          }
          }
          if(strcmp(code_dico,code_bin)==0){/* la fonction strcmp permet de comparer deux chaine de caracteres*/
          fprintf(fichier,"%c",lettre);
          for(k=0;k<20;k++)
            code_bin[k]='\0';
          for(k=0;k<20;k++)
            code_dico[k]='\0';
          break;
          }else{
          for(k=0;k<20;k++)
          code_dico[k]='\0';
          }
          lettre=caractere;
          caractere=fgetc(fin);
        }
        cara_comp=fgetc(tmp_file);
	    }
      fclose(tmp_file);
      fclose(fin);
    }
      return EXIT_SUCCESS;
}

/* -------------- Définition des fonctions ---------------*/
/*permet de faire resortir tout les element du fichier dans un tableau*/
char* recupere( FILE *fic ){
	char *chain = (char*)malloc(sizeof(char)*1000) ;
	if(fic == NULL ) 
  exit(1);
	int i=0;
	while( (chain[i] = fgetc(fic)) != EOF ){
		i+=1;

	}
	//fclose(fic);
	return chain ;
}
/* Catalogage des caracteres et mise en mémoire de leurs occurences*/
int catalogage(FILE* origine,elem tab[]){
  int taille=0,i;
  int nbcar=0;
  int caractere;
  caractere=fgetc(origine);
  while(caractere!=EOF){
    if(caractere>=0 && caractere!='\0'){
	    nbcar++;
      for(i=0;i<=taille;i++){
        if(tab[i].fre==0){
        tab[i].car=caractere;
        tab[i].fre=1;
        taille++;
        break;
        }
        if(tab[i].car==caractere){
        tab[i].fre++;
        break;
        }
      }
	  }
      caractere=fgetc(origine);
  }  
  for(i=0;i<taille;i++)
    tab[i].fre=tab[i].fre/nbcar;
  return taille;
}

/* Tri du tableau en fonction de l'occurence des caracteres*/
void  tri_tableau(int taille,elem tab[]){
  int i,k;
  elem x;
  for(i=1;i<taille;i++){
      x=tab[i];
      k=i-1;
      while((k>=0)&&(x.fre<tab[k].fre)){
	      tab[k+1]=tab[k];
	      k--;
	    }
    tab[k+1]=x;
  }
}
/* Création et allocation de la mémoire d'un noeud*/
noeud_t* creer_noeud(float e,int car){
    noeud_t *n=(noeud_t*)malloc(sizeof(noeud_t));/*syntaxe de creation d'un noeud*/
    if(n==NULL){
      perror("erreur malloc");/*affiche un sms sur la sortie standart decrivant la derniere erreurs rencontres durant l'execution du programme*/
      exit(1);
    }
    n->prob=e;
    n->gauche=NULL;
    n->droit=NULL;
    n->car=car;
    return n;
}

/*Construction d'un arbre, allocation de la mémoire */
noeud_t* arbre_cons(noeud_t *a1, noeud_t *a2,float prob,int car){
    noeud_t* a=creer_noeud(prob,car);
    if(a1!=NULL)
      a->gauche=arbre_cons(a1->gauche,a1->droit,a1->prob,a1->car);
    if(a2!=NULL)
      a->droit=arbre_cons(a2->gauche,a2->droit,a2->prob,a2->car);
    return a;
}

/* Construction de l'arbre de Huffman */
void arbre_huffman(noeud_t** feuilles,int taille){ 
    int i,k;
    noeud_t* x;
    /* Tant que prob!=1 ou qu'il n'y a pas qu'un noeud : */
    if(taille!=1)
      {
        /*Construction d'un noeud a partir des deux feuilles a gauche, qu'on supprime */
        noeud_t *tmpnoeud=arbre_cons(feuilles[0],feuilles[1],feuilles[0]->prob+feuilles[1]->prob,-1);
        feuilles[0]=tmpnoeud;
        feuilles[1]=feuilles[taille-1];
        taille--;
        /* tri les nouveux noeuds selon prob */
        for(i=0;i<taille;i++){
	        x=feuilles[i];
	        k=i-1;
	          while((k>=0)&&(x->prob<feuilles[k]->prob)){
	            feuilles[k+1]=feuilles[k];
	            k--;
	          }
	          feuilles[k+1]=x;
	      }
        /*On recommence*/
        arbre_huffman(feuilles,taille);
      }
}

/* On décode l'arbre pour attribuer un nouveau code a chaque lettre (feuilles de l'arbre) */
void parcour_profondeur(noeud_t *arbre, int code, int nb_bits, dico_t dico[]){
  /* Si on tombe sur une feuille*/
    if(arbre->gauche==NULL && arbre->droit==NULL){
      arbre->code=code;
      arbre->bits=nb_bits;
      dico[arbre->car].code_dec=code;
      dico[arbre->car].bits=nb_bits;
      return;
    }
    if(arbre->gauche!=NULL)
      parcour_profondeur(arbre->gauche,(code<<1),nb_bits++,dico);
    if(arbre->droit!=NULL)
      parcour_profondeur(arbre->droit,(code<<1)+1,nb_bits++,dico);
}

/*Convertisseur d'un decimal en binaire, écriture dans un fichier*/
void convertir_nombre(int nombre,FILE* fichier){
  if(nombre==0)
  return;
  convertir_nombre(nombre/2,fichier);/*car ma fonction est recursive*/
  fprintf(fichier,"%d",nombre%2);
}

/* Ecriture du fichier dictionnaire*/
void ecriture_dico(int taille, FILE* dictionnaire,dico_t dico[],elem tab[]){
    int i;
    for(i=0;i<taille;i++){
      fprintf(dictionnaire,"%c",tab[i].car);
      if(dico[tab[i].car].code_dec==0){
        fprintf(dictionnaire,"00");
        dico[tab[i].car].bits=2;
	    }else if(dico[tab[i].car].code_dec==1){
        fprintf(dictionnaire,"0");
        dico[tab[i].car].bits=1;
	    }
      convertir_nombre(dico[tab[i].car].code_dec,dictionnaire);
    }
}
/* Trouver le code binaire correspondant a un caractere*/
void trouver_caractere(int caractere,FILE* dico,FILE* dest){
    if(caractere==EOF)
      return;
    int lettre;
    lettre=fgetc(dico);
    while(lettre!=EOF){
      if(lettre==caractere){
        lettre=fgetc(dico);
        while(lettre==48 || lettre==49){
          fprintf(dest,"%c",lettre);
          lettre=fgetc(dico);
        }
      }
        lettre=fgetc(dico);
    }
}
void encodage_binaire(FILE* dictionnaire, FILE* origine, FILE* compresse){
  int caractere;
  dictionnaire=fopen("dico.txt","r"); /* Ouverture pour lecture uniquement*/
  fseek(origine,0,SEEK_SET);
  caractere=fgetc(origine);
  while(caractere!=EOF){
      rewind(dictionnaire);
      trouver_caractere(caractere,dictionnaire,compresse);
      caractere=fgetc(origine);
  }
  fclose(compresse);
}
		      
int cryptage(FILE* fichier){
  long position=1;
  int car=fgetc(fichier);
    if(car=='d'){
	    position++;
	  car=fgetc(fichier);
	  if(car=='i'){
	    position++;
	    car=fgetc(fichier);
	  if(car=='c'){
		  position++;
		  car=fgetc(fichier);
		if(car=='o'){
		    return 0;
		}
	  }
	  }
    }
  fseek(fichier,-position,SEEK_CUR);
  return 1;
}
void finalisation_compression(FILE* compresse,char* nom_du_fichier,FILE* dictionnaire)
{
  int i;
  int caractere;
  int nb_bits=0;
  compresse=fopen("compresse.txt","r+");
  caractere=fgetc(compresse);
  while(caractere!=EOF){
      nb_bits++;
      caractere=fgetc(compresse);
  }
  while(nb_bits%8!=0){
      fprintf(compresse,"0");
      nb_bits++;
  }
  rewind(compresse);
  FILE* fin=NULL;
  strcat(nom_du_fichier,".huff");/*ajoute au fichier le .huff*/
  fin=fopen(nom_du_fichier,"w");
  fseek(compresse,0,SEEK_SET);
  caractere=fgetc(compresse);
  int new_code=0;
  while(caractere!=EOF){
    for(i=0;i<8;i++){
      if(i==0 && caractere==49)
        new_code=new_code+1;
      if(i==1 && caractere==49)
        new_code=new_code+2;
      if(i==2 && caractere==49)
        new_code=new_code+4;
      if(i==3 && caractere==49)
        new_code=new_code+8;
      if(i==4 && caractere==49)
        new_code=new_code+16;
      if(i==5 && caractere==49)
        new_code=new_code+32;
      if(i==6 && caractere==49)
        new_code=new_code+64;
      if(i==7 && caractere==49)
        new_code=new_code+128;
      caractere=fgetc(compresse);
	  }
      fprintf(fin,"%c",new_code);
      new_code=0;
  }
  fclose(fin);
  fclose(compresse);
  /* Copie du dictionnaire a la fin du fichier compressé*/
  rewind(dictionnaire);
  fin=fopen(nom_du_fichier,"r+");
  fseek(fin,0,SEEK_END);
  fprintf(fin,"dico");
  caractere=fgetc(dictionnaire);
  while(caractere!=EOF){
      fprintf(fin,"%c",caractere);
      caractere=fgetc(dictionnaire);
  }
  remove("compresse.txt");
  fclose(fin);
  fclose(dictionnaire);
  remove("dico.txt");
}
/* Fonction prinipal de compression */
void compresser(FILE* origine,char* nom_du_fichier){
  /* Variables utiles */
  dico_t dico[NB_CAR_MAX]={0,0,0};
  elem tab[NB_CAR_MAX]={0,0};
  int i,taille;
  FILE* dictionnaire=NULL;
  FILE* compresse=NULL;
  dictionnaire=fopen("dico.txt","w"); /* Ouverture du dictionnaire */
  compresse=fopen("compresse.txt","w"); /* Ouverture du fichier dans lequel le cryptage est écrit */
  if(dictionnaire==NULL) printf("Impossible d'ouvire le fichier dictionnaire.txt");
  if(compresse==NULL) printf("Impossible d'ouvire le fichier compresse.txt");

  taille=catalogage(origine,tab);/* Listage des caracters différents et de leurs occurences*/
  tri_tableau(taille,tab);/* Tri du tableau*/
  /*********************************************************************************************************************/
  noeud_t *noeud[taille];                       /***********************************************************************/
  for(i=taille-1;i>=0;i--)                      /*Créations de feuilles contenant un caractere et son occurence chacune*/
    noeud[i]=creer_noeud(tab[i].fre,tab[i].car);/************************************************************************/
  /*********************************************************************************************************************/
  arbre_huffman(noeud,taille);   /* Construction de l'arbre de Huffman */
  parcour_profondeur(noeud[0],0,0,dico);  /*Determination du nouveau code pour chaque caractere*/
  ecriture_dico(taille,dictionnaire,dico,tab);  /* Ecriture dans dico.txt du nouveau code de chque caractère en binaire */ 
  fclose(dictionnaire); /* On a fini d'écrire dans dico.txt*/
  encodage_binaire(dictionnaire,origine,compresse); /* Ecriture dans un nouveau fichier texte du nouveau code */
  finalisation_compression(compresse,nom_du_fichier,dictionnaire);/* Placement de bits supplémentaire pour avoir un nombre de d'octet entier, conversions des bits en caracteres: tous les 8 bits un caractere! Ecriture des lettres dans un fichier, ainsi que du dictionnaire a la fin*/
  printf("Fichier compressé sous le nom : %s \n",nom_du_fichier);
  fclose(origine);
}
