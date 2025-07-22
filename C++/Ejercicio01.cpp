#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct personaje{
  string nombre;
  string clase;
  int edad;
};
void consultarPersonaje(const vector<personaje>& personajes, const string& nombreP) {
    bool encontrado = false;
    
    for (const personaje p : personajes) {
        if (p.nombre == nombreP) {
            cout << "Nombre: " << p.nombre << endl;
            cout << "Clase: " << p.clase << endl;
            cout << "Edad: " << p.edad << endl;
            encontrado = true;
            break;
        }
    }
    if (!encontrado) {
        cout << "Personaje no encontrado..." << endl;
    }
    cout << "" << endl;
}
void eliminarPersonaje(vector<personaje>& personajes, const string& nombreP) {
    for (int i = 0; i < personajes.size(); ++i) {
        if (personajes[i].nombre == nombreP) {
            personajes.erase(personajes.begin() + i);
            cout << "Personaje eliminado correctamente." << endl;
            return;
        }
    }
    cout << "Personaje no encontrado." << endl;
}
void mostrarPersonajes(const vector<personaje>& personajes) {
    if (personajes.empty()) {
        cout << "No hay personajes registrados." << endl;
        cout << "" << endl;
        return;
    }
    cout << "Listado de personajes:" << endl;
    for (const auto& p : personajes) {
        cout << "Nombre: " << p.nombre << ", Clase: " << p.clase << ", Edad: " << p.edad << endl;
    }
    cout << "" << endl;
}

int main(){
    vector<personaje> personajes;
    int n;
    int ini = 0; 
    personaje p;

    while (ini == 0) {
        cout << "Por favor ingresa el numero de la funcion que deseas utilizar:\n";
        cout << "1. Ingresar un nuevo personaje al juego" << endl;
        cout << "2. Consultar datos de un personaje" << endl;
        cout << "3. Eliminar un personaje" << endl;
        cout << "4. Ver listado de personajes" << endl;
        cout << "5. Salir del programa" << endl;
        cin >> n;

        if (n == 1) {
            
            int opc;
            cout << "Has seleccionado la opcion 1: Ingresar un personaje" << endl;
            do {
                cout << "Ingrese el numero que aparece al lado de la clase para seleccionarla: " << endl;
                cout << "1. Guerrero" << endl;
                cout << "2. Mago" << endl;
                cout << "3. Semihumano" << endl;
                cin >> opc;

                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(1000, '\n');
                    cout << "Entrada no valida. Por favor, ingrese un numero entre 1 y 3" << endl;
                    opc = 0;
                } else if (opc < 1 || opc > 3) {
                    cout << "Opcion no valida. Por favor, intente de nuevo." << endl;
                }
            } while (opc < 1 || opc > 3);

            switch(opc) {
                case 1: p.clase = "Guerrero"; break;
                case 2: p.clase = "Mago"; break;
                case 3: p.clase = "Semihumano"; break;
            }
            cout << "Ingrese el nombre del personaje: ";
            cin >> p.nombre;

            cout << "Ingrese la edad del personaje: ";
            cin >> p.edad;
            while (cin.fail() || p.edad < 0) {
            cin.clear(); 
            cin.ignore(1000, '\n'); 
            cout << "Edad no valida. Ingrese un numero entero positivo: ";
            cin >> p.edad;
}
            personajes.push_back(p);
            cout << "Personaje de clase: " << p.clase << " con nombre: " << p.nombre << " y edad: " << p.edad << " ha sido creado." << endl;
            cout << "" << endl;

        } else if (n == 2) {
            cout << "Ingrese el nombre del personaje que quiere buscar:" << endl;
            cin >> p.nombre;
            consultarPersonaje(personajes, p.nombre);
        } else if (n == 3) {
            string nomElim;

            cout << "Ingrese el nombre del personaje que desea eliminar:" << endl;
            cin >> nomElim;
            eliminarPersonaje(personajes, nomElim);
           
        } else if (n == 4) {
            mostrarPersonajes(personajes);
            
        } else if (n == 5) {
             cout << "Saliendo del programa..." << endl;
             break;
        }else{
            cout << "Opción no válida. Intente de nuevo." << endl;
        }
    }
    return 0;
}
