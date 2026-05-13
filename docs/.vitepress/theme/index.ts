import DefaultTheme from "vitepress/theme";
import Layout from './Layout.vue'
import OrderPage from './OrderPage.vue'

import ContactModal from './components/ContactModal.vue'
import LEDBadge from './components/LEDBadge.vue'

//import { useData, useRoute } from 'vitepress';
//import codeblocksFold from 'vitepress-plugin-codeblocks-fold'; // import method
//import 'vitepress-plugin-codeblocks-fold/style/index.css'; // import style


import { enhanceAppWithTabs } from 'vitepress-plugin-tabs/client'
import 'virtual:group-icons.css'

import "./styles.css";

export default {
    extends: DefaultTheme,
    // override the Layout with a wrapper component that
    // injects the slots
    Layout: Layout,
    enhanceApp({ app }) {
        app.component('LEDBadge', LEDBadge)
        app.component('ContactModal', ContactModal)
        app.component('orderpage', OrderPage)

        enhanceAppWithTabs(app);

        if (typeof window !== 'undefined')
        {
            import('improv-wifi-sdk/dist/web/launch-button.js');
            import('improv-wifi-serial-sdk/dist/web/serial-launch-button.js');
            import('esp-web-tools/dist/web/install-button.js');
        }
    },
    setup() {
        // get frontmatter and route
        //const { frontmatter } = useData();
        //const route = useRoute();

        // basic use
        //codeblocksFold({route, frontmatter});
    }
}