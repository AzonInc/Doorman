import { css } from "lit";

export default css`
  .flex-grid-half {
    display: grid;
    grid-template-columns: 700px 2fr;
  }
  .flex-grid-half.expanded_entity,
  .flex-grid-half.expanded_logs {
    grid-template-columns: 1fr;
  }
  .flex-grid-half .col {
    margin: 8px;
  }
  .flex-grid-half .col:nth-child(2) {
    overflow: hidden;
  }
  .flex-grid-half.expanded_logs .col:nth-child(1) {
    display: none;
  }
  .flex-grid-half.expanded_entity .col:nth-child(2) {
    display: none;
  }

  @media (max-width: 1024px) {
    .flex-grid,
    .flex-grid-half {
      display: block;
    }
    .flex-grid-half .col {
      /*width: 100% !important;*/
      /*margin: 0 0 10px 0 !important;*/
      display: block !important;
    }
  }

  * {
    box-sizing: border-box;
  }
  .flex-grid {
    margin: 0 0 20px 0;
  }
  h1 {
    font-size: 18px;
    line-height: 1.1em;
    margin-block: 0;
  }
  header {
    /*background-color: rgba(127, 127, 127, 0.05);
    border: 2px solid rgba(127, 127, 127, 0.3);
    border-radius: 12px;*/
    padding: 10px;
    display: flex;
    justify-content: space-between;
    align-items: center;
    gap: 15px;
    margin: 8px;
  }
  infobox {
    background-color: rgba(127, 127, 127, 0.05);
    padding: 10px;
    border: 2px solid rgba(127, 127, 127, 0.3);
    border-radius: 12px;
    margin: 8px;
    display: flex;
    align-items: center;
    gap: 15px;
  }
  infobox.danger {
    background-color: rgba(244, 63, 94, .16) !important;
  }
  infobox.warning {
    background-color: rgba(234, 179, 8, .16) !important;
  }

  infobox iconify-icon {
    color: #9269fe;
  }
  infobox.danger iconify-icon {
    color: #f14158 !important;
  }
  infobox.warning iconify-icon {
    color: #da8b17 !important;
  }


  a {
    color: #9269fe;
  }
  a:visted {
    color: #9269fe;
  }
  header iconify-icon {
    font-size: 2.5rem;
    color: rgba(127, 127, 127, 0.5);
  }
  header #logo {
    color: rgba(127, 127, 127, 0.5);
  }
  header .title {
    flex-grow: 1;
  }
  header .status {
    display: flex;
  }
  .connected {
    color: #9269fe;
  }
  esp-logo {
    float: left;
    line-height: 1em;
    font-size: initial;
  }
  form {
    display: flex;
    justify-content: space-between;
    background-color: rgba(127, 127, 127, 0.05);
  }
  form .btn {
    margin-right: 6px;
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
`;
