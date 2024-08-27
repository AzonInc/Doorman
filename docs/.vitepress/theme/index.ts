import DefaultTheme from "vitepress/theme";
import Layout from './Layout.vue'

import 'uno.css'
import "./custom.css";

export default {
    extends: DefaultTheme,
    // override the Layout with a wrapper component that
    // injects the slots
    Layout: Layout
}