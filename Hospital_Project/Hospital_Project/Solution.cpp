#include "Solution.hpp"
#include <iostream>
#include <set>

Solution::Solution()
{
    i_valeur_fonction_objectif = 0;
}
Solution::~Solution()
{

}

bool Solution::Verification_Solution(Instance* instance)
{
    bool b_solution_ok = true;
    bool b_test_si_premier_jour_off = false;
    int i_fc_obj, i, j, jj, k, i_duree_travail, i_shift_consecutif, i_nb_WE;
    vector<vector<int>> v_i_nb_personne_par_Shift_et_jour(instance->get_Nombre_Shift(), vector<int>(instance->get_Nombre_Jour(), 0));

    //Vérification de la taille de v_v_IdShift_Par_Personne_et_Jour
    if (v_v_IdShift_Par_Personne_et_Jour.size() != instance->get_Nombre_Personne())
    {
        cout << "Erreur : v_v_IdShift_Par_Personne_et_Jour n'est pas de la bonne taille en nombre de personnes." << endl;
        b_solution_ok = false;
    }
    for (i = 0; i < v_v_IdShift_Par_Personne_et_Jour.size(); i++)
    {
        vector<int> v_i_Nb_shift(instance->get_Nombre_Shift(), 0);
        i_duree_travail = 0;
        i_shift_consecutif = 0;
        i_nb_WE = 0;
        if (v_v_IdShift_Par_Personne_et_Jour[i].size() != instance->get_Nombre_Jour())
        {
            cout << "Erreur : v_v_IdShift_Par_Personne_et_Jour n'est pas de la bonne taille en nombre de jour pour la personne " << i << "." << endl;
            b_solution_ok = false;
        }
        //Vérification que v_v_IdShift_Par_Personne_et_Jour contient que des Ids Shifts valident ou -1
        for (j = 0; j < v_v_IdShift_Par_Personne_et_Jour[i].size(); j++)
        {
            if ((v_v_IdShift_Par_Personne_et_Jour[i][j] != -1) && ((v_v_IdShift_Par_Personne_et_Jour[i][j] < 0) || (v_v_IdShift_Par_Personne_et_Jour[i][j] > instance->get_Nombre_Shift())))
            {
                cout << "Erreur : v_v_IdShift_Par_Personne_et_Jour[" << i << "][" << j << "] ne contient pas un shift valide." << endl;
                b_solution_ok = false;
            }
            else
            {
                if (v_v_IdShift_Par_Personne_et_Jour[i][j] != -1)
                {
                    v_i_Nb_shift[v_v_IdShift_Par_Personne_et_Jour[i][j]] = v_i_Nb_shift[v_v_IdShift_Par_Personne_et_Jour[i][j]] + 1;
                    v_i_nb_personne_par_Shift_et_jour[v_v_IdShift_Par_Personne_et_Jour[i][j]][j] = v_i_nb_personne_par_Shift_et_jour[v_v_IdShift_Par_Personne_et_Jour[i][j]][j] + 1;
                    i_duree_travail = i_duree_travail + instance->get_Shift_Duree(v_v_IdShift_Par_Personne_et_Jour[i][j]);
                    i_shift_consecutif++;
                    if ((j % 7) == 5)
                        i_nb_WE++;
                    if (((j % 7) == 6) && (v_v_IdShift_Par_Personne_et_Jour[i][j - 1] == -1))
                        i_nb_WE++;
                    //Vérification du nombre de shifts consécutifs maximum assignable à chaque personne
                    if (i_shift_consecutif > instance->get_Personne_Nbre_Shift_Consecutif_Max(i))
                    {
                        cout << "Erreur : Le nombre max de shift consécutif de la personne " << i << " a été dépassé." << endl;
                        b_solution_ok = false;
                    }

                    //Vérification des jours de congés de chaque personne
                    if (!instance->is_Available_Personne_Jour(i, j))
                    {
                        {
                            cout << "Erreur : Jour de congé " << j << " de la personne " << i << " non respecté." << endl;
                            b_solution_ok = false;
                        }
                    }
                }
                else
                {
                    //Vérification du nombre de shifts consécutifs minimum assignable à chaque personne
                    if ((i_shift_consecutif < instance->get_Personne_Nbre_Shift_Consecutif_Min(i)) && (i_shift_consecutif != 0) && ((j - instance->get_Personne_Nbre_Shift_Consecutif_Min(i)) > 0))
                    {
                        cout << "Erreur : Le nombre min de shift consécutif de la personne " << i << " n'a pas été dépassé." << endl;
                        b_solution_ok = false;
                    }
                    i_shift_consecutif = 0;
                    b_test_si_premier_jour_off = false;
                    if (j != 0)
                    {
                        if (v_v_IdShift_Par_Personne_et_Jour[i][j - 1] != -1)
                            b_test_si_premier_jour_off = true;
                    }
                    if (b_test_si_premier_jour_off)
                    {
                        b_test_si_premier_jour_off = false;
                        for (jj = j; jj < (j + instance->get_Personne_Jour_OFF_Consecutif_Min(i)); jj++)
                        {
                            if (jj < v_v_IdShift_Par_Personne_et_Jour[i].size())
                                if (v_v_IdShift_Par_Personne_et_Jour[i][jj] != -1)
                                    b_test_si_premier_jour_off = true;
                        }
                        if (b_test_si_premier_jour_off)
                        {
                            cout << "Erreur : Le nombre de jours OFF consécutifs minimum de la personne " << i << " n'a pas été respecté." << endl;
                            b_solution_ok = false;
                        }
                    }
                }
                //Vérification des successions des Shifts
                if (j != (v_v_IdShift_Par_Personne_et_Jour[i].size() - 1))
                {
                    if ((v_v_IdShift_Par_Personne_et_Jour[i][j] != -1) && (v_v_IdShift_Par_Personne_et_Jour[i][j + 1] != -1))
                    {
                        if (!instance->is_possible_Shift_Succede(v_v_IdShift_Par_Personne_et_Jour[i][j], v_v_IdShift_Par_Personne_et_Jour[i][j + 1]))
                        {
                            cout << "Erreur : Deux shifts se succèdent alors qu'il ne devrait pas, pour la personne " << i << "le jour" << j << " et " << j + 1 << "." << endl;
                            b_solution_ok = false;
                        }
                    }
                }
            }
        }
        //Vérification de la durée totale maximale et minimale de chaque personne
        if ((i_duree_travail > instance->get_Personne_Duree_total_Max(i)) || (i_duree_travail < instance->get_Personne_Duree_total_Min(i)))
        {
            cout << "Erreur : La durée totale de travail de la personne " << i << " ne respect pas sa valeur max ou min." << endl;
            b_solution_ok = false;
        }
        //Vérification du nombre de WE (samedi ou/et dimanche) de travail maximal pour chaque personne
        if (i_nb_WE > instance->get_Personne_Nbre_WE_Max(i))
        {
            cout << "Erreur : Le nombre max de travail le WE de la personne " << i << " n'est pas respecté." << endl;
            b_solution_ok = false;
        }
        //Vérification du nombre maximal de shifts de chaque personne
        for (j = 0; j < instance->get_Nombre_Shift(); j++)
        {
            if (v_i_Nb_shift[j] > instance->get_Personne_Shift_Nbre_Max(i, j))
            {
                cout << "Erreur : Le nombre max de shift " << j << " de la personne " << i << " n'est pas respecté." << endl;
                b_solution_ok = false;
            }
        }
    }
    //Vérification de la fonction objectif
    i_fc_obj = 0;
    for (i = 0; i < v_v_IdShift_Par_Personne_et_Jour.size(); i++)
    {
        for (j = 0; j < v_v_IdShift_Par_Personne_et_Jour[i].size(); j++)
        {
            if (v_v_IdShift_Par_Personne_et_Jour[i][j] != -1)
            {
                i_fc_obj = i_fc_obj + instance->get_Poids_Refus_Pers_Jour_Shift(i, j, v_v_IdShift_Par_Personne_et_Jour[i][j]);
            }
            for (k = 0; k < instance->get_Nombre_Shift(); k++)
            {
                if (v_v_IdShift_Par_Personne_et_Jour[i][j] != k)
                    i_fc_obj = i_fc_obj + instance->get_Poids_Affectation_Pers_Jour_Shift(i, j, k);
            }
        }
    }
    for (k = 0; k < instance->get_Nombre_Shift(); k++)
    {
        for (j = 0; j < instance->get_Nombre_Jour(); j++)
        {
            if (v_i_nb_personne_par_Shift_et_jour[k][j] < instance->get_Nbre_Personne_Requis_Jour_Shift(j, k))
                i_fc_obj = i_fc_obj + instance->get_Poids_Personne_En_Moins_Jour_Shift(j, k) * (instance->get_Nbre_Personne_Requis_Jour_Shift(j, k) - v_i_nb_personne_par_Shift_et_jour[k][j]);
            if (v_i_nb_personne_par_Shift_et_jour[k][j] > instance->get_Nbre_Personne_Requis_Jour_Shift(j, k))
                i_fc_obj = i_fc_obj + instance->get_Poids_Personne_En_Plus_Jour_Shift(j, k) * (v_i_nb_personne_par_Shift_et_jour[k][j] - instance->get_Nbre_Personne_Requis_Jour_Shift(j, k));
        }
    }
    if (i_fc_obj != i_valeur_fonction_objectif)
    {
        cout << "Erreur : la fonction objectif est mal calculée, cela devrait être : " << i_fc_obj << "." << endl;
        b_solution_ok = false;
    }

    return b_solution_ok;
}

void Solution::creation_Solution_Sans_Contrainte(Instance inst) {
    int iShiftAFaire = 0;
    for (int iIndexPersonne = 0; iIndexPersonne < inst.get_Nombre_Personne(); iIndexPersonne++) {
        for (int iIndexJour = 0; iIndexJour < inst.get_Nombre_Jour(); iIndexJour++) {
            iShiftAFaire = rand() % inst.get_Nombre_Shift();
            v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour] = iShiftAFaire;
        }
    }
}

void Solution::ajout_conges_personne(Instance inst) {
    for (int p = 0; p < inst.get_Nombre_Personne(); p++) {
        for (int j = 0; j < inst.get_Personne(p).v_Id_Jour_Conges.size(); j++) {
            int day_off = inst.get_Personne(p).v_Id_Jour_Conges[j];
            v_v_IdShift_Par_Personne_et_Jour[p][day_off] = -1;
        }
    }
}

void Solution::suppression_jours_WE_de_trop(Instance inst) {
    int i_Nb_WE_Plage_Horaire = inst.get_Nombre_Jour() / 7;
    for (int p = 0; p < inst.get_Nombre_Personne(); p++) {
        int i_Nb_WE_A_Travailler = inst.get_Personne_Nbre_WE_Max(p);

        vector<int> i_Indices_WE_A_Travailler = {};
        for (int i_Boucle = 0; i_Boucle < i_Nb_WE_A_Travailler; i_Boucle++) {
            int i_WE = rand() % i_Nb_WE_Plage_Horaire;
            while (std::find(i_Indices_WE_A_Travailler.begin(), i_Indices_WE_A_Travailler.end(), i_WE) != i_Indices_WE_A_Travailler.end()) {
                i_WE = rand() % i_Nb_WE_Plage_Horaire;
            }
            i_Indices_WE_A_Travailler.push_back(i_WE);
        }
        /*
        printf("Personne %d : WE à travailler :\n", p);
        for (int i = 0; i < i_Indices_WE_A_Travailler.size(); i++) {
            printf("%d ", i_Indices_WE_A_Travailler[i]);
        }
        printf("\n");
        */

        int compteur_WE = 0;
        for (int j = 0; j < inst.get_Nombre_Jour(); j++) {
            if (j % 7 == 5) {
                if (std::find(i_Indices_WE_A_Travailler.begin(), i_Indices_WE_A_Travailler.end(), compteur_WE) == i_Indices_WE_A_Travailler.end()) {
                    v_v_IdShift_Par_Personne_et_Jour[p][j] = -1;
                    v_v_IdShift_Par_Personne_et_Jour[p][j + 1] = -1;
                }
                compteur_WE++;
            }
        }
    }
}

void Solution::suppression_max_shifts_consecutifs(Instance inst) {
    for (int p = 0; p < inst.get_Nombre_Personne(); p++) {
        int compteur_consecutif = 0;
        for (int j = 0; j < inst.get_Nombre_Jour(); j++) {
            if (v_v_IdShift_Par_Personne_et_Jour[p][j] != -1) {
                compteur_consecutif++;
                if (compteur_consecutif > inst.get_Personne(p).i_Nbre_Shift_Consecutif_Max) {
                    v_v_IdShift_Par_Personne_et_Jour[p][j] = -1;
                    compteur_consecutif = 0;
                }
            }
        }
    }
}


void Solution::suppression_shifts_par_type_de_trop(Instance inst) {
    for (int p = 0; p < inst.get_Nombre_Personne(); p++) {
        vector<int> compteur_shifts = {};
        for (int i = 0; i < inst.get_Nombre_Shift(); i++) {
            compteur_shifts.push_back(0);
        }

        for (int j = 0; j < inst.get_Nombre_Jour(); j++) {
            int shift_actuel = v_v_IdShift_Par_Personne_et_Jour[p][j];
            if (shift_actuel != -1) {
                compteur_shifts[shift_actuel]++;
                if (compteur_shifts[shift_actuel] > inst.get_Personne(p).v_Nbre_Max_Chaque_Shift[v_v_IdShift_Par_Personne_et_Jour[p][j]]) {
                    int shift_aleatoire = rand() % inst.get_Nombre_Shift();
                    int i_compteur_boucle = 0;
                    while (shift_aleatoire == shift_actuel || compteur_shifts[shift_aleatoire] >= inst.get_Personne(p).v_Nbre_Max_Chaque_Shift[shift_aleatoire]) {
                        shift_aleatoire = rand() % inst.get_Nombre_Shift();
                        i_compteur_boucle++;
                        if (i_compteur_boucle > 1000) {
                            shift_aleatoire = -1;
                            break;
                        }
                    }
                    v_v_IdShift_Par_Personne_et_Jour[p][j] = shift_aleatoire;
                    compteur_shifts[shift_aleatoire]++;
                    compteur_shifts[shift_actuel]--;
                }
            }
        }
    }
}


void Solution::Shift_succede(Instance inst) {
    size_t iNbPersonne = inst.get_Nombre_Personne();
    size_t iNbJour = inst.get_Nombre_Jour();
    for (int iIndexPersonne = 0; iIndexPersonne < iNbPersonne; iIndexPersonne++) {
        for (int iIndexJour = 1; iIndexJour < iNbJour; iIndexJour++) {
            int prev = v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour - 1];
            int curr = v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour];
            if (prev == -1 || curr == -1 || inst.is_possible_Shift_Succede(prev,curr)) {
                continue;
            }
			curr = 0;
			int compteur = std::count(v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne].begin(), v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne].end(), curr);
            while ((inst.is_possible_Shift_Succede(prev, curr) == false || compteur >= inst.get_Personne_Shift_Nbre_Max(iIndexPersonne,curr))&& curr < inst.get_Nombre_Shift()-1 ) {
                curr++;
				compteur = std::count(v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne].begin(), v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne].end(), curr);              
            }

            if (curr == inst.get_Nombre_Shift() -1 && (inst.is_possible_Shift_Succede(prev, curr) == false || compteur >= inst.get_Personne_Shift_Nbre_Max(iIndexPersonne, curr))) {
                curr = -1; 
			}

            v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour] = curr;
        }
    }
}


void Solution::afficher_solution()
{
    cout << "\n=== Solution générée ===\n";

    for (size_t p = 0; p < v_v_IdShift_Par_Personne_et_Jour.size(); p++)
    {
        cout << "Personne " << p << " : ";
        for (size_t j = 0; j < v_v_IdShift_Par_Personne_et_Jour[p].size(); j++)
        {
            cout << v_v_IdShift_Par_Personne_et_Jour[p][j] << " ";
        }
        cout << endl;
    }
}

void Solution::maximum_min_per_personne(Instance inst) {
    size_t iNbPersonne = inst.get_Nombre_Personne();
    size_t iNbJour = inst.get_Nombre_Jour();
    for (int iIndexPersonne = 0; iIndexPersonne < iNbPersonne; iIndexPersonne++) {
        int duree_travail = 0;
        int shift_aleatoire;
        int duree_shift_actuel = 0;
        int compteur_boucle = 0;
        for (int iIndexJour = 0; iIndexJour < iNbJour; iIndexJour++) {
            int shift_actuel = v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour];
            if (shift_actuel != -1) {
                duree_travail += inst.get_Shift_Duree(shift_actuel);
            }
        }

        while (duree_travail > inst.get_Personne_Duree_total_Max(iIndexPersonne)) {
            shift_aleatoire = rand() % inst.get_Nombre_Jour();
            while (v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][shift_aleatoire] == -1) {
                compteur_boucle++;
                shift_aleatoire = rand() % inst.get_Nombre_Jour();
                if (compteur_boucle > 1000) {
                    cout << "Erreur : pas de solution possible avec les contraintes de durée maximale de travail par personne." << endl;
                    return;
                }
            }
            duree_shift_actuel = inst.get_Shift_Duree(v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][shift_aleatoire]);
            v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][shift_aleatoire] = -1;
            duree_travail -= duree_shift_actuel;
        }
    }
}

void Solution::ajout_jours_de_repos_consecutif(Instance inst) {
	size_t iNbPersonne = inst.get_Nombre_Personne();
	size_t iNbJour = inst.get_Nombre_Jour();
	for (int iIndexPersonne = 0; iIndexPersonne < iNbPersonne; iIndexPersonne++) {
		int min_repos = inst.get_Personne_Jour_OFF_Consecutif_Min(iIndexPersonne);
		for (int iIndexJour = 0; iIndexJour < iNbJour; iIndexJour++) {
			if (v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour] == -1) {
				int compteur_repos = 1;
                int j = iIndexJour + 1;
				while (j < iNbJour && v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][j] == -1) {
                    compteur_repos += 1;
                    j++;
				}
				if (compteur_repos < min_repos) {
					int i_reste_a_mettre = min_repos - compteur_repos;
					//Si on peut mettre tous les jours off avant ou après le jour off actuel
                    if (0 <= iIndexJour - i_reste_a_mettre && iIndexJour + i_reste_a_mettre <= iNbJour - 1) {
						int i_indiceDebut = iIndexJour - i_reste_a_mettre;
						int i_indiceFin = iIndexJour + i_reste_a_mettre;
                        int i_countDayOffBefore = std::count(v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne].begin() + i_indiceDebut, v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne].begin() + iIndexJour, -1);
                        int i_countDayOffAfter = std::count(v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne].begin() + iIndexJour, v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne].begin() + i_indiceFin, -1);
                        //Si on peut créer un "pont" avant le jour off actuel
                        if (i_countDayOffBefore == 0 && i_indiceDebut > 0 && v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][i_indiceDebut-1] == -1) {
                            for(int k = i_indiceDebut; k < iIndexJour; k++) {
                                v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][k] = -1;
                                iIndexJour = i_indiceDebut;
							}
                        }
						//Si on peut créer un "pont" après le jour off actuel
                        else if (i_countDayOffAfter == 0 && i_indiceFin < iNbJour-1  && v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][i_indiceFin+1] == -1) {
                            for (int k = iIndexJour + 1; k <= i_indiceFin; k++) {
                                v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][k] = -1;
                            }
                        }
                        //Sinon, mettre les jours off avant ou après selon la disponibilité
                        else {
                            int i_jours_off_mis = 0;
                            int k = i_indiceDebut;
                            while (i_jours_off_mis < i_reste_a_mettre && k < iNbJour) {
                                if (v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][k] != -1) {
                                    v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][k] = -1;
                                    i_jours_off_mis++;
                                }
                                k++;
                            }
						}
                    }
                    //Sinon si on ne peut mettre les jours off qu'avant le jour off actuel
					else if (0 <= iIndexJour - i_reste_a_mettre) {
                        int i_indiceDebut = iIndexJour - i_reste_a_mettre;
                        for(int k = i_indiceDebut; k < iIndexJour; k++) {
                            v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][k] = -1;
							iIndexJour = i_indiceDebut;
                        }
                    }
                    //Sinon si on ne peut mettre les jours off qu'après le jour off actuel
                    else if (iIndexJour + i_reste_a_mettre <= iNbJour - 1) {
                        int i_indiceFin = iIndexJour + i_reste_a_mettre;
                        for (int k = iIndexJour + 1; k <= i_indiceFin; k++) {
                            v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][k] = -1;

                        }
                    }
				}
				iIndexJour += min_repos - 1;
			}
		}
	}
}

bool Solution::check_max_we(Instance inst) {
    size_t iNbPersonne = inst.get_Nombre_Personne();
    size_t iNbJour = inst.get_Nombre_Jour();
    for (int iIndexPersonne = 0; iIndexPersonne < iNbPersonne; iIndexPersonne++) {
		int max_we = inst.get_Personne_Nbre_WE_Max(iIndexPersonne);
        int compteur_we = 0;
        for (int iIndexJour = 0; iIndexJour < iNbJour; iIndexJour++) {
			if (iIndexJour % 7 == 5) {
				if (v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour] != -1 || v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour + 1] != -1) {
					compteur_we += 1;
				}
				if (compteur_we > max_we) {
					return false;
				}
			}
        }
    }
	return true;
}

bool Solution::check_min_repos_consecutif(Instance inst) {
    size_t iNbPersonne = inst.get_Nombre_Personne();
    size_t iNbJour = inst.get_Nombre_Jour();
    for (int iIndexPersonne = 0; iIndexPersonne < iNbPersonne; iIndexPersonne++) {
        int min_repos = inst.get_Personne_Jour_OFF_Consecutif_Min(iIndexPersonne);
        int compteur_repos = 0;
        for (int iIndexJour = 0; iIndexJour < iNbJour; iIndexJour++) {
            if (v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour] == -1) {
                compteur_repos += 1;
            }
			else if (v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour] != -1) {
				if (compteur_repos > 0 && compteur_repos < min_repos) {
					return false;
				}
				compteur_repos = 0;
			}

        }
    }
	return true;
}

bool Solution::check_max_shift_consecutif(Instance inst) {
    size_t iNbPersonne = inst.get_Nombre_Personne();
    size_t iNbJour = inst.get_Nombre_Jour();
    for (int iIndexPersonne = 0; iIndexPersonne < iNbPersonne; iIndexPersonne++) {
        int compteur_consecutif = 0;
		int max_shift_consecutif = inst.get_Personne_Nbre_Shift_Consecutif_Max(iIndexPersonne);
        for (int iIndexJour = 0; iIndexJour < iNbJour; iIndexJour++) {
            if (v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour] != -1) {
				compteur_consecutif += 1;
            }
            else if (v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour] == -1) {
                compteur_consecutif = 0;
            }
            if (compteur_consecutif > max_shift_consecutif) {
                return false;
            }
        }
    }
	return true;
}

bool Solution::check_max_minutes_per_personne(Instance inst) {
    size_t iNbPersonne = inst.get_Nombre_Personne();
    size_t iNbJour = inst.get_Nombre_Jour();
    for (int iIndexPersonne = 0; iIndexPersonne < iNbPersonne; iIndexPersonne++) {
        int compteur_minute = 0;
        int nbMaxMinutes = inst.get_Personne_Duree_total_Max(iIndexPersonne);
        for (int iIndexJour = 0; iIndexJour < iNbJour; iIndexJour++) {
            if (v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour] != -1) {
                compteur_minute += inst.get_Shift_Duree(v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour]);
            }
            if (compteur_minute > nbMaxMinutes) {
                cout << "Personne " << iIndexPersonne << " compteur_minute : " << compteur_minute << " maximum : " << nbMaxMinutes << "\n";
                return false;
            }
        }
    }
	return true;
}

bool Solution::check_shift_succede(Instance inst) {
	size_t iNbPersonne = inst.get_Nombre_Personne();
	size_t iNbJour = inst.get_Nombre_Jour();
    for (int iIndexPersonne = 0; iIndexPersonne < iNbPersonne; iIndexPersonne++) {
		for (int iIndexJour = 1; iIndexJour < iNbJour; iIndexJour++) {
			int prev = v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour - 1];
			int curr = v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour];
			if (prev == -1 || curr == -1) {
				continue;
			}
			if (!inst.is_possible_Shift_Succede(prev, curr)) {
				printf("Personne %d, jour %d: shift %d ne peut pas succéder à shift %d\n", iIndexPersonne, iIndexJour, curr, prev);
				return false;
			}
		}
    }
	return true;
}
bool Solution::check_conges(Instance inst) {
    for (int p = 0; p < inst.get_Nombre_Personne(); p++) {
        for (int j = 0; j < inst.get_Personne(p).v_Id_Jour_Conges.size(); j++) {
            int day_off = inst.get_Personne(p).v_Id_Jour_Conges[j];
            if (v_v_IdShift_Par_Personne_et_Jour[p][day_off] != -1) {
                return false;
            }
        }
    }
	return true;
}


bool Solution::check_min_consecutif_shifts(Instance inst) {
    for (int p = 0; p < inst.get_Nombre_Personne(); p++) {
        int compteur_consecutif = 0;
        for (int j = 0; j < inst.get_Nombre_Jour(); j++) {
            if (v_v_IdShift_Par_Personne_et_Jour[p][j] != -1) {
                compteur_consecutif++;
            }
            else {
                if (compteur_consecutif > 0 && compteur_consecutif < inst.get_Personne_Nbre_Shift_Consecutif_Min(p)) {
					cout << "Personne " << p << " jour " << j << " compteur_consecutif " << compteur_consecutif << "\n";
                    return false;
                }
                compteur_consecutif = 0;
            }
        }
        if (compteur_consecutif > 0 && compteur_consecutif < inst.get_Personne_Nbre_Shift_Consecutif_Min(p)) {
            cout << "Personne " << p << " dernier jour " << " compteur_consecutif " << compteur_consecutif << "\n";
            return false;
        }
	}
    return true;
}

bool Solution::check_min_minutes_travailees(Instance inst) {
    for (int p = 0; p < inst.get_Nombre_Personne(); p++) {
        int duree_travail = 0;
        for (int j = 0; j < inst.get_Nombre_Jour(); j++) {
            int shift_actuel = v_v_IdShift_Par_Personne_et_Jour[p][j];
            if (shift_actuel != -1) {
                duree_travail += inst.get_Shift_Duree(shift_actuel);
            }
        }
        if (duree_travail < inst.get_Personne_Duree_total_Min(p)) {
			cout << "Personne " << p << " duree_travail " << duree_travail << "minimum : " << inst.get_Personne_Duree_total_Min(p) << "\n";
            return false;
        }
	}
    return true;
}


bool Solution::check_max_assignable_shifts(Instance inst){
    for (int p = 0; p < inst.get_Nombre_Personne(); p++) {
        vector<int> compteur_shifts(inst.get_Nombre_Shift(), 0);
        for (int j = 0; j < inst.get_Nombre_Jour(); j++) {
            int shift_actuel = v_v_IdShift_Par_Personne_et_Jour[p][j];
            if (shift_actuel != -1) {
                compteur_shifts[shift_actuel]++;
                if (compteur_shifts[shift_actuel] > inst.get_Personne_Shift_Nbre_Max(p, shift_actuel)) {
                    return false;
                }
            }
        }
    }
    return true;
}

int Solution::check_solution(Instance inst) {
	int i_Nb_Contrainte_Respectees = 1;
    if (this->check_conges(inst)==true){
        cout << "Conge OK\n";
        i_Nb_Contrainte_Respectees++;
    } else {
        cout << "Conge NOT OK\n";
	}

    if (this->check_min_consecutif_shifts(inst)==true){
        cout << "Min consecutif shifts OK\n";
        i_Nb_Contrainte_Respectees++;
    }
    else {
        cout << "Min consecutif shifts NOT OK\n";
    }

    if( this->check_min_minutes_travailees(inst)==true){
        cout << "Min minutes travaillees OK\n";
        i_Nb_Contrainte_Respectees++;
	}
    else {
        cout << "Min minutes travaillees NOT OK\n";
    }

    if (this->check_max_assignable_shifts(inst) == true) {
        cout << "Max assignable shifts OK\n";
        i_Nb_Contrainte_Respectees++;
    }
    else {
		cout << "Max assignable shifts NOT OK\n";
    }

    if (this->check_max_we(inst) == true) {
        cout << "Max WE OK\n";
        i_Nb_Contrainte_Respectees++;
    }
    else {
        cout << "Max WE NOT OK\n";
	}

    if(this->check_min_repos_consecutif(inst)==true){
        cout << "Min repos consecutif OK\n";
		i_Nb_Contrainte_Respectees++;
    }
    else {
        cout << "Min repos consecutif NOT OK\n";
    }

    if (this->check_max_shift_consecutif(inst) == true) {
        cout << "Max shift consecutif OK\n";
		i_Nb_Contrainte_Respectees++;
    }
    else {
        cout << "Max shift consecutif NOT OK\n";
    }

    if (this->check_max_minutes_per_personne(inst) == true) {
        cout << "Max minutes par personne OK\n";
		i_Nb_Contrainte_Respectees++;
    }
    else {
        cout << "Max minutes par personne NOT OK\n";
    }

    if (this->check_shift_succede(inst) == true) {
        cout << "Shift succede OK\n";
		i_Nb_Contrainte_Respectees++;
    }
    else {
        cout << "Shift succede NOT OK\n";
	}

	return i_Nb_Contrainte_Respectees;
}


vector<vector<int>> Solution::creation_Solution_Initiale(Instance inst) {
    v_v_IdShift_Par_Personne_et_Jour.assign(
        inst.get_Nombre_Personne(),
        vector<int>(inst.get_Nombre_Jour(), -1)
    );
    this->creation_Solution_Sans_Contrainte(inst);
    this->ajout_conges_personne(inst);
    this->suppression_jours_WE_de_trop(inst);
    this->suppression_shifts_par_type_de_trop(inst);
    this->Shift_succede(inst);
    this->suppression_max_shifts_consecutifs(inst);
	this->maximum_min_per_personne(inst);   
	this->ajout_jours_de_repos_consecutif(inst);
	int i_Nb_Contraintes_Respectees = this->check_solution(inst);
	cout << "Nombre de contraintes respectées : " << i_Nb_Contraintes_Respectees << " / 10\n";
    return v_v_IdShift_Par_Personne_et_Jour;
}


