import pkg from 'whatsapp-web.js';
const { Client, LocalAuth } = pkg;
import qrcode from 'qrcode-terminal';

import { addDosen, showDosen, removeDosen } from './dosenutils.js';

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
    if (message.body.trim().toLowerCase() === '!menu') {
        const menuMessage = 
`Menu Utama:
!jadwalmingguan
!jadwalharian
!jadwalsekarang

Perkuliahan:
!notifikasi`;
        message.reply(menuMessage);
        return;
    }
    if (message.body.startsWith('!adddosen ')) {
        // Format: !adddosen hari, nama,kode,kelas
        const args = message.body.slice(10).split(',');
        if (args.length !== 4) {
            message.reply('Format salah! Gunakan: !adddosen hari,nama,kode,kelas');
            return;
        }
        const [hari, nama, kode, kelas] = args.map(s => s.trim());
        const result = addDosen(hari, nama, kode, kelas);
        if (result && result.status === 'duplicate') {
            message.reply(result.message);
        } else if (result && result.status === 'ok') {
            message.reply(result.message);
        } else if (result && result.status === 'error') {
            message.reply(result.message);
        }
    }

    if (message.body.startsWith('!showdosen ')) {
        // Format: !showdosen hari
        const hari = message.body.slice(11).trim();
        if (!hari) {
            message.reply('Format salah! Gunakan: !showdosen hari');
            return;
        }
        const result = showDosen(hari);
        message.reply(result);
    }

    if (message.body.startsWith('!removedosen ')) {
        // Format: !removedosen hari,namaAtauKode
        const args = message.body.slice(13).split(',');
        if (args.length !== 2) {
            message.reply('Format salah! Gunakan: !removedosen hari,namaAtauKode');
            return;
        }
        const [hari, namaOrKode] = args.map(s => s.trim());
        const result = removeDosen(hari, namaOrKode);
        message.reply(result);
    }
});

client.initialize();
