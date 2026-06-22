import { css } from "lit";

export default css`
  :host {
    display: flex;
    flex-direction: column;
    min-height: 100dvh;
    margin: 0;
    padding: 0;
  }
  header {
    flex-shrink: 0;
  }
  esp-entity-table {
    flex: 1;
    min-height: 0;
  }

  .bg-orbs {
    position: fixed;
    inset: 0;
    pointer-events: none;
    z-index: 0;
    overflow: hidden;
  }
  .bg-orbs::before {
    content: '';
    position: absolute;
    top: -130px;
    left: -90px;
    width: 520px;
    height: 520px;
    border-radius: 50%;
    background: rgba(146, 105, 254, 0.14);
    filter: blur(80px);
  }
  .bg-orbs::after {
    content: '';
    position: absolute;
    top: 90px;
    left: 200px;
    width: 300px;
    height: 300px;
    border-radius: 50%;
    background: rgba(80, 30, 200, 0.09);
    filter: blur(100px);
  }
  @media (min-width: 641px) {
    .bg-orbs::before {
      top: -180px;
      left: -120px;
      width: 800px;
      height: 800px;
      filter: blur(120px);
    }
    .bg-orbs::after {
      top: 120px;
      left: 280px;
      width: 500px;
      height: 500px;
      filter: blur(140px);
    }
  }


  * {
    box-sizing: border-box;
  }
  h1 {
    font-size: 16px;
    font-weight: 600;
    line-height: 1.2em;
    margin-block: 0;
  }
  header {
    padding: 0 24px;
    display: flex;
    justify-content: space-between;
    align-items: center;
    gap: 15px;
    height: 64px;
    position: sticky;
    top: 0;
    z-index: 50;
    background-color: color-mix(in srgb, var(--c-bg, #1b1b1f) 50%, transparent);
    backdrop-filter: blur(12px);
    -webkit-backdrop-filter: blur(12px);
    border-bottom: 1px solid rgba(127, 127, 127, 0.15);
  }
  infobox {
    background-color: rgba(146, 105, 254, 0.08);
    padding: 10px max(24px, calc((100% - 960px) / 2 + 24px));
    display: flex;
    align-items: center;
    gap: 14px;
    font-size: 13px;
  }
  infobox.danger {
    background-color: rgba(244, 63, 94, 0.08) !important;
    border-top-color: rgba(244, 63, 94, 0.2) !important;
  }
  infobox.warning {
    background-color: rgba(234, 179, 8, 0.08) !important;
    border-top-color: rgba(234, 179, 8, 0.2) !important;
  }

  .infobox-dismiss {
    margin-left: auto;
    flex-shrink: 0;
    display: flex;
    align-items: center;
    justify-content: center;
    width: 28px;
    height: 28px;
    padding: 0;
    margin-right: 0;
    border: none;
    border-radius: 6px;
    background: none;
    color: currentColor;
    opacity: 0.4;
    cursor: pointer;
    text-transform: none;
    letter-spacing: normal;
  }
  .infobox-dismiss:hover {
    opacity: 0.75;
    background: rgba(127, 127, 127, 0.12);
  }

  infobox iconify-icon {
    color: #9269fe;
    flex-shrink: 0;
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
    font-size: 2rem;
    color: rgba(127, 127, 127, 0.5);
    transition: color 0.2s;
  }
  header #logo {
    color: rgba(127, 127, 127, 0.6);
    display: flex;
    align-items: center;
    transition: color 0.2s;
  }
  header #logo:hover {
    color: #9269fe;
  }
  header .title {
    flex-grow: 1;
    min-width: 0;
  }
  header .title h1 {
    white-space: nowrap;
    overflow: hidden;
    text-overflow: ellipsis;
  }
  header .title div {
    font-size: 11px;
    opacity: 0.45;
    margin-top: 3px;
    white-space: nowrap;
    overflow: hidden;
    text-overflow: ellipsis;
  }
  header .status {
    display: flex;
    align-items: center;
  }
  header .status iconify-icon {
    font-size: 1.4rem;
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
    flex: 0 0 48px;
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
