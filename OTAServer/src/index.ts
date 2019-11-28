import bodyParser from "body-parser";
import cors from "cors";
import { Firmware } from "entity/Firmware";
import express from "express";
import fs from "fs";
import multer from "multer";
import path from "path";
import * as Config from "./config";

const app = express();
const upload = multer({});
let lastId = -1;
const firmwares: Firmware[] = [];

app.use(cors());
app.use(bodyParser());

app.get("/firmware", (req, res) => {
  res.json(firmwares);
});
app.post("/firmware", (req, res) => {
  const firmware = req.body;
  firmwares.push({ ...firmware, id: ++lastId });
  res.json({
    id: lastId
  });
});
app.patch("/firmware/:id", (req, res) => {
  const changeset = req.body;
  const id = Number(req.params.id);
  const firmware = firmwares.find(f => f.id === id);

  if (!firmware) {
    res.status(404).json({
      error: "The firmware was not found"
    });
    return;
  }

  Object.assign(firmware, changeset);

  res.json({});
});
app.get("/firmware/:id/download", (req, res) => {
  const id = Number(req.params.id);
  const firmware = firmwares.find(f => f.id === id);

  if (!firmware) {
    res.status(404).json({
      error: "The firmware was not found"
    });
  } else
    res.sendFile(
      path.join(
        __dirname,
        "..",
        "./firmwares/",
        firmware.id.toString(),
        firmware.fileName
      )
    );
});
app.post("/firmware/:id/upload", upload.single("file"), (req, res) => {
  const id = Number(req.params.id);
  const firmware = firmwares.find(f => f.id === id);

  if (!firmware) {
    res.status(404).json({
      error: "The firmware was not found"
    });
    return;
  }

  const dirPath = path.join(__dirname, "..", "firmwares", firmware.id.toString());
  const filePath = path.join(dirPath, firmware.fileName);
  if (!fs.existsSync(dirPath)) fs.mkdirSync(dirPath);
  fs.writeFileSync(filePath, req.file.buffer);

  res.end();
});

app.listen(Config.getPort(), () => console.log("running"));
