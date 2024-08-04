const digest = require(`crypto`)

module.exports = (data) => {
  return digest.createHash(`md5`).update(data).digest(`hex`)
}