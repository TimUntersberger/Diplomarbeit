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

export const TableHeader = styled.th<{
  sortable?: boolean;
  alignRight?: boolean;
}>`
  padding: 10px 20px;
  font-weight: normal;
  border-top: 1px solid lightgrey;
  border-bottom: 1px solid lightgrey;
  user-select: none;
  ${({ alignRight }) => (alignRight ? "text-align: right;" : "")}
  ${({ sortable }) => (sortable ? "cursor: pointer;" : "")}
`;

export const TableData = styled.td<{ actions?: boolean; isNumber?: boolean }>`
  padding: 15px 20px;
  font-weight: lighter;

  text-align: ${({ actions, isNumber }) =>
    actions || isNumber ? "right" : "left"};

  &:last-child {
  }
`;

export const TableHead = styled.thead`
  text-align: left;
`;

export const TableBody = styled.tbody``;

export const TableActions = styled.div`
  display: flex;
  width: 100%;
  margin-top: 10px;
  padding-bottom: 10px;
  justify-content: flex-end;
`;
