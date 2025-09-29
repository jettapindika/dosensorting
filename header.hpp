#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>

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
        cout << "2. Lihat Jadwal 1 Hari\n\n";
        cout << "Perkuliahan:\n";
        cout << "3. Absen\n";
        cout << "4. Notifikasi Dosen\n\n";
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

json readAbsenJson()
{
    ifstream f("./absen.json");
    if (!f.is_open())
        return json();
    json absenData;
    try
    {
        f >> absenData;
    }
    catch (...)
    {
        return json();
    }
    return absenData;
}

void writeAbsenJson(const json &absenData)
{
    ofstream f("./absen.json");
    if (f.is_open())
        f << absenData.dump(4);
}

void inputAbsen()
{
    string hari, kode;
    cout << "Masukkan hari absen: ";
    cin >> hari;
    cout << "Masukkan kode matkul: ";
    cin >> kode;
    json absenData = readAbsenJson();
    if (!hari.empty())
    {
        hari = toLowerCase(hari);
        hari[0] = toupper(hari[0]);
    }
    absenData[hari][kode] = true;
    writeAbsenJson(absenData);
    cout << "Absen untuk " << kode << " di hari " << hari << " berhasil!" << endl;
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

    if (dosenData.contains(tempHari) && !dosenData[tempHari].empty())
    {
        json absenData = readAbsenJson();
        cout << "Jadwal untuk hari " << tempHari << ":\n";
        for (const auto &d : dosenData[tempHari])
        {
            string kode = d["kode"];
            string emoji = "❌";
            if (absenData.contains(tempHari) && absenData[tempHari].contains(kode))
                emoji = absenData[tempHari][kode].get<bool>() ? "✅" : "❌";
            else
                emoji = "❌";
            cout << emoji << "  Nama: " << d["nama"]
                 << ", Kode: " << d["kode"]
                 << ", Kelas: " << d["kelas"]
                 << ", Jam Kelas: " << d["jam_kelas"]
                 << ", Matkul: ";
            if (d.contains("matkul"))
                cout << d["matkul"];
            else
                cout << "-";
            cout << endl;
        }
    }
    else
    {
        cout << "Tidak ada jadwal untuk hari " << tempHari << ".\n";
    }
}
int getCurrentDayIdx()
{
    time_t now = time(nullptr);
    tm *ltm = localtime(&now);

    int idx = ltm->tm_wday == 0 ? 6 : ltm->tm_wday - 1;
    return idx;
}

int getCurrentMinutes()
{
    time_t now = time(nullptr);
    tm *ltm = localtime(&now);
    return ltm->tm_hour * 60 + ltm->tm_min;
}

void send1MingguChecklist(const string &filename)
{
    json dosenData = readDosenJson(filename);
    const vector<string> days = {"Senin", "Selasa", "Rabu", "Kamis", "Jumat"};
    int todayIdx = getCurrentDayIdx();
    int nowMinutes = getCurrentMinutes();
    bool adaJadwal = false;
    for (int i = 0; i < days.size(); ++i)
    {
        string hari = days[i];
        cout << "Hari: " << hari << endl;
        if (dosenData.contains(hari) && !dosenData[hari].empty())
        {
            adaJadwal = true;
            for (const auto &d : dosenData[hari])
            {
                string jamKelas = d["jam_kelas"];
                size_t dash = jamKelas.find('-');
                int endMinutes = 0;
                if (dash != string::npos)
                {
                    string end = jamKelas.substr(dash + 1);
                    int h, m;
                    if (sscanf(end.c_str(), "%d:%d", &h, &m) == 2)
                    {
                        endMinutes = h * 60 + m;
                    }
                }
                json absenData = readAbsenJson();
                string kode = d["kode"];
                string emoji = "❌";
                if (absenData.contains(hari) && absenData[hari].contains(kode))
                {
                    emoji = absenData[hari][kode].get<bool>() ? "✅" : "❌";
                }
                else
                {
                    emoji = "❌";
                }
                cout << emoji << "  Nama: " << d["nama"]
                     << ", Kode: " << d["kode"]
                     << ", Kelas: " << d["kelas"]
                     << ", Jam Kelas: " << d["jam_kelas"]
                     << ", Matkul: ";
                if (d.contains("matkul"))
                    cout << d["matkul"];
                cout << endl;
            }
        }
        cout << endl;
    }

    if (!adaJadwal)
    {
        cout << "Tidak ada jadwal minggu ini." << endl;
    }
}