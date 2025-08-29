#include <iostream>
#include <string>
#include <queue>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <limits>

using namespace std;
using namespace std::chrono;

struct Amenaza {
    string id;
    string descripcion;
    string clasificacion; // Alto, Bajo, Falsa
    system_clock::time_point horaRecepcion;
    system_clock::time_point horaHistorial;
    bool enHistorial = false;
};

// ===== Funciones auxiliares =====
string formatearHora(system_clock::time_point tp) {
    time_t tiempo = system_clock::to_time_t(tp);
    tm local_tm = *localtime(&tiempo);
    stringstream ss;
    ss << put_time(&local_tm, "%H:%M:%S");
    return ss.str();
}

long long diferenciaSegundos(system_clock::time_point inicio, system_clock::time_point fin) {
    return duration_cast<seconds>(fin - inicio).count();
}

int leerEntero() {
    int valor;
    while (true) {
        if (!(cin >> valor)) {
            cout << "Entrada invalida, debe ingresar un numero(1-3): ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return valor;
        }
    }
}

string leerClasificacion() {
    string clasif;
    while (true) {
        cout << "Clasificacion de riesgo (Alta, Baja, Falsa): ";
        getline(cin, clasif);
        if (clasif == "Alta" || clasif == "Baja" || clasif == "Falsa" ||
            clasif == "alta" || clasif == "baja" || clasif == "falsa") {
            return clasif;
        } else {
            cout << "Clasificacion invalida. Debe ser: Alto, Bajo o Falsa.\n";
        }
    }
}

// ===== MAIN =====
int main() {
    queue<Amenaza> colaAltas, colaBajas, historial;
    int contadorMisiones = 1;
    int opcion;

    do {
        
        cout << "1. Registrar amenaza" << endl;
        cout << "2. Ver informe de amenazas" << endl;
        cout << "3. Salir" << endl;
        cout << "Ingrese el numero de la opcion que decea ejecutar: " << endl;

        opcion = leerEntero();

        if (opcion == 1) {
            Amenaza nueva;
            nueva.id = "M" + to_string(contadorMisiones++);
            cout << "Descripcion de la amenaza: ";
            getline(cin, nueva.descripcion);
            nueva.clasificacion = leerClasificacion();
            nueva.horaRecepcion = system_clock::now();

            if (nueva.clasificacion == "Falsa" || nueva.clasificacion == "falsa") {
                nueva.enHistorial = true;
                nueva.horaHistorial = nueva.horaRecepcion;
                historial.push(nueva);
            } else if (nueva.clasificacion == "Alta" || nueva.clasificacion == "alta") {
                colaAltas.push(nueva);
            } else {
                colaBajas.push(nueva);
            }

            cout << "Amenaza registrada con ID " << nueva.id
                 << " a las " << formatearHora(nueva.horaRecepcion) << endl;

        } else if (opcion == 2) {
            cout << "\n===== INFORME =====\n";
            auto ahora = system_clock::now();
            cout << "Hora actual del informe: " << formatearHora(ahora) << "\n";

            // Mover de colas a historial según tiempos
            int n = colaAltas.size();
            for (int i = 0; i < n; i++) {
                Amenaza a = colaAltas.front();
                colaAltas.pop();
                if (diferenciaSegundos(a.horaRecepcion, ahora) >= 60) {
                    a.enHistorial = true;
                    a.horaHistorial = a.horaRecepcion + minutes(1);
                    historial.push(a);
                } else {
                    colaAltas.push(a);
                }
            }

            n = colaBajas.size();
            for (int i = 0; i < n; i++) {
                Amenaza a = colaBajas.front();
                colaBajas.pop();
                if (diferenciaSegundos(a.horaRecepcion, ahora) >= 180) {
                    a.enHistorial = true;
                    a.horaHistorial = a.horaRecepcion + minutes(3);
                    historial.push(a);
                } else {
                    colaBajas.push(a);
                }
            }

            // Mostrar listas
            cout << "\nLista de Amenazas Altas:\n";
            if (colaAltas.empty()) cout << " - (vacia)\n";
            else {
                queue<Amenaza> temp = colaAltas;
                while (!temp.empty()) {
                    Amenaza a = temp.front(); temp.pop();
                    cout << " - " << a.id << " (" << a.clasificacion << "), "
                         << formatearHora(a.horaRecepcion) << endl;
                }
            }

            cout << "\nLista de Amenazas Bajas:\n";
            if (colaBajas.empty()) cout << " - (vacia)\n";
            else {
                queue<Amenaza> temp = colaBajas;
                while (!temp.empty()) {
                    Amenaza a = temp.front(); temp.pop();
                    cout << " - " << a.id << " (" << a.clasificacion << "), "
                         << formatearHora(a.horaRecepcion) << endl;
                }
            }

            cout << "\nHistorial:\n";
            if (historial.empty()) cout << " - (vacio)\n";
            else {
                queue<Amenaza> temp = historial;
                while (!temp.empty()) {
                    Amenaza a = temp.front(); temp.pop();
                    cout << " - " << a.id << " (" << a.clasificacion << "), "
                         << formatearHora(a.horaHistorial) << endl;
                }
            }

        } else if (opcion == 3) {
            cout << "Saliendo del programa..." << endl;
        } else {
            cout << "Opcion invalida. Intente de nuevo." << endl;
        }

    } while (opcion != 3);

    return 0;
}

