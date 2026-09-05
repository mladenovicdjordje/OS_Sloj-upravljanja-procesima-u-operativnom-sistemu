#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <limits>
using namespace std;

// Struktura koja predstavlja jedan proces u operativnom sistemu
struct Proces {
    int pid;            
    string naziv;      
    int prioritet;      
    int trajanje;       
    Proces* sledeci;    
};

// Ucitava ceo broj i ponavlja unos dok korisnik ne unese ispravnu vrednost
int unesiCeoBroj(const string& poruka) {
    int broj;
    cout << poruka;
    while (!(cin >> broj)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Neispravan unos. Unesite ceo broj: ";
    }
    return broj;
}

// Dodaje novi proces na kraj jednostruko spregnute liste
void dodajProces(Proces*& glava, Proces*& rep, int pid, string naziv, int prioritet, int trajanje) {
    Proces* novi = new Proces;
    novi->pid = pid;
    novi->naziv = naziv;
    novi->prioritet = prioritet;
    novi->trajanje = trajanje;
    novi->sledeci = nullptr;

    if (glava == nullptr) {       // lista je prazna
        glava = novi;
        rep = novi;
    } else {                      // dodavanje na kraj liste
        rep->sledeci = novi;
        rep = novi;
    }
}

// Proverava da li proces sa datim PID-om vec postoji u listi
bool pidPostoji(Proces* glava, int pid) {
    Proces* tekuci = glava;
    while (tekuci != nullptr) {
        if (tekuci->pid == pid) {
            return true;
        }
        tekuci = tekuci->sledeci;
    }
    return false;
}

// Brise proces sa zadatim PID-om
bool obrisiProcesPoPID(Proces*& glava, Proces*& rep, int pid) {
    if (glava == nullptr) {
        return false;
    }

    if (glava->pid == pid) {
        Proces* zaBrisanje = glava;
        glava = glava->sledeci;
        if (glava == nullptr) {
            rep = nullptr;
        }
        delete zaBrisanje;
        return true;
    }

    Proces* prethodni = glava;
    Proces* tekuci = glava->sledeci;
    while (tekuci != nullptr) {
        if (tekuci->pid == pid) {
            prethodni->sledeci = tekuci->sledeci;
            if (tekuci == rep) {
                rep = prethodni;
            }
            delete tekuci;
            return true;
        }
        prethodni = tekuci;
        tekuci = tekuci->sledeci;
    }

    return false;
}

// Oslobadja memoriju cele liste
void oslobodiListu(Proces*& glava, Proces*& rep) {
    Proces* pomocni;
    while (glava != nullptr) {
        pomocni = glava;
        glava = glava->sledeci;
        delete pomocni;
    }
    rep = nullptr;
}

// Pravi radnu kopiju liste da simulacija ne menja originalne procese
void kopirajListu(Proces* izvor, Proces*& novaGlava, Proces*& noviRep) {
    novaGlava = nullptr;
    noviRep = nullptr;
    Proces* tekuci = izvor;
    while (tekuci != nullptr) {
        dodajProces(novaGlava, noviRep, tekuci->pid, tekuci->naziv, tekuci->prioritet, tekuci->trajanje);
        tekuci = tekuci->sledeci;
    }
}

// Unosi vise procesa u listu, sve dok korisnik ne odluci da prekine unos
void unosProcesa(Proces*& glava, Proces*& rep) {
    char nastavi;
    do {
        int pid, prioritet, trajanje;
        string naziv;

        pid = unesiCeoBroj("\nUnesite PID procesa: ");
        while (pid <= 0 || pidPostoji(glava, pid)) {
            if (pid <= 0) {
                pid = unesiCeoBroj("PID mora biti veci od 0. Unesite drugi PID: ");
            } else {
                cout << "Proces sa izabranim PID-om " << pid << " vec postoji. ";
                pid = unesiCeoBroj("Unesite drugi PID: ");
            }
        }

        cout << "Unesite naziv procesa (bez razmaka): ";
        cin >> naziv;

        prioritet = unesiCeoBroj("Unesite prioritet procesa (1 = najvisi prioritet): ");
        while (prioritet < 1 || prioritet > 5) {
            prioritet = unesiCeoBroj("Prioritet mora biti od 1 do 5. Unesite ponovo: ");
        }

        trajanje = unesiCeoBroj("Unesite trajanje procesa (broj vremenskih jedinica): ");
        while (trajanje < 1) {
            trajanje = unesiCeoBroj("Trajanje mora biti najmanje 1. Unesite ponovo: ");
        }

        dodajProces(glava, rep, pid, naziv, prioritet, trajanje);

        cout << "Da li zelite da unesete jos jedan proces? (d/n): ";
        cin >> nastavi;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        while (nastavi != 'd' && nastavi != 'D' && nastavi != 'n' && nastavi != 'N') {
            cout << "Uneli ste pogresan karakter. Da li zelite da unesete jos jedan proces? (d/n): ";
            cin >> nastavi;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (nastavi == 'd' || nastavi == 'D');
}

// Prikazuje sve procese iz liste na ekranu
void prikaziListu(Proces* glava) {
    if (glava == nullptr) {
        cout << "\nLista procesa je prazna.\n";
        return;
    }

    cout << "\n-----------------------------------------------------------------\n";
    cout << left << setw(10) << "PID" << setw(30) << "Naziv" << setw(12) << "Prioritet" << setw(12) << "Trajanje" << "\n";
    cout << "-----------------------------------------------------------------\n";

    Proces* tekuci = glava;
    while (tekuci != nullptr) {
        cout << left << setw(10) << tekuci->pid << setw(30) << tekuci->naziv
             << setw(12) << tekuci->prioritet << setw(12) << tekuci->trajanje << "\n";
        tekuci = tekuci->sledeci;
    }
    cout << "-----------------------------------------------------------------\n";
}

// Prolazi kroz listu i odjednom snima sve procese u tekstualnu datoteku
void snimiUDatoteku(Proces* glava, const string& imeDatoteke) {
    ofstream izlaznaDatoteka(imeDatoteke);

    if (!izlaznaDatoteka) {
        cout << "\nGreska: datoteka ne moze da se otvori.\n";
        return;
    }

    Proces* tekuci = glava;
    while (tekuci != nullptr) {
        izlaznaDatoteka << tekuci->pid << " " << tekuci->naziv << " "
                        << tekuci->prioritet << " " << tekuci->trajanje << "\n";
        tekuci = tekuci->sledeci;
    }

    izlaznaDatoteka.close();
    cout << "\nSvi procesi su uspesno snimljeni u datoteku \"" << imeDatoteke << "\".\n";
}

// Ucitava procese iz datoteke u listu
void ucitajIzDatoteke(Proces*& glava, Proces*& rep, const string& imeDatoteke) {
    ifstream ulaznaDatoteka(imeDatoteke);

    if (!ulaznaDatoteka) {
        cout << "\nGreska: datoteka \"" << imeDatoteke << "\" ne postoji.\n";
        return;
    }

    oslobodiListu(glava, rep);

    int pid, prioritet, trajanje;
    string naziv;
    int ucitano = 0;
    int preskoceno = 0;

    while (ulaznaDatoteka >> pid >> naziv >> prioritet >> trajanje) {
        if (pid <= 0 || prioritet < 1 || trajanje < 1 || pidPostoji(glava, pid)) {
            preskoceno++;
            continue;
        }
        dodajProces(glava, rep, pid, naziv, prioritet, trajanje);
        ucitano++;
    }

    ulaznaDatoteka.close();
    cout << "\nProcesi su ucitani iz datoteke \"" << imeDatoteke << "\".\n";
    cout << "Ucitano: " << ucitano << ", preskoceno (neispravan red ili dupli PID): " << preskoceno << ".\n";
}

// Trazi PID od korisnika i brise odgovarajuci proces iz liste
void obrisiProces(Proces*& glava, Proces*& rep) {
    if (glava == nullptr) {
        cout << "\nLista procesa je prazna.\n";
        return;
    }

    int pid = unesiCeoBroj("\nUnesite PID procesa koji zelite da obrisete: ");
    if (obrisiProcesPoPID(glava, rep, pid)) {
        cout << "Proces sa PID-om " << pid << " je obrisan.\n";
    } else {
        cout << "Proces sa PID-om " << pid << " ne postoji u listi.\n";
    }
}

void fifoSimulacija(Proces* glava) {
    if (glava == nullptr) {
        cout << "\nNema procesa za simulaciju.\n";
        return;
    }

    cout << "\n===== FIFO simulacija izvrsavanja procesa =====\n";
    int trenutnoVreme = 0;
    Proces* tekuci = glava;

    while (tekuci != nullptr) {
        cout << "Vreme " << trenutnoVreme << " - " << (trenutnoVreme + tekuci->trajanje)
             << ": izvrsava se proces PID=" << tekuci->pid
             << " (" << tekuci->naziv << "), trajanje " << tekuci->trajanje << "\n";
        trenutnoVreme += tekuci->trajanje;
        tekuci = tekuci->sledeci;
    }

    cout << "Ukupno vreme izvrsavanja svih procesa: " << trenutnoVreme << " jedinica.\n";
    cout << "================================================\n";
}

void rrSimulacija(Proces* glava) {
    if (glava == nullptr) {
        cout << "\nNema procesa za simulaciju.\n";
        return;
    }

    int kvant = unesiCeoBroj("\nUnesite kvant vremena: ");
    while (kvant < 1) {
        kvant = unesiCeoBroj("Kvant mora biti najmanje 1. Unesite ponovo: ");
    }

    Proces* redGlava = nullptr;
    Proces* redRep = nullptr;
    kopirajListu(glava, redGlava, redRep);

    cout << "\n===== Round Robin simulacija (kvant = " << kvant << ") =====\n";
    int trenutnoVreme = 0;

    while (redGlava != nullptr) {
        Proces* p = redGlava;
        redGlava = redGlava->sledeci;
        if (redGlava == nullptr) {
            redRep = nullptr;
        }
        p->sledeci = nullptr;

        int odsecak = (p->trajanje < kvant) ? p->trajanje : kvant;

        cout << "Vreme " << trenutnoVreme << " - " << (trenutnoVreme + odsecak)
             << ": izvrsava se proces PID=" << p->pid
             << " (" << p->naziv << "), preostalo pre ovog kvanta: " << p->trajanje << "\n";

        trenutnoVreme += odsecak;
        p->trajanje -= odsecak;

        if (p->trajanje > 0) {
            if (redGlava == nullptr) {
                redGlava = p;
                redRep = p;
            } else {
                redRep->sledeci = p;
                redRep = p;
            }
        } else {
            cout << "  Proces PID=" << p->pid << " je zavrsen u trenutku " << trenutnoVreme << ".\n";
            delete p;
        }
    }

    cout << "Ukupno vreme izvrsavanja svih procesa: " << trenutnoVreme << " jedinica.\n";
    cout << "=======================================================\n";
}

// Glavni meni programa
void prikaziMeni() {
    cout << "\n===== SLOJ UPRAVLJANJA PROCESIMA =====\n";
    cout << "1. Unos procesa u listu\n";
    cout << "2. Obrisi proces po PID-u\n";
    cout << "3. Prikaz liste procesa\n";
    cout << "4. Snimi listu procesa u datoteku\n";
    cout << "5. Ucitaj procese iz datoteke\n";
    cout << "6. Pokreni FIFO simulaciju\n";
    cout << "7. Pokreni Round Robin simulaciju\n";
    cout << "8. Izlaz iz programa\n";
    cout << "Vas izbor: ";
}

int main() {
    Proces* glava = nullptr;
    Proces* rep = nullptr;
    const string imeDatoteke = "procesi.txt";

    int izbor;
    do {
        prikaziMeni();
        izbor = unesiCeoBroj("");

                switch (izbor) {
            case 1:
                unosProcesa(glava, rep);
                break;
            case 2:
                obrisiProces(glava, rep);
                break;
            case 3:
                prikaziListu(glava);
                break;
            case 4:
                snimiUDatoteku(glava, imeDatoteke);
                break;
            case 5:
                ucitajIzDatoteke(glava, rep, imeDatoteke);
                prikaziListu(glava);
                break;
            case 6:
                fifoSimulacija(glava);
                break;
            case 7:
                rrSimulacija(glava);
                break;
            case 8:
                cout << "\nZavrsetak programa.\n";
                break;
            default:
                cout << "\nNepostojeca opcija, pokusajte ponovo.\n";
        }
    } while (izbor != 8);

    oslobodiListu(glava, rep);
    return 0;
}