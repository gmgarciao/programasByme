#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>

using namespace std;

enum Gravedad { LEVE, MODERADO, SEVERO, CRITICO };

struct Paciente {
    string nombre;
    Gravedad gravedad;
    int tiempoLlegada;

    // este lo usé para hacer una comparación entre los pacientes q tienen la misma gravedad
    bool operator<(const Paciente& otro) const {
        return tiempoLlegada > otro.tiempoLlegada; // usé el fifo por tiempo de llegada
    }
};

struct Doctor {
    int velocidadAtencion;
    int id;
    int tiempoDisponible; // basicamente el tiempo queel doctor estará disponible
};

class SalaEmergencias {
private:

    // se encuentran encapsulados
    priority_queue<Paciente> colaCritica;      /*usé una cola de prioridad, esta a diferencia del queue los elementos se procesan en 
                                                funcion de su prioridad, no en el orden de llegada*/
    priority_queue<Paciente> colaSevera;
    priority_queue<Paciente> colaModerada;
    priority_queue<Paciente> colaLeve;
    vector<Doctor> doctores;//cree un vector donde estarán los doctores
    int turnoActual;
    int tiempoActual;
    int totalPacientes;
    int tiempoTotalEspera;

public:
    SalaEmergencias() : turnoActual(0), tiempoActual(0), totalPacientes(0), tiempoTotalEspera(0) {}//contsructor donde se inicializan los atributos, especificamente en 0

    void agregarDoctor(int velocidad) {
        Doctor doctor;
        doctor.velocidadAtencion = velocidad;
        doctor.id = doctores.size() + 1;//tamaño del vector de doctores
        doctor.tiempoDisponible = 0; // al inicio todos los doctores estarán disponibles
        doctores.push_back(doctor);
    }

    void encolarPaciente(const string& nombre, Gravedad gravedad, int tiempoLlegada) {
        Paciente paciente;
        paciente.nombre = nombre;
        paciente.gravedad = gravedad;
        paciente.tiempoLlegada = tiempoLlegada;

        switch (gravedad) {
            case CRITICO:
                colaCritica.push(paciente);
                break;                                  /*Añadimos la gravedad a cada paciente*/
            case SEVERO:
                colaSevera.push(paciente);
                break;
            case MODERADO:
                colaModerada.push(paciente);
                break;
            case LEVE:
                colaLeve.push(paciente);
                break;
        }
    }

    void procesarPacientes() {
        while (!colaCritica.empty() || !colaSevera.empty() || !colaModerada.empty() || !colaLeve.empty()) {
            if (!colaCritica.empty()) {
                procesarPaciente(colaCritica);
            } else if (!colaSevera.empty()) {
                procesarPaciente(colaSevera);              /* aqui se procesan todos los pacientes que están en las 
                                                            colas de emergencia y se le asignan atención médica de acuerdo con el orden de prioridad*/
            } else if (!colaModerada.empty()) { 
                procesarPaciente(colaModerada);
            } else if (!colaLeve.empty()) {
                procesarPaciente(colaLeve);
            }
            cambiarTurno();//se cambia el turno segun los pacientes
        }
    }

    void procesarPaciente(priority_queue<Paciente>& cola) {
        Paciente paciente = cola.top();
        cola.pop();
        
        Doctor& doctorActual = doctores[turnoActual];
        
        // calculamos el tiempo de espera del siguiente paciente
        int tiempoEspera = max(doctorActual.tiempoDisponible - paciente.tiempoLlegada, 0);
        
        // aqui se actualiza el tiempo del doctor
        doctorActual.tiempoDisponible = max(paciente.tiempoLlegada, doctorActual.tiempoDisponible) + doctorActual.velocidadAtencion;

        cout << "Atendiendo a: " << paciente.nombre << ", Gravedad: " << paciente.gravedad 
             << ", Tiempo de espera: " << tiempoEspera << " minutos con doctor turno " 
             << doctorActual.id << "." << endl;

        tiempoTotalEspera += tiempoEspera;
        totalPacientes++;
    }

    void cambiarTurno() {
        tiempoActual++; //incrementamos el tiempo en 1 min
        turnoActual = (turnoActual + 1) % doctores.size(); //actualizamos en turno del doctor actual
        //se suma 1 al turno actual y se usa el operador módulo para rotar entre los doctores
        //por ejm,si hay 3 doctores, por ejemplo, después del doctor 2 (índice 2), el turno vuelve al doctor 1 (índice 0)
    }

    double tiempoPromedioEspera() {
        if (totalPacientes == 0) return 0;
        return static_cast<double>(tiempoTotalEspera) / totalPacientes;
    }
};

int main() {
    SalaEmergencias sala;

    int nDoctores;
    cout << "Ingrese el número de doctores disponibles: ";
    cin >> nDoctores;

    for (int i = 0; i < nDoctores; i++) {
        int velocidad;
        cout << "Ingrese la velocidad de atención del doctor " << i + 1 << ": ";
        cin >> velocidad;
        sala.agregarDoctor(velocidad);
    }

    int idPaciente = 1;
    while (true) {
        string nombre;
        Gravedad gravedad;
        int tiempoLlegada;

        cout << "Ingrese el nombre del paciente " << idPaciente << ": ";
        cin >> nombre;

        cout << "Ingrese la gravedad del paciente (0 = leve, 1 = moderado, 2 = severo, 3 = critico): ";
        int gravedadInput;
        cin >> gravedadInput;
        gravedad = static_cast<Gravedad>(gravedadInput);//usé esto para convertir la entrada de la gravedad a un numero entero

        cout << "Ingrese el tiempo de llegada del paciente: ";
        cin >> tiempoLlegada;

        sala.encolarPaciente(nombre, gravedad, tiempoLlegada);

        char respuesta;
        cout << "Agregar otro paciente? (s/n): ";
        cin >> respuesta;

        if (respuesta == 'n') break;
        idPaciente++;
    }

    sala.procesarPacientes();

    cout << "Tiempo promedio de espera: " << sala.tiempoPromedioEspera() << " minutos." << endl;

    return 0;
}
