import ky from "ky";
import * as Config from "lib/config";
import { Firmware } from "lib/entity/Firmware";

const api = ky.create({
  prefixUrl: Config.getBaseUrl()
});

export function addFirmware(firmware: Firmware): Promise<any> {
  return api
    .post("firmware", {
      json: firmware
    })
    .json();
}

export function updateFirmware(firmware: Firmware): Promise<any> {
  return api
    .patch(`firmware/${firmware.id}`, {
      json: firmware
    })
    .json();
}

export function uploadFirmware(id: number, file: File): Promise<any> {
  const fd = new FormData();
  fd.append("file", file);
  return api.post(`firmware/${id}/upload`, {
    body: fd
  });
}

export function getFirmwares(): Promise<Firmware[]> {
  return api.get("firmware").json();
}
