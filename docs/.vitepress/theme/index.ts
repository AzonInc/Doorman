import DefaultTheme from "vitepress/theme";
import Layout from './Layout.vue'

import ContactModal from './components/ContactModal.vue'

import { enhanceAppWithTabs } from 'vitepress-plugin-tabs/client'
import 'virtual:group-icons.css'

import "./styles.css";

export default {
    extends: DefaultTheme,
    // override the Layout with a wrapper component that
    // injects the slots
    Layout: Layout,
    enhanceApp({ app }) {
        app.component('ContactModal', ContactModal)

        enhanceAppWithTabs(app);

        if (typeof window !== 'undefined')
        {
            import('improv-wifi-sdk/dist/web/launch-button.js');
            import('improv-wifi-serial-sdk/dist/web/serial-launch-button.js');
            import('esp-web-tools/dist/web/install-button.js');
        }
    },
    setup() {

    }
}