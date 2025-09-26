import fs from 'fs';
import pkg from 'whatsapp-web.js';
const { Client, LocalAuth } = pkg;
import qrcode from 'qrcode-terminal';

import { addDosen, showDosen, removeDosen } from './dosenutils.js';
import moment from 'moment-timezone';

const client = new Client({
    authStrategy: new LocalAuth()
});

client.on('qr', (qr) => {
    qrcode.generate(qr, { small: true });
    console.log('Scan the QR code above with WhatsApp.');
});

client.on('ready', () => {
    console.log('WhatsApp bot is ready!');
});


client.on('message', async message => {
    // Handle absen command
    if (message.body.trim().toLowerCase() === '!menu') {
        const menuMessage = 
        `Menu Utama:
!jadwalmingguan
!jadwalharian
!jadwalsekarang

Perkuliahan:
!absen
!notifikasi`;
message.reply(menuMessage);
return;
}
if (message.body.startsWith('!adddosen ')) {
    // Format: !adddosen hari|nama|kode|kelas|jam_kelas|matkul
        const args = message.body.slice(10).split('|');
        if (args.length !== 6) {
            message.reply('Format salah! Gunakan: !adddosen hari|nama|kode|kelas|jam_kelas|matkul');
            return;
        }
        const [hari, nama, kode, kelas, jam_kelas, matkul] = args.map(s => s.trim());
        const result = addDosen(hari, nama, kode, kelas, jam_kelas, matkul);
        if (result && result.status === 'duplicate') {
            message.reply(result.message);
        } else if (result && result.status === 'ok') {
            message.reply(result.message);
        } else if (result && result.status === 'error') {
            message.reply(result.message);
        }
    }
    
    if (message.body.startsWith('!showdosen ')) {
        // Format: !showdosen hari OR !showdosen all
        const hari = message.body.slice(11).trim();
        if (!hari) {
            message.reply('Format salah! Gunakan: !showdosen hari|all');
            return;
        }
        // Load absen data
        let absenData = {};
        const absenPath = './absen.json';
        if (fs.existsSync(absenPath)) {
            absenData = JSON.parse(fs.readFileSync(absenPath, 'utf8'));
        }
        function addAbsenEmoji(hari, result) {
            return result.split('\n').map(line => {
                // Try to extract kode
                const kodeMatch = line.match(/\(([^,]+),/);
                let kode = kodeMatch ? kodeMatch[1].trim() : null;
                if (!kode) return '⏭️ ' + line; // skip if can't find kode
                if (absenData[hari] && absenData[hari][kode]) return '✅ ' + line;
                return '❌ ' + line;
            }).join('\n');
        }
        if (hari.toLowerCase() === 'all') {
            let allMsg = '';
            const days = ['Senin','Selasa','Rabu','Kamis','Jumat'];
            for (const d of days) {
                let result = showDosen(d);
                result = addAbsenEmoji(d, result);
                allMsg += `*${d.charAt(0).toUpperCase() + d.slice(1)}*\n` + result + '\n\n';
            }
            message.reply(allMsg.trim());
        } else {
            let result = showDosen(hari);
            result = addAbsenEmoji(hari, result);
            message.reply(result);
        }
    }
    
    if (message.body.startsWith('!removedosen ')) {
        // Format: !removedosen hari|namaAtauKode
        const args = message.body.slice(13).split('|');
        if (args.length !== 2) {
            message.reply('Format salah! Gunakan: !removedosen hari|namaAtauKode');
            return;
        }
        const [hari, namaOrKode] = args.map(s => s.trim());
        const result = removeDosen(hari, namaOrKode);
        message.reply(result);
    }
    
    if (message.body.startsWith('!absen ')) {
        // Format: !absen hari|kode
        const args = message.body.slice(7).split('|');
        if (args.length !== 2) {
            message.reply('Format salah! Gunakan: !absen hari|kode');
            return;
        }
        const [hari, kode] = args.map(s => s.trim());
        let absenData = {};
        const absenPath = './absen.json';
        if (fs.existsSync(absenPath)) {
            absenData = JSON.parse(fs.readFileSync(absenPath, 'utf8'));
        }
        if (!absenData[hari]) absenData[hari] = {};
        absenData[hari][kode] = true;
        fs.writeFileSync(absenPath, JSON.stringify(absenData, null, 2));
        message.reply(`Absen untuk ${kode} di hari ${hari} berhasil!`);
        return;
    }
});

client.initialize();