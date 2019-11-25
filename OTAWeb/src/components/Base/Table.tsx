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

export const TableHeader = styled.th<{ sortable?: boolean }>`
  padding: 10px 20px;
  font-weight: normal;
  border-top: 1px solid lightgrey;
  border-bottom: 1px solid lightgrey;
  user-select: none;
  ${({ sortable }) => (sortable ? "cursor: pointer;" : "")}
`;

export const TableData = styled.td<{ actions?: boolean }>`
  padding: 15px 20px;
  font-weight: lighter;

  text-align: ${({ actions }) => (actions ? "right" : "left")};

  &:last-child {
  }
`;

export const TableHead = styled.thead`
  text-align: left;
`;

export const TableBody = styled.tbody``;
