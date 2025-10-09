<script setup lang="ts">
import type { DefaultTheme } from 'vitepress/theme'
import { VPButton } from 'vitepress/theme'

import IconFluentEmojiRocket from '~icons/fluent-emoji/rocket'
import IconNotoPackage from '~icons/noto/package'
import IconTwemojiFlagGermany from '~icons/twemoji/flag-germany'
import IconTwemojiFlagSwitzerland from '~icons/twemoji/flag-switzerland';
import IconTwemojiFlagEuropeanUnion from '~icons/twemoji/flag-european-union'
import IconLogosPaypal from '~icons/logos/paypal';
import IconSimpleIconsSepa from '~icons/simple-icons/sepa';
</script>

<script lang="ts">
import axios from 'axios'
import moment from 'moment';

const api = axios.create({
   withCredentials: true,
   baseURL: 'https://order.doorman.azon.ai'
})

const allCountries = [
    { value: 'AT', label: 'Austria' },
    { value: 'BE', label: 'Belgium' },
    { value: 'BG', label: 'Bulgaria' },
    { value: 'HR', label: 'Croatia' },
    { value: 'CY', label: 'Cyprus' },
    { value: 'CZ', label: 'Czech Republic' },
    { value: 'DK', label: 'Denmark' },
    { value: 'EE', label: 'Estonia' },
    { value: 'FI', label: 'Finland' },
    { value: 'FR', label: 'France' },
    { value: 'DE', label: 'Germany' },
    { value: 'GR', label: 'Greece' },
    { value: 'HU', label: 'Hungary' },
    { value: 'IE', label: 'Ireland' },
    { value: 'IT', label: 'Italy' },
    { value: 'LV', label: 'Latvia' },
    { value: 'LT', label: 'Lithuania' },
    { value: 'LU', label: 'Luxembourg' },
    { value: 'MT', label: 'Malta' },
    { value: 'NL', label: 'Netherlands' },
    { value: 'PL', label: 'Poland' },
    { value: 'PT', label: 'Portugal' },
    { value: 'RO', label: 'Romania' },
    { value: 'SK', label: 'Slovakia' },
    { value: 'SI', label: 'Slovenia' },
    { value: 'SE', label: 'Sweden' },
    { value: 'ES', label: 'Spain' },
    { value: 'CH', label: 'Switzerland' },
];

export default {
    data() {
        const standardTrackingDetails = 'Wähle diese Option, wenn du auf Sendungsverfolgung verzichten kannst. Verlorene Sendungen können nicht erstattet oder ersetzt werden.';
        const trackedTrackingDetails = 'Empfohlen für eine zuverlässige Lieferung mit Sendungsverfolgung. Verlorene Sendungen können untersucht oder reklamiert werden.';

        return {
            errors: {
                name: false,
                email: false,
                street: false,
                zip: false,
                city: false,
                country: false,
            },
            status: {},
            orderHash: '',
            form: {
                name: '',
                email: '',
                discord: '',
                street: '',
                zip: '',
                city: '',
                country: 'DE',
                address_extra: '',
                product: 'pcb',
                amount: 1,
                shipping_region: 'DE',
                shipping_method: 'standard',
                payment_option: 'paypal',
                message: '',
            },
            max_items: 4,
            payment_options: [
                {
                    key: 'paypal',
                    name: 'PayPal',
                    icon: IconLogosPaypal,
                    details: 'Bitte wähle bei PayPal <code>Freunde & Familie</code>, um zusätzliche Gebühren zu vermeiden. Wenn du eine andere Option nutzt, musst du die Gebühren selbst tragen.',
                },
                {
                    key: 'sepa',
                    name: 'SEPA-Überweisung',
                    icon: IconSimpleIconsSepa,
                    details: 'Wenn PayPal für dich nicht geeignet ist (z. B. wegen Gebühren), kannst du auch per SEPA-Überweisung bezahlen.',
                },
            ],
            products: [
                {
                    key: 'pcb',
                    name: 'Doorman-S3',
                    image: '/pcb.png',
                    details: 'Nur die Platine – ideal, wenn du sie in eine bestehende Wandbox oder deine Innenstation einbauen kannst.',
                    price: 35
                },
                {
                    key: 'bundle',
                    name: 'Doorman-S3 Bundle',
                    image: '/enclosure.png',
                    details: 'Beinhaltet Platine und Gehäuse – perfekt, wenn du deinen Doorman sichtbar anbringen möchtest.',
                    price: 37.5
                }
            ],
            shipping_regions: [
                {
                    key: 'DE',
                    name: 'Deutschland',
                    details: '',
                    icon: IconTwemojiFlagGermany,
                    options: [
                        {
                            key: 'standard',
                            name: 'Standardversand',
                            icon: '',
                            details: standardTrackingDetails,
                            price: 1.8
                        },
                        {
                            key: 'tracking',
                            name: 'Sendungsverfolgung',
                            icon: '',
                            details: trackedTrackingDetails,
                            price: 6.19
                        }
                    ],
                    countries: [
                        'DE'
                    ],
                    defaultCountry: 'DE'
                },
                {
                    key: 'CH',
                    name: 'Schweiz',
                    details: '',
                    icon: IconTwemojiFlagSwitzerland,
                    options: [
                        {
                            key: 'standard',
                            name: 'Standardversand',
                            icon: '',
                            details: standardTrackingDetails,
                            price: 9
                        },
                        {
                            key: 'tracking',
                            name: 'Sendungsverfolgung',
                            icon: '',
                            details: trackedTrackingDetails,
                            price: 27
                        }
                    ],
                    countries: [
                        'CH'
                    ],
                    defaultCountry: 'CH'
                },
                {
                    key: 'EU',
                    name: 'Europäische Union',
                    details: '',
                    icon: IconTwemojiFlagEuropeanUnion,
                    options: [
                        {
                            key: 'standard',
                            name: 'Standardversand',
                            icon: '',
                            details: standardTrackingDetails,
                            price: 7
                        },
                        {
                            key: 'tracking',
                            name: 'Sendungsverfolgung',
                            icon: '',
                            details: trackedTrackingDetails,
                            price: 14.5
                        }
                    ],
                    countries: [
                        'AT','BE','BG','HR','CY','CZ','DK','EE','FI','FR','GR','HU','IE','IT','LV','LT','LU','MT','NL','PL','PT','RO','SK','SI','SE','ES'
                    ],
                    defaultCountry: 'AT'
                }
            ],
            step: 1,
            modalOpen: false,
            result_title: '',
            result_text: '',
            available_units: -1
        }
    },
    created() {
        api.get('/status', { 
            withCredentials: true 
        })
        .then(res => {
            this.status = res.data;
        })
        .catch(() => {
            this.status = { status: 'error' };
        });

        api.get('/product_data', { 
            withCredentials: true 
        })
        .then(res => {
            this.available_units = res.data.available_units;
            
            // merge into products
            if (res.data.products) {
                this.products = this.products.map(p => {
                    const override = res.data.products.find(x => { return x.key == p.key });
                    return override ? { ...p, price: override.price } : p;
                });
            }

            if (res.data.shipping_regions) {
                this.shipping_regions = this.shipping_regions.map(r => {
                    const override = res.data.shipping_regions.find(x => x.key === r.key);
                    if (!override) return r;

                    // merge options inside region
                    const mergedOptions = r.options.map(opt => {
                        const optOverride = override.options?.find(o => o.key === opt.key);
                        return optOverride ? { ...opt, ...optOverride } : opt;
                    });

                    return { ...r, ...override, options: mergedOptions };
                });
            }
        })
        .catch(() => {
            
        })
    },
    watch: {
        'form.shipping_region'(new_value) {
            const destination = this.shipping_regions.find(d => d.key === new_value);
            if (!destination) return [];
            this.form.country = destination.defaultCountry;
        },
        'form.name'(val) {
            this.errors.name = !val;
        },
        'form.email'(val) {
            // Simple email regex validation
            const emailRegex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
            this.errors.email = !emailRegex.test(val);
        },
        'form.street'(val) {
            this.errors.street = !val;
        },
        'form.zip'(val) {
            this.errors.zip = !val;
        },
        'form.city'(val) {
            this.errors.city = !val;
        },
        'form.country'(val) {
            this.errors.country = !val;
        }
    },
    computed: {
        availability_class() {
            if(this.available_units > 5) {
                return 'tip';
            } else if(this.available_units > 0) {
                return 'warning';
            }
            return 'danger';
        },
        availability_text() {
            if(this.available_units > 0) {
                return this.available_units + ' verfügbar';
            }
            return 'Derzeit nicht verfügbar';
        },
        available_shipping_options() {
            return this.shipping_regions.find(dest => dest.key === this.form.shipping_region)?.options || [];
        },
        countryOptions() {
            const destination = this.shipping_regions.find(d => d.key === this.form.shipping_region);
            if (!destination) return [];
            return allCountries.filter(c => destination.countries.includes(c.value));
        },
        total_price() {
            const product = this.products.find(p => p.key === this.form.product);
            const destination = this.shipping_regions.find(d => d.key === this.form.shipping_region);
            const shipping = destination?.options.find(o => o.key === this.form.shipping_method);

            // Calculate total
            const productPrice = (product ? product.price : 0) * this.form.amount;
            const shippingPrice = shipping ? shipping.price : 0;

            return (productPrice + shippingPrice);
        },
        last_update() {
            return moment.unix(this.status.updated_timestamp).format('DD.MM.YYYY');
        }
    },
    methods: {
        showModal(title, text) {
            this.modalOpen = true;
            this.result_title = title;
            this.result_text = text;
        },
        validate() {
            const { name, email, street, zip, city, country } = this.form;

            // Reset errors
            for (const key in this.errors) this.errors[key] = false;

            let valid = true;

            if (!name) { this.errors.name = true; valid = false; }
            if (!email) { this.errors.email = true; valid = false; }
            if (!street) { this.errors.street = true; valid = false; }
            if (!zip) { this.errors.zip = true; valid = false; }
            if (!city) { this.errors.city = true; valid = false; }
            if (!country) { this.errors.country = true; valid = false; }

            return valid;
        },
        nextStep() {
            if (this.validate()) {
                this.step++;
            } else {
                this.showModal("Entschuldigung!", "Bitte fülle alle erforderlichen Felder aus, um fortzufahren.");
            }
        },
        previousStep() {
            this.step--;
        },
        async submit() {
            api.post('/submit', this.form, { 
                withCredentials: true 
            })
            .then(response => {
                this.showModal("Erhalten!", "Vielen Dank. Ich melde mich so schnell wie möglich bei dir.");
                this.status = response.data;
            })
            .catch(error => {
                if (error.request.status == 429) {
                    this.showModal("Entschuldigung!", "Du hast zu viele Anfragen gesendet – bitte versuche es später erneut.");
                } else {
                    this.showModal("Entschuldigung!", "Etwas ist schiefgelaufen. Bitte versuche es später erneut.");
                }
            });
        },
        async closeOrder() {
            api.post('/close', {}, { 
                withCredentials: true 
            })
            .then(response => {
                this.status = response.data;
            })
            .catch(error => {
                if (error.request.status == 429) {
                    this.showModal("Entschuldigung!", "Du hast zu viele Anfragen gesendet – bitte versuche es später erneut.");
                } else {
                    this.showModal("Entschuldigung!", "Etwas ist schiefgelaufen. Bitte versuche es später erneut.");
                }
            });
        },
        async resetOrder() {
            api.post('/reset', {}, { 
                withCredentials: true 
            })
            .then(response => {
                this.status = response.data;
            })
            .catch(error => {
                if (error.request.status == 429) {
                    this.showModal("Entschuldigung!", "Du hast zu viele Anfragen gesendet – bitte versuche es später erneut.");
                } else {
                    this.showModal("Entschuldigung!", "Etwas ist schiefgelaufen. Bitte versuche es später erneut.");
                }
            });
        },
        async checkOrder() {
            if (!this.orderHash) return;

            try {
                await api.get(`/order/${this.orderHash}/details`, { withCredentials: true });

                const { data: statusData } = await api.get('/status', { withCredentials: true });
                this.status = statusData;

                if (this.status.status === 'none') {
                    this.showModal("Entschuldigung!", "Diese Bestellung existiert nicht! Bitte überprüfe die Bestellnummer.");
                }
            } catch (error) {
                let title = "Entschuldigung!";
                let text = "Etwas ist schiefgelaufen. Bitte versuche es später erneut.";

                if (error.response?.status === 429) {
                    title = "Langsamer!";
                    text = "Du hast zu viele Anfragen gesendet – bitte versuche es später erneut.";
                }

                this.showModal(title, text);
            }
        }
    }
}
</script>

<style scoped>
form {
    width: 100%
}

.form-element {
    margin-bottom: 1rem
}

.top {
    display: inline-block;
    width: 48%
}

.top label {
    display: block
}

.email-container {
    float: right
}

textarea,input[type=text],input[type=email],select {
    background-color: var(--vp-c-bg);
    width: 100%;
    font-size: 14px;
    border-radius: 8px;
    border: 1px solid var(--vp-c-divider);
    padding: 10px 12px;
    transition: color .25s,border-color .25s,background-color .25s;
    height: 40px;
    font-family: inherit;
    margin-top: .5rem
}

textarea:hover,input[type=text]:hover,input[type=email]:hover,select:hover {
    border: 1px solid var(--vp-button-brand-active-bg);
    transition: color .1s,border-color .1s,background-color .1s
}

textarea {
    height: 100px
}

.link-wrap {
    display: grid;
    grid-template-columns: 1fr 1fr
}

.submit {
    align-items: center;
    justify-content: center;
    display: flex;
    gap: 10px;
}

.estimate-list {
    list-style: none;
    padding: 0;
    margin: 1rem 0 1.5rem 0;
}
.estimate-list li {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: .5rem;
}

.invalid {
  border: 1px solid var(--vp-c-red-2) !important;
}

.amount-control {
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 8px;
  margin-top: 10px;
  padding-top: 15px;
  border-top: solid 1px var(--vp-c-default-soft);
}

.amount-control button {
    height: 25px !important;
    line-height: 25px !important;
}

.amount-control span {
  width: 64px;
  text-align: center;
  font-weight: 600;
  font-size: 14px;
}
</style>


# Hol dir deinen Doorman <Badge v-if="status.status !== 'error' && available_units >= 0" :type="availability_class" :text="availability_text"></Badge>

Du suchst eine einfache Lösung? Ich hab einen einsatzbereiten Doorman-S3 für dich, schon mit der [Doorman-Firmware](guide/firmware/installation) geflasht. Den kannst du direkt in dein Home Assistant Setup einbinden – ganz ohne Stress.

Um eine Anfrage zu stellen, fülle einfach das untenstehende Formular aus. **Dies ist kein Kauf oder Checkout** — nur eine unverbindliche Anfrage, damit ich das Interesse einschätzen und Bestellungen privat koordinieren kann.

Sobald ich deine Nachricht erhalte, melde ich mich schnellstmöglich bei dir.

<div v-if="status.status == 'none' && available_units === 0" class="warning custom-block">
    <p class="custom-block-title">ACHTUNG</p>
    <p>Alle Doormans sind gerade ausverkauft. Aber kein Grund zur Sorge - bald kommen neue!<br>Du kannst deine Anfrage trotzdem schonmal senden, dann reserviere ich dir einen.</p>
</div>

<hr />

<ContactModal :show="modalOpen" @close="modalOpen = false">
    <template #header>
        <h3>{{ result_title }}</h3>
    </template>
    <template #body>
        {{ result_text }}
    </template>
</ContactModal>

<div v-if="status.status == 'error'" class="danger custom-block">
    <p class="custom-block-title">SORRY</p>
    <p>Das Anfrageformular ist derzeit nicht verfügbar. Wahrscheinlich arbeite ich bereits an einer Lösung – bitte versuche es später erneut oder kontaktiere mich direkt in der Zwischenzeit.</p>
</div>
<div v-else-if="status.status == 'pending_review'" class="warning custom-block">
    <p class="custom-block-title">ÜBERPRÜFUNG AUSSTEHEND</p>
    <p>Ich habe deine Bestellung erhalten und werde diese mit dir prüfen, um die Kompatibilität abzuklären. Du wirst benachrichtigt, sobald die Überprüfung abgeschlossen ist.</p>
</div>
<div v-else-if="status.status == 'pending_payment'" class="warning custom-block">
    <p class="custom-block-title">ZAHLUNG AUSSTEHEND</p>
    <p>Deine Bestellung ist bereit für den nächsten Schritt. Bitte führe die Zahlung gemäß den bereitgestellten Anweisungen durch, damit ich mit der Bearbeitung des Versands fortfahren kann.</p>
</div>
<div v-else-if="status.status == 'pending_shipment'" class="tip custom-block">
    <p class="custom-block-title">VERSAND WIRD VORBEREITET</p>
    <p>
        Deine Bestellung wird für den Versand vorbereitet und wird bald versendet. Sobald sie verschickt wurde, erhälst du ein Update mit den Versanddetails. Vielen Dank für deine Geduld!
    </p>
</div>
<div v-else-if="status.status == 'shipped'" class="tip custom-block">
    <p class="custom-block-title">BESTELLUNG VERSCHICKT</p>
    <p>
        Gute Nachrichten! Deine Bestellung wurde am {{last_update}} versandt und sollte normalerweise innerhalb einer Woche eintreffen.
        <br>
        Bitte beachte, dass der Zoll gelegentlich zu leichten Verzögerungen führen kann.
        <br><br>Sobald die Lieferung erfolgt ist, informiere mich bitte hier. Vielen Dank!
        <br><br>
        <VPButton text="Ich habe meinen Doorman erhalten" @click="closeOrder" />
    </p>
</div>
<div v-else-if="status.status == 'closed'" class="tip custom-block">
    <p class="custom-block-title">DANKE</p>
    <p>
        Deine Bestellung wurde erfolgreich abgeschlossen. Ich hoffe, du hast viel Freude mit deinem Doorman 😊<br>
        Wenn du eine neue Bestellung aufgeben möchten, klicke bitte auf den Button unten.
        <br><br>
        <VPButton text="Neue Bestellung" @click="resetOrder" />
    </p>
</div>
<div v-else-if="status.status == 'cancelled'" class="danger custom-block">
    <p class="custom-block-title">BESTELLUNG ABGESAGT</p>
    <p>
        Deine Bestellung wurde storniert!<br>
        Wenn du eine neue Bestellung aufgeben möchten, klicke bitte auf den Button unten.
        <br><br>
        <VPButton text="Neue Bestellung" @click="resetOrder" />
    </p>
</div>
<form v-else-if="status.status == 'none'" @submit.prevent="submit">
    <div v-if="step == 1">
        <h5 class="firmware_title_row">Schon bestellt?</h5>
        <div style="display: flex; gap: 15px;justify-content: space-between; align-items: center;">
            <input type="text" name="order_hash" id="order_hash" maxlength="8" v-model="orderHash" placeholder="Bestellnummer" style="text-transform: uppercase; margin: 0px;" />
            <VPButton type="button" text="Status prüfen" @click="checkOrder" />
        </div>
        <br>
        <hr />
        <h5 class="firmware_title_row">Wähle dein Doorman-Paket</h5>
        <div class="firmware_option_row" :class="{ half: products.length <= 2 }">
            <label class="firmware_option" v-for="product in products" :key="product.key">
                <input type="radio" class="reset_default" v-model="form.product" :value="product.key">
                <span class="checkmark">
                    <div class="image" v-if="product.image">
                        <img :src="product.image" />
                    </div>
                    <div class="title">{{ product.name }} <Badge type="tip">{{ product.price.toFixed(2) }} €</Badge></div>
                    <div class="details" v-html="product.details"></div>
                    <div class="amount-control" v-if="form.product == product.key">
                        <VPButton theme="alt" type="button" text="-" @click="form.amount = Math.max(1, form.amount - 1)" />
                        <span class="font-semibold w-8 text-center">{{ form.amount }} {{ form.amount === 1 ? 'Stück' : 'Stücke' }}</span>
                        <VPButton theme="alt" type="button" text="+" @click="form.amount = Math.min(max_items, form.amount + 1)" />
                    </div>
                </span>
            </label>
        </div>
        <h5 class="firmware_title_row">Wohin soll's geschickt werden?</h5>
        <div class="firmware_option_row" :class="{ half: shipping_regions.length <= 2 }">
            <label class="firmware_option" v-for="destination in shipping_regions" :key="destination.key">
                <input type="radio" class="reset_default" v-model="form.shipping_region" :value="destination.key">
                <span class="checkmark">
                    <div class="icon" v-if="destination.icon"><component :is="destination.icon" /></div>
                    <div class="title">{{ destination.name }}</div>
                    <div class="details" v-html="destination.details"></div>
                </span>
            </label>
        </div>
        <h5 class="firmware_title_row">Wie soll's verschickt werden?</h5>
        <div class="firmware_option_row" :class="{ half: available_shipping_options.length <= 2 }">
            <label class="firmware_option" v-for="option in available_shipping_options" :key="option.key">
                <input type="radio" class="reset_default" v-model="form.shipping_method" :value="option.key">
                <span class="checkmark">
                    <div class="icon" v-if="option.icon"><component :is="option.icon" /></div>
                    <div class="title">{{ option.name }} <Badge type="tip">{{ option.price.toFixed(2) }} €</Badge></div>
                    <div class="details" v-html="option.details"></div>
                </span>
            </label>
        </div>
        <h5 class="firmware_title_row">Deine Lieferadresse</h5>
        <div class="form-element">   
            <label for="name">Name</label>
            <input type="text" name="name" id="name" placeholder="Max Mustermann" v-model="form.name" :class="{ 'invalid': errors.name }" required />
        </div>
        <div class="form-element">
            <label for="address_extra">Zusätzliche Adressinfos <Badge type="info">Optional</Badge></label>
            <input type="text" name="address_extra" id="address_extra" placeholder="" v-model="form.address_extra" />
        </div>
        <div class="form-element">
            <label for="street">Straße & Hausnummer</label>
            <input type="text" name="street" id="street" placeholder="Musterstraße 1" v-model="form.street" :class="{ 'invalid': errors.street }" required />
        </div>
        <div class="form-element top name-container">
            <label for="zip">Postleitzahl</label>
            <input type="text" name="zip" id="zip" placeholder="97080" v-model="form.zip" :class="{ 'invalid': errors.zip }" required />
        </div>
        <div class="form-element top email-container">
            <label for="city">Ort / Stadt</label>
            <input type="text" name="city" id="city" placeholder="Würzburg" v-model="form.city" :class="{ 'invalid': errors.city }" required />
        </div>
        <div class="form-element">
            <label for="city">Land</label>
            <select name="country" class="form-control" id="country" v-model="form.country" required>
                <option v-for="country in countryOptions" :key="country.value" :value="country.value" :label="country.label">{{ country.label }}</option>
            </select>
        </div>
        <h5 class="firmware_title_row">Wie kann ich dich erreichen?</h5>
        <div class="form-element top name-container">
            <label for="email">E-Mail</label>
            <input type="email" name="email" id="email" placeholder="max@mustermann.net" v-model="form.email" :class="{ 'invalid': errors.email }" required />
        </div>
        <div class="form-element top email-container">
            <label for="discord">Discord-Tag <Badge type="info">Optional</Badge></label>
            <input type="text" name="discord" id="discord" placeholder="azonflo" v-model="form.discord" />
        </div>
        <h5 class="firmware_title_row">Noch was, das du mir sagen willst?</h5>
        <div class="form-element">
            <label for="discord">Notizen <Badge type="info">Optional</Badge></label>
            <textarea id="message" name="message" v-model="form.message" placeholder="z.B. das Modell der Innenstation, besondere Anforderungen oder was du sonst fragen willst?"></textarea>
        </div>
        <div class="submit">
            <VPButton type="button" text="Weiter" @click="nextStep" />
        </div>
    </div>
    <div v-else-if="step == 2">
        <h5 class="firmware_title_row">Wie willst du bezahlen?</h5>
        <div class="firmware_option_row" :class="{ half: payment_options.length <= 2 }">
            <label class="firmware_option" v-for="option in payment_options" :key="option.key">
                <input type="radio" class="reset_default" v-model="form.payment_option" :value="option.key">
                <span class="checkmark">
                    <div class="icon" v-if="option.icon"><component :is="option.icon" /></div>
                    <div class="title">{{ option.name }}</div>
                    <div class="details" v-html="option.details"></div>
                </span>
            </label>
        </div>
        <h5 class="firmware_title_row">Bestellübersicht</h5>
        <ul class="estimate-list">
            <li>
                <span>
                    {{ form.amount }} &times; {{
                        (products.find(p => p.key === form.product)?.name || '—')
                    }}
                </span>
                <span>
                    <Badge type="tip">
                        {{
                            ((products.find(p => p.key === form.product)?.price || 0) * form.amount).toFixed(2)
                        }} €
                    </Badge>
                </span>
            </li>
            <li>
                <span>
                    1 &times; {{
                        (available_shipping_options.find(o => o.key === form.shipping_method)?.name || '—')
                    }} ({{
                        (shipping_regions.find(o => o.key === form.shipping_region)?.name || '—')
                    }})
                </span>
                <span>
                    <Badge type="tip">
                        {{
                            available_shipping_options.find(o => o.key === form.shipping_method)?.price?.toFixed(2)
                        }} €
                    </Badge>
                </span>
            </li>
            <hr>
            <li>
                <span><strong>Voraussichtlicher Gesamtpreis: </strong></span>
                <span>
                    <Badge type="warning">{{ total_price.toFixed(2) }} €</Badge>
                </span>
            </li>
        </ul>
        <div class="warning custom-block" style="padding-top: 8px">
            Das ist nur ein unverbindlicher Kostenvoranschlag für das ausgewählte Produkt und Versand. Nach Absenden deiner Anfrage bekommst du eine Bestätigung und Zahlungsinfos per E-Mail.
        </div>
        <div class="submit">
            <VPButton type="button" text="Zurück" @click="previousStep" />
            <VPButton type="submit" text="Anfrage absenden" />
        </div>
    </div>
</form>