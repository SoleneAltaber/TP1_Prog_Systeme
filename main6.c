#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>


  
// fonction coupe chaine de caractere
char **cutstring(char *tab, const char *cara){
	char **chain = NULL;
	if(tab != NULL && cara != NULL){  	 // si on a bien une chaine de caract 
		unsigned int i;
		char *morco = NULL;				// pointeur sur les mots
		size_t size = 1;
		for(i=0; (morco = strtok(tab, cara)); i++){
			if(size <= i+1){
				void *tmp = NULL;
				size <<= 1;
				tmp = realloc(chain, sizeof(*chain) * size);	// on réaloue de la memoire tant
																//que la commande n'est pas entierement lu
				if(tmp != NULL){ 				// on a tout lu
					chain = tmp; 				// on a notre chaine de caractere séparé
				}
				else{  							// sinon erreur
					free(tab);  				// on vide la table
					chain = NULL;
					exit(EXIT_FAILURE);  
				}
			}
			chain[i] = morco; 	// strtok(tab, car) ca decoupe la chaine et la met dans morco
								//de caractere, et on met chaque morceau dans chaine[i]
			tab = NULL;
		}
		chain[i] = NULL;
	}
	return chain;
}




int main(void)
{
	// Variables
	
	char cmd[50]; 														//création du buffer de lecture (jusqu a 50 octets)
	int nbread; 														//création de la variable pour connaitre la taille des commande lu
	pid_t pid;
	int status;
	struct timespec ts1;
	struct timespec ts2;
	char *msg = "Bienvenue dans le Shell ENSEA.\n";
	
	//debut shell
	write(STDOUT_FILENO, msg, strlen(msg)); 
	msg = "Pour quitter, tapez 'exit'.\n";
	write(STDOUT_FILENO, msg, strlen(msg)); 
	msg = "enseash %";
	
	while(1){

		write(STDOUT_FILENO, msg, strlen(msg)); 
		
		nbread=read(STDIN_FILENO, cmd,50); 								//lecture de la commande tapé dans le terminal et écriture de la taille de celle-ci dans nbread
		cmd[nbread-1]='\0';	
		
 		char **cmdcut = cutstring(cmd, " ");							// on separe les element de la commande 

		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts1);					// on preleve le temps dans ts1
		
		if(strncmp(cmd,"exit",4)==0){ 									//compare les 4 premiers caractères de la commande avec exit
			msg= "Aurevoir à bientôt .\n";
			write(STDOUT_FILENO, msg, strlen(msg)); 
			exit(EXIT_SUCCESS); 														//on sort de enseash
		}
		else{
			pid = fork();
			if (pid!=0){   													//si le pid est différent de 0 éxecute le code du "père"
				wait(&status);    											// le "père attends que le fils s'éxecute	
			}
			else{ 														// si pid=0 on éxecute le code fils
				if(execvp(cmdcut[0], cmdcut) < 0){
					exit(EXIT_FAILURE);
				}
				unsigned int i = 0;
				for(i=0; cmdcut[i]!=NULL; i++){
					free(cmdcut[i]);
				}
			free(cmdcut);
			cmdcut = NULL;
		}
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts2);					// on preleve le temps dans ts2
		int deltaT=((ts2.tv_nsec)-(ts1.tv_nsec));						//on trouve le temps ecoulé
		if(WIFEXITED(status)){                                   		// si le fils se termine normalement      
              printf("error: %d | %d ns \n",WEXITSTATUS(status),deltaT);     
																		// on renvoie la valeur d'erreur de sortie du fils
              }
        else if(WIFSIGNALED(status)){
			printf("signal: %d | %d ns \n ",WTERMSIG(status),deltaT) ;																	// on renvoie la valeur du signal de sortie du fils
			  }
		}
	}
	return 0;
    
}


