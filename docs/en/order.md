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
    const standardTrackingDetails = 'Choose this option if you are comfortable with not being able to track the shipment. Lost packages cannot be refunded or replaced.';

    const trackedTrackingDetails = 'Recommended for reliable delivery and shipment visibility. Lost packages may be eligible for investigation or claim.';

    return {
      formEndpoint: 'https://order.doorman.azon.ai',
      form: {
        name: '',
        email: '',
        street: '',
        zip: '',
        city: '',
        country: 'DE',
        address_extra: '',
        product: 'pcb',
        shipping_destination: 'de',
        shipping_method: 'standard',
        payment_option: 'paypal',
        message: '',
      },
      payment_options: [
        {
            key: 'paypal',
            name: 'PayPal',
            icon: IconLogosPaypal,
            details: 'Please select <code>Family & Friends</code> to avoid additional fees. If you choose another option, you\'ll need to cover the fees.',
        },
        {
            key: 'sepa',
            name: 'SEPA Bank Transfer',
            icon: IconSimpleIconsSepa,
            details: 'If PayPal isn\'t suitable due to fees or other reasons, SEPA bank transfer is also available.',
        },
      ],
      products: [
        {
            key: 'pcb',
            name: 'Doorman-S3',
            image: '/pcb.png',
            details: 'PCB only – ideal if you can mount it inside a wallbox or hidden enclosure.',
            price: 35
        },
        {
            key: 'bundle',
            name: 'Doorman-S3 Bundle',
            image: '/enclosure.png',
            details: 'Includes PCB and case – perfect when the device is installed in a visible spot.',
            price: 37.5
        }
      ],
      shipping_destinations: [
        {
            key: 'de',
            name: 'Germany',
            details: '',
            icon: IconTwemojiFlagGermany,
            options: [
                {
                    key: 'standard',
                    name: 'Standard Shipping',
                    icon: '',
                    details: standardTrackingDetails,
                    price: 1.80
                },
                {
                    key: 'tracking',
                    name: 'Tracked Shipping',
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
            key: 'ch',
            name: 'Switzerland',
            details: '',
            icon: IconTwemojiFlagSwitzerland,
            options: [
                {
                    key: 'standard',
                    name: 'Standard Shipping',
                    icon: '',
                    details: standardTrackingDetails,
                    price: 8.99
                },
                {
                    key: 'tracking',
                    name: 'Tracked Shipping',
                    icon: '',
                    details: trackedTrackingDetails,
                    price: 26.99
                }
            ],
            countries: [
                'CH'
            ],
            defaultCountry: 'CH'
        },
        {
            key: 'eu',
            name: 'European Union',
            details: '',
            icon: IconTwemojiFlagEuropeanUnion,
            options: [
                {
                    key: 'standard',
                    name: 'Standard Shipping',
                    icon: '',
                    details: standardTrackingDetails,
                    price: 6.49
                },
                {
                    key: 'tracking',
                    name: 'Tracked Shipping',
                    icon: '',
                    details: trackedTrackingDetails,
                    price: 14.49
                }
            ],
            countries: [
                'AT','BE','BG','HR','CY','CZ','DK','EE','FI','FR','GR','HU','IE','IT','LV','LT','LU','MT','NL','PL','PT','RO','SK','SI','SE','ES'
            ],
            defaultCountry: 'AT'
        }
      ],
      modalOpen: false,
      result_title: '',
      result_text: ''
    }
  },
  watch: {
    'form.shipping_destination'(new_value) {
        const destination = this.shipping_destinations.find(d => d.key === new_value);
        if (!destination) return [];
        this.form.country = destination.defaultCountry;
    },
  },
  computed: {
    available_shipping_options() {
        return this.shipping_destinations.find(dest => dest.key === this.form.shipping_destination)?.options || [];
    },
    countryOptions() {
        const destination = this.shipping_destinations.find(d => d.key === this.form.shipping_destination);
        if (!destination) return [];
        return allCountries.filter(c => destination.countries.includes(c.value));
    },
    total_price() {
        const product = this.products.find(p => p.key === this.form.product);
        const destination = this.shipping_destinations.find(d => d.key === this.form.shipping_destination);
        const shipping = destination?.options.find(o => o.key === this.form.shipping_method);

        // Calculate total
        const productPrice = product ? product.price : 0;
        const shippingPrice = shipping ? shipping.price : 0;

        return (productPrice + shippingPrice);
    },
  },
  methods: {
    async submit() {
      axios.post(this.formEndpoint, this.form)
      .then(response => {
        this.modalOpen = true;
        this.result_title = "Received!"
        this.result_text = "Thank you."
      })
      .catch(error => {
        this.modalOpen = true;
        if (error.request.status == 429) {
            this.result_title = "Slow down!"
            this.result_text = "You made too many requests, try again later."
        } else {
          this.result_title = "Sorry!";
          this.result_text = "Something went wrong. Please try again later.";
        }
      });
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
    display: flex
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
</style>

# Get your own Doorman

Interested in a ready-to-use solution? I offer fully assembled and tested Doorman-S3 units with the [Doorman Firmware](guide/firmware/installation) pre-installed — ready for seamless integration with Home Assistant.

To request one, simply fill out the form below.
**This is not a purchase or checkout** — it's just a non-binding inquiry to help me gauge interest and manage requests privately.

Once I receive your message, I'll get back to you as soon as possible.

<ContactModal :show="modalOpen" @close="modalOpen = false">
    <template #header>
        <h3>{{ result_title }}</h3>
    </template>
    <template #body>
        {{ result_text }}
    </template>
</ContactModal>

<form @submit.prevent="submit">
    <h5 class="firmware_title_row">What would you like to order?</h5>
    <div class="firmware_option_row" :class="{ half: products.length <= 2 }">
        <label class="firmware_option" v-for="product in products" :key="product.key">
            <input type="radio" class="reset_default" v-model="form.product" :value="product.key">
            <span class="checkmark">
                <div class="image" v-if="product.image">
                    <img :src="product.image" />
                </div>
                <div class="title">{{ product.name }} <Badge type="tip">{{ product.price.toFixed(2) }}€</Badge></div>
                <div class="details" v-html="product.details"></div>
            </span>
        </label>
    </div>
    <h5 class="firmware_title_row">Where do you live?</h5>
    <div class="firmware_option_row" :class="{ half: shipping_destinations.length <= 2 }">
        <label class="firmware_option" v-for="destination in shipping_destinations" :key="destination.key">
            <input type="radio" class="reset_default" v-model="form.shipping_destination" :value="destination.key">
            <span class="checkmark">
                <div class="icon" v-if="destination.icon"><component :is="destination.icon" /></div>
                <div class="title">{{ destination.name }}</div>
                <div class="details" v-html="destination.details"></div>
            </span>
        </label>
    </div>
    <h5 class="firmware_title_row">What's your shipping preference?</h5>
    <div class="firmware_option_row" :class="{ half: available_shipping_options.length <= 2 }">
        <label class="firmware_option" v-for="option in available_shipping_options" :key="option.key">
            <input type="radio" class="reset_default" v-model="form.shipping_method" :value="option.key">
            <span class="checkmark">
                <div class="icon" v-if="option.icon"><component :is="option.icon" /></div>
                <div class="title">{{ option.name }} <Badge type="tip">{{ option.price.toFixed(2) }}€</Badge></div>
                <div class="details" v-html="option.details"></div>
            </span>
        </label>
    </div>
    <h5 class="firmware_title_row">What's your address?</h5>
    <div class="form-element top name-container">
        <label for="name">Name</label>
        <input type="text" name="name" id="name" placeholder="Max Mustermann" v-model="form.name" required />
    </div>
    <div class="form-element top email-container">
        <label for="email">Email</label>
        <input type="email" name="email" id="email" placeholder="max@mustermann.net" v-model="form.email" required />
    </div>
    <div class="form-element">
        <label for="address_extra">Additional address information</label>
        <input type="text" name="address_extra" id="address_extra" placeholder="" v-model="form.address_extra" />
    </div>
    <div class="form-element">
        <label for="street">Street & House number</label>
        <input type="text" name="street" id="street" placeholder="Musterstrasse 1" v-model="form.street" required />
    </div>
    <div class="form-element top name-container">
        <label for="zip">Postal Code</label>
        <input type="text" name="zip" id="zip" placeholder="97080" v-model="form.zip" required />
    </div>
    <div class="form-element top email-container">
        <label for="city">Town/city</label>
        <input type="text" name="city" id="city" placeholder="Würzburg" v-model="form.city" required />
    </div>
    <div class="form-element">
        <label for="city">Country</label>
        <select name="country" class="form-control" id="country" v-model="form.country" required>
            <option v-for="country in countryOptions" :key="country.value" :value="country.value" :label="country.label">{{ country.label }}</option>
        </select>
    </div>
    <h5 class="firmware_title_row">Which payment method do you prefer?</h5>
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
    <h5 class="firmware_title_row">Any other questions?</h5>
    <div class="form-element">
        <textarea id="message" name="message" v-model="form.message" placeholder="Any special requirements or something else you want to ask?"></textarea>
    </div>
    <h5 class="firmware_title_row">Estimated total:</h5>
    <ul class="estimate-list">
        <li>
            <span>Product: </span>
            <span>
                {{
                    (products.find(p => p.key === form.product)?.name || '—')
                }}
                <Badge type="tip">
                    {{
                        products.find(p => p.key === form.product)?.price?.toFixed(2)
                    }}€
                </Badge>
            </span>
        </li>
        <li>
            <span>Shipping: </span>
            <span>
                {{
                    (available_shipping_options.find(o => o.key === form.shipping_method)?.name || '—')
                }} ({{
                    (shipping_destinations.find(o => o.key === form.shipping_destination)?.name || '—')
                }})
                <Badge type="tip">
                    {{
                        available_shipping_options.find(o => o.key === form.shipping_method)?.price?.toFixed(2)
                    }}€
                </Badge>
            </span>
        </li>
        <li>
            <span><strong>Total: </strong></span>
            <span>
                <Badge type="warning">{{ total_price.toFixed(2) }}€</Badge>
            </span>
        </li>
    </ul>
    <div class="warning custom-block" style="padding-top: 8px">
        This is a non-binding estimate including your selected product and shipping. You will receive a confirmation and payment instructions by email after submitting your request.
    </div>
    <div class="submit">
        <VPButton text="Submit Inquiry" />
    </div>
</form>