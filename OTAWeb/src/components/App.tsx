import dayjs from "dayjs";
import { Firmware } from "lib/entity/Firmware";
import * as FirmwareService from "lib/services/firmware";
import React, { useEffect, useState } from "react";
import { useDropzone } from "react-dropzone";
import { FaArrowDown, FaArrowUp } from "react-icons/fa";
import useInput from "react-use-input";
import styled from "styled-components";
import { Button, IconButton } from "./Base/Button";
import { Dropzone } from "./Base/Dropzone";
import { Input as BaseInput } from "./Base/Input";
import { Modal as BaseModal, ModalActions, ModalContent } from "./Base/Modal";
import {
  Table as BaseTable,
  TableActions,
  TableBody,
  TableData,
  TableHead,
  TableHeader,
  TableRow
} from "./Base/Table";

export const Container = styled.div`
  --primary-color-one: #2d8632;
  --primary-color-two: #9cdaa0;
  --primary-color-three: #76da7c;
  --primary-color-four: #062d08;
  --primary-color-five: #002d03;

  height: calc(100vh - 100px);
  /* - 417px because of the 200px padding and I think 17px for the scrollbar */
  width: calc(100vw - 417px);
  padding: 50px 200px;

  display: flex;
  flex-direction: column;
  align-items: center;

  font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Oxygen,
    Ubuntu, Cantarell, "Open Sans", "Helvetica Neue", sans-serif;
`;

const Table = styled(BaseTable)`
  width: 100%;
`;

const Modal = styled(BaseModal)`
  justify-content: center;
  align-items: center;
  padding: 15px 30px;
  padding-top: 30px;
`;

const Input = styled(BaseInput)`
  margin-bottom: 10px;
`;

const timestampToDateString = (timestamp: number): String =>
  dayjs.unix(timestamp).format("DD.MM.YYYY HH:mm:ss");

const tableHeaders = [
  { name: "Name", sortable: false },
  { name: "File Name", sortable: true },
  { name: "File Size", sortable: true, alignRight: true },
  { name: "Created At", sortable: true },
  { name: "Updated At", sortable: true },
  { name: "", sortable: false }
];

const App = () => {
  const [firmwares, setFirmwares] = useState<Firmware[]>([]);

  const { getRootProps, getInputProps, acceptedFiles } = useDropzone({
    multiple: false
  });

  const [sortedBy, setSortedBy] = useState<number | null>(null);
  const [isSortReversed, setIsSortReversed] = useState(false);

  const [isModalOpen, setIsModalOpen] = useState(false);
  const [isUpdateModalOpen, setIsUpdateModalOpen] = useState(false);

  const [firmwareName, setFirmwareName, setFirmwareNameValue] = useInput("");
  const [firmware, setFirmware] = useState<Firmware | null>(null);

  const toggleSort = (index: number): void => {
    setSortedBy(index);
    if (index == sortedBy) {
      setIsSortReversed(!isSortReversed);
    } else {
      setIsSortReversed(false);
    }
  };

  useEffect(() => {
    if (sortedBy === null) return;

    const sortedFirmwares = firmwares.sort((a, b) => {
      const key = Object.keys(a)[sortedBy];
      const aValue = a[key];
      const bValue = b[key];
      let result = 0;

      if (typeof aValue === "number") {
        result = bValue - aValue;
      }

      return isSortReversed ? result * -1 : result;
    });

    setFirmwares(sortedFirmwares);
  }, [sortedBy, isSortReversed]);

  useEffect(() => {
    fetchFirmwares();
  }, []);

  const onCancel = () => {
    setIsModalOpen(false);
    setFirmwareNameValue("");
    acceptedFiles.pop();
  };

  const onCancelUpdate = () => {
    setIsUpdateModalOpen(false);
    setFirmwareNameValue("");
    setFirmware(null);
    acceptedFiles.pop();
  };

  const fetchFirmwares = async () => {
    const firmwares = await FirmwareService.getFirmwares();
    setFirmwares(firmwares);
  };

  const onUpdate = async () => {
    const file = acceptedFiles[0];

    const now = Math.round(Date.now() / 1000);
    const newFirmware: Firmware = {
      ...firmware,
      updatedAt: now,
      fileName: file.name,
      fileSize: file.size
    } as any;

    const res = await FirmwareService.updateFirmware(newFirmware);
    FirmwareService.uploadFirmware(newFirmware.id, file);

    fetchFirmwares();

    onCancelUpdate();
  };

  const onEditFirmware = (fw: Firmware) => {
    setFirmwareNameValue(fw.name);
    setIsUpdateModalOpen(true);
    setFirmware(fw);
  };

  const onUpload = async () => {
    const file = acceptedFiles[0];
    const now = Math.round(Date.now() / 1000);
    const newFirmware = {
      id: -1,
      createdAt: now,
      updatedAt: now,
      fileName: file.name,
      fileSize: file.size,
      name: firmwareName
    };

    const res = await FirmwareService.addFirmware(newFirmware);
    FirmwareService.uploadFirmware(res.id, file);

    fetchFirmwares();

    onCancel();
  };

  return (
    <Container>
      <Modal visible={isUpdateModalOpen}>
        <ModalContent>
          <Input type="text" disabled value={firmwareName} />
          <Dropzone {...getRootProps()}>
            <input {...getInputProps()} />
            <p>Drag 'n' drop some files here, or click to select files</p>
          </Dropzone>
          {acceptedFiles[0] && (
            <div>
              <p>Filename: {acceptedFiles[0].name}</p>
              <p>Filesize: {acceptedFiles[0].size} Bytes</p>
            </div>
          )}
          <ModalActions>
            <Button flat onClick={onCancel}>
              Cancel
            </Button>
            <Button onClick={onUpdate} disabled={!acceptedFiles[0]}>
              Upload
            </Button>
          </ModalActions>
        </ModalContent>
      </Modal>
      <Modal visible={isModalOpen}>
        <ModalContent>
          <Input type="text" value={firmwareName} onChange={setFirmwareName} />
          <Dropzone {...getRootProps()}>
            <input {...getInputProps()} />
            <p>Drag 'n' drop some files here, or click to select files</p>
          </Dropzone>
          {acceptedFiles[0] && (
            <div>
              <p>Filename: {acceptedFiles[0].name}</p>
              <p>Filesize: {acceptedFiles[0].size} Bytes</p>
            </div>
          )}
          <ModalActions>
            <Button flat onClick={onCancel}>
              Cancel
            </Button>
            <Button
              onClick={onUpload}
              disabled={!acceptedFiles[0] || firmwareName === ""}
            >
              Upload
            </Button>
          </ModalActions>
        </ModalContent>
      </Modal>
      <TableActions>
        <Button onClick={() => setIsModalOpen(true)}>Add Firmware</Button>
      </TableActions>
      <Table>
        <TableHead>
          <TableRow>
            {tableHeaders.map((header, i) => (
              <TableHeader
                key={i}
                sortable={header.sortable}
                alignRight={header.alignRight}
                onClick={header.sortable ? () => toggleSort(i) : () => { }}
              >
                {header.name}
                <FaArrowUp
                  style={{
                    marginLeft: "10px",
                    fontSize: "0.8em",
                    display:
                      sortedBy == i && !isSortReversed ? "inherit" : "none"
                  }}
                ></FaArrowUp>
                <FaArrowDown
                  style={{
                    marginLeft: "10px",
                    fontSize: "0.8em",
                    display:
                      sortedBy == i && isSortReversed ? "inherit" : "none"
                  }}
                ></FaArrowDown>
              </TableHeader>
            ))}
          </TableRow>
        </TableHead>
        <TableBody>
          {firmwares.map((fw, i) => (
            <TableRow key={i}>
              <TableData>{fw.name}</TableData>
              <TableData>{fw.fileName}</TableData>
              <TableData isNumber>{fw.fileSize} B</TableData>
              <TableData>{timestampToDateString(fw.createdAt)}</TableData>
              <TableData>{timestampToDateString(fw.updatedAt)}</TableData>
              <TableData actions>
                <IconButton onClick={() => onEditFirmware(fw)}>
                  <FaArrowUp></FaArrowUp>
                </IconButton>
              </TableData>
            </TableRow>
          ))}
        </TableBody>
      </Table>
    </Container>
  );
};

export default App;
