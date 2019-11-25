import dayjs from "dayjs";
import React, { useEffect, useState } from "react";
import { useDropzone } from "react-dropzone";
import { FaArrowDown, FaArrowUp } from "react-icons/fa";
import useInput from "react-use-input";
import styled from "styled-components";
import { Button, IconButton } from "./Base/Button";
import {
  Table as BaseTable,
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

const TableActions = styled.div`
  display: flex;
  width: 100%;
  margin-top: 10px;
  padding-bottom: 10px;
  justify-content: flex-end;
`;

const BaseModal = styled.div<{ visible?: boolean }>`
  position: absolute;
  top: 50%;
  transform: translateY(-50%);
  z-index: 100;

  box-shadow: 0 3px 6px rgba(0, 0, 0, 0.16), 0 3px 6px rgba(0, 0, 0, 0.23);
  border-radius: 4px;
  background-color: white;
  min-width: 300px;
  min-height: 100px;

  ${({ visible }) => (visible ? "display: flex;" : "display: none;")}
`;

const Modal = styled(BaseModal)`
  justify-content: center;
  align-items: center;
  padding: 15px 30px;
  padding-top: 30px;
`;

const ModalContent = styled.div`
  display: flex;
  flex-direction: column;
`;

const ModalActions = styled.div`
  display: flex;
  margin-top: 10px;
  justify-content: flex-end;
`;

const Dropzone = styled.div`
  color: grey;
  padding: 10px 15px;
  border: 2px dashed grey;
`;

const BaseInput = styled.input`
  border: 1px solid grey;
  padding: 10px 15px;
  border-radius: 3px;
`;

const Input = styled(BaseInput)`
  margin-bottom: 10px;
`;

const timestampToDateString = (timestamp: number): String =>
  dayjs.unix(timestamp).format("DD.MM.YYYY HH:mm:ss");

type Firmware = {
  [key: string]: any;

  name: string;
  updatedAt: number;
  createdAt: number;
  // in bytes
  fileSize: number;
  fileName: string;
};

const tableHeaders = [
  { name: "Name", sortable: false },
  { name: "File Name", sortable: true },
  { name: "File Size", sortable: true },
  { name: "Created At", sortable: true },
  { name: "Updated At", sortable: true },
  { name: "", sortable: false }
];

const App = () => {
  const [firmwares, setFirmwares] = useState<Firmware[]>([
    {
      name: "Test 1",
      createdAt: 1574678937,
      updatedAt: 1574678937,
      fileSize: 200,
      fileName: "test.txt"
    },
    {
      name: "Test 2",
      createdAt: 1574678965,
      updatedAt: 1574678965,
      fileSize: 200,
      fileName: "test.txt"
    },
    {
      name: "Test 3",
      createdAt: 1574678975,
      updatedAt: 1574678975,
      fileSize: 200,
      fileName: "test.txt"
    }
  ]);

  const { getRootProps, getInputProps, acceptedFiles } = useDropzone({
    multiple: false
  });

  const [sortedBy, setSortedBy] = useState<number | null>(null);
  const [isSortReversed, setIsSortReversed] = useState(false);

  const [isModalOpen, setIsModalOpen] = useState(true);

  const [firmwareName, setFirmwareName] = useInput("");

  const addFirmware = () => {
    setIsModalOpen(true);
  };

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

  const onCancel = () => {
    setIsModalOpen(false);
    setFirmwareName("");
    acceptedFiles.pop();
  };

  const onUpload = () => {
    const file = acceptedFiles[0];
    const now = Date.now() / 1000;
    setFirmwares([
      ...firmwares,
      {
        createdAt: now,
        updatedAt: now,
        fileName: file.name,
        fileSize: file.size,
        name: firmwareName
      }
    ]);
    onCancel();
  };

  return (
    <Container>
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
        <Button onClick={addFirmware}>Add Firmware</Button>
      </TableActions>
      <Table>
        <TableHead>
          <TableRow>
            {tableHeaders.map((header, i) => (
              <TableHeader
                key={i}
                sortable={header.sortable}
                onClick={header.sortable ? () => toggleSort(i) : () => {}}
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
              <TableData>{fw.fileSize} B</TableData>
              <TableData>{timestampToDateString(fw.createdAt)}</TableData>
              <TableData>{timestampToDateString(fw.updatedAt)}</TableData>
              <TableData actions>
                <IconButton>
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
