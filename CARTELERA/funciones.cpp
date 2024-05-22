#include "nodos.h"
#include <fstream>
#include <strsafe.h>
#include <shlwapi.h> // Para PathRemoveFileSpec
#pragma comment(lib, "Shlwapi.lib")

EmployeeData* g_employeeHead = nullptr;
EmployeeData* g_employeeTail = nullptr;
SaleData* g_salesHead = nullptr;
SaleData* g_salesTail = nullptr;

TCHAR* GetExecutablePath()
{
    static TCHAR path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);
    PathRemoveFileSpec(path); // Elimina el nombre del ejecutable dejando solo la ruta
    return path;
}

void AddEmployee(EmployeeData* employee)
{
    employee->prev = g_employeeTail;
    employee->next = nullptr;
    if (g_employeeTail) {
        g_employeeTail->next = employee;
    }
    else {
        g_employeeHead = employee;
    }
    g_employeeTail = employee;
}

void AddSale(SaleData* sale)
{
    sale->prev = g_salesTail;
    sale->next = nullptr;
    if (g_salesTail) {
        g_salesTail->next = sale;
    }
    else {
        g_salesHead = sale;
    }
    g_salesTail = sale;
}

void SaveEmployeeData(EmployeeData* head)
{
    TCHAR path[MAX_PATH];
    _stprintf_s(path, _T("%s\\employeeData.bin"), GetExecutablePath());

    std::ofstream outFile(path, std::ios::binary);
    if (outFile.is_open()) {
        EmployeeData* current = head;
        while (current) {
            outFile.write(reinterpret_cast<const char*>(current), sizeof(EmployeeData));
            current = current->next;
        }
        outFile.close();
    }
}

EmployeeData* LoadEmployeeData()
{
    TCHAR path[MAX_PATH];
    _stprintf_s(path, _T("%s\\employeeData.bin"), GetExecutablePath());

    std::ifstream inFile(path, std::ios::binary);
    if (inFile.is_open()) {
        EmployeeData* head = nullptr;
        EmployeeData* tail = nullptr;
        while (true) {
            EmployeeData* employee = new EmployeeData;
            if (!inFile.read(reinterpret_cast<char*>(employee), sizeof(EmployeeData))) {
                delete employee;
                break;
            }
            employee->prev = tail;
            employee->next = nullptr;
            if (tail) {
                tail->next = employee;
            }
            else {
                head = employee;
            }
            tail = employee;
        }
        inFile.close();
        return head;
    }
    return nullptr;
}

void SaveSaleData(SaleData* head)
{
    TCHAR path[MAX_PATH];
    _stprintf_s(path, _T("%s\\salesData.bin"), GetExecutablePath());

    std::ofstream outFile(path, std::ios::binary);
    if (outFile.is_open()) {
        SaleData* current = head;
        while (current) {
            outFile.write(reinterpret_cast<const char*>(current), sizeof(SaleData));
            current = current->next;
        }
        outFile.close();
    }
}

SaleData* LoadSaleData()
{
    TCHAR path[MAX_PATH];
    _stprintf_s(path, _T("%s\\salesData.bin"), GetExecutablePath());

    std::ifstream inFile(path, std::ios::binary);
    if (inFile.is_open()) {
        SaleData* head = nullptr;
        SaleData* tail = nullptr;
        while (true) {
            SaleData* sale = new SaleData;
            if (!inFile.read(reinterpret_cast<char*>(sale), sizeof(SaleData))) {
                delete sale;
                break;
            }
            sale->prev = tail;
            sale->next = nullptr;
            if (tail) {
                tail->next = sale;
            }
            else {
                head = sale;
            }
            tail = sale;
        }
        inFile.close();
        return head;
    }
    return nullptr;
}
