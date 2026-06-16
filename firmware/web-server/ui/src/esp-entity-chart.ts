import { html, css, LitElement, TemplateResult, nothing } from "lit";
import { customElement, state, property } from "lit/decorators.js";
import {
  Chart,
  Colors,
  LineController,
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
} from "chart.js";

Chart.register(
  Colors,
  LineController,
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement
);

@customElement("esp-entity-chart")
export class ChartElement extends LitElement {
  @property({ type: Array }) chartdata = [];
  private chartSubComponent: Chart;

  constructor() {
    super();
  }

  updated(changedProperties: Map<string, unknown>) {
    super.updated(changedProperties);
    if (changedProperties.has("chartdata")) {
      this.chartSubComponent.data.datasets[0].data = this.chartdata;
      this.chartSubComponent.data.labels = this.chartdata;
      this.chartSubComponent?.update();
    }
  }

  firstUpdated() {
    const ctx = this.renderRoot.querySelector("canvas").getContext("2d");
    this.chartSubComponent = new Chart(ctx, {
      type: "line",
      data: {
        labels: this.chartdata,
        datasets: [
          {
            data: this.chartdata,
            borderColor: '#9269fe',
            backgroundColor: '#9269fe',
            borderWidth: 1,
            tension: 0.3,
          },
        ],
      },
      options: {
        animation: { duration: 0 },
        plugins: { legend: { display: false } },
        scales: { x: { display: false }, y: { position: "right" } },
        responsive: true,
        maintainAspectRatio: false,
      },
    });
    this.updateStylesIfExpanded();
  }
  // since the :host-context(.expanded) selector is not supported in Safari and Firefox we need to use JS to apply styles
  // whether the parent element is expanded or not
  updateStylesIfExpanded() {
    const parentElement = this.parentElement;
    const expandedClass = "expanded";

    const applyStyles = () => {
      if (parentElement && parentElement.classList.contains(expandedClass)) {
        this.style.height = "240px";
        this.style.opacity = "0.5";
      } else {
        this.style.height = "42px";
        this.style.opacity = "0.1";
      }
    };

    applyStyles();

    // Observe class changes
    const observer = new MutationObserver(applyStyles);
    if (parentElement)
      observer.observe(parentElement, {
        attributes: true,
        attributeFilter: ["class"],
      });
  }

  static get styles() {
    return css`
      :host {
        position: absolute;
        left: 24px;
        height: 42px;
        width: calc(100% - 42px);
        z-index: -100;
      }
    `;
  }

  render() {
    return html`<canvas></canvas>`;
  }
}
