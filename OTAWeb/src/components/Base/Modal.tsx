import styled from "styled-components";

export const Modal = styled.div<{ visible?: boolean }>`
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

export const ModalContent = styled.div`
  display: flex;
  flex-direction: column;
`;

export const ModalActions = styled.div`
  display: flex;
  margin-top: 10px;
  justify-content: flex-end;
`;
