#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <sstream>

using namespace std;
using json = nlohmann::json;

string toLowerCase(string s)
{
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

int displayMenu()
{
    int choiceMenu;
    string hariInput;
    while (true)
    {
        cout << "=========================\n";
        cout << "   Dosen Sorting Menu    \n";
        cout << "=========================\n\n";
        cout << "Jadwal:\n";
        cout << "1. Lihat Jadwal Seminggu\n";
        cout << "2. Lihat Jadwal 1 Hari\n";
        cout << "3. Lihat Jadwal Terdekat\n\n";
        cout << "Perkuliahan:\n";
        cout << "3. Notifikasi Dosen\n\n";
        cout << "Others:\n";
        cout << "0. Exit\n";
        cout << "Choose an option: ";

        cin >> choiceMenu;
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Please enter a number.\n\n";
            continue;
        }
        if (choiceMenu < 0 || choiceMenu > 3)
        {
            cout << "Choice only from 0 - 3. Please try again.\n\n";
            continue;
        }
        break;
    }
    return choiceMenu;
}

json readDosenJson(const string &filename)
{
    ifstream f(filename);
    if (!f.is_open())
    {
        cerr << "Gagal membuka file: " << filename << endl;
        return json();
    }
    json dosenData;
    try
    {
        f >> dosenData;
    }
    catch (const exception &e)
    {
        cerr << "Gagal parse JSON: " << e.what() << endl;
        return json();
    }
    return dosenData;
}

void send1Minggu(const string &filename)
{
    json dosenData = readDosenJson(filename);

    for (auto &[hari, arr] : dosenData.items())
    {
        string tempHari = hari;
        if (!tempHari.empty())
        {
            tempHari[0] = toupper(tempHari[0]);
        }

        cout << "Hari: " << tempHari << endl;
        for (const auto &d : arr)
        {
            cout << "  Nama: " << d["nama"]
                 << ", Kode: " << d["kode"]
                 << ", Kelas: " << d["kelas"]
                 << ", Jam Kelas: " << d["jam_kelas"] << endl;
        }
    }
}

void send1Hari(const string &filename)
{
    json dosenData = readDosenJson(filename);

    string hariInput;
    cout << "Masukkan hari: ";
    cin >> hariInput;
    hariInput = toLowerCase(hariInput);

    string tempHari = hariInput;
    if (!tempHari.empty())
    {
        tempHari[0] = toupper(tempHari[0]);
    }

    if (dosenData.contains(hariInput))
    {
        cout << "Jadwal untuk hari " << tempHari << ":\n";
        for (const auto &d : dosenData[hariInput])
        {
            cout << "  Nama: " << d["nama"]
                 << ", Kode: " << d["kode"]
                 << ", Kelas: " << d["kelas"]
                 << ", Jam Kelas: " << d["jam_kelas"] << endl;
        }
    }
    else
    {
        cout << "Tidak ada jadwal untuk hari " << tempHari << ".\n";
    }
}