export interface Firmware {
  [key: string]: any;
  //pk
  id: number;
  //unique
  name: string;
  updatedAt: number;
  createdAt: number;
  // in bytes
  fileSize: number;
  fileName: string;
}
