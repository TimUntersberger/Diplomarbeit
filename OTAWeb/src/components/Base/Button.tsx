import styled from "styled-components";

export const Button = styled.button<{ color?: string }>`
  background: none;
  border: none;

  font-size: 0.9em;
  padding: 8px 20px;
  border-radius: 2px;
  color: black;
  cursor: pointer;

  background-color: ${({ color }) =>
    color ? "var(--primary-color-" + color + ")" : "var(--primary-color-two)"};
`;
