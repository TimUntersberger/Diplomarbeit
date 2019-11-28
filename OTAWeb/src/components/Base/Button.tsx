import styled from "styled-components";

const getBackgroundColor = (color?: string): string =>
  color ? "var(--primary-color-" + color + ")" : "var(--primary-color-two)";

export const Button = styled.button<{ color?: string; flat?: boolean }>`
  background: none;
  border: none;
  font-size: 0.9em;
  padding: 8px 15px;
  border-radius: 2px;
  user-select: none;
  color: ${({ flat, color }) =>
    flat ? getBackgroundColor(color || "two") : "white"};
  background-color: ${({ color, flat }) =>
    !flat ? getBackgroundColor(color) : "none"};
  cursor: pointer;

  &:disabled {
    cursor: inherit;
    background-color: darkgrey;
  }
`;

export const IconButton = styled.div`
  cursor: pointer;
`;
