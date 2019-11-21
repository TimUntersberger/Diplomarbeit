const express = require("express")
const multer = require("multer")
const path = require("path")
const fs = require("fs")

const app = express()
const upload = multer({})

let updateTimestamp = 0

app.get("/", (req, res) => {
  res.sendFile(path.join(__dirname, "./main.html"))
})

app.get("/timestamp", (req, res) => {
  res.send(updateTimestamp)
})

app.get("/download", (req, res) => {
  res.sendFile(path.join(__dirname, "./program"))
})

app.post("/upload", upload.single("file"), (req, res) => {
  fs.writeFileSync(path.join(__dirname, "./program"), req.file.buffer)
  updateTimestamp = Date.now()
  res.end()
})

app.listen(3000, () => console.log("running"))
