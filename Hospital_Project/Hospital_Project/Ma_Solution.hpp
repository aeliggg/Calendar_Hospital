#pragma once
#ifndef MASOLUTION_HPP
#define MASOLUTION_HPP

#include "Instance.hpp"
#include "Solution.hpp"
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

class Ma_Solution
{
private:
    vector<vector<int>> v_v_IdShift_Par_Personne_et_Jour;

public:
    // Constructeurs et destructeur
    Ma_Solution();
    ~Ma_Solution();

    // Méthodes de génération de solution initiale
	void creation_Solution_Sans_Contrainte(Instance* inst);   // Création d'une solution initiale, respectant la contrainte d'avoir un shift assigné maximum par jour par personne
	void ajout_conges_personne(Instance* inst);               // Ajout des congés pour chaque personne 
	void suppression_jours_WE_de_trop(Instance* inst);        // Ajout de jours de week-end de repos
	void suppression_max_shifts_consecutifs(Instance* inst);  // Suppression des shifts consécutifs de trop 
	void suppression_shifts_par_type_de_trop(Instance* inst); // Remplacement par un autre shift ou par un jour de repos des shifts dépassant le nombre max de shift d'un certain type
	void Shift_succede(Instance* inst);                       // Faire en sorte que tous les shifts aient un succésseur valide
	void maximum_min_per_personne(Instance* inst);            // Retirer des shifts si une personne dépasse le maximum de minutes travaillées
	void ajout_jours_de_repos_consecutif(Instance* inst);     // Ajout de jours de repos consécutifs si une personne n'en a pas assez

    // Méthode principale
    vector<vector<int>> creation_Solution_Initiale(Instance* inst);

    // Métaheuristique
	void MetaHeuristique_Recherche_Local(Instance* inst);    // Application d'une métaheuristique de recherche locale pour améliorer la solution initiale
	bool Verifie_Neuf_Contraintes(Instance* inst, int ligne_a_verifier);  // Vérificateur 9 contraintes sur une ligne donnée (toutes sauf la contrainte sur le minimum de minutes travaillées)
	bool Verifie_Dix_Contraintes(Instance* inst, int ligne_a_verifier);   // Vérificateur des 10 contraintes sur une ligne donnée
	vector<int> Genere_Ligne_Voisine_Minimum_Min_Travaille(Instance* inst, int ligne_a_modifier); // Génération d'une ligne voisine jusqu'à avoir le minimum de minutes travaillées
	vector<int> Genere_Ligne_Voisine_Consecutifs_Shifts(Instance* inst, int ligne_a_modifier);    // Génération d'une ligne voisine en corrigeant les shifts consécutifs

    // Méthodes de vérification
	bool check_max_we(Instance* inst);                                   // Vérifie la contrainte du maximum de week-ends travaillés
	bool check_min_repos_consecutif(Instance* inst);                     // Vérifie la contrainte du minimum de jours de repos consécutifs
	bool check_max_shift_consecutif(Instance* inst);                     // Vérifie la contrainte du maximum de shifts consécutifs
	bool check_max_minutes_per_personne(Instance* inst);                 // Vérifie la contrainte du maximum de minutes travaillées par personne
	bool check_shift_succede(Instance* inst);                            // Vérifie la contrainte des shifts qui doivent être suivis par un shift spécifique
	bool check_conges(Instance* inst);                                   // Vérifie la contrainte des congés
	bool check_min_consecutif_shifts(Instance* inst);					 // Verifie la contrainte du minimum de shifts consécutifs
	bool check_min_minutes_travailees(Instance* inst);					 // Verifie la contrainte du minimum de minutes travaillées
	bool check_max_assignable_shifts(Instance* inst);					 // Verifie la contrainte du maximum de shifts assignables par planning par personne
	int check_solution(Instance* inst);									 // Vérifie le nombre de contraintes violées dans la solution

    // Affichage
	void afficher_solution();											 // Affiche la solution dans la console
    vector<vector<int>> get_Solution() { return v_v_IdShift_Par_Personne_et_Jour; } // Getter de la solution
};

#endif