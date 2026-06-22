import { html, css, LitElement } from "lit";
import { customElement, property } from "lit/decorators.js";
import cssReset from "./css/reset";

const inputRangeID: string = "range";
const currentValueID: string = "rangeValue";
const pressTimeToShowPopup = 500;

@customElement("esp-range-slider")
export class EspRangeSlider extends LitElement {
  private inputRange: HTMLInputElement | null = null;
  private currentValue: HTMLInputElement | null = null;

  private longPressTimer: ReturnType<typeof setTimeout> | null = null;
  private isPopupInputVisible: boolean = false;

  @property({ type: String }) value = 0;
  @property({ type: String }) min = 0;
  @property({ type: String }) max = 0;
  @property({ type: String }) step = 0;
  @property({ type: String }) name = "";

  protected firstUpdated(
    _changedProperties: Map<string | number | symbol, unknown>
  ): void {
    this.inputRange = this.shadowRoot?.getElementById(
      inputRangeID
    ) as HTMLInputElement;

    this.currentValue = this.shadowRoot?.getElementById(
      currentValueID
    ) as HTMLInputElement;
    document.addEventListener('mousedown', (event) => {
      if(!document.querySelector('.popup-number-input')) {
        return;
      }
      const isClickedOutside = !document.querySelector('.popup-number-input')?.contains(event.target as Node);      
      if (isClickedOutside && this.isPopupInputVisible) {
        this.deletePopupInput();
      }
    });    
  }  
  
  protected updated(): void {
    this.updateCurrentValueOverlay();
  }

  onMouseDownCurrentValue(event: MouseEvent): void {
    this.longPressTimer = setTimeout(() => {
      this.showPopupInput(event.pageX, event.pageY);
    }, pressTimeToShowPopup); 
  }
  
  onMouseUpCurrentValue(event: MouseEvent): void {
    if (this.longPressTimer && !this.isPopupInputVisible) {
      clearTimeout(this.longPressTimer);
      this.longPressTimer = null;
    }
  }
  
  onTouchStartCurrentValue(event: TouchEvent): void {
    this.longPressTimer = setTimeout(() => {      
      this.showPopupInput(event.touches[0].pageX,event.touches[0].pageY);
    }, pressTimeToShowPopup); 
  }
  
  onTouchEndCurrentValue(event: TouchEvent): void {
    if (this.longPressTimer && !this.isPopupInputVisible) {
      clearTimeout(this.longPressTimer);
      this.longPressTimer = null;
    }
  }
  
  deletePopupInput(): void {
    const popupInputElement = document.querySelector('.popup-number-input');
    if (popupInputElement) {
      popupInputElement.remove();
    }
    this.isPopupInputVisible = false;
  }

  showPopupInput(x: number, y: number): void {
    const popupInputElement = document.createElement('input');
    popupInputElement.type = 'number'; 
    popupInputElement.value = this.inputRange.value;
    popupInputElement.min = this.inputRange.min;
    popupInputElement.max = this.inputRange.max;
    popupInputElement.step = this.inputRange.step;
    popupInputElement.classList.add('popup-number-input');

    const styles = `
    position: absolute;
    left: ${x}px;
    top: ${y}px;
    width: 50px;
    -webkit-appearance: none;
    margin: 0;
    `;  
    popupInputElement.setAttribute('style', styles);
    document.body.appendChild(popupInputElement);

    popupInputElement.addEventListener('contextmenu', (event) => {
      event.preventDefault();
    });

    popupInputElement.addEventListener('change', (ev: Event) =>{
      let input = ev.target as HTMLInputElement;
      this.inputRange.value = input?.value;

      var event = new Event('input');    
      this.inputRange?.dispatchEvent(event);
      var event = new Event('change');    
      this.inputRange?.dispatchEvent(event);
    });

    popupInputElement.addEventListener('keydown', (event) => {
      if (event.key === 'Enter') {
        this.deletePopupInput();
      }
    });

    popupInputElement.focus();
    this.isPopupInputVisible = true;
  }

  updateCurrentValueOverlay(): void {
    const min = parseFloat(this.inputRange.min);
    const max = parseFloat(this.inputRange.max);
    const val = parseFloat(this.inputRange.value);
    const newValueAsPercent = ((val - min) / (max - min)) * 100;
    const newPosition = 10 - (newValueAsPercent * 0.2);
    this.currentValue.innerHTML = `<span>${this.inputRange?.value}</span>`;
    this.currentValue.style.left = `calc(${newValueAsPercent}% + (${newPosition}px))`;

    const spanTooltip = this.currentValue?.querySelector('span');
    spanTooltip?.addEventListener('mousedown', this.onMouseDownCurrentValue.bind(this));
    spanTooltip?.addEventListener('mouseup', this.onMouseUpCurrentValue.bind(this));
    spanTooltip?.addEventListener('touchstart', this.onTouchStartCurrentValue.bind(this));
    spanTooltip?.addEventListener('touchend', this.onTouchEndCurrentValue.bind(this));

    spanTooltip?.addEventListener('contextmenu', (event) => {
      event.preventDefault();
    });
  }

  onInputEvent(ev: Event): void {
    this.updateCurrentValueOverlay();
  }

  onInputChangeEvent(ev: Event): void {
    this.sendState(this.inputRange?.value);
  }

  sendState(value: string|undefined): void {
    let event = new CustomEvent("state", {
      detail: {
        state: value,
        id: this.id,
      },
    });
    this.dispatchEvent(event); 
  }

  render() {
    return html`
      <div class="range-wrap">
        <div class="slider-wrap">
          <div class="range-value" id="rangeValue"></div>
          <input
            id="${inputRangeID}"
            type="range"
            name="${this.name}"
            step="${this.step}"
            min="${this.min || Math.min(0, this.value)}"
            max="${this.max || Math.max(10, this.value)}"
            .value="${this.value}"
            @input="${this.onInputEvent}"
            @change="${this.onInputChangeEvent}"
          />
          <div class="range-bounds">
            <span>${this.min || 0}</span>
            <span>${this.max || 100}</span>
          </div>
        </div>
      </div>
    `;
  }

  static get styles() {
    return [
      cssReset,
      css`
        :host {
          min-width: 100px;
          flex: 1;
          display: block;
        }

        .range-wrap {
          width: 100%;
        }

        .slider-wrap {
          position: relative;
          padding-top: 20px;
        }

        /* ── Track ── */
        input[type=range] {
          -webkit-appearance: none;
          appearance: none;
          width: 100%;
          height: 4px;
          border-radius: 999px;
          background: rgba(146, 105, 254, 0.35);
          cursor: pointer;
          outline: none;
          touch-action: none;
          margin: 0;
        }
        input[type=range]::-webkit-slider-runnable-track {
          height: 4px;
          border-radius: 999px;
          background: rgba(146, 105, 254, 0.35);
        }
        input[type=range]::-moz-range-track {
          height: 4px;
          border-radius: 999px;
          background: rgba(146, 105, 254, 0.35);
        }

        /* ── Thumb ── */
        input[type=range]::-webkit-slider-thumb {
          -webkit-appearance: none;
          width: 16px;
          height: 16px;
          border-radius: 50%;
          background: #9269fe;
          border: 2px solid rgba(255, 255, 255, 0.12);
          box-shadow: 0 0 0 3px rgba(146, 105, 254, 0.2);
          cursor: pointer;
          transition: box-shadow 0.15s ease;
          margin-top: -6px;
        }
        input[type=range]:hover::-webkit-slider-thumb {
          box-shadow: 0 0 0 6px rgba(146, 105, 254, 0.22);
        }
        input[type=range]::-moz-range-thumb {
          width: 16px;
          height: 16px;
          border-radius: 50%;
          background: #9269fe;
          border: 2px solid rgba(255, 255, 255, 0.12);
          box-shadow: 0 0 0 3px rgba(146, 105, 254, 0.2);
          cursor: pointer;
        }

        /* ── Tooltip ── */
        .range-value {
          position: absolute;
          top: 0;
          left: 0;
        }
        .range-value span {
          position: absolute;
          transform: translateX(-50%);
          display: inline-block;
          font-family: var(--vp-font-family-mono);
          font-size: 10px;
          font-weight: 600;
          padding: 2px 6px;
          border-radius: 4px;
          background: rgba(146, 105, 254, 0.15);
          border: 1px solid rgba(146, 105, 254, 0.3);
          color: #9269fe;
          white-space: nowrap;
          cursor: pointer;
          user-select: none;
        }

        /* ── Bounds ── */
        .range-bounds {
          display: flex;
          justify-content: space-between;
          padding: 0 2px;
          margin-top: 4px;
        }
        .range-bounds span {
          font-family: var(--vp-font-family-mono);
          font-size: 10px;
          color: rgba(127, 127, 127, 0.4);
          user-select: none;
        }
      `,
    ];
  }
}
