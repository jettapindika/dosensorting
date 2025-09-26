import fs from 'fs';

export function addDosen(hari, nama, kode, kelas, jam_kelas, matkul) {
    let dosenData = {};
    try {
        if (fs.existsSync('../cppFile/datadosen.json')) {
            const data = fs.readFileSync('../cppFile/datadosen.json', 'utf8');
            const parsed = JSON.parse(data);
            if (Array.isArray(parsed)) {
                dosenData = {};
            } else {
                dosenData = parsed;
            }
        }
    } catch (err) {
        console.error('Error reading or parsing datadosen.json:', err);
        return { status: 'error', message: 'Terjadi kesalahan saat membaca data dosen.' };
    }
    if (!dosenData[hari]) {
        dosenData[hari] = [];
    }
    const exists = dosenData[hari].some(d => d.nama === nama || d.kode === kode);
    if (exists) {
        return { status: 'duplicate', message: 'Nama atau kode sudah ada di hari tersebut. Masukkan data lain.' };
    }
    dosenData[hari].push({ nama, kode, kelas, jam_kelas, matkul });
    fs.writeFileSync('../cppFile/datadosen.json', JSON.stringify(dosenData, null, 4));
    return { status: 'ok', message: `Dosen ditambahkan di hari ${hari}: ${nama} (${kode}, ${kelas}, Jam: ${jam_kelas}, Matkul: ${matkul})` };
}

export function showDosen(hari) {
    try {
        if (fs.existsSync('../cppFile/datadosen.json')) {
            const data = fs.readFileSync('../cppFile/datadosen.json', 'utf8');
            const dosenData = JSON.parse(data);
            const hariKey = Object.keys(dosenData).find(h => h.toLowerCase() === hari.toLowerCase());
            if (hariKey && dosenData[hariKey].length > 0) {
                return dosenData[hariKey]
                    .map((d, i) => `${i+1}. ${d.nama} (${d.kode}, ${d.kelas}, Jam: ${d.jam_kelas}, Matkul: ${d.matkul || '-'})`)
                    .join('\n');
            } else {
                return 'Tidak ada dosen di hari tersebut.';
            }
        } else {
            return 'Data dosen belum ada.';
        }
    } catch (err) {
        return 'Gagal membaca data dosen.';
    }
}

export function removeDosen(hari, namaOrKode) {
    try {
        if (fs.existsSync('../cppFile/datadosen.json')) {
            const data = fs.readFileSync('../cppFile/datadosen.json', 'utf8');
            const dosenData = JSON.parse(data);
            const hariKey = Object.keys(dosenData).find(h => h.toLowerCase() === hari.toLowerCase());
            if (!hariKey) return 'Hari tidak ditemukan.';
            const before = dosenData[hariKey].length;
            dosenData[hariKey] = dosenData[hariKey].filter(d => d.nama !== namaOrKode && d.kode !== namaOrKode);
            if (dosenData[hariKey].length === before) return 'Dosen tidak ditemukan.';
            fs.writeFileSync('../cppFile/datadosen.json', JSON.stringify(dosenData, null, 4));
            return 'Dosen berhasil dihapus.';
        } else {
            return 'Data dosen belum ada.';
        }
    } catch (err) {
        return 'Gagal menghapus data dosen.';
    }
}
