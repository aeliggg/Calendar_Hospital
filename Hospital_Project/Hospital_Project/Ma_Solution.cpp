#include "Ma_Solution.hpp"
#include <cstdlib>
#include <cstdio>

// Constructeurs et destructeur
Ma_Solution::Ma_Solution() {}
Ma_Solution::~Ma_Solution() {}

void Ma_Solution::creation_Solution_Sans_Contrainte(Instance* inst) {
    // On crée une solution initiale aléatoire sans contrainte
    int iShiftAFaire = 0;
    for (int iIndexPersonne = 0; iIndexPersonne < inst->get_Nombre_Personne(); iIndexPersonne++) {
        for (int iIndexJour = 0; iIndexJour < inst->get_Nombre_Jour(); iIndexJour++) {
            // On associe à chaque personne un shift aléatoire disponible pour chaque jour
            iShiftAFaire = rand() % inst->get_Nombre_Shift();
            v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour] = iShiftAFaire;
        }
    }
}

void Ma_Solution::ajout_conges_personne(Instance* inst) {
    // On veut ajouter les congés pour chaque personne dans la solution initiale
    for (int p = 0; p < inst->get_Nombre_Personne(); p++) {
        vector<int> conges = inst->get_vector_Personne_Id_Jour_Conges(p);
        for (int j = 0; j < conges.size(); j++) {
            int day_off = conges[j];
            v_v_IdShift_Par_Personne_et_Jour[p][day_off] = -1;
        }
    }
}

void Ma_Solution::suppression_jours_WE_de_trop(Instance* inst) {
    // Ici on supprime les jours de week-end de trop pour chaque personne
    int i_Nb_WE_Plage_Horaire = inst->get_Nombre_Jour() / 7;
    for (int p = 0; p < inst->get_Nombre_Personne(); p++) {
        // Pour chaque personnes on détermine le nombre de week-ends à faire travailler
        int i_Nb_WE_A_Travailler = inst->get_Personne_Nbre_WE_Max(p);

        vector<int> i_Indices_WE_A_Travailler = {};
        for (int i_Boucle = 0; i_Boucle < i_Nb_WE_A_Travailler; i_Boucle++) {
            // Pour chaque week-end à faire travailler, on choisit un week-end aléatoire qui n'a pas encore été choisi
            int i_WE = rand() % i_Nb_WE_Plage_Horaire;
            while (std::find(i_Indices_WE_A_Travailler.begin(), i_Indices_WE_A_Travailler.end(), i_WE) != i_Indices_WE_A_Travailler.end()) {
                // Si le week-end a déjà été choisi, on en choisit un autre
                i_WE = rand() % i_Nb_WE_Plage_Horaire;
            }
            i_Indices_WE_A_Travailler.push_back(i_WE);
        }

        int compteur_WE = 0;
        for (int j = 0; j < inst->get_Nombre_Jour(); j++) {
            // Pour chaque jour, si c'est un week-end et que ce week-end n'a pas été choisi, on met des jours de repos
            if (j % 7 == 5) {
                if (std::find(i_Indices_WE_A_Travailler.begin(), i_Indices_WE_A_Travailler.end(), compteur_WE) == i_Indices_WE_A_Travailler.end()) {
                    // On met des jours de repos sur tout le week-end pour satisfaire la contrainte 
                    v_v_IdShift_Par_Personne_et_Jour[p][j] = -1;
                    v_v_IdShift_Par_Personne_et_Jour[p][j + 1] = -1;
                }
                compteur_WE++;
            }
        }
    }
}

void Ma_Solution::suppression_max_shifts_consecutifs(Instance* inst) {
    // On supprime les shifts consécutifs de trop pour chaque personne
    for (int p = 0; p < inst->get_Nombre_Personne(); p++) {
        int compteur_consecutif = 0;
        int max_consecutif = inst->get_Personne_Nbre_Shift_Consecutif_Max(p);
        // On récupere le nombre maximum de shifts consécutifs autorisés pour la personne p
        for (int j = 0; j < inst->get_Nombre_Jour(); j++) {
            if (v_v_IdShift_Par_Personne_et_Jour[p][j] != -1) {
                compteur_consecutif++;
                if (compteur_consecutif > max_consecutif) {
                    v_v_IdShift_Par_Personne_et_Jour[p][j] = -1;
                    compteur_consecutif = 0;
                    // Enfin on remet le compteur de jours consecutifs à 0 après avoir mis un jour de repos
                }
            }
        }
    }
}

void Ma_Solution::suppression_shifts_par_type_de_trop(Instance* inst) {
    // On veut supprimer les shifts par type de shift de trop pour chaque personne
    for (int p = 0; p < inst->get_Nombre_Personne(); p++) {
        vector<int> compteur_shifts = {};
        for (int i = 0; i < inst->get_Nombre_Shift(); i++) {
            compteur_shifts.push_back(0);
        }
        // On compte le nombre de shifts par type assignés à la personne p
        for (int j = 0; j < inst->get_Nombre_Jour(); j++) {
            int shift_actuel = v_v_IdShift_Par_Personne_et_Jour[p][j];
            if (shift_actuel != -1) {
                compteur_shifts[shift_actuel]++;
                int max_shift = inst->get_Personne_Shift_Nbre_Max(p, shift_actuel);
                // Si le nombre de shifts du type actuel dépasse le maximum autorisé, on remplace par un autre shift ou par un jour de repos
                if (compteur_shifts[shift_actuel] > max_shift) {
                    int shift_aleatoire = rand() % inst->get_Nombre_Shift();
                    int i_compteur_boucle = 0;
                    // On cherche un shift aléatoire différent du shift actuel et qui n'a pas encore atteint son maximum
                    while (shift_aleatoire == shift_actuel || compteur_shifts[shift_aleatoire] >= inst->get_Personne_Shift_Nbre_Max(p, shift_aleatoire)) {
                        shift_aleatoire = rand() % inst->get_Nombre_Shift();
                        i_compteur_boucle++;
                        if (i_compteur_boucle > 1000) {
                            shift_aleatoire = -1;
                            break;
                        }
                    }
                    // On remplace le shift actuel par le shift aléatoire trouvé ou par un jour de repos si aucun shift n'est disponible
                    v_v_IdShift_Par_Personne_et_Jour[p][j] = shift_aleatoire;
                    compteur_shifts[shift_aleatoire]++;
                    compteur_shifts[shift_actuel]--;
                }
            }
        }
    }
}

void Ma_Solution::Shift_succede(Instance* inst) {
    // On veut s'assurer que les shifts qui se succèdent sont possibles pour chaque personne
    size_t iNbPersonne = inst->get_Nombre_Personne();
    size_t iNbJour = inst->get_Nombre_Jour();
    // Pour chaque personne et chaque jour, on vérifie si le shift du jour précédent peut être suivi par le shift du jour actuel
    for (int iIndexPersonne = 0; iIndexPersonne < iNbPersonne; iIndexPersonne++) {
        for (int iIndexJour = 1; iIndexJour < iNbJour; iIndexJour++) {
            int prev = v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour - 1];
            int curr = v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour];
            // On cherche un shift possible qui peut succéder au shift précédent
            if (prev == -1 || curr == -1 || inst->is_possible_Shift_Succede(prev, curr)) {
                continue;
            }
            curr = 0;
            int compteur = std::count(v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne].begin(), v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne].end(), curr);
            while ((inst->is_possible_Shift_Succede(prev, curr) == false || compteur >= inst->get_Personne_Shift_Nbre_Max(iIndexPersonne, curr)) && curr < inst->get_Nombre_Shift() - 1) {
                curr++;
                compteur = std::count(v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne].begin(), v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne].end(), curr);
            }
            // Si aucun shift n'est possible, on met un jour de repos
            if (curr == inst->get_Nombre_Shift() - 1 && (inst->is_possible_Shift_Succede(prev, curr) == false || compteur >= inst->get_Personne_Shift_Nbre_Max(iIndexPersonne, curr))) {
                curr = -1;
            }
            // On met à jour le shift du jour actuel
            v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour] = curr;
        }
    }
}

void Ma_Solution::afficher_solution() {
    // Permet d'afficher la solution dans la console
    std::cout << u8"\n=== Solution générée ===\n";
    for (size_t p = 0; p < v_v_IdShift_Par_Personne_et_Jour.size(); p++) {
        std::cout << "Personne " << p << " : ";
        for (size_t j = 0; j < v_v_IdShift_Par_Personne_et_Jour[p].size(); j++) {
            std::cout << v_v_IdShift_Par_Personne_et_Jour[p][j] << " ";
        }
        std::cout << endl;
    }
}

void Ma_Solution::maximum_min_per_personne(Instance* inst) {
    // On veut s'assurer que chaque personne ne dépasse pas le maximum de minutes travaillées
    size_t iNbPersonne = inst->get_Nombre_Personne();
    size_t iNbJour = inst->get_Nombre_Jour();
    for (int iIndexPersonne = 0; iIndexPersonne < iNbPersonne; iIndexPersonne++) {
        int duree_travail = 0;
        int shift_aleatoire;
        int duree_shift_actuel = 0;
        int compteur_boucle = 0;
        // On calcule la durée totale de travail de la personne iIndexPersonne
        for (int iIndexJour = 0; iIndexJour < iNbJour; iIndexJour++) {
            int shift_actuel = v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour];
            if (shift_actuel != -1) {
                duree_travail += inst->get_Shift_Duree(shift_actuel);
            }
        }
        // Si la durée totale de travail dépasse le maximum autorisé, on retire des shifts aléatoires jusqu'à ce que la durée soit correcte
        while (duree_travail > inst->get_Personne_Duree_total_Max(iIndexPersonne)) {
            shift_aleatoire = rand() % inst->get_Nombre_Jour();
            while (v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][shift_aleatoire] == -1) {
                compteur_boucle++;
                shift_aleatoire = rand() % inst->get_Nombre_Jour();
                if (compteur_boucle > 1000) {
                    std::cout << u8"Erreur : pas de solution possible avec les contraintes de durée maximale de travail par personne." << endl;
                    return;
                }
            }
            // On retire le shift aléatoire et on met à jour la durée totale de travail
            duree_shift_actuel = inst->get_Shift_Duree(v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][shift_aleatoire]);
            v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][shift_aleatoire] = -1;
            duree_travail -= duree_shift_actuel;
        }
    }
}

void Ma_Solution::ajout_jours_de_repos_consecutif(Instance* inst) {
	// On veut ajouter des jours de repos consécutifs pour chaque personne
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
    // On veut vérifier la contrainte du maximum de week-ends travaillés pour chaque personne
    size_t iNbPersonne = inst->get_Nombre_Personne();
    size_t iNbJour = inst->get_Nombre_Jour();
    // Pour chaque personne, on compte le nombre de week-ends travaillés
    for (int iIndexPersonne = 0; iIndexPersonne < iNbPersonne; iIndexPersonne++) {
        int max_we = inst->get_Personne_Nbre_WE_Max(iIndexPersonne);
        int compteur_we = 0;
        for (int iIndexJour = 0; iIndexJour < iNbJour; iIndexJour++) {
            // Si c'est un samedi, on vérifie si la personne travaille ce week-end
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
    // On veut vérifier la contrainte du minimum de jours de repos consécutifs pour chaque personne
    size_t iNbPersonne = inst->get_Nombre_Personne();
    size_t iNbJour = inst->get_Nombre_Jour();
    // Pour chaque personne, on compte les jours de repos consécutifs
    for (int iIndexPersonne = 0; iIndexPersonne < iNbPersonne; iIndexPersonne++) {
        // On récupère le minimum de jours de repos consécutifs pour la personne iIndexPersonne pour vérifier qu'on respecte cette contrainte
        int min_repos = inst->get_Personne_Jour_OFF_Consecutif_Min(iIndexPersonne);
        int compteur_repos = 0;
        for (int iIndexJour = 0; iIndexJour < iNbJour; iIndexJour++) {
            // On compte les jours de repos consécutifs
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
    // On veut vérifier la contrainte du maximum de shifts consécutifs pour chaque personne
    size_t iNbPersonne = inst->get_Nombre_Personne();
    size_t iNbJour = inst->get_Nombre_Jour();
    // Pour chaque personne, on compte les shifts consécutifs pour vérifier qu'on respecte cette contrainte
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
    // On veut vérifier la contrainte du maximum de minutes travaillées par personne
    size_t iNbPersonne = inst->get_Nombre_Personne();
    size_t iNbJour = inst->get_Nombre_Jour();
    // Pour chaque personne, on compte le nombre de minutes travaillées pour vérifier qu'on respecte cette contrainte
    for (int iIndexPersonne = 0; iIndexPersonne < iNbPersonne; iIndexPersonne++) {
        int compteur_minute = 0;
        int nbMaxMinutes = inst->get_Personne_Duree_total_Max(iIndexPersonne);
        for (int iIndexJour = 0; iIndexJour < iNbJour; iIndexJour++) {
            if (v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour] != -1) {
                compteur_minute += inst->get_Shift_Duree(v_v_IdShift_Par_Personne_et_Jour[iIndexPersonne][iIndexJour]);
            }
            if (compteur_minute > nbMaxMinutes) {
                std::cout << "Personne " << iIndexPersonne << " compteur_minute : " << compteur_minute << " maximum : " << nbMaxMinutes << "\n";
                return false;
            }
        }
    }
    return true;
}

bool Ma_Solution::check_shift_succede(Instance* inst) {
    // On veut vérifier la contrainte des shifts qui doivent être suivis par un shift spécifique pour chaque personne
    size_t iNbPersonne = inst->get_Nombre_Personne();
    size_t iNbJour = inst->get_Nombre_Jour();
    // Pour chaque personne et chaque jour, on vérifie si le shift du jour précédent peut être suivi par le shift du jour actuel pour vérifier qu'on respecte cette contrainte
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
    // On veut vérifier la contrainte des congés pour chaque personne
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
    // On veut vérifier la contrainte du minimum de shifts consécutifs pour chaque personne
    for (int p = 0; p < inst->get_Nombre_Personne(); p++) {
        int compteur_consecutif = 0;
        for (int j = 0; j < inst->get_Nombre_Jour(); j++) {
            if (v_v_IdShift_Par_Personne_et_Jour[p][j] != -1) {
                compteur_consecutif++;
            }
            // Si on rencontre un jour de repos, on vérifie si le compteur de shifts consécutifs est inférieur au minimum requis
            else {
                if (compteur_consecutif > 0 && compteur_consecutif < inst->get_Personne_Nbre_Shift_Consecutif_Min(p)) {
                    std::cout << "Personne " << p << " jour " << j << " compteur_consecutif " << compteur_consecutif << "\n";
                    return false;
                }
                compteur_consecutif = 0;
            }
        }
        if (compteur_consecutif > 0 && compteur_consecutif < inst->get_Personne_Nbre_Shift_Consecutif_Min(p)) {
            std::cout << "Personne " << p << " dernier jour compteur_consecutif " << compteur_consecutif << "\n";
            return false;
        }
    }
    return true;
}

bool Ma_Solution::check_min_minutes_travailees(Instance* inst) {
    // On veut vérifier la contrainte du minimum de minutes travaillées pour chaque personne
    for (int p = 0; p < inst->get_Nombre_Personne(); p++) {
        int duree_travail = 0;
        // On calcule la durée totale de travail de la personne p
        for (int j = 0; j < inst->get_Nombre_Jour(); j++) {
            int shift_actuel = v_v_IdShift_Par_Personne_et_Jour[p][j];
            if (shift_actuel != -1) {
                duree_travail += inst->get_Shift_Duree(shift_actuel);
            }
        }
        // On vérifie si la durée totale de travail est inférieure au minimum requis
        if (duree_travail < inst->get_Personne_Duree_total_Min(p)) {
            std::cout << "Personne " << p << " duree_travail " << duree_travail << " minimum : " << inst->get_Personne_Duree_total_Min(p) << "\n";
            return false;
        }
    }
    return true;
}

bool Ma_Solution::check_max_assignable_shifts(Instance* inst) {
    // On veut vérifier la contrainte du maximum de shifts assignables par type de shift pour chaque personne
    for (int p = 0; p < inst->get_Nombre_Personne(); p++) {
        vector<int> compteur_shifts(inst->get_Nombre_Shift(), 0);
        for (int j = 0; j < inst->get_Nombre_Jour(); j++) {
            int shift_actuel = v_v_IdShift_Par_Personne_et_Jour[p][j];
            // On compte le nombre de shifts par type assignés à la personne p
            if (shift_actuel != -1) {
                compteur_shifts[shift_actuel]++;
                // On vérifie si le nombre de shifts du type actuel dépasse le maximum autorisé
                if (compteur_shifts[shift_actuel] > inst->get_Personne_Shift_Nbre_Max(p, shift_actuel)) {
                    return false;
                }
            }
        }
    }
    return true;
}

int Ma_Solution::check_solution(Instance* inst) {
    // On vérifie toutes les contraintes et on compte le nombre de contraintes respectées
    int i_Nb_Contrainte_Respectees = 1;

    if (this->check_conges(inst) == true) {
        std::cout << "Conge OK\n";
        i_Nb_Contrainte_Respectees++;
    }
    else {
        std::cout << "Conge NOT OK\n";
    }

    if (this->check_min_consecutif_shifts(inst) == true) {
        std::cout << "Min consecutif shifts OK\n";
        i_Nb_Contrainte_Respectees++;
    }
    else {
        std::cout << "Min consecutif shifts NOT OK\n";
    }

    if (this->check_min_minutes_travailees(inst) == true) {
        std::cout << "Min minutes travaillees OK\n";
        i_Nb_Contrainte_Respectees++;
    }
    else {
        std::cout << "Min minutes travaillees NOT OK\n";
    }

    if (this->check_max_assignable_shifts(inst) == true) {
        std::cout << "Max assignable shifts OK\n";
        i_Nb_Contrainte_Respectees++;
    }
    else {
        std::cout << "Max assignable shifts NOT OK\n";
    }

    if (this->check_max_we(inst) == true) {
        std::cout << "Max WE OK\n";
        i_Nb_Contrainte_Respectees++;
    }
    else {
        std::cout << "Max WE NOT OK\n";
    }

    if (this->check_min_repos_consecutif(inst) == true) {
        std::cout << "Min repos consecutif OK\n";
        i_Nb_Contrainte_Respectees++;
    }
    else {
        std::cout << "Min repos consecutif NOT OK\n";
    }

    if (this->check_max_shift_consecutif(inst) == true) {
        std::cout << "Max shift consecutif OK\n";
        i_Nb_Contrainte_Respectees++;
    }
    else {
        std::cout << "Max shift consecutif NOT OK\n";
    }

    if (this->check_max_minutes_per_personne(inst) == true) {
        std::cout << "Max minutes par personne OK\n";
        i_Nb_Contrainte_Respectees++;
    }
    else {
        std::cout << "Max minutes par personne NOT OK\n";
    }

    if (this->check_shift_succede(inst) == true) {
        std::cout << "Shift succede OK\n";
        i_Nb_Contrainte_Respectees++;
    }
    else {
        std::cout << "Shift succede NOT OK\n";
    }

    return i_Nb_Contrainte_Respectees;
}

vector<vector<int>> Ma_Solution::creation_Solution_Initiale(Instance* inst) {
    // Création d'une solution initiale en respectant les contraintes
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
        std::cout << u8"Nombre de contraintes respectées : " << i_Nb_Contraintes_Respectees << " / 10\n";
        return v_v_IdShift_Par_Personne_et_Jour;
    }


bool Ma_Solution::Verifie_Neuf_Contraintes(Instance* inst, int ligne_a_verifier) {
	//Reprise du code de Solution::Verification_Solution, adapté pour une seule ligne et sans la contrainte sur la durée minimale travaillée (qui sera la 10ème contrainte à vérifier après execution de cette fonction dans la métaheuristique)
    
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

	// Vérification du nombre minimum de jours off consécutifs
    int min_jours_off = inst->get_Personne_Jour_OFF_Consecutif_Min(ligne_a_verifier);
    if (min_jours_off > 1) {
        int i = 0;
        while (i < v_v_IdShift_Par_Personne_et_Jour[ligne_a_verifier].size()) {
            if (v_v_IdShift_Par_Personne_et_Jour[ligne_a_verifier][i] == -1) {
                int debut_sequence = i;
                int nb_jours_off_consecutifs = 0;

                while (i < v_v_IdShift_Par_Personne_et_Jour[ligne_a_verifier].size() &&
                    v_v_IdShift_Par_Personne_et_Jour[ligne_a_verifier][i] == -1) {
                    nb_jours_off_consecutifs++;
                    i++;
                }

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
    // On veut générer un voisin en permutant deux shifts dans la ligne à modifier, jusqu'à ce que les 9 premières contraintes soient respectées (sans la contrainte sur la durée minimale travaillée)
    vector<int> v_Nouvelle_Ligne = v_v_IdShift_Par_Personne_et_Jour[ligne_a_modifier];

    vector<int> v_Ligne_Originale = v_v_IdShift_Par_Personne_et_Jour[ligne_a_modifier];

    int compteur_tentatives = 0;
    int max_tentatives = 1000; 
    int compteur_max_tentatives_atteint = 0;


    while (!this->Verifie_Neuf_Contraintes(inst, ligne_a_modifier) && compteur_max_tentatives_atteint < 2) {
        v_v_IdShift_Par_Personne_et_Jour[ligne_a_modifier] = v_Nouvelle_Ligne;
        // On prend deux indices aléatoires dans la ligne
        int indice1 = rand() % v_Nouvelle_Ligne.size();
        int indice2 = rand() % v_Nouvelle_Ligne.size();

        while (indice2 == indice1) {
            indice2 = rand() % v_Nouvelle_Ligne.size();
        }
        // On permute les deux shifts aux indices choisis
        int temp = v_Nouvelle_Ligne[indice1];
        v_Nouvelle_Ligne[indice1] = v_Nouvelle_Ligne[indice2];
        v_Nouvelle_Ligne[indice2] = temp;

        compteur_tentatives++;
        // Si on a atteint le nombre maximal de tentatives, on réinitialise la ligne à son état original
        if (compteur_tentatives > max_tentatives) {
            v_Nouvelle_Ligne = v_Ligne_Originale;
            compteur_tentatives = 0;
			compteur_max_tentatives_atteint++;
            std::cout << "Attention : nombre maximal de tentatives atteint pour la ligne "
                << ligne_a_modifier << u8", redémarrage...\n";
        }
    }

    v_v_IdShift_Par_Personne_et_Jour[ligne_a_modifier] = v_Ligne_Originale;

    return v_Nouvelle_Ligne;
}

vector<int> Ma_Solution::Genere_Ligne_Voisine_Minimum_Min_Travaille(Instance* inst, int ligne_a_modifier) {
    // On veut générer un voisin en remplaçant des jours de repos par des shifts en respectant les contraintes de maximul de shift par type de trop et de possibilité de travailler sur un shift en fonction du shift précedent/suivant, jusqu'à ce que toutes les contraintes soient respectées
    vector<int> v_Nouvelle_Ligne = v_v_IdShift_Par_Personne_et_Jour[ligne_a_modifier];
    vector<int> v_Ligne_Originale = v_v_IdShift_Par_Personne_et_Jour[ligne_a_modifier];

    // On trouve tous les jours de repos 
    vector<int> v_indices_repos;
    for (int i = 0; i < v_Nouvelle_Ligne.size(); i++) {
        if (v_Nouvelle_Ligne[i] == -1) {
            v_indices_repos.push_back(i);
        }
    }

    // Calculer la durée actuelle
    int duree_actuelle = 0;
    for (int j = 0; j < v_Nouvelle_Ligne.size(); j++) {
        if (v_Nouvelle_Ligne[j] != -1) {
            duree_actuelle += inst->get_Shift_Duree(v_Nouvelle_Ligne[j]);
        }
    }

    int duree_manquante = inst->get_Personne_Duree_total_Min(ligne_a_modifier) - duree_actuelle;

    // Si on a déjà le minimum, pas besoin de modifier
    if (duree_manquante <= 0) {
        return v_Nouvelle_Ligne;
    }

    int compteur_tentatives = 0;
    int max_tentatives = 1000;
    int compteur_max_tentatives_atteint = 0;

    while (!this->Verifie_Dix_Contraintes(inst, ligne_a_modifier) && compteur_max_tentatives_atteint < 1) {
        v_v_IdShift_Par_Personne_et_Jour[ligne_a_modifier] = v_Nouvelle_Ligne;

        // Calculer combien de shifts minimum on doit ajouter
        int duree_temp = 0;
        vector<int> v_i_Nb_shift(inst->get_Nombre_Shift(), 0);

        for (int j = 0; j < v_Nouvelle_Ligne.size(); j++) {
            if (v_Nouvelle_Ligne[j] != -1) {
                duree_temp += inst->get_Shift_Duree(v_Nouvelle_Ligne[j]);
                v_i_Nb_shift[v_Nouvelle_Ligne[j]]++;
            }
        }

        int manque = inst->get_Personne_Duree_total_Min(ligne_a_modifier) - duree_temp;
        // Si on a encore du manque de temps minimum et des jours de repos disponibles
        if (manque > 0 && v_indices_repos.size() > 0) {
            // Choisir un jour de repos aléatoire
            int idx_random = rand() % v_indices_repos.size();
            int jour_a_remplacer = v_indices_repos[idx_random];

            // Trouver les shifts disponibles (pas au max + peuvent se suivre)
            vector<int> shifts_disponibles;

            for (int s = 0; s < inst->get_Nombre_Shift(); s++) {
                // Vérifier si le shift n'a pas atteint son max
                if (v_i_Nb_shift[s] < inst->get_Personne_Shift_Nbre_Max(ligne_a_modifier, s)) {
                    bool peut_se_placer = true;

                    // Vérifier la succession avec le shift précédent
                    if (jour_a_remplacer > 0 && v_Nouvelle_Ligne[jour_a_remplacer - 1] != -1) {
                        if (!inst->is_possible_Shift_Succede(v_Nouvelle_Ligne[jour_a_remplacer - 1], s)) {
                            peut_se_placer = false;
                        }
                    }

                    // Vérifier la succession avec le shift suivant
                    if (jour_a_remplacer < v_Nouvelle_Ligne.size() - 1 && v_Nouvelle_Ligne[jour_a_remplacer + 1] != -1) {
                        if (!inst->is_possible_Shift_Succede(s, v_Nouvelle_Ligne[jour_a_remplacer + 1])) {
                            peut_se_placer = false;
                        }
                    }

                    if (peut_se_placer) {
                        shifts_disponibles.push_back(s);
                    }
                }
            }

            // Si on a des shifts disponibles, en choisir un aléatoirement
            if (shifts_disponibles.size() > 0) {
                int shift_choisi = shifts_disponibles[rand() % shifts_disponibles.size()];
                v_Nouvelle_Ligne[jour_a_remplacer] = shift_choisi;

                //cout << "Ajout du shift " << shift_choisi << " au jour "
                    //<< jour_a_remplacer << "\n";

                // Retirer cet indice de la liste des repos disponibles
                v_indices_repos.erase(v_indices_repos.begin() + idx_random);
            }
            else {
                // Aucun shift disponible pour ce jour, essayer un autre jour
                //cout << "Aucun shift disponible pour le jour " << jour_a_remplacer << "\n";
            }
        }
        else {
            // Sinon, permuter deux éléments aléatoires car on ne peut plus ajouter de shifts
            int indice1 = rand() % v_Nouvelle_Ligne.size();
            int indice2 = rand() % v_Nouvelle_Ligne.size();

            while (indice2 == indice1) {
                indice2 = rand() % v_Nouvelle_Ligne.size();
            }

            int temp = v_Nouvelle_Ligne[indice1];
            v_Nouvelle_Ligne[indice1] = v_Nouvelle_Ligne[indice2];
            v_Nouvelle_Ligne[indice2] = temp;
        }

        compteur_tentatives++;

        if (compteur_tentatives > max_tentatives) {
            v_Nouvelle_Ligne = v_Ligne_Originale;
            compteur_tentatives = 0;
			compteur_max_tentatives_atteint++;

            // Recalculer les indices de repos
            v_indices_repos.clear();
            for (int i = 0; i < v_Nouvelle_Ligne.size(); i++) {
                if (v_Nouvelle_Ligne[i] == -1) {
                    v_indices_repos.push_back(i);
                }
            }

            std::cout << "Attention : nombre maximal de tentatives atteint pour la ligne "
                << ligne_a_modifier << u8", redémarrage...\n";
        }
    }

    v_v_IdShift_Par_Personne_et_Jour[ligne_a_modifier] = v_Ligne_Originale;
    return v_Nouvelle_Ligne;
}

void Ma_Solution::MetaHeuristique_Recherche_Local(Instance* inst) {
    // Métaheuristique de recherche locale pour améliorer la solution en respectant les contraintes
    int Meilleur_Score = this->check_solution(inst);

	int precScoreLignes = 0;  // On initialise le score des lignes vérifiant 9 contraintes
    for (int ligne = 0; ligne < inst->get_Nombre_Personne(); ligne++) {
        if (this->Verifie_Neuf_Contraintes(inst, ligne)) {
            precScoreLignes++;  // Le score s'incrémente dès qu'une ligne vérifie 9 contraintes
        }
    }
	int compteurStagnation = 0; // Compteur pour suivre les itérations sans amélioration en nombre de lignes vérifiant 9 contraintes
	int maxStagnation = 500;

    while (Meilleur_Score < 9 && compteurStagnation < maxStagnation) {
        bool progression = false;

        for (int ligne = 0; ligne < inst->get_Nombre_Personne(); ligne++) {

            if (this->Verifie_Neuf_Contraintes(inst, ligne)) {
                std::cout << "Ligne " << ligne << u8" déjà OK, on passe à la suivante.\n";
                continue;
            }

            std::cout << "Ligne " << ligne << " ne respecte pas les 9 contraintes, tentative de correction...\n";

            vector<int> v_Ligne_Avant = v_v_IdShift_Par_Personne_et_Jour[ligne];

            vector<int> v_Nouvelle_Ligne = this->Genere_Ligne_Voisine_Consecutifs_Shifts(inst, ligne);

            v_v_IdShift_Par_Personne_et_Jour[ligne] = v_Nouvelle_Ligne;

            // VÉRIFIER que la nouvelle ligne respecte bien les 9 contraintes
            if (this->Verifie_Neuf_Contraintes(inst, ligne)) {
                std::cout << "Ligne " << ligne << u8" corrigée avec succès !\n";
                progression = true;
            }
            else {
                std::cout << u8"Échec de correction pour la ligne " << ligne << ", restauration...\n";
                v_v_IdShift_Par_Personne_et_Jour[ligne] = v_Ligne_Avant; // Restaurer
            }
        }

        // Recalculer le score après avoir traité toutes les lignes
        int Nouveau_Score = this->check_solution(inst);
        int scoreLignes = 0;
        for (int ligne = 0; ligne < inst->get_Nombre_Personne(); ligne++) {
            if (this->Verifie_Neuf_Contraintes(inst, ligne)) {
                scoreLignes++;
            }
        }

        if (scoreLignes == precScoreLignes) {
            compteurStagnation++;
        }
        else {
            compteurStagnation = 0;
			precScoreLignes = scoreLignes;
        }
        std::cout << u8"\n=== Score après itération : " << Nouveau_Score << " / 10 ===\n";
        std::cout << u8"=== Nb personnes respectant les 9 contraintes : " << scoreLignes << " / " << inst->get_Nombre_Personne() << " === \n";
        if (compteurStagnation > 0) {
            std::cout << u8"=== Stagnation détectée : " << compteurStagnation << "/" << maxStagnation << u8" itérations sans amélioration === \n";
        }
        
        
        Meilleur_Score = Nouveau_Score;
    }
    // On initialise le score des lignes vérifiant 9 contraintes
    int precScoreLignes10 = 0; 
    for (int ligne = 0; ligne < inst->get_Nombre_Personne(); ligne++) {
        if (this->Verifie_Dix_Contraintes(inst, ligne)) {
            // Le score s'incrémente dès qu'une ligne vérifie 9 contraintes
            precScoreLignes10++;  
        }
    }
    // Compteur pour suivre les itérations sans amélioration en nombre de lignes vérifiant 9 contraintes
    int compteurStagnation10 = 0; 


    while (Meilleur_Score < 10 && compteurStagnation10 <maxStagnation) {
        bool progression = false;

        for (int ligne = 0; ligne < inst->get_Nombre_Personne(); ligne++) {

            if (this->Verifie_Dix_Contraintes(inst, ligne)) {
                std::cout << "Ligne " << ligne << u8" déjà OK, on passe à la suivante.\n";
                continue;
            }

            std::cout << "Ligne " << ligne << " ne respecte pas les 10 contraintes, tentative de correction...\n";

            vector<int> v_Ligne_Avant = v_v_IdShift_Par_Personne_et_Jour[ligne];

            vector<int> v_Nouvelle_Ligne = this->Genere_Ligne_Voisine_Minimum_Min_Travaille(inst, ligne);

            v_v_IdShift_Par_Personne_et_Jour[ligne] = v_Nouvelle_Ligne;

            // VÉRIFIER que la nouvelle ligne respecte bien les 10 contraintes
            if (this->Verifie_Dix_Contraintes(inst, ligne)) {
                std::cout << "Ligne " << ligne << u8" corrigée avec succès !\n";
                progression = true;
            }
            else {
                std::cout << u8"Échec de correction pour la ligne " << ligne << ", restauration...\n";
                v_v_IdShift_Par_Personne_et_Jour[ligne] = v_Ligne_Avant; // Restaurer
            }
        }

        // Recalculer le score après avoir traité toutes les lignes
        int Nouveau_Score = this->check_solution(inst);
        int scoreLignes10 = 0;
        for (int ligne = 0; ligne < inst->get_Nombre_Personne(); ligne++) {
            if (this->Verifie_Dix_Contraintes(inst, ligne)) {
                scoreLignes10++;
            }
        }

		// Suivi de la stagnation
        if(scoreLignes10 == precScoreLignes10) {
            compteurStagnation10 ++;
        } else {
            compteurStagnation10 = 0;
			precScoreLignes10 = scoreLignes10;
		}

        std::cout << u8"\n=== Score après itération : " << Nouveau_Score << " / 10 ===\n";
        std::cout << u8"=== Nb personnes respectant les 9 contraintes : " << precScoreLignes << " / " << inst->get_Nombre_Personne() << " === \n";
        std::cout << u8"=== Nb personnes respectant les 10 contraintes : " << scoreLignes10 << " / " << inst->get_Nombre_Personne() <<" === \n";
        if(compteurStagnation10 > 0 ){
            std::cout << u8"=== Stagnation détectée : " << compteurStagnation10 << "/" << maxStagnation << u8" itérations sans amélioration === \n";
        }


        Meilleur_Score = Nouveau_Score;
    }

    std::cout << u8"\n=== Métaheuristique terminée avec score final : " << Meilleur_Score << " / 10 ===\n";
}