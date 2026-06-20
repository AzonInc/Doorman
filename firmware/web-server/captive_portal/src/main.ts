if (document.location.search === "?save") {
  (document.getElementById("saveinfo") as HTMLElement).style.display = "flex";
  (document.getElementById("docsinfo") as HTMLElement).style.display = "none";
  (document.getElementById("main") as HTMLElement).style.display = "none";
  (document.getElementById("nav") as HTMLElement).style.display = "none";
}

// Nav is-stuck background
const _nav = document.getElementById("nav");
let _checkStuck: () => void = () => {};
if (_nav) {
  const _threshold = Math.max(0, _nav.getBoundingClientRect().top + window.scrollY - 64);
  _checkStuck = () => _nav.classList.toggle("is-stuck", window.scrollY > _threshold);
  window.addEventListener("scroll", _checkStuck, { passive: true });
}

// Tab switching
document.querySelectorAll<HTMLElement>(".nav-item[data-tab]").forEach(btn => {
  btn.addEventListener("click", () => {
    const target = btn.dataset.tab!;
    document.querySelectorAll<HTMLElement>("[id^='tab-']").forEach(p => p.style.display = "none");
    const panel = document.getElementById(`tab-${target}`);
    if (panel) panel.style.display = "";
    document.querySelectorAll(".nav-item").forEach(b => b.classList.remove("active"));
    btn.classList.add("active");
    window.scrollTo({ top: 0, behavior: "instant" });
    _checkStuck();
  });
});

function pickNetwork(el: HTMLElement) {
  const ssid = el.querySelector(".net-name")?.textContent?.trim() ?? "";
  (document.getElementById("ssid") as HTMLInputElement).value = ssid;
  (document.getElementById("psk") as HTMLInputElement).focus();
  const netEl = document.getElementById("net");
  if (netEl) netEl.style.display = "none";
  const toggleBtn = document.getElementById("net-toggle");
  if (toggleBtn) toggleBtn.classList.remove("open");
}
(window as any).pickNetwork = pickNetwork;

function wifi(dBm: number) {
  const quality: number = Math.max(Math.min(2 * (dBm + 100), 100), 0) / 100.0;
  return svg(`<path d="m12.008 19.25-11.3-15c7-5 14-5 22.5 0z" fill="none" stroke="currentColor"/>
              <path d="m12.008 19.25-11.3-15c7-5 14-5 22.5 0z" transform="scale(${quality} ${quality})" transform-origin="12 18"/>`);
}
function svg(el: string) {
  return html([`<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24">${el}</svg>`]);
}
function lock(show: boolean) {
  return show
    ? svg(`<path d='M12 17a2 2 0 0 0 2-2 2 2 0 0 0-2-2 2 2 0 0 0-2 2 2 2 0 0 0 2
  2m6-9a2 2 0 0 1 2 2v10a2 2 0 0 1-2 2H6a2 2 0 0 1-2-2V10a2
   2 0 0 1 2-2h1V6a5 5 0 0 1 5-5 5 5 0 0 1 5
   5v2h1m-6-5a3 3 0 0 0-3 3v2h6V6a3 3 0 0 0-3-3z'/>`)
    : "";
}
function html(h: string[]) {
  return h.join("");
}

const otaFile = document.getElementById("ota-file") as HTMLInputElement | null;
const otaFilenameEl = document.getElementById("ota-filename");
const otaSubmit = document.getElementById("ota-submit") as HTMLButtonElement | null;
const otaIcon = document.getElementById("ota-icon");
const uploadSvg = `<path fill="currentColor" d="M9 16h6v-6h4l-7-7-7 7h4zm-4 2h14v2H5z"/>`;
const fileSvg = `<path fill="currentColor" d="M14 2H6c-1.1 0-2 .9-2 2v16c0 1.1.9 2 2 2h12c1.1 0 2-.9 2-2V8l-6-6zm4 18H6V4h7v5h5v11zm-8.5-5.5 1.77 1.77 3.73-4.27.5.5-4.23 5L9 12.5l.5-.5z"/>`;
if (otaFile) {
  otaFile.addEventListener("change", () => {
    const name = otaFile.files?.[0]?.name ?? "";
    if (otaFilenameEl) otaFilenameEl.textContent = name || "Choose firmware file (.bin)";
    if (otaSubmit) otaSubmit.disabled = !name;
    if (otaIcon) otaIcon.innerHTML = name ? fileSvg : uploadSvg;
  });
}

fetch("/config.json").then(function (response) {
  response.json().then(function (config: { name: string; aps: { rssi: number; ssid: string; lock: boolean }[] }) {
    document.title = config.name;
    const hostEl = document.getElementById("host");
    if (hostEl) hostEl.innerText = config.name;
    const aps = config.aps.slice(1);
    const result = aps.map(function (ap) {
      return `<div class="net-item" onclick="pickNetwork(this)">
          <div class="net-signal">${wifi(ap.rssi)}</div>
          <div class="net-name">${ap.ssid}</div>
          <div class="net-lock">${lock(ap.lock)}</div>
        </div>`;
    });
    const netEl = document.getElementById("net");
    if (netEl) netEl.innerHTML = html(result);
    const toggleBtn = document.getElementById("net-toggle") as HTMLButtonElement | null;
    if (toggleBtn && aps.length > 0) {
      toggleBtn.disabled = false;
      toggleBtn.addEventListener("click", () => {
        const open = netEl?.style.display !== "none";
        if (netEl) netEl.style.display = open ? "none" : "";
        toggleBtn.classList.toggle("open", !open);
      });
    }
    const iconEl = document.querySelector("link[rel~='icon']") as HTMLLinkElement | null;
    if (iconEl) iconEl.href = `data:image/svg+xml,${wifi(-65)}`;
  });
});
