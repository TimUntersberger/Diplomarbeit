import React from "react";
import styled from "styled-components";
import {
  Table as BaseTable,
  TableHead,
  TableRow,
  TableHeader,
  TableBody,
  TableData
} from "./Base/Table";
import { FaArrowUp } from "react-icons/fa";
import { Button } from "./Base/Button";

export const Container = styled.div`
  --primary-color-one: #2d8632;
  --primary-color-two: #9cdaa0;
  --primary-color-three: #76da7c;
  --primary-color-four: #062d08;
  --primary-color-five: #002d03;

  height: 100vh;
  width: 100vw;

  font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Oxygen,
    Ubuntu, Cantarell, "Open Sans", "Helvetica Neue", sans-serif;
`;

const Table = styled(BaseTable)`
  margin: 100px auto;
`;

const App = () => {
  return (
    <Container>
      <Table>
        <TableHead>
          <TableRow>
            <TableHeader>Name</TableHeader>
            <TableHeader>Created At</TableHeader>
            <TableHeader>Updated At</TableHeader>
            <TableHeader></TableHeader>
          </TableRow>
        </TableHead>
        <TableBody>
          <TableRow>
            <TableData>Test</TableData>
            <TableData>21.11.2019 14:36:39</TableData>
            <TableData>21.11.2019 15:35:40</TableData>
            <TableData>
              <FaArrowUp></FaArrowUp>
            </TableData>
          </TableRow>
          <TableRow>
            <TableData>Test</TableData>
            <TableData>21.11.2019 14:36:39</TableData>
            <TableData>21.11.2019 15:35:40</TableData>
          </TableRow>
          <TableRow>
            <TableData>Test</TableData>
            <TableData>21.11.2019 14:36:39</TableData>
            <TableData>21.11.2019 15:35:40</TableData>
          </TableRow>
        </TableBody>
      </Table>
    </Container>
  );
};

export default App;
