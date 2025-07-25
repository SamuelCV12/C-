#include <iostream>
#include <limits>
using namespace std;

int main() {
    int correct1[10] = {1, 3, 6, 10, 15, 21, 28, 36, 45, 55};       
    int correct2[10] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29};        
    int correct3[10] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34};          

    string nombresSeries[3] = {"Triangulares", "Primos", "Fibonacci"};
    int* series[3] = {correct1, correct2, correct3};

    int matrix[3][10];  
    int valoresIncorrectos[3]; 

    cout << "=== Bienvenido al Descifrador de Codigo ===\n";
    cout << "Ingrese una matriz 3x10 basada en las siguientes secuencias:\n";

    for(int fila = 0; fila < 3; ++fila) {
        while (true) {
            int erroresFila = 0;

            cout << "\nFila " << fila + 1 << " - Serie " << nombresSeries[fila] << ":\n";
            cout << "Ten en cuenta que solo puedes poner un valor erroneo dentro de la secuencia.\n";
            cout << "Secuencia esperada: ";
            for (int i = 0; i < 10; ++i) cout << series[fila][i] << " ";
            cout << "\n";

            for(int col = 0; col < 10; ++col) {
                while (true) {
                    cout << "Ingrese valor para fila " << (fila+1) << ", columna " << (col+1) << ": ";
                    cin >> matrix[fila][col];

                    if (cin.fail()) {
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        cout << "Entrada invalida. Por favor ingrese un numero entero.\n";
                        continue;
                    }

                    if (matrix[fila][col] != series[fila][col]) {
                        erroresFila++;
                        if (erroresFila > 1) {
                            cout << "Ya se ha ingresado un numero erroneo en esta secuencia.\n";
                            cout << "No puede haber mas de un valor incorrecto.\n";
                            cout << "Ingrese nuevamente el valor que continua en la secuencia.\n";
                            erroresFila--;
                            continue;
                        }
                    }

                    break;
                }
            }

            if (erroresFila == 0) {
                cout << "No se detecto ningun error en esta fila. Debe haber exactamente un valor erroneo. Ingrese la fila de nuevo.\n";
                continue;
            }

            for (int col = 0; col < 10; ++col) {
                if (matrix[fila][col] != series[fila][col]) {
                    valoresIncorrectos[fila] = matrix[fila][col];
                    break;
                }
            }

            break; 
        }
    }
    cout << "" << endl;
    
    cout << "\n Codigo incorrecto detectado: "
        << valoresIncorrectos[0] << "-"
        << valoresIncorrectos[1] << "-"
        << valoresIncorrectos[2] << "\n";
     cout << "" << endl;

    return 0;
}
