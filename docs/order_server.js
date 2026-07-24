const express = require('express');
const cors = require('cors');
const app = express();
const port = 30002;

const path = require('path');
const dayjs = require('dayjs');
const crypto = require('crypto');
const axios = require('axios');
const { CronJob } = require('cron');

const session = require('express-session');

const FileStore = require('session-file-store')(session);
const fileStoreOptions = {};

const nodemailer = require('nodemailer');
const Handlebars = require('handlebars');

const USERS = [
  { username: 'florian', password: 'Start123!' }
];

const sqlite3 = require('sqlite3').verbose();
const db = new sqlite3.Database('./orders.db');

const baseComponents = ['core_board', 'audio_extension', 'enclosure'];

const defaultProducts = [
  {
    key: 'core_board',
    name: 'Core Board',
    price: 37,
    available: true,
    components: { core_board: 1 }
  },
  {
    key: 'audio_extension',
    name: 'Audio Extension',
    price: 8,
    available: false,
    components: { audio_extension: 1 }
  },
  {
    key: 'enclosure',
    name: 'Protective Enclosure',
    price: 4,
    available: true,
    components: { enclosure: 1 }
  },
  {
    key: 'core_board_enclosure',
    name: 'Starter Pack',
    price: 39,
    available: true,
    components: { core_board: 1, enclosure: 1 }
  },
  {
    key: 'core_board_audio_extension',
    name: 'Audio Essentials Pack',
    price: 42,
    available: false,
    components: { core_board: 1, audio_extension: 1 }
  },
  {
    key: 'core_board_audio_extension_enclosure',
    name: 'All inclusive Pack',
    price: 45,
    available: false,
    components: { core_board: 1, audio_extension: 1, enclosure: 1 }
  }
];

let products = [];
let availabilityExtraText = '';

db.serialize(() => {

  db.run(`CREATE TABLE IF NOT EXISTS orders (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp INTEGER DEFAULT 0,
    updated_timestamp INTEGER DEFAULT 0,
    hash TEXT DEFAULT '',
    name TEXT DEFAULT '',
    fullname TEXT DEFAULT '',
    email TEXT DEFAULT '',
    discord TEXT DEFAULT '',
    street TEXT DEFAULT '',
    zip TEXT DEFAULT '',
    city TEXT DEFAULT '',
    country TEXT DEFAULT 'DE',
    address_extra TEXT,
    product TEXT DEFAULT 'pcb',
    amount INTEGER DEFAULT 1,
    shipping_region TEXT DEFAULT 'DE',
    shipping_method TEXT DEFAULT 'envelope',
    payment_option TEXT DEFAULT 'paypal',
    message TEXT DEFAULT '',
    model TEXT DEFAULT '',
    status TEXT DEFAULT 'pending_review',
    tracking TEXT DEFAULT '',
    language TEXT DEFAULT 'en'
  )`);

  db.run(`CREATE TABLE IF NOT EXISTS stock (
    product TEXT PRIMARY KEY,
    amount INTEGER DEFAULT 0,
    available_from INTEGER DEFAULT 0
  )`);

  for (const component of baseComponents) {
    db.run(`INSERT OR IGNORE INTO stock (product, amount, available_from) VALUES (?, 0, 0)`, [component]);
  }

  db.run(`CREATE TABLE IF NOT EXISTS products (
    key TEXT PRIMARY KEY,
    name TEXT NOT NULL,
    price REAL NOT NULL,
    available INTEGER NOT NULL DEFAULT 0,
    components TEXT NOT NULL DEFAULT '{}'
  )`);

  for (const p of defaultProducts) {
    db.run(
      `INSERT OR IGNORE INTO products (key, name, price, available, components) VALUES (?, ?, ?, ?, ?)`,
      [p.key, p.name, p.price, p.available ? 1 : 0, JSON.stringify(p.components)]
    );
  }

  db.run(`CREATE TABLE IF NOT EXISTS settings (
    key TEXT PRIMARY KEY,
    value TEXT NOT NULL DEFAULT ''
  )`);

  db.run(
    `INSERT OR IGNORE INTO settings (key, value) VALUES ('availability_extra_text', ?)`,
    ['<b>Note:</b> The first batch of revision 2.x.x is currently in production.']
  );

});


const transporter = nodemailer.createTransport({
  host: 'mail.azon.ai', // e.g., smtp.gmail.com
  port: 587, // 465 for secure SMTP (SSL)
  secure: false, // true for port 465, false for other ports
  auth: {
    user: 'florian.hess@azon.ai',
    pass: '{,atIcherVeRfEratErRUstrACkeySAltO,65', // Use app password if needed
  },
});



const complete_old_orders_job = new CronJob('0 0 * * *', async () => {
  closeStaleOrders();
	completePendingArrivalOrders();
});


const check_paypal_job = new CronJob('0 0 * * *', async () => {
  checkPaypal();
});



app.set('trust proxy', 1); 
app.use(express.json());
app.use(express.urlencoded({ extended: true }));

app.use(cors({
  origin: ['https://doorman.azon.ai', 'https://dev.doorman.azon.ai', 'https://experimental.doorman.azon.ai', 'https://order.doorman.azon.ai', 'http://localhost:8123', 'http://localhost:4173', 'http://localhost'],
  credentials: true
}));

app.use(session({
    store: new FileStore(fileStoreOptions),
    secret: 'keyboard cat',
    resave: false,
    saveUninitialized: true,
    cookie: {
      domain: '.doorman.azon.ai',
      maxAge: 3153599998385,
      sameSite: 'none',
      secure: true
    }
}));

app.use((req, res, next) => {
  req.session.touch();
  next();
});

function authRequired(req, res, next) {
  if (!req.session.user) return res.status(401).json({ error: 'Unauthorized!' });
  next();
}





const payment_options = [
  {
      key: 'paypal',
      name: 'PayPal'
  },
  {
      key: 'sepa',
      name: 'SEPA'
  },
];

const shipping_regions = [
  {
      key: 'DE',
      options: [
          {
              key: 'envelope',
              name: 'Deutsche Post Großbrief',
              price: 2,
              max_items: 2
          },
          {
              key: 'standard',
              name: 'DHL Päckchen XS',
              price: 5,
              max_items: 0
          },
          {
              key: 'tracking',
              name: 'DHL Paket (Tracking)',
              price: 6.5,
              max_items: 0
          }
      ],
  },
  {
      key: 'CH',
      options: [
          {
              key: 'standard',
              name: 'DHL Päckchen XS',
              price: 9,
              max_items: 0
          },
          {
              key: 'tracking',
              name: 'DHL Paket (Tracking)',
              price: 27,
              max_items: 0
          }
      ],
  },
  {
      key: 'EU',
      options: [
          {
              key: 'standard',
              name: 'DHL Päckchen XS',
              price: 7,
              max_items: 0
          },
          {
              key: 'tracking',
              name: 'DHL Paket (Tracking)',
              price: 14.5,
              max_items: 0
          }
      ]
  },
  {
      key: 'INT',
      options: [
          {
              key: 'standard',
              name: 'DHL Päckchen XS',
              price: 15,
              max_items: 0
          },
          {
              key: 'tracking',
              name: 'DHL Paket (Tracking)',
              price: 30,
              max_items: 0
          }
      ]
  }
];

const allCountries = [
  { key: 'AT', name: 'Austria' },
  { key: 'BE', name: 'Belgium' },
  { key: 'BG', name: 'Bulgaria' },
  { key: 'HR', name: 'Croatia' },
  { key: 'CY', name: 'Cyprus' },
  { key: 'CZ', name: 'Czech Republic' },
  { key: 'DK', name: 'Denmark' },
  { key: 'EE', name: 'Estonia' },
  { key: 'FI', name: 'Finland' },
  { key: 'FR', name: 'France' },
  { key: 'DE', name: 'Germany' },
  { key: 'GR', name: 'Greece' },
  { key: 'HU', name: 'Hungary' },
  { key: 'IE', name: 'Ireland' },
  { key: 'IT', name: 'Italy' },
  { key: 'LV', name: 'Latvia' },
  { key: 'LT', name: 'Lithuania' },
  { key: 'LU', name: 'Luxembourg' },
  { key: 'MT', name: 'Malta' },
  { key: 'NL', name: 'Netherlands' },
  { key: 'PL', name: 'Poland' },
  { key: 'PT', name: 'Portugal' },
  { key: 'RO', name: 'Romania' },
  { key: 'SK', name: 'Slovakia' },
  { key: 'SI', name: 'Slovenia' },
  { key: 'SE', name: 'Sweden' },
  { key: 'ES', name: 'Spain' },
  { key: 'CH', name: 'Switzerland' },
  { key: 'RS', name: 'Serbia' },
  { key: 'CN', name: 'China' },
  { key: 'IS', name: 'Iceland' },
];





app.post('/user/login', (req, res) => {
  const { username, password } = req.body;

  const user = USERS.find(u => u.username === username);

  if (!user || user.password !== password) {
    return res.status(401).json({ error: 'Invalid username or password!' });
  }

  // Save login state in session
  req.session.user = { username };
  res.json({ success: true });
});

app.post('/user/logout', (req, res) => {
  req.session.user = null;
  res.json({ success: true });
});

app.get('/user', async (req, res) => {
  if (!req.session.user) {
    return res.json(null);
  }
  return res.json(req.session.user);
});

// Current order status
app.get('/order', async (req, res) => {
  try {
    const id = Number(req.session.order_id) || 0;
    const order = await getOrderById(id);

    if (!order) {
      req.session.order_id = 0;
    }

    if(!req.session.user) {
      res.json(buildOrderStatus(order, false));
    } else {
      res.json(buildOrderStatus(order));
    }
  } catch (err) {
    res.status(500).json({ error: 'Failed to fetch order!' });
  }
});

// Create new order
app.post('/order', async (req, res) => {
  try {
    const order = await createOrder(req.body);
    req.session.order_id = order.id;

    try {
      await sendOrderEmail(order, order.status);
      await sendOrderEmail(order, 'admin_new_order');
      res.json(buildOrderStatus(order));
    } catch (error) {
      console.error(error);
      res.status(500).json({ error: 'Failed to create order!' });
    }
    
  } catch (err) {
    console.error(err);
    res.status(500).json({ error: 'Failed to create order!' });
  }
});

// Close current order
app.post('/order/close', async (req, res) => {
  try {
    const orderId = Number(req.session.order_id) || 0;
    const order = await getOrderById(orderId);

    if(!order) {
      return res.status(500).json({ error: 'Failed to close order!' });
    }

    if(order.status != 'shipped') {
      return res.status(400).json({ error: 'Only shipped orders can be closed!' });
    }

    const result = await closeOrderAndNotify(order);

    res.json(buildOrderStatus(result));
  } catch (err) {
    res.status(500).json({ error: 'Failed to close order!' });
  }
});

// Reset current order in session
app.post('/order/reset', async (req, res) => {
  req.session.order_id = 0;
  res.json(buildOrderStatus(null));
});

// Cancel current order
app.post('/order/cancel', async (req, res) => {
  const orderId = Number(req.session.order_id) || 0;
  try {
    const result = await cancelOrder(orderId, true);
    res.json(buildOrderStatus(result));
  } catch (err) {
    console.error(err);
    res.status(500).json({ error: err.message || 'Failed to cancel order!' });
  }
});

// Next step for current order
app.post('/order/next', authRequired, async (req, res) => {
  const orderId = Number(req.session.order_id) || 0;
  const tracking = req.body.tracking || '';

  try {
    const order = await getOrderById(orderId);
    if (!order) {
      return res.status(500).json({ error: 'Order not found!' });
    }

    const result = await moveOrderToNextStatus(order, tracking);
    res.json(buildOrderStatus(result));
  } catch (err) {
    console.error(err);
    return res.status(500).json({ error: err.message || 'Failed to process order!' });
  }
});



// Order list for admin
app.get('/order/list', authRequired, async (req, res) => {
  db.all('SELECT * FROM orders ORDER BY id DESC', [], (err, rows) => {
    if (err) {
      console.error(err);
      return res.status(500).json({ error: 'Failed to fetch orders!' });
    }


    let newrows = rows.map(row => {
      row.country_details = allCountries.find(p => p.key === row.country) || null;
      row.product_details = products.find(p => p.key === row.product) || null;
      row.payment_details = payment_options.find(p => p.key === row.payment_option) || null;
      const region = shipping_regions.find(r => r.key === row.shipping_region);
      if (region) {
        row.shipping_details = region.options.find(o => o.key === row.shipping_method) || null;
      } else {
        row.shipping_details = null;
      }

      return row;
    });

    res.json(newrows);
  });
});




app.get('/order/:orderHash', async (req, res) => {
  try {
    const hash = req.params.orderHash;
    const order = await getOrderByHash(hash);

    if (!order) {
      req.session.order_id = 0;
    } else {
      req.session.order_id = order.id;
    }

    res.redirect('https://doorman.azon.ai/order');
  } catch (err) {
    res.status(500).json({ error: 'Failed to fetch order!' });
  }
});

app.get('/order/:orderHash/details', async (req, res) => {
  try {
    const hash = req.params.orderHash;
    const order = await getOrderByHash(hash);

    console.log("Requested order details for hash:", hash);
    console.log("Fetched order for details page:", order);

    if (!order) {
      req.session.order_id = 0;
    } else {
      req.session.order_id = order.id;
    }

    if(!req.session.user) {
      res.json(buildOrderStatus(order, false));
    } else {
      res.json(buildOrderStatus(order));
    }
  } catch (err) {
    res.status(500).json({ error: 'Failed to fetch order!' });
  }
});




app.post('/order/:id/next', authRequired, async (req, res) => {
  const orderId = Number(req.params.id);
  const tracking = req.body.tracking || '';

  try {
    const order = await getOrderById(orderId);
    if (!order) {
      return res.status(500).json({ error: 'Order not found!' });
    }

    const result = await moveOrderToNextStatus(order, tracking);
    res.json(buildOrderStatus(result));
  } catch (err) {
    console.error(err);
    return res.status(500).json({ error: err.message || 'Failed to process order!' });
  }
});

app.post('/order/:id/cancel', authRequired, async (req, res) => {
  const orderId = Number(req.params.id);
  try {
    const result = await cancelOrder(orderId);
    res.json(buildOrderStatus(result));
  } catch (err) {
    console.error(err);
    res.status(500).json({ error: err.message || 'Failed to cancel order!' });
  }
});

app.post('/order/:id/delete', authRequired, async (req, res) => {
  const orderId = Number(req.params.id);

  try {
    const order = await getOrderById(orderId);

    if (!order) {
      return res.status(404).json({ error: 'Order not found!' });
    }

    if (order.status != 'cancelled') {
      return res.status(400).json({ error: 'Only cancelled orders can be deleted!' });
    }

    await db.run('DELETE FROM orders WHERE id = ?', [orderId]);

    res.json(buildOrderStatus(null));
  } catch (err) {
    console.error(err);
    return res.status(500).json({ error: 'Failed to delete order!' });
  }
});




app.get('/products', async (req, res) => {
  const componentAvailability = await getComponentAvailability();

  const productsWithAvailability = products.map(p => {
    const { available_units, available_from } = computeProductAvailability(p, componentAvailability);
    return {
      key: p.key,
      name: p.name,
      price: p.price,
      available: isProductEffectivelyAvailable(p),
      manual_available: p.available,
      components: p.components,
      available_units,
      available_timestamp: available_from
    };
  });

  const components = baseComponents.map(key => {
    const meta = products.find(p => p.key === key);
    const c = componentAvailability[key];
    return {
      key,
      name: meta ? meta.name : key,
      stock: c.stock,
      reserved: c.reserved,
      available_units: c.available_units,
      available_timestamp: c.available_from
    };
  });

  const coreBoard = productsWithAvailability.find(p => p.key === 'core_board') || { available_units: 0, available_timestamp: 0 };

  res.json({
    products: productsWithAvailability,
    shipping_regions,
    components,
    available_units: coreBoard.available_units,
    available_timestamp: coreBoard.available_timestamp,
    availability_extra_text: availabilityExtraText
  });
});

app.post('/product/:name/availability', authRequired, async (req, res) => {
  const product = req.params.name;
  const timestamp = Number(req.body.timestamp);

  if (!baseComponents.includes(product)) {
    return res.status(400).json({ error: 'Unknown component!' });
  }

  if(timestamp < dayjs().startOf('day').unix()) {
    return res.status(400).json({ error: 'Date must be today or in the future!' });
  }

  try {
    const result = await updateAvailabilityDate(product, timestamp);

    res.json({ product: product, status: 'updated' });

  } catch (err) {
    console.error(err);
    return res.status(500).json({ error: 'Failed to update availability date!' });
  }
});

app.post('/product/:name/increase', authRequired, async (req, res) => {
  const product = req.params.name;
  const amount = Number(req.body.amount);

  if (!baseComponents.includes(product)) {
    return res.status(400).json({ error: 'Unknown component!' });
  }

  if(amount == 0) {
    return res.status(400).json({ error: 'Amount must be greater than 0!' });
  }

  try {
    const old_stock = await getStock(product);

    console.log(`Increasing stock for component ${product} by ${amount}. Old stock: ${old_stock.amount}`);

    await increaseStock(product, amount);
    await updateAvailabilityDate(product, 0);

    await processReservedOrders();

    const stock = await getStock(product);

    // Wenn vorher kein Bestand und keine reservierten Bestellungen waren, jetzt aber schon, sende Benachrichtigung
    let reserved_orders = await getOrdersByStatus('reserved');

    console.log('Check for notifying new stock...');
    console.log(`Reserved orders: ${reserved_orders.length}, Old stock: ${old_stock.amount}, New stock: ${stock.amount}`);

    if(reserved_orders.length == 0 && old_stock.amount == 0 && stock.amount > 0)
    {
      console.log('Notifying new stock on Discord...');
      await notifyNewStockDiscord();
    }

    return res.json({ product: product, stock: stock.amount, status: 'updated' });
  } catch (err) {
    console.error(err);
    return res.status(500).json({ error: 'Failed to update stock!' });
  }
});


app.post('/product/:name/decrease', authRequired, async (req, res) => {
  const product = req.params.name;
  const amount = Number(req.body.amount);

  if (!baseComponents.includes(product)) {
    return res.status(400).json({ error: 'Unknown component!' });
  }

  if(amount == 0) {
    return res.status(400).json({ error: 'Amount must be greater than 0!' });
  }

  try {
    const old_stock = await getStock(product);

    if(old_stock.amount < amount) {
      return res.status(400).json({ error: 'Not enough stock available!' });
    }

    await decreaseStock(product, amount);

    let stock = await getStock(product);

    return res.json({ product: product, stock: stock.amount, status: 'updated' });
  } catch (err) {
    console.error(err);
    return res.status(500).json({ error: 'Failed to update stock!' });
  }
});

app.post('/product/:key/toggle-available', authRequired, async (req, res) => {
  const key = req.params.key;
  const available = !!req.body.available;

  const product = products.find(p => p.key === key);
  if (!product) {
    return res.status(404).json({ error: 'Unknown product!' });
  }

  try {
    await setProductAvailable(key, available);
    product.available = available;

    return res.json({ key, available, status: 'updated' });
  } catch (err) {
    console.error(err);
    return res.status(500).json({ error: 'Failed to update product!' });
  }
});

app.post('/settings/announcement', authRequired, async (req, res) => {
  const value = String(req.body.value || '');

  try {
    await setAvailabilityExtraText(value);
    availabilityExtraText = value;

    return res.json({ value, status: 'updated' });
  } catch (err) {
    console.error(err);
    return res.status(500).json({ error: 'Failed to update announcement!' });
  }
});

app.post('/notify', authRequired, async (req, res) => {
  const status = req.body.status;
  const title = req.body.title;
  const message_de = req.body.message_de;
  const message_en = req.body.message_en;

  try {
      const pending_orders = await getOrdersByStatus(status);

      for(const order of pending_orders) {
        if(order.language === 'de') {
          await sendOrderEmail(order, 'custom', {
            title: title,
            message: message_de
          });
        } else {
          await sendOrderEmail(order, 'custom', {
            title: title,
            message: message_en
          });
        }
      }

    res.json({ status: 'notified' });

  } catch (err) {
    console.error(err);
    return res.status(500).json({ error: 'Failed to notify customers!' });
  }
});

app.get('/', (req, res) => {
  if (!req.session.user) {
    res.redirect('https://doorman.azon.ai/order');
  } else {
    res.redirect('https://doorman.azon.ai/order-management');
  }
});



app.post('/paypal_webhook', async (req, res) => {
  console.log(req.body);
  res.status(200).json(true);
});

Promise.all([loadProducts(), loadAvailabilityExtraText()]).then(() => {
  app.listen(port, () => {
    console.log(`Example app listening on port ${port}`);

    checkPaypal();
  });

  complete_old_orders_job.start();
  check_paypal_job.start();
}).catch(err => {
  console.error('Failed to load products from database:', err);
  process.exit(1);
});


async function processReservedOrders() {
  const reservedOrders = await getOrdersByStatus('reserved');

  console.log(`Processing reserved orders. Reserved orders: ${reservedOrders.length}`);

  for (const order of reservedOrders) {
    const available = await checkComponentsAvailable(order.product, order.amount);

    if (!available) {
      console.log(`Skipping order ${order.hash}: components not available for ${order.amount} × ${order.product}`);
      continue;
    }

    try {
      await moveOrderToNextStatus(order);
      console.log(`Successfully moved order ${order.hash} to next status`);
    } catch (err) {
      console.error(`Failed to move order ${order.hash} to next status:`, err);
    }
  }
}

function buildOrderStatus(order, full = true) {
  if (!order) {
    return { id: 0, status: 'none' };
  }

  if (full) {
    return order;
  } else {
    return {
      id: order.id,
      status: order.status,
      timestamp: order.timestamp,
      updated_timestamp: order.updated_timestamp,
      hash: order.hash,
      amount: order.amount,
      product: order.product,
      tracking: order.tracking,
      shipping_method: order.shipping_method,
      shipping_region: order.shipping_region
    };
  }
}

async function moveOrderToNextStatus(order, tracking = '') {
  const stageFlow = {
    pending_review: 'reserved',
    reserved: 'pending_payment',
    pending_payment: 'pending_shipment',
    pending_shipment: 'shipped',
    shipped: 'closed',
  };

  const nextStatus = stageFlow[order.status];
  if (!nextStatus) {
    throw new Error('No next stage available');
  }

  if (nextStatus === 'pending_payment') {
    const available = await checkComponentsAvailable(order.product, order.amount);
    if (!available) {
      throw new Error(`Product is out of stock!`);
    }
  }

  if (tracking !== '') {
    await db.run('UPDATE orders SET tracking = ? WHERE id = ?', [tracking, order.id]);
    order.tracking = tracking;
  }

  await db.run('UPDATE orders SET status = ?, updated_timestamp = ? WHERE id = ?', [nextStatus, dayjs().unix(), order.id]);
  order.status = nextStatus;

  if (nextStatus === 'pending_payment') {
    await decreaseComponentsStock(order.product, order.amount);
  }

  if (nextStatus === 'closed') {
    await closeOrderAndNotify(order);
  } else {
    await sendOrderEmail(order, nextStatus);
  }

  return order;
}

async function cancelOrder(orderId, by_customer = false) {
  const order = await getOrderById(orderId);

  if (!order) throw new Error('Order not found');

  if (['shipped', 'closed', 'cancelled'].includes(order.status)) {
    throw new Error('Only orders which are not shipped, closed or cancelled can be cancelled!');
  }

  if (['pending_payment', 'pending_shipment'].includes(order.status)) {
    await increaseComponentsStock(order.product, order.amount);
    await processReservedOrders();
  }

  await db.run(
    'UPDATE orders SET status = ?, updated_timestamp = ? WHERE id = ?', ['cancelled', dayjs().unix(), orderId]
  );

  order.status = 'cancelled';

  if(by_customer) {
    await sendOrderEmail(order, 'cancelled', { subtype: 'standard' });
    await sendOrderEmail(order, 'admin_order_cancelled');
  } else {
    await sendOrderEmail(order, 'cancelled');
  }

  return order;
}

async function closeOrderAndNotify(order) {
  if (!order) return null;

  // Status auf closed setzen
  const result = await closeOrder(order.id);

  await sendOrderEmail(order, 'closed');

  return result;
}

function createOrder(orderData) {
  return new Promise((resolve, reject) => {
    const {
      language = 'en',
      fullname = 'Max Mustermann',
      name = '',
      email = '',
      discord = '',
      street = '',
      zip = '',
      city = '',
      country = 'DE',
      address_extra = '',
      product = 'pcb',
      amount = 1,
      shipping_region = 'DE',
      shipping_method = 'envelope',
      payment_option = 'paypal',
      message = '',
      model = '',
    } = orderData;

    const status = 'pending_review';
    const timestamp = dayjs().unix();
    const hash = crypto.createHash('sha1')
      .update(`${timestamp}-${Math.random()}`)
      .digest('hex')
      .slice(0, 8)
      .toUpperCase();

    db.run(
      `INSERT INTO orders 
      (language,fullname,name,email,discord,street,zip,city,country,address_extra,product,amount,shipping_region,shipping_method,payment_option,message,model,status,timestamp,updated_timestamp,hash)
      VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)`,
      [language,fullname,name,email.toLowerCase(),discord.toLowerCase(),street,zip,city,country,address_extra,product,amount,shipping_region,shipping_method,payment_option,message,model,status,timestamp,timestamp,hash],
      function(err) {
        if (err) reject(err);
        else resolve({ id: this.lastID, ...orderData, status, hash });
      }
    );
  });
}

function getOrderById(id) {
  return new Promise((resolve, reject) => {
    db.get('SELECT * FROM orders WHERE id = ?', [id], (err, row) => {
      if (err) reject(err);
      if (!row) return resolve(null);
      resolve(row);
    });
  });
}

function getOrderByHash(hash) {
  return new Promise((resolve, reject) => {
    db.get('SELECT * FROM orders WHERE hash = ?', [hash.toUpperCase()], (err, row) => {
      if (err) reject(err);
      if (!row) return resolve(null);
      resolve(row);
    });
  });
}

function getOrdersByStatus(status) {
  return new Promise((resolve, reject) => {
    db.all('SELECT * FROM orders WHERE status = ?', [status], (err, rows) => {
      if (err) reject(err);
      if (!rows) return resolve([]);
      resolve(rows);
    });
  });
}

function getStock(product) {
  return new Promise((resolve, reject) => {
    db.get('SELECT amount, available_from FROM stock WHERE product = ?', [product], (err, row) => {
      if (err) reject(err);
      if (!row) return resolve(null);
      resolve(row);
    });
  });
}

function loadProducts() {
  return new Promise((resolve, reject) => {
    db.all('SELECT * FROM products', [], (err, rows) => {
      if (err) return reject(err);

      products = rows.map(row => ({
        key: row.key,
        name: row.name,
        price: row.price,
        available: !!row.available,
        components: JSON.parse(row.components)
      }));

      resolve(products);
    });
  });
}

function setProductAvailable(key, available) {
  return new Promise((resolve, reject) => {
    db.run('UPDATE products SET available = ? WHERE key = ?', [available ? 1 : 0, key], (err) => {
      if (err) reject(err);
      else resolve();
    });
  });
}

function loadAvailabilityExtraText() {
  return new Promise((resolve, reject) => {
    db.get(`SELECT value FROM settings WHERE key = 'availability_extra_text'`, [], (err, row) => {
      if (err) return reject(err);
      availabilityExtraText = row ? row.value : '';
      resolve(availabilityExtraText);
    });
  });
}

function setAvailabilityExtraText(value) {
  return new Promise((resolve, reject) => {
    db.run(`UPDATE settings SET value = ? WHERE key = 'availability_extra_text'`, [value], (err) => {
      if (err) reject(err);
      else resolve();
    });
  });
}

function increaseStock(product, amount) {
  return new Promise((resolve, reject) => {
    db.run('UPDATE stock SET amount = amount + ? WHERE product = ?', [amount, product], (err, row) => {
      if (err) reject(err);
      if (!row) return resolve(null);
      resolve(row);
    });
  });
}

function updateAvailabilityDate(product, timestamp) {
  return new Promise((resolve, reject) => {
    db.run('UPDATE stock SET available_from = ? WHERE product = ?', [timestamp, product], (err, row) => {
      if (err) reject(err);
      if (!row) return resolve(null);
      resolve(row);
    });
  });
}

function decreaseStock(product, amount) {
  return new Promise((resolve, reject) => {
    db.run('UPDATE stock SET amount = amount - ? WHERE product = ?', [amount, product], (err, row) => {
      if (err) reject(err);
      if (!row) return resolve(null);
      resolve(row);
    });
  });
}

async function getComponentAvailability() {
  const reservedOrders = await getOrdersByStatus('reserved');
  const reservedUsage = {};
  for (const key of baseComponents) reservedUsage[key] = 0;

  for (const order of reservedOrders) {
    const product = products.find(p => p.key === order.product);
    if (!product) continue;

    for (const [component, qty] of Object.entries(product.components)) {
      reservedUsage[component] = (reservedUsage[component] || 0) + qty * order.amount;
    }
  }

  const availability = {};
  for (const key of baseComponents) {
    const stockRow = await getStock(key) || { amount: 0, available_from: 0 };
    let available_units = stockRow.amount - reservedUsage[key];
    if (available_units < 0) available_units = 0;

    availability[key] = {
      stock: stockRow.amount,
      reserved: reservedUsage[key],
      available_units,
      available_from: stockRow.available_from
    };
  }

  return availability;
}

function computeProductAvailability(product, componentAvailability) {
  let available_units = Infinity;
  let available_from = 0;

  for (const [component, qty] of Object.entries(product.components)) {
    const c = componentAvailability[component] || { available_units: 0, available_from: 0 };
    available_units = Math.min(available_units, Math.floor(c.available_units / qty));
    available_from = Math.max(available_from, c.available_from);
  }

  if (!isFinite(available_units)) available_units = 0;

  return { available_units, available_from };
}

function isProductEffectivelyAvailable(product) {
  if (!product.available) return false;

  return Object.keys(product.components).every(componentKey => {
    const componentProduct = products.find(p => p.key === componentKey);
    return !componentProduct || componentProduct.available;
  });
}

async function checkComponentsAvailable(productKey, amount) {
  const product = products.find(p => p.key === productKey);
  if (!product) return false;

  for (const [component, qty] of Object.entries(product.components)) {
    const stock = await getStock(component);
    if (!stock || stock.amount < qty * amount) return false;
  }

  return true;
}

async function increaseComponentsStock(productKey, amount) {
  const product = products.find(p => p.key === productKey);
  if (!product) return;

  for (const [component, qty] of Object.entries(product.components)) {
    await increaseStock(component, qty * amount);
  }
}

async function decreaseComponentsStock(productKey, amount) {
  const product = products.find(p => p.key === productKey);
  if (!product) return;

  for (const [component, qty] of Object.entries(product.components)) {
    await decreaseStock(component, qty * amount);
  }
}

function closeOrder(id) {
  return new Promise((resolve, reject) => {
    db.run('UPDATE orders SET status = ?, updated_timestamp = ? WHERE id = ?', ['closed', dayjs().unix(), id], function(err) {
      if (err) reject(err);
      else resolve({ id, status: 'closed' });
    });
  });
}

function niceName(name) {
  if (typeof name !== 'string') {
    return 'Error';
  }

  const trimmed = name.trim();
  if (!trimmed) {
    return 'Error';
  }

  const firstWord = trimmed.split(/\s+/)[0];
  return firstWord || 'Error';
}


async function notifyNewStockDiscord() {
  const payload = {
    "content": "@everyone",
    "embeds": [
      {
        "title": "Doorman Order Service",
        "url": "https://doorman.azon.ai/order",
        "color": 6684927,
        "fields": [
          {
            "name": "Neue Doormans verfügbar! :tada:",
            "value": "Jetzt wieder auf Lager!\n\n**Hol dir deinen Doorman:**\n👉 [Jetzt anfragen](https://doorman.azon.ai/de/order)\n\n⏳ Nur solange der Vorrat reicht!",
            "inline": true
          },
          {
            "name": "New Doormans available! :tada:",
            "value": "They're back in stock!\n\n**Get your Doorman:**\n👉 [Request now](https://doorman.azon.ai/order)\n\n⏳ Don't wait too long — limited supply!",
            "inline": true
          }
        ]
      }
    ],
    "attachments": []
  };

  try {
    const response = await axios.post('https://discord.com/api/webhooks/1425949680545562798/zciEe2ceSynL6cb4RguSQoZZIUjqSh7msL2Sy3YZ_D6B91XBIZI7v-Tcy9VPkydgPhRT', payload);
    console.log("Sent message on discord:", response.status);
  } catch (error) {
    console.error("Failed to send message on discord:", error.message);
  }
}


const emailTemplates = {
  pending_review: {
    de: {
      subject: 'Doorman #{{hash}}',
      template: `<p>Hey {{firstName}},</p>
<p>ich habe deine Anfrage erhalten und werde mich schnellstmöglich mit dir in Verbindung setzen um die Kompatibilität abzuklären.<br/>Dies ist eine <u>automatisch generierte</u> E-Mail.</p>
<p style="margin: 30px 0;">
  <a href="https://order.doorman.azon.ai/order/{{hash}}" style="background-color: #3e63dd; color: white; padding: 9px 23px; text-decoration: none; border-radius: 35px; display: inline-block; font-family: Arial, sans-serif;">
    Bestellstatus ansehen
  </a>
</p>
<p>Beste Grüße<br/>Flo</p>
<br></hr><i>Dies ist eine automatisch generierte E-Mail.</i>`
    },
    en: {
      subject: 'Doorman #{{hash}}',
      template: `<p>Hey {{firstName}},</p>
<p>I received your request and will reach out to you as soon as possible to talk about compatibility.<br/>This is an <u>automatically generated</u> email.</p>
<p style="margin: 30px 0;">
  <a href="https://order.doorman.azon.ai/order/{{hash}}" style="background-color: #3e63dd; color: white; padding: 9px 23px; text-decoration: none; border-radius: 35px; display: inline-block; font-family: Arial, sans-serif;">
    View order status
  </a>
</p>
<p>Best regards<br/>Flo</p>
<br></hr><i>This is an automatically generated email.</i>`
    }
  },

  reserved: {
    de: {
      subject: 'Doorman #{{hash}} Reserviert',
      template: `<p>Hey {{firstName}},</p>
<p>vielen Dank für dein Interesse! Ich habe dir <b>{{productComponents}}</b> reserviert.</p>
<p style="margin: 30px 0;">
  <a href="https://order.doorman.azon.ai/order/{{hash}}" style="background-color: #3e63dd; color: white; padding: 9px 23px; text-decoration: none; border-radius: 35px; display: inline-block; font-family: Arial, sans-serif;">
    Bestellstatus ansehen
  </a>
</p>
<p>Beste Grüße<br/>Flo</p>
<br></hr><i>Dies ist eine automatisch generierte E-Mail.</i>`
    },
    en: {
      subject: 'Doorman #{{hash}} Reserved',
      template: `<p>Hey {{firstName}},</p>
<p>Thank you very much for your interest. I've reserved <b>{{productComponents}}</b> for you.</p>
<p style="margin: 30px 0;">
  <a href="https://order.doorman.azon.ai/order/{{hash}}" style="background-color: #3e63dd; color: white; padding: 9px 23px; text-decoration: none; border-radius: 35px; display: inline-block; font-family: Arial, sans-serif;">
    View order status
  </a>
</p>
<p>Best regards<br/>Flo</p>
<br></hr><i>This is an automatically generated email.</i>`
    }
  },

  pending_payment: {
    de: {
      paypal: {
        subject: 'Doorman #{{hash}} Zahlungsanweisung',
        template: `<p>Hey {{firstName}},</p>
<p>vielen Dank für dein Interesse! ✨</p>
<p>Bitte sende <b>{{price}} €</b> per <b>Paypal Freunde & Familie</b> (muss explizit gewählt werden) an:</p>
<p>
  <b>Empfänger:</b> doorman@azon.ai<br>
  <b>Bemerkung:</b> Doorman #{{hash}}
</p>
<p>
  💡 Hinweis: Bitte wähle <u>NICHT</u> "Waren & Dienstleistungen", damit keine Gebühren entstehen.<br>
  Sobald die Zahlung eingegangen ist, macht sich dein Paket direkt zu dir auf den Weg 🚀.
</p>
<p>Falls du noch Fragen hast, melde dich jederzeit gerne bei mir - ich helfe dir sofort weiter. 🤗</p>
<p style="margin: 30px 0;">
  <a href="https://order.doorman.azon.ai/order/{{hash}}" style="background-color: #3e63dd; color: white; padding: 9px 23px; text-decoration: none; border-radius: 35px; display: inline-block; font-family: Arial, sans-serif;">
    Bestellstatus ansehen
  </a>
</p>
<p>Beste Grüße<br/>Flo</p>
<br></hr><i>Dies ist eine automatisch generierte E-Mail.</i>`
      },
      sepa: {
        subject: 'Doorman #{{hash}} Zahlungsanweisung',
        template: `<p>Hey {{firstName}},</p>
<p>vielen Dank für dein Interesse! ✨</p>
<p>Bitte überweise <b>{{price}} €</b> an folgendes Konto:</p>
<p>
  <b>Empfänger:</b> Florian Hess<br>
  <b>Empfänger Adresse:</b> 97080 Würzburg<br>
  <b>IBAN:</b> DE90 5001 0517 5441 6168 04<br>
  <b>BIC/SWIFT:</b> INGDDEFFXXX<br>
  <b>Bank:</b> ING-DiBa<br>
  <b>Referenz:</b> Doorman #{{hash}}
</p>
<p>Sobald die Zahlung eingegangen ist, macht sich dein Paket direkt zu dir auf den Weg 🚀.</p>
<p>Falls du noch Fragen hast, melde dich jederzeit gerne bei mir - ich helfe dir sofort weiter. 🤗</p>
<p style="margin: 30px 0;">
  <a href="https://order.doorman.azon.ai/order/{{hash}}" style="background-color: #3e63dd; color: white; padding: 9px 23px; text-decoration: none; border-radius: 35px; display: inline-block; font-family: Arial, sans-serif;">
    Bestellstatus ansehen
  </a>
</p>
<p>Beste Grüße<br/>Flo</p>
<br></hr><i>Dies ist eine automatisch generierte E-Mail.</i>`
      }
    },
    en: {
      paypal: {
        subject: 'Doorman #{{hash}} Payment Instructions',
        template: `<p>Hey {{firstName}},</p>
<p>Thank you so much for your interest! ✨</p>
<p>Please send <b>{{price}} €</b> via <b>PayPal Friends & Family</b> (make sure to select this option explicitly) to:</p>
<p>
  <b>Receiver:</b> doorman@azon.ai<br>
  <b>Note:</b> Doorman #{{hash}}
</p>
<p>
💡 Note: Please <u>DO NOT</u> select "Goods & Services" to avoid any extra fees.<br>
Once your payment is received, your package will be on its way to you 🚀.
</p>
<p>If you have any questions, feel free to reach out anytime - I'm happy to help! 🤗</p>
<p style="margin: 30px 0;">
  <a href="https://order.doorman.azon.ai/order/{{hash}}" style="background-color: #3e63dd; color: white; padding: 9px 23px; text-decoration: none; border-radius: 35px; display: inline-block; font-family: Arial, sans-serif;">
    View order status
  </a>
</p>
<p>Best regards,<br/>Flo</p>
<br></hr><i>This is an automatically generated email.</i>`
      },
      sepa: {
        subject: 'Doorman #{{hash}} Payment Instructions',
        template: `<p>Hey {{firstName}},</p>
<p>Thank you so much for your interest! ✨</p>
<p>Please transfer <b>{{price}} €</b> to the following bank account:</p>
<p>
  <b>Account Holder:</b> Florian Hess<br>
  <b>Account Holder address:</b> 97080 Würzburg<br>
  <b>IBAN:</b> DE90 5001 0517 5441 6168 04<br>
  <b>BIC/SWIFT:</b> INGDDEFFXXX<br>
  <b>Bank:</b> ING-DiBa<br>
  <b>Reference/Payment Note:</b> Doorman #{{hash}}
</p>
<p>Once your payment is received, your package will be on its way to you 🚀.</p>
<p>If you have any questions, feel free to reach out anytime - I'm happy to help! 🤗</p>
<p style="margin: 30px 0;">
  <a href="https://order.doorman.azon.ai/order/{{hash}}" style="background-color: #3e63dd; color: white; padding: 9px 23px; text-decoration: none; border-radius: 35px; display: inline-block; font-family: Arial, sans-serif;">
    View order status
  </a>
</p>
<p>Best regards,<br/>Flo</p>
<br></hr><i>This is an automatically generated email.</i>`
      }
    }
  },

  pending_shipment: {
    de: {
      subject: 'Doorman #{{hash}} Zahlung erhalten',
      template: `<p>Hey {{firstName}},</p>
<p>super, deine Zahlung ist angekommen! Ich mache in den nächsten Tagen alles fertig und verschicke es dann direkt.</p>
<p style="margin: 30px 0;">
  <a href="https://order.doorman.azon.ai/order/{{hash}}" style="background-color: #3e63dd; color: white; padding: 9px 23px; text-decoration: none; border-radius: 35px; display: inline-block; font-family: Arial, sans-serif;">
    Bestellstatus ansehen
  </a>
</p>
<p>Beste Grüße<br/>Flo</p>
<br></hr><i>Dies ist eine automatisch generierte E-Mail.</i>`
    },
    en: {
      subject: 'Doorman #{{hash}} Payment received',
      template: `<p>Hey {{firstName}},</p>
<p>Great, your payment has been received! I'll get everything ready over the next few days and then ship it out right away.</p>
<p style="margin: 30px 0;">
  <a href="https://order.doorman.azon.ai/order/{{hash}}" style="background-color: #3e63dd; color: white; padding: 9px 23px; text-decoration: none; border-radius: 35px; display: inline-block; font-family: Arial, sans-serif;">
    View order status
  </a>
</p>
<p>Best regards<br/>Flo</p>
<br></hr><i>This is an automatically generated email.</i>`
    }
  },

  shipped: {
    de: {
      tracking: {
        subject: 'Deine Bestellung #{{hash}} ist auf dem Weg!',
        template: `<p>Hey {{firstName}},</p>
<p>deine Bestellung befindet sich jetzt auf dem Weg zu dir 🚀<br>Bitte bestätige den Erhalt deines Päckchens, sobald es bei dir angekommen ist.</p>
<p style="margin: 30px 0;">
  <a href="https://order.doorman.azon.ai/order/{{hash}}" style="background-color: #3e63dd; color: white; padding: 9px 23px; text-decoration: none; border-radius: 35px; display: inline-block; font-family: Arial, sans-serif;">
    Empfang bestätigen
  </a>
  <a href="https://www.dhl.de/de/privatkunden/dhl-sendungsverfolgung.html?piececode={{tracking}}" style="background-color: #3e63dd; color: white; padding: 9px 23px; text-decoration: none; border-radius: 35px; display: inline-block; font-family: Arial, sans-serif;">
    DHL Sendungsverfolgung
  </a>
</p>
<p>Beste Grüße<br/>Flo</p>
<br></hr><i>Dies ist eine automatisch generierte E-Mail.</i>`
      },
      standard: {
        subject: 'Deine Bestellung #{{hash}} ist auf dem Weg!',
        template: `<p>Hey {{firstName}},</p>
<p>deine Bestellung befindet sich jetzt auf dem Weg zu dir 🚀<br>Bitte bestätige den Erhalt deines Päckchens auf der Bestellstatus-Seite, sobald es bei dir angekommen ist.</p>
<p style="margin: 30px 0;">
  <a href="https://order.doorman.azon.ai/order/{{hash}}" style="background-color: #3e63dd; color: white; padding: 9px 23px; text-decoration: none; border-radius: 35px; display: inline-block; font-family: Arial, sans-serif;">
    Bestellstatus ansehen
  </a>
</p>
<p>Beste Grüße<br/>Flo</p>
<br></hr><i>Dies ist eine automatisch generierte E-Mail.</i>`
      }
    },
    en: {
      tracking: {
        subject: 'Your order #{{hash}} is on the way to you!',
        template: `<p>Hey {{firstName}},</p>
<p>your order is on the way to you 🚀<br>Please confirm once you've received your package.</p>
<p style="margin: 30px 0;">
  <a href="https://order.doorman.azon.ai/order/{{hash}}" style="background-color: #3e63dd; color: white; padding: 9px 23px; text-decoration: none; border-radius: 35px; display: inline-block; font-family: Arial, sans-serif;">
    Confirm Delivery
  </a>
  <a href="https://www.dhl.de/en/privatkunden/dhl-sendungsverfolgung.html?piececode={{tracking}}" style="background-color: #3e63dd; color: white; padding: 9px 23px; text-decoration: none; border-radius: 35px; display: inline-block; font-family: Arial, sans-serif;">
    DHL Tracking
  </a>
</p>
<p>Best regards<br/>Flo</p>
<br></hr><i>This is an automatically generated email.</i>`
      },
      standard: {
        subject: 'Your order #{{hash}} is on the way to you!',
        template: `<p>Hey {{firstName}},</p>
<p>your order is on the way to you 🚀<br>Please confirm once you've received your package on the order status page.</p>
<p style="margin: 30px 0;">
  <a href="https://order.doorman.azon.ai/order/{{hash}}" style="background-color: #3e63dd; color: white; padding: 9px 23px; text-decoration: none; border-radius: 35px; display: inline-block; font-family: Arial, sans-serif;">
    View order status
  </a>
</p>
<p>Best regards<br/>Flo</p>
<br></hr><i>This is an automatically generated email.</i>`
      }
    }
  },

  closed: {
    de: {
      subject: 'Doorman #{{hash}} Abgeschlossen',
      template: `<p>Hey {{firstName}},</p>
<p>deine Bestellung ist hiermit abgeschlossen.<br/>Vielen lieben Dank dass du das Doorman Projekt unterstützt :)</p>
<p>Beste Grüße<br/>Flo</p>
<br></hr><i>Dies ist eine automatisch generierte E-Mail.</i>`
    },
    en: {
      subject: 'Doorman #{{hash}} Complete',
      template: `<p>Hey {{firstName}},</p>
<p>your order has been successfully completed.<br/>Thank you so much for supporting the Doorman project :)</p>
<p>Best regards<br/>Flo</p>
<br></hr><i>This is an automatically generated email.</i>`
    }
  },

  cancelled: {
    de: {
      review: {
        subject: 'Doorman #{{hash}} Storniert',
        template: `<p>Hey {{firstName}},</p>
<p>leider konnte deine Anfrage nicht angenommen werden.</p>
<p>Beste Grüße<br/>Flo</p>
<br></hr><i>Dies ist eine automatisch generierte E-Mail.</i>`
      },
      standard: {
        subject: 'Doorman #{{hash}} Storniert',
        template: `<p>Hey {{firstName}},</p>
<p>deine Bestellung wurde storniert.<br>Meld dich gerne falls du doch noch Interesse haben solltest 🤗</p>
<p>Beste Grüße<br/>Flo</p>
<br></hr><i>Dies ist eine automatisch generierte E-Mail.</i>`
      }
    },
    en: {
      review: {
        subject: 'Doorman #{{hash}} Cancelled',
        template: `<p>Hey {{firstName}},</p>
<p>Unfortunately your request could not be accepted.</p>
<p>Best regards<br/>Flo</p>
<br></hr><i>This is an automatically generated email.</i>`
      },
      standard: {
        subject: 'Doorman #{{hash}} Cancelled',
        template: `<p>Hey {{firstName}},</p>
<p>your order was cancelled.<br>Feel free to contact me anytime in case you're interested 🤗</p>
<p>Best regards<br/>Flo</p>
<br></hr><i>This is an automatically generated email.</i>`
      }
    }
  },

  admin_new_order: {
    subject: 'Doorman Anfrage #{{hash}}',
    template: `<p>Hey,</p>
<p>es gibt eine neue Doorman Anfrage von {{firstName}}!<br/>Schau doch gleich mal nach.</p>
{{#if customerNotes}}
<p><b>Notiz vom Kunden:</b><br/><span style="white-space: pre-line;">{{customerNotes}}</span></p>
{{/if}}
<p style="margin: 30px 0;">
  <a href="https://order.doorman.azon.ai/" style="background-color: #3e63dd; color: white; padding: 9px 23px; text-decoration: none; border-radius: 35px; display: inline-block; font-family: Arial, sans-serif;">
    Verwaltung öffnen
  </a>
</p>
<p>Grüße<br/>Doorman</p>
<br></hr><i>Dies ist eine automatisch generierte E-Mail.</i>`
  },

  admin_order_cancelled: {
    subject: 'Doorman Anfrage #{{hash}} storniert',
    template: `<p>Hey,</p>
<p>die Doorman Anfrage von {{firstName}} wurde storniert.</p>
<p style="margin: 30px 0;">
  <a href="https://order.doorman.azon.ai/" style="background-color: #3e63dd; color: white; padding: 9px 23px; text-decoration: none; border-radius: 35px; display: inline-block; font-family: Arial, sans-serif;">
    Verwaltung öffnen
  </a>
</p>
<p>Grüße<br/>Doorman</p>
<br></hr><i>Dies ist eine automatisch generierte E-Mail.</i>`
  },

  custom: {
    de: {
      subject: '{{title}}',
      template: `<p>Hey {{firstName}},</p>
<p>{{message}}</p>
<p>Beste Grüße<br/>Flo</p>
<br></hr><i>Dies ist eine automatisch generierte E-Mail.</i>`
    },
    en: {
      subject: '{{title}}',
      template: `<p>Hey {{firstName}},</p>
<p>{{message}}</p>
<p>Best regards<br/>Flo</p>
<br></hr><i>This is an automatically generated email.</i>`
    }
  }
};


function getTemplate(templateType, templateSubType, order) {
  const template = emailTemplates[templateType];
  
  if (!template) {
    throw new Error(`Unknown email template type: ${templateType}`);
  }

  if (templateType === 'admin_new_order' || templateType === 'admin_order_cancelled') {
    return template;
  }

  const langTemplate = template[order.language] || template['en'];

  if (templateSubType !== '') {
    return langTemplate[templateSubType];
  }

  if (templateType === 'pending_payment') {
    return langTemplate[order.payment_option];
  }

  if (templateType === 'shipped') {
    const shippingMethod = order.shipping_method === 'tracking' ? 'tracking' : 'standard';
    return langTemplate[shippingMethod];
  }

  if (templateType === 'cancelled') {
    const cancelType = order.status === 'pending_review' ? 'review' : 'standard';
    return langTemplate[cancelType];
  }

  return langTemplate;
}

/**
 * Sendet eine E-Mail basierend auf einem Template-Typ
 * @param {Object} order - Das Order-Objekt (muss order.language enthalten: 'de' oder 'en')
 * @param {string} templateType - Der Template-Typ
 * @param {Object} options - Zusätzliche Optionen (nur für custom Template benötigt)
 * @param {string} options.title - Titel (für custom Template)
 * @param {string} options.message - Nachricht (für custom Template)
 * @param {string} options.recipientEmail - Alternative E-Mail-Adresse
 * @returns {Promise} - Promise mit Sendeergebnis
 */
async function sendOrderEmail(order, templateType, options = {})
{
  const {
    title = '',
    message = '',
    subtype = '',
    recipientEmail
  } = options;

  let templateSubType = subtype;

  const templateData = {
    ...order,
    firstName: niceName(order.fullname),
    title: title,
    message: message,
    customerNotes: order.message
  };

  const product = products.find(p => p.key === order.product);
  const region = shipping_regions.find(r => r.key === order.shipping_region);
  const method = region?.options.find(o => o.key === order.shipping_method);
  
  const productPrice = (product ? product.price : 0) * order.amount;
  const shippingPrice = method ? method.price : 0;
  const price = productPrice + shippingPrice;

  templateData.price = price.toFixed(2);
  templateData.productName = product ? product.name : order.product;
  templateData.productComponents = product
    ? Object.entries(product.components)
        .map(([key, qty]) => {
          const meta = products.find(p => p.key === key);
          return `${qty * order.amount} × ${meta ? meta.name : key}`;
        })
        .join(', ')
    : `${order.amount} × ${order.product}`;


  const template = getTemplate(templateType, templateSubType, order);

  const subjectTemplate = Handlebars.compile(template.subject);
  const bodyTemplate = Handlebars.compile(template.template);

  const subject = subjectTemplate(templateData);
  const html = bodyTemplate(templateData);

  try {
    const result = await transporter.sendMail({
      from: '"Doorman" <doorman@azon.ai>',
      to: recipientEmail || (templateType === 'admin_new_order' || templateType === 'admin_order_cancelled'
        ? '"Florian Heß" <florian.hess@azon.ai>'
        : order.email),
      replyTo: (templateType === 'admin_new_order' || templateType === 'admin_order_cancelled'
        ? order.email : undefined),
      subject: subject,
      html: html
    });

    console.log(`Email sent successfully: ${templateType} (${order.language}) for order #${order.hash}`);
    return result;
  } catch (error) {
    console.error(`Failed to send email: ${templateType} for order #${order.hash}`, error);
    throw error;
  }
}




async function closeStaleOrders()
{
  const fourWeeksAgo = dayjs().subtract(4, 'weeks').unix();
  
  try {
    const rows = await new Promise((resolve, reject) => {
      db.all(
        'SELECT * FROM orders WHERE status = ? AND updated_timestamp <= ?', ['pending_payment', fourWeeksAgo], (err, rows) => {
          if (err) reject(err);
          else resolve(rows);
        }
      );
    });

    console.log(`Found ${rows.length} orders waiting for payment 4+ weeks:`);

    for (const order of rows)
    {
      console.log(`Order #${order.id} - ${order.email} - waiting for payment since ${dayjs.unix(order.updated_timestamp).format('DD.MM.YYYY')}`);
      //await cancelOrder(order.id);
    }
  } catch (err) {
    console.error(err);
  }
}


async function completePendingArrivalOrders()
{
  const fourWeeksAgo = dayjs().subtract(4, 'weeks').unix();
  
  try {
    const rows = await new Promise((resolve, reject) => {
      db.all(
        'SELECT * FROM orders WHERE status = ? AND updated_timestamp <= ?', ['shipped', fourWeeksAgo], (err, rows) => {
          if (err) reject(err);
          else resolve(rows);
        }
      );
    });

    console.log(`Found ${rows.length} orders shipped 4+ weeks ago:`);

    for (const order of rows)
    {
      console.log(`Order #${order.id} - ${order.email} - shipped on ${dayjs.unix(order.updated_timestamp).format('DD.MM.YYYY')}`);
      await moveOrderToNextStatus(order);
    }
  } catch (err) {
    console.error(err);
  }
}

async function checkPaypal()
{
  console.log('Checking PayPal transactions...');
  
  // WICHTIG: Verwende LIVE Credentials
  const CLIENT_ID = 'ATI3dNz-PpyJX-Zsn5MpcFG0XhIhpd4zhZZ8341QvzvservVQUYjQjKHFmPU26PzfepcXd89JzEcuYUO';
  const CLIENT_SECRET = 'ELl1yTjCsWCudwl2j502FocACdZjM2a8IokxT3a4GFBLdxW85Y-q4KtQtbtpca1Fl77YuuCVo4CbNLx2';
  
  const paypal = new PayPalFullAPI(CLIENT_ID, CLIENT_SECRET, true); // true = LIVE
  
  try {
    // Zeitraum: Letzte 30 Tage
    const endDate = new Date().toISOString();
    const startDate = new Date(Date.now() - 30 * 24 * 60 * 60 * 1000).toISOString();

    // Transaktionen abrufen
    const data = await paypal.getAllTransactions(startDate, endDate);

    if (data.transaction_details && data.transaction_details.length > 0)
    {
      const incoming = data.transaction_details.filter(tx => {
        const info = tx.transaction_info;
        const amount = parseFloat(info.transaction_amount?.value || 0);
        
        // Positive Beträge = Eingänge
        return amount > 0 && (
          info.transaction_event_code?.includes('PAYMENT') ||
          info.transaction_event_code?.includes('RECEIVE') ||
          info.transaction_status === 'S' // Success
        ) && info.transaction_amount?.currency_code == 'EUR' && info.transaction_note != undefined && info.transaction_note != "";
      });

      for(tx of incoming)
      {
        const info = tx.transaction_info;
        const payer = tx.payer_info;

        let data = {
          von: payer?.payer_name?.alternate_full_name || payer?.email_address || 'N/A',
          email: payer?.email_address || 'N/A',
          amount: Number(info.transaction_amount?.value) || 0,
          fee: Number(info.fee_amount?.value) || 0,
          note: info.transaction_note || 'n/a'
        }

        let total_amount = (data.amount + data.fee);
        let orderHash = extractOrderNumbers(data.note)[0];

        const order = await getOrderByHash(orderHash);
        if (order)
        {
          if(order.status == 'pending_payment') 
          {
            const product = products.find(p => p.key === order.product);
            const region = shipping_regions.find(r => r.key === order.shipping_region);
            const method = region?.options.find(o => o.key === order.shipping_method);
            
            const productPrice = (product ? product.price : 0) * order.amount;
            const shippingPrice = method ? method.price : 0;
            const price = productPrice + shippingPrice;

            if(price == total_amount)
            {
              console.log("Payment for order " + orderHash + " received from: " + data.von + " (" + total_amount + " €)");
            }
            else
            {
              console.log("Incomplete payment for order " + orderHash + " received from: " + data.von + " (" + total_amount + " € instead of " + price + " €)");
            }
          }
        }
      }

    } else {
      console.log('Keine Transaktionen im angegebenen Zeitraum gefunden');
    }

  } catch (error) {
    console.error('FEHLER:', error.message);
    
    if (error.response) {
      console.error('API Antwort:', JSON.stringify(error.response.data, null, 2));
    }
  }


}

class PayPalFullAPI {
  constructor(clientId, clientSecret, isLive = false) {
    this.clientId = clientId;
    this.clientSecret = clientSecret;
    this.baseURL = isLive 
      ? 'https://api-m.paypal.com'
      : 'https://api-m.sandbox.paypal.com';
    this.accessToken = null;
  }

  // Access Token mit erweiterten Scopes
  async getAccessToken() {
    const auth = Buffer.from(`${this.clientId}:${this.clientSecret}`).toString('base64');
    
    try {
      const response = await axios.post(
        `${this.baseURL}/v1/oauth2/token`,
        'grant_type=client_credentials',
        {
          headers: {
            'Authorization': `Basic ${auth}`,
            'Content-Type': 'application/x-www-form-urlencoded'
          }
        }
      );
      
      this.accessToken = response.data.access_token;
      /*console.log('✓ Access Token erhalten');
      console.log('  Scopes:', response.data.scope);*/
      return this.accessToken;
    } catch (error) {
      console.error('Fehler beim Access Token:', error.response?.data || error.message);
      throw error;
    }
  }

  // HAUPTMETHODE: Alle Transaktionen abrufen (inkl. manueller Zahlungen)
  async getAllTransactions(startDate, endDate, options = {}) {
    if (!this.accessToken) {
      await this.getAccessToken();
    }

    const params = {
      start_date: startDate,
      end_date: endDate,
      fields: 'all',
      page_size: options.pageSize || 100,
      page: options.page || 1
    };

    // Optional: Nur bestimmte Status
    if (options.status) {
      params.transaction_status = options.status; // S = Success, P = Pending, etc.
    }

    try {
      const response = await axios.get(
        `${this.baseURL}/v1/reporting/transactions`,
        {
          headers: {
            'Authorization': `Bearer ${this.accessToken}`,
            'Content-Type': 'application/json'
          },
          params
        }
      );
      return response.data;
    } catch (error) {
      if (error.response?.status === 403) {
        console.error('ZUGRIFF VERWEIGERT (403)');
        console.error('   Problem: Transaction Search API ist nicht freigeschaltet');
      }
      throw error;
    }
  }

  // Paginierung: Alle Seiten abrufen
  async getAllTransactionsPaginated(startDate, endDate) {
    let allTransactions = [];
    let page = 1;
    let hasMore = true;

    while (hasMore) {
      const data = await this.getAllTransactions(startDate, endDate, { page });
      
      if (data.transaction_details && data.transaction_details.length > 0) {
        allTransactions = allTransactions.concat(data.transaction_details);
        
        // Prüfe ob es weitere Seiten gibt
        if (data.links) {
          const nextLink = data.links.find(link => link.rel === 'next');
          hasMore = !!nextLink;
          page++;
        } else {
          hasMore = false;
        }
      } else {
        hasMore = false;
      }
    }

    console.log(`Insgesamt ${allTransactions.length} Transaktionen über alle Seiten`);
    return allTransactions;
  }

}

function extractOrderNumbers(text) {
  const regex = /#[A-Z0-9]{8}/g;
  const matches = text.match(regex);
  return matches ? matches.map(match => match.substring(1)) : [];
}