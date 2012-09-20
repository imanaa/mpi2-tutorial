/*######################################################################

 Exemple 10a : MPI::COMM_WORLD.Allgather

 Description:
  Exemple 10a identique a l'exemple 10 sauf que le meme tableau
  est utilise pour l'envoie et la reception. Cette facon de faire
  necessite moins de memoire.

            <-----------  buff ----------->
          ####################################
          #      #      #      #      #      #
        0 #  AA  #      #      #      #      #
          #      #      #      #      #      #
          ####################################
     T    #      #      #      #      #      #
        1 #  BB  #      #      #      #      #
     a    #      #      #      #      #      #
          ####################################
     c    #      #      #      #      #      #
        2 #  CC  #      #      #      #      #        AVANT
     h    #      #      #      #      #      #
          ####################################
     e    #      #      #      #      #      #
        3 #  DD  #      #      #      #      #
     s    #      #      #      #      #      #
          ####################################
          #      #      #      #      #      #
        4 #  EE  #      #      #      #      #
          #      #      #      #      #      #
          ####################################

            <------------ buff ----------->
          ####################################
          #      #      #      #      #      #
        0 #  AA  #  BB  #  CC  #  DD  #  EE  #
          #      #      #      #      #      #
          ####################################
     T    #      #      #      #      #      #
        1 #  AA  #  BB  #  CC  #  DD  #  EE  #
     a    #      #      #      #      #      #
          ####################################
     c    #      #      #      #      #      #
        2 #  AA  #  BB  #  CC  #  DD  #  EE  #       APRES
     h    #      #      #      #      #      #
          ####################################
     e    #      #      #      #      #      #
        3 #  AA  #  BB  #  CC  #  DD  #  EE  #
     s    #      #      #      #      #      #
          ####################################
          #      #      #      #      #      #
        4 #  AA  #  BB  #  CC  #  DD  #  EE  #
          #      #      #      #      #      #
          ####################################

 Auteur: Carol Gauthier
         Francis Jackson (traduction en C++)
         Centre de Calcul scientifique
         Universite de Sherbrooke

 Derniere revision: Mai 2005

######################################################################*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <mpi.h>

int main(int argc,char** argv)
{
   /*===============================================================*/
   /* Declaration des variables                                     */
   int          taskid, ntasks;
   MPI::Status  status;
   int          ierr,i,j,itask,jtask;
   int          buffsize;
   double       **buff,buffsum;
   double       inittime,totaltime;

   /*===============================================================*/
   /* Initialisation de MPI. Il est important de placer cet appel   */
   /* au debut du programme, immediatement apres les declarations   */
   MPI::Init(argc, argv);

   /*===============================================================*/
   /* Obtenir le nombre de taches MPI et le numero d'identification */
   /* de la tache courante taskid                                   */
   taskid = MPI::COMM_WORLD.Get_rank();
   ntasks = MPI::COMM_WORLD.Get_size();

   /*===============================================================*/
   /* Obtenir buffsize a partir des arguments.                      */
   buffsize=atoi(argv[1]);

   /*===============================================================*/
   /* Affichage de la description de l'exemple.                     */
   if ( taskid == 0 ){
     printf("\n\n\n");
     printf("##########################################################\n\n");
     printf(" Exemple 10 \n\n");
     printf(" Communication collective : MPI::COMM_WORLD.Allgather \n\n");
     printf(" Dimension de chaque vecteur: %d\n",buffsize);
     printf(" Nombre total de taches: %d\n\n",ntasks);
     printf("##########################################################\n\n");
     printf("                --> AVANT COMMUNICATION <--\n");
   }

   /*=============================================================*/
   /* Allocation de la memoire.                                   */
   buff = new double*[ntasks];
   buff[0] = new double[ntasks*buffsize];
   for(i=1;i<ntasks;i++)buff[i]=buff[i-1]+buffsize;

   /*=============================================================*/
   /* Initialisation du/des vecteurs et/ou tableaux               */
   srand((unsigned)time( NULL ) + taskid);
   for(i=0;i<buffsize;i++){
       buff[0][i]=(double)rand()/RAND_MAX;
   }

   /*==============================================================*/
   /* Affichage avant communication.                               */

   if( taskid == 0 )
     printf("\n");
   MPI::COMM_WORLD.Barrier();

   buffsum=0.0;
   for(i=0;i<buffsize;i++){
     buffsum=buffsum+buff[0][i];
   }
   printf("Tache %d : Somme du vecteur = %e \n",taskid,buffsum);

   /*===============================================================*/
   /* Communication.                                                */

   inittime = MPI::Wtime();

   MPI::COMM_WORLD.Allgather(buff[0],buffsize,MPI::DOUBLE,
                             buff[0],buffsize,MPI::DOUBLE);

   totaltime = MPI::Wtime() - inittime;

   /*===============================================================*/
   /* Affichage apres communication.                                */

   if ( taskid == 0 ){
     printf("\n");
     printf("##########################################################\n\n");
     printf("                --> APRES COMMUNICATION <-- \n");
   }

   for(jtask=0;jtask<ntasks;jtask++){
     MPI::COMM_WORLD.Barrier();
     if ( taskid == jtask ){
       printf("\n");
       for(itask=0;itask<ntasks;itask++){
         buffsum=0.0;
         for(i=0;i<buffsize;i++){
           buffsum=buffsum+buff[itask][i];
         }
         printf("Tache %d : Somme du vecteur recu par %d -> %e \n",
               taskid,itask,buffsum);
       }
     }
   }

   MPI::COMM_WORLD.Barrier();

   if(taskid==0){
     printf("\n");
     printf("##########################################################\n\n");
     printf(" Temps total de communication : %f secondes\n\n",totaltime);
     printf("##########################################################\n\n");
   }

   /*===============================================================*/
   /* Liberation de la memoire                                      */
     delete [] buff[0];
     delete [] buff;

   /*===============================================================*/
   /* Finalisation de MPI                                           */
   MPI::Finalize();

}


