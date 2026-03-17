---
layout: orderpage
description: Get a pre-assembled Doorman S3 with firmware, ready for seamless setup and use.
editLink: false
lastUpdated: false
---

<script setup lang="ts">
import type { DefaultTheme } from 'vitepress/theme'
import { VPButton } from 'vitepress/theme'

import dayjs from 'dayjs';
import relativeTime from 'dayjs/plugin/relativeTime';
dayjs.extend(relativeTime);
dayjs.locale('en');

import IconFluentEmojiRocket from '~icons/fluent-emoji/rocket'
import IconNotoPackage from '~icons/noto/package'
import IconTwemojiFlagGermany from '~icons/twemoji/flag-germany'
import IconTwemojiFlagSwitzerland from '~icons/twemoji/flag-switzerland';
import IconTwemojiFlagEuropeanUnion from '~icons/twemoji/flag-european-union'
import EmojioneV1GlobeShowingAmericas from '~icons/emojione-v1/globe-showing-americas';
import IconLogosPaypal from '~icons/logos/paypal';
import IconSimpleIconsSepa from '~icons/simple-icons/sepa';
</script>

<script lang="ts">
import axios from 'axios'

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
    { value: 'RS', label: 'Serbia' },
    { value: 'CN', label: 'China' },
    { value: 'IS', label: 'Iceland' },
];

export default {
    data() {
        const envelopeTrackingDetails = 'This option is similar to Parcel but intended for smaller, low-quantity orders.';
        const parcelTrackingDetails = 'Choose this option if you are comfortable with not being able to track the shipment.<br>Lost or damaged packages cannot be refunded or replaced.';
        const packageTrackingDetails = 'Recommended for reliable delivery and shipment visibility with <u>tracking</u>.<br>Lost or damaged packages may be eligible for investigation or claim.';

        return {
            errors: {
                fullname: false,
                name: false,
                email: false,
                street: false,
                zip: false,
                city: false,
                country: false,
                model: false,
            },
            user: null,
            status: {},
            orderHash: '',
            processing: false,
            form: {
                language: 'en',
                fullname: '',
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
                shipping_method: 'envelope',
                payment_option: 'paypal',
                message: '',
                model: ''
            },
            max_items: 4,
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
                    key: 'core_board',
                    name: 'Core Board',
                    image: '/base-board.png',
                    details: 'Core Board only. For mounting inside a wall box or indoor station enclosure.',
                    price: 37
                },
                {
                    key: 'audio_extension',
                    name: 'Audio Extension',
                    image: '/audio-extension.png',
                    details: 'Audio Extension only - upgrade your Doorman.<br><i>*Requires Core Board revision 2.0.0 or newer.</i>',
                    price: 8
                },
                {
                    key: 'enclosure',
                    name: 'Protective Enclosure',
                    image: '/enclosure-only.png',
                    details: 'The Enclosure for everything - perfect for visible installations. Replacement or spare.',
                    price: 4
                },
                {
                    key: 'core_board_enclosure',
                    name: 'Starter Pack',
                    image: '/enclosure-base-board.png',
                    details: 'Core Board and Enclosure. Ideal for visible and surface-mounted installations in your home.',
                    price: 39
                },
                {
                    key: 'core_board_audio_extension',
                    name: 'Audio Essentials Pack',
                    image: '/base-board-audio-extension.png',
                    details: 'Core Board and Audio Extension. For mounting inside a wall box or indoor station enclosure.',
                    price: 42
                },
                {
                    key: 'core_board_audio_extension_enclosure',
                    name: 'All inclusive Pack',
                    image: '/enclosure-audio-extension.png',
                    details: 'Core Board, Audio Extension, and Enclosure. This is everything you will ever need.',
                    price: 45
                }
            ],
            shipping_regions: [
                {
                    key: 'DE',
                    name: 'Germany',
                    details: 'Doorman is made in Germany, which allows for the most affordable shipping options.',
                    icon: IconTwemojiFlagGermany,
                    options: [
                        {
                            key: 'envelope',
                            name: 'Envelope',
                            icon: '',
                            details: envelopeTrackingDetails,
                            price: 0,
                            max_items: 0
                        },
                        {
                            key: 'standard',
                            name: 'Parcel',
                            icon: '',
                            details: parcelTrackingDetails,
                            price: 0,
                            max_items: 0
                        },
                        {
                            key: 'tracking',
                            name: 'Package',
                            icon: '',
                            details: packageTrackingDetails,
                            price: 0,
                            max_items: 0
                        }
                    ],
                    countries: [
                        'DE'
                    ],
                    defaultCountry: 'DE'
                },
                {
                    key: 'CH',
                    name: 'Switzerland',
                    details: 'Please note that customs may occasionally cause slight delays.',
                    icon: IconTwemojiFlagSwitzerland,
                    options: [
                        {
                            key: 'standard',
                            name: 'Parcel',
                            icon: '',
                            details: parcelTrackingDetails,
                            price: 0,
                            max_items: 0
                        },
                        {
                            key: 'tracking',
                            name: 'Package',
                            icon: '',
                            details: packageTrackingDetails,
                            price: 0,
                            max_items: 0
                        }
                    ],
                    countries: [
                        'CH'
                    ],
                    defaultCountry: 'CH'
                },
                {
                    key: 'EU',
                    name: 'European Union',
                    details: 'Delivery times may vary slightly depending on your location.',
                    icon: IconTwemojiFlagEuropeanUnion,
                    options: [
                        {
                            key: 'standard',
                            name: 'Parcel',
                            icon: '',
                            details: parcelTrackingDetails,
                            price: 0,
                            max_items: 0
                        },
                        {
                            key: 'tracking',
                            name: 'Package',
                            icon: '',
                            details: packageTrackingDetails,
                            price: 0,
                            max_items: 0
                        }
                    ],
                    countries: [
                        'AT','BE','BG','HR','CY','CZ','DK','EE','FI','FR','GR','HU','IE','IT','LV','LT','LU','MT','NL','PL','PT','RO','SK','SI','SE','ES'
                    ],
                    defaultCountry: 'AT'
                },
                {
                    key: 'INT',
                    name: 'Somewhere else',
                    details: 'If your country is not on the list, I would be happy to assist you - just get in touch.',
                    icon: EmojioneV1GlobeShowingAmericas,
                    options: [
                        {
                            key: 'standard',
                            name: 'Parcel',
                            icon: '',
                            details: parcelTrackingDetails,
                            price: 0,
                            max_items: 0
                        },
                        {
                            key: 'tracking',
                            name: 'Package',
                            icon: '',
                            details: packageTrackingDetails,
                            price: 0,
                            max_items: 0
                        }
                    ],
                    countries: [
                        'RS','CN','IS'
                    ],
                    defaultCountry: 'RS'
                }
            ],
            step: 1,
            modalOpen: false,
            result_title: '',
            result_text: '',
            available_units: -1,
            available_timestamp: 0
        }
    },
    created() {
        api.get('/order', { 
            withCredentials: true 
        })
        .then(res => {
            this.status = res.data;
        })
        .catch(() => {
            this.status = { status: 'error' };
        });

        api.get('/user', { 
            withCredentials: true 
        })
        .then(response => {
            this.user = response.data;
        })
        .catch(() => {
            this.user = null;
        });

        this.fetchProductData();
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
        'form.fullname'(val) {
            this.errors.fullname = !val;
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
        },
        'form.model'(val) {
            this.errors.model = !val;
        },
        'form.amount'(newAmount, oldAmount) {
            const shipping_region = this.shipping_regions.find(d => d.key === this.form.shipping_region);
            const shipping_option = shipping_region.options.find(d => d.key === this.form.shipping_method);

            if (shipping_option.max_items != 0 && newAmount > shipping_option.max_items) {
                this.form.shipping_method = 'standard';
            }
        },
        'form.shipping_region'(newRegion, oldRegion) {
            const shipping_region = this.shipping_regions.find(d => d.key === this.form.shipping_region);
            const shipping_option = shipping_region.options.find(d => d.key === this.form.shipping_method);

            if(!shipping_option) {
                this.form.shipping_method = 'standard';
            }
        },
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
                return this.available_units + ' available';
            }
            return 'Currently unavailable';
        },
        availability_time_text() {
            const now = dayjs().unix();
            if(this.available_timestamp > now) {
                return 'Don\'t worry - new Doormans are on their way and are expected to be available <b>' + dayjs.unix(this.available_timestamp).fromNow() + '</b>.';
            } else if(this.available_timestamp < now) {
                if(this.available_units == 0) {
                    return 'Looks like there is a delay - the new Doormans were expected <b>' + dayjs.unix(this.available_timestamp).fromNow() + '</b>.';
                }
            }
            return '';
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
            return dayjs.unix(this.status.updated_timestamp).fromNow();
        }
    },
    methods: {
        openManagement() {
            window.location = '/order-management';
        },
        showModal(title, text) {
            this.modalOpen = true;
            this.result_title = title;
            this.result_text = text;
        },
        validate() {
            const { fullname, name, email, street, zip, city, country, model } = this.form;

            // Reset errors
            for (const key in this.errors) this.errors[key] = false;

            let valid = true;

            if (!fullname) { this.errors.fullname = true; valid = false; }
            if (!name) { this.errors.name = true; valid = false; }
            if (!email) { this.errors.email = true; valid = false; }

            if (!street) { this.errors.street = true; valid = false; }
            if (street.split(' ').length < 2) { this.errors.street = true; valid = false; }
            if (/\d/.test(street) == false) { this.errors.street = true; valid = false; }

            if (!zip) { this.errors.zip = true; valid = false; }
            if (!city) { this.errors.city = true; valid = false; }
            if (!country) { this.errors.country = true; valid = false; }
            if (!model) { this.errors.model = true; valid = false; }

            return valid;
        },
        nextStep() {
            if (this.validate()) {
                this.step++;
            } else {
                this.showModal("Sorry!", "Please fill all the required fields to continue.");
            }
        },
        previousStep() {
            this.step--;
        },
        async fetchProductData() {
            try {
                const res = await api.get('/products', { withCredentials: true });

                this.available_units = res.data.available_units;
                this.available_timestamp = res.data.available_timestamp;

                // merge into products
                if (res.data.products) {
                    this.products = this.products.map(p => {
                        const override = res.data.products.find(x => x.key === p.key);
                        return override ? { ...p, price: override.price } : p;
                    });
                }

                // merge shipping regions
                if (res.data.shipping_regions) {
                    this.shipping_regions = this.shipping_regions.map(r => {
                        const override = res.data.shipping_regions.find(x => x.key === r.key);
                        if (!override) return r;

                        const mergedOptions = r.options.map(opt => {
                            const optOverride = override.options?.find(o => o.key === opt.key);
                            return optOverride ? { ...opt, ...optOverride, name: opt.name } : opt;
                        });

                        // destructure name out of override so it doesn't overwrite r.name
                        const { name: _ignored, ...restOverride } = override;

                        return { 
                            ...r, 
                            ...restOverride, 
                            options: mergedOptions 
                        };
                    });
                }
            } catch (err) {
                console.error('Failed to fetch product data:', err);
            }
        },
        async submit() {
            this.processing = true;

            api.post('/order', this.form, { 
                withCredentials: true 
            })
            .then(response => {
                this.processing = false;
                this.showModal("Received!", "Thank you. I will reach out to you as soon as possible.");
                this.status = response.data;

                this.fetchProductData();
            })
            .catch(error => {
                this.processing = false;
                this.showModal("Sorry!", this.getErrorMessage(error, 'Something went wrong. Please try again later.'));
            });
        },
        async closeOrder() {
            if (!confirm('Are you sure you want to close the order?')) return;
            api.post('/order/close', {}, { 
                withCredentials: true 
            })
            .then(response => {
                this.status = response.data;
            })
            .catch(error => {
                this.showModal("Sorry!", this.getErrorMessage(error, 'Something went wrong. Please try again later.'));
            });
        },
        async openTracking() {
            if(!this.status) {
                alert('Missing order data!');
                return;
            }

            window.open('https://www.dhl.de/en/privatkunden/dhl-sendungsverfolgung.html?piececode=' + this.status.tracking);
        },
        async updateOrderStatus() {
            if(!this.status) {
                alert('Missing order data!');
                return;
            }

            if (!confirm('Are you sure you want update the order status?')) return;

            if(this.status.shipping_method == 'tracking' && this.status.status == 'pending_shipment') {
                const trackingUrl = prompt('Provide the Tracking ID:');
                if (!trackingUrl) {
                    alert('No Tracking provided! Aborted.');
                    return;
                }
                this.setNextStep(trackingUrl);
            } else {
                this.setNextStep('');
            }
        },
        async setNextStep(id, tracking) {
            api.post('/order/next', { tracking }, { 
                withCredentials: true 
            })
            .then(response => {
                this.showModal("Updated!", "The order status was updated successfully.");
                this.status = response.data;
            })
            .catch(error => {
                this.showModal("Oops!", this.getErrorMessage(error, 'Failed to update order status!'));
            });
        },
        async resetOrder() {
            api.post('/order/reset', {}, { 
                withCredentials: true 
            })
            .then(response => {
                this.status = response.data;
            })
            .catch(error => {
                this.showModal("Oops!", this.getErrorMessage(error, 'Failed to reset order!'));
            });
        },
        async cancelOrder() {
            if (!confirm('Are you sure you want to cancel the order?')) return;
            api.post('/order/cancel', {}, { 
                withCredentials: true 
            })
            .then(response => {
                this.status = response.data;
            })
            .catch(error => {
                this.showModal("Oops!", this.getErrorMessage(error, 'Failed to cancel order!'));
            });
        },
        async checkOrder() {
            if (!this.orderHash) return;

            try {
                await api.get(`/order/${this.orderHash}/details`, { withCredentials: true });
                
                const { data: statusData } = await api.get('/order', { withCredentials: true });
                this.status = statusData;

                if (this.status.status === 'none') {
                    this.showModal("Sorry!", "This order does not exist! Please check the order number.");
                }
            } catch (error) {
                this.showModal("Sorry!", this.getErrorMessage(error, 'Something went wrong. Please try again later.'));
            }
        },
        getErrorMessage(error, defaultMessage = "Something went wrong.") {
            if (!error) return defaultMessage;

            if (error.response) {
                return error.response.data?.error || error.response.data?.message || `Server error (${error.response.status})`;
            } else if (error.request) {
                return "No response from server. Please check your connection.";
            } else {
                return error.message || defaultMessage;
            }
        },
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

# Get your own Doorman <Badge v-if="status.status !== 'error' && available_units >= 0" :type="availability_class" :text="availability_text" @click="openManagement" />

As part of this open-source, community-driven project, I occasionally make fully assembled Doorman S3 hardware available with the [Doorman Firmware](guide/firmware/installation) pre-installed — ready for seamless integration with Home Assistant.

:::tip PLEASE NOTE
This is **not an official, commercial or certified product**.  
Both the hardware and firmware are outcomes of reverse-engineering work and are offered without any **warranties concerning safety, reliability, or compatibility**.

This page does **not** constitute a shop, sale, or checkout process.  
Completing the form below simply registers your **non-binding interest** to help me understand demand and organize requests.
:::

**Each device acquired through me directly contributes to the continued development and improvement of the Doorman project.**

If you submit an inquiry, I may contact you if clarification is needed. Otherwise, you'll only hear from me if and when a unit becomes available.

Availability is limited and occurs **without a fixed schedule**. Any notifications are automated, so **please check your spam folder** if you don't hear back within a month.

<div v-if="status.status == 'none' && available_units === 0" class="danger custom-block">
    <p class="custom-block-title">CURRENTLY UNAVAILABLE</p>
    <p v-html="availability_time_text"></p>
    <p>You can send your inquiry anyway, and I'll make sure to reserve one for you as soon as they arrive.</p>
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
    <p>The inquiry form is currently down. I'm probably working to fix it already—please try again later, or contact me directly in the meantime.</p>
</div>
<div v-else-if="status.status == 'pending_review'" class="warning custom-block">
    <p class="custom-block-title">REVIEW PENDING</p>
    <p>I will now review your inquiry to confirm compatibility. You'll be notified as soon as the review is complete.</p>
    <p>
        <VPButton text="Cancel Inquiry" @click="cancelOrder" />
    </p>
</div>
<div v-else-if="status.status == 'reserved'" class="warning custom-block">
    <p class="custom-block-title">RESERVED</p>
    <p>Your Doorman(s) are reserved! You'll receive payment instructions as soon as everything is prepared.</p>
    <p>
        <VPButton text="Cancel Order" @click="cancelOrder" />
    </p>
</div>
<div v-else-if="status.status == 'pending_payment'" class="warning custom-block">
    <p class="custom-block-title">PAYMENT PENDING</p>
    <p>Your order is ready for the next step. Please complete the payment using the instructions provided so I can proceed with processing your shipment.</p>
</div>
<div v-else-if="status.status == 'pending_shipment'" class="tip custom-block">
    <p class="custom-block-title">PREPARING SHIPMENT</p>
    <p>
        Your order is being prepared for shipment and will be dispatched soon. Once it has been sent, you will receive an update with tracking details. Thank you for your patience!
    </p>
    <p v-if="user">
        <VPButton text="Mark as shipped (admin)" @click="updateOrderStatus" />
    </p>
</div>
<div v-else-if="status.status == 'shipped'" class="tip custom-block">
    <p class="custom-block-title">ORDER SHIPPED</p>
    <p>
        Good news! Your order was shipped {{last_update}} and should normally arrive within a week.
        <br>
        Please note that customs may occasionally cause slight delays.
        <br><br>Once it's delivered, kindly let me know here. Thank you!
    </p>
    <p v-if="user" style="display:flex;gap: 10px;">
        <VPButton v-if="status.tracking" text="Tracking" @click="openTracking" />
        <VPButton text="Close order (admin)" @click="closeOrder" />
    </p>
    <p v-else style="display:flex;gap: 10px;">
        <VPButton v-if="status.tracking" text="Tracking" @click="openTracking" />
        <VPButton text="I received my Doorman" @click="closeOrder" />
    </p>
</div>
<div v-else-if="status.status == 'closed'" class="tip custom-block">
    <p class="custom-block-title">THANK YOU</p>
    <p>
        Your order has been successfully completed. I hope you have fun with your Doorman 😊<br>
    </p>
    <p>
        <b>Make sure to check the <a href="/guide/getting-started">Quickstart Guide</a> to get started.</b>
    </p>
    <p>
        If you'd like to place a new order, please click the button below.
    </p>
    <p>
        <VPButton text="New Order" @click="resetOrder" />
    </p>
</div>
<div v-else-if="status.status == 'cancelled'" class="danger custom-block">
    <p class="custom-block-title">ORDER CANCELLED</p>
    <p>
        Your order has been cancelled!<br>
        If you'd like to place a new order, please click the button below.
    </p>
    <p>
        <VPButton text="New Order" @click="resetOrder" />
    </p>
</div>
<form v-else-if="status.status == 'none'" @submit.prevent="submit">
    <div v-if="step == 1">
        <h5 class="firmware_title_row">Already submitted a request?</h5>
        <div style="display: flex; gap: 15px;justify-content: space-between; align-items: center;">
            <input type="text" name="order_hash" id="order_hash" maxlength="8" v-model="orderHash" placeholder="Request Number #A1B2C3D4" style="text-transform: uppercase; margin: 0px;" />
            <VPButton type="button" text="Check Status" :disabled="orderHash.length != 8" @click="checkOrder" />
        </div>
        <br>
        <hr />
        <h5 class="firmware_title_row">Choose your Doorman package</h5>
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
                        <span class="font-semibold w-8 text-center">{{ form.amount }} {{ form.amount === 1 ? 'Unit' : 'Units' }}</span>
                        <VPButton theme="alt" type="button" text="+" @click="form.amount = Math.min(max_items, form.amount + 1)" />
                    </div>
                </span>
            </label>
        </div>
        <h5 class="firmware_title_row">Tell me something about your System</h5>
        <div class="form-element">
            <label for="model">Indoor Station Model</label>
            <input type="text" name="model" id="model" placeholder="e.g. TCS ISH3030" maxlength="30" v-model="form.model" :class="{ 'invalid': errors.model }" required />
        </div>
        <h5 class="firmware_title_row">Where should it be shipped?</h5>
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
        <h5 class="firmware_title_row">How should it be shipped?</h5>
        <div class="firmware_option_row" :class="{ half: available_shipping_options.length <= 2 }">
            <label class="firmware_option" v-for="option in available_shipping_options" :key="option.key">
                <input type="radio" class="reset_default" :disabled="option.max_items != 0 && form.amount > option.max_items" v-model="form.shipping_method" :value="option.key">
                <span class="checkmark">
                    <div class="icon" v-if="option.icon"><component :is="option.icon" /></div>
                    <div class="title">{{ option.name }} <Badge type="tip">{{ option.price.toFixed(2) }} €</Badge></div>
                    <div class="details" v-html="option.details"></div>
                </span>
            </label>
        </div>
        <h5 class="firmware_title_row">Your shipping address</h5>
        <div class="form-element">   
            <label for="name">Name</label>
            <input type="text" name="name" id="name" placeholder="Max Mustermann" v-model="form.name" :class="{ 'invalid': errors.name }" required />
        </div>
        <div class="form-element">
            <label for="address_extra">Additional address information <Badge type="info">Optional</Badge></label>
            <input type="text" name="address_extra" id="address_extra" placeholder="" v-model="form.address_extra" />
        </div>
        <div class="form-element">
            <label for="street">Street & House number</label>
            <input type="text" name="street" id="street" placeholder="Musterstrasse 1" v-model="form.street" :class="{ 'invalid': errors.street }" required />
        </div>
        <div class="form-element top name-container">
            <label for="zip">Postal Code</label>
            <input type="text" name="zip" id="zip" placeholder="97080" v-model="form.zip" :class="{ 'invalid': errors.zip }" required />
        </div>
        <div class="form-element top email-container">
            <label for="city">Town/city</label>
            <input type="text" name="city" id="city" placeholder="Würzburg" v-model="form.city" :class="{ 'invalid': errors.city }" required />
        </div>
        <div class="form-element">
            <label for="city">Country</label>
            <select name="country" class="form-control" id="country" v-model="form.country" required>
                <option v-for="country in countryOptions" :key="country.value" :value="country.value" :label="country.label">{{ country.label }}</option>
            </select>
        </div>
        <h5 class="firmware_title_row">How can I reach you?</h5>
        <div class="form-element">
            <label for="fullname">Name</label>
            <input type="text" name="fullname" id="fullname" placeholder="Max" v-model="form.fullname" :class="{ 'invalid': errors.fullname }" required />
        </div>
        <div class="form-element top name-container">
            <label for="email">Email</label>
            <input type="email" name="email" id="email" placeholder="max@mustermann.net" v-model="form.email" :class="{ 'invalid': errors.email }" required />
        </div>
        <div class="form-element top email-container">
            <label for="discord">Discord Tag <Badge type="info">Optional</Badge></label>
            <input type="text" name="discord" id="discord" placeholder="azonflo" v-model="form.discord" />
        </div>
        <h5 class="firmware_title_row">Anything else you'd like to share?</h5>
        <div class="form-element">
            <label for="discord">Notes <Badge type="info">Optional</Badge></label>
            <textarea id="message" name="message" v-model="form.message" placeholder="Any special requirements or something else you want to ask?"></textarea>
        </div>
        <div class="submit">
            <VPButton type="button" text="Next" @click="nextStep" />
        </div>
    </div>
    <div v-else-if="step == 2">
        <h5 class="firmware_title_row">How would you like to pay?</h5>
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
        <h5 class="firmware_title_row">Order summary</h5>
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
                <span><strong>Estimated Total: </strong></span>
                <span>
                    <Badge type="warning">{{ total_price.toFixed(2) }} €</Badge>
                </span>
            </li>
        </ul>
        <div class="warning custom-block" style="padding-top: 8px">
            This is a non-binding estimate including your selected product and shipping. You will receive a confirmation and payment instructions by email after submitting your request.
        </div>
        <div class="submit">
            <VPButton type="button" text="Back" @click="previousStep" />
            <VPButton type="submit" text="Submit Inquiry" :disabled="processing" />
        </div>
    </div>
</form>