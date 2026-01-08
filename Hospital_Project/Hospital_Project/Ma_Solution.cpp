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
        std::cout << "Initial         " << " : ";
        for (size_t j = 0; j < v_v_IdShift_Par_Personne_et_Jour[1].size(); j++)
        {
            std::cout << v_v_IdShift_Par_Personne_et_Jour[1][j] << " ";
        }
        std::cout << endl;
        this->ajout_conges_personne(inst);
        std::cout << "Conges          " << " : ";
        for (size_t j = 0; j < v_v_IdShift_Par_Personne_et_Jour[1].size(); j++)
        {
            std::cout << v_v_IdShift_Par_Personne_et_Jour[1][j] << " ";
        }
        std::cout << endl;
        this->suppression_jours_WE_de_trop(inst);
        std::cout << "Week-ends       " << " : ";
        for (size_t j = 0; j < v_v_IdShift_Par_Personne_et_Jour[1].size(); j++)
        {
            std::cout << v_v_IdShift_Par_Personne_et_Jour[1][j] << " ";
        }
        std::cout << endl;
        this->suppression_shifts_par_type_de_trop(inst);
        std::cout << "Shifts par type " << " : ";
        for (size_t j = 0; j < v_v_IdShift_Par_Personne_et_Jour[1].size(); j++)
        {
            std::cout << v_v_IdShift_Par_Personne_et_Jour[1][j] << " ";
        }
        std::cout << endl;
        this->Shift_succede(inst);
        std::cout << "Shift succedent " << " : ";
        for (size_t j = 0; j < v_v_IdShift_Par_Personne_et_Jour[1].size(); j++)
        {
            std::cout << v_v_IdShift_Par_Personne_et_Jour[1][j] << " ";
        }
        std::cout << endl;
        this->suppression_max_shifts_consecutifs(inst);
        std::cout << "Max shifts cons " << " : ";
        for (size_t j = 0; j < v_v_IdShift_Par_Personne_et_Jour[1].size(); j++)
        {
            std::cout << v_v_IdShift_Par_Personne_et_Jour[1][j] << " ";
        }
        std::cout << endl;
        this->maximum_min_per_personne(inst);
        std::cout << "Max minutes     " << " : ";
        for (size_t j = 0; j < v_v_IdShift_Par_Personne_et_Jour[1].size(); j++)
        {
            std::cout << v_v_IdShift_Par_Personne_et_Jour[1][j] << " ";
        }
        std::cout << endl;
        this->ajout_jours_de_repos_consecutif(inst);
        std::cout << "Repos consecutif" << " : ";
        for (size_t j = 0; j < v_v_IdShift_Par_Personne_et_Jour[1].size(); j++)
        {
            std::cout << v_v_IdShift_Par_Personne_et_Jour[1][j] << " ";
        }
        std::cout << endl;
        int i_Nb_Contraintes_Respectees = this->check_solution(inst);
        cout << "Nombre de contraintes respectÃ©es : " << i_Nb_Contraintes_Respectees << " / 10\n";
        return v_v_IdShift_Par_Personne_et_Jour;
    }

vector<vector<int>> Ma_Solution::Genere_Voisin(Instance* inst) {
    vector<vector<int>> v_v_Nouvelle_Solution = v_v_IdShift_Par_Personne_et_Jour;
    int p = rand() % inst->get_Nombre_Personne();
    int j = rand() % inst->get_Nombre_Jour();
    int nouveau_shift = rand() % inst->get_Nombre_Shift();
    v_v_Nouvelle_Solution[p][j] = nouveau_shift;
    return v_v_Nouvelle_Solution;
}

void Ma_Solution::MetaHeuristique_Recherche_Local(Instance* inst) {
    Ma_Solution MeilleurSolution = *this;
    int nb_contraintes_a_respecter = 10;
    vector<vector<int>> v_v_Nouvelle_Solution = v_v_IdShift_Par_Personne_et_Jour;
    vector<vector<int>> v_v_Meilleur_Voisin = v_v_IdShift_Par_Personne_et_Jour;
    int iteration = 0;
    int Meilleur_Score = this->check_solution(inst);
    bool is_improved = false;
    int iterationVoisin = 0;
    while (Meilleur_Score < nb_contraintes_a_respecter && iteration < 100) {
        is_improved = false;
        iterationVoisin = 0;
        while (!is_improved && iterationVoisin < 100) {
            v_v_Nouvelle_Solution = MeilleurSolution.Genere_Voisin(inst);
            MeilleurSolution.v_v_IdShift_Par_Personne_et_Jour = v_v_Nouvelle_Solution;
            int i_Nb_Contraintes_Respectees = MeilleurSolution.check_solution(inst);
            if (i_Nb_Contraintes_Respectees > Meilleur_Score) {
                cout << "Nouvelle Solution trouvée avec " << i_Nb_Contraintes_Respectees << " contraintes respectées.\n";
                v_v_Meilleur_Voisin = MeilleurSolution.v_v_IdShift_Par_Personne_et_Jour;
                Meilleur_Score = i_Nb_Contraintes_Respectees;
                is_improved = true;
            }
            iterationVoisin++;
        }
        MeilleurSolution.v_v_IdShift_Par_Personne_et_Jour = v_v_Meilleur_Voisin;
        iteration++;
    }
    if (iteration <= 100) {
        this->v_v_IdShift_Par_Personne_et_Jour = MeilleurSolution.v_v_IdShift_Par_Personne_et_Jour;
    }
}