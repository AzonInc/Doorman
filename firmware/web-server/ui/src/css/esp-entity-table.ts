import { css } from "lit";

export default css`
  :host {
    display: block;
    position: relative;
  }

  /* ── Two-column layout ── */
  .layout {
    display: flex;
    flex-direction: row;
    box-sizing: border-box;
    max-width: 960px;
    margin: 0 auto;
    min-height: calc(100vh - var(--header-height, 64px));
  }

  /* ── Sidebar (desktop) ── */
  .nav-group {
    width: 200px;
    flex-shrink: 0;
    display: flex;
    flex-direction: column;
    gap: 2px;
    padding: 12px 12px 72px;
    position: sticky;
    top: var(--header-height, 64px);
    align-self: flex-start;
    max-height: calc(100vh - var(--header-height, 64px));
    overflow-y: auto;
    scrollbar-width: none;
    border-right: 1px solid rgba(127, 127, 127, 0.1);
  }
  .nav-group::-webkit-scrollbar { display: none; }

  /* ── Sidebar search ── */
  .nav-search-wrap {
    position: relative;
    display: flex;
    align-items: center;
    padding: 0 0 2px;
    flex-shrink: 0;
  }
  .nav-search-icon {
    position: absolute;
    left: 9px;
    color: rgba(127, 127, 127, 0.35);
    pointer-events: none;
  }
  .nav-search-input {
    width: 100%;
    padding: 6px 28px 6px 28px;
    background: rgba(127, 127, 127, 0.08);
    border: 1px solid rgba(127, 127, 127, 0.15);
    border-radius: 8px;
    color: inherit;
    font-family: inherit;
    font-size: 12px;
    outline: none;
    box-sizing: border-box;
    appearance: none;
    -webkit-appearance: none;
  }
  .nav-search-input:focus {
    border-color: rgba(146, 105, 254, 0.4);
    background: rgba(146, 105, 254, 0.06);
  }
  .nav-search-input::placeholder {
    color: rgba(127, 127, 127, 0.3);
  }
  .nav-search-input::-webkit-search-cancel-button { display: none; }
  .nav-search-clear {
    position: absolute;
    right: 5px;
    display: flex;
    align-items: center;
    justify-content: center;
    width: 20px;
    height: 20px;
    padding: 0;
    margin: 0;
    border: none;
    background: none;
    color: rgba(127, 127, 127, 0.4);
    cursor: pointer;
    border-radius: 4px;
    text-transform: none;
    letter-spacing: normal;
  }
  .nav-search-clear:hover {
    color: currentColor;
    background: rgba(127, 127, 127, 0.15);
  }

  .nav-search-divider {
    height: 1px;
    background: rgba(127, 127, 127, 0.1);
    margin: 4px 4px 6px;
    flex-shrink: 0;
  }

  .search-empty {
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    gap: 10px;
    padding: 48px 16px;
    color: rgba(127, 127, 127, 0.4);
    font-size: 13px;
    text-align: center;
  }
  .search-empty iconify-icon {
    font-size: 32px;
    opacity: 0.5;
  }

  .nav-item {
    display: block;
    width: 100%;
    text-align: left;
    padding: 8px 12px;
    margin: 0;
    border-radius: 8px;
    border: none;
    background: none;
    font-size: 13px;
    font-family: inherit;
    font-weight: 400;
    color: currentColor;
    text-transform: none;
    letter-spacing: normal;
    opacity: 0.5;
    cursor: pointer;
    flex-shrink: 0;
    white-space: nowrap;
    overflow: hidden;
    text-overflow: ellipsis;
    transition: none;
  }
  .nav-item.active {
    opacity: 1;
    color: #9269fe;
    background: rgba(146, 105, 254, 0.12);
    font-weight: 500;
  }
  .nav-item:hover:not(.active) {
    opacity: 0.75;
    background: rgba(127, 127, 127, 0.08);
  }

  /* ── Content area ── */
  .content-area {
    flex: 1;
    min-width: 0;
    padding: 12px 24px 72px;
  }

  /* ── Mobile: horizontal tab bar ── */
  @media (max-width: 640px) {
    .nav-search-wrap { display: none; }
    .layout {
      flex-direction: column;
      max-width: none;
    }
    .nav-group {
      box-sizing: border-box;
      width: 100%;
      min-width: 0;
      height: auto;
      flex-direction: row;
      flex-wrap: nowrap;
      overflow-x: auto;
      overflow-y: hidden;
      padding: 8px 8px 8px 0;
      gap: 4px;
      border-right: none;
      border-bottom: 1px solid rgba(127, 127, 127, 0.1);
      position: sticky;
      top: var(--header-height, 64px);
      z-index: 10;
      transition: background-color 0.2s ease;
    }
    .nav-group.is-stuck {
      background-color: color-mix(in srgb, var(--c-bg, #1b1b1f) 88%, transparent);
      backdrop-filter: blur(12px);
      -webkit-backdrop-filter: blur(12px);
    }
    .nav-item {
      flex: 0 0 auto;
      width: auto;
      padding: 7px 14px;
      overflow: visible;
      text-overflow: clip;
    }
    .nav-item.active {
      color: #9269fe;
      background: rgba(146, 105, 254, 0.12);
    }
    .content-area {
      padding: 12px 8px 72px;
    }
  }

  select {
    width: 100%;
    appearance: none;
    -webkit-appearance: none;
    padding: 7px 30px 7px 10px;
    border-radius: 8px;
    border: 1px solid rgba(127, 127, 127, 0.2);
    background-color: rgba(127, 127, 127, 0.08);
    background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='16' height='16' viewBox='0 0 24 24'%3E%3Cpath fill='rgba(160,160,160,0.7)' d='M7 10l5 5 5-5z'/%3E%3C/svg%3E");
    background-repeat: no-repeat;
    background-position: right 8px center;
    background-size: 16px;
    color: inherit;
    font-family: inherit;
    font-size: 13px;
    cursor: pointer;
    outline: none;
    box-sizing: border-box;
    transition: border-color 0.15s ease, background-color 0.15s ease;
  }
  select:hover {
    border-color: rgba(127, 127, 127, 0.35);
    background-color: rgba(127, 127, 127, 0.12);
  }
  select:focus {
    border-color: rgba(146, 105, 254, 0.4);
    background-color: rgba(146, 105, 254, 0.06);
  }
  option {
    background-color: #1b1b1f;
  }
  input[type="range"] {
    width: 100%;
    height: 0.75rem;
  }
  .range {
    width: 100%;
    display: flex;
    flex-direction: column;
    gap: 3px;
  }
  .range input[type="number"] {
    width: 100%;
  }
  .range-bounds {
    display: flex;
    justify-content: space-between;
    padding: 0 2px;
  }
  .range-bounds span {
    font-family: monospace;
    font-size: 10px;
    color: rgba(127, 127, 127, 0.4);
    user-select: none;
  }
  .entity-row {
    display: flex;
    align-items: center;
    min-height: 56px;
    padding: 3px 16px 3px 0;
    position: relative;
    border-bottom: 1px solid rgba(127, 127, 127, 0.1);
  }
  .entity-row:last-child,
  .entity-row:not(:has(~ .entity-row)) {
    border-bottom: none;
  }
  .entity-row.expanded {
    min-height: 240px;
    align-items: flex-start;
  }
  .entity-row iconify-icon {
    vertical-align: middle;
  }
  .entity-row > :nth-child(1) {
    flex: 0 0 48px;
    color: #9269fe;
    display: flex;
    align-items: center;
    justify-content: center;
    align-self: stretch;
  }
  .entity-row > :nth-child(2) {
    flex: 1 1 0;
    min-width: 80px;
    padding: 4px 12px 4px 0;
    font-size: 14px;
  }
  .entity-row > :nth-child(3) {
    flex: 0 0 44%;
    min-width: 80px;
    margin-right: 0;
    display: flex;
    align-items: center;
    justify-content: flex-end;
    gap: 4px;
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

  /* ── Press button ── */
  .press-btn {
    display: inline-flex;
    align-items: center;
    gap: 6px;
    padding: 7px 14px;
    border-radius: 8px;
    border: 1px solid rgba(127, 127, 127, 0.2);
    background: rgba(127, 127, 127, 0.08);
    color: currentColor;
    font-family: inherit;
    font-size: 13px;
    font-weight: 500;
    text-transform: none;
    letter-spacing: normal;
    margin: 0;
    cursor: pointer;
    opacity: 0.8;
    transition: background 0.15s ease, opacity 0.15s ease,
                border-color 0.15s ease, transform 0.1s ease;
  }
  .press-btn:hover {
    opacity: 1;
    background: rgba(127, 127, 127, 0.14);
    border-color: rgba(127, 127, 127, 0.35);
  }
  .press-btn:active {
    transform: scale(0.95);
    background: rgba(146, 105, 254, 0.12);
    border-color: rgba(146, 105, 254, 0.25);
  }

  /* ── Lock control ── */
  .lock-control {
    display: flex;
    gap: 4px;
    align-items: center;
  }
  .lock-btn {
    display: flex;
    align-items: center;
    justify-content: center;
    width: 34px;
    height: 34px;
    padding: 0;
    margin: 0;
    border-radius: 8px;
    border: 1px solid rgba(127, 127, 127, 0.15);
    background: rgba(127, 127, 127, 0.08);
    color: currentColor;
    opacity: 0.55;
    cursor: pointer;
    text-transform: none;
    letter-spacing: normal;
    transition: background 0.15s ease, opacity 0.15s ease, border-color 0.15s ease;
  }
  .lock-btn:hover:not([disabled]) {
    opacity: 0.85;
    background: rgba(127, 127, 127, 0.14);
  }
  .lock-btn--on {
    opacity: 1;
    color: #9269fe;
    background: rgba(146, 105, 254, 0.12);
    border-color: rgba(146, 105, 254, 0.25);
    cursor: default;
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
    display: flex;
    align-items: flex-start;
    gap: 10px;
    padding: 10px 12px;
    margin-bottom: 4px;
    border-left: 2px solid rgba(146, 105, 254, 0.5);
    border-radius: 0 6px 6px 0;
    background: rgba(146, 105, 254, 0.05);
    font-size: 13px;
    line-height: 1.6;
    color: rgba(255, 255, 255, 0.55);
  }
  .description-row > :nth-child(1) {
    flex-shrink: 0;
    color: rgba(146, 105, 254, 0.7);
    margin-top: 1px;
  }
  .description-row > :nth-child(2) {
    flex: 1;
    min-width: 0;
  }
  .description-row a {
    color: #9269fe;
    opacity: 0.9;
  }
  .description-row iconify-icon {
    vertical-align: middle;
  }
    
  input[type="color"]::-webkit-color-swatch-wrapper {
    padding: 0 !important;
  }

  .tab-container {
    border: none;
    border-radius: 0;
    margin-bottom: 1.5em;
  }

  .ota-section {
    margin-top: 1.5em;
    padding-top: 1em;
    border-top: 1px solid rgba(127, 127, 127, 0.1);
  }

  .ota-form {
    display: flex;
    flex-direction: column;
    gap: 10px;
    margin-top: 12px;
    padding: 0;
  }

  .ota-upload-label {
    display: flex;
    align-items: center;
    gap: 10px;
    padding: 13px 16px;
    border-radius: 8px;
    border: 1px dashed rgba(127, 127, 127, 0.25);
    background: rgba(127, 127, 127, 0.05);
    cursor: pointer;
    font-size: 13px;
    color: rgba(127, 127, 127, 0.6);
    transition: border-color 0.15s ease, background 0.15s ease, color 0.15s ease;
  }
  .ota-upload-label:hover {
    border-color: rgba(146, 105, 254, 0.4);
    background: rgba(146, 105, 254, 0.05);
    color: currentColor;
  }
  .ota-upload-label iconify-icon {
    flex-shrink: 0;
    color: #9269fe;
  }
  .ota-file-input {
    display: none;
  }

  .ota-submit-btn {
    align-self: flex-end;
    display: inline-flex;
    align-items: center;
    gap: 7px;
    padding: 8px 18px;
    border-radius: 8px;
    border: 1px solid rgba(146, 105, 254, 0.3);
    background: rgba(146, 105, 254, 0.1);
    color: #9269fe;
    font-family: inherit;
    font-size: 13px;
    font-weight: 500;
    text-transform: none;
    letter-spacing: normal;
    margin: 0;
    cursor: pointer;
    transition: background 0.15s ease, border-color 0.15s ease;
  }
  .ota-submit-btn:hover:not([disabled]) {
    background: rgba(146, 105, 254, 0.18);
    border-color: rgba(146, 105, 254, 0.5);
  }
  .ota-submit-btn[disabled] {
    opacity: 0.35;
    cursor: not-allowed;
  }

  form {
    display: flex;
    padding: 6px 16px 6px 0;
    justify-content: space-between;
  }
`;
