#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <set>

using namespace std;

int main() {

    // Archivo a leer
    string filename = "EBPlusPlusAdjustPANCAN_IlluminaHiSeq_RNASeqV2.geneExp.tsv";
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: No pude abrir el archivo." << endl;
        return 1;
    }

    // Leer la línea de encabezados
    string header;
    getline(file, header);

    // Extraer los nombres de columna separados por tabuladores
    stringstream ss(header);
    string col;
    vector<string> headerNames;

    while (getline(ss, col, '\t')) {
        headerNames.push_back(col);
    }

    int numColumns = headerNames.size();
    cout << "Columnas detectadas: " << numColumns << endl;

    // Tamaño del Top-K
    const int K = 5;

    // Crear un Top-K por columna usando multiset (orden ascendente)
    vector<multiset<double>> topK(numColumns);

    string line;
    int maxLines = 200;   // Puedes aumentar o remover este límite
    int counter = 0;

    // Procesar cada línea del archivo
    while (getline(file, line) && counter < maxLines) {

        stringstream row(line);
        string cell;
        int colIndex = 0;

        // Procesar cada columna de la línea
        while (getline(row, cell, '\t')) {
            if (colIndex >= numColumns)
                break;

            // Convertir a double
            double val = strtod(cell.c_str(), nullptr);

            // Referencia al Top-K de la columna
            auto& tk = topK[colIndex];

            // Si el Top-K tiene menos de K valores → insertar
            if (tk.size() < K) {
                tk.insert(val);
            }
            // Si está lleno, verificamos si reemplazamos
            else {
                auto smallest = tk.begin();  // menor valor actual
                if (val > *smallest) {
                    tk.erase(smallest);      // eliminar menor
                    tk.insert(val);          // agregar el nuevo
                }
            }

            colIndex++;
        }

        counter++;
    }


    // =============================
    //   IMPRIMIR RESULTADOS
    // =============================
    cout << "\n=== TOP K por columna ===\n\n";

    for (int i = 0; i < numColumns; i++) {
        cout << "Columna " << i << " (" << headerNames[i] << "): ";
        for (double v : topK[i])
            cout << v << " ";
        cout << "\n";
    }


    // =============================
    //   GENERAR ARCHIVO CSV
    // =============================
    ofstream out("topk_results.csv");

    if (!out.is_open()) {
        cerr << "Error: No pude crear el archivo CSV." << endl;
        return 1;
    }

    // Escribir encabezado del CSV
    out << "ColumnIndex,ColumnName,TopKValues\n";

    // Escribir resultados por columna
    for (int i = 0; i < numColumns; i++) {

        out << i << "," << headerNames[i] << ",";

        // Escribir los valores del Top-K separados por espacio
        int count = 0;
        for (double v : topK[i]) {
            out << v;
            if (++count < topK[i].size())
                out << " ";
        }

        out << "\n";
    }

    out.close();

    cout << "\nArchivo CSV generado: topk_results.csv\n";

    return 0;
}