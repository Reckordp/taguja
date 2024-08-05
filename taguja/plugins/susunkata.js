let timeout = 60000
let susunkata = {}
let handler = (m) => {
  let id = m.chat
  if (id in susunkata) {
    sendMessage(m.chat, { text: `Masih ada soal belum terjawab` })
    return false
  }
  let soalkata = fetch(`https://raw.githubusercontent.com/BochilTeam/database/master/games/susunkata.json`)
    .then((response) => {
      response.json().then((body) => {
        let soal = body[Math.floor(Math.random() * body.length)]
        let caption = ` *[ s u s u n - k a t a ]* 
Susunlah kata dibawah ini.
soal: ${soal.soal}
tipe: ${soal.tipe}

*[ Timeout ]* ${(timeout / 1000).toFixed(2)} detik
*[ Bantuan ]* .suka untuk bantuan

!!! Reply pesan ini untuk menjawab
`.trim()
        sendMessage(m.chat, { text: caption }).then((message) => {
          susunkata[id] = [
            message, 
            soal, 
            setTimeout(() => {
              if (susunkata[id]) {
                let gameOver = `Soal selesai jawabannya ${soal.jawaban}`
                sendMessage(m.chat, { text: gameOver}, { quoted: message })
                delete susunkata[id]
              }
            }, timeout)
          ]
        })
      })
    })
}

module.exports = handler