#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <vector>
#include <set>

// --------------------------------------------------
// Utilidades de entrada (validación)
// --------------------------------------------------
int readInt(const std::string &prompt) {
    std::string line;
    while (true) {
        std::cout << prompt;
        if (!std::getline(std::cin, line)) throw std::runtime_error("Entrada interrumpida.");
        try {
            size_t idx;
            int val = std::stoi(line, &idx);
            if (idx != line.size()) throw std::invalid_argument("Caracteres invalidos");
            return val;
        } catch (std::exception &e) {
            std::cout << "Entrada invalida. Intente nuevamente (ingrese un numero entero).\n";
        }
    }
}

std::string trim(const std::string &s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::string readNonEmpty(const std::string &prompt) {
    std::string line;
    while (true) {
        std::cout << prompt;
        if (!std::getline(std::cin, line)) throw std::runtime_error("Entrada interrumpida.");
        std::string t = trim(line);
        if (t.empty()) {
            std::cout << "El valor no puede estar vacio. Intente otra vez.\n";
            continue;
        }
        return t;
    }
}

/* Nueva función: lee una cadena no vacía compuesta SOLO por dígitos */
std::string readDigitsOnly(const std::string &prompt) {
    std::string line;
    while (true) {
        std::cout << prompt;
        if (!std::getline(std::cin, line)) throw std::runtime_error("Entrada interrumpida.");
        std::string t = trim(line);
        if (t.empty()) {
            std::cout << "El valor no puede estar vacio. Intente otra vez.\n";
            continue;
        }
        bool allDigits = std::all_of(t.begin(), t.end(), [](unsigned char c){ return std::isdigit(c); });
        if (!allDigits) {
            std::cout << "Entrada invalida. Ingrese solo numeros (sin espacios ni letras).\n";
            continue;
        }
        return t;
    }
}

// --------------------------------------------------
// Estructuras de datos (listas enlazadas simples)
// --------------------------------------------------
struct Patient {
    std::string name;
    std::string document;
    std::string phone;
};

struct AvailableAppointment {
    int id; // identificador unico
    std::string doctor;
    std::string date; // formato string "dd/mm/yyyy hh:mm" por simplicidad
};

struct BookedAppointment {
    int id;
    Patient patient;
    std::string doctor;
    std::string date;
};

struct WaitlistEntry {
    int appointmentId; // id de la cita ocupada que se esta "poniendo en espera"
    Patient patient;
    WaitlistEntry* next;
    WaitlistEntry(int aid, const Patient& p): appointmentId(aid), patient(p), next(nullptr) {}
};

struct NodeAvailable {
    AvailableAppointment data;
    NodeAvailable* next;
    NodeAvailable(const AvailableAppointment &a): data(a), next(nullptr) {}
};

struct NodeBooked {
    BookedAppointment data;
    NodeBooked* next;
    NodeBooked(const BookedAppointment &b): data(b), next(nullptr) {}
};

struct NodeCanceled {
    BookedAppointment data;
    NodeCanceled* next;
    NodeCanceled(const BookedAppointment &b): data(b), next(nullptr) {}
};

struct NodeReprogrammed {
    BookedAppointment oldData; // cita anterior
    BookedAppointment newData; // nueva cita
    NodeReprogrammed* next;
    NodeReprogrammed(const BookedAppointment &o, const BookedAppointment &n): oldData(o), newData(n), next(nullptr) {}
};

struct Turn {
    int number;
    Patient patient;
    Turn* next;
    Turn(int num, const Patient& p): number(num), patient(p), next(nullptr) {}
};

// --------------------------------------------------
// Listas (cabezas)
// --------------------------------------------------
NodeAvailable* headAvailable = nullptr;
NodeBooked* headBooked = nullptr;
NodeCanceled* headCanceled = nullptr;
NodeReprogrammed* headReprogrammed = nullptr;
/* WAITLIST ahora mantiene head y tail para comportamiento FIFO */
WaitlistEntry* headWaitlist = nullptr;
WaitlistEntry* tailWaitlist = nullptr;

Turn* headTurn = nullptr;
Turn* tailTurn = nullptr;
int nextTurnNumber = 1;

// --------------------------------------------------
// Funciones auxiliares para gestionar memoria y listas
// --------------------------------------------------
void pushAvailable(const AvailableAppointment &a) {
    NodeAvailable* n = new NodeAvailable(a);
    n->next = headAvailable;
    headAvailable = n;
}

void pushBooked(const BookedAppointment &b) {
    NodeBooked* n = new NodeBooked(b);
    n->next = headBooked;
    headBooked = n;
}

void pushCanceled(const BookedAppointment &b) {
    NodeCanceled* n = new NodeCanceled(b);
    n->next = headCanceled;
    headCanceled = n;
}

void pushReprogrammed(const BookedAppointment &oldB, const BookedAppointment &newB) {
    NodeReprogrammed* n = new NodeReprogrammed(oldB, newB);
    n->next = headReprogrammed;
    headReprogrammed = n;
}

/* pushWaitlist ahora agrega al final (tail) para FIFO */
void pushWaitlist(int appointmentId, const Patient &p) {
    WaitlistEntry* n = new WaitlistEntry(appointmentId, p);
    if (!headWaitlist) {
        headWaitlist = tailWaitlist = n;
    } else {
        tailWaitlist->next = n;
        tailWaitlist = n;
    }
}

void pushTurn(const Patient &p) {
    Turn* t = new Turn(nextTurnNumber++, p);
    if (!headTurn) {
        headTurn = tailTurn = t;
    } else {
        tailTurn->next = t;
        tailTurn = t;
    }
}

// busca en disponibles por id, devuelve puntero y su anterior (para eliminación si hace falta)
NodeAvailable* findAvailableById(int id, NodeAvailable*& prev) {
    prev = nullptr;
    NodeAvailable* cur = headAvailable;
    while (cur) {
        if (cur->data.id == id) return cur;
        prev = cur;
        cur = cur->next;
    }
    return nullptr;
}

// busca en agendadas por id
NodeBooked* findBookedById(int id, NodeBooked*& prev) {
    prev = nullptr;
    NodeBooked* cur = headBooked;
    while (cur) {
        if (cur->data.id == id) return cur;
        prev = cur;
        cur = cur->next;
    }
    return nullptr;
}

// buscar por documento (todas las citas agendadas para ese documento)
void findBookedByDocument(const std::string &doc) {
    NodeBooked* cur = headBooked;
    bool any = false;
    while (cur) {
        if (cur->data.patient.document == doc) {
            any = true;
            std::cout << "Id: " << cur->data.id << " - Medico: " << cur->data.doctor
                      << " - Fecha: " << cur->data.date << " - Paciente: " << cur->data.patient.name << "\n";
        }
        cur = cur->next;
    }
    if (!any) std::cout << "No se encontraron citas agendadas para el documento " << doc << ".\n";
}

void showAvailable() {
    NodeAvailable* cur = headAvailable;
    if (!cur) { std::cout << "No hay citas disponibles.\n"; return; }
    std::cout << "Citas disponibles (Id - Medico - Fecha):\n";
    while (cur) {
        std::cout << cur->data.id << " - " << cur->data.doctor << " - " << cur->data.date << "\n";
        cur = cur->next;
    }
}

void showDoctorsSchedule() {
    std::cout << "Estos son nuestros medicos:\n";
    // Podríamos listar dinámicamente; por simplicidad mostramos horarios ejemplo
    std::cout << "1) Dr. Pepe Perez. Horario: Lunes a Viernes 8:00 a.m a 5:00 p.m.\n";
    std::cout << "2) Dra. Ana Gomez. Horario: Lunes a Viernes 9:00 a.m a 6:00 p.m.\n";
    std::cout << "3) Dr. Luis Martinez. Horario: Martes y Jueves 8:00 a.m a 4:00 p.m.\n";
    std::cout << "4) Dr. Carlos Ruiz. Horario: Lunes a Viernes 7:00 a.m a 3:00 p.m.\n";
    std::cout << "5) Dra. Sofia Morales. Horario: Lunes a Viernes 10:00 a.m a 6:00 p.m.\n";
}

// elimina un nodo de disponibles (usado cuando se reserva)
void removeAvailableNode(NodeAvailable* node, NodeAvailable* prev) {
    if (!node) return;
    if (!prev) headAvailable = node->next;
    else prev->next = node->next;
    delete node;
}

// elimina nodo agendado (se usará al cancelar)
void removeBookedNode(NodeBooked* node, NodeBooked* prev) {
    if (!node) return;
    if (!prev) headBooked = node->next;
    else prev->next = node->next;
    delete node;
}

// asigna la primer persona en lista de espera de esa cita, si existe
bool assignFromWaitlist(int appointmentId, BookedAppointment &assigned) {
    WaitlistEntry* cur = headWaitlist;
    WaitlistEntry* prev = nullptr;
    WaitlistEntry* found = nullptr;
    WaitlistEntry* foundPrev = nullptr;
    // Recorremos desde head hacia tail: el primer match que encontremos sera el mas antiguo (FIFO)
    while (cur) {
        if (cur->appointmentId == appointmentId) {
            found = cur;
            foundPrev = prev;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    if (!found) return false;
    // crear booked appointment con la info del paciente de la waitlist
    assigned.id = appointmentId;
    assigned.doctor = "Asignado (desde lista de espera)";
    assigned.date = "Fecha asignada automaticamente";
    assigned.patient = found->patient;
    // remover found de la waitlist, actualizar head/tail segun corresponda
    if (!foundPrev) {
        // found es head
        headWaitlist = found->next;
        if (!headWaitlist) tailWaitlist = nullptr; // lista quedó vacía
    } else {
        foundPrev->next = found->next;
        if (found == tailWaitlist) tailWaitlist = foundPrev;
    }
    delete found;
    return true;
}

// imprimir lista de espera para una cita
void showWaitlistForAppointment(int appointmentId) {
    WaitlistEntry* cur = headWaitlist;
    bool any = false;
    while (cur) {
        if (cur->appointmentId == appointmentId) {
            any = true;
            std::cout << " - " << cur->patient.name << " (Doc: " << cur->patient.document << ", Tel: " << cur->patient.phone << ")\n";
        }
        cur = cur->next;
    }
    if (!any) std::cout << "No hay lista de espera para la cita " << appointmentId << ".\n";
}

void showAllBooked() {
    NodeBooked* cur = headBooked;
    if (!cur) { std::cout << "No hay citas agendadas.\n"; return; }
    std::cout << "Citas agendadas:\n";
    while (cur) {
        std::cout << "Id: " << cur->data.id << " - Medico: " << cur->data.doctor << " - Fecha: " << cur->data.date
                  << " - Paciente: " << cur->data.patient.name << " - Documento: " << cur->data.patient.document << "\n";
        cur = cur->next;
    }
}

void showCanceled() {
    NodeCanceled* cur = headCanceled;
    if (!cur) { std::cout << "No hay citas canceladas.\n"; return; }
    std::cout << "Citas canceladas:\n";
    while (cur) {
        std::cout << "Id: " << cur->data.id << " - Medico: " << cur->data.doctor << " - Fecha: " << cur->data.date
                  << " - Paciente: " << cur->data.patient.name << "\n";
        cur = cur->next;
    }
}

void showReprogrammed() {
    NodeReprogrammed* cur = headReprogrammed;
    if (!cur) { std::cout << "No hay citas reprogramadas.\n"; return; }
    std::cout << "Citas reprogramadas (antes -> despues):\n";
    while (cur) {
        std::cout << "Id: " << cur->oldData.id << " - " << cur->oldData.doctor << " " << cur->oldData.date
                  << " -> " << cur->newData.doctor << " " << cur->newData.date
                  << " - Paciente: " << cur->oldData.patient.name << "\n";
        cur = cur->next;
    }
}

void showTurns() {
    Turn* cur = headTurn;
    if (!cur) { std::cout << "No hay turnos reservados.\n"; return; }
    std::cout << "Turnos para medicamentos:\n";
    while (cur) {
        std::cout << "#" << cur->number << " - " << cur->patient.name << " (Doc: " << cur->patient.document << ")\n";
        cur = cur->next;
    }
}

// --------------------------------------------------
// Nueva funcion: ver turnos asignados a una persona por identificacion
// --------------------------------------------------
void viewTurnsByDocument() {
    try {
        std::cout << "== Ver turnos por documento ==\n";
        std::string doc = readDigitsOnly("Ingrese su Documento de Identidad (solo numeros): ");
        Turn* cur = headTurn;
        bool any = false;
        while (cur) {
            if (cur->patient.document == doc) {
                any = true;
                std::cout << "Turno #" << cur->number
                          << " - Paciente: " << cur->patient.name
                          << " - Documento: " << cur->patient.document
                          << " - Telefono: " << cur->patient.phone << "\n";
            }
            cur = cur->next;
        }
        if (!any) {
            std::cout << "No se encontraron turnos asignados para el documento " << doc << ".\n";
        }
    } catch (std::exception &e) {
        std::cout << "Ocurrio un error: " << e.what() << "\n";
    }
}

// --------------------------------------------------
// Funcionalidades del menu
// --------------------------------------------------

// 1. Programación de citas médicas
void scheduleAppointment() {
    try {
        std::cout << "== Programacion de citas medicas ==\n";
        showAvailable();
        int id = readInt("Ingrese el idCita que desea reservar: ");
        NodeAvailable* prev = nullptr;
        NodeAvailable* slot = findAvailableById(id, prev);
        if (!slot) {
            std::cout << "No existe una cita disponible con id " << id << ".\n";
            return;
        }
        Patient p;
        p.name = readNonEmpty("Ingrese Nombre: ");
        /* ahora validamos que documento y telefono sean solo numeros */
        p.document = readDigitsOnly("Ingrese Documento de Identidad (solo numeros): ");
        p.phone = readDigitsOnly("Ingrese Numero de contacto (solo numeros): ");
        // crear booked appointment con info del slot
        BookedAppointment b;
        b.id = slot->data.id;
        b.doctor = slot->data.doctor;
        b.date = slot->data.date;
        b.patient = p;
        // agregar a agendadas y remover de disponibles
        pushBooked(b);
        removeAvailableNode(slot, prev);
        std::cout << "Cita Agendada correctamente:\n";
        std::cout << "Id: " << b.id << " - Paciente: " << b.patient.name << " - Documento: " << b.patient.document
                  << " - Telefono: " << b.patient.phone << " - Fecha: " << b.date << " - Medico: " << b.doctor << "\n";
    } catch (std::exception &e) {
        std::cout << "Ocurrio un error: " << e.what() << "\n";
    }
}

// 2. Horarios de atención de los médicos
void showSchedules() {
    try {
        std::cout << "== Horarios de atencion de los medicos ==\n";
        showDoctorsSchedule();
    } catch (std::exception &e) {
        std::cout << "Ocurrio un error: " << e.what() << "\n";
    }
}

// 3. Cancelación de citas médicas (versión corregida)
void cancelAppointment() {
    try {
        std::cout << "== Cancelacion de citas medicas ==\n";
        std::string name = readNonEmpty("Ingrese nombre: ");
        std::string doc = readDigitsOnly("Ingrese Documento de Identidad (solo numeros): ");
        std::string phone = readDigitsOnly("Ingrese Telefono (solo numeros): ");
        int id = readInt("Ingrese IdCita de la cita que desea cancelar: ");

        NodeBooked* prev = nullptr;
        NodeBooked* target = findBookedById(id, prev);
        if (!target) {
            std::cout << "No existe una cita agendada con id " << id << ".\n";
            return;
        }
        if (target->data.patient.document != doc) {
            std::cout << "El documento no coincide con la reserva. Operacion cancelada.\n";
            return;
        }

        // Guardamos una copia de la cita que vamos a cancelar (doctor/fecha/ID)
        BookedAppointment canceledData = target->data;

        // Pasar a canceladas y remover de agendadas
        pushCanceled(canceledData);
        removeBookedNode(target, prev);
        std::cout << "La cita fue cancelada exitosamente.\n";

        // Intentar asignar desde la lista de espera (FIFO)
        BookedAppointment newlyAssigned;
        if (assignFromWaitlist(id, newlyAssigned)) {
            // Usar doctor y date de la cita cancelada (mantener consistencia)
            newlyAssigned.id = id;
            newlyAssigned.doctor = canceledData.doctor;
            newlyAssigned.date = canceledData.date;
            pushBooked(newlyAssigned);
            std::cout << "Se asigno la cita cancelada a persona en lista de espera: " << newlyAssigned.patient.name << "\n";
        } else {
            // No hay nadie en lista de espera -> la cita vuelve a estar disponible
            AvailableAppointment freed;
            freed.id = canceledData.id;
            freed.doctor = canceledData.doctor;
            freed.date = canceledData.date;
            pushAvailable(freed);
            std::cout << "La cita ha quedado disponible nuevamente (Id " << freed.id << ").\n";
        }

    } catch (std::exception &e) {
        std::cout << "Ocurrio un error: " << e.what() << "\n";
    }
}

// 4. Reprogramación de citas (ahora preserva doctor/date del slot liberado)
void rescheduleAppointment() {
    try {
        std::cout << "== Reprogramacion de citas ==\n";
        std::string name = readNonEmpty("Ingrese nombre: ");
        std::string doc = readDigitsOnly("Ingrese Documento de Identidad (solo numeros): ");
        std::string phone = readDigitsOnly("Ingrese Telefono (solo numeros): ");
        int id = readInt("Ingrese el numero/IdCita de la cita que desea Reprogramar: ");

        NodeBooked* prev = nullptr;
        NodeBooked* target = findBookedById(id, prev);
        if (!target) {
            std::cout << "No existe una cita agendada con id " << id << ".\n";
            return;
        }
        if (target->data.patient.document != doc) {
            std::cout << "El documento no coincide con la reserva. Operacion cancelada.\n";
            return;
        }

        // Guardar info de la cita antigua antes de moverla
        BookedAppointment oldBooked = target->data;

        std::cout << "Citas disponibles para reprogramar:\n";
        showAvailable();
        int newId = readInt("Ingrese el idCita nuevo que desea seleccionar: ");
        NodeAvailable* prevAvail = nullptr;
        NodeAvailable* newSlot = findAvailableById(newId, prevAvail);
        if (!newSlot) {
            std::cout << "No existe la cita disponible con id " << newId << ".\n";
            return;
        }
        // crear nueva booked con misma info del paciente
        BookedAppointment newBooked;
        newBooked.id = newSlot->data.id;
        newBooked.doctor = newSlot->data.doctor;
        newBooked.date = newSlot->data.date;
        newBooked.patient = oldBooked.patient; // mantener paciente
        // mover target antiguo a reprogramadas
        pushReprogrammed(oldBooked, newBooked);
        // remover target de agendadas
        removeBookedNode(target, prev);
        // remover newSlot de disponibles y agregar newBooked a agendadas
        removeAvailableNode(newSlot, prevAvail);
        pushBooked(newBooked);
        // la cita anterior (id) pasa a estar disponible nuevamente con su doctor y date originales
        AvailableAppointment freed;
        freed.id = oldBooked.id; // reutilizamos el id anterior como slot disponible
        freed.doctor = oldBooked.doctor;
        freed.date = oldBooked.date;
        pushAvailable(freed);
        std::cout << "Reprogramacion realizada correctamente. Nueva cita Id: " << newBooked.id << " - " << newBooked.date << "\n";

    } catch (std::exception &e) {
        std::cout << "Ocurrio un error: " << e.what() << "\n";
    }
}

// 5. Reserva de turnos para reclamar medicamentos
void reserveTurn() {
    try {
        std::cout << "== Reserva de turnos para reclamar medicamentos ==\n";
        Patient p;
        p.name = readNonEmpty("Ingrese Nombre: ");
        p.document = readDigitsOnly("Ingrese Documento de Identidad (solo numeros): ");
        p.phone = readDigitsOnly("Ingrese Numero de contacto (solo numeros): ");
        pushTurn(p);
        std::cout << "Turno asignado correctamente. Su numero es: " << nextTurnNumber - 1 << "\n";
        int opt = readInt("Desea reservar otro turno? (1=Si, 0=No): ");
        if (opt == 1) reserveTurn();
    } catch (std::exception &e) {
        std::cout << "Ocurrio un error: " << e.what() << "\n";
    }
}

// 6. Consultar citas programadas
void consultAppointments() {
    try {
        std::cout << "== Consultar citas programadas ==\n";
        std::string doc = readDigitsOnly("Ingrese su Documento de Identidad (solo numeros): ");
        std::cout << "-> Citas agendadas:\n";
        findBookedByDocument(doc);
        std::cout << "-> Citas canceladas:\n";
        // mostrar canceladas para este doc
        NodeCanceled* ccur = headCanceled;
        bool any = false;
        while (ccur) {
            if (ccur->data.patient.document == doc) {
                any = true;
                std::cout << "Id: " << ccur->data.id << " - Medico: " << ccur->data.doctor << " - Fecha: " << ccur->data.date << "\n";
            }
            ccur = ccur->next;
        }
        if (!any) std::cout << "No hay citas canceladas para este documento.\n";

        std::cout << "-> Citas reprogramadas:\n";
        NodeReprogrammed* rcur = headReprogrammed;
        any = false;
        while (rcur) {
            if (rcur->oldData.patient.document == doc || rcur->newData.patient.document == doc) {
                any = true;
                std::cout << "Antes (Id " << rcur->oldData.id << "): " << rcur->oldData.doctor << " " << rcur->oldData.date
                          << " -> Ahora (Id " << rcur->newData.id << "): " << rcur->newData.doctor << " " << rcur->newData.date << "\n";
            }
            rcur = rcur->next;
        }
        if (!any) std::cout << "No hay reprogramaciones registradas para este documento.\n";

    } catch (std::exception &e) {
        std::cout << "Ocurrio un error: " << e.what() << "\n";
    }
}

// 7. Reserva de Citas Ocupadas (lista de espera) - ahora lista de medicos y seleccion por numero
void reserveOccupied() {
    try {
        std::cout << "== Reserva de citas ocupadas (lista de espera) ==\n";

        // Construir lista única de médicos (tanto de disponibles como de agendadas)
        std::set<std::string> doctorSet;
        NodeAvailable* av = headAvailable;
        while (av) { doctorSet.insert(av->data.doctor); av = av->next; }
        NodeBooked* bk = headBooked;
        while (bk) { doctorSet.insert(bk->data.doctor); bk = bk->next; }

        if (doctorSet.empty()) {
            std::cout << "No hay medicos registrados en el sistema.\n";
            return;
        }

        // pasar a vector para indexado
        std::vector<std::string> doctors;
        for (const auto &d : doctorSet) doctors.push_back(d);

        // mostrar lista numerada
        std::cout << "Medicos disponibles (seleccione por numero):\n";
        for (size_t i = 0; i < doctors.size(); ++i) {
            std::cout << i+1 << ") " << doctors[i] << "\n";
        }

        int sel = readInt("Ingrese el numero del medico que desea (por ejemplo 1): ");
        if (sel < 1 || sel > (int)doctors.size()) {
            std::cout << "Seleccion invalida.\n";
            return;
        }
        std::string chosenDoctor = doctors[sel-1];

        // mostramos citas que ya estan agendadas (ocupadas) para el medico elegido
        std::cout << "Citas ocupadas actualmente para " << chosenDoctor << ":\n";
        NodeBooked* cur = headBooked;
        bool any = false;
        while (cur) {
            if (cur->data.doctor == chosenDoctor) {
                any = true;
                std::cout << "Id: " << cur->data.id << " - " << cur->data.doctor << " - " << cur->data.date
                          << " - Paciente: " << cur->data.patient.name << "\n";
            }
            cur = cur->next;
        }
        if (!any) {
            std::cout << "No se encontraron citas ocupadas para ese medico.\n";
            return;
        }

        int id = readInt("Ingrese el id de la cita ocupada en la que desea anotarse (lista de espera): ");
        // comprobar que exista la cita ocupada y que corresponda al medico elegido
        NodeBooked* prev = nullptr;
        NodeBooked* target = findBookedById(id, prev);
        if (!target) {
            std::cout << "No existe una cita agendada con id " << id << ".\n";
            return;
        }
        if (target->data.doctor != chosenDoctor) {
            std::cout << "La cita seleccionada no pertenece al medico elegido. Operacion cancelada.\n";
            return;
        }

        Patient p;
        p.name = readNonEmpty("Ingrese Nombre: ");
        p.document = readDigitsOnly("Ingrese Documento de Identidad (solo numeros): ");
        p.phone = readDigitsOnly("Ingrese Numero de contacto (solo numeros): ");
        pushWaitlist(id, p); // ahora encola al final (FIFO)
        std::cout << "Se ha inscrito en la lista de espera para la cita " << id << ".\n";
        int opt = readInt("Desea anotarse en otra lista de espera? (1=Si, 0=No): ");
        if (opt == 1) reserveOccupied();
    } catch (std::exception &e) {
        std::cout << "Ocurrio un error: " << e.what() << "\n";
    }
}

// 8. Salir del programa (se maneja en el main)

// --------------------------------------------------
// Inicializacion: crear algunos slots disponibles y doctores extra
// --------------------------------------------------
void initSampleData() {
    // generamos algunos id's unicos para disponibles (incluyendo 2 medicos mas)
    AvailableAppointment a1{1, "Dr. Pepe Perez", "15/08/2025 09:00"};
    AvailableAppointment a2{2, "Dra. Ana Gomez", "15/08/2025 10:00"};
    AvailableAppointment a3{3, "Dr. Luis Martinez", "16/08/2025 11:00"};
    AvailableAppointment a4{4, "Dr. Pepe Perez", "17/08/2025 08:30"};
    AvailableAppointment a5{5, "Dr. Carlos Ruiz", "18/08/2025 09:15"};     // nuevo medico
    AvailableAppointment a6{6, "Dra. Sofia Morales", "19/08/2025 14:00"};  // nuevo medico
    pushAvailable(a6);
    pushAvailable(a5);
    pushAvailable(a4);
    pushAvailable(a3);
    pushAvailable(a2);
    pushAvailable(a1);

    // ejemplo de booked (ocupadas)
    BookedAppointment b1;
    b1.id = 100;
    b1.doctor = "Dr. Pepe Perez";
    b1.date = "15/08/2025 11:00";
    b1.patient = {"Juan Perez", "12345", "300111222"};
    pushBooked(b1);

    BookedAppointment b2;
    b2.id = 101;
    b2.doctor = "Dra. Ana Gomez";
    b2.date = "16/08/2025 09:30";
    b2.patient = {"Maria Lopez", "67890", "300333444"};
    pushBooked(b2);

    // citas ocupadas adicionales para los nuevos medicos
    BookedAppointment b3;
    b3.id = 102;
    b3.doctor = "Dr. Carlos Ruiz";
    b3.date = "18/08/2025 09:15";
    b3.patient = {"Andres Torres", "112233", "300555666"};
    pushBooked(b3);

    BookedAppointment b4;
    b4.id = 103;
    b4.doctor = "Dra. Sofia Morales";
    b4.date = "19/08/2025 14:00";
    b4.patient = {"Lucia Diaz", "445566", "300777888"};
    pushBooked(b4);
}

// --------------------------------------------------
// Limpiar memoria al final (liberar nodos)
// --------------------------------------------------
void cleanupAll() {
    while (headAvailable) {
        NodeAvailable* t = headAvailable;
        headAvailable = headAvailable->next;
        delete t;
    }
    while (headBooked) {
        NodeBooked* t = headBooked;
        headBooked = headBooked->next;
        delete t;
    }
    while (headCanceled) {
        NodeCanceled* t = headCanceled;
        headCanceled = headCanceled->next;
        delete t;
    }
    while (headReprogrammed) {
        NodeReprogrammed* t = headReprogrammed;
        headReprogrammed = headReprogrammed->next;
        delete t;
    }
    while (headWaitlist) {
        WaitlistEntry* t = headWaitlist;
        headWaitlist = headWaitlist->next;
        delete t;
    }
    tailWaitlist = nullptr;
    while (headTurn) {
        Turn* t = headTurn;
        headTurn = headTurn->next;
        delete t;
    }
}

// --------------------------------------------------
// Main
// --------------------------------------------------
int main() {
    try {
        initSampleData();
        bool running = true;
        while (running) {
            std::cout << "\n====== MENU PRINCIPAL ======\n";
            std::cout << "1. Programacion de citas medicas\n";
            std::cout << "2. Horarios de atencion de los medicos\n";
            std::cout << "3. Cancelacion de citas medicas\n";
            std::cout << "4. Reprogramacion de citas\n";
            std::cout << "5. Reserva de turnos para reclamar medicamentos\n";
            std::cout << "6. Consultar citas programadas\n";
            std::cout << "7. Reserva de citas ocupadas (lista de espera)\n";
            std::cout << "8. Ver turnos asignados a una persona (por documento)\n";
            std::cout << "9. Salir\n";
            int opt = readInt("Ingrese una opcion (1-9): ");
            switch (opt) {
                case 1: scheduleAppointment(); break;
                case 2: showSchedules(); break;
                case 3: cancelAppointment(); break;
                case 4: rescheduleAppointment(); break;
                case 5: reserveTurn(); break;
                case 6: consultAppointments(); break;
                case 7: reserveOccupied(); break;
                case 8: viewTurnsByDocument(); break;
                case 9:
                    std::cout << "Saliendo del programa...\n";
                    running = false;
                    break;
                default:
                    std::cout << "Opcion invalida. Intente nuevamente.\n";
            }
        }
        cleanupAll();
    } catch (std::exception &e) {
        std::cout << "Error fatal: " << e.what() << "\n";
    }
    return 0;
}