import express from "express";
import path from "path";
import fs from "fs";
import * as FirmwareRepo from "../repo/firmwareRepo";
import multer from "multer";

const router = express.Router();

const upload = multer({});

router.get("/firmware", async (req, res) => {
  const firmwares = await FirmwareRepo.findAll();
  res.json(firmwares);
});

router.post("/firmware", async (req, res) => {
  try {
    const firmware = await FirmwareRepo.create(req.body);
    res.json(firmware);
  } catch (ex) {
    if (ex.code === "23505")
      res.status(400).json({
        type: "FIRMWARE_NAME_UNIQUE_VIOLATED",
        error: "Name is already being used"
      });
    else res.status(500).end();
  }
});

router.patch("/firmware/:id", async (req, res) => {
  const changeset = req.body;
  const id = Number(req.params.id);
  const firmware = await FirmwareRepo.update(id, changeset);

  if (!firmware) {
    res.status(404).json({
      error: "The firmware was not found"
    });
    return;
  }

  res.json({});
});

router.get("/firmware/:id/updatedAt", async (req, res) => {
  const firmware =
    typeof req.params.id == "number"
      ? await FirmwareRepo.findById(req.params.id)
      : await FirmwareRepo.findByName(req.params.id);

  if (!firmware) {
    res.status(404).json({
      error: "The firmware was not found"
    });
  } else {
    res.send(firmware.updatedAt);
  }
});

router.get("/firmware/:id/download", async (req, res) => {
  const firmware =
    typeof req.params.id == "number"
      ? await FirmwareRepo.findById(req.params.id)
      : await FirmwareRepo.findByName(req.params.id, {
          caseInsensitive: true
        });

  if (!firmware) {
    res.status(404).json({
      error: "The firmware was not found"
    });
  } else {
    res.sendFile(
      path.join(
        __dirname,
        "..",
        "./firmwares/",
        firmware.id.toString(),
        firmware.fileName
      )
    );
  }
});

router.post("/firmware/:id/upload", upload.single("file"), async (req, res) => {
  const id = Number(req.params.id);
  const firmware = await FirmwareRepo.findById(id);

  if (!firmware) {
    res.status(404).json({
      error: "The firmware was not found"
    });
    return;
  }

  const dirPath = path.join(__dirname, "..", "firmwares");
  const firmwarePath = path.join(dirPath, firmware.id.toString());
  const filePath = path.join(firmwarePath, firmware.fileName);
  if (!fs.existsSync(dirPath)) fs.mkdirSync(dirPath);
  if (!fs.existsSync(firmwarePath)) fs.mkdirSync(firmwarePath);
  fs.writeFileSync(filePath, req.file.buffer);

  res.end();
});

export default router;
