import styled from "styled-components";

export const Table = styled.table`
  border-collapse: collapse;
`;

export const TableRow = styled.tr`
  border-bottom: 1px solid lightgrey;

  &:last-child {
    border-bottom: none;
  }
`;

export const TableHeader = styled.th`
  padding: 10px 20px;
  font-weight: normal;
  border-bottom: 1px solid lightgrey;
`;

export const TableData = styled.td`
  padding: 15px 20px;
  font-weight: lighter;

  &:last-child {
  }
`;

export const TableHead = styled.thead``;

export const TableBody = styled.tbody``;
