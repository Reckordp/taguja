const { 
	makeWASocket, 
	useMultiFileAuthState, 
	useSingleFileAuthState, 
	jidDecode, 
	areJidsSameUser, 
	DisconnectReason, 
	proto
} = require('baileys')
const pino = require('pino')
const WebSocket = require('ws')
const chalk = require('chalk')
const QRCode = require('qrcode-terminal/vendor/QRCode')
const QRErrorCorrectLevel = require('qrcode-terminal/vendor/QRCode/QRErrorCorrectLevel')
const penampilLogin = require('taguja/login')
const responPesan = require('taguja/respon')
const Nusalogin = require('taguja/terlogin')
const Nusalogout = require('taguja/terlogout')
const Logout = require('taguja/logout')

let isInit = true
function reloadConn(postur, reconnect=false) {
	if (reconnect) {
		try { conn.ws.close() } catch {}
		global.conn = {
			...global.conn, ...makeWASocket(conn.optionSocket)
		}
	}
	if (!isInit) {
		conn.ev.off('connection.update', postur.update)
		conn.ev.off('messages.upsert', postur.upsert)
		conn.ev.off('creds.update', postur.creds)
	}
	conn.ev.on('connection.update', postur.update)
	conn.ev.on('messages.upsert', postur.upsert)
	conn.ev.on('creds.update', postur.creds)
	isInit = false
}

function rapikan_pesan(pesan) {
	let hasil = {}
	let decodeJid = (jid) => {
		if (!jid) return jid
		if (/:\d+@/gi.test(jid)) {
			let decode = jidDecode(jid) || {}
			return decode.user && decode.server && decode.user + '@' + decode.server || jid
		} else return jid
	}
	let getName = (jid) => {
		let v
		if (jid.endsWith('@g.us')) {
			return new Promise(async (resolve) => {
				v = conn.chats[jid] || {}
				if (!(v.name || v.subject)) v = await conn.groupMetadata(jid) || {}
				resolve(v.name || v.subject || ('+' + jid.replace('@s.whatsapp.net', '')))
			})
		} else v = jid === '0@s.whatsapp.net' ? {
			jid,
			vname: 'WhatsApp'
		} : areJidsSameUser(jid, conn.user.id) ?
				conn.user :
				(conn.chats[jid] || {})
		return v.name || v.subject || v.vname || v.notify || v.verifiedName || ('+' + jid.replace('@s.whatsapp.net', ''))
	}
	let parseMentions = (text = '') => {
		return [...text.matchAll(/@([0-9]{5,16}|0)/g)].map(v => v[1] + '@s.whatsapp.net')
	}
	if (pesan.key) {
		let m = pesan
		hasil.id = pesan.key.id
		hasil.chat = decodeJid(pesan.key.remoteJid)
		hasil.sender = decodeJid(m.key.fromMe && conn.user.id || m.participant || m.key.participant || '')
		hasil.subjek = getName(hasil.chat)
		hasil.nama = pesan.pushName ? pesan.pushName : getName(hasil.sender)
		hasil.fromMe = m.key.fromMe || areJidsSameUser(m.sender, conn.user.id)
	}
	if (pesan.message) {
		if (pesan.message.conversation) {
			hasil.text = pesan.message.conversation
		} else if (pesan.message.extendedTextMessage) {
			if (pesan.message.extendedTextMessage.text) {
				hasil.text = pesan.message.extendedTextMessage.text
			} else {
				return undefined
			}
		} else {
			return undefined
		}
	} else {
		return undefined
	}
	hasil.balas = (b) => {
		m = parseMentions(b)
		return conn.sendMessage(hasil.chat, { text: b, mentions: m }, { quoted: pesan })
	}
	if (hasil.subjek.length > 0) {
		conn.chats[hasil.chat] = { id: hasil.chat, name: hasil.subjek }
	}
	if (hasil.nama.length > 0) {
		conn.chats[hasil.sender] = { id: hasil.sender, name: hasil.nama }
	}
	return hasil
}

async function pesanDatang(paket) {
	let tindakLanjut = (pesan) => {
		Promise.wait(new Promise((resolve) => {
			let jawaban = responPesan(pesan)
			if (jawaban && jawaban.pesan) {
				pesan.balas(jawaban.pesan)
			}
			resolve(0)
		}))
	}
	for (var i = paket.messages.length - 1; i >= 0; i--) {
		let pesan = rapikan_pesan(paket.messages[i])
		if (!pesan) {
			continue
		}
		// if ((nl = pesan.text.indexOf("\n")) != -1) {
		// 	pesan.text = pesan.text.substr(0, nl)
		// }
		// if (pesan.fromMe) {
		// 	if (/\.menu/.test(pesan.text)) {
		// 		pesan.balas('Nothing')
		// 	}
		// } else {
		// 	console.log(`${pesan.nama}(${pesan.chat}): ${pesan.text}`)
		// }
		let res = undefined
		if (pesan.subjek.then) {
			res = await pesan.subjek
			pesan.subjek = res
		}
		if (pesan.nama.then) {
			res = await pesan.nama
			pesan.nama = res
		}
		tindakLanjut(pesan)
	}
}

function tampilkan_qrcode(encoded) {
	const builder = new QRCode(-1, QRErrorCorrectLevel.L)
	builder.addData(encoded)
	builder.make()
	const jumlah = builder.getModuleCount()
	const hasil = builder.modules
//   [
//     true,  false, false, false, false, false,
//     true,  false, true,  false, true,  false,
//     false, true,  false, true,  false, false,
//     true,  false, false, true,  true,  true,
//     false, false, false, false, false, false,
//     true,  true,  true,  true,  false, true,
//     false, true,  true,  false, true,  false,
//     false, true,  false, false, false, false,
//     false, true,  false, true,  false
//   ],
//   [
//     true,  true,  true,  true,  true,  true,
//     true,  false, true,  true,  true,  true,
//     false, true,  true,  false, false, false,
//     false, false, true,  true,  false, false,
//     false, true,  true,  true,  false, true,
//     false, false, true,  true,  false, true,
//     false, true,  false, false, false, true,
//     true,  true,  false, false, true,  false,
//     false, true,  false, true,  false
//   ]
// ]

	penampilLogin(jumlah, hasil)
}

async function connUpdate(update) {
	const { 
		receivedPendingNotifications, 
		connection, 
		lastDisconnect, 
		isOnline, 
		isNewLogin, 
		qr
	} = update

	if (qr) tampilkan_qrcode(qr)
	if (isNewLogin) conn.isInit = true
	// if (connection == 'connecting') console.log(chalk.redBright('⚡ Activate the Bot, please wait a moment...'))
	if (connection == 'open') console.log(chalk.green('✅ Connected'))
	if (isOnline == true) console.log(chalk.green('Status Online'))
	if (isOnline == false) console.log(chalk.red('Status Offline'))
	if (receivedPendingNotifications) {
		// console.log(chalk.yellow('Waiting New Messages'))
		// console.log(`Chat as ${conn.user.name} with id ${conn.user.id}`)
		let nama = conn.user.name || 'No Name'
		Nusalogin(nama, conn.user.id)
	}
	// if (connection == 'close') console.log(chalk.red('⏱️ Connection stopped and tried to reconnect...'))
	if (lastDisconnect && lastDisconnect.error) {
		if (lastDisconnect.error.output !== DisconnectReason.loggedOut) {
			if (conn.ws.readyState !== WebSocket.CONNECTING) {
				Nusalogout()
				reloadConn(conn.postur, true)
			}
		}
	}
}

async function makewa() {
	const { state, saveState, saveCreds } = await useMultiFileAuthState('../sessions')
	let connOpt = { 
		auth: state, 
		logger: pino({ level: 'silent' }) , 
		patchMessageBeforeSending: (message) => {
	       const requiresPatch = !!(
	           message.buttonsMessage 
	           || message.templateMessage
	           || message.listMessage
	       )
	       if (requiresPatch) {
	           message = {
	               viewOnceMessage: {
	                   message: {
	                       messageContextInfo: {
	                           deviceListMetadataVersion: 2,
	                           deviceListMetadata: {}
	                       },
	                       ...message
	                   }           
	               }
	           }
	       }
	       return message
	   }
	}
	global.conn = makeWASocket(connOpt)
	conn.optionSocket = connOpt
	conn.chats = {}
	conn.postur = { update: connUpdate, upsert: pesanDatang, creds: saveCreds }
	reloadConn(conn.postur)
}

function perform_logout() {
	if (global.conn) {
		global.conn.logout("Bye.")
		makewa()
	}
}
Logout(perform_logout)

// QRCode.toString('2@HO1/6yLF3u8JJSwGhbhLJv0tCFFs9KgETx3+dEmlPuL2kdo72e/uGE/juk5FGHpAnullg+W9scp1yw==,f+EqcOZFRjXb+sLgpTBTKtYt32FQG8Zm0ZzLhMje7Uw=,oMnidm6FvFPWIgXbJ7N3kEplXFCc2o7tQ2CFHgY2vFQ=,j7Wyfo+DeJPTFhX+UumZM7AVidqT+3DcSFiUa27XRVE=')
makewa()