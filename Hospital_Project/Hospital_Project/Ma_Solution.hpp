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
    void creation_Solution_Sans_Contrainte(Instance* inst);
    void ajout_conges_personne(Instance* inst);
    void suppression_jours_WE_de_trop(Instance* inst);
    void suppression_max_shifts_consecutifs(Instance* inst);
    void suppression_shifts_par_type_de_trop(Instance* inst);
    void Shift_succede(Instance* inst);
    void maximum_min_per_personne(Instance* inst);
    void ajout_jours_de_repos_consecutif(Instance* inst);

    // Méthode principale
    vector<vector<int>> creation_Solution_Initiale(Instance* inst);

    // Métaheuristique
    void MetaHeuristique_Recherche_Local(Instance* inst);
    vector<vector<int>> Genere_Voisin(Instance* inst);

    // Méthodes de vérification
    bool check_max_we(Instance* inst);
    bool check_min_repos_consecutif(Instance* inst);
    bool check_max_shift_consecutif(Instance* inst);
    bool check_max_minutes_per_personne(Instance* inst);
    bool check_shift_succede(Instance* inst);
    bool check_conges(Instance* inst);
    bool check_min_consecutif_shifts(Instance* inst);
    bool check_min_minutes_travailees(Instance* inst);
    bool check_max_assignable_shifts(Instance* inst);
    bool check_ligne_min_consecutif_shifts(Instance* inst, int ligne);
    bool check_ligne_min_minutes_travailees(Instance* inst, int ligne);
    int check_solution(Instance* inst);

    // Affichage
    void afficher_solution();
    vector<vector<int>> get_Solution() { return v_v_IdShift_Par_Personne_et_Jour; }
};

#endif