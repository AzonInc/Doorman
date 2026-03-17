import { css } from "lit";

export default css`
  :host {
    position: relative;
  }
  select {
    width: 100%;
    border-radius: 4px;
  }
  input[type="range"],
  input[type="text"],
  input[type="number"],
  input[type="password"] {
    width: calc(100% - 3rem);
    height: 0.75rem;
  }
  input, select
  {
    border-radius: 5px;
    padding: 4px;
    border: solid 1px #333333;
    background-color: transparent;
    box-sizing: border-box;
  }
  .range {
    width: 100%;
    display: flex;
    justify-content: space-between;
    gap: 15px;
    align-items: center;
    flex-wrap: wrap;
  }
  .range label {
    flex: 0 0 auto;
  }
  .range input{
    min-width: 50px;
    flex: 1;
  }
  @media (max-width: 500px) {
    .range {
      flex-direction: column;
      align-items: stretch;
      padding: 5px;
      gap: 5px;
    }

    .range label {
      text-align: center;
      width: 100%;
    }

    .range input {
      width: 100%;
    }
  }
  .entity-row {
    display: flex;
    align-items: center;
    flex-direction: row;
    transition: all 0.3s ease-out 0s;
    min-height: 40px;
    position: relative;
  }
  .entity-row.expanded {
    min-height: 240px;
  }
  .entity-row:nth-child(2n) {
    background-color: rgba(90, 90, 90, 0.1);
  }
  .entity-row iconify-icon {
    vertical-align: middle;
  }
  .entity-row > :nth-child(1) {
    flex: 0 0 40px;
    color: #9269fe;
    line-height: 40px;
    text-align: center;
  }
  .entity-row > :nth-child(2) {
    flex: 1 1 40%;
    margin-left: 8px;
    margin-right: 8px;
    /*text-wrap: nowrap;
    overflow: hidden;
    text-overflow: ellipsis;*/
    min-width: 100px;
    padding: 5px 0px;
  }
  .entity-row > :nth-child(3) {
    flex: 1 1 50%;
    margin-right: 8px;
    margin-left: 20px;
    text-align: right;
    display: flex;
    justify-content: space-between;
  }
  .entity-row > :nth-child(3) > :only-child {
    margin-left: auto;
  }
  .binary_sensor_off {
    color: rgba(127, 127, 127, 0.7);
  }
  .singlebutton-row button {
    margin: auto;
    display: flex;
  }
  .climate-wrap{
    width: 100%;
    margin: 10px 0 10px 0;
  }
  .climate-row {
    width: 100%;
    display: inline-flex;
    flex-wrap: wrap;
    text-align: left;
  }
  .climate-row > select{
    width: 50%;
  }
  .climate-row > label{
    align-content: center;
    width: 100px;
  }

  .description-row {
    background-color: rgb(146 105 254 / 20%);
    border-radius: 0px 10px 0 0;
    display: flex;
    align-items: center;
    flex-direction: row;
    transition: all 0.3s ease-out 0s;
    min-height: 40px;
    position: relative;
    box-shadow: 0px 2px 3px -2px #000000ab;
  }
  .description-row > :nth-child(1) {
    flex: 0 0 40px;
    color: #9269fe;
    line-height: 40px;
    text-align: center;
  }
  .description-row > :nth-child(2) {
    flex: 1 1 40%;
    margin-left: 8px;
    margin-right: 8px;
    min-width: 100px;
    padding: 15px 0px;
  }
  .description-row iconify-icon {
    vertical-align: middle;
  }
    
  input[type="color"]::-webkit-color-swatch-wrapper {
    padding: 0 !important;
  }
`;
