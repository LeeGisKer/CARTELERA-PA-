#ifndef NODOS_H
#define NODOS_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <Windows.h>
#include <tchar.h>

using namespace std;


const int MAX_EMPLOYEES = 100;
const int MAX_SALAS = 50;

struct EmployeeData {
    TCHAR name[50];
    TCHAR rfc[13];
    TCHAR photoPath[MAX_PATH];
    TCHAR username[24];
    TCHAR password[24];
    EmployeeData* prev;
    EmployeeData* next;
};

struct NodoFuncion {
    char nombrePelicula[50];
    char poster[200];
    int duracion;
    char clasificacion[3];
    char idioma[10];
    int sala;
    int cantidadDeAsientos;
    int hora;
    double precioBoleto;
    int fechaInicioProyeccion;
    int fechaFinProyeccion;
    NodoFuncion* anterior;
    NodoFuncion* siguiente;
};

struct SaleData {
    TCHAR fecha[11]; 
    TCHAR pelicula[50];
    int cantidad;
    float total;
    SaleData* prev;
    SaleData* next;
};

struct SalaData {
    TCHAR nombre[50];
    int asientos;
};

extern EmployeeData* g_employeeHead;
extern EmployeeData* g_employeeTail;
extern SaleData* g_salesHead;
extern SaleData* g_salesTail;

void AddEmployee(EmployeeData* employee);
void AddSale(SaleData* sale);
void SaveEmployeeData(EmployeeData* head);
EmployeeData* LoadEmployeeData();
void SaveSaleData(SaleData* head);
SaleData* LoadSaleData();

#endif

