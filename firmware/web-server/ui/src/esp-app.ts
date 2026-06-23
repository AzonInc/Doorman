import { LitElement, html, css, PropertyValues, nothing } from "lit";
import { customElement, state, query } from "lit/decorators.js";
import { getBasePath, buildEntityActionUrl } from "./esp-entity-table";
import type { entityConfig } from "./esp-entity-table";

import "./esp-entity-table";
import "./esp-log";
import "./esp-switch";
import "./esp-range-slider";
import "./esp-logo";
import cssReset from "./css/reset";
import cssButton from "./css/button";
import cssApp from "./css/app";
import cssTab from "./css/tab";

window.source = new EventSource(getBasePath() + "/events");

interface Config {
  ota: boolean;
  log: boolean;
  title: string;
  comment: string;
}

function getRelativeTime(diff: number) {
  const mark = Math.sign(diff);

  if (diff === 0) return new Intl.RelativeTimeFormat("en").format(0, "second");

  const times = [
    { type: "year", seconds: 12 * 30 * 24 * 60 * 60 * 1000 },
    { type: "month", seconds: 30 * 24 * 60 * 60 * 1000 },
    { type: "week", seconds: 7 * 24 * 60 * 60 * 1000 },
    { type: "day", seconds: 24 * 60 * 60 * 1000 },
    { type: "hour", seconds: 60 * 60 * 1000 },
    { type: "minute", seconds: 60 * 1000 },
    { type: "second", seconds: 1000 },
  ];

  let result = "";
  const timeformat = new Intl.RelativeTimeFormat("en");
  let count = 0;
  for (let t of times) {
    const segment = Math.trunc(Math.abs(diff / t.seconds));
    if (segment > 0) {
      const part = timeformat.format(
        segment * mark,
        t.type as Intl.RelativeTimeFormatUnit
      );
      diff -= segment * t.seconds * mark;
      // remove "ago" from the first segment - if not the only one
      result +=
        count === 0 && t.type != "second" ? part.replace(" ago", " ") : part;
      if (count++ >= 1) break; // do not display detail after two segments
    }
  }
  return result;
}

@customElement("esp-app")
export default class EspApp extends LitElement {
  @state() scheme: string = "";
  @state() ping: number = 0;
  @state() connected: boolean = true;
  @state() lastUpdate: number = 0;
  @state() infoDismissed: boolean = false;
  @state() showAll: boolean = localStorage.getItem('esp-show-all') === 'true';
  @state() pendingUpdates: entityConfig[] = [];
  private _hasJsonUptime: boolean = false;
  @query("#beat") beat!: HTMLSpanElement;
  @query("esp-log") logPanel!: any;

  version: String = import.meta.env.PACKAGE_VERSION;
  config: Config = { ota: false, log: true, title: "", comment: "" };

  firmwareVersion: string = "";
  esphomeVersion: string = "";
  hardwareVersion: string = "";

  darkQuery: MediaQueryList = window.matchMedia("(prefers-color-scheme: dark)");

  frames = [{}, { color: "#9269feb5" }, {}];

  constructor() {
    super();
    const conf = document.querySelector("script#config");
    if (conf) this.setConfig(JSON.parse(conf.innerText));
  }

  setConfig(config: any) {
    if (!("log" in config)) {
      config.log = this.config.log;
    }
    this.config = config;

    document.title = config.title;
    document.documentElement.lang = config.lang;
  }

  async getFirmwareVersion() {
    const firmwareRequest = await fetch(`${getBasePath()}/text_sensor/Doorman%20Version`);
    const firmwareValue = await firmwareRequest.json();

    const esphomeRequest = await fetch(`${getBasePath()}/text_sensor/ESPHome%20Version`);
    const esphomeValue = await esphomeRequest.json();

    this.firmwareVersion = firmwareValue.value;
    this.esphomeVersion = 'ESPHome ' + esphomeValue.value;
  }

  async getHardwareVersion() {
    const response_model = await fetch(`${getBasePath()}/text_sensor/Model`);
    const data_model = await response_model.json();

    const response_revision = await fetch(`${getBasePath()}/text_sensor/Revision`);
    const data_revision = await response_revision.json();

    if(data_model.value.toLowerCase() == 'unsupported')
    {
      this.hardwareVersion = 'Unsupported Hardware';
    } else {
      this.hardwareVersion = data_model.value + ' (' + data_revision.value + ')';
    }
  }

  firstUpdated(changedProperties: PropertyValues) {
    super.firstUpdated(changedProperties);
    document.getElementsByTagName("head")[0].innerHTML +=
      '<meta name=viewport content="width=device-width, initial-scale=1,user-scalable=no">';
    const l = <HTMLLinkElement>document.querySelector("link[rel~='icon']"); // Set favicon to house
    l.href =
      'data:image/svg+xml,<svg width="32" height="32" xmlns="http://www.w3.org/2000/svg"><style>path{stroke-width:1;fill:black;stroke:black;stroke-linecap:round;stroke-linejoin:round}@media (prefers-color-scheme:dark){path{fill:white;stroke:white}}</style><path d="M1.3 18H5v10h21.8V18h3.7l-3.7-3.7V7.8h-2.4V12l-8.7-8.7L1.3 18Z"/></svg>';
    this.scheme = this.schemeDefault();
    window.source.addEventListener("ping", (e: MessageEvent) => {
      if (e.data?.length) {
        const data = JSON.parse(e.data);
        if (data.title !== undefined) {
          // Full config: {"title":"...","comment":"...","ota":true,"log":true,"lang":"en","uptime":123456}
          this.setConfig(data);
          this.requestUpdate();
        }
        if (data.uptime !== undefined) {
          // New firmware sends uptime in seconds in JSON data (overflow-safe)
          // Full config (on connect): {"title":"...","uptime":123456}
          // Interval ping: {"uptime":123456}
          this._hasJsonUptime = true;
          this._setUptime(data.uptime * 1000);
        } else {
          // Old firmware sends uptime in lastEventId (32-bit, may overflow after ~49 days)
          this._updateUptime(e);
        }
      } else {
        // Old firmware interval ping: empty data, uptime in lastEventId
        this._updateUptime(e);
      }
      this._updateUptime(e);
      this.lastUpdate = Date.now();
    });
    window.source.addEventListener("log", (e: MessageEvent) => {
      // Old firmware sends uptime in lastEventId for log events
      // Skip when new firmware provides uptime via JSON ping to avoid
      // millis() overwriting the overflow-safe seconds-based value
      if (!this._hasJsonUptime) {
        this._updateUptime(e);
      }
      this.lastUpdate = Date.now();
    });
    window.source.addEventListener("state", (e: MessageEvent) => {
      this.lastUpdate = Date.now();
    });
    window.source.addEventListener("error", (e: Event) => {
      console.dir(e);
      //console.log("Lost event stream!")
      this.connected = false;
      this.requestUpdate();
    });
    setInterval(() => {
      this.connected = !!this.ping && Date.now() - this.lastUpdate < 15000;
    }, 5000);
    this.infoDismissed = localStorage.getItem('doorman_infobox_dismissed') === '1';

    this.getFirmwareVersion();
    this.getHardwareVersion();

    const header = this.shadowRoot?.querySelector('header');
    const updatePageOffset = () => {
      const headerH = header?.offsetHeight ?? 0;
      const infobox = this.shadowRoot?.querySelector('infobox') as HTMLElement | null;
      const infoboxH = infobox?.offsetHeight ?? 0;
      document.documentElement.style.setProperty('--header-height', `${headerH}px`);
      document.documentElement.style.setProperty('--page-offset', `${headerH + infoboxH}px`);
    };
    if (header) {
      new ResizeObserver(updatePageOffset).observe(header);
    }
    new ResizeObserver(updatePageOffset).observe(this);
    updatePageOffset();

  }

  schemeDefault() {
    return "dark";
  }

  updated(changedProperties: Map<string, unknown>) {
    super.updated(changedProperties);
    if (changedProperties.has("scheme")) {
      let el = document.documentElement;
      document.documentElement.style.setProperty("color-scheme", this.scheme);
    }
    if (changedProperties.has("ping")) {
      if (!!this.ping) this.beat.animate(this.frames, 1000);
    }
  }

  uptime() {
    return getRelativeTime(-this.ping || 0)
      .replace(' years', 'y').replace(' year', 'y')
      .replace(' months', 'mo').replace(' month', 'mo')
      .replace(' weeks', 'w').replace(' week', 'w')
      .replace(' days', 'd').replace(' day', 'd')
      .replace(' hours', 'h').replace(' hour', 'h')
      .replace(' minutes', 'min').replace(' minute', 'min')
      .replace(' seconds', 's').replace(' second', 's')
      .replace(' ago', '');
  }


  private _dismissInfo() {
    this.infoDismissed = true;
    localStorage.setItem('doorman_infobox_dismissed', '1');
  }

  renderNotSupportedHardware() {
    const debugUpdates: entityConfig[] = new URLSearchParams(location.search).has('debug_update')
      ? [{ unique_id: 'update/Doorman Firmware', domain: 'update', name: 'Doorman Firmware', state: 'UPDATE AVAILABLE', value: '2026.6.0', id: '', detail: '', when: '', color: {}, value_numeric_history: [], sorting_weight: 0, speed: '' } as entityConfig]
      : [];
    const updates = debugUpdates.length > 0 ? debugUpdates : this.pendingUpdates;
    if (updates.length > 0) {
      return html`${updates.map(entity => {
        let version = entity.value ?? '';
        let label = version;
        if (version.includes('-')) {
          const [base, suffix] = version.split('-');
          label = `${base} (Experimental · ${suffix.split('.')[1]})`;
        }
        const basePath = getBasePath();
        return html`<infobox>
          <iconify-icon icon="mdi:package-up" height="24px"></iconify-icon>
          <span><b>Update available</b> — ${entity.name} ${label}</span>
          <button class="infobox-install-btn" @click="${() => { fetch(buildEntityActionUrl(basePath, entity, 'install'), { method: 'POST' }); if (this.logPanel) this.logPanel.expanded = true; }}">
            <iconify-icon icon="mdi:download" height="14px"></iconify-icon>
            Install
          </button>
        </infobox>`;
      })}`;
    }
    if (this.infoDismissed) return nothing;
    const dismissBtn = html`<button class="infobox-dismiss" @click="${this._dismissInfo}" title="Dismiss"><iconify-icon icon="mdi:close" height="16px"></iconify-icon></button>`;
    if (this.hardwareVersion.toLowerCase().includes('unknown') || this.hardwareVersion.toLowerCase().includes('unsupported')) {
      return html`<infobox class="warning">
        <iconify-icon icon="mdi:warning" height="24px"></iconify-icon>
        <span>For optimal performance, the official <a target="_blank" href="https://doorman.azon.ai/guide/what-is-doorman">Doorman S3</a> board is recommended.</span>
        ${dismissBtn}
      </infobox>`;
    } else {
      return html`<infobox>
        <iconify-icon icon="mdi:file-link" height="24px"></iconify-icon>
        <span>See the <a target="_blank" href="https://doorman.azon.ai/reference/entities">documentation</a> to learn more about each entity and how to configure them. You can also join our <a target="_blank" href="https://discord.gg/t2d34dvmBf">Discord server</a> for assistance and community support.</span>
        ${dismissBtn}
      </infobox>`;
    }
  }

  renderLog() {
    return this.config.log
      ? html`<esp-log rows="100" .scheme="${this.scheme}"></esp-log>`
      : nothing;
  }

  renderTitle() {
    return html`
      <h1>${this.config.title || html`Doorman`}</h1>
      <div>
        ${[this.hardwareVersion, this.firmwareVersion, `${this.uptime()} uptime`]
          .filter((n) => n)
          .map((e) => `${e}`)
          .join(" · ")}
      </div>
    `;
  }

  render() {
    if (new URLSearchParams(location.search).has('logs')) {
      return html`
        <esp-log rows="500" .scheme="${this.scheme}" .expanded="${true}" .standalone="${true}"></esp-log>
      `;
    }
    return html`
      <div class="bg-orbs" aria-hidden="true"></div>
      <header>
        <a href="https://doorman.azon.ai" id="logo" title="${this.version}">
          <esp-logo style="width: 30px; height: 30px;"></esp-logo>
        </a>
        <div class="title">
          ${this.renderTitle()}
        </div>
        <div class="status">
          <iconify-icon
            .icon="${!!this.connected ? "mdi:wifi" : "mdi:wifi-off"}"
            .title="${this.uptime()}"
            class="top-icon ${!!this.connected ? "connected" : ""}"
            id="beat"
          ></iconify-icon>
        </div>
      </header>
      ${this.renderNotSupportedHardware()}
      <esp-entity-table
        .scheme="${this.scheme}"
        .ota="${this.config.ota}"
        .showAll="${this.showAll}"
        @toggle-show-all="${() => { this.showAll = !this.showAll; localStorage.setItem('esp-show-all', String(this.showAll)); }}"
        @firmware-update="${(e: CustomEvent) => { this.pendingUpdates = e.detail; }}"
      ></esp-entity-table>
      ${this.renderLog()}
    `;
  }

  private _setUptime(uptime: number) {
    this.ping = uptime;
    this.connected = true;
    this.requestUpdate();
  }

  private _updateUptime(e: MessageEvent) {
    if (e.lastEventId) {
      this._setUptime(parseInt(e.lastEventId));
    }
  }

  static get styles() {
    return [cssReset, cssButton, cssApp, cssTab];
  }
}
