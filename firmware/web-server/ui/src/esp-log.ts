import { html, css, LitElement, nothing } from "lit";
import { customElement, property, state, query } from "lit/decorators.js";
import cssTab from "./css/tab";

interface recordConfig {
  type: string;
  level: string;
  tag: string;
  detail: string;
  when: string;
}

@customElement("esp-log")
export class DebugLog extends LitElement {
  @property({ type: Number }) rows = 10;
  @property({ type: String }) scheme = "";
  @property({ type: Boolean }) standalone = false;
  @state() logs: recordConfig[] = [];
  @state() expanded: boolean = false;
  @state() fullscreen: boolean = false;
  @state() private filterText: string = "";
  @state() private filterLevels: Set<string> = new Set();
  @query(".logs") private logsEl?: HTMLDivElement;
  private shouldAutoScroll = true;

  constructor() {
    super();
  }

  connectedCallback() {
    super.connectedCallback();
    window.source?.addEventListener("log", (e: Event) => {
      const container = this.logsEl;
      if (container) {
        const distanceFromBottom =
          container.scrollHeight - container.scrollTop - container.clientHeight;
        this.shouldAutoScroll = distanceFromBottom < 8;
      } else {
        this.shouldAutoScroll = true;
      }

      const messageEvent = e as MessageEvent;
      const d: String = messageEvent.data;

      const types: Record<string, string> = {
        "\x1b[1;31m": "e",
        "\x1b[0;33m": "w",
        "\x1b[0;32m": "i",
        "\x1b[0;35m": "c",
        "\x1b[0;36m": "d",
        "\x1b[0;37m": "v",
      };

      const prefix = d.slice(0, 7);
      const type = types[prefix];
      if (!type) {
        return;
      }

      const content = (d as string).replace(/\x1b\[[\d;]*m/g, "");
      const lines = content.split('\n');
      const firstLine = lines[0];
      const parts = firstLine.slice(3).split(":");
      const tag = parts.slice(0, 2).join(":");
      const firstDetail = firstLine.slice(5 + tag.length);
      const level = firstLine.slice(0, 3);
      const when = new Date().toTimeString().split(" ")[0];

      lines.forEach((line, index) => {
        const record = {
          type: type,
          level: level,
          tag: tag,
          detail: index === 0 ? firstDetail : line,
          when: when,
        } as recordConfig;
        this.logs.push(record);
      });

      this.logs = this.logs.slice(-this.rows);
    });
  }

  protected updated() {
    this.classList.toggle('fullscreen', this.fullscreen || this.standalone);
    this.classList.toggle('standalone', this.standalone);
    if (this.shouldAutoScroll && this.logsEl) {
      this.logsEl.scrollTop = this.logsEl.scrollHeight;
    }
  }

  private get filteredLogs(): recordConfig[] {
    return this.logs.filter((log) => {
      if (this.filterLevels.size && !this.filterLevels.has(log.type)) return false;
      if (this.filterText) {
        const q = this.filterText.toLowerCase();
        return (
          log.tag.toLowerCase().includes(q) ||
          log.detail.toLowerCase().includes(q)
        );
      }
      return true;
    });
  }

  private toggleLevel(lvl: string) {
    const s = new Set(this.filterLevels);
    s.has(lvl) ? s.delete(lvl) : s.add(lvl);
    this.filterLevels = s;
  }

  private clearFilters() {
    this.filterText = "";
    this.filterLevels = new Set();
  }

  private get hasFilter(): boolean {
    return !!this.filterText || this.filterLevels.size > 0;
  }

  private get levelCounts(): Record<string, number> {
    return this.logs.reduce((acc, log) => {
      acc[log.type] = (acc[log.type] || 0) + 1;
      return acc;
    }, {} as Record<string, number>);
  }

  render() {
    const filtered = this.filteredLogs;
    const counts = this.levelCounts;
    const levelDefs: { key: string; label: string }[] = [
      { key: "e", label: "E" },
      { key: "w", label: "W" },
      { key: "i", label: "I" },
      { key: "d", label: "D" },
      { key: "c", label: "C" },
      { key: "v", label: "V" },
    ];

    return html`
      <div
        class="log-header ${this.standalone ? 'log-header--standalone' : ''}"
        @click="${() => {
          if (this.standalone) return;
          this.expanded = !this.expanded;
          if (this.expanded) {
            this.shouldAutoScroll = true;
          } else {
            this.fullscreen = false;
          }
        }}"
      >
        <iconify-icon icon="mdi:console" height="16px"></iconify-icon>
        <span>Log Console</span>
        <span class="log-count">${this.logs.length}</span>
        ${(counts["e"] ?? 0) > 0 ? html`<span class="log-badge log-badge--e">${counts["e"]}</span>` : nothing}
        ${(counts["w"] ?? 0) > 0 ? html`<span class="log-badge log-badge--w">${counts["w"]}</span>` : nothing}
        ${this.standalone ? nothing : html`
          <div class="log-header-right">
            ${this.expanded ? html`
              <button class="fullscreen-btn" title="${this.fullscreen ? 'Exit fullscreen' : 'Fullscreen'}"
                @click="${(e: Event) => { e.stopPropagation(); this.fullscreen = !this.fullscreen; }}">
                <iconify-icon icon="${this.fullscreen ? 'mdi:fullscreen-exit' : 'mdi:fullscreen'}" height="16px"></iconify-icon>
              </button>
            ` : nothing}
            <iconify-icon
              icon="mdi:chevron-up"
              height="16px"
              class="chevron ${this.expanded ? "chevron--open" : ""}"
            ></iconify-icon>
          </div>
        `}
      </div>

      <div class="log-body ${this.expanded ? "log-body--open" : ""}">
        <div class="log-body-inner">
          <div class="tab-container">

            <div class="toolbar" @click="${(e: Event) => e.stopPropagation()}">
              <div class="search-wrap">
                <iconify-icon icon="mdi:magnify" height="14px" class="search-icon"></iconify-icon>
                <input
                  type="text"
                  class="search-input"
                  placeholder="Search…"
                  .value="${this.filterText}"
                  @input="${(e: Event) => {
                    this.filterText = (e.target as HTMLInputElement).value;
                  }}"
                />
              </div>

              <div class="pill-section">
                <div class="sep"></div>
                <div class="pill-group">
                  ${levelDefs.map(({ key, label }) => html`
                    <button
                      class="pill ${this.filterLevels.has(key) ? "pill--" + key : ""}"
                      @click="${() => this.toggleLevel(key)}"
                      title="${key}"
                    >
                      ${label}
                    </button>
                  `)}
                </div>
                <div class="sep"></div>
                <button
                  class="clear-btn"
                  ?disabled="${!this.hasFilter}"
                  @click="${() => this.clearFilters()}"
                  title="Clear filters"
                >
                  <iconify-icon icon="mdi:close" height="12px"></iconify-icon>
                  Clear
                </button>
              </div>
            </div>

            <div class="logs" color-scheme="${this.scheme}">
              <div class="thead">
                <div class="trow">
                  <div class="col-time">Time</div>
                  <div class="col-level">Lvl</div>
                  <div class="col-tag">Tag</div>
                  <div class="col-msg">Message</div>
                </div>
              </div>
              <div class="tbody">
                ${filtered.map((log: recordConfig) => {
                  const tagChips: Array<[string, boolean]> = [];
                  for (const [gi, m] of [...log.tag.matchAll(/\[([^\]]*)\]/g)].entries()) {
                    const inner = m[1];
                    const ci = inner.lastIndexOf(":");
                    if (gi === 0 && ci > 0 && /^\d+$/.test(inner.slice(ci + 1))) {
                      tagChips.push([inner.slice(0, ci), false]);
                      tagChips.push([inner.slice(ci + 1), true]);
                    } else {
                      tagChips.push([inner, gi > 0]);
                    }
                  }
                  return html`
                    <div class="trow trow--${log.type}">
                      <div class="col-time">${log.when}</div>
                      <div class="col-level">
                        <span class="level-badge level-badge--${log.type}">${log.type.toUpperCase()}</span>
                      </div>
                      <div class="col-tag">
                        ${tagChips.map(([text, sub]) => html`<span class="tag-chip${sub ? " tag-chip--sub" : ""}">${text}</span>`)}
                      </div>
                      <div class="col-msg">${log.detail}</div>
                    </div>
                  `;
                })}
              </div>
            </div>

            ${this.hasFilter ? html`
              <div class="filter-status">
                ${filtered.length} of ${this.logs.length} entries
              </div>
            ` : nothing}

          </div>
        </div>
      </div>
    `;
  }

  static get styles() {
    return [
      cssTab,
      css`
        :host {
          position: fixed;
          bottom: 0;
          left: 0;
          right: 0;
          z-index: 100;
          background-color: color-mix(in srgb, var(--c-bg, #1b1b1f) 50%, transparent);
          backdrop-filter: blur(12px);
          -webkit-backdrop-filter: blur(12px);
          border-top: 1px solid rgba(127, 127, 127, 0.15);
          display: block;
          isolation: isolate;
        }

        /* ── Header ── */
        .log-header {
          display: flex;
          align-items: center;
          gap: 8px;
          padding: 15px 16px;
          cursor: pointer;
          user-select: none;
          -webkit-tap-highlight-color: transparent;
          font-weight: 600;
          font-size: 14px;
          letter-spacing: 0.01em;
          position: relative;
          z-index: 2;
          background-color: transparent;
          border-bottom: 1px solid rgba(127, 127, 127, 0.12);
        }
        .log-header:hover {
          background-color: rgba(127, 127, 127, 0.08);
        }
        .log-header--standalone {
          cursor: default;
        }
        .log-header--standalone:hover {
          background-color: transparent;
        }
        :host(.standalone) {
          top: 0;
          display: flex;
          flex-direction: column;
        }
        :host(.standalone) .log-body {
          flex: 1;
          min-height: 0;
          grid-template-rows: 1fr !important;
        }
        :host(.standalone) .log-body-inner {
          height: 100%;
          overflow: visible;
          display: flex;
          flex-direction: column;
        }
        :host(.standalone) .tab-container {
          flex: 1;
          min-height: 0;
          display: flex;
          flex-direction: column;
          margin-bottom: 0;
        }
        :host(.standalone) .toolbar { flex-shrink: 0; }
        :host(.standalone) .logs {
          flex: 1;
          min-height: 0;
          max-height: none;
        }
        .log-count,
        .log-badge {
          display: inline-flex;
          align-items: center;
          font-size: 11px;
          font-weight: 700;
          line-height: 1;
          padding: 3px 7px;
          border-radius: 999px;
        }
        .log-count {
          background-color: rgba(127, 127, 127, 0.18);
          color: inherit;
          opacity: 0.85;
        }
        .log-badge--e {
          background: rgba(255, 60, 60, 0.15);
          color: #ff6b6b;
        }
        .log-badge--w {
          background: rgba(255, 210, 0, 0.12);
          color: #ffd000;
        }
        .log-header-right {
          margin-left: auto;
          display: flex;
          align-items: center;
          gap: 2px;
        }
        .chevron {
          transition: transform 0.3s cubic-bezier(0.4, 0, 0.2, 1);
        }
        .chevron--open {
          transform: rotate(180deg);
        }
        .log-body {
          display: grid;
          grid-template-rows: 0fr;
          transition: grid-template-rows 0.3s cubic-bezier(0.4, 0, 0.2, 1);
        }
        .log-body--open {
          grid-template-rows: 1fr;
        }
        .log-body-inner {
          overflow: hidden;
          min-height: 0;
        }

        /* ── Toolbar ── */
        .toolbar {
          display: grid;
          grid-template-columns: 1fr auto;
          align-items: center;
          gap: 8px;
          padding: 10px 16px;
          border-bottom: 1px solid rgba(127, 127, 127, 0.12);
          background-color: transparent;
          position: relative;
          z-index: 2;
        }
        .sep {
          width: 1px;
          height: 16px;
          background-color: rgba(127, 127, 127, 0.15);
          flex-shrink: 0;
        }

        /* Search */
        .search-wrap {
          position: relative;
          display: flex;
          align-items: center;
          min-width: 0;
        }
        .search-icon {
          position: absolute;
          left: 7px;
          color: rgba(127, 127, 127, 0.4);
          pointer-events: none;
        }
        .search-input {
          width: 100%;
          padding: 5px 6px 5px 24px;
          background: rgba(127, 127, 127, 0.08);
          border: 1px solid rgba(127, 127, 127, 0.15);
          border-radius: 6px;
          color: inherit;
          font-family: var(--vp-font-family-mono);
          font-size: 11px;
          outline: none;
          box-sizing: border-box;
          transition: border-color 0.15s ease, background 0.15s ease;
        }
        .search-input:focus {
          border-color: rgba(146, 105, 254, 0.5);
          background: rgba(146, 105, 254, 0.06);
        }
        .search-input::placeholder {
          color: rgba(127, 127, 127, 0.3);
        }

        /* Level pills */
        .pill-section {
          display: flex;
          align-items: center;
          gap: 10px;
          flex-shrink: 0;
        }
        .pill-group {
          display: flex;
          gap: 3px;
        }
        .pill {
          font-size: 11px;
          font-family: var(--vp-font-family-mono);
          font-weight: 600;
          padding: 5px 7px;
          border-radius: 6px;
          border: 1px solid transparent;
          background: rgba(127, 127, 127, 0.1);
          color: rgba(200, 200, 200, 0.4);
          cursor: pointer;
          transition: background 0.12s ease, color 0.12s ease;
        }
        .pill:hover {
          background: rgba(127, 127, 127, 0.2);
          color: rgba(200, 200, 200, 0.8);
        }
        .pill--e { background: rgba(255, 60, 60, 0.15); color: #ff6b6b; }
        .pill--w { background: rgba(255, 210, 0, 0.12); color: #ffd000; }
        .pill--i { background: rgba(50, 205, 50, 0.12); color: limegreen; }
        .pill--d { background: rgba(146, 105, 254, 0.15); color: #9269fe; }
        .pill--c { background: rgba(255, 0, 255, 0.1); color: magenta; }
        .pill--v { background: rgba(136, 136, 136, 0.15); color: #aaa; }
        .level-badge {
          display: inline-block;
          font-size: 10px;
          font-weight: 700;
          font-family: var(--vp-font-family-mono);
          padding: 1px 5px;
          border-radius: 4px;
          line-height: 1.5;
        }
        .level-badge--e { background: rgba(255, 60, 60, 0.15); color: #ff6b6b; }
        .level-badge--w { background: rgba(255, 210, 0, 0.12); color: #ffd000; }
        .level-badge--i { background: rgba(50, 205, 50, 0.12); color: limegreen; }
        .level-badge--d { background: rgba(146, 105, 254, 0.15); color: #9269fe; }
        .level-badge--c { background: rgba(255, 0, 255, 0.1); color: magenta; }
        .level-badge--v { background: rgba(136, 136, 136, 0.15); color: #aaa; }

        /* Clear button */
        .clear-btn {
          display: flex;
          align-items: center;
          gap: 4px;
          background: none;
          border: none;
          border-radius: 6px;
          color: rgba(200, 200, 200, 0.35);
          font-size: 11px;
          font-family: var(--vp-font-family-mono);
          padding: 5px 6px;
          cursor: pointer;
          flex-shrink: 0;
          transition: color 0.15s ease, background 0.15s ease;
          white-space: nowrap;
        }
        .clear-btn:not([disabled]):hover {
          color: rgba(200, 200, 200, 0.8);
          background: rgba(127, 127, 127, 0.1);
        }
        .clear-btn[disabled] {
          opacity: 0.25;
          cursor: default;
        }

        /* ── Table container ── */
        .tab-container {
          border: none;
          border-radius: 0;
          margin-bottom: 0;
          position: relative;
          z-index: 1;
        }
        .logs {
          overflow-x: auto;
          overflow-y: auto;
          max-height: 40vh;
          padding: 0 16px;
        }

        /* ── Table structure ── */
        .trow {
          display: flex;
          align-items: baseline;
          gap: 0;
        }
        .thead {
          position: sticky;
          top: 0;
          z-index: 1;
          min-width: 100%;
          background-color: color-mix(in srgb, var(--c-bg, #1b1b1f) 92%, transparent);
          border-bottom: 1px solid rgba(127, 127, 127, 0.12);
          box-shadow: 16px 0 0 color-mix(in srgb, var(--c-bg, #1b1b1f) 92%, transparent),
                      -16px 0 0 color-mix(in srgb, var(--c-bg, #1b1b1f) 92%, transparent);
        }
        .thead .trow {
          padding: 7px 0 6px;
          min-width: 100%;
        }
        .thead .col-time,
        .thead .col-level,
        .thead .col-tag,
        .thead .col-msg {
          font-family: inherit;
          font-size: 10px;
          font-weight: 600;
          letter-spacing: 0.07em;
          text-transform: uppercase;
          color: rgba(127, 127, 127, 0.4);
        }
        .tbody {
          padding-bottom: 16px;
        }
        .tbody .trow {
          padding: 5px 0;
          border-bottom: 1px solid rgba(127, 127, 127, 0.06);
          align-items: center;
          transition: background 0.1s ease;
        }
        .tbody .trow:last-child {
          border-bottom: none;
        }
        .tbody .trow:hover {
          background: rgba(127, 127, 127, 0.06);
        }

        /* ── Column definitions ── */
        .col-time {
          flex: 0 0 64px;
          font-family: var(--vp-font-family-mono);
          font-size: 10px;
          color: rgba(127, 127, 127, 0.38);
          font-variant-numeric: tabular-nums;
          white-space: nowrap;
          padding-right: 10px;
        }
        .col-level {
          flex: 0 0 38px;
          display: flex;
          align-items: center;
          justify-content: center;
          padding-right: 10px;
        }
        .col-tag {
          flex: 0 0 220px;
          width: 220px;
          display: flex;
          flex-wrap: wrap;
          gap: 3px;
          align-items: center;
          padding-right: 12px;
        }
        .col-msg {
          flex: 1;
          min-width: 0;
          font-family: var(--vp-font-family-mono);
          font-size: 12px;
          line-height: 1.55;
          word-break: break-word;
          white-space: pre-wrap;
          color: rgba(200, 200, 200, 0.55);
        }

        /* ── Message color by level ── */
        .trow--e .col-msg { color: rgba(255, 107, 107, 0.9); }
        .trow--w .col-msg { color: rgba(255, 208, 0, 0.85); }
        .trow--i .col-msg { color: rgba(50, 205, 50, 0.8); }
        .trow--d .col-msg { color: rgba(146, 105, 254, 0.78); }
        .trow--c .col-msg { color: rgba(255, 0, 255, 0.75); }
        .trow--v .col-msg { color: rgba(136, 136, 136, 0.55); }

        /* ── Tag chips ── */
        .tag-chip {
          display: inline-block;
          font-family: var(--vp-font-family-mono);
          font-size: 10px;
          font-weight: 500;
          padding: 1px 5px;
          border-radius: 4px;
          background: rgba(127, 127, 127, 0.12);
          border: 1px solid rgba(127, 127, 127, 0.1);
          color: rgba(200, 200, 200, 0.65);
          white-space: nowrap;
          line-height: 1.5;
        }
        .tag-chip--sub {
          font-size: 9px;
          opacity: 0.6;
          background: rgba(127, 127, 127, 0.07);
        }

        /* ── Status bar ── */
        .filter-status {
          font-family: var(--vp-font-family-mono);
          font-size: 11px;
          color: rgba(127, 127, 127, 0.45);
          padding: 4px 16px 6px;
          border-top: 1px solid rgba(127, 127, 127, 0.08);
        }

        /* ── Fullscreen mode ── */
        .fullscreen-btn {
          display: flex;
          align-items: center;
          justify-content: center;
          width: 28px;
          height: 28px;
          padding: 0;
          border: none;
          border-radius: 6px;
          background: none;
          color: rgba(200, 200, 200, 0.4);
          cursor: pointer;
          transition: color 0.15s, background 0.15s;
          flex-shrink: 0;
        }
        .fullscreen-btn:hover {
          color: currentColor;
          background: rgba(127, 127, 127, 0.12);
        }
        :host(.fullscreen) {
          top: var(--header-height, 0px);
          display: flex;
          flex-direction: column;
        }
        :host(.fullscreen) .log-body {
          flex: 1;
          min-height: 0;
          grid-template-rows: 1fr !important;
        }
        :host(.fullscreen) .log-body-inner {
          height: 100%;
          overflow: visible;
          display: flex;
          flex-direction: column;
        }
        :host(.fullscreen) .tab-container {
          flex: 1;
          min-height: 0;
          display: flex;
          flex-direction: column;
          margin-bottom: 0;
        }
        :host(.fullscreen) .toolbar {
          flex-shrink: 0;
        }
        :host(.fullscreen) .logs {
          flex: 1;
          min-height: 0;
          max-height: none;
        }

        /* ── Responsive ── */
        @media (max-width: 640px) {
          .col-time { display: none; }
          .col-tag { max-width: 100px; min-width: 40px; }
        }
        @media (max-width: 415px) {
          .pill-section { display: none; }
          .toolbar { grid-template-columns: 1fr; }
          .col-tag { display: none; }
        }
      `,
    ];
  }
}