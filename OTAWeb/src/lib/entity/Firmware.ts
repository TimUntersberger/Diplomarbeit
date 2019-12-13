export interface Firmware {
  [key: string]: any;

  id: number;
  name: string;
  updatedAt: number;
  createdAt: number;
  // in bytes
  fileSize: number;
  fileName: string;
}
