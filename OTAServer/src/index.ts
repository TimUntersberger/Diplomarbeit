import "dotenv/config";
import bodyParser from "body-parser";
import cors from "cors";
import express from "express";
import FirmwareRouter from "./router/firmwareRouter";
import * as Config from "./config";

const app = express();

app.use(cors());
app.use(bodyParser());

app.use(FirmwareRouter);


app.listen(Config.getPort(), () => console.log("running"));
