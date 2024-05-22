#include <Windows.h>
#include <tchar.h>
#include <commdlg.h>
#include <strsafe.h>
#include <objidl.h>
#include <gdiplus.h>
#include <fstream>
#include <shlwapi.h> 
#include <commctrl.h> 

#pragma comment(lib, "Gdiplus.lib")
#pragma comment(lib, "Comctl32.lib") 
#pragma comment(lib, "Shlwapi.lib") 

using namespace Gdiplus;

#include "resource.h"
#include "nodos.h"  


EmployeeData g_employees[MAX_EMPLOYEES];
int g_employeeCount = 0;
EmployeeData g_currentEmployee;
HBITMAP g_hCurrentEmployeePhoto = NULL;

SalaData g_salas[MAX_SALAS];
int g_salaCount = 0;

ULONG_PTR gdiplusToken;
TCHAR g_szPhotoPath[MAX_PATH] = { 0 };

void SaveEmployeeData(EmployeeData employees[], int count);
bool LoadEmployeeData(EmployeeData employees[], int& count);
void SaveSalaData(SalaData salas[], int count);
bool LoadSalaData(SalaData salas[], int& count);

void UpdateSalaList(HWND hDlg);
void UpdateEmployeeInfoControls(HWND hDlg);



INT_PTR CALLBACK LoginDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK MainMenuDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK EmployeeRegisterDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK EmployeeInfoDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK SalaListDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK SalesReportDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam); 


TCHAR* GetExecutablePath()
{
    static TCHAR path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);
    PathRemoveFileSpec(path); 
    return path;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    InitCommonControls();

    
    if (!LoadEmployeeData(g_employees, g_employeeCount)) {
        g_employeeCount = 0;
    }
    if (!LoadSalaData(g_salas, g_salaCount)) {
        g_salaCount = 0;
    }

    
    if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_LOGIN_DIALOG), NULL, LoginDlgProc) == IDOK)
    {
        
        DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN_MENU_DIALOG), NULL, MainMenuDlgProc);
    }

    GdiplusShutdown(gdiplusToken);

    
    SaveEmployeeData(g_employees, g_employeeCount);
    SaveSalaData(g_salas, g_salaCount);

    return 0;
}

void UpdateEmployeeInfoControls(HWND hDlg)
{
    SetDlgItemText(hDlg, IDC_STATIC_EMPLOYEE_NAME, g_currentEmployee.name);
    if (g_hCurrentEmployeePhoto)
    {
        SendDlgItemMessage(hDlg, IDC_STATIC_EMPLOYEE_PHOTO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)g_hCurrentEmployeePhoto);
    }
    else
    {
        HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, g_currentEmployee.photoPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        if (hBitmap)
        {
            g_hCurrentEmployeePhoto = hBitmap;
            SendDlgItemMessage(hDlg, IDC_STATIC_EMPLOYEE_PHOTO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)g_hCurrentEmployeePhoto);
        }
    }
}

void SaveEmployeeData(EmployeeData employees[], int count)
{
    TCHAR path[MAX_PATH];
    _stprintf_s(path, _T("%s\\employeeData.bin"), GetExecutablePath());

    std::ofstream outFile(path, std::ios::binary);
    if (outFile.is_open()) {
        outFile.write(reinterpret_cast<const char*>(&count), sizeof(count));
        outFile.write(reinterpret_cast<const char*>(employees), sizeof(EmployeeData) * count);
        outFile.close();
    }
}

bool LoadEmployeeData(EmployeeData employees[], int& count)
{
    TCHAR path[MAX_PATH];
    _stprintf_s(path, _T("%s\\employeeData.bin"), GetExecutablePath());

    std::ifstream inFile(path, std::ios::binary);
    if (inFile.is_open()) {
        inFile.read(reinterpret_cast<char*>(&count), sizeof(count));
        inFile.read(reinterpret_cast<char*>(employees), sizeof(EmployeeData) * count);
        inFile.close();
        return true;
    }
    return false;
}

void SaveSalaData(SalaData salas[], int count)
{
    TCHAR path[MAX_PATH];
    _stprintf_s(path, _T("%s\\salaData.bin"), GetExecutablePath());

    std::ofstream outFile(path, std::ios::binary);
    if (outFile.is_open()) {
        outFile.write(reinterpret_cast<const char*>(&count), sizeof(count));
        outFile.write(reinterpret_cast<const char*>(salas), sizeof(SalaData) * count);
        outFile.close();
    }
}

bool LoadSalaData(SalaData salas[], int& count)
{
    TCHAR path[MAX_PATH];
    _stprintf_s(path, _T("%s\\salaData.bin"), GetExecutablePath());

    std::ifstream inFile(path, std::ios::binary);
    if (inFile.is_open()) {
        inFile.read(reinterpret_cast<char*>(&count), sizeof(count));
        inFile.read(reinterpret_cast<char*>(salas), sizeof(SalaData) * count);
        inFile.close();
        return true;
    }
    return false;
}

void UpdateSalaList(HWND hDlg)
{
    HWND hList = GetDlgItem(hDlg, IDC_LIST_SALAS);
    HWND hNoSalas = GetDlgItem(hDlg, IDC_STATIC_NO_SALAS);

    if (g_salaCount == 0) {
        ShowWindow(hList, SW_HIDE);
        ShowWindow(hNoSalas, SW_SHOW);
    }
    else {
        ShowWindow(hNoSalas, SW_HIDE);
        ShowWindow(hList, SW_SHOW);
        ListView_DeleteAllItems(hList);

        LVITEM lvItem;
        lvItem.mask = LVIF_TEXT;
        for (int i = 0; i < g_salaCount; ++i) {
            lvItem.iItem = i;
            lvItem.iSubItem = 0;
            lvItem.pszText = g_salas[i].nombre;
            ListView_InsertItem(hList, &lvItem);

            lvItem.iSubItem = 1;
            TCHAR buffer[10];
            _stprintf_s(buffer, _T("%d"), g_salas[i].asientos);
            lvItem.pszText = buffer;
            ListView_SetItem(hList, &lvItem);
        }
    }
}

INT_PTR CALLBACK SalaListDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        HWND hList = GetDlgItem(hDlg, IDC_LIST_SALAS);
        LVCOLUMN lvCol;
        lvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
        lvCol.cx = 120;
        wchar_t colName1[] = TEXT("Nombre de la Sala");
        lvCol.pszText = colName1;
        ListView_InsertColumn(hList, 0, &lvCol);

        lvCol.cx = 50;
        wchar_t colName2[] = TEXT("Asientos");
        lvCol.pszText = colName2;
        ListView_InsertColumn(hList, 1, &lvCol);

        UpdateSalaList(hDlg);
        return (INT_PTR)TRUE;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK LoginDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            if (LOWORD(wParam) == IDOK)
            {
                
                TCHAR username[24];
                TCHAR password[24];
                GetDlgItemText(hDlg, IDC_EDIT_USERNAME, username, 24);
                GetDlgItemText(hDlg, IDC_EDIT_PASSWORD, password, 24);

                
                bool authenticated = false;
                if ((_tcscmp(username, TEXT("admin")) == 0 && _tcscmp(password, TEXT("admin")) == 0)) {
                    authenticated = true;
                }
                else {
                    for (int i = 0; i < g_employeeCount; ++i) {
                        if (_tcscmp(username, g_employees[i].username) == 0 && _tcscmp(password, g_employees[i].password) == 0) {
                            g_currentEmployee = g_employees[i];
                            g_hCurrentEmployeePhoto = (HBITMAP)LoadImage(NULL, g_currentEmployee.photoPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
                            authenticated = true;
                            break;
                        }
                    }
                }

                if (authenticated) {
                    MessageBox(hDlg, TEXT("Inicio de sesión exitoso"), TEXT("Login"), MB_OK);
                    EndDialog(hDlg, IDOK);  
                }
                else {
                    MessageBox(hDlg, TEXT("Usuario o contraseña incorrectos"), TEXT("Login"), MB_OK);
                }
            }
            else
            {
                EndDialog(hDlg, IDCANCEL);  
            }
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK MainMenuDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        
        HMENU hMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MAINMENU));
        SetMenu(hDlg, hMenu);
        UpdateEmployeeInfoControls(hDlg); 
        EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_INFO), TRUE); 
        EnableWindow(GetDlgItem(hDlg, IDC_BUTTON_EMPLEADO), FALSE); 
        return (INT_PTR)TRUE;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON_INFO:
        case IDC_MENU_INFOEMPLEADO:
            DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_EMPLOYEE_INFO_DIALOG), hDlg, EmployeeInfoDlgProc);
            UpdateEmployeeInfoControls(hDlg); 
            break;
        case IDC_BUTTON_SALASREGISTRO:
        case IDC_MENU_SALASREGISTRO:
            MessageBox(hDlg, TEXT("Registro de Salas"), TEXT("Main Menu"), MB_OK);
            break;
        case IDC_BUTTON_SALASLISTA:
        case IDC_MENU_SALASLISTA:
            DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SALA_LIST_DIALOG), hDlg, SalaListDlgProc);
            break;
        case IDC_BUTTON_FUNCIONESREGISTRO:
        case IDC_MENU_FUNCIONESREGISTRO:
            MessageBox(hDlg, TEXT("Registro de Funciones"), TEXT("Main Menu"), MB_OK);
            break;
        case IDC_BUTTON_REPORTEVENTAS:
        case IDC_MENU_REPORTEVENTAS:
            DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SALES_REPORT_DIALOG), hDlg, SalesReportDlgProc);
            break;
        case IDC_BUTTON_SALIR:
        case IDC_MENU_EXIT:
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


INT_PTR CALLBACK EmployeeRegisterDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        UpdateEmployeeInfoControls(hDlg);
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_BUTTON_SELECT_PHOTO)
        {
            OPENFILENAME ofn;
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hDlg;
            ofn.lpstrFile = g_szPhotoPath;
            ofn.lpstrFile[0] = '\0';
            ofn.nMaxFile = sizeof(g_szPhotoPath);
            ofn.lpstrFilter = TEXT("Bitmaps\0*.bmp\0");
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileName(&ofn) == TRUE)
            {
                HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, g_szPhotoPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
                if (hBitmap)
                {
                    SendDlgItemMessage(hDlg, IDC_STATIC_PHOTO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
                    if (g_hCurrentEmployeePhoto)
                    {
                        DeleteObject(g_hCurrentEmployeePhoto);
                    }
                    g_hCurrentEmployeePhoto = hBitmap;
                }
                else
                {
                    MessageBox(hDlg, TEXT("Error al cargar la imagen."), TEXT("Error"), MB_OK | MB_ICONERROR);
                }
            }
            return (INT_PTR)TRUE;
        }

        if (LOWORD(wParam) == IDOK)
        {
            if (g_employeeCount >= MAX_EMPLOYEES) {
                MessageBox(hDlg, TEXT("No se pueden registrar más empleados."), TEXT("Error"), MB_OK | MB_ICONERROR);
                return (INT_PTR)FALSE;
            }

            EmployeeData& newEmployee = g_employees[g_employeeCount];
            GetDlgItemText(hDlg, IDC_EDIT_NOMBRE, newEmployee.name, 50);
            GetDlgItemText(hDlg, IDC_EDIT_RFC, newEmployee.rfc, 13);
            StringCchCopy(newEmployee.photoPath, MAX_PATH, g_szPhotoPath);
            GetDlgItemText(hDlg, IDC_EDIT_CLAVE, newEmployee.username, 24);
            GetDlgItemText(hDlg, IDC_EDIT_CONTRASENA, newEmployee.password, 24);

            // Validar nombre completo (solo letras y espacios)
            for (int i = 0; i < _tcslen(newEmployee.name); ++i) {
                if (!_istalpha(newEmployee.name[i]) && newEmployee.name[i] != ' ') {
                    MessageBox(hDlg, TEXT("El nombre solo debe contener letras y espacios."), TEXT("Error"), MB_OK | MB_ICONERROR);
                    return (INT_PTR)FALSE;
                }
            }

            // Validar RFC (4 letras seguidas de 6 dígitos)
            if (_tcslen(newEmployee.rfc) != 10 || !_istalpha(newEmployee.rfc[0]) || !_istalpha(newEmployee.rfc[1]) || !_istalpha(newEmployee.rfc[2]) || !_istalpha(newEmployee.rfc[3]) ||
                !_istdigit(newEmployee.rfc[4]) || !_istdigit(newEmployee.rfc[5]) || !_istdigit(newEmployee.rfc[6]) || !_istdigit(newEmployee.rfc[7]) || !_istdigit(newEmployee.rfc[8]) || !_istdigit(newEmployee.rfc[9])) {
                MessageBox(hDlg, TEXT("El RFC debe tener 4 letras seguidas de 6 dígitos."), TEXT("Error"), MB_OK | MB_ICONERROR);
                return (INT_PTR)FALSE;
            }

            // Actualizar la foto del empleado
            if (g_hCurrentEmployeePhoto)
            {
                DeleteObject(g_hCurrentEmployeePhoto);
            }
            g_hCurrentEmployeePhoto = (HBITMAP)LoadImage(NULL, newEmployee.photoPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

            g_employeeCount++;
            EndDialog(hDlg, IDOK);
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, IDCANCEL);
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK EmployeeInfoDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        SetDlgItemText(hDlg, IDC_EDIT_INFO_NAME, g_currentEmployee.name);
        SetDlgItemText(hDlg, IDC_EDIT_INFO_RFC, g_currentEmployee.rfc);
        SetDlgItemText(hDlg, IDC_EDIT_INFO_USERNAME, g_currentEmployee.username);
        SetDlgItemText(hDlg, IDC_EDIT_INFO_PASSWORD, g_currentEmployee.password);
        if (g_hCurrentEmployeePhoto)
        {
            SendDlgItemMessage(hDlg, IDC_STATIC_INFO_PHOTO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)g_hCurrentEmployeePhoto);
        }
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_BUTTON_INFO_SELECT_PHOTO)
        {
            OPENFILENAME ofn;
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hDlg;
            ofn.lpstrFile = g_szPhotoPath;
            ofn.lpstrFile[0] = '\0';
            ofn.nMaxFile = sizeof(g_szPhotoPath);
            ofn.lpstrFilter = TEXT("Bitmaps\0*.bmp\0");
            ofn.nFilterIndex = 1;
            ofn.lpstrFileTitle = NULL;
            ofn.nMaxFileTitle = 0;
            ofn.lpstrInitialDir = NULL;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileName(&ofn) == TRUE)
            {
                HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, g_szPhotoPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
                if (hBitmap)
                {
                    SendDlgItemMessage(hDlg, IDC_STATIC_INFO_PHOTO, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
                    if (g_hCurrentEmployeePhoto)
                    {
                        DeleteObject(g_hCurrentEmployeePhoto);
                    }
                    g_hCurrentEmployeePhoto = hBitmap;
                }
            }
            return (INT_PTR)TRUE;
        }

        if (LOWORD(wParam) == IDOK)
        {
            GetDlgItemText(hDlg, IDC_EDIT_INFO_NAME, g_currentEmployee.name, 50);
            GetDlgItemText(hDlg, IDC_EDIT_INFO_RFC, g_currentEmployee.rfc, 13);
            GetDlgItemText(hDlg, IDC_EDIT_INFO_USERNAME, g_currentEmployee.username, 24);
            GetDlgItemText(hDlg, IDC_EDIT_INFO_PASSWORD, g_currentEmployee.password, 24);

            // Guardar cambios en la estructura de empleados
            for (int i = 0; i < g_employeeCount; ++i)
            {
                if (_tcscmp(g_employees[i].username, g_currentEmployee.username) == 0)
                {
                    g_employees[i] = g_currentEmployee;
                    break;
                }
            }

            EndDialog(hDlg, IDOK);
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, IDCANCEL);
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK SalesReportDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
    {
        HWND hList = GetDlgItem(hDlg, IDC_LIST_SALES);
        LVCOLUMN lvCol;
        lvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

        lvCol.cx = 100;
        wchar_t colName1[] = TEXT("Película");
        lvCol.pszText = colName1;
        ListView_InsertColumn(hList, 0, &lvCol);

        lvCol.cx = 50;
        wchar_t colName2[] = TEXT("Cantidad");
        lvCol.pszText = colName2;
        ListView_InsertColumn(hList, 1, &lvCol);

        lvCol.cx = 70;
        wchar_t colName3[] = TEXT("Total");
        lvCol.pszText = colName3;
        ListView_InsertColumn(hList, 2, &lvCol);

        return (INT_PTR)TRUE;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_BUTTON_GENERATE_REPORT)
        {
            SYSTEMTIME st;
            TCHAR fecha[11];
            DateTime_GetSystemtime(GetDlgItem(hDlg, IDC_DATEPICKER), &st);
            _stprintf_s(fecha, _T("%04d-%02d-%02d"), st.wYear, st.wMonth, st.wDay);

            HWND hList = GetDlgItem(hDlg, IDC_LIST_SALES);
            ListView_DeleteAllItems(hList);

            bool salesFound = false;
            LVITEM lvItem;
            lvItem.mask = LVIF_TEXT;
            SaleData* current = g_salesHead;
            while (current) {
                if (_tcscmp(current->fecha, fecha) == 0) {
                    salesFound = true;

                    lvItem.iItem = ListView_GetItemCount(hList);
                    lvItem.iSubItem = 0;
                    lvItem.pszText = current->pelicula;
                    ListView_InsertItem(hList, &lvItem);

                    lvItem.iSubItem = 1;
                    TCHAR buffer[10];
                    _stprintf_s(buffer, _T("%d"), current->cantidad);
                    lvItem.pszText = buffer;
                    ListView_SetItem(hList, &lvItem);

                    lvItem.iSubItem = 2;
                    _stprintf_s(buffer, _T("%.2f"), current->total);
                    lvItem.pszText = buffer;
                    ListView_SetItem(hList, &lvItem);
                }
                current = current->next;
            }

            if (!salesFound) {
                MessageBox(hDlg, TEXT("No hay ventas registradas para la fecha seleccionada."), TEXT("Reporte de Ventas"), MB_OK | MB_ICONINFORMATION);
            }

            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, IDCANCEL);
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}



