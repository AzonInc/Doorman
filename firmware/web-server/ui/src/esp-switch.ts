import { html, css, LitElement } from "lit";
import { customElement, property } from "lit/decorators.js";
import { stateOn, stateOff } from "./esp-entity-table";
import cssReset from "./css/reset";

@customElement("esp-switch")
export class EspSwitch extends LitElement {
  @property({ type: String }) stateOn = stateOn;
  @property({ type: String }) stateOff = stateOff;
  @property({ type: String }) state = stateOff;
  @property({ type: String }) color = "currentColor";
  @property({ type: Boolean }) disabled = false;

  private isOn(): boolean {
    return this.state === this.stateOn;
  }

  toggle(ev: Event): void {
    const newState = this.isOn() ? this.stateOff : this.stateOn;
    this.dispatchEvent(new CustomEvent("state", {
      detail: { state: newState, id: this.id },
    }));
  }

  private _handleKeyDown(e: KeyboardEvent) {
    if (e.key === " " || e.key === "Enter") {
      e.preventDefault();
      if (!this.disabled) this.toggle(e);
    }
  }

  render() {
    const on = this.isOn();
    return html`
      <div
        class="sw ${on ? "on" : ""} ${this.disabled ? "disabled" : ""}"
        role="switch"
        aria-checked="${on}"
        tabindex="${this.disabled ? -1 : 0}"
        style="--sw-color: ${this.color}"
        @click="${this.disabled ? null : this.toggle}"
        @keydown="${this._handleKeyDown}"
      >
        <div class="track">
          <div class="thumb"></div>
        </div>
      </div>
    `;
  }

  static get styles() {
    return [
      cssReset,
      css`
        :host {
          display: inline-flex;
          align-items: center;
        }

        .sw {
          display: inline-flex;
          align-items: center;
          cursor: pointer;
          -webkit-tap-highlight-color: transparent;
          user-select: none;
          outline: none;
        }

        .sw.disabled {
          cursor: not-allowed;
          opacity: 0.38;
          pointer-events: none;
        }

        .sw:focus-visible .track {
          outline: 2px solid var(--sw-color, #9269fe);
          outline-offset: 3px;
        }

        /* ── Track ── */
        .track {
          position: relative;
          width: 44px;
          height: 26px;
          border-radius: 13px;
          box-sizing: border-box;
          background: rgba(127, 127, 127, 0.1);
          border: 1px solid rgba(127, 127, 127, 0.2);
          transition: background 0.25s ease, border-color 0.25s ease;
        }

        .on .track {
          background: rgba(146, 105, 254, 0.2);
          border-color: rgba(146, 105, 254, 0.35);
        }

        .sw:not(.disabled):hover .track {
          border-color: rgba(127, 127, 127, 0.35);
          background: rgba(127, 127, 127, 0.15);
        }

        .sw.on:not(.disabled):hover .track {
          background: rgba(146, 105, 254, 0.28);
          border-color: rgba(146, 105, 254, 0.5);
        }

        /* ── Thumb ── */
        .thumb {
          position: absolute;
          top: 50%;
          left: 4px;
          width: 16px;
          height: 16px;
          border-radius: 50%;
          background: rgba(127, 127, 127, 0.45);
          transform: translateY(-50%);
          transition:
            left 0.25s cubic-bezier(0.2, 0, 0, 1),
            background 0.2s ease;
        }

        .on .thumb {
          left: 22px;
          background: #9269fe;
        }
      `,
    ];
  }
}
