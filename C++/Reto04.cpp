#include <iostream>
#include <string>
#include <cctype>
using namespace std;

struct Sospechoso {
    int id;
    string nombre;
    string cabello; 
    char genero; 
    bool anteojos;
    bool tieneBarbaBigote;
    string tipoBarbaBigote;  
    string colorBarbaBigote; 
};

struct Nodo {
    Sospechoso dato;
    Nodo* siguiente;
};

bool barbaColorConocido = false;
string colorBarbaConocido = "";

bool cabelloColorConocido = false;
string colorCabelloConocido = "";

int preguntasRealizadas = 0;
const int MAX_PREGUNTAS = 10;

Nodo* cargarLista();
void filtrar(Nodo*& cabeza, int pregunta, bool respuestaSi);
void hacerPreguntas(Nodo*& cabeza);
void preguntarPorNombres(Nodo*& cabeza);
void mostrarResultado(Nodo* cabeza);
bool aplicaPregunta(Nodo* cabeza, int pregunta);
char leerRespuestaSiNo();

Nodo* cargarLista() {
    Nodo* cabeza = nullptr;
    Nodo* cola = nullptr;

    auto agregar = [&](Sospechoso s) {
        Nodo* nuevo = new Nodo;
        nuevo->dato = s;
        nuevo->siguiente = nullptr;
        if (cabeza == nullptr) cabeza = nuevo;
        else cola->siguiente = nuevo;
        cola = nuevo;
    };

    Sospechoso s;
    s = {1, "Ana", "Rubio", 'F', false, false, "", ""}; agregar(s);
    s = {2, "Carlos", "Castano", 'M', true, true, "Barba", "Negro"}; agregar(s);
    s = {3, "Sofia", "Negro", 'F', false, false, "", ""}; agregar(s);
    s = {4, "Diego", "Calvo", 'M', false, true, "Bigote", "Castano"}; agregar(s);
    s = {5, "Laura", "Rubio", 'F', true, false, "", ""}; agregar(s);      // Laura con anteojos
    s = {6, "Javier", "Pelirrojo", 'M', true, true, "Barba", "Pelirrojo"}; agregar(s);
    s = {7, "Maria", "Castano", 'F', false, false, "", ""}; agregar(s);
    s = {8, "Pablo", "Negro", 'M', true, true, "Barba", "Negro"}; agregar(s);
    s = {9, "Marta", "Rubio", 'F', true, false, "", ""}; agregar(s);
    s = {10, "Alejandro", "Castano", 'M', false, true, "Barba", "Castano"}; agregar(s);
    s = {11, "Paula", "Negro", 'F', false, false, "", ""}; agregar(s);
    s = {12, "Daniel", "Rubio", 'M', true, false, "", ""}; agregar(s);
    s = {13, "Elena", "Pelirrojo", 'F', false, false, "", ""}; agregar(s);
    s = {14, "Luis", "Castano", 'M', true, true, "Barba", "Negro"}; agregar(s);
    s = {15, "Andrea", "Negro", 'F', true, false, "", ""}; agregar(s);
    s = {16, "Miguel", "Rubio", 'M', false, true, "Barba", "Rubio"}; agregar(s);
    s = {17, "Ana", "Castano", 'F', false, false, "", ""}; agregar(s);
    s = {18, "Carlos", "Negro", 'M', true, false, "", ""}; agregar(s);
    s = {19, "Laura", "Rubio", 'F', false, false, "", ""}; agregar(s);  
    s = {20, "Diego", "Castano", 'M', true, false, "", ""}; agregar(s);
    s = {21, "Sofia", "Negro", 'F', true, false, "", ""}; agregar(s);
    s = {22, "Javier", "Rubio", 'M', false, true, "Barba", "Castano"}; agregar(s);
    s = {23, "Maria", "Negro", 'F', false, false, "", ""}; agregar(s);
    s = {24, "Pablo", "Rubio", 'M', true, false, "", ""}; agregar(s);
    s = {25, "Marta", "Pelirrojo", 'F', false, false, "", ""}; agregar(s);

    return cabeza;
}

bool aplicaPregunta(Nodo* cabeza, int pregunta) {
    Nodo* actual = cabeza;
    switch (pregunta) {
        case 4:
        case 5: 
            while (actual != nullptr) {
                if (actual->dato.genero == 'M') return true;
                actual = actual->siguiente;
            }
            return false;
        case 7: 
            if (barbaColorConocido) return false; 
            while (actual != nullptr) {
                if (actual->dato.tieneBarbaBigote && actual->dato.tipoBarbaBigote == "Barba") return true;
                actual = actual->siguiente;
            }
            return false;
        case 9: 
        case 10: 
            if (barbaColorConocido) return false; 
            while (actual != nullptr) {
                if (actual->dato.tieneBarbaBigote && actual->dato.tipoBarbaBigote == "Barba") return true;
                actual = actual->siguiente;
            }
            return false;
        case 3: 
            if (cabelloColorConocido) return false; 
            while (actual != nullptr) {
                if (actual->dato.cabello == "Negro") return true;
                actual = actual->siguiente;
            }
            return false;
        case 6: 
            if (cabelloColorConocido) return false; 
            actual = cabeza;
            while (actual != nullptr) {
                if (actual->dato.cabello == "Pelirrojo") return true;
                actual = actual->siguiente;
            }
            return false;
        case 8: 
            if (cabelloColorConocido) return false; 
            actual = cabeza;
            while (actual != nullptr) {
                if (actual->dato.cabello == "Rubio") return true;
                actual = actual->siguiente;
            }
            return false;
        default:
            return true; 
    }
}
void liberarLista(Nodo*& cabeza) {
    Nodo* actual = cabeza;
    while (actual != nullptr) {
        Nodo* prox = actual->siguiente;
        delete actual;
        actual = prox;
    }
    cabeza = nullptr;
}

void filtrar(Nodo*& cabeza, int pregunta, bool respuestaSi) {
    Nodo* actual = cabeza;
    Nodo* anterior = nullptr;

    while (actual != nullptr) {
        bool cumple = false;
        Sospechoso &p = actual->dato;

        switch (pregunta) {
            case 1: 
                cumple = (p.genero == 'M');
                break;
            case 2: 
                cumple = p.anteojos;
                break;
            case 3: 
                cumple = (p.cabello == "Negro");
                break;
            case 4: 
                if (p.genero == 'M') cumple = p.tieneBarbaBigote;
                else cumple = false;
                break;
            case 5: 
                if (p.genero == 'M') cumple = (p.cabello == "Calvo");
                else cumple = false;
                break;
            case 6: 
                cumple = (p.cabello == "Pelirrojo");
                break;
            case 7: 
                if (p.tieneBarbaBigote && p.tipoBarbaBigote == "Barba")
                    cumple = (p.colorBarbaBigote == "Negro");
                else cumple = false;
                break;
            case 8: 
                cumple = (p.cabello == "Rubio");
                break;
            case 9: 
                if (p.tieneBarbaBigote && p.tipoBarbaBigote == "Barba")
                    cumple = (p.colorBarbaBigote == "Castano");
                else cumple = false;
                break;
            case 10: 
                if (p.tieneBarbaBigote && p.tipoBarbaBigote == "Barba")
                    cumple = (p.colorBarbaBigote == "Rubio");
                else cumple = false;
                break;
        }

        bool eliminar = false;
        if (respuestaSi) {
            if (!cumple) eliminar = true; 
        } else {
            if (cumple) eliminar = true; 
        }

        if (eliminar) {
            Nodo* aEliminar = actual;
            if (anterior == nullptr) {
                cabeza = actual->siguiente;
                actual = cabeza;
            } else {
                anterior->siguiente = actual->siguiente;
                actual = actual->siguiente;
            }
            delete aEliminar;
        } else {
            anterior = actual;
            actual = actual->siguiente;
        }
    }

    if (respuestaSi) {
        if (pregunta == 7) {
            barbaColorConocido = true;
            colorBarbaConocido = "Negro";
        } else if (pregunta == 9) {
            barbaColorConocido = true;
            colorBarbaConocido = "Castano";
        } else if (pregunta == 10) {
            barbaColorConocido = true;
            colorBarbaConocido = "Rubio";
        }
        if (pregunta == 3) {
            cabelloColorConocido = true;
            colorCabelloConocido = "Negro";
        } else if (pregunta == 6) {
            cabelloColorConocido = true;
            colorCabelloConocido = "Pelirrojo";
        } else if (pregunta == 8) {
            cabelloColorConocido = true;
            colorCabelloConocido = "Rubio";
        }
    }
    preguntasRealizadas++;
}

char leerRespuestaSiNo() {
    string linea;
    while (true) {
        if (!getline(cin, linea)) {
            cin.clear();
            cout << "Error de entrada. Por favor ingrese S o N: ";
            continue;
        }
        size_t i = 0;
        while (i < linea.size() && isspace(static_cast<unsigned char>(linea[i]))) ++i;

        if (i == linea.size()) {
            cout << "Error: entrada vacia. Por favor ingrese S o N: ";
            continue;
        }

        char c = toupper(static_cast<unsigned char>(linea[i]));
        if (c == 'S' || c == 'N') return c;

        cout << "Error: entrada invalida. Por favor ingrese S o N: ";
    }
}

void hacerPreguntas(Nodo*& cabeza) {
    for (int i = 1; i <= 10; i++) {
        if (cabeza == nullptr || cabeza->siguiente == nullptr) break; // cero o uno -> terminar
        if (preguntasRealizadas >= MAX_PREGUNTAS) break; // si ya llegamos al max
        if (!aplicaPregunta(cabeza, i)) continue; // saltar si no aplica o si ya conocemos color

        char r;
        switch (i) {
            case 1: cout << "1. El sospechoso es hombre? (escriba S para si o N para no): "; break;
            case 2: cout << "2. Usa anteojos? (escriba S para si o N para no): "; break;
            case 3: cout << "3. Tiene cabello negro? (escriba S para si o N para no): "; break;
            case 4: cout << "4. Tiene barba o bigote? (escriba S para si o N para no): "; break;
            case 5: cout << "5. Es calvo (si es hombre)? (escriba S para si o N para no): "; break;
            case 6: cout << "6. Es pelirrojo/a? (escriba S para si o N para no): "; break;
            case 7: cout << "7. (Si tiene barba) La barba es negra? (escriba S para si o N para no): "; break;
            case 8: cout << "8. Tiene cabello rubio? (escriba S para si o N para no): "; break;
            case 9: cout << "9. (Si tiene barba) El color de la barba es castano? (escriba S para si o N para no): "; break;
            case 10: cout << "10. (Si tiene barba) El color de la barba es rubio? (escriba S para si o N para no): "; break;
            cout << "" << endl;
        }
        r = leerRespuestaSiNo();
        bool si = (r == 'S');
        filtrar(cabeza, i, si);
    }
}

void preguntarPorNombres(Nodo*& cabeza) {

    while (cabeza != nullptr && cabeza->siguiente != nullptr && preguntasRealizadas < MAX_PREGUNTAS) {
        
        Nodo* candidato = cabeza;
        Sospechoso s = candidato->dato;

        cout << "El sospechoso es " << s.nombre << " (id " << s.id << ")? (S/N): ";
        char r = leerRespuestaSiNo();
        preguntasRealizadas++;

        if (r == 'S') {
        
            Nodo* actual = cabeza;
            while (actual != nullptr) {
                Nodo* prox = actual->siguiente;
                if (actual->dato.id != s.id) delete actual;
                actual = prox;
            }
            
            Nodo* unico = new Nodo;
            unico->dato = s;
            unico->siguiente = nullptr;
            cabeza = unico;
            break;
        } else {
            Nodo* aEliminar = cabeza;
            cabeza = cabeza->siguiente;
            delete aEliminar;
            
        }
    }
}

void mostrarResultado(Nodo* cabeza) {
    if (cabeza == nullptr) {
        cout << "No quedan sospechosos que coincidan con las respuestas.\n";
        return;
    }

    if (cabeza->siguiente == nullptr) {
        Sospechoso s = cabeza->dato;
        cout << "  El Implicado/a es: " << s.nombre << " (id " << s.id << ")\n";
        cout << "  Cabello: " << s.cabello << ", Genero: " << (s.genero=='M'?"M":"F")
            << ", Anteojos: " << (s.anteojos?"Si":"No") << "\n";
        if (s.tieneBarbaBigote) {
            cout << "  Barba/Bigote: " << s.tipoBarbaBigote << ", Color: " << s.colorBarbaBigote << "\n";
        }
        delete cabeza;
        return;
    }

    cout << "No pudo identificar al sospechoso. Sospechosos restantes:\n";
    Nodo* actual = cabeza;
    while (actual != nullptr) {
        Sospechoso &s = actual->dato;
        cout << "- " << s.nombre << " (id " << s.id << "): Cabello=" << s.cabello
            << ", Genero=" << (s.genero=='M'?"M":"F")
            << ", Anteojos=" << (s.anteojos?"Si":"No");
        if (s.tieneBarbaBigote) {
            cout << ", Barba/Bigote=" << s.tipoBarbaBigote << " color=" << s.colorBarbaBigote;
        }
        cout << "\n";
        Nodo* prox = actual->siguiente;
        delete actual;
        actual = prox;
    }
}

int main() {
    Nodo* cabeza = cargarLista();
    hacerPreguntas(cabeza);

    if (cabeza != nullptr && cabeza->siguiente != nullptr && preguntasRealizadas < MAX_PREGUNTAS) {
        preguntarPorNombres(cabeza);
    }

    mostrarResultado(cabeza);

    liberarLista(cabeza);
    return 0;
    cout << "" << endl;
}