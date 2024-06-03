// ======================================================================= 
//   Programme :
// ----------------------------------------------------------------------- 
//     Ecrit le	:							par :
// ----------------------------------------------------------------------- 
//     Modifie le							par :
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
#define FICHIERMABANQUE "U:\\C\\ProgSysteme_Banque\\MaBanque.txt"
#define PARALLELE
//#define LINEAIRE
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
//     Espace de donnees Global accessible par tous les threads                    
// ----------------------------------------------------------------------- 


// ----------------------------------------------------------------------- 
//     Prototype des Fonctions 
// ----------------------------------------------------------------------- 

bool LectureCompte(struct ST_Compte*);
bool EcritureCompte(struct ST_Compte*);

void AfficherCompte(struct ST_Compte*);
int fileExists(const char*);

DWORD WINAPI ThreadCrediter(LPVOID param);
DWORD WINAPI ThreadDebiter(LPVOID param);

// ======================================================================= 
//     Fonction Crediter                
// ======================================================================= 
void Crediter(struct ST_Operation* p_Credit)
{
	// Lire le compte du fichier
	struct ST_Compte compte = p_Credit->CompteClient;
	if (LectureCompte(&compte)) {
		// Ajouter le montant de l'operation au solde
		compte.Solde += p_Credit->MontantOperation;
		// Incrementer le nombre d'operations
		compte.NbreOperations += 1;
		// ecrire le compte mis à jour dans le fichier
		if (EcritureCompte(&compte)) {
			printf("Credit reussi. Nouveau solde: %d\n", compte.Solde);
		}
		else {
			printf("Erreur lors de l'ecriture du compte.\n");
		}
	}
	else {
		printf("Erreur lors de la lecture du compte.\n");
	}
}

// ======================================================================= 
//     Fonction Debiter                
// ======================================================================= 
void Debiter(struct ST_Operation* p_Debit)
{
	// Lire le compte du fichier
	struct ST_Compte compte = p_Debit->CompteClient;
	if (LectureCompte(&compte)) {
		// Verifier si le solde est suffisant pour debiter
		if (compte.Solde >= p_Debit->MontantOperation) {
			// Soustraire le montant de l'operation du solde
			compte.Solde -= p_Debit->MontantOperation;
			// Incrementer le nombre d'operations
			compte.NbreOperations += 1;
			// ecrire le compte mis à jour dans le fichier
			if (EcritureCompte(&compte)) {
				printf("Debit reussi. Nouveau solde: %d\n", compte.Solde);
			}
			else {
				printf("Erreur lors de l'ecriture du compte.\n");
			}
		}
		else {
			printf("Fonds insuffisants pour debiter.\n");
		}
	}
	else {
		printf("Erreur lors de la lecture du compte.\n");
	}
}

#ifdef PARALLELE
// ======================================================================= 
//     Fonctions PARALLELES                 
// ======================================================================= 

DWORD WINAPI ThreadCrediter(LPVOID param)
{
	struct ST_Operation* p_Credit = (struct ST_Operation*)param;
	Crediter(p_Credit);
	return 0;
}

DWORD WINAPI ThreadDebiter(LPVOID param)
{
	struct ST_Operation* p_Debit = (struct ST_Operation*)param;
	Debiter(p_Debit);
	return 0;
}
#endif // PARALLELE

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
			// ---- Client trouve on met a jour les infos dans la structure passee en parametres
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
			// on boucle si l'open ne s'est pas bien passe car le fopen n accepte pas le partage de fichier
			// donc on attend si il est occupe
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

// ======================================================================= 
//     Main                      
// ======================================================================= 
int main()
{
#ifdef LINEAIRE
	struct ST_Compte compte;
	struct ST_Operation operation;

	// Lecture initiale du compte pour afficher son etat
	if (LectureCompte(&compte)) {
		printf("etat initial du compte:\n");
		AfficherCompte(&compte);
	}
	else {
		printf("Erreur lors de la lecture initiale du compte.\n");
	}

	// Scenario de credit
	printf("Credit de 100 unites.\n");
	operation.MontantOperation = 100;
	Crediter(&operation);

	// Lecture du compte apres credit
	if (LectureCompte(&compte)) {
		printf("etat du compte apres credit:\n");
		AfficherCompte(&compte);
	}
	else {
		printf("Erreur lors de la lecture du compte apres credit.\n");
	}

	// Scenario de debit
	printf("Debit de 50 unites.\n");
	operation.MontantOperation = 50;
	Debiter(&operation);

	// Lecture du compte apres debit
	if (LectureCompte(&compte)) {
		printf("etat du compte apres debit:\n");
		AfficherCompte(&compte);
	}
	else {
		printf("Erreur lors de la lecture du compte apres debit.\n");
	}

	// Debit qui devrait echouer
	printf("Tentative de debit de 1000 unites (devrait echouer si fonds insuffisants).\n");
	operation.MontantOperation = 1000;
	Debiter(&operation);

	// Lecture du compte apres tentative de debit echouee
	if (LectureCompte(&compte)) {
		printf("etat du compte apres tentative de debit echouee:\n");
		AfficherCompte(&compte);
	}
	else {
		printf("Erreur lors de la lecture du compte apres tentative de debit echouee.\n");
	}

#endif // LINEAIRE

#ifdef PARALLELE
	struct ST_Compte compte;
	struct ST_Operation operationCredit, operationDebit;
	HANDLE hThreadCredit, hThreadDebit;
	DWORD dwThreadIdCredit, dwThreadIdDebit;

	// Initialisation des operations
	operationCredit.MontantOperation = 100; // Credit de 100 unites
	operationDebit.MontantOperation = 50;   // Debit de 50 unites

	// Lecture initiale du compte pour afficher son etat
	if (LectureCompte(&compte)) {
		printf("etat initial du compte:\n");
		AfficherCompte(&compte);
	}
	else {
		printf("Erreur lors de la lecture initiale du compte.\n");
	}

	// Creation des threads
	hThreadCredit = CreateThread(NULL, 0, ThreadCrediter, &operationCredit, 0, &dwThreadIdCredit);
	hThreadDebit = CreateThread(NULL, 0, ThreadDebiter, &operationDebit, 0, &dwThreadIdDebit);

	// Attente de la fin des threads
	WaitForSingleObject(hThreadCredit, INFINITE);
	WaitForSingleObject(hThreadDebit, INFINITE);

	// Fermeture des handles de threads
	CloseHandle(hThreadCredit);
	CloseHandle(hThreadDebit);

	// Lecture du compte apres les operations
	if (LectureCompte(&compte)) {
		printf("etat du compte apres les operations:\n");
		AfficherCompte(&compte);
	}
	else {
		printf("Erreur lors de la lecture du compte apres les operations.\n");
	}
#endif // PARALLELE

	// ---- Fin du Processus maître
	getchar();

	ExitProcess(0);

	return 0;

}