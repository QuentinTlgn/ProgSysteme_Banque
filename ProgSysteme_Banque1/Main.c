// ======================================================================= 
//   Programme :
// ----------------------------------------------------------------------- 
//     Ecrit le	:							par :
// ----------------------------------------------------------------------- 
//     Modifié le							par :
// ========================================================== data C ===== 
// ----------------------------------------------------------------------- 
//     Include                      
// ----------------------------------------------------------------------- 
#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <direct.h> // _getcwd

// ----------------------------------------------------------------------- 
//     define                      
// ----------------------------------------------------------------------- 
#define FICHIERMABANQUE "U:\\C\\ProgSysteme_Banque1\\MaBanque.txt"
// ----------------------------------------------------------------------- 
//     Structure                       
// ----------------------------------------------------------------------- 
struct ST_Compte {
	char NomClient[32];
	int  Solde;
	int	 NbreOperations;
};

struct ST_Operation {
	int MontantOperation;
	struct ST_Compte CompteClient;
};

// ----------------------------------------------------------------------- 
//     Espace de données Global accessible par tous les threads                    
// ----------------------------------------------------------------------- 


// ----------------------------------------------------------------------- 
//     Prototype des Fonctions 
// ----------------------------------------------------------------------- 

bool LectureCompte(struct ST_Compte*);
bool EcritureCompte(struct ST_Compte*);

void AfficherCompte(struct ST_Compte*);
int fileExists(const char*);


// ======================================================================= 
//     Main                      
// ======================================================================= 
int main()
{

	// ---- Fin du Processus maître
	getchar();

	ExitProcess(0);

	return 0;

}


// ======================================================================= 
//     Fonction Crediter                
// ======================================================================= 
void Crediter(struct ST_Operation* p_Credit)
{
}

// ======================================================================= 
//     Fonction Debiter                
// ======================================================================= 
void Debiter(struct ST_Operation* p_Debit)
{
}

// ======================================================================= 
//     Fonction Lecture Compte                 
// ======================================================================= 
bool LectureCompte(struct ST_Compte* p_Compte)
{
	FILE* pFichier;
	char* pLigne;
	char Buffer[256];
	bool bOK = false;
	char Nom[32];
	int	Montant, NbOperations;

	while (true) {
		pLigne = Buffer;

		// ---- Lecture du Fichier Banque
		fopen_s(&pFichier, FICHIERMABANQUE, "r");

		// ---- Lecture en erreur
		if (pFichier == NULL) {
			printf("\nErreur : Ouverture du Fichier %s", FICHIERMABANQUE);
			break;
		}
		// ---- Lecture du Fichier  : on lit que la premiere ligne
		if (fscanf_s(pFichier, "%s %d %d", Nom, _countof(Nom), (int*)&Montant, (int*)&NbOperations) > 0) {
			// ---- Client trouvé on met a jour les infos dans la structure passée en paramètres
			strcpy_s(p_Compte->NomClient, sizeof(p_Compte->NomClient), Nom);
			p_Compte->Solde = Montant;
			p_Compte->NbreOperations = NbOperations;
			bOK = true;
		}
		fclose(pFichier);
		break;
	}

	return(bOK);
}

// ======================================================================= 
//     Fonction Ecriture Compte dans Fichier                 
// ======================================================================= 
bool EcritureCompte(struct ST_Compte* p_Compte)
{
	FILE* pFichier;
	char* pLigne;
	char Buffer[256];
	bool bOK = false;

	while (true) {
		pLigne = Buffer;

		// ---- Open du Fichier MaBanque 
		while (true) {
			fopen_s(&pFichier, FICHIERMABANQUE, "w+");	// ouverture du fichier a vide si existant
			// on boucle si l'open ne s'est pas bien passé car le fopen n accepte pas le partage de fichier
			// donc on attend si il est occupé
			if (pFichier != NULL) break;
		}
		bOK = true;
		// ---- Ecriture du Fichier
		if (fprintf_s(pFichier, "%s %d %d", p_Compte->NomClient, p_Compte->Solde, p_Compte->NbreOperations) == 0) {
			printf("\nErreur : Ecriture dans Fichier %s", FICHIERMABANQUE);
			bOK = false;
		}
		fclose(pFichier);
		break;
	}

	return(bOK);
}
// ======================================================================= 
//     Fonction Afficher Compte
// ======================================================================= 
void AfficherCompte(struct ST_Compte* p_Compte)
{
	printf("\n ----------------------------------------------------------\n");
	printf("\n\tCLIENT : %s  -  SOLDE =  %d avec %d Operations \n", p_Compte->NomClient, p_Compte->Solde, p_Compte->NbreOperations);
	printf("\n ----------------------------------------------------------\n");
}

// ======================================================================= 
//     Fonction Test d'existence d'un fichier
// ======================================================================= 
int fileExists(const char* fileName)
{
	char* buffer;
	// ---- On recherche le Repertoire en cours et on l'affiche
	if ((buffer = _getcwd(NULL, 0)) == NULL) 						// _getcwd : Get the current working directory:
		perror("_getcwd error");
	else
	{
		printf(" Repertoire en cours %s \n\n", buffer);
		free(buffer);
	}

	return (0 <= (INT_PTR)GetFileAttributesA(fileName));
}
