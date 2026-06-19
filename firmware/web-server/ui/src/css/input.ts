import { css } from "lit";

export default css`
  input[type="text"],
  input[type="password"],
  input[type="number"] {
    width: 100%;
    height: auto;
    padding: 7px 10px;
    border-radius: 8px;
    border: 1px solid rgba(127, 127, 127, 0.2);
    background-color: rgba(127, 127, 127, 0.08);
    color: inherit;
    font-family: inherit;
    font-size: 13px;
    box-sizing: border-box;
    outline: none;
  }
  input[type="text"]:hover,
  input[type="password"]:hover,
  input[type="number"]:hover {
    border-color: rgba(127, 127, 127, 0.35);
  }
  input[type="text"]:focus,
  input[type="password"]:focus,
  input[type="number"]:focus {
    border-color: rgba(146, 105, 254, 0.4);
    background-color: rgba(146, 105, 254, 0.06);
  }
`;
