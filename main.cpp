#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <random>
#include <stack>
#include <unordered_set>
#include <algorithm>
using namespace std;

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> dis(1, 1000);
const double big_double = 10000000000;

struct Nodo {
public:
    int id;
    int profundidad;
    vector<Nodo*> hijos;
    int id_barrera;
    int id_llave;
    
    Nodo(int id, int profundidad) : id(id), profundidad(profundidad), id_barrera(-1), id_llave(-1) {}

    bool asignarBarrera(int id){
        bool asignada = false;
        if (id_barrera == -1){
            id_barrera = id;
            asignada = true;
        } 
        return asignada;
    }

    bool asignarLlave(int id){
        bool asignada = false;
        if (id_llave == -1){
            id_llave = id;
            asignada = true;
        } 
        return asignada;
    }
};

class Dungeon {
    public:
    Nodo* raiz;
    Nodo* fin;
    vector<Nodo*> recorrido; // por conveniencia es de atras hacia adelante
    int num_habitaciones;
    int num_llaves;
    int llaves_actual;
    int num_barreras;
    int barreras_actual;
    int llaves_necesarias;
    int llaves_necesarias_actual;
    int habitaciones_generadas;
    double coef_lineal;
    double coef_lineal_actual;
    int nodos_con_hijos;
    vector<Nodo*> nodos_barreras;
    vector<Nodo*> nodos_llaves;

    Dungeon(int num_habitaciones, int num_llaves, int num_barreras, double coef_lineal, int llaves_necesarias) 
        : num_habitaciones(num_habitaciones), num_llaves(num_llaves), num_barreras(num_barreras), 
          coef_lineal(coef_lineal), llaves_necesarias(llaves_necesarias) {
        raiz = new Nodo(0, 0); // Inicializamos la raíz
        habitaciones_generadas = 1; // Contamos la raíz

        // Inicializamos valores actuales
        llaves_actual = 0;
        barreras_actual = 0;
        llaves_necesarias_actual = 0;
        coef_lineal_actual = 0.0;
        nodos_con_hijos = 0;
    }

    double funcionObjetivoHabitaciones(int habitaciones_actuales, int nodos_padres) {
        return abs(num_habitaciones - habitaciones_actuales) + ((num_habitaciones / 10.0) * abs(coef_lineal - ((habitaciones_actuales-1)/nodos_padres)));
    }

    double funcionObjetivoBarrerasLlaves(){
        llaves_necesarias_actual = 0;
        unordered_set<int> barrerasRecorridas;
        queue<Nodo*> cola;

        for (Nodo* actual : recorrido) {
            if (actual->id_barrera != -1) {
                barrerasRecorridas.insert(actual->id_barrera);
                cola.push(actual);
                llaves_necesarias_actual++;
            }
        }

        return 2*(abs(num_barreras - barreras_actual) + abs(num_llaves - llaves_actual)) + abs(llaves_necesarias - llaves_necesarias_actual);
    }

    // Método para generar la estructura de la mazmorra
    void generarDungeon() {
        queue<Nodo*> cola;
        cola.push(raiz);
        
        while (habitaciones_generadas < num_habitaciones && !cola.empty()) {
            int nivel_size = cola.size(); // Nodos en el nivel actual
            bool se_agregaron_hijos = false;
            stack<Nodo*> pendiente;
            
            // Para cada nodo del nivel actual
            for (int i = 0; i < nivel_size; ++i) {
                Nodo* actual = nullptr;

                if (!pendiente.empty() && dis(gen) < 201){
                    actual = pendiente.top();
                    pendiente.pop();
                    --i;
                }

                else {
                    actual = cola.front();
                    cola.pop();

                    if (!cola.empty() && dis(gen) < 201){
                        pendiente.push(actual);
                        actual = cola.front();
                        cola.pop();
                        ++i;
                    }

                }



                // Determinamos el número máximo de hijos que se pueden agregar
                int max_hijos = min(3, num_habitaciones - habitaciones_generadas);
                
                // Actualizamos el coeficiente lineal actual si hay nodos con hijos
                if (nodos_con_hijos > 0) {
                    coef_lineal_actual = (habitaciones_generadas - 1) / static_cast<double>(nodos_con_hijos);
                }

                // Número de hijos a agregar para acercarse al coeficiente lineal
                int hijos_a_agregar; 

                if (abs(coef_lineal - (habitaciones_generadas - 1 + (int)coef_lineal) / (static_cast<double>(nodos_con_hijos)+1)) < abs(coef_lineal - (habitaciones_generadas - 1 + (int)ceil(coef_lineal)) / (static_cast<double>(nodos_con_hijos)+1))) {
                    hijos_a_agregar = (int)coef_lineal;
                }
                else {
                    hijos_a_agregar = (int)ceil(coef_lineal);
                }
                
                // Agregar hijos
                if (hijos_a_agregar > 0) nodos_con_hijos++; // Incrementamos nodos que tendrán hijos
                for (int j = 0; j < hijos_a_agregar && habitaciones_generadas < num_habitaciones; ++j) {
                    Nodo* hijo = new Nodo(habitaciones_generadas++, actual->profundidad + 1);
                    actual->hijos.push_back(hijo);
                    cola.push(hijo);
                    se_agregaron_hijos = true;
                }
            }

            if (!se_agregaron_hijos && !cola.empty()) {
                Nodo* actual = cola.front();
                int max_hijos = min(3, num_habitaciones - habitaciones_generadas);

                for (int j = 0; j < max_hijos && habitaciones_generadas < num_habitaciones; ++j) {
                    Nodo* hijo = new Nodo(habitaciones_generadas++, actual->profundidad + 1);
                    actual->hijos.push_back(hijo);
                    cola.push(hijo);
                    se_agregaron_hijos = true;
                }
                nodos_con_hijos++;
            }
        }
    }

    double movimientoHabitaciones(bool permitido[4], double& menor){
        int movimiento;
        double agregar = funcionObjetivoHabitaciones(habitaciones_generadas + 1, nodos_con_hijos); // se agrega una habitacion a un nodo padre
        double agregar_hoja = funcionObjetivoHabitaciones(habitaciones_generadas + 1, nodos_con_hijos + 1); // se agrega una habitacion a un nodo hoja
        double quitar = funcionObjetivoHabitaciones(habitaciones_generadas - 1, nodos_con_hijos); // se quita una habitacion hija de un nodo con más de 1 hijo
        double quitar_solo = funcionObjetivoHabitaciones(habitaciones_generadas - 1, nodos_con_hijos - 1); // se quita una habitacion hija de un nodo con solo 1 hijo


        if (!permitido[0]){
            agregar = big_double;
        }
        if (!permitido[1]){
            agregar_hoja = big_double;
        }
        if (!permitido[2]){
            quitar = big_double;
        }
        if (!permitido[3]) {
            quitar_solo = big_double;
        }

        vector<double> movimientos = {agregar, agregar_hoja, quitar, quitar_solo};
        sort(movimientos.begin(), movimientos.end());
        menor = movimientos[0];
        
        if (menor == agregar) {
            movimiento = 1;
        } else if (menor == agregar_hoja){
            movimiento = 2;
        } else if (menor == quitar){
            movimiento = 3;
        } else {
            movimiento = 4;
        }

        if (menor == big_double){
            movimiento = 0;
        }

        return movimiento;
    }

    void colocarBarrerasLlaves(){
        Nodo* actual = raiz;
        stack<Nodo*> ruta;
        int id_hijo;
        queue<Nodo*> cola;
        cola.push(raiz);
        vector<Nodo*> nodos;

        while (!actual->hijos.empty()) {
            ruta.push(actual);
            id_hijo = dis(gen) % actual->hijos.size();
            actual = actual->hijos[id_hijo];
            if (actual->hijos.size() == 0) {
                break;
            }
            
        }
        fin = actual;
        

        while (!cola.empty()){
            actual = cola.front();
            for (Nodo* hijo : actual->hijos) {
                    cola.push(hijo);
                }
            cola.pop();
            nodos.push_back(actual);
        }

        int i = 0;
        bool colocado = false;
        // se colocan tantas barreras como llaves necesarias entre la habitacion final y la raiz
        while (!ruta.empty()) {
            actual = ruta.top();
            recorrido.push_back(actual);
            ruta.pop();
            if (actual->asignarBarrera(barreras_actual) && barreras_actual < llaves_necesarias){
                while (i < 100) {
                    int pos_llave = dis(gen) % nodos.size();
                    if (nodos[pos_llave]->profundidad < actual->profundidad) {
                        nodos[pos_llave]->asignarLlave(llaves_actual);
                        nodos_llaves.push_back(nodos[pos_llave]);
                        nodos_barreras.push_back(actual);
                        barreras_actual++;
                        llaves_actual++;
                        colocado = true;
                        i = 0;
                        break;
                    } else {
                        i++;
                    }
                }
            }
            if (!colocado) {
                actual->id_barrera = -1;
            }
            colocado = false;
        }

        // se colocan las barreras sobrantes al azar
        int elegido;
        i = 0;
        colocado = false;
        int j=0;
        while (barreras_actual < num_barreras && llaves_actual < num_llaves && j < 10000){
            elegido = dis(gen) % nodos.size();
            if (nodos[elegido]->asignarBarrera(barreras_actual)){
                while (i < 100) {
                    int pos_llave = dis(gen) % nodos.size();
                    if (nodos[pos_llave]->profundidad < nodos[elegido]->profundidad) {
                        nodos_llaves.push_back(nodos[pos_llave]);
                        nodos_barreras.push_back(nodos[elegido]);
                        barreras_actual++;
                        llaves_actual++;
                        colocado = true;
                        i = 0;
                        j = 0;
                        break;
                    } else {
                        i++;
                    }
                }
                barreras_actual++;
            } 
            if (!colocado){
                j++;
            }
            colocado = false;
        }
    }

    Nodo* movimientoBarreras(int id) {
        int profundidad_llave = nodos_llaves[id]->profundidad;
        Nodo* actual = raiz;
        queue<Nodo*> cola;
        cola.push(raiz);
        vector<Nodo*> nodos_permitidos;
        double mejor_objetivo = big_double;

        while (!cola.empty()){
            actual = cola.front();
            if (actual->profundidad > profundidad_llave && actual->id_barrera == -1){
                nodos_permitidos.push_back(actual);
            }
            for (Nodo* hijo : actual->hijos) {
                    cola.push(hijo);
                }
            cola.pop();
        }


        Nodo* mejor_movimiento;

        for (Nodo* movimiento : nodos_permitidos){
            movimiento->asignarBarrera(id);
            nodos_barreras[id]->id_barrera = -1;
            double objetivo = funcionObjetivoBarrerasLlaves();
            nodos_barreras[id]->asignarBarrera(movimiento->id_barrera);
            movimiento->id_barrera = -1;
            if (objetivo < mejor_objetivo || (objetivo == mejor_objetivo && dis(gen) < 500)){
                mejor_objetivo = objetivo;
                mejor_movimiento = movimiento;
            } 
        }
        
        return mejor_movimiento;
    }

    void imprimirDungeon() {
        queue<Nodo*> cola;
        cola.push(raiz);
        
        cout << "Estructura de la Dungeon:" << endl;
        
        while (!cola.empty()) {
            Nodo* actual = cola.front();
            cola.pop();
            
            cout << "Habitación ID: " << actual->id << ", Profundidad: " << actual->profundidad;
            if (actual->id_barrera != -1){
                cout << ", Barrera ID: " << actual->id_barrera;
            }
            if (actual->id_llave != -1){
                cout << ", Llave ID: " << actual->id_llave;
            }
            cout << ", Hijos: ";
            for (auto hijo : actual->hijos) {
                cout << hijo->id << " ";
                cola.push(hijo);
            }
            cout << endl;
        }
    }
};

double tabuSearchHabitaciones(Dungeon& dungeon,int N){
    int tabu = 0;
    int movimiento;
    double mejor_objetivo = dungeon.funcionObjetivoHabitaciones(dungeon.habitaciones_generadas, dungeon.nodos_con_hijos);
    double objetivo;
    int i = 0;

    while (i < N){

        vector<Nodo*> hojas;
        vector<Nodo*> padres_hojas;
        vector<Nodo*> hijos1;
        vector<Nodo*> hijos2;
        vector<Nodo*> hijos3;

        queue<Nodo*> cola;
        Nodo* actual = dungeon.raiz;
        cola.push(dungeon.raiz);
        while (!cola.empty()){
            Nodo* anterior = actual;
            actual = cola.front();
            cola.pop();
            int hijos = 0;

            if (actual->hijos.empty()) { 
                hojas.push_back(actual);
            } else {
                for (Nodo* hijo : actual->hijos) {
                    cola.push(hijo);
                    hijos++;
                }
                if (hijos == 1) {
                    hijos1.push_back(actual);
                } else if (hijos == 2) {
                    hijos2.push_back(actual);
                } else {
                    hijos3.push_back(actual);
                }
            }
        }
        

        bool permitido[4] = {!(hijos1.empty() && hijos2.empty()), !hojas.empty(), !(hijos2.empty() && hijos3.empty()), !hijos1.empty()};
        if (movimiento == 1 || movimiento == 2) {
            permitido[movimiento + 1] = false;
        } else if (movimiento == 3 || movimiento == 4) {
            permitido[movimiento - 3] = false;
        }

        movimiento = dungeon.movimientoHabitaciones(permitido, objetivo);

        if (movimiento == 1) {

            if ((dis(gen) < 500 && !hijos1.empty()) || hijos2.empty()){
                int indice_nodo = dis(gen) % hijos1.size();
                Nodo* nodo = hijos1[indice_nodo];
                Nodo* hijo = new Nodo(dungeon.habitaciones_generadas++, nodo->profundidad + 1);
                nodo->hijos.push_back(hijo);
            } else {
                int indice_nodo = dis(gen) % hijos2.size();
                Nodo* nodo = hijos2[indice_nodo];
                Nodo* hijo = new Nodo(dungeon.habitaciones_generadas++, nodo->profundidad + 1);
                nodo->hijos.push_back(hijo);
            }

        } else if (movimiento == 2) {

            int indice_hoja = dis(gen) % hojas.size();
            Nodo* hoja = hojas[indice_hoja];
            Nodo* hijo = new Nodo(dungeon.habitaciones_generadas++, hoja->profundidad + 1);
            dungeon.nodos_con_hijos++;
            hoja->hijos.push_back(hijo);

        } else if (movimiento == 3) {

            int i = 0;
            while (i < 100) {
                if ((dis(gen) < 500 && !hijos2.empty()) || hijos3.empty()){

                    int indice_nodo = dis(gen) % hijos2.size();
                    Nodo* nodo = hijos2[indice_nodo];
                    if (nodo->hijos[0]->hijos.empty()) {
                        nodo->hijos.erase(nodo->hijos.begin());
                        break;
                    } else if (nodo->hijos[1]->hijos.empty()) {
                        nodo->hijos.erase(nodo->hijos.begin() + 1);
                        break;
                    }

                } else {

                    int indice_nodo = dis(gen) % hijos3.size();
                    Nodo* nodo = hijos3[indice_nodo];
                    if (nodo->hijos[0]->hijos.empty()) {
                        nodo->hijos.erase(nodo->hijos.begin());
                        break;
                    } else if (nodo->hijos[1]->hijos.empty()) {
                        nodo->hijos.erase(nodo->hijos.begin() + 1);
                        break;
                    } else if (nodo->hijos[2]->hijos.empty()) {
                        nodo->hijos.erase(nodo->hijos.begin() + 2);
                        break;
                    }
                }
                i++;
            }
            dungeon.habitaciones_generadas--;

        } else if (movimiento == 4) {
            int i = 0;
            while (i < 100) {
                int indice_nodo = dis(gen) % hijos1.size();
                Nodo* nodo = hijos1[indice_nodo];
                if (nodo->hijos[0]->hijos.empty())
                    nodo->hijos.erase(nodo->hijos.begin());
                    break;
                i++;
            }
            dungeon.habitaciones_generadas--;
            dungeon.nodos_con_hijos--;
        }

        if (objetivo <= mejor_objetivo){
            mejor_objetivo = objetivo;
        }

        i++;
    }

    return mejor_objetivo;
}

double tabuSearchBarreras(Dungeon& dungeon, int N){
    vector<int> tabu;
    int i = 0;
    while (i < N){
        int j = 0;
        double objetivo = big_double;
        Nodo* mejor_movimiento;
        int id_mejor_movimiento;

        while (j < dungeon.nodos_barreras.size()){
        //    try {
                // hacer movimiento para funcion objetivo
                Nodo* movimiento = dungeon.movimientoBarreras(j);
                movimiento->asignarBarrera(dungeon.nodos_barreras[j]->id_barrera);
                dungeon.nodos_barreras[j]->id_barrera = -1;
                double objetivo_movimiento = dungeon.funcionObjetivoBarrerasLlaves();
                // desacer movimiento
                dungeon.nodos_barreras[j]->asignarBarrera(movimiento->id_barrera);
                movimiento->id_barrera = -1;
                if (objetivo_movimiento <= objetivo && find(tabu.begin(), tabu.end(), j) == tabu.end()){
                    objetivo = objetivo_movimiento;
                    mejor_movimiento = movimiento;
                    id_mejor_movimiento = j;
                }
                j++;
        //    }
        }
        // hacer movimiento        
        mejor_movimiento->asignarBarrera(dungeon.nodos_barreras[id_mejor_movimiento]->id_barrera);
        dungeon.nodos_barreras[id_mejor_movimiento]->id_barrera = -1;
        dungeon.nodos_barreras[id_mejor_movimiento] = mejor_movimiento;
        tabu.push_back(id_mejor_movimiento);

        if (tabu.size() > 3) {
            tabu.erase(tabu.begin());
        }
        i++;

    }
    return dungeon.funcionObjetivoBarrerasLlaves();
}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        cerr << "Uso: " << argv[0] << " <habitaciones> <llaves> <barreras> <coef_lineal> <llaves_necesarias>" << endl;
        return 1;
    }
    
    int num_habitaciones = atoi(argv[1]);
    int num_llaves = atoi(argv[2]);
    int num_barreras = atoi(argv[3]);
    double coef_lineal = atof(argv[4]);
    int llaves_necesarias = atoi(argv[5]);

    int resets = 3;
    int iteraciones = 100;
    
    double mejor_objetivo = big_double;
    Dungeon* mejor_dungeon = nullptr;
    int i = 0;
    while (i < resets) {
        Dungeon* dungeon = new Dungeon(num_habitaciones, num_llaves, num_barreras, coef_lineal, llaves_necesarias);
        dungeon->generarDungeon();

        double objetivo_h = tabuSearchHabitaciones(*dungeon, iteraciones);

        dungeon->colocarBarrerasLlaves();
        
        double objetivo_bl = tabuSearchBarreras(*dungeon, iteraciones);

        
        if (2*objetivo_h + objetivo_bl < mejor_objetivo) {
            mejor_objetivo = 2*objetivo_h + objetivo_bl;
            if (mejor_dungeon != nullptr) {
                delete mejor_dungeon;
            }
            mejor_dungeon = dungeon;
        } else {
            delete dungeon;
        }
        i++;
    };

    // quitar // de abajo para visualizar la dungeon
    // mejor_dungeon->imprimirDungeon
    cout << mejor_dungeon->habitaciones_generadas<< endl;
    cout << mejor_dungeon->llaves_actual << endl;
    cout << mejor_dungeon->barreras_actual << endl;
    cout << mejor_dungeon->coef_lineal_actual << endl;
    cout << (mejor_dungeon->funcionObjetivoHabitaciones(mejor_dungeon->habitaciones_generadas, mejor_dungeon-> nodos_con_hijos) * 2) + mejor_dungeon->funcionObjetivoBarrerasLlaves()<< endl;


    return 0;
}