#include "Ma_Solution.hpp"
#include <cstdlib>
#include <cstdio>

// Constructeurs et destructeur
Ma_Solution::Ma_Solution() {}
Ma_Solution::~Ma_Solution() {}

void Ma_Solution::creation_Solution_Sans_Contrainte(Instance* inst) {
    int iShiftAFaire = 0;
    for (int iIndexPersonne = 0; iIndexPersonne < inst->get_Nombre_Personne(); iIndexPersonne++) {
        for (int iIndexJour = 0; iIndexJour < inst->get_Nombre_Jour(); iIndexJour++) {
            iShiftAFaire = rand() % inst->get_Nombre_Shift();
            v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour] = iShiftAFaire;
        }
    }
}

void Ma_Solution::ajout_conges_personne(Instance* inst) {
    for (int p = 0; p < inst->get_Nombre_Personne(); p++) {
        vector<int> conges = inst->get_vector_Personne_Id_Jour_Conges(p);
        for (int j = 0; j < conges.size(); j++) {
            int day_off = conges[j];
            v_v_IdShift_Par_Personne_et_Jour[p][day_off] = -1;
        }
    }
}

void Ma_Solution::suppression_jours_WE_de_trop(Instance* inst) {
    int i_Nb_WE_Plage_Horaire = inst->get_Nombre_Jour() / 7;
    for (int p = 0; p < inst->get_Nombre_Personne(); p++) {
        int i_Nb_WE_A_Travailler = inst->get_Personne_Nbre_WE_Max(p);

        vector<int> i_Indices_WE_A_Travailler = {};
        for (int i_Boucle = 0; i_Boucle < i_Nb_WE_A_Travailler; i_Boucle++) {
            int i_WE = rand() % i_Nb_WE_Plage_Horaire;
            while (std::find(i_Indices_WE_A_Travailler.begin(), i_Indices_WE_A_Travailler.end(), i_WE) != i_Indices_WE_A_Travailler.end()) {
                i_WE = rand() % i_Nb_WE_Plage_Horaire;
            }
            i_Indices_WE_A_Travailler.push_back(i_WE);
        }

        int compteur_WE = 0;
        for (int j = 0; j < inst->get_Nombre_Jour(); j++) {
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

void Ma_Solution::suppression_max_shifts_consecutifs(Instance* inst) {
    for (int p = 0; p < inst->get_Nombre_Personne(); p++) {
        int compteur_consecutif = 0;
        int max_consecutif = inst->get_Personne_Nbre_Shift_Consecutif_Max(p);
        for (int j = 0; j < inst->get_Nombre_Jour(); j++) {
            if (v_v_IdShift_Par_Personne_et_Jour[p][j] != -1) {
                compteur_consecutif++;
                if (compteur_consecutif > max_consecutif) {
                    v_v_IdShift_Par_Personne_et_Jour[p][j] = -1;
                    compteur_consecutif = 0;
                }
            }
        }
    }
}

void Ma_Solution::suppression_shifts_par_type_de_trop(Instance* inst) {
    for (int p = 0; p < inst->get_Nombre_Personne(); p++) {
        vector<int> compteur_shifts = {};
        for (int i = 0; i < inst->get_Nombre_Shift(); i++) {
            compteur_shifts.push_back(0);
        }

        for (int j = 0; j < inst->get_Nombre_Jour(); j++) {
            int shift_actuel = v_v_IdShift_Par_Personne_et_Jour[p][j];
            if (shift_actuel != -1) {
                compteur_shifts[shift_actuel]++;
                int max_shift = inst->get_Personne_Shift_Nbre_Max(p, shift_actuel);
                if (compteur_shifts[shift_actuel] > max_shift) {
                    int shift_aleatoire = rand() % inst->get_Nombre_Shift();
                    int i_compteur_boucle = 0;
                    while (shift_aleatoire == shift_actuel || compteur_shifts[shift_aleatoire] >= inst->get_Personne_Shift_Nbre_Max(p, shift_aleatoire)) {
                        shift_aleatoire = rand() % inst->get_Nombre_Shift();
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

void Ma_Solution::Shift_succede(Instance* inst) {
    size_t iNbPersonne = inst->get_Nombre_Personne();
    size_t iNbJour = inst->get_Nombre_Jour();
    for (int iIndexPersonne = 0; iIndexPersonne < iNbPersonne; iIndexPersonne++) {
        for (int iIndexJour = 1; iIndexJour < iNbJour; iIndexJour++) {
            int prev = v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour - 1];
            int curr = v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour];
            if (prev == -1 || curr == -1 || inst->is_possible_Shift_Succede(prev, curr)) {
                continue;
            }
            curr = 0;
            int compteur = std::count(v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne].begin(), v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne].end(), curr);
            while ((inst->is_possible_Shift_Succede(prev, curr) == false || compteur >= inst->get_Personne_Shift_Nbre_Max(iIndexPersonne, curr)) && curr < inst->get_Nombre_Shift() - 1) {
                curr++;
                compteur = std::count(v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne].begin(), v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne].end(), curr);
            }

            if (curr == inst->get_Nombre_Shift() - 1 && (inst->is_possible_Shift_Succede(prev, curr) == false || compteur >= inst->get_Personne_Shift_Nbre_Max(iIndexPersonne, curr))) {
                curr = -1;
            }

            v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour] = curr;
        }
    }
}

void Ma_Solution::afficher_solution() {
    cout << "\n=== Solution générée ===\n";
    for (size_t p = 0; p < v_v_IdShift_Par_Personne_et_Jour.size(); p++) {
        cout << "Personne " << p << " : ";
        for (size_t j = 0; j < v_v_IdShift_Par_Personne_et_Jour[p].size(); j++) {
            cout << v_v_IdShift_Par_Personne_et_Jour[p][j] << " ";
        }
        cout << endl;
    }
}

void Ma_Solution::maximum_min_per_personne(Instance* inst) {
    size_t iNbPersonne = inst->get_Nombre_Personne();
    size_t iNbJour = inst->get_Nombre_Jour();
    for (int iIndexPersonne = 0; iIndexPersonne < iNbPersonne; iIndexPersonne++) {
        int duree_travail = 0;
        int shift_aleatoire;
        int duree_shift_actuel = 0;
        int compteur_boucle = 0;
        for (int iIndexJour = 0; iIndexJour < iNbJour; iIndexJour++) {
            int shift_actuel = v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour];
            if (shift_actuel != -1) {
                duree_travail += inst->get_Shift_Duree(shift_actuel);
            }
        }

        while (duree_travail > inst->get_Personne_Duree_total_Max(iIndexPersonne)) {
            shift_aleatoire = rand() % inst->get_Nombre_Jour();
            while (v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][shift_aleatoire] == -1) {
                compteur_boucle++;
                shift_aleatoire = rand() % inst->get_Nombre_Jour();
                if (compteur_boucle > 1000) {
                    cout << "Erreur : pas de solution possible avec les contraintes de durée maximale de travail par personne." << endl;
                    return;
                }
            }
            duree_shift_actuel = inst->get_Shift_Duree(v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][shift_aleatoire]);
            v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][shift_aleatoire] = -1;
            duree_travail -= duree_shift_actuel;
        }
    }
}

void Ma_Solution::ajout_jours_de_repos_consecutif(Instance* inst) {
        size_t iNbPersonne = inst->get_Nombre_Personne();
        size_t iNbJour = inst->get_Nombre_Jour();

        for (int personne = 0; personne < iNbPersonne; personne++) {
            int min_consecutif = inst->get_Personne_Jour_OFF_Consecutif_Min(personne);
            if (min_consecutif <= 1) continue; // Aucune contrainte pour 1 jour ou moins

            int jour = 0;
            while (jour < iNbJour) {
                if (v_v_IdShift_Par_Personne_et_Jour[personne][jour] == -1) {  //Si c'est un jour de repos, trouver la séquence
                    int debut_sequence = jour;
                    int fin_sequence = jour;

                    while (fin_sequence + 1 < iNbJour &&
                        v_v_IdShift_Par_Personne_et_Jour[personne][fin_sequence + 1] == -1) {
                        fin_sequence++;
                    }

                    int longueur_sequence = fin_sequence - debut_sequence + 1;

                    if (longueur_sequence < min_consecutif) { // Si la séquence est trop courte, il faut ajouter des jours de repos
                        int jours_manquants = min_consecutif - longueur_sequence;

                        int extension_avant_possible = 0;    // On regarde si on peut étendre avant
                        for (int k = debut_sequence - 1; k >= 0 && extension_avant_possible < jours_manquants; k--) {
                            if (v_v_IdShift_Par_Personne_et_Jour[personne][k] != -1) {
                                extension_avant_possible++;
                            }
                            else {
                                break;
                            }
                        }

                        int extension_apres_possible = 0;    // On regarde si on peut étendre après
                        for (int k = fin_sequence + 1; k < iNbJour && extension_apres_possible < jours_manquants; k++) {
                            if (v_v_IdShift_Par_Personne_et_Jour[personne][k] != -1) {
                                extension_apres_possible++;
                            }
                            else {
                                break;
                            }
                        }

                        // On étend d'abord avant si possible
                        for (int k = debut_sequence - 1; k >= 0 && jours_manquants > 0; k--) {
                            if (v_v_IdShift_Par_Personne_et_Jour[personne][k] != -1) {
                                v_v_IdShift_Par_Personne_et_Jour[personne][k] = -1;
                                jours_manquants--;
                                debut_sequence = k;
                            }
                        }

                        // Puis après si nécessaire
                        for (int k = fin_sequence + 1; k < iNbJour && jours_manquants > 0; k++) {
                            if (v_v_IdShift_Par_Personne_et_Jour[personne][k] != -1) {
                                v_v_IdShift_Par_Personne_et_Jour[personne][k] = -1;
                                jours_manquants--;
                                fin_sequence = k;
                            }
                        }


                        jour = fin_sequence + 1;
                    }
                    else {
                        jour = fin_sequence + 1;
                    }
                }
                else {
                    jour++;
                }
            }
        }
    }

bool Ma_Solution::check_max_we(Instance* inst) {
    size_t iNbPersonne = inst->get_Nombre_Personne();
    size_t iNbJour = inst->get_Nombre_Jour();
    for (int iIndexPersonne = 0; iIndexPersonne < iNbPersonne; iIndexPersonne++) {
        int max_we = inst->get_Personne_Nbre_WE_Max(iIndexPersonne);
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

bool Ma_Solution::check_min_repos_consecutif(Instance* inst) {
    size_t iNbPersonne = inst->get_Nombre_Personne();
    size_t iNbJour = inst->get_Nombre_Jour();
    for (int iIndexPersonne = 0; iIndexPersonne < iNbPersonne; iIndexPersonne++) {
        int min_repos = inst->get_Personne_Jour_OFF_Consecutif_Min(iIndexPersonne);
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

bool Ma_Solution::check_max_shift_consecutif(Instance* inst) {
    size_t iNbPersonne = inst->get_Nombre_Personne();
    size_t iNbJour = inst->get_Nombre_Jour();
    for (int iIndexPersonne = 0; iIndexPersonne < iNbPersonne; iIndexPersonne++) {
        int compteur_consecutif = 0;
        int max_shift_consecutif = inst->get_Personne_Nbre_Shift_Consecutif_Max(iIndexPersonne);
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

bool Ma_Solution::check_max_minutes_per_personne(Instance* inst) {
    size_t iNbPersonne = inst->get_Nombre_Personne();
    size_t iNbJour = inst->get_Nombre_Jour();
    for (int iIndexPersonne = 0; iIndexPersonne < iNbPersonne; iIndexPersonne++) {
        int compteur_minute = 0;
        int nbMaxMinutes = inst->get_Personne_Duree_total_Max(iIndexPersonne);
        for (int iIndexJour = 0; iIndexJour < iNbJour; iIndexJour++) {
            if (v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour] != -1) {
                compteur_minute += inst->get_Shift_Duree(v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour]);
            }
            if (compteur_minute > nbMaxMinutes) {
                cout << "Personne " << iIndexPersonne << " compteur_minute : " << compteur_minute << " maximum : " << nbMaxMinutes << "\n";
                return false;
            }
        }
    }
    return true;
}

bool Ma_Solution::check_shift_succede(Instance* inst) {
    size_t iNbPersonne = inst->get_Nombre_Personne();
    size_t iNbJour = inst->get_Nombre_Jour();
    for (int iIndexPersonne = 0; iIndexPersonne < iNbPersonne; iIndexPersonne++) {
        for (int iIndexJour = 1; iIndexJour < iNbJour; iIndexJour++) {
            int prev = v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour - 1];
            int curr = v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour];
            if (prev == -1 || curr == -1) {
                continue;
            }
            if (!inst->is_possible_Shift_Succede(prev, curr)) {
                printf("Personne %d, jour %d: shift %d ne peut pas succéder à shift %d\n", iIndexPersonne, iIndexJour, curr, prev);
                return false;
            }
        }
    }
    return true;
}

bool Ma_Solution::check_conges(Instance* inst) {
    for (int p = 0; p < inst->get_Nombre_Personne(); p++) {
        vector<int> conges = inst->get_vector_Personne_Id_Jour_Conges(p);
        for (int j = 0; j < conges.size(); j++) {
            int day_off = conges[j];
            if (v_v_IdShift_Par_Personne_et_Jour[p][day_off] != -1) {
                return false;
            }
        }
    }
    return true;
}

bool Ma_Solution::check_min_consecutif_shifts(Instance* inst) {
    for (int p = 0; p < inst->get_Nombre_Personne(); p++) {
        int compteur_consecutif = 0;
        for (int j = 0; j < inst->get_Nombre_Jour(); j++) {
            if (v_v_IdShift_Par_Personne_et_Jour[p][j] != -1) {
                compteur_consecutif++;
            }
            else {
                if (compteur_consecutif > 0 && compteur_consecutif < inst->get_Personne_Nbre_Shift_Consecutif_Min(p)) {
                    cout << "Personne " << p << " jour " << j << " compteur_consecutif " << compteur_consecutif << "\n";
                    return false;
                }
                compteur_consecutif = 0;
            }
        }
        if (compteur_consecutif > 0 && compteur_consecutif < inst->get_Personne_Nbre_Shift_Consecutif_Min(p)) {
            cout << "Personne " << p << " dernier jour compteur_consecutif " << compteur_consecutif << "\n";
            return false;
        }
    }
    return true;
}

bool Ma_Solution::check_min_minutes_travailees(Instance* inst) {
    for (int p = 0; p < inst->get_Nombre_Personne(); p++) {
        int duree_travail = 0;
        for (int j = 0; j < inst->get_Nombre_Jour(); j++) {
            int shift_actuel = v_v_IdShift_Par_Personne_et_Jour[p][j];
            if (shift_actuel != -1) {
                duree_travail += inst->get_Shift_Duree(shift_actuel);
            }
        }
        if (duree_travail < inst->get_Personne_Duree_total_Min(p)) {
            cout << "Personne " << p << " duree_travail " << duree_travail << " minimum : " << inst->get_Personne_Duree_total_Min(p) << "\n";
            return false;
        }
    }
    return true;
}

bool Ma_Solution::check_max_assignable_shifts(Instance* inst) {
    for (int p = 0; p < inst->get_Nombre_Personne(); p++) {
        vector<int> compteur_shifts(inst->get_Nombre_Shift(), 0);
        for (int j = 0; j < inst->get_Nombre_Jour(); j++) {
            int shift_actuel = v_v_IdShift_Par_Personne_et_Jour[p][j];
            if (shift_actuel != -1) {
                compteur_shifts[shift_actuel]++;
                if (compteur_shifts[shift_actuel] > inst->get_Personne_Shift_Nbre_Max(p, shift_actuel)) {
                    return false;
                }
            }
        }
    }
    return true;
}

int Ma_Solution::check_solution(Instance* inst) {
    int i_Nb_Contrainte_Respectees = 1;

    if (this->check_conges(inst) == true) {
        cout << "Conge OK\n";
        i_Nb_Contrainte_Respectees++;
    }
    else {
        cout << "Conge NOT OK\n";
    }

    if (this->check_min_consecutif_shifts(inst) == true) {
        cout << "Min consecutif shifts OK\n";
        i_Nb_Contrainte_Respectees++;
    }
    else {
        cout << "Min consecutif shifts NOT OK\n";
    }

    if (this->check_min_minutes_travailees(inst) == true) {
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

    if (this->check_min_repos_consecutif(inst) == true) {
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

vector<vector<int>> Ma_Solution::creation_Solution_Initiale(Instance* inst) {
        v_v_IdShift_Par_Personne_et_Jour.assign(
            inst->get_Nombre_Personne(),
            vector<int>(inst->get_Nombre_Jour(), -1)
        );
        this->creation_Solution_Sans_Contrainte(inst);
        this->ajout_conges_personne(inst);
        this->suppression_jours_WE_de_trop(inst);
        this->suppression_shifts_par_type_de_trop(inst);
        this->Shift_succede(inst);
        this->suppression_max_shifts_consecutifs(inst);
        this->maximum_min_per_personne(inst);
        this->ajout_jours_de_repos_consecutif(inst);
        this->MetaHeuristique_Recherche_Local(inst);
        int i_Nb_Contraintes_Respectees = this->check_solution(inst);
        cout << "Nombre de contraintes respectées : " << i_Nb_Contraintes_Respectees << " / 10\n";
        return v_v_IdShift_Par_Personne_et_Jour;
    }


bool Ma_Solution::Verifie_Neuf_Contraintes(Instance* inst, int ligne_a_verifier) {
    bool b_ligne_ok = true;
    int i_duree_travail = 0;
    int i_shift_consecutif = 0;
    int i_nb_WE = 0;
    vector<int> v_i_Nb_shift(inst->get_Nombre_Shift(), 0);

    for (int j = 0; j < v_v_IdShift_Par_Personne_et_Jour[ligne_a_verifier].size(); j++) {
        if (v_v_IdShift_Par_Personne_et_Jour[ligne_a_verifier][j] != -1) {
            v_i_Nb_shift[v_v_IdShift_Par_Personne_et_Jour[ligne_a_verifier][j]]++;
            i_duree_travail += inst->get_Shift_Duree(v_v_IdShift_Par_Personne_et_Jour[ligne_a_verifier][j]);
            i_shift_consecutif++;

            if ((j % 7) == 5) i_nb_WE++;
            if (((j % 7) == 6) && (j > 0) && (v_v_IdShift_Par_Personne_et_Jour[ligne_a_verifier][j - 1] == -1))
                i_nb_WE++;

            // Vérification du nombre de shifts consécutifs maximum
            if (i_shift_consecutif > inst->get_Personne_Nbre_Shift_Consecutif_Max(ligne_a_verifier)) {
                b_ligne_ok = false;
            }

            // Vérification des jours de congés
            if (!inst->is_Available_Personne_Jour(ligne_a_verifier, j)) {
                b_ligne_ok = false;
            }
        }
        else {
            // Jour OFF (-1)

            // Vérification du minimum de shifts consécutifs pour la séquence qui vient de se terminer
            if (i_shift_consecutif > 0 && i_shift_consecutif < inst->get_Personne_Nbre_Shift_Consecutif_Min(ligne_a_verifier)) {
                b_ligne_ok = false;
            }
            i_shift_consecutif = 0;
        }

        // Vérification des successions de shifts
        if (j < v_v_IdShift_Par_Personne_et_Jour[ligne_a_verifier].size() - 1) {
            if ((v_v_IdShift_Par_Personne_et_Jour[ligne_a_verifier][j] != -1) &&
                (v_v_IdShift_Par_Personne_et_Jour[ligne_a_verifier][j + 1] != -1)) {
                if (!inst->is_possible_Shift_Succede(v_v_IdShift_Par_Personne_et_Jour[ligne_a_verifier][j],
                    v_v_IdShift_Par_Personne_et_Jour[ligne_a_verifier][j + 1])) {
                    b_ligne_ok = false;
                }
            }
        }
    }

    // Vérifier la dernière séquence de shifts si la ligne se termine par des shifts
    if (i_shift_consecutif > 0 && i_shift_consecutif < inst->get_Personne_Nbre_Shift_Consecutif_Min(ligne_a_verifier)) {
        b_ligne_ok = false;
    }

    // VÉRIFICATION COMPLÈTE : Parcourir TOUTES les séquences de jours OFF
    int min_jours_off = inst->get_Personne_Jour_OFF_Consecutif_Min(ligne_a_verifier);
    if (min_jours_off > 1) {
        int i = 0;
        while (i < v_v_IdShift_Par_Personne_et_Jour[ligne_a_verifier].size()) {
            if (v_v_IdShift_Par_Personne_et_Jour[ligne_a_verifier][i] == -1) {
                // Début d'une séquence OFF, compter sa longueur
                int debut_sequence = i;
                int nb_jours_off_consecutifs = 0;

                while (i < v_v_IdShift_Par_Personne_et_Jour[ligne_a_verifier].size() &&
                    v_v_IdShift_Par_Personne_et_Jour[ligne_a_verifier][i] == -1) {
                    nb_jours_off_consecutifs++;
                    i++;
                }

                // Vérifier si cette séquence OFF respecte le minimum
                // On vérifie TOUTES les séquences, y compris celles en début/fin
                // SAUF si c'est toute la ligne qui est en OFF (cas extrême)
                bool a_des_shifts_avant = false;
                bool a_des_shifts_apres = false;

                // Vérifier s'il y a des shifts avant cette séquence
                for (int k = 0; k < debut_sequence; k++) {
                    if (v_v_IdShift_Par_Personne_et_Jour[ligne_a_verifier][k] != -1) {
                        a_des_shifts_avant = true;
                        break;
                    }
                }

                // Vérifier s'il y a des shifts après cette séquence
                for (int k = i; k < v_v_IdShift_Par_Personne_et_Jour[ligne_a_verifier].size(); k++) {
                    if (v_v_IdShift_Par_Personne_et_Jour[ligne_a_verifier][k] != -1) {
                        a_des_shifts_apres = true;
                        break;
                    }
                }

                // On vérifie la séquence si elle a au moins un shift avant OU après
                // (donc on exclut seulement le cas où toute la ligne est en OFF)
                if ((a_des_shifts_avant || a_des_shifts_apres) &&
                    nb_jours_off_consecutifs < min_jours_off) {
                    b_ligne_ok = false;
                }
            }
            else {
                i++;
            }
        }
    }

    // Vérification de la durée totale maximale
    if (i_duree_travail > inst->get_Personne_Duree_total_Max(ligne_a_verifier)) {
        b_ligne_ok = false;
    }

    // Vérification du nombre de WE maximum
    if (i_nb_WE > inst->get_Personne_Nbre_WE_Max(ligne_a_verifier)) {
        b_ligne_ok = false;
    }

    // Vérification du nombre maximal de shifts par type
    for (int shift_type = 0; shift_type < inst->get_Nombre_Shift(); shift_type++) {
        if (v_i_Nb_shift[shift_type] > inst->get_Personne_Shift_Nbre_Max(ligne_a_verifier, shift_type)) {
            b_ligne_ok = false;
        }
    }

    return b_ligne_ok;
}

bool Ma_Solution::Verifie_Dix_Contraintes(Instance* inst, int ligne_a_verifier) {
    // On vérifie d'abord les 9 contraintes
    if (!this->Verifie_Neuf_Contraintes(inst, ligne_a_verifier)) {
        return false;
    }
    int i_duree_travail = 0;
    // On calcul maintenant la durée totale travaillée
    for (int j = 0; j < v_v_IdShift_Par_Personne_et_Jour[ligne_a_verifier].size(); j++) {
        if (v_v_IdShift_Par_Personne_et_Jour[ligne_a_verifier][j] != -1) {
            i_duree_travail += inst->get_Shift_Duree(
                v_v_IdShift_Par_Personne_et_Jour[ligne_a_verifier][j]
            );
        }
    }
    // Si la durée totale travaillée est inférieure au minimum, on retourne false
    if (i_duree_travail < inst->get_Personne_Duree_total_Min(ligne_a_verifier)) {
        return false;
    }
    // Sinon on retourne true
    return true;
}


vector<int> Ma_Solution::Genere_Ligne_Voisine_Consecutifs_Shifts(Instance* inst, int ligne_a_modifier) {
    vector<int> v_Nouvelle_Ligne = v_v_IdShift_Par_Personne_et_Jour[ligne_a_modifier];

    vector<int> v_Ligne_Originale = v_v_IdShift_Par_Personne_et_Jour[ligne_a_modifier];

    int compteur_tentatives = 0;
    int max_tentatives = 50000; 

    while (!this->Verifie_Neuf_Contraintes(inst, ligne_a_modifier)) {
        v_v_IdShift_Par_Personne_et_Jour[ligne_a_modifier] = v_Nouvelle_Ligne;

        int indice1 = rand() % v_Nouvelle_Ligne.size();
        int indice2 = rand() % v_Nouvelle_Ligne.size();

        while (indice2 == indice1) {
            indice2 = rand() % v_Nouvelle_Ligne.size();
        }

        int temp = v_Nouvelle_Ligne[indice1];
        v_Nouvelle_Ligne[indice1] = v_Nouvelle_Ligne[indice2];
        v_Nouvelle_Ligne[indice2] = temp;

        compteur_tentatives++;

        if (compteur_tentatives > max_tentatives) {
            v_Nouvelle_Ligne = v_Ligne_Originale;
            compteur_tentatives = 0;
            cout << "Attention : nombre maximal de tentatives atteint pour la ligne "
                << ligne_a_modifier << ", redémarrage...\n";
        }
    }

    v_v_IdShift_Par_Personne_et_Jour[ligne_a_modifier] = v_Ligne_Originale;

    return v_Nouvelle_Ligne;
}

vector<int> Ma_Solution::Genere_Ligne_Voisine_Minimum_Min_Travaille(Instance* inst, int ligne_a_modifier) {
    vector<int> v_Nouvelle_Ligne = v_v_IdShift_Par_Personne_et_Jour[ligne_a_modifier];

    // Sauvegarder la ligne actuelle dans la solution pour les vérifications
    vector<int> v_Ligne_Originale = v_v_IdShift_Par_Personne_et_Jour[ligne_a_modifier];
    vector<int> v_id_shift_repos;
    int compteur_tentatives = 0;
    int max_tentatives = 10000; // Pour éviter une boucle infinie
	for (int i = 0; i < v_Nouvelle_Ligne.size(); i++) {
		if (v_Nouvelle_Ligne[i]==-1) {
			v_id_shift_repos.push_back(i);
		}
	}
    while (!this->Verifie_Dix_Contraintes(inst, ligne_a_modifier)) {
        // Restaurer la ligne modifiée dans la solution pour la vérification
        v_v_IdShift_Par_Personne_et_Jour[ligne_a_modifier] = v_Nouvelle_Ligne;

        // Générer deux indices aléatoires différents
        int indice1 = rand() % v_id_shift_repos .size();
		int indice_a_changer = v_id_shift_repos[indice1];

        // Effectuer un changement
        int temp = v_Nouvelle_Ligne[indice_a_changer];
        v_Nouvelle_Ligne[indice1] = rand() % inst->get_Nombre_Shift();

        compteur_tentatives++;

        // Si on a fait trop de tentatives, on retourne à la ligne originale et on réessaye
        if (compteur_tentatives > max_tentatives) {
            v_Nouvelle_Ligne = v_Ligne_Originale;
            compteur_tentatives = 0;
            cout << "Attention : nombre maximal de tentatives atteint pour la ligne "
                << ligne_a_modifier << ", redémarrage...\n";
        }
    }

    // Restaurer la ligne originale dans la solution avant de retourner
    v_v_IdShift_Par_Personne_et_Jour[ligne_a_modifier] = v_Ligne_Originale;

    return v_Nouvelle_Ligne;
}

void Ma_Solution::MetaHeuristique_Recherche_Local(Instance* inst) {
    int Meilleur_Score = this->check_solution(inst);

    while (Meilleur_Score < 9) {
        bool progression = false;

        for (int ligne = 0; ligne < inst->get_Nombre_Personne(); ligne++) {

            if (this->Verifie_Neuf_Contraintes(inst, ligne)) {
                cout << "Ligne " << ligne << " déjà OK, on passe à la suivante.\n";
                continue;
            }

            cout << "Ligne " << ligne << " ne respecte pas les 9 contraintes, tentative de correction...\n";

            vector<int> v_Ligne_Avant = v_v_IdShift_Par_Personne_et_Jour[ligne];

            vector<int> v_Nouvelle_Ligne = this->Genere_Ligne_Voisine_Consecutifs_Shifts(inst, ligne);

            v_v_IdShift_Par_Personne_et_Jour[ligne] = v_Nouvelle_Ligne;

            // VÉRIFIER que la nouvelle ligne respecte bien les 9 contraintes
            if (this->Verifie_Neuf_Contraintes(inst, ligne)) {
                cout << "Ligne " << ligne << " corrigée avec succès !\n";
                progression = true;
            }
            else {
                cout << "Échec de correction pour la ligne " << ligne << ", restauration...\n";
                v_v_IdShift_Par_Personne_et_Jour[ligne] = v_Ligne_Avant; // Restaurer
            }
        }

        // Recalculer le score après avoir traité toutes les lignes
        int Nouveau_Score = this->check_solution(inst);

        cout << "\n=== Score après itération : " << Nouveau_Score << " / 10 ===\n";

        // Si aucune progression, on risque de boucler à l'infini
        if (!progression && Nouveau_Score == Meilleur_Score) {
            cout << "ATTENTION : Aucune progression possible, arrêt de la métaheuristique.\n";
            break;
        }

        Meilleur_Score = Nouveau_Score;
    }
    while (Meilleur_Score < 10) {
        bool progression = false;

        for (int ligne = 0; ligne < inst->get_Nombre_Personne(); ligne++) {

            if (this->Verifie_Dix_Contraintes(inst, ligne)) {
                cout << "Ligne " << ligne << " déjà OK, on passe à la suivante.\n";
                continue;
            }

            cout << "Ligne " << ligne << " ne respecte pas les 10 contraintes, tentative de correction...\n";

            vector<int> v_Ligne_Avant = v_v_IdShift_Par_Personne_et_Jour[ligne];

            vector<int> v_Nouvelle_Ligne = this->Genere_Ligne_Voisine_Minimum_Min_Travaille(inst, ligne);

            v_v_IdShift_Par_Personne_et_Jour[ligne] = v_Nouvelle_Ligne;

            // VÉRIFIER que la nouvelle ligne respecte bien les 10 contraintes
            if (this->Verifie_Dix_Contraintes(inst, ligne)) {
                cout << "Ligne " << ligne << " corrigée avec succès !\n";
                progression = true;
            }
            else {
                cout << "Échec de correction pour la ligne " << ligne << ", restauration...\n";
                v_v_IdShift_Par_Personne_et_Jour[ligne] = v_Ligne_Avant; // Restaurer
            }
        }

        // Recalculer le score après avoir traité toutes les lignes
        int Nouveau_Score = this->check_solution(inst);

        cout << "\n=== Score après itération : " << Nouveau_Score << " / 10 ===\n";

        // Si aucune progression, on risque de boucler à l'infini
        if (!progression && Nouveau_Score == Meilleur_Score) {
            cout << "ATTENTION : Aucune progression possible, arrêt de la métaheuristique.\n";
            break;
        }

        Meilleur_Score = Nouveau_Score;
    }

    cout << "\n=== Métaheuristique terminée avec score final : " << Meilleur_Score << " / 10 ===\n";
}