import { html, css, LitElement, TemplateResult, nothing } from "lit";
import { customElement, state, property } from "lit/decorators.js";
import cssReset from "./css/reset";
import cssButton from "./css/button";
import cssInput from "./css/input";
import cssEntityTable from "./css/esp-entity-table";
import cssTab from "./css/tab";
import "./esp-entity-chart";
import "iconify-icon";

interface entityConfig {
  unique_id: string;
  sorting_weight: number;
  sorting_group?: string;
  domain: string;
  id: string;
  state: string;
  detail: string;
  value: string;
  name: string;
  device?: string;  // Device name for hierarchical URLs (sub-devices only)
  entity_category?: number;
  when: string;
  icon?: string;
  option?: string[];
  assumed_state?: boolean;
  brightness?: number;
  color_mode?: string;
  color: object;
  target_temperature?: number;
  target_temperature_low?: number;
  target_temperature_high?: number;
  min_temp?: number;
  max_temp?: number;
  min_value?: string;
  max_value?: string;
  step?: number;
  min_length?: number;
  max_length?: number;
  pattern?: string;
  current_temperature?: number;
  modes?: number[];
  mode?: number;
  speed_count?: number;
  speed_level?: number;
  speed: string;
  effects?: string[];
  effect?: string;
  has_action?: boolean;
  value_numeric_history: number[];
  uom?: string;
  is_disabled_by_default?: boolean;
  // Water heater specific
  away?: boolean;
  is_on?: boolean;
  // Infrared specific
  supports_transmitter?: boolean;
  supports_receiver?: boolean;
}

interface groupConfig {
  name: string;
  sorting_weight: number;  
}

export const stateOn = "ON";
export const stateOff = "OFF";

export function getBasePath() {
  let str = window.location.pathname;
  return str.endsWith("/") ? str.slice(0, -1) : str;
}

// ID format detection and parsing helpers
// New format: "domain/entity_name" or "domain/device_name/entity_name"
// Old format: "domain-object_id" (deprecated)

function isNewIdFormat(id: string): boolean {
  return id.includes('/');
}

function parseDomainFromId(id: string): string {
  if (isNewIdFormat(id)) {
    return id.split('/')[0];
  }
  // Old format: domain-object_id
  return id.split('-')[0];
}

function buildEntityActionUrl(basePath: string, entity: entityConfig, action: string): string {
  if (isNewIdFormat(entity.unique_id)) {
    // New format: /{domain}/{device?}/{name}/{action}
    const entityName = encodeURIComponent(entity.name);
    const devicePart = entity.device
      ? `${encodeURIComponent(entity.device)}/`
      : '';
    return `${basePath}/${entity.domain}/${devicePart}${entityName}/${action}`;
  }
  // Old format: /{domain}/{object_id}/{action}
  const objectId = entity.unique_id.split('-').slice(1).join('-');
  return `${basePath}/${entity.domain}/${objectId}/${action}`;
}

function buildIdFetchUrl(basePath: string, id: string): string {
  // URL-encode each path segment for fetching detail_all
  let urlPath: string;
  if (isNewIdFormat(id)) {
    // New format: domain/name or domain/device/name
    urlPath = id.split('/').map((s: string) => encodeURIComponent(s)).join('/');
  } else {
    // Old format: domain-object_id -> domain/object_id
    const parts = id.split('-');
    const domain = parts[0];
    const objectId = parts.slice(1).join('-');
    urlPath = `${domain}/${encodeURIComponent(objectId)}`;
  }
  return `${basePath}/${urlPath}?detail=all`;
}

interface RestAction {
  restAction(entity?: entityConfig, action?: string): void;
}

@customElement("esp-entity-table")
export class EntityTable extends LitElement implements RestAction {
  @state() entities: entityConfig[] = [];
  @state() has_controls: boolean = false;
  @state() show_setup: boolean = false;
  @state() activeGroup: string = "";
  @state() searchQuery: string = "";
  @state() otaFilename: string = "";
  @property({ type: Boolean }) showAll: boolean = false;
  @property({ type: Boolean }) ota: boolean = false;

  private _userSelectedGroup: boolean = false;
  private _touchStartX = 0;
  private _touchStartY = 0;
  private _swipeLocked: boolean | null = null;
  private _actionRenderer = new ActionRenderer();
  private _basePath = getBasePath();
  private groups: groupConfig[] = [] 
  private static ENTITY_UNDEFINED = "States";
  private static ENTITY_CATEGORIES = [
    "Sensor and Control",
    "Configuration",
    "Diagnostic",
  ];

  private _unknown_state_events: {[key: string]: number} = {};
  private _navScrollInitialized = false;

  protected updated() {
    if (this._navScrollInitialized) return;
    const nav = this.shadowRoot?.querySelector('.nav-group') as HTMLElement | null;
    const sentinel = this.shadowRoot?.querySelector('.nav-sentinel') as HTMLElement | null;
    const layout = this.shadowRoot?.querySelector('.layout') as HTMLElement | null;
    if (!nav || !sentinel || !layout) return;
    this._navScrollInitialized = true;
    const headerHeight = parseFloat(
      document.documentElement.style.getPropertyValue('--header-height') || '64'
    );
    new IntersectionObserver(([entry]) => {
      nav.classList.toggle('is-stuck', !entry.isIntersecting);
    }, {
      rootMargin: `-${headerHeight}px 0px 0px 0px`,
      threshold: 1.0,
    }).observe(sentinel);
    layout.addEventListener('touchmove', this._onTouchMove, { passive: false });
  }

  connectedCallback() {
    super.connectedCallback();

    window.source?.addEventListener('state', (e: Event) => {
      const messageEvent = e as MessageEvent;
      const data = JSON.parse(messageEvent.data);
      // Prefer name_id (new format) over id (legacy format) for entity identification
      const entityId = data.name_id || data.id;
      let idx = this.entities.findIndex((x) => x.unique_id === entityId);
      if (idx != -1 && entityId) {
        if (typeof data.value === 'number') {
          let history = [...this.entities[idx].value_numeric_history];
          history.push(data.value);
          this.entities[idx].value_numeric_history = history.splice(-50);
        }

        if(data.id == 'switch-setup_mode') {
          if(data.value) {
            this.show_setup = true;
          } else {
            this.show_setup = false;
          }
          this.requestUpdate();
        }

        delete data.id;
        delete data.name_id;
        delete data.domain;
        delete data.unique_id;
        Object.assign(this.entities[idx], data);
        this.requestUpdate();
      } else {
        // is it a `detail_all` event already? (has name and domain)
        if (data?.name && data?.domain) {
          this.addEntity(data);
        } else {
          if (this._unknown_state_events[entityId]) {
            this._unknown_state_events[entityId]++;
          } else {
            this._unknown_state_events[entityId] = 1;
          }
          // ignore the first few events, maybe the esp will send a detail_all
          // event soon
          if (this._unknown_state_events[entityId] < 1) {
            return;
          }

          fetch(buildIdFetchUrl(this._basePath, entityId), {
            method: 'GET',
          })
              .then((r) => {
                console.log(r);
                if (!r.ok) {
                  throw new Error(`HTTP error! Status: ${r.status}`);
                }
                return r.json();
              })
              .then((data) => {
                console.log(data);
                this.addEntity(data);
              })
              .catch((error) => {
                console.error('Fetch error:', error);
              });
        }
      }
    });

    window.source?.addEventListener("sorting_group", (e: Event) => {
      const messageEvent = e as MessageEvent;
      const data = JSON.parse(messageEvent.data);
      const groupIndex = this.groups.findIndex((x) => x.name === data.name);
      if (groupIndex === -1) {
        let group = {
           ...data,
        } as groupConfig;
        this.groups.push(group);
        this.groups.sort((a, b) => {
          return a.sorting_weight < b.sorting_weight  
            ? -1  
            : 1  
        });
      }
    });

    this.groups = EntityTable.ENTITY_CATEGORIES.map((category, index) => ({
      name: category,
      sorting_weight: index
    }));

    this.groups.push({
      name: EntityTable.ENTITY_UNDEFINED,
      sorting_weight: -1 
    });
  }

  addEntity(data: any) {
    // Prefer name_id (new format) over id (legacy format) for entity identification
    const entityId = data.name_id || data.id;
    let idx = this.entities.findIndex((x) => x.unique_id === entityId);
    if (idx === -1 && entityId) {
      // Dynamically add discovered entity
      // domain comes from JSON (new format) or parsed from id (old format)
      const domain = data.domain || parseDomainFromId(entityId);
      let entity = {
        ...data,
        domain: domain,
        unique_id: entityId,
        entity_category: data.entity_category,
        sorting_group: data.sorting_group ?? (EntityTable.ENTITY_CATEGORIES[parseInt(data.entity_category)] || EntityTable.ENTITY_UNDEFINED),
        value_numeric_history: [data.value],
      } as entityConfig;
      entity.has_action = this.hasAction(entity);
      if (entity.has_action) {
        this.has_controls = true;
      }
      this.entities.push(entity);
      this.entities.sort((a, b) => {
        const sortA = a.sorting_weight ?? a.name;
        const sortB = b.sorting_weight ?? b.name;
        return a.sorting_group < b.sorting_group
          ? -1
          : a.sorting_group === b.sorting_group
          ? sortA === sortB
            ? a.name.toLowerCase() < b.name.toLowerCase()
              ? -1
              : 1
            : sortA < sortB
              ? -1
              : 1
          : 1
      });

      if(data.id == 'switch-setup_mode') {
        if(data.value) {
          this.show_setup = true;
        } else {
          this.show_setup = false;
        }
      }

      this.requestUpdate();
    }
  }

  hasAction(entity: entityConfig): boolean {
    return `render_${entity.domain}` in this._actionRenderer;
  }

  control(entity: entityConfig) {
    this._actionRenderer.entity = entity;
    this._actionRenderer.actioner = this;
    return this._actionRenderer.exec(
      `render_${entity.domain}` as ActionRendererMethodKey
    );
  }

  restAction(entity: entityConfig, action: string) {
    fetch(buildEntityActionUrl(this._basePath, entity, action), {
      method: "POST",
      headers:{
        'Content-Type': 'application/x-www-form-urlencoded'
      },
    }).then((r) => {
      console.log(r);
    });
  }


  private static _DOMAIN_ICONS: Record<string, string> = {
    binary_sensor: "mdi:checkbox-blank-circle-outline",
    sensor:        "mdi:chart-line",
    switch:        "mdi:toggle-switch-outline",
    button:        "mdi:gesture-tap",
    select:        "mdi:format-list-bulleted",
    number:        "mdi:numeric",
    text:          "mdi:form-textbox",
    lock:          "mdi:lock-outline",
    cover:         "mdi:window-shutter",
    fan:           "mdi:fan",
    light:         "mdi:lightbulb-outline",
    climate:       "mdi:thermostat",
    date:          "mdi:calendar",
    time:          "mdi:clock-outline",
    datetime:      "mdi:calendar-clock",
    update:        "mdi:update",
    event:         "mdi:calendar-star",
    valve:         "mdi:pipe-valve",
    water_heater:  "mdi:water-boiler",
    infrared:      "mdi:remote",
  };

  private _subGroupPrefix(entities: entityConfig[]): string {
    if (entities.length <= 1) return '';
    const names = entities.map(e => {
      const c = e.name.indexOf(': ');
      return c >= 0 ? e.name.slice(c + 2) : e.name;
    });
    let prefix = names[0];
    for (const name of names.slice(1)) {
      let i = 0;
      while (i < prefix.length && i < name.length && prefix[i] === name[i]) i++;
      prefix = prefix.slice(0, i);
    }
    return prefix.replace(/\s*-\s*$/, '').trimEnd();
  }

  private _renderEntityRow(component: entityConfig, groupName: string, idx: number, sgPrefix?: string) {
    const icon = component.icon
      || (component.domain === 'lock'
        ? (component.state === 'LOCKED' ? 'mdi:lock' : component.state === 'UNLOCKED' ? 'mdi:lock-open-variant' : EntityTable._DOMAIN_ICONS['lock'])
        : component.domain === 'binary_sensor'
          ? (component.state === stateOn ? 'mdi:checkbox-marked-circle' : 'mdi:checkbox-blank-circle-outline')
          : EntityTable._DOMAIN_ICONS[component.domain])
      || "mdi:help-circle-outline";
    return html`
      <div
        class="entity-row"
        .domain="${component.domain}"
        @click="${this._handleEntityRowClick}"
      >
        <div>
          <iconify-icon icon="${icon}" height="24px"></iconify-icon>
        </div>
        <div>
          <div class="entity-name">${this.formatComponentName(component, groupName, idx, sgPrefix)}</div>
          ${!component.has_action && component.when ? html`<div class="entity-when">${component.when}</div>` : nothing}
        </div>
        <div>
          ${this.has_controls && component.has_action
            ? this.control(component)
            : component.domain === "event"
            ? (component as any).event_type
              ? html`<div class="event-type-badge">${(component as any).event_type}</div>`
              : html`<div class="state-empty">—</div>`
            : component.domain === "sensor" && component.state
            ? html`<div class="sensor-state"><span class="sensor-value">${component.value ?? component.state}</span>${component.uom ? html`<span class="sensor-uom">${component.uom}</span>` : nothing}</div>`
            : component.state
            ? html`<div>${component.state}</div>`
            : html`<div class="state-empty">—</div>`}
        </div>
        ${component.domain === "sensor"
          ? html`<esp-entity-chart .chartdata="${component.value_numeric_history}"></esp-entity-chart>`
          : nothing}
      </div>
    `;
  }

  private _groupBy(xs: Array<any>, key: string): Map<string, Array<any>> {
    const groupedMap = xs.reduce(function (rv, x) {
      (
        rv.get(x[key]) ||
        (() => {
          let tmp: Array<any> = [];
          rv.set(x[key], tmp);
          return tmp;
        })()
      ).push(x);
      return rv;
    }, new Map<string, Array<any>>());

    const sortedGroupedMap = new Map<string, Array<any>>();
    for (const group of this.groups) {
      const groupName = group.name;
      if (groupedMap.has(groupName)) {
        sortedGroupedMap.set(groupName, groupedMap.get(groupName) || []);
      }
    }

    return sortedGroupedMap;
  }

  private _selectGroup(name: string, animDirection: -1 | 0 | 1 = 0) {
    this.activeGroup = name;
    this._userSelectedGroup = true;
    this.searchQuery = '';
    if (animDirection === 0) return;
    requestAnimationFrame(() => {
      const ca = this.shadowRoot?.querySelector('.content-area') as HTMLElement | null;
      if (!ca) return;
      const cls = animDirection > 0 ? 'swipe-in-right' : 'swipe-in-left';
      ca.classList.remove('swipe-in-right', 'swipe-in-left');
      void ca.offsetWidth;
      ca.classList.add(cls);
      ca.addEventListener('animationend', () => ca.classList.remove(cls), { once: true });
    });
  }

  private _visibleGroupNames(): string[] {
    const filtered = this.showAll ? this.entities : this.entities.filter(e => !e.is_disabled_by_default);
    const allNames = Array.from(this._groupBy(filtered, "sorting_group").keys());
    const subGroupParents = new Set<string>();
    for (const name of allNames) {
      if (name.includes(': ')) subGroupParents.add(name.slice(0, name.indexOf(': ')).trim());
    }
    const allNameSet = new Set(allNames);
    return this.groups
      .filter(g => !g.name.includes(': ') && (allNameSet.has(g.name) || subGroupParents.has(g.name)))
      .map(g => g.name);
  }

  private _swipeToGroup(direction: 1 | -1) {
    const names = this._visibleGroupNames();
    const idx = names.indexOf(this.activeGroup);
    const next = idx + direction;
    if (next < 0 || next >= names.length) return;
    this._selectGroup(names[next], direction);
    requestAnimationFrame(() => {
      const navItems = this.shadowRoot?.querySelectorAll<HTMLElement>('.nav-item');
      navItems?.[next]?.scrollIntoView({ behavior: 'smooth', block: 'nearest', inline: 'center' });
    });
  }

  private _clickGroup(name: string) {
    const names = this._visibleGroupNames();
    const dir = Math.sign(names.indexOf(name) - names.indexOf(this.activeGroup)) as -1 | 0 | 1;
    this._selectGroup(name, dir);
  }

  private _onTouchStart = (e: TouchEvent) => {
    const onSlider = e.composedPath().some(el =>
      (el instanceof HTMLInputElement && el.type === 'range') ||
      (el instanceof Element && el.tagName === 'ESP-RANGE-SLIDER')
    );
    if (onSlider) {
      this._swipeLocked = false;
      return;
    }
    this._touchStartX = e.touches[0].clientX;
    this._touchStartY = e.touches[0].clientY;
    this._swipeLocked = null;
    const ca = this.shadowRoot?.querySelector('.content-area') as HTMLElement | null;
    if (ca) ca.style.transition = 'none';
  };

  private _onTouchMove = (e: TouchEvent) => {
    const dx = e.touches[0].clientX - this._touchStartX;
    const dy = e.touches[0].clientY - this._touchStartY;
    if (this._swipeLocked === null) {
      if (Math.abs(dx) > 8 || Math.abs(dy) > 8)
        this._swipeLocked = Math.abs(dx) > Math.abs(dy);
      return;
    }
    if (!this._swipeLocked) return;
    e.preventDefault();
    const names = this._visibleGroupNames();
    const idx = names.indexOf(this.activeGroup);
    const atBoundary = (dx < 0 && idx >= names.length - 1) || (dx > 0 && idx <= 0);
    const ca = this.shadowRoot?.querySelector('.content-area') as HTMLElement | null;
    if (ca) {
      ca.style.transition = 'none';
      ca.style.transform = `translateX(${dx * (atBoundary ? 0.12 : 0.35)}px)`;
    }
  };

  private _onTouchEnd = (e: TouchEvent) => {
    const dx = e.changedTouches[0].clientX - this._touchStartX;
    const dy = e.changedTouches[0].clientY - this._touchStartY;
    const ca = this.shadowRoot?.querySelector('.content-area') as HTMLElement | null;
    if (this._swipeLocked && Math.abs(dx) > Math.abs(dy) && Math.abs(dx) > 50) {
      if (ca) { ca.style.transition = 'none'; ca.style.transform = ''; }
      this._swipeToGroup(dx < 0 ? 1 : -1);
    } else if (ca?.style.transform) {
      ca.style.transition = 'transform 0.3s cubic-bezier(0.25, 0.46, 0.45, 0.94)';
      ca.style.transform = '';
    }
  };

  /** Render entities for the active group followed by any sub-group sections. */
  private _renderGroupEntities(
    primaryEntities: entityConfig[],
    subGroups: Array<{ label: string; entities: entityConfig[] }>,
    groupName: string,
    allEntities: entityConfig[],
    hasExtraSection = false
  ): TemplateResult[] {
    const localIds = new Set<string>(
      [...primaryEntities, ...subGroups.flatMap(sg => sg.entities)].map(e => e.id)
    );
    // Inject entities from other groups whose stripped display name matches this group's name.
    const injected = allEntities.filter(e => {
      if (localIds.has(e.id)) return false;
      const c = e.name.indexOf(': ');
      const dn = c >= 0 ? e.name.slice(c + 2) : e.name;
      return dn === groupName;
    });

    const result: TemplateResult[] = [];
    let globalIdx = 0;
    const allPrimary = [...primaryEntities, ...injected];
    const showGeneralHeader = allPrimary.length > 0 && (subGroups.length > 0 || hasExtraSection);

    if (showGeneralHeader) {
      result.push(html`<div class="sub-group-header"><span class="sub-group-header__label">General</span></div>`);
    }
    for (const entity of allPrimary) {
      result.push(this._renderEntityRow(entity, groupName, globalIdx++));
    }

    for (const sg of subGroups) {
      const sgPrefix = this._subGroupPrefix(sg.entities);
      result.push(html`
        <div class="sub-group-header">
          <span class="sub-group-header__label">${sg.label}</span>
        </div>
      `);
      for (const entity of sg.entities) {
        result.push(this._renderEntityRow(entity, groupName, globalIdx++, sgPrefix));
      }
    }

    return result;
  }

  render() {
    const hiddenCount = this.entities.filter(e => e.is_disabled_by_default).length;
    const entities = this.showAll
      ? this.entities
      : this.entities.filter(e => !e.is_disabled_by_default);
    const grouped = this._groupBy(entities, "sorting_group");
    const allElems = Array.from(grouped, ([name, value]) => ({ name, value }));
    const allElemsMap = new Map(allElems.map(e => [e.name, e]));

    // Primary groups have no ":" in their name. A group qualifies as primary even
    // if it has no direct entities, as long as it is the parent of a sub-group.
    const subGroupParents = new Set<string>();
    for (const g of allElems) {
      if (g.name.includes(': ')) subGroupParents.add(g.name.slice(0, g.name.indexOf(': ')).trim());
    }
    // Use this.groups (sorted by sorting_weight) to preserve declared order.
    const elems = this.groups
      .filter(g => !g.name.includes(': ') && (allElemsMap.has(g.name) || subGroupParents.has(g.name)))
      .map(g => allElemsMap.get(g.name) ?? { name: g.name, value: [] as entityConfig[] });

    if (!this._userSelectedGroup && elems.length > 0) {
      this.activeGroup = elems[0].name;
    }
    const activeGroup = elems.find((g) => g.name === this.activeGroup) ?? elems[0];

    // Sub-groups whose name starts with "ActiveGroup: ", sorted by their declared weight.
    const activeSubGroups = activeGroup
      ? allElems
          .filter(g => g.name.startsWith(activeGroup.name + ': '))
          .map(g => {
            const groupConfig = this.groups.find(gr => gr.name === g.name);
            return {
              label: g.name.slice(activeGroup.name.length + 2),
              entities: g.value,
              sorting_weight: groupConfig?.sorting_weight ?? 0,
            };
          })
          .sort((a, b) => a.sorting_weight - b.sorting_weight)
      : [];

    const activeGroupEntities: entityConfig[] = activeGroup ? activeGroup.value : [];

    const hiddenCountInGroup = activeGroup
      ? this.entities.filter(e =>
          e.is_disabled_by_default && (
            e.sorting_group === activeGroup.name ||
            e.sorting_group?.startsWith(activeGroup.name + ': ')
          )
        ).length
      : 0;

    const searchQ = this.searchQuery.trim().toLowerCase();
    const isSearching = searchQ.length > 0;
    const stripSearchName = (name: string): string => {
      const c = name.indexOf(': ');
      return c >= 0 ? name.slice(c + 2) : name;
    };
    const rankResult = (e: entityConfig): number => {
      const full = e.name.toLowerCase();
      const stripped = stripSearchName(e.name).toLowerCase();
      if (full === searchQ || stripped === searchQ) return 0;
      if (full.startsWith(searchQ) || stripped.startsWith(searchQ)) return 1;
      return 2;
    };
    const searchResults = isSearching
      ? entities
          .filter(e => {
            const full = e.name.toLowerCase();
            const stripped = stripSearchName(e.name).toLowerCase();
            return full.includes(searchQ) || stripped.includes(searchQ);
          })
          .sort((a, b) => rankResult(a) - rankResult(b))
      : [];

    if (elems.length === 0 && !isSearching) {
      return html`
        <div class="loading-state">
          <iconify-icon icon="mdi:loading" height="56px" class="loading-spinner"></iconify-icon>
          <div class="loading-title">Loading</div>
          <p class="loading-text">Fetching configuration and entities from your Doorman.<br>This usually takes just a moment.</p>
        </div>
      `;
    }

    return html`
      <div class="layout" @touchstart="${this._onTouchStart}" @touchend="${this._onTouchEnd}">
        <div class="nav-sentinel" aria-hidden="true"></div>
        <nav class="nav-group" @touchstart="${(e: Event) => e.stopPropagation()}" @touchmove="${(e: Event) => e.stopPropagation()}" @touchend="${(e: Event) => e.stopPropagation()}">
          <div class="nav-search-wrap">
            <iconify-icon icon="mdi:magnify" height="14px" class="nav-search-icon"></iconify-icon>
            <input
              class="nav-search-input"
              type="search"
              placeholder="Search…"
              .value="${this.searchQuery}"
              @input="${(e: Event) => { this.searchQuery = (e.target as HTMLInputElement).value; }}"
            />
            ${this.searchQuery ? html`
              <button class="nav-search-clear" @click="${() => { this.searchQuery = ''; }}">
                <iconify-icon icon="mdi:close" height="14px"></iconify-icon>
              </button>
            ` : nothing}
          </div>
          <div class="nav-search-divider"></div>
          <div class="nav-items-scroll">
            ${elems.map((group, i) => {
              const isActive = !isSearching && this.activeGroup === group.name;
              if (i === 0) return html`
                <button
                  class="nav-item nav-item--quick ${isActive ? "active" : ""}"
                  @click="${() => this._clickGroup(group.name)}"
                >
                  <iconify-icon icon="mdi:flash" height="13px" class="nav-item-icon"></iconify-icon>
                  <span class="nav-item-text">${group.name || EntityTable.ENTITY_UNDEFINED}</span>
                </button>
                <div class="nav-item-divider"></div>
              `;
              return html`
                <button
                  class="nav-item ${isActive ? "active" : ""}"
                  @click="${() => this._clickGroup(group.name)}"
                >
                  ${group.name || EntityTable.ENTITY_UNDEFINED}
                </button>
              `;
            })}
            ${hiddenCount > 0 ? html`
              <div class="nav-item-divider"></div>
              <button class="nav-item nav-show-all" title="${this.showAll ? 'Show less' : 'Show all'}" @click="${() => this.dispatchEvent(new CustomEvent('toggle-show-all', { bubbles: true, composed: true }))}">
                <iconify-icon icon="${this.showAll ? 'mdi:eye-off-outline' : 'mdi:eye-outline'}" height="13px" class="nav-item-icon"></iconify-icon>
                <span class="nav-show-all-text">${this.showAll ? 'Show less' : 'Show all'}</span>
              </button>
            ` : nothing}
          </div>
        </nav>
        <div class="content-area">
          ${isSearching ? html`
            <div class="tab-container">
              ${searchResults.length > 0
                ? (() => {
                    const byGroup = new Map<string, entityConfig[]>();
                    for (const e of searchResults) {
                      const g = e.sorting_group ?? '';
                      if (!byGroup.has(g)) byGroup.set(g, []);
                      byGroup.get(g)!.push(e);
                    }
                    const sorted = Array.from(byGroup.entries()).sort(([a, aEntities], [b, bEntities]) => {
                      const minRankA = Math.min(...aEntities.map(e => rankResult(e)));
                      const minRankB = Math.min(...bEntities.map(e => rankResult(e)));
                      if (minRankA !== minRankB) return minRankA - minRankB;
                      const wa = this.groups.find(g => g.name === a)?.sorting_weight ?? 0;
                      const wb = this.groups.find(g => g.name === b)?.sorting_weight ?? 0;
                      return wa - wb;
                    });
                    return sorted.map(([groupName, groupEntities]) => html`
                      <div class="sub-group-header">
                        <span class="sub-group-header__label">${groupName || EntityTable.ENTITY_UNDEFINED}</span>
                      </div>
                      ${groupEntities.map((component, idx) =>
                        this._renderEntityRow(component, groupName, idx)
                      )}
                    `);
                  })()
                : html`<div class="search-empty">
                    <iconify-icon icon="mdi:magnify-close"></iconify-icon>
                    <span>No results for "${this.searchQuery}"</span>
                  </div>`}
            </div>
          ` : activeGroup ? html`
            <div class="tab-container">
              ${this._renderGroupDescription(activeGroup.name, activeGroupEntities)}
              ${(() => {
                  const showOta = !!(this.ota && (
                    activeGroupEntities.some((c: entityConfig) => c.domain === 'update') ||
                    activeGroup.name.toLowerCase().includes('firmware') ||
                    activeGroup.name.toLowerCase().includes('update')
                  ));
                  return html`
                    ${this._renderGroupEntities(activeGroupEntities, activeSubGroups, activeGroup.name, entities, showOta)}
                    ${showOta ? this._renderOta() : nothing}
                  `;
                })()}
            </div>
            ${hiddenCountInGroup > 0 ? html`
              <div class="show-all-row ${this.showAll ? 'show-all-row--expanded' : ''}">
                <button class="show-all-row-btn" @click="${() => this.dispatchEvent(new CustomEvent('toggle-show-all', { bubbles: true, composed: true }))}">
                  ${this.showAll ? 'Show less' : `Show ${hiddenCountInGroup} more`}
                  <iconify-icon icon="${this.showAll ? 'mdi:chevron-up' : 'mdi:chevron-down'}" height="14px"></iconify-icon>
                </button>
              </div>
            ` : nothing}
          ` : nothing}
        </div>
      </div>
    `;
  }

  private _renderOta() {
    return html`
      <div class="sub-group-header">
        <span class="sub-group-header__label">Manual Update</span>
      </div>
      <div class="ota-section">
        <form class="ota-form" method="POST" action="${this._basePath}/update" enctype="multipart/form-data">
          <label class="ota-upload-label">
            <input
              class="ota-file-input"
              type="file"
              name="update"
              accept="application/octet-stream"
              @change="${(e: Event) => {
                const files = (e.target as HTMLInputElement).files;
                this.otaFilename = files?.[0]?.name ?? '';
              }}"
            />
            <iconify-icon icon="${this.otaFilename ? 'mdi:file-check-outline' : 'mdi:upload'}" height="20px"></iconify-icon>
            <span>${this.otaFilename || 'Choose firmware file (.bin)'}</span>
          </label>
          <button type="submit" class="ota-submit-btn" ?disabled="${!this.otaFilename}">
            <iconify-icon icon="mdi:flash" height="15px"></iconify-icon>
            Install Update
          </button>
        </form>
      </div>
    `;
  }

  private _renderGroupDescription(groupName: string, groupEntities: entityConfig[] = []) {
    if (this.show_setup && groupName.toLowerCase().includes('setup')) {
      return html`<div class="description-row">
        <div><iconify-icon icon="mdi:clipboard-list" height="24px"></iconify-icon></div>
        <div>
          The Setup Mode is currently active, please follow the steps below.<br><br>
          <b>Note:</b> Some steps may take up to 30 seconds.
        </div>
      </div>`;
    }
    if (!this.show_setup && groupName.toLowerCase().includes('setup')) {
      return html`<div class="description-row">
        <div><iconify-icon icon="mdi:file-link" height="24px"></iconify-icon></div>
        <div>
          Learn more about the interactive setup process <a target="_blank" href="https://doorman.azon.ai/guide/getting-started#interactive-setup">in the guide</a>.<br>
          Enabling Setup Mode will <u>erase all previously stored setup data</u>.
        </div>
      </div>`;
    }
    if (groupName.toLowerCase().includes('homekit')) {
      return html`<div class="description-row">
        <div><iconify-icon icon="mdi:file-link" height="24px"></iconify-icon></div>
        <div>
          Learn more about integrating with Apple HomeKit <a target="_blank" href="https://doorman.azon.ai/guide/firmware/homekit">in the guide</a>.
        </div>
      </div>`;
    }
    if (groupName.toLowerCase().includes('mqtt')) {
      return html`<div class="description-row">
        <div><iconify-icon icon="mdi:file-link" height="24px"></iconify-icon></div>
        <div>
          Learn more about integrating with MQTT <a target="_blank" href="https://doorman.azon.ai/guide/firmware/mqtt">in the guide</a>.
        </div>
      </div>`;
    }
    const hasUpdate = groupEntities.some((e) => e.domain === 'update')
      || groupName.toLowerCase().includes('firmware')
      || groupName.toLowerCase().includes('update');
    if (hasUpdate) {
      return html`<div class="description-row">
        <div><iconify-icon icon="mdi:update" height="24px"></iconify-icon></div>
        <div>
          Learn more about Doorman firmware updates <a target="_blank" href="https://doorman.azon.ai/guide/firmware/installation">in the guide</a>.
        </div>
      </div>`;
    }
    return nothing;
  }

  formatComponentName(component: entityConfig, _groupName: string, _index: number, sgPrefix?: string): string {
    const colonIdx = component.name.indexOf(': ');
    let displayName = colonIdx >= 0 ? component.name.slice(colonIdx + 2) : component.name;
    if (sgPrefix && displayName.startsWith(sgPrefix + ' - ')) {
      displayName = displayName.slice(sgPrefix.length + 3);
    }
    return displayName;
  }

  static get styles() {
    return [cssReset, cssButton, cssInput, cssTab, cssEntityTable];
  }

  _handleEntityRowClick(e: any) {
    if (e?.currentTarget?.domain === "sensor") {
      if (!e?.ctrlKey) e.stopPropagation();
      e?.currentTarget?.classList.toggle(
        "expanded",
        !e.ctrlKey ? undefined : true
      );
    }
  }
}


type ActionRendererNonCallable = "entity" | "actioner" | "exec";
type ActionRendererMethodKey = keyof Omit<
  ActionRenderer,
  ActionRendererNonCallable
>;

class ActionRenderer {
  public entity?: entityConfig;
  public actioner?: RestAction;

  exec(method: ActionRendererMethodKey) {
    if (!this[method] || typeof this[method] !== "function") {
      console.log(`ActionRenderer.${method} is not callable`);
      return;
    }
    return this[method]();
  }

  private _actionButton(entity: entityConfig, label: string, action: string, isCurrentState: boolean = false) {
    if (!entity) return;
    let a = action || label.toLowerCase();
    return html`<button
      class="${isCurrentState ? 'abuttonIsState' : 'abutton'}"
      ?disabled=${isCurrentState}
      @click=${() => this.actioner?.restAction(entity, a)}
    >
      ${label}
    </button>`;
  }

  private _datetime(
    entity: entityConfig,
    type: string,
    action: string,
    opt: string,
    value: string,
  ) {
    return html`
      <input 
        type="${type}" 
        name="${entity.unique_id}"
        id="${entity.unique_id}"
        .value="${value}"
        @change="${(e: Event) => {
          const val = (<HTMLTextAreaElement>e.target)?.value;
          this.actioner?.restAction(
            entity,
            `${action}?${opt}=${val.replace('T', ' ')}`
          );
        }}"
      />
    `;
  }

  private _switch(entity: entityConfig) {
    return html`<esp-switch
      color="var(--primary-color,currentColor)"
      .state=${entity.state}
      @state="${(e: CustomEvent) => {
        let act = "turn_" + e.detail.state;
        this.actioner?.restAction(entity, act.toLowerCase());
      }}"
    ></esp-switch>`;
  }

  private _select(
    entity: entityConfig,
    action: string,
    opt: string,
    options: string[] | number[],
    val: string | number | undefined
  ) {
    return html`<select
      @change="${(e: Event) => {
        const val = (<HTMLTextAreaElement>e.target)?.value;
        this.actioner?.restAction(
          entity,
          `${action}?${opt}=${encodeURIComponent(val)}`
        );
      }}"
    >
      ${options.map(
        (option) =>
          html`
            <option value="${option}" ?selected="${option == val}">
              ${option}
            </option>
          `
      )}
    </select>`;
  }

  private _range(
    entity: entityConfig,
    action: string,
    opt: string,
    value: string | number,
    min?: string | undefined,
    max?: string | undefined,
    step = 1
  ) {
    if(entity.mode == 1) {
      return html`<div class="range">
        <input
          type="number"
          name="${entity.unique_id}"
          id="${entity.unique_id}"
          step="${step}"
          min="${min || Math.min(0, value as number)}"
          max="${max || Math.max(10, value as number)}"
          .value="${value}"
          @change="${(e: Event) => {
            const val = (<HTMLTextAreaElement>e.target)?.value;
            this.actioner?.restAction(entity, `${action}?${opt}=${val}`);
          }}"
        />
        <div class="range-bounds">
          <span>${min ?? 0}</span>
          <span>${max ?? "–"}</span>
        </div>
      </div>`;      
    } else {
      return html`    
      <esp-range-slider
        name="${entity.unique_id}"
        step="${step}"
        min="${min}"
        max="${max}"
        .value="${value}"
        @state="${(e: CustomEvent) => {
            const val = (<HTMLTextAreaElement>e.target)?.value;
            this.actioner?.restAction(entity, `${action}?${opt}=${e.detail.state}`);
          }}"
      ></esp-range-slider>`;
    }

  }

  private _textinput(
    entity: entityConfig,
    action: string,
    opt: string,
    value: string | number,
    min: number | undefined,
    max: number | undefined,
    pattern: string | undefined
  ) {
    return html`
      <input
        type="${entity.mode == 1 ? "password" : "text"}"
        name="${entity.unique_id}"
        id="${entity.unique_id}"
        minlength="${min || Math.min(0, value as number)}"
        maxlength="${max || Math.max(255, value as number)}"
        pattern="${pattern || ""}"
        .value="${value!}"
        @change="${(e: Event) => {
          const val = (<HTMLTextAreaElement>e.target)?.value;
          this.actioner?.restAction(
            entity,
            `${action}?${opt}=${encodeURIComponent(val)}`
          );
        }}"
      />
    `;
  }

  private _colorpicker(entity: entityConfig, action: string, value: any) {
    function u16tohex(d: number) {
      return Number(d).toString(16).padStart(2, "0");
    }
    function rgb_to_str(rgbhex: string) {
      const rgb = rgbhex
        .match(/[0-9a-f]{2}/gi)
        ?.map((x) => parseInt(x, 16)) || [0, 0, 0];
      return `r=${rgb[0]}&g=${rgb[1]}&b=${rgb[2]}`;
    }

    return html`<div class="colorpicker">
      <input
        type="color"
        name="${entity.unique_id}"
        id="${entity.unique_id}"
        value="#${u16tohex(value?.r)}${u16tohex(value?.g)}${u16tohex(value?.b)}"
        @change="${(e: Event) => {
          const val = (<HTMLTextAreaElement>e.target)?.value;
          this.actioner?.restAction(entity, `${action}?${rgb_to_str(val)}`);
        }}"
      />
    </div>`;
  }

  render_binary_sensor() {
    if (!this.entity) return;
    const isOn = this.entity.state === stateOn;
    return html`
      <div class="binary-sensor-badge ${isOn ? "binary-sensor-badge--on" : "binary-sensor-badge--off"}">
        <span class="binary-sensor-dot"></span>
        <span>${this.entity.state ?? "—"}</span>
      </div>
    `;
  }

  render_date() {
    if (!this.entity) return;
    return html`
      ${this._datetime(
        this.entity,
        "date",
        "set",
        "value",
        this.entity.value,
      )}
    `;
  }

  render_time() {
    if (!this.entity) return;
    return html`
      ${this._datetime(
        this.entity,
        "time",
        "set",
        "value",
        this.entity.value,
      )}
    `;
  }

  render_datetime() {
    if (!this.entity) return;
    return html`
      ${this._datetime(
        this.entity,
        "datetime-local",
        "set",
        "value",
        this.entity.value,
      )}
    `;
  }

  render_switch() {
    if (!this.entity) return;
    if (this.entity.assumed_state)
      return html`
        ${this._actionButton(this.entity, "Off", "turn_off", this.entity.state === stateOff)}
        ${this._actionButton(this.entity, "On", "turn_on", this.entity.state === stateOn)}
      `;
    else return this._switch(this.entity);
  }

  render_fan() {
    if (!this.entity) return;
    return [
      this.entity.speed,
      " ",
      this.entity.speed_level,
      this._switch(this.entity),
      this.entity.speed_count
        ? this._range(
            this.entity,
            `turn_${this.entity.state.toLowerCase()}`,
            "speed_level",
            this.entity.speed_level ? this.entity.speed_level : 0,
            0,
            this.entity.speed_count,
            1
          )
        : "",
    ];
  }

  render_light() {
    if (!this.entity) return;
    return [
      html`<div class="entity">
        ${this._switch(this.entity)}
        ${this.entity.brightness
          ? this._range(
              this.entity,
              "turn_on",
              "brightness",
              this.entity.brightness,
              0,
              255,
              1
            )
          : ""}
        ${this.entity.color_mode === "rgb" || this.entity.color_mode === "rgbw"
          ? this._colorpicker(this.entity, "turn_on", this.entity?.color)
          : ""}
        ${this.entity.effects?.filter((v) => v != "None").length
          ? this._select(
              this.entity,
              "turn_on",
              "effect",
              this.entity.effects || [],
              this.entity.effect
            )
          : ""}
      </div> `,
    ];
  }

  render_lock() {
    if (!this.entity) return;
    const entity = this.entity;
    const actioner = this.actioner;
    const isLocked = entity.state === "LOCKED";
    const isUnlocked = entity.state === "UNLOCKED";
    return html`
      <div class="lock-control">
        <div class="lock-toggle" data-state="${isLocked ? "locked" : isUnlocked ? "unlocked" : ""}">
          <div class="lock-thumb"></div>
          <button
            class="lock-toggle-btn ${isLocked ? "lock-toggle-btn--active" : ""}"
            ?disabled="${isLocked}"
            title="Lock"
            @click="${() => actioner?.restAction(entity, "lock")}"
          ><iconify-icon icon="${isLocked ? "mdi:lock" : "mdi:lock-outline"}" height="16px"></iconify-icon></button>
          <button
            class="lock-toggle-btn ${isUnlocked ? "lock-toggle-btn--active" : ""}"
            ?disabled="${isUnlocked}"
            title="Unlock"
            @click="${() => actioner?.restAction(entity, "unlock")}"
          ><iconify-icon icon="mdi:lock-open-variant" height="16px"></iconify-icon></button>
        </div>
        <button
          class="lock-open-btn"
          title="Open"
          @click="${() => actioner?.restAction(entity, "open")}"
        ><iconify-icon icon="mdi:door-open" height="17px"></iconify-icon></button>
      </div>
    `;
  }

  render_cover() {
    if (!this.entity) return;
    const entity = this.entity;
    const actioner = this.actioner;
    const isOpen = entity.state === "OPEN";
    const isClosed = entity.state === "CLOSED";
    return html`
      <div class="cover-controls">
        <button
          class="cover-btn ${isOpen ? "cover-btn--active" : ""}"
          ?disabled="${isOpen}"
          title="Open"
          @click="${() => actioner?.restAction(entity, "open")}"
        ><iconify-icon icon="mdi:chevron-up" height="18px"></iconify-icon></button>
        <button
          class="cover-btn"
          title="Stop"
          @click="${() => actioner?.restAction(entity, "stop")}"
        ><iconify-icon icon="mdi:stop" height="15px"></iconify-icon></button>
        <button
          class="cover-btn ${isClosed ? "cover-btn--active" : ""}"
          ?disabled="${isClosed}"
          title="Close"
          @click="${() => actioner?.restAction(entity, "close")}"
        ><iconify-icon icon="mdi:chevron-down" height="18px"></iconify-icon></button>
      </div>
    `;
  }

  render_button() {
    if (!this.entity) return;
    const entity = this.entity;
    const actioner = this.actioner;
    return html`
      <button class="press-btn" @click="${() => actioner?.restAction(entity, "press")}">
        Press
      </button>
    `;
  }

  render_select() {
    if (!this.entity) return;
    return this._select(
      this.entity,
      "set",
      "option",
      this.entity.option || [],
      this.entity.value
    );
  }

  render_number() {
    if (!this.entity) return;
    return html`
      ${this._range(
        this.entity,
        "set",
        "value",
        this.entity.value,
        this.entity.min_value,
        this.entity.max_value,
        this.entity.step
      )}
      ${this.entity.uom}
    `;
  }

  render_text() {
    if (!this.entity) return;
    return this._textinput(
      this.entity,
      "set",
      "value",
      this.entity.value,
      this.entity.min_length,
      this.entity.max_length,
      this.entity.pattern
    );
  }

  render_climate() {
    if (!this.entity) return;
    let target_temp_slider, target_temp_label, target_temp;
    let current_temp = html`<div class="climate-row" style="padding-bottom: 10px";>
                              <label>Current:&nbsp;${this.entity.current_temperature} °C</label>
                            </div>`;
    
    if (
      this.entity.target_temperature_low !== undefined &&
      this.entity.target_temperature_high !== undefined
    ) {
      target_temp = html`
        <div class="climate-row">
          <label>Target Low:&nbsp;</label>
          ${this._range(
            this.entity,
            "set",
            "target_temperature_low",
            this.entity.target_temperature_low,
            this.entity.min_temp,
            this.entity.max_temp,
            this.entity.step
          )}
        </div>
        <div class="climate-row">
          <label>Target High:&nbsp;</label>
          ${this._range(
            this.entity,
            "set",
            "target_temperature_high",
            this.entity.target_temperature_high,
            this.entity.min_temp,
            this.entity.max_temp,
            this.entity.step
          )}
        </div>`;
    } else {
      target_temp = html`
        <div class="climate-row">
          <label>Target:&nbsp;</label>
          ${this._range(
            this.entity,
            "set",
            "target_temperature",
            this.entity.target_temperature!!,
            this.entity.min_temp,
            this.entity.max_temp,
            this.entity.step
          )}
        </div>`;
    }
    let modes = html``;
    if ((this.entity.modes ? this.entity.modes.length : 0) > 0) {
      modes = html`
        <div class="climate-row">
          <label>Mode:&nbsp;</label>
          ${this._select(
            this.entity,
            "set",
            "mode",
            this.entity.modes || [],
            this.entity.mode || ""
          )}
        </div>`;
    }
    return html`
      <div class="climate-wrap">
        ${current_temp} ${target_temp} ${modes}
      </div>
    `;
  }
  render_valve() {
    if (!this.entity) return;
    return html`${this._actionButton(this.entity, "OPEN", "open", this.entity.state === "OPEN")}
    ${this._actionButton(this.entity, "☐", "stop")}
    ${this._actionButton(this.entity, "CLOSE", "close", this.entity.state === "CLOSED")}`;
  }
  render_water_heater() {
    if (!this.entity) return;

    let current_temp = this.entity.current_temperature !== undefined
      ? html`<div class="climate-row" style="padding-bottom: 10px">
               <label>Current:&nbsp;${this.entity.current_temperature} °C</label>
             </div>`
      : nothing;

    let target_temp;
    if (
      this.entity.target_temperature_low !== undefined &&
      this.entity.target_temperature_high !== undefined
    ) {
      target_temp = html`
        <div class="climate-row">
          <label>Target Low:&nbsp;</label>
          ${this._range(
            this.entity,
            "set",
            "target_temperature_low",
            this.entity.target_temperature_low,
            this.entity.min_temp,
            this.entity.max_temp,
            this.entity.step
          )}
        </div>
        <div class="climate-row">
          <label>Target High:&nbsp;</label>
          ${this._range(
            this.entity,
            "set",
            "target_temperature_high",
            this.entity.target_temperature_high,
            this.entity.min_temp,
            this.entity.max_temp,
            this.entity.step
          )}
        </div>`;
    } else if (this.entity.target_temperature !== undefined) {
      target_temp = html`
        <div class="climate-row">
          <label>Target:&nbsp;</label>
          ${this._range(
            this.entity,
            "set",
            "target_temperature",
            this.entity.target_temperature,
            this.entity.min_temp,
            this.entity.max_temp,
            this.entity.step
          )}
        </div>`;
    } else {
      target_temp = nothing;
    }

    let modes = (this.entity.modes?.length ?? 0) > 0
      ? html`
          <div class="climate-row">
            <label>Mode:&nbsp;</label>
            ${this._select(
              this.entity,
              "set",
              "mode",
              this.entity.modes || [],
              this.entity.state || ""
            )}
          </div>`
      : nothing;

    let away = this.entity.away !== undefined
      ? html`
          <div class="climate-row">
            <label>Away:&nbsp;</label>
            ${this._actionButton(
              this.entity,
              this.entity.away ? "ON" : "OFF",
              `set?away=${!this.entity.away}`,
              false
            )}
          </div>`
      : nothing;

    let on_off = this.entity.is_on !== undefined
      ? html`
          <div class="climate-row">
            <label>Power:&nbsp;</label>
            ${this._actionButton(
              this.entity,
              this.entity.is_on ? "ON" : "OFF",
              `set?is_on=${!this.entity.is_on}`,
              false
            )}
          </div>`
      : nothing;

    return html`
      <div class="climate-wrap">
        ${current_temp} ${target_temp} ${modes} ${away} ${on_off}
      </div>
    `;
  }
  render_infrared() {
    if (!this.entity) return;

    if (this.entity.supports_transmitter !== true) {
      return nothing;
    }

    const entity = this.entity;

    const encodeTimings = (timingsStr: string): string => {
      const timings = timingsStr.split(',').map(s => parseInt(s.trim(), 10)).filter(n => !isNaN(n));
      const buffer = new ArrayBuffer(timings.length * 4);
      const view = new DataView(buffer);
      timings.forEach((val, i) => view.setInt32(i * 4, val, true));
      const bytes = new Uint8Array(buffer);
      let binary = '';
      bytes.forEach(b => binary += String.fromCharCode(b));
      return btoa(binary).replace(/\+/g, '-').replace(/\//g, '_').replace(/=+$/, '');
    };

    const handleTransmit = (e: Event) => {
      const button = e.currentTarget as HTMLElement;
      const container = button.parentElement?.parentElement;
      if (!container) {
        console.error('Infrared: Could not find container');
        return;
      }

      const carrierInput = container.querySelector('input[data-field="carrier"]') as HTMLInputElement;
      const repeatInput = container.querySelector('input[data-field="repeat"]') as HTMLInputElement;
      const timingsInput = container.querySelector('input[data-field="timings"]') as HTMLInputElement;

      if (!carrierInput || !repeatInput || !timingsInput) {
        console.error('Infrared: Could not find input elements', { carrierInput, repeatInput, timingsInput });
        return;
      }

      const carrier = carrierInput.value || '38000';
      const repeat = repeatInput.value || '1';
      const timingsRaw = timingsInput.value || '';

      if (!timingsRaw.trim()) {
        console.warn('Infrared: No timings provided');
        return;
      }

      const timingsEncoded = encodeTimings(timingsRaw);
      const basePath = getBasePath();
      const url = buildEntityActionUrl(basePath, entity, 'transmit');

      const body = new URLSearchParams();
      body.append('carrier_frequency', carrier);
      body.append('repeat_count', repeat);
      body.append('data', timingsEncoded);

      fetch(url, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/x-www-form-urlencoded'
        },
        body: body.toString()
      }).then(r => {
        console.log('Infrared: Transmit response', r);
      }).catch(err => {
        console.error('Infrared: Transmit error', err);
      });
    };

    return html`
      <div class="infrared-wrap">
        <div class="infrared-row">
          <label>Carrier (Hz):&nbsp;</label>
          <input
            type="number"
            data-field="carrier"
            value="38000"
            min="1000"
            max="100000"
            style="width: 80px"
          />
        </div>
        <div class="infrared-row">
          <label>Repeat:&nbsp;</label>
          <input
            type="number"
            data-field="repeat"
            value="1"
            min="1"
            max="100"
            style="width: 50px"
          />
        </div>
        <div class="infrared-row">
          <label>Timings:&nbsp;</label>
          <input
            type="text"
            data-field="timings"
            placeholder="e.g. 9000,-4500,560,-560,..."
            style="width: 100%; min-width: 200px"
          />
        </div>
        <div class="infrared-row">
          <button class="abutton" @click=${handleTransmit}>TX</button>
        </div>
      </div>
    `;
  }
  render_update() {
    if (!this.entity) return;
    if(this.entity.state == "UPDATE AVAILABLE") {
      let version = this.entity.value;
      let type = "";

      if (version.includes("-")) {
        const [base, suffix] = version.split("-");
        type = "Experimental";
        version = `${base} (${suffix.split(".")[1]})`;
      }

      const label = `Install ${type ? type + " " : ""}${version}`;
      return html`${this._actionButton(this.entity, label, "install")}`;
    }
    return html`<div>${this.entity.state}</div>`;
  }
}